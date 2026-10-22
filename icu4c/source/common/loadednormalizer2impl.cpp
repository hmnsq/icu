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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಯದಿ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/normalizer2.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucptrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "mutex.h"
#ಅನ್ತರ್ಭಾವ "norm2allmodes.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"

U_NAMESPACE_BEGIN

ವರ್ಗ LoadedNormalizer2Impl : ಸಾರ್ವಜನಿಕ Normalizer2Impl {
ಸಾರ್ವಜನಿಕ:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    ಆಭಾಸ ~LoadedNormalizer2Impl();

    ಶೂನ್ಯ load(ಧ್ರುವ ವರ್ಣ *packageName, ಧ್ರುವ ವರ್ಣ *name, UErrorCode &errorCode);

ಗುಪ್ತ:
    ಸ್ಥಿರ UBool U_CALLCONV
    isAcceptable(ಶೂನ್ಯ *context, ಧ್ರುವ ವರ್ಣ *type, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(ಶೂನ್ಯ * /*context*/,
                                    ಧ್ರುವ ವರ್ಣ * /* type */, ಧ್ರುವ ವರ್ಣ * /*name*/,
                                    ಧ್ರುವ UDataInfo *pInfo) {
    ಯದಿ(
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
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
}

ಶೂನ್ಯ
LoadedNormalizer2Impl::load(ಧ್ರುವ ವರ್ಣ *packageName, ಧ್ರುವ ವರ್ಣ *name, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, ಏತತ್, &errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಧ್ರುವ uint8_t* inBytes = static_cast<ಧ್ರುವ uint8_t*>(udata_getMemory(memory));
    ಧ್ರುವ int32_t* inIndexes = reinterpret_cast<ಧ್ರುವ int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    ಯದಿ(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ಪ್ರತ್ಯಾಗಮ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_ದ್ವಿಮಾನ(UCPTRIE_TYPE_FAST, UCPTRIE_ಮೌಲ್ಯ_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ಧ್ರುವ uint16_t* inExtraData = reinterpret_cast<ಧ್ರುವ uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ಧ್ರುವ uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ಧ್ರುವ ವರ್ಣ *packageName,
                              ಧ್ರುವ ವರ್ಣ *name,
                              UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    LoadedNormalizer2Impl *impl=ನೂತನ LoadedNormalizer2Impl;
    ಯದಿ(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    impl->load(packageName, name, errorCode);
    ಪ್ರತ್ಯಾಗಮ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ಸ್ಥಿರ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#ಯದಿ !NORM2_HARDCODE_NFC_DATA
ಸ್ಥಿರ Norm2AllModes *nfcSingleton;
ಸ್ಥಿರ icu::UInitOnce nfcInitOnce {};
#endif

ಸ್ಥಿರ Norm2AllModes *nfkcSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkcInitOnce {};

ಸ್ಥಿರ Norm2AllModes *nfkc_cfSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkc_cfInitOnce {};

ಸ್ಥಿರ Norm2AllModes *nfkc_scfSingleton;
ಸ್ಥಿರ icu::UInitOnce nfkc_scfInitOnce {};

ಸ್ಥಿರ UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV initSingletons(ಧ್ರುವ ವರ್ಣ *what, UErrorCode &errorCode) {
#ಯದಿ !NORM2_HARDCODE_NFC_DATA
    ಯದಿ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } ಅನ್ಯಥಾ
#endif
    ಯದಿ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } ಅನ್ಯಥಾ ಯದಿ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } ಅನ್ಯಥಾ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV deleteNorm2AllModes(ಶೂನ್ಯ *allModes) {
    delete (Norm2AllModes *)allModes;
}

ಸ್ಥಿರ UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ಯದಿ !NORM2_HARDCODE_NFC_DATA
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
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_CDECL_END

#ಯದಿ !NORM2_HARDCODE_NFC_DATA
ಧ್ರುವ Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ಪ್ರತ್ಯಾಗಮ nfcSingleton;
}
#endif

ಧ್ರುವ Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ಪ್ರತ್ಯಾಗಮ nfkcSingleton;
}

ಧ್ರುವ Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ಪ್ರತ್ಯಾಗಮ nfkc_cfSingleton;
}

ಧ್ರುವ Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) { ಪ್ರತ್ಯಾಗಮ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ಪ್ರತ್ಯಾಗಮ nfkc_scfSingleton;
}

#ಯದಿ !NORM2_HARDCODE_NFC_DATA
ಧ್ರುವ Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->comp : nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ಧ್ರುವ Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->fcd : nullptr;
}

