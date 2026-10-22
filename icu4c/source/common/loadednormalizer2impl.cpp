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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/udata.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/normalizer2.h"
#شامل "unicode/ucptrie.h"
#شامل "unicode/unistr.h"
#شامل "unicode/unorm.h"
#شامل "cstring.h"
#شامل "mutex.h"
#شامل "norm2allmodes.h"
#شامل "normalizer2impl.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"

U_NAMESPACE_BEGIN

رده LoadedNormalizer2Impl : عمومی Normalizer2Impl {
عمومی:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    مجازی ~LoadedNormalizer2Impl();

    تهی load(ثابت نویسه *packageName, ثابت نویسه *name, UErrorCode &errorCode);

خصوصی:
    ایستا UBool U_CALLCONV
    isAcceptable(تهی *context, ثابت نویسه *type, ثابت نویسه *name, ثابت UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(تهی * /*context*/,
                                    ثابت نویسه * /* type */, ثابت نویسه * /*name*/,
                                    ثابت UDataInfo *pInfo) {
    اگر(
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
        بازگشت درست;
    } وگرنه {
        بازگشت نادرست;
    }
}

تهی
LoadedNormalizer2Impl::load(ثابت نویسه *packageName, ثابت نویسه *name, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, این, &errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
    }
    ثابت uint8_t* inBytes = static_cast<ثابت uint8_t*>(udata_getMemory(memory));
    ثابت int32_t* inIndexes = reinterpret_cast<ثابت int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    اگر(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        بازگشت;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_دودویی(UCPTRIE_TYPE_FAST, UCPTRIE_مقدار_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    اگر(U_FAILURE(errorCode)) {
        بازگشت;
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
Norm2AllModes::createInstance(ثابت نویسه *packageName,
                              ثابت نویسه *name,
                              UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    LoadedNormalizer2Impl *impl=جدید LoadedNormalizer2Impl;
    اگر(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }
    impl->load(packageName, name, errorCode);
    بازگشت createInstance(impl, errorCode);
}

U_CDECL_BEGIN
ایستا UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#اگر !NORM2_HARDCODE_NFC_DATA
ایستا Norm2AllModes *nfcSingleton;
ایستا icu::UInitOnce nfcInitOnce {};
#endif

ایستا Norm2AllModes *nfkcSingleton;
ایستا icu::UInitOnce nfkcInitOnce {};

ایستا Norm2AllModes *nfkc_cfSingleton;
ایستا icu::UInitOnce nfkc_cfInitOnce {};

ایستا Norm2AllModes *nfkc_scfSingleton;
ایستا icu::UInitOnce nfkc_scfInitOnce {};

ایستا UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
ایستا تهی U_CALLCONV initSingletons(ثابت نویسه *what, UErrorCode &errorCode) {
#اگر !NORM2_HARDCODE_NFC_DATA
    اگر (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } وگرنه
#endif
    اگر (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } وگرنه اگر (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } وگرنه اگر (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } وگرنه {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

ایستا تهی U_CALLCONV deleteNorm2AllModes(تهی *allModes) {
    delete (Norm2AllModes *)allModes;
}

ایستا UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#اگر !NORM2_HARDCODE_NFC_DATA
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
    بازگشت درست;
}

U_CDECL_END

#اگر !NORM2_HARDCODE_NFC_DATA
ثابت Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    بازگشت nfcSingleton;
}
#endif

ثابت Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    بازگشت nfkcSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    بازگشت nfkc_cfSingleton;
}

ثابت Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) { بازگشت nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    بازگشت nfkc_scfSingleton;
}

#اگر !NORM2_HARDCODE_NFC_DATA
ثابت Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->fcd : nullptr;
}

ثابت Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->fcc : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    بازگشت allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ثابت Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->decomp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    بازگشت allModes!=nullptr ? &allModes->comp : nullptr;
}

ثابت Normalizer2 *
Normalizer2::getInstance(ثابت نویسه *packageName,
                         ثابت نویسه *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    اگر(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت nullptr;
    }
    ثابت Norm2AllModes *allModes=nullptr;
    اگر(packageName==nullptr) {
        اگر(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } وگرنه اگر(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } وگرنه اگر(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } وگرنه اگر(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    اگر(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            اگر(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        اگر(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            اگر(U_SUCCESS(errorCode)) {
                Mutex lock;
                اگر(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    اگر(U_FAILURE(errorCode)) {
                        بازگشت nullptr;
                    }
                    uhash_set_کلید_Deleter(cache, uprv_free);
                    uhash_set_مقدار_Deleter(cache, deleteNorm2AllModes);
                }
                تهی *temp=uhash_get(cache, name);
                اگر(temp==nullptr) {
                    int32_t کلید_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    نویسه* nameCopy = static_cast<نویسه*>(uprv_malloc(کلید_Length));
                    اگر(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        بازگشت nullptr;
                    }
                    uprv_memcpy(nameCopy, name, کلید_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } وگرنه {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    اگر(allModes!=nullptr && U_SUCCESS(errorCode)) {
        انتخاب(mode) {
        حالت UNORM2_COMPOSE:
            بازگشت &allModes->comp;
        حالت UNORM2_DECOMPOSE:
            بازگشت &allModes->decomp;
        حالت UNORM2_FCD:
            بازگشت &allModes->fcd;
        حالت UNORM2_COMPOSE_CONTIGUOUS:
            بازگشت &allModes->fcc;
        پیشفرض:
            بشکن;  // do nothing
        }
    }
    بازگشت nullptr;
}

ثابت Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    اگر(U_FAILURE(errorCode)) {
        بازگشت nullptr;
    }
    انتخاب(mode) {
    حالت UNORM_NFD:
        بازگشت Normalizer2::getNFDInstance(errorCode);
    حالت UNORM_NFKD:
        بازگشت Normalizer2::getNFKDInstance(errorCode);
    حالت UNORM_NFC:
        بازگشت Normalizer2::getNFCInstance(errorCode);
    حالت UNORM_NFKC:
        بازگشت Normalizer2::getNFKCInstance(errorCode);
    حالت UNORM_FCD:
        بازگشت getFCDInstance(errorCode);
    پیشفرض:  // UNORM_NONE
        بازگشت getNoopInstance(errorCode);
    }
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    بازگشت allModes!=nullptr ? allModes->impl : nullptr;
}

ثابت Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ثابت Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    بازگشت allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    بازگشت (ثابت UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    بازگشت (ثابت UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    بازگشت (ثابت UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    بازگشت (ثابت UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ثابت UNormalizer2 * U_EXPORT2
unorm2_getInstance(ثابت نویسه *packageName,
                   ثابت نویسه *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    بازگشت (ثابت UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    اگر(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        بازگشت UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ثابت Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    اگر(U_SUCCESS(errorCode)) {
        بازگشت ((ثابت Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } وگرنه {
        بازگشت UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
