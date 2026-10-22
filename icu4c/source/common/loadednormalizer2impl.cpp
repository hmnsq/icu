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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/localpointer.h"
#ଅନ୍ତର୍ଭାଵ "unicode/normalizer2.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ucptrie.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unorm.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "mutex.h"
#ଅନ୍ତର୍ଭାଵ "norm2allmodes.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"

U_NAMESPACE_BEGIN

ଶ୍ରେଣୀ LoadedNormalizer2Impl : ସର୍ବସାଧାରଣ Normalizer2Impl {
ସର୍ବସାଧାରଣ:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    ଆଭାସୀ ~LoadedNormalizer2Impl();

    ଶୂନ୍ୟ load(ଅଚଳ ଅକ୍ଷର *packageName, ଅଚଳ ଅକ୍ଷର *name, UErrorCode &errorCode);

ବ୍ୟକ୍ତିଗତ:
    ସ୍ଥିର UBool U_CALLCONV
    isAcceptable(ଶୂନ୍ୟ *context, ଅଚଳ ଅକ୍ଷର *type, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(ଶୂନ୍ୟ * /*context*/,
                                    ଅଚଳ ଅକ୍ଷର * /* type */, ଅଚଳ ଅକ୍ଷର * /*name*/,
                                    ଅଚଳ UDataInfo *pInfo) {
    ଯଦି(
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
        ଫେରନ୍ତୁ ସତ୍ୟ;
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
}

ଶୂନ୍ୟ
LoadedNormalizer2Impl::load(ଅଚଳ ଅକ୍ଷର *packageName, ଅଚଳ ଅକ୍ଷର *name, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, ଏହି, &errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }
    ଅଚଳ uint8_t* inBytes = static_cast<ଅଚଳ uint8_t*>(udata_getMemory(memory));
    ଅଚଳ int32_t* inIndexes = reinterpret_cast<ଅଚଳ int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    ଯଦି(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ଫେରନ୍ତୁ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_ଦ୍ୱିମିକ(UCPTRIE_TYPE_FAST, UCPTRIE_ମୂଲ୍ୟ_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ଅଚଳ uint16_t* inExtraData = reinterpret_cast<ଅଚଳ uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ଅଚଳ uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ଅଚଳ ଅକ୍ଷର *packageName,
                              ଅଚଳ ଅକ୍ଷର *name,
                              UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    LoadedNormalizer2Impl *impl=ନୂତନ LoadedNormalizer2Impl;
    ଯଦି(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    impl->load(packageName, name, errorCode);
    ଫେରନ୍ତୁ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ସ୍ଥିର UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#ଯଦି !NORM2_HARDCODE_NFC_DATA
ସ୍ଥିର Norm2AllModes *nfcSingleton;
ସ୍ଥିର icu::UInitOnce nfcInitOnce {};
#endif

ସ୍ଥିର Norm2AllModes *nfkcSingleton;
ସ୍ଥିର icu::UInitOnce nfkcInitOnce {};

ସ୍ଥିର Norm2AllModes *nfkc_cfSingleton;
ସ୍ଥିର icu::UInitOnce nfkc_cfInitOnce {};

ସ୍ଥିର Norm2AllModes *nfkc_scfSingleton;
ସ୍ଥିର icu::UInitOnce nfkc_scfInitOnce {};

ସ୍ଥିର UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV initSingletons(ଅଚଳ ଅକ୍ଷର *what, UErrorCode &errorCode) {
#ଯଦି !NORM2_HARDCODE_NFC_DATA
    ଯଦି (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } ଅନ୍ୟଥା
#endif
    ଯଦି (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } ଅନ୍ୟଥା ଯଦି (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } ଅନ୍ୟଥା {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV deleteNorm2AllModes(ଶୂନ୍ୟ *allModes) {
    delete (Norm2AllModes *)allModes;
}

ସ୍ଥିର UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ଯଦି !NORM2_HARDCODE_NFC_DATA
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
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_CDECL_END

#ଯଦି !NORM2_HARDCODE_NFC_DATA
ଅଚଳ Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ଫେରନ୍ତୁ nfcSingleton;
}
#endif

ଅଚଳ Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ଫେରନ୍ତୁ nfkcSingleton;
}

ଅଚଳ Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ଫେରନ୍ତୁ nfkc_cfSingleton;
}

ଅଚଳ Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) { ଫେରନ୍ତୁ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ଫେରନ୍ତୁ nfkc_scfSingleton;
}

#ଯଦି !NORM2_HARDCODE_NFC_DATA
ଅଚଳ Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->comp : nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ଅଚଳ Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->fcd : nullptr;
}

ଅଚଳ Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->fcc : nullptr;
}

ଅଚଳ Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ଅଚଳ Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->comp : nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->comp : nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? &allModes->comp : nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2::getInstance(ଅଚଳ ଅକ୍ଷର *packageName,
                         ଅଚଳ ଅକ୍ଷର *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଯଦି(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ Norm2AllModes *allModes=nullptr;
    ଯଦି(packageName==nullptr) {
        ଯଦି(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } ଅନ୍ୟଥା ଯଦି(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } ଅନ୍ୟଥା ଯଦି(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } ଅନ୍ୟଥା ଯଦି(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    ଯଦି(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            ଯଦି(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ଯଦି(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            ଯଦି(U_SUCCESS(errorCode)) {
                Mutex lock;
                ଯଦି(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    ଯଦି(U_FAILURE(errorCode)) {
                        ଫେରନ୍ତୁ nullptr;
                    }
                    uhash_set_ଚାବି_Deleter(cache, uprv_free);
                    uhash_set_ମୂଲ୍ୟ_Deleter(cache, deleteNorm2AllModes);
                }
                ଶୂନ୍ୟ *temp=uhash_get(cache, name);
                ଯଦି(temp==nullptr) {
                    int32_t ଚାବି_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    ଅକ୍ଷର* nameCopy = static_cast<ଅକ୍ଷର*>(uprv_malloc(ଚାବି_Length));
                    ଯଦି(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ଫେରନ୍ତୁ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, ଚାବି_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } ଅନ୍ୟଥା {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    ଯଦି(allModes!=nullptr && U_SUCCESS(errorCode)) {
        ବାଛ(mode) {
        ସ୍ଥିତି UNORM2_COMPOSE:
            ଫେରନ୍ତୁ &allModes->comp;
        ସ୍ଥିତି UNORM2_DECOMPOSE:
            ଫେରନ୍ତୁ &allModes->decomp;
        ସ୍ଥିତି UNORM2_FCD:
            ଫେରନ୍ତୁ &allModes->fcd;
        ସ୍ଥିତି UNORM2_COMPOSE_CONTIGUOUS:
            ଫେରନ୍ତୁ &allModes->fcc;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ଭାଙ୍ଗ;  // do nothing
        }
    }
    ଫେରନ୍ତୁ nullptr;
}

ଅଚଳ Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    ଯଦି(U_FAILURE(errorCode)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ବାଛ(mode) {
    ସ୍ଥିତି UNORM_NFD:
        ଫେରନ୍ତୁ Normalizer2::getNFDInstance(errorCode);
    ସ୍ଥିତି UNORM_NFKD:
        ଫେରନ୍ତୁ Normalizer2::getNFKDInstance(errorCode);
    ସ୍ଥିତି UNORM_NFC:
        ଫେରନ୍ତୁ Normalizer2::getNFCInstance(errorCode);
    ସ୍ଥିତି UNORM_NFKC:
        ଫେରନ୍ତୁ Normalizer2::getNFKCInstance(errorCode);
    ସ୍ଥିତି UNORM_FCD:
        ଫେରନ୍ତୁ getFCDInstance(errorCode);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // UNORM_NONE
        ଫେରନ୍ତୁ getNoopInstance(errorCode);
    }
}

ଅଚଳ Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? allModes->impl : nullptr;
}

ଅଚଳ Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ଅଚଳ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ଫେରନ୍ତୁ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ଅଚଳ UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ (ଅଚଳ UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ଅଚଳ UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ (ଅଚଳ UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ଅଚଳ UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ (ଅଚଳ UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ଅଚଳ UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ (ଅଚଳ UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ଅଚଳ UNormalizer2 * U_EXPORT2
unorm2_getInstance(ଅଚଳ ଅକ୍ଷର *packageName,
                   ଅଚଳ ଅକ୍ଷର *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ (ଅଚଳ UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    ଯଦି(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ଫେରନ୍ତୁ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ଅଚଳ Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    ଯଦି(U_SUCCESS(errorCode)) {
        ଫେରନ୍ତୁ ((ଅଚଳ Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
