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

#إذا !UCONFIG_NO_NORMALIZATION

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
    إذا(
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
        إرجاع true;
    } وإلا {
        إرجاع خطأ;
    }
}

فراغ
LoadedNormalizer2Impl::load(ثابت حرف *packageName, ثابت حرف *name, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, هذا, &errorCode);
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
    }
    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(memory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    إذا(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        إرجاع;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_ثنائي(UCPTRIE_TYPE_FAST, UCPTRIE_قيمة_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    إذا(U_FAILURE(errorCode)) {
        إرجاع;
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
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    LoadedNormalizer2Impl *impl=جديد LoadedNormalizer2Impl;
    إذا(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        إرجاع nullptr;
    }
    impl->load(packageName, name, errorCode);
    إرجاع createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#إذا !NORM2_HARDCODE_NFC_DATA
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
#إذا !NORM2_HARDCODE_NFC_DATA
    إذا (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } وإلا
#endif
    إذا (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } وإلا إذا (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } وإلا إذا (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } وإلا {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ساكن فراغ U_CALLCONV deleteNorm2AllModes(فراغ *allModes) {
    delete (Norm2AllModes *)allModes;
}

ساكن UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#إذا !NORM2_HARDCODE_NFC_DATA
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
    إرجاع true;
}

U_CDECL_END

#إذا !NORM2_HARDCODE_NFC_DATA
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) { إرجاع nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    إرجاع nfcSingleton;
}
#endif

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) { إرجاع nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    إرجاع nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) { إرجاع nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    إرجاع nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) { إرجاع nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    إرجاع nfkc_scfSingleton;
}

#إذا !NORM2_HARDCODE_NFC_DATA
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->fcd : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->fcc : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    إرجاع allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    إرجاع allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت حرف *packageName,
                         ثابت حرف *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    إذا(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        إرجاع nullptr;
    }
    ثابت Norm2AllModes *allModes=nullptr;
    إذا(packageName==nullptr) {
        إذا(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } وإلا إذا(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    إذا(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            إذا(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        إذا(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            إذا(U_SUCCESS(errorCode)) {
                Mutex lock;
                إذا(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    إذا(U_FAILURE(errorCode)) {
                        إرجاع nullptr;
                    }
                    uhash_set_مفتاح_Deleter(cache, uprv_free);
                    uhash_set_قيمة_Deleter(cache, deleteNorm2AllModes);
                }
                فراغ *temp=uhash_get(cache, name);
                إذا(temp==nullptr) {
                    int32_t مفتاح_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    حرف* nameCopy = static_cast<حرف*>(uprv_malloc(مفتاح_Length));
                    إذا(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        إرجاع nullptr;
                    }
                    uprv_memcpy(nameCopy, name, مفتاح_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } وإلا {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    إذا(allModes!=nullptr && U_SUCCESS(errorCode)) {
        بدل(mode) {
        حالة UNORM2_COMPOSE:
            إرجاع &allModes->comp;
        حالة UNORM2_DECOMPOSE:
            إرجاع &allModes->decomp;
        حالة UNORM2_FCD:
            إرجاع &allModes->fcd;
        حالة UNORM2_COMPOSE_CONTIGUOUS:
            إرجاع &allModes->fcc;
        افتراضي:
            اكسر;  // do nothing
        }
    }
    إرجاع nullptr;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    إذا(U_FAILURE(errorCode)) {
        إرجاع nullptr;
    }
    بدل(mode) {
    حالة UNORM_NFD:
        إرجاع Normalizer2::getNFDInstance(errorCode);
    حالة UNORM_NFKD:
        إرجاع Normalizer2::getNFKDInstance(errorCode);
    حالة UNORM_NFC:
        إرجاع Normalizer2::getNFCInstance(errorCode);
    حالة UNORM_NFKC:
        إرجاع Normalizer2::getNFKCInstance(errorCode);
    حالة UNORM_FCD:
        إرجاع getFCDInstance(errorCode);
    افتراضي:  // UNORM_NONE
        إرجاع getNoopInstance(errorCode);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    إرجاع allModes!=nullptr ? allModes->impl : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    إرجاع allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    إرجاع (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    إرجاع (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    إرجاع (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    إرجاع (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت حرف *packageName,
                   ثابت حرف *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    إرجاع (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    إذا(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        إرجاع UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    إذا(U_SUCCESS(errorCode)) {
        إرجاع ((ثابت Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } وإلا {
        إرجاع UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
