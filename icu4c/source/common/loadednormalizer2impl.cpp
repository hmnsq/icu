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

#אם !UCONFIG_NO_NORMALIZATION

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

מחלקה LoadedNormalizer2Impl : عام Normalizer2Impl {
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
    אם(
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
        החזר אמת;
    } אחרת {
        החזר שקר;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &errorCode);
    אם(U_FAILURE(errorCode)) {
        החזר;
    }
    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(memory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    אם(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        החזר;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    אם(U_FAILURE(errorCode)) {
        החזר;
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
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    אם(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        החזר nullptr;
    }
    impl->load(packageName, name, errorCode);
    החזר createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#אם !NORM2_HARDCODE_NFC_DATA
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
#אם !NORM2_HARDCODE_NFC_DATA
    אם (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } אחרת
#endif
    אם (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } אחרת אם (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } אחרת אם (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } אחרת {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    delete (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#אם !NORM2_HARDCODE_NFC_DATA
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
    החזר אמת;
}

U_CDECL_END

#אם !NORM2_HARDCODE_NFC_DATA
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) { החזר nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    החזר nfcSingleton;
}
#endif

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) { החזר nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    החזר nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) { החזר nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    החזר nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) { החזר nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    החזר nfkc_scfSingleton;
}

#אם !NORM2_HARDCODE_NFC_DATA
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->fcd : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->fcc : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    החזר allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    החזר allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    אם(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        החזר nullptr;
    }
    ثابت Norm2AllModes *allModes=nullptr;
    אם(packageName==nullptr) {
        אם(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } אחרת אם(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } אחרת אם(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } אחרת אם(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    אם(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            אם(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        אם(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            אם(U_SUCCESS(errorCode)) {
                Mutex lock;
                אם(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    אם(U_FAILURE(errorCode)) {
                        החזר nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                فراغ *temp=uhash_get(cache, name);
                אם(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(keyLength));
                    אם(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        החזר nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } אחרת {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    אם(allModes!=nullptr && U_SUCCESS(errorCode)) {
        بدل(mode) {
        حالة UNORM2_COMPOSE:
            החזר &allModes->comp;
        حالة UNORM2_DECOMPOSE:
            החזר &allModes->decomp;
        حالة UNORM2_FCD:
            החזר &allModes->fcd;
        حالة UNORM2_COMPOSE_CONTIGUOUS:
            החזר &allModes->fcc;
        افتراضي:
            اكسر;  // do nothing
        }
    }
    החזר nullptr;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    אם(U_FAILURE(errorCode)) {
        החזר nullptr;
    }
    بدل(mode) {
    حالة UNORM_NFD:
        החזר Normalizer2::getNFDInstance(errorCode);
    حالة UNORM_NFKD:
        החזר Normalizer2::getNFKDInstance(errorCode);
    حالة UNORM_NFC:
        החזר Normalizer2::getNFCInstance(errorCode);
    حالة UNORM_NFKC:
        החזר Normalizer2::getNFKCInstance(errorCode);
    حالة UNORM_FCD:
        החזר getFCDInstance(errorCode);
    افتراضي:  // UNORM_NONE
        החזר getNoopInstance(errorCode);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    החזר allModes!=nullptr ? allModes->impl : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    החזר allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    החזר (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    החזר (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    החזר (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    החזר (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    החזר (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    אם(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        החזר UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    אם(U_SUCCESS(errorCode)) {
        החזר ((ثابت Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } אחרת {
        החזר UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
