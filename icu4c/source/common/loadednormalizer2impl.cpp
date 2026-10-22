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

#ضمن "unicode/utypes.h"

#ܐܢ !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/udata.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/normalizer2.h"
#ضمن "unicode/ucptrie.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/unorm.h"
#ضمن "cstring.h"
#ضمن "mutex.h"
#ضمن "norm2allmodes.h"
#ضمن "normalizer2impl.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"

U_NAMESPACE_BEGIN

صنف LoadedNormalizer2Impl : عام Normalizer2Impl {
عام:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    virtual ~LoadedNormalizer2Impl();

    فراغ load(ثابت حرف *packageName, ثابت حرف *name, UErrorCode &errorCode);

خاص:
    ساكن UBool U_CALLCONV
    isAcceptable(فراغ *context, ثابت حرف *type, ثابت حرف *name, ثابت UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(فراغ * /*context*/,
                                    ثابت حرف * /* type */, ثابت حرف * /*name*/,
                                    ثابت UDataInfo *pInfo) {
    ܐܢ(
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
        ܦܢܐ ܫܪܪܐ;
    } ܐܚܪܢܐ {
        ܦܢܐ ܕܓܠܐ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }
    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(memory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    ܐܢ(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ܦܢܐ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ثابت uint16_t* inExtraData = reinterpret_cast<ثابت uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ثابت uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ثابت حرف *packageName,
                              ثابت حرف *name,
                              UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ nullptr;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    ܐܢ(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ nullptr;
    }
    impl->load(packageName, name, errorCode);
    ܦܢܐ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#ܐܢ !NORM2_HARDCODE_NFC_DATA
ساكن Norm2AllModes *nfcSingleton;
ساكن icu::UInitOnce nfcInitOnce {};
#endif

ساكن Norm2AllModes *nfkcSingleton;
ساكن icu::UInitOnce nfkcInitOnce {};

ساكن Norm2AllModes *nfkc_cfSingleton;
ساكن icu::UInitOnce nfkc_cfInitOnce {};

ساكن Norm2AllModes *nfkc_scfSingleton;
ساكن icu::UInitOnce nfkc_scfInitOnce {};

ساكن UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
ساكن فراغ U_CALLCONV initSingletons(ثابت حرف *what, UErrorCode &errorCode) {
#ܐܢ !NORM2_HARDCODE_NFC_DATA
    ܐܢ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } ܐܚܪܢܐ
#endif
    ܐܢ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } ܐܚܪܢܐ ܐܢ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } ܐܚܪܢܐ ܐܢ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } ܐܚܪܢܐ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    delete (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ܐܢ !NORM2_HARDCODE_NFC_DATA
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
    ܦܢܐ ܫܪܪܐ;
}

U_CDECL_END

#ܐܢ !NORM2_HARDCODE_NFC_DATA
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) { ܦܢܐ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ܦܢܐ nfcSingleton;
}
#endif

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) { ܦܢܐ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ܦܢܐ nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) { ܦܢܐ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ܦܢܐ nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) { ܦܢܐ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ܦܢܐ nfkc_scfSingleton;
}

#ܐܢ !NORM2_HARDCODE_NFC_DATA
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->fcd : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->fcc : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ nullptr;
    }
    ܐܢ(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ܦܢܐ nullptr;
    }
    ثابت Norm2AllModes *allModes=nullptr;
    ܐܢ(packageName==nullptr) {
        ܐܢ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } ܐܚܪܢܐ ܐܢ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } ܐܚܪܢܐ ܐܢ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } ܐܚܪܢܐ ܐܢ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    ܐܢ(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            ܐܢ(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ܐܢ(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            ܐܢ(U_SUCCESS(errorCode)) {
                Mutex lock;
                ܐܢ(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    ܐܢ(U_FAILURE(errorCode)) {
                        ܦܢܐ nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                فراغ *temp=uhash_get(cache, name);
                ܐܢ(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(keyLength));
                    ܐܢ(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ܦܢܐ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } ܐܚܪܢܐ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    ܐܢ(allModes!=nullptr && U_SUCCESS(errorCode)) {
        بدل(mode) {
        حالة UNORM2_COMPOSE:
            ܦܢܐ &allModes->comp;
        حالة UNORM2_DECOMPOSE:
            ܦܢܐ &allModes->decomp;
        حالة UNORM2_FCD:
            ܦܢܐ &allModes->fcd;
        حالة UNORM2_COMPOSE_CONTIGUOUS:
            ܦܢܐ &allModes->fcc;
        افتراضي:
            اكسر;  // do nothing
        }
    }
    ܦܢܐ nullptr;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    ܐܢ(U_FAILURE(errorCode)) {
        ܦܢܐ nullptr;
    }
    بدل(mode) {
    حالة UNORM_NFD:
        ܦܢܐ Normalizer2::getNFDInstance(errorCode);
    حالة UNORM_NFKD:
        ܦܢܐ Normalizer2::getNFKDInstance(errorCode);
    حالة UNORM_NFC:
        ܦܢܐ Normalizer2::getNFCInstance(errorCode);
    حالة UNORM_NFKC:
        ܦܢܐ Normalizer2::getNFKCInstance(errorCode);
    حالة UNORM_FCD:
        ܦܢܐ getFCDInstance(errorCode);
    افتراضي:  // UNORM_NONE
        ܦܢܐ getNoopInstance(errorCode);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? allModes->impl : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ܦܢܐ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ܦܢܐ (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ܦܢܐ (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ܦܢܐ (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ܦܢܐ (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ܦܢܐ (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    ܐܢ(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ܦܢܐ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    ܐܢ(U_SUCCESS(errorCode)) {
        ܦܢܐ ((ثابت Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } ܐܚܪܢܐ {
        ܦܢܐ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
