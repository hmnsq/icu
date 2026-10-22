// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
* Copyright (C) 2014, International Business Machines
* Corporation and others.  All Rights Reserved.
*******************************************************************************
* loadednormalizer2impl.cpp
*
* created on: 2014sep03
* created by: Markus W. Scherer
*/

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/udata.h"
#समावेश "unicode/localpointer.h"
#समावेश "unicode/normalizer2.h"
#समावेश "unicode/ucptrie.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/unorm.h"
#समावेश "cstring.h"
#समावेश "mutex.h"
#समावेश "norm2allmodes.h"
#समावेश "normalizer2impl.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "uhash.h"

U_NAMESPACE_BEGIN

श्रेणी LoadedNormalizer2Impl : खुला Normalizer2Impl {
खुला:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    भव ~LoadedNormalizer2Impl();

    व्योम load(स्थिर अक्षर *packageName, स्थिर अक्षर *name, UErrorCode &errorCode);

निजी:
    अटल UBool U_CALLCONV
    isAcceptable(व्योम *context, स्थिर अक्षर *type, स्थिर अक्षर *name, स्थिर UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(व्योम * /*context*/,
                                    स्थिर अक्षर * /* type */, स्थिर अक्षर * /*name*/,
                                    स्थिर UDataInfo *pInfo) {
    अगर(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->तथ्य_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->तथ्य_Format[1]==0x72 &&
        pInfo->तथ्य_Format[2]==0x6d &&
        pInfo->तथ्य_Format[3]==0x32 &&
        pInfo->formatVersion[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        वापस true;
    } अन्यथा {
        वापस false;
    }
}

व्योम
LoadedNormalizer2Impl::load(स्थिर अक्षर *packageName, स्थिर अक्षर *name, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, यह, &errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }
    स्थिर uint8_t* inBytes = static_cast<स्थिर uint8_t*>(udata_getMemory(memory));
    स्थिर int32_t* inIndexes = reinterpret_cast<स्थिर int32_t*>(inBytes);
    int32_t indexes_लंबाई=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    अगर(indexes_लंबाई<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        वापस;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t अग्र_Offset=inIndexes[IX_EXTRA_तथ्य_OFFSET];
    ownedTrie=ucptrie_open_From_द्विआधारी(UCPTRIE_TYPE_FAST, UCPTRIE_मूल्य_BITS_16,
                                     inBytes+offset, अग्र_Offset-offset, nullptr,
                                     &errorCode);
    अगर(U_FAILURE(errorCode)) {
        वापस;
    }

    offset=अग्र_Offset;
    अग्र_Offset=inIndexes[IX_SMALL_FCD_OFFSET];
    स्थिर uint16_t* in_Extra_तथ्य = reinterpret_cast<स्थिर uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=अग्र_Offset;
    स्थिर uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, in_Extra_तथ्य, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(स्थिर अक्षर *packageName,
                              स्थिर अक्षर *name,
                              UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    LoadedNormalizer2Impl *impl=नया LoadedNormalizer2Impl;
    अगर(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }
    impl->load(packageName, name, errorCode);
    वापस createInstance(impl, errorCode);
}

U_CDECL_BEGIN
अटल UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#अगर !NORM2_HARDCODE_NFC_तथ्य
अटल Norm2AllModes *nfcSingleton;
अटल icu::UInitOnce nfcInitOnce {};
#पूर्ण_अगर

अटल Norm2AllModes *nfkcSingleton;
अटल icu::UInitOnce nfkcInitOnce {};

अटल Norm2AllModes *nfkc_cfSingleton;
अटल icu::UInitOnce nfkc_cfInitOnce {};

अटल Norm2AllModes *nfkc_scfSingleton;
अटल icu::UInitOnce nfkc_scfInitOnce {};

अटल UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
अटल व्योम U_CALLCONV initSingletons(स्थिर अक्षर *what, UErrorCode &errorCode) {
#अगर !NORM2_HARDCODE_NFC_तथ्य
    अगर (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } अन्यथा
#पूर्ण_अगर
    अगर (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } अन्यथा अगर (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } अन्यथा अगर (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } अन्यथा {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

अटल व्योम U_CALLCONV deleteNorm2AllModes(व्योम *allModes) {
    मिटाओ (Norm2AllModes *)allModes;
}

अटल UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#अगर !NORM2_HARDCODE_NFC_तथ्य
    मिटाओ nfcSingleton;
    nfcSingleton = nullptr;
    nfcInitOnce.reset();
#पूर्ण_अगर

    मिटाओ nfkcSingleton;
    nfkcSingleton = nullptr;
    nfkcInitOnce.reset();

    मिटाओ nfkc_cfSingleton;
    nfkc_cfSingleton = nullptr;
    nfkc_cfInitOnce.reset();

    मिटाओ nfkc_scfSingleton;
    nfkc_scfSingleton = nullptr;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=nullptr;
    वापस true;
}

U_CDECL_END

#अगर !NORM2_HARDCODE_NFC_तथ्य
स्थिर Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    वापस nfcSingleton;
}
#पूर्ण_अगर

स्थिर Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    वापस nfkcSingleton;
}

स्थिर Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    वापस nfkc_cfSingleton;
}

स्थिर Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) { वापस nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    वापस nfkc_scfSingleton;
}

#अगर !NORM2_HARDCODE_NFC_तथ्य
स्थिर Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिर Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->decomp : nullptr;
}

