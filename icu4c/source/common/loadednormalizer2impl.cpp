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

#सचे !UCONFIG_NO_NORMALIZATION

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

वग्ग LoadedNormalizer2Impl : सार्वजनिक Normalizer2Impl {
सार्वजनिक:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    आभास ~LoadedNormalizer2Impl();

    शून्य load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UErrorCode &errorCode);

गुप्त:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, ध्रुव वर्ण *type, ध्रुव वर्ण *name, ध्रुव UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(शून्य * /*context*/,
                                    ध्रुव वर्ण * /* type */, ध्रुव वर्ण * /*name*/,
                                    ध्रुव UDataInfo *pInfo) {
    सचे(
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
        पटिनिवत्त सच्च;
    } अञ्ञथा {
        पटिनिवत्त मिच्छा;
    }
}

शून्य
LoadedNormalizer2Impl::load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, एतत्, &errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }
    ध्रुव uint8_t* inBytes = static_cast<ध्रुव uint8_t*>(udata_getMemory(memory));
    ध्रुव int32_t* inIndexes = reinterpret_cast<ध्रुव int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    सचे(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        पटिनिवत्त;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मानम्_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ध्रुव uint16_t* inExtraData = reinterpret_cast<ध्रुव uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ध्रुव uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ध्रुव वर्ण *packageName,
                              ध्रुव वर्ण *name,
                              UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    LoadedNormalizer2Impl *impl=नूतन LoadedNormalizer2Impl;
    सचे(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }
    impl->load(packageName, name, errorCode);
    पटिनिवत्त createInstance(impl, errorCode);
}

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#सचे !NORM2_HARDCODE_NFC_DATA
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
स्थिर शून्य U_CALLCONV initSingletons(ध्रुव वर्ण *what, UErrorCode &errorCode) {
#सचे !NORM2_HARDCODE_NFC_DATA
    सचे (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } अञ्ञथा
#endif
    सचे (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } अञ्ञथा सचे (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } अञ्ञथा सचे (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } अञ्ञथा {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV deleteNorm2AllModes(शून्य *allModes) {
    delete (Norm2AllModes *)allModes;
}

स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#सचे !NORM2_HARDCODE_NFC_DATA
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
    पटिनिवत्त सच्च;
}

U_CDECL_END

#सचे !NORM2_HARDCODE_NFC_DATA
ध्रुव Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    पटिनिवत्त nfcSingleton;
}
#endif

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    पटिनिवत्त nfkcSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    पटिनिवत्त nfkc_cfSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) { पटिनिवत्त nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    पटिनिवत्त nfkc_scfSingleton;
}

#सचे !NORM2_HARDCODE_NFC_DATA
ध्रुव Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->decomp : nullptr;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->fcd : nullptr;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->fcc : nullptr;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ध्रुव Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->decomp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getInstance(ध्रुव वर्ण *packageName,
                         ध्रुव वर्ण *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    सचे(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त nullptr;
    }
    ध्रुव Norm2AllModes *allModes=nullptr;
    सचे(packageName==nullptr) {
        सचे(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } अञ्ञथा सचे(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } अञ्ञथा सचे(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } अञ्ञथा सचे(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    सचे(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            सचे(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        सचे(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            सचे(U_SUCCESS(errorCode)) {
                Mutex lock;
                सचे(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    सचे(U_FAILURE(errorCode)) {
                        पटिनिवत्त nullptr;
                    }
                    uhash_set_कुञ्चिका_Deleter(cache, uprv_free);
                    uhash_set_मानम्_Deleter(cache, deleteNorm2AllModes);
                }
                शून्य *temp=uhash_get(cache, name);
                सचे(temp==nullptr) {
                    int32_t कुञ्चिका_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    वर्ण* nameCopy = static_cast<वर्ण*>(uprv_malloc(कुञ्चिका_Length));
                    सचे(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        पटिनिवत्त nullptr;
                    }
                    uprv_memcpy(nameCopy, name, कुञ्चिका_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } अञ्ञथा {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    सचे(allModes!=nullptr && U_SUCCESS(errorCode)) {
        विकल्प(mode) {
        पक्ष UNORM2_COMPOSE:
            पटिनिवत्त &allModes->comp;
        पक्ष UNORM2_DECOMPOSE:
            पटिनिवत्त &allModes->decomp;
        पक्ष UNORM2_FCD:
            पटिनिवत्त &allModes->fcd;
        पक्ष UNORM2_COMPOSE_CONTIGUOUS:
            पटिनिवत्त &allModes->fcc;
        स्वभाव:
            भङ्ग;  // do nothing
        }
    }
    पटिनिवत्त nullptr;
}

ध्रुव Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    सचे(U_FAILURE(errorCode)) {
        पटिनिवत्त nullptr;
    }
    विकल्प(mode) {
    पक्ष UNORM_NFD:
        पटिनिवत्त Normalizer2::getNFDInstance(errorCode);
    पक्ष UNORM_NFKD:
        पटिनिवत्त Normalizer2::getNFKDInstance(errorCode);
    पक्ष UNORM_NFC:
        पटिनिवत्त Normalizer2::getNFCInstance(errorCode);
    पक्ष UNORM_NFKC:
        पटिनिवत्त Normalizer2::getNFKCInstance(errorCode);
    पक्ष UNORM_FCD:
        पटिनिवत्त getFCDInstance(errorCode);
    स्वभाव:  // UNORM_NONE
        पटिनिवत्त getNoopInstance(errorCode);
    }
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? allModes->impl : nullptr;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    पटिनिवत्त allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    पटिनिवत्त (ध्रुव UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    पटिनिवत्त (ध्रुव UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    पटिनिवत्त (ध्रुव UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    पटिनिवत्त (ध्रुव UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getInstance(ध्रुव वर्ण *packageName,
                   ध्रुव वर्ण *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    पटिनिवत्त (ध्रुव UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    सचे(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        पटिनिवत्त UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ध्रुव Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    सचे(U_SUCCESS(errorCode)) {
        पटिनिवत्त ((ध्रुव Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } अञ्ञथा {
        पटिनिवत्त UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
