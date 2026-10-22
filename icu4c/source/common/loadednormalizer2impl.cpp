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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/localpointer.h"
#അന്തര്ഭാവ "unicode/normalizer2.h"
#അന്തര്ഭാവ "unicode/ucptrie.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/unorm.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "mutex.h"
#അന്തര്ഭാവ "norm2allmodes.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uhash.h"

U_NAMESPACE_BEGIN

വർഗ്ഗം LoadedNormalizer2Impl : പൊതു Normalizer2Impl {
പൊതു:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    മായാ ~LoadedNormalizer2Impl();

    ശൂന്യം load(സ്ഥിരം അക്ഷരം *packageName, സ്ഥിരം അക്ഷരം *name, UErrorCode &errorCode);

സ്വകാര്യ:
    സ്ഥിതം UBool U_CALLCONV
    isAcceptable(ശൂന്യം *context, സ്ഥിരം അക്ഷരം *type, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(ശൂന്യം * /*context*/,
                                    സ്ഥിരം അക്ഷരം * /* type */, സ്ഥിരം അക്ഷരം * /*name*/,
                                    സ്ഥിരം UDataInfo *pInfo) {
    എങ്കിൽ(
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
        തിരികെ സത്യം;
    } അല്ലെങ്കിൽ {
        തിരികെ അസത്യം;
    }
}

ശൂന്യം
LoadedNormalizer2Impl::load(സ്ഥിരം അക്ഷരം *packageName, സ്ഥിരം അക്ഷരം *name, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, ഇത്, &errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }
    സ്ഥിരം uint8_t* inBytes = static_cast<സ്ഥിരം uint8_t*>(udata_getMemory(memory));
    സ്ഥിരം int32_t* inIndexes = reinterpret_cast<സ്ഥിരം int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    എങ്കിൽ(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        തിരികെ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_ദ്വിമാന(UCPTRIE_TYPE_FAST, UCPTRIE_മൂല്യം_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    സ്ഥിരം uint16_t* inExtraData = reinterpret_cast<സ്ഥിരം uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    സ്ഥിരം uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(സ്ഥിരം അക്ഷരം *packageName,
                              സ്ഥിരം അക്ഷരം *name,
                              UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    LoadedNormalizer2Impl *impl=പുതിയ LoadedNormalizer2Impl;
    എങ്കിൽ(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }
    impl->load(packageName, name, errorCode);
    തിരികെ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
സ്ഥിതം UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#എങ്കിൽ !NORM2_HARDCODE_NFC_DATA
സ്ഥിതം Norm2AllModes *nfcSingleton;
സ്ഥിതം icu::UInitOnce nfcInitOnce {};
#endif

സ്ഥിതം Norm2AllModes *nfkcSingleton;
സ്ഥിതം icu::UInitOnce nfkcInitOnce {};

സ്ഥിതം Norm2AllModes *nfkc_cfSingleton;
സ്ഥിതം icu::UInitOnce nfkc_cfInitOnce {};

സ്ഥിതം Norm2AllModes *nfkc_scfSingleton;
സ്ഥിതം icu::UInitOnce nfkc_scfInitOnce {};

സ്ഥിതം UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
സ്ഥിതം ശൂന്യം U_CALLCONV initSingletons(സ്ഥിരം അക്ഷരം *what, UErrorCode &errorCode) {
#എങ്കിൽ !NORM2_HARDCODE_NFC_DATA
    എങ്കിൽ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } അല്ലെങ്കിൽ
#endif
    എങ്കിൽ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } അല്ലെങ്കിൽ എങ്കിൽ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } അല്ലെങ്കിൽ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

സ്ഥിതം ശൂന്യം U_CALLCONV deleteNorm2AllModes(ശൂന്യം *allModes) {
    delete (Norm2AllModes *)allModes;
}

സ്ഥിതം UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#എങ്കിൽ !NORM2_HARDCODE_NFC_DATA
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
    തിരികെ സത്യം;
}

U_CDECL_END

#എങ്കിൽ !NORM2_HARDCODE_NFC_DATA
സ്ഥിരം Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    തിരികെ nfcSingleton;
}
#endif

സ്ഥിരം Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    തിരികെ nfkcSingleton;
}

സ്ഥിരം Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    തിരികെ nfkc_cfSingleton;
}

സ്ഥിരം Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) { തിരികെ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    തിരികെ nfkc_scfSingleton;
}