स्थिर Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->fcd : nullptr;
}

स्थिर Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->fcc : nullptr;
}

स्थिर Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    वापस allModes!=nullptr ? allModes->impl : nullptr;
}
#पूर्ण_अगर

स्थिर Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिर Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->decomp : nullptr;
}

स्थिर Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिर Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    वापस allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिर Normalizer2 *
Normalizer2::getInstance(स्थिर अक्षर *packageName,
                         स्थिर अक्षर *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    अगर(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस nullptr;
    }
    स्थिर Norm2AllModes *allModes=nullptr;
    अगर(packageName==nullptr) {
        अगर(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } अन्यथा अगर(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } अन्यथा अगर(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } अन्यथा अगर(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    अगर(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            अगर(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        अगर(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            अगर(U_SUCCESS(errorCode)) {
                Mutex lock;
                अगर(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    अगर(U_FAILURE(errorCode)) {
                        वापस nullptr;
                    }
                    uhash_set_कुंजी_Deleter(cache, uprv_free);
                    uhash_set_मूल्य_Deleter(cache, deleteNorm2AllModes);
                }
                व्योम *अस्थायी=uhash_get(cache, name);
                अगर(अस्थायी==nullptr) {
                    int32_t कुंजी_लंबाई= static_cast<int32_t>(uprv_strlen(name)+1);
                    अक्षर* nameCopy = static_cast<अक्षर*>(uprv_malloc(कुंजी_लंबाई));
                    अगर(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        वापस nullptr;
                    }
                    uprv_memcpy(nameCopy, name, कुंजी_लंबाई);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } अन्यथा {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(अस्थायी);
                }
            }
        }
    }
    अगर(allModes!=nullptr && U_SUCCESS(errorCode)) {
        चयन(mode) {
        हाल UNORM2_COMPOSE:
            वापस &allModes->comp;
        हाल UNORM2_DECOMPOSE:
            वापस &allModes->decomp;
        हाल UNORM2_FCD:
            वापस &allModes->fcd;
        हाल UNORM2_COMPOSE_CONTIGUOUS:
            वापस &allModes->fcc;
        शेष:
            अवरोध;  // do nothing
        }
    }
    वापस nullptr;
}

स्थिर Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    अगर(U_FAILURE(errorCode)) {
        वापस nullptr;
    }
    चयन(mode) {
    हाल UNORM_NFD:
        वापस Normalizer2::getNFDInstance(errorCode);
    हाल UNORM_NFKD:
        वापस Normalizer2::getNFKDInstance(errorCode);
    हाल UNORM_NFC:
        वापस Normalizer2::getNFCInstance(errorCode);
    हाल UNORM_NFKC:
        वापस Normalizer2::getNFKCInstance(errorCode);
    हाल UNORM_FCD:
        वापस getFCDInstance(errorCode);
    शेष:  // UNORM_NONE
        वापस getNoopInstance(errorCode);
    }
}

स्थिर Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    वापस allModes!=nullptr ? allModes->impl : nullptr;
}

स्थिर Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    स्थिर Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    वापस allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI स्थिर UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    वापस (स्थिर UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI स्थिर UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    वापस (स्थिर UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI स्थिर UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    वापस (स्थिर UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI स्थिर UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    वापस (स्थिर UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI स्थिर UNormalizer2 * U_EXPORT2
unorm2_getInstance(स्थिर अक्षर *packageName,
                   स्थिर अक्षर *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    वापस (स्थिर UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalization_Check_परिणाम
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    अगर(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        वापस UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिर Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    अगर(U_SUCCESS(errorCode)) {
        वापस ((स्थिर Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } अन्यथा {
        वापस UNORM_MAYBE;
    }
}

#पूर्ण_अगर  // !UCONFIG_NO_NORMALIZATION