ಧ್ರುವ Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->fcc : nullptr;
}

ಧ್ರುವ Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ಧ್ರುವ Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->comp : nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->comp : nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? &allModes->comp : nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2::getInstance(ಧ್ರುವ ವರ್ಣ *packageName,
                         ಧ್ರುವ ವರ್ಣ *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಯದಿ(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ Norm2AllModes *allModes=nullptr;
    ಯದಿ(packageName==nullptr) {
        ಯದಿ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } ಅನ್ಯಥಾ ಯದಿ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } ಅನ್ಯಥಾ ಯದಿ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } ಅನ್ಯಥಾ ಯದಿ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    ಯದಿ(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            ಯದಿ(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ಯದಿ(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            ಯದಿ(U_SUCCESS(errorCode)) {
                Mutex lock;
                ಯದಿ(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    ಯದಿ(U_FAILURE(errorCode)) {
                        ಪ್ರತ್ಯಾಗಮ nullptr;
                    }
                    uhash_set_ಕೀಲಿ_Deleter(cache, uprv_free);
                    uhash_set_ಮೌಲ್ಯ_Deleter(cache, deleteNorm2AllModes);
                }
                ಶೂನ್ಯ *temp=uhash_get(cache, name);
                ಯದಿ(temp==nullptr) {
                    int32_t ಕೀಲಿ_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    ವರ್ಣ* nameCopy = static_cast<ವರ್ಣ*>(uprv_malloc(ಕೀಲಿ_Length));
                    ಯದಿ(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ಪ್ರತ್ಯಾಗಮ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, ಕೀಲಿ_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } ಅನ್ಯಥಾ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    ಯದಿ(allModes!=nullptr && U_SUCCESS(errorCode)) {
        ವಿಕಲ್ಪ(mode) {
        ಪಕ್ಷ UNORM2_COMPOSE:
            ಪ್ರತ್ಯಾಗಮ &allModes->comp;
        ಪಕ್ಷ UNORM2_DECOMPOSE:
            ಪ್ರತ್ಯಾಗಮ &allModes->decomp;
        ಪಕ್ಷ UNORM2_FCD:
            ಪ್ರತ್ಯಾಗಮ &allModes->fcd;
        ಪಕ್ಷ UNORM2_COMPOSE_CONTIGUOUS:
            ಪ್ರತ್ಯಾಗಮ &allModes->fcc;
        ಸ್ವಭಾವ:
            ಭಙ್ಗ;  // do nothing
        }
    }
    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಧ್ರುವ Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    ಯದಿ(U_FAILURE(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ವಿಕಲ್ಪ(mode) {
    ಪಕ್ಷ UNORM_NFD:
        ಪ್ರತ್ಯಾಗಮ Normalizer2::getNFDInstance(errorCode);
    ಪಕ್ಷ UNORM_NFKD:
        ಪ್ರತ್ಯಾಗಮ Normalizer2::getNFKDInstance(errorCode);
    ಪಕ್ಷ UNORM_NFC:
        ಪ್ರತ್ಯಾಗಮ Normalizer2::getNFCInstance(errorCode);
    ಪಕ್ಷ UNORM_NFKC:
        ಪ್ರತ್ಯಾಗಮ Normalizer2::getNFKCInstance(errorCode);
    ಪಕ್ಷ UNORM_FCD:
        ಪ್ರತ್ಯಾಗಮ getFCDInstance(errorCode);
    ಸ್ವಭಾವ:  // UNORM_NONE
        ಪ್ರತ್ಯಾಗಮ getNoopInstance(errorCode);
    }
}

ಧ್ರುವ Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? allModes->impl : nullptr;
}

ಧ್ರುವ Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ಧ್ರುವ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ಪ್ರತ್ಯಾಗಮ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ಧ್ರುವ UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ಧ್ರುವ UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ಧ್ರುವ UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ಧ್ರುವ UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ಧ್ರುವ UNormalizer2 * U_EXPORT2
unorm2_getInstance(ಧ್ರುವ ವರ್ಣ *packageName,
                   ಧ್ರುವ ವರ್ಣ *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    ಯದಿ(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ಪ್ರತ್ಯಾಗಮ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ಧ್ರುವ Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    ಯದಿ(U_SUCCESS(errorCode)) {
        ಪ್ರತ್ಯಾಗಮ ((ಧ್ರುವ Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
