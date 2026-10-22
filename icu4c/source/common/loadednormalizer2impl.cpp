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

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "norm2allmodes.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

वर्ग LoadedNormalizer2Impl : सार्वजनिक Normalizer2Impl {
सार्वजनिक:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    आभासी ~LoadedNormalizer2Impl();

    शून्य load(स्थिरांक अक्षर *packageName, स्थिरांक अक्षर *name, UErrorCode &errorCode);

खाजगी:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, स्थिरांक अक्षर *type, स्थिरांक अक्षर *name, स्थिरांक UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(शून्य * /*context*/,
                                    स्थिरांक अक्षर * /* type */, स्थिरांक अक्षर * /*name*/,
                                    स्थिरांक UDataInfo *pInfo) {
    जर(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->dataFormat[1]==0x72 &&
        pInfo->dataFormat[2]==0x6d &&
        pInfo->dataFormat[3]==0x32 &&
        pInfo->formatVersion[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        परत सत्य;
    } अन्यथा {
        परत असत्य;
    }
}

शून्य
LoadedNormalizer2Impl::load(स्थिरांक अक्षर *packageName, स्थिरांक अक्षर *name, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, हे, &errorCode);
    जर(U_FAILURE(errorCode)) {
        परत;
    }
    स्थिरांक uint8_t* inBytes = static_cast<स्थिरांक uint8_t*>(udata_getMemory(memory));
    स्थिरांक int32_t* inIndexes = reinterpret_cast<स्थिरांक int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    जर(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        परत;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मूल्य_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    जर(U_FAILURE(errorCode)) {
        परत;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    स्थिरांक uint16_t* inExtraData = reinterpret_cast<स्थिरांक uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    स्थिरांक uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(स्थिरांक अक्षर *packageName,
                              स्थिरांक अक्षर *name,
                              UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    LoadedNormalizer2Impl *impl=नवीन LoadedNormalizer2Impl;
    जर(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }
    impl->load(packageName, name, errorCode);
    परत createInstance(impl, errorCode);
}

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#जर !NORM2_HARDCODE_NFC_DATA
स्थिर Norm2AllModes *nfcSingleton;
स्थिर icu::UInitOnce nfcInitOnce {};
#endif

स्थिर Norm2AllModes *nfkcSingleton;
स्थिर icu::UInitOnce nfkcInitOnce {};

स्थिर Norm2AllModes *nfkc_cfSingleton;
स्थिर icu::UInitOnce nfkc_cfInitOnce {};

स्थिर Norm2AllModes *nfkc_scfSingleton;
स्थिर icu::UInitOnce nfkc_scfInitOnce {};

स्थिर UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
स्थिर शून्य U_CALLCONV initSingletons(स्थिरांक अक्षर *what, UErrorCode &errorCode) {
#जर !NORM2_HARDCODE_NFC_DATA
    जर (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } अन्यथा
#endif
    जर (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } अन्यथा जर (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } अन्यथा जर (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } अन्यथा {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV deleteNorm2AllModes(शून्य *allModes) {
    delete (Norm2AllModes *)allModes;
}

स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#जर !NORM2_HARDCODE_NFC_DATA
    delete nfcSingleton;
    nfcSingleton = nullptr;
    nfcInitOnce.reset();
#endif

    delete nfkcSingleton;
    nfkcSingleton = nullptr;
    nfkcInitOnce.reset();

    delete nfkc_cfSingleton;
    nfkc_cfSingleton = nullptr;
    nfkc_cfInitOnce.reset();

    delete nfkc_scfSingleton;
    nfkc_scfSingleton = nullptr;
    nfkc_scfInitOnce.reset();

    uhash_close(cache);
    cache=nullptr;
    परत सत्य;
}

U_CDECL_END

#जर !NORM2_HARDCODE_NFC_DATA
स्थिरांक Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    परत nfcSingleton;
}
#endif

स्थिरांक Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    परत nfkcSingleton;
}

स्थिरांक Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    परत nfkc_cfSingleton;
}

स्थिरांक Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) { परत nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    परत nfkc_scfSingleton;
}

#जर !NORM2_HARDCODE_NFC_DATA
स्थिरांक Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->decomp : nullptr;
}

स्थिरांक Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->fcd : nullptr;
}

स्थिरांक Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->fcc : nullptr;
}

स्थिरांक Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    परत allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

स्थिरांक Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    परत allModes!=nullptr ? &allModes->decomp : nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    परत allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    परत allModes!=nullptr ? &allModes->comp : nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2::getInstance(स्थिरांक अक्षर *packageName,
                         स्थिरांक अक्षर *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    जर(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत nullptr;
    }
    स्थिरांक Norm2AllModes *allModes=nullptr;
    जर(packageName==nullptr) {
        जर(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } अन्यथा जर(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } अन्यथा जर(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } अन्यथा जर(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    जर(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            जर(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        जर(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            जर(U_SUCCESS(errorCode)) {
                Mutex lock;
                जर(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    जर(U_FAILURE(errorCode)) {
                        परत nullptr;
                    }
                    uhash_set_किल्ली_Deleter(cache, uprv_free);
                    uhash_set_मूल्य_Deleter(cache, deleteNorm2AllModes);
                }
                शून्य *temp=uhash_get(cache, name);
                जर(temp==nullptr) {
                    int32_t किल्ली_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    अक्षर* nameCopy = static_cast<अक्षर*>(uprv_malloc(किल्ली_Length));
                    जर(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        परत nullptr;
                    }
                    uprv_memcpy(nameCopy, name, किल्ली_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } अन्यथा {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    जर(allModes!=nullptr && U_SUCCESS(errorCode)) {
        निवड(mode) {
        स्थिती UNORM2_COMPOSE:
            परत &allModes->comp;
        स्थिती UNORM2_DECOMPOSE:
            परत &allModes->decomp;
        स्थिती UNORM2_FCD:
            परत &allModes->fcd;
        स्थिती UNORM2_COMPOSE_CONTIGUOUS:
            परत &allModes->fcc;
        पूर्वनिर्धारित:
            थांबा;  // do nothing
        }
    }
    परत nullptr;
}

स्थिरांक Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    जर(U_FAILURE(errorCode)) {
        परत nullptr;
    }
    निवड(mode) {
    स्थिती UNORM_NFD:
        परत Normalizer2::getNFDInstance(errorCode);
    स्थिती UNORM_NFKD:
        परत Normalizer2::getNFKDInstance(errorCode);
    स्थिती UNORM_NFC:
        परत Normalizer2::getNFCInstance(errorCode);
    स्थिती UNORM_NFKC:
        परत Normalizer2::getNFKCInstance(errorCode);
    स्थिती UNORM_FCD:
        परत getFCDInstance(errorCode);
    पूर्वनिर्धारित:  // UNORM_NONE
        परत getNoopInstance(errorCode);
    }
}

स्थिरांक Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    परत allModes!=nullptr ? allModes->impl : nullptr;
}

स्थिरांक Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    स्थिरांक Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    परत allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI स्थिरांक UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    परत (स्थिरांक UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI स्थिरांक UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    परत (स्थिरांक UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI स्थिरांक UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    परत (स्थिरांक UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI स्थिरांक UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    परत (स्थिरांक UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI स्थिरांक UNormalizer2 * U_EXPORT2
unorm2_getInstance(स्थिरांक अक्षर *packageName,
                   स्थिरांक अक्षर *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    परत (स्थिरांक UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    जर(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        परत UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    स्थिरांक Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    जर(U_SUCCESS(errorCode)) {
        परत ((स्थिरांक Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } अन्यथा {
        परत UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