#എങ്കിൽ !NORM2_HARDCODE_NFC_DATA
സ്ഥിരം Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->comp : nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->decomp : nullptr;
}

സ്ഥിരം Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->fcd : nullptr;
}

സ്ഥിരം Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->fcc : nullptr;
}

സ്ഥിരം Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    തിരികെ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

സ്ഥിരം Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->comp : nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->decomp : nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->comp : nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    തിരികെ allModes!=nullptr ? &allModes->comp : nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2::getInstance(സ്ഥിരം അക്ഷരം *packageName,
                         സ്ഥിരം അക്ഷരം *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    എങ്കിൽ(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ nullptr;
    }
    സ്ഥിരം Norm2AllModes *allModes=nullptr;
    എങ്കിൽ(packageName==nullptr) {
        എങ്കിൽ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } അല്ലെങ്കിൽ എങ്കിൽ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    എങ്കിൽ(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            എങ്കിൽ(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        എങ്കിൽ(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            എങ്കിൽ(U_SUCCESS(errorCode)) {
                Mutex lock;
                എങ്കിൽ(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    എങ്കിൽ(U_FAILURE(errorCode)) {
                        തിരികെ nullptr;
                    }
                    uhash_set_താക്കോൽ_Deleter(cache, uprv_free);
                    uhash_set_മൂല്യം_Deleter(cache, deleteNorm2AllModes);
                }
                ശൂന്യം *temp=uhash_get(cache, name);
                എങ്കിൽ(temp==nullptr) {
                    int32_t താക്കോൽ_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    അക്ഷരം* nameCopy = static_cast<അക്ഷരം*>(uprv_malloc(താക്കോൽ_Length));
                    എങ്കിൽ(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        തിരികെ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, താക്കോൽ_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } അല്ലെങ്കിൽ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    എങ്കിൽ(allModes!=nullptr && U_SUCCESS(errorCode)) {
        തിരഞ്ഞെടുപ്പ്(mode) {
        അവസ്ഥ UNORM2_COMPOSE:
            തിരികെ &allModes->comp;
        അവസ്ഥ UNORM2_DECOMPOSE:
            തിരികെ &allModes->decomp;
        അവസ്ഥ UNORM2_FCD:
            തിരികെ &allModes->fcd;
        അവസ്ഥ UNORM2_COMPOSE_CONTIGUOUS:
            തിരികെ &allModes->fcc;
        സ്ഥിരസ്ഥിതി:
            നിർത്തുക;  // do nothing
        }
    }
    തിരികെ nullptr;
}

സ്ഥിരം Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    എങ്കിൽ(U_FAILURE(errorCode)) {
        തിരികെ nullptr;
    }
    തിരഞ്ഞെടുപ്പ്(mode) {
    അവസ്ഥ UNORM_NFD:
        തിരികെ Normalizer2::getNFDInstance(errorCode);
    അവസ്ഥ UNORM_NFKD:
        തിരികെ Normalizer2::getNFKDInstance(errorCode);
    അവസ്ഥ UNORM_NFC:
        തിരികെ Normalizer2::getNFCInstance(errorCode);
    അവസ്ഥ UNORM_NFKC:
        തിരികെ Normalizer2::getNFKCInstance(errorCode);
    അവസ്ഥ UNORM_FCD:
        തിരികെ getFCDInstance(errorCode);
    സ്ഥിരസ്ഥിതി:  // UNORM_NONE
        തിരികെ getNoopInstance(errorCode);
    }
}

സ്ഥിരം Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    തിരികെ allModes!=nullptr ? allModes->impl : nullptr;
}

സ്ഥിരം Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    സ്ഥിരം Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    തിരികെ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI സ്ഥിരം UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    തിരികെ (സ്ഥിരം UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI സ്ഥിരം UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    തിരികെ (സ്ഥിരം UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI സ്ഥിരം UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    തിരികെ (സ്ഥിരം UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI സ്ഥിരം UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    തിരികെ (സ്ഥിരം UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI സ്ഥിരം UNormalizer2 * U_EXPORT2
unorm2_getInstance(സ്ഥിരം അക്ഷരം *packageName,
                   സ്ഥിരം അക്ഷരം *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    തിരികെ (സ്ഥിരം UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    എങ്കിൽ(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        തിരികെ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    സ്ഥിരം Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    എങ്കിൽ(U_SUCCESS(errorCode)) {
        തിരികെ ((സ്ഥിരം Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } അല്ലെങ്കിൽ {
        തിരികെ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
