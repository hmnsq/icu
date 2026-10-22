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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/localpointer.h"
#অন্তর্ভাব "unicode/normalizer2.h"
#অন্তর্ভাব "unicode/ucptrie.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/unorm.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "mutex.h"
#অন্তর্ভাব "norm2allmodes.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uhash.h"

U_NAMESPACE_BEGIN

শ্রেণী LoadedNormalizer2Impl : সর্বজনীন Normalizer2Impl {
সর্বজনীন:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    আভাসী ~LoadedNormalizer2Impl();

    শূন্য load(ধ্রুবক অক্ষর *packageName, ধ্রুবক অক্ষর *name, UErrorCode &errorCode);

ব্যক্তিগত:
    স্থির UBool U_CALLCONV
    isAcceptable(শূন্য *context, ধ্রুবক অক্ষর *type, ধ্রুবক অক্ষর *name, ধ্রুবক UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(শূন্য * /*context*/,
                                    ধ্রুবক অক্ষর * /* type */, ধ্রুবক অক্ষর * /*name*/,
                                    ধ্রুবক UDataInfo *pInfo) {
    যদি(
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->তথ্য_Format[0]==0x4e &&    /* dataFormat="Nrm2" */
        pInfo->তথ্য_Format[1]==0x72 &&
        pInfo->তথ্য_Format[2]==0x6d &&
        pInfo->তথ্য_Format[3]==0x32 &&
        pInfo->formatVersion[0]==5
    ) {
        // Normalizer2Impl *me=(Normalizer2Impl *)context;
        // uprv_memcpy(me->dataVersion, pInfo->dataVersion, 4);
        ফেরত সত্য;
    } অন্যথা {
        ফেরত মিথ্যা;
    }
}

শূন্য
LoadedNormalizer2Impl::load(ধ্রুবক অক্ষর *packageName, ধ্রুবক অক্ষর *name, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, এই, &errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }
    ধ্রুবক uint8_t* inBytes = static_cast<ধ্রুবক uint8_t*>(udata_getMemory(memory));
    ধ্রুবক int32_t* inIndexes = reinterpret_cast<ধ্রুবক int32_t*>(inBytes);
    int32_t indexes_দৈর্ঘ্য=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    যদি(indexes_দৈর্ঘ্য<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ফেরত;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t পরবর্তী_Offset=inIndexes[IX_EXTRA_তথ্য_OFFSET];
    ownedTrie=ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST, UCPTRIE_মান_BITS_16,
                                     inBytes+offset, পরবর্তী_Offset-offset, nullptr,
                                     &errorCode);
    যদি(U_FAILURE(errorCode)) {
        ফেরত;
    }

    offset=পরবর্তী_Offset;
    পরবর্তী_Offset=inIndexes[IX_SMALL_FCD_OFFSET];
    ধ্রুবক uint16_t* in_Extra_তথ্য = reinterpret_cast<ধ্রুবক uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=পরবর্তী_Offset;
    ধ্রুবক uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, in_Extra_তথ্য, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ধ্রুবক অক্ষর *packageName,
                              ধ্রুবক অক্ষর *name,
                              UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    LoadedNormalizer2Impl *impl=নতুন LoadedNormalizer2Impl;
    যদি(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ফেরত nullptr;
    }
    impl->load(packageName, name, errorCode);
    ফেরত createInstance(impl, errorCode);
}

U_CDECL_BEGIN
স্থির UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#যদি !NORM2_HARDCODE_NFC_তথ্য
স্থির Norm2AllModes *nfcSingleton;
স্থির icu::UInitOnce nfcInitOnce {};
#endif

স্থির Norm2AllModes *nfkcSingleton;
স্থির icu::UInitOnce nfkcInitOnce {};

স্থির Norm2AllModes *nfkc_cfSingleton;
স্থির icu::UInitOnce nfkc_cfInitOnce {};

স্থির Norm2AllModes *nfkc_scfSingleton;
স্থির icu::UInitOnce nfkc_scfInitOnce {};

স্থির UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
স্থির শূন্য U_CALLCONV initSingletons(ধ্রুবক অক্ষর *what, UErrorCode &errorCode) {
#যদি !NORM2_HARDCODE_NFC_তথ্য
    যদি (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } অন্যথা
#endif
    যদি (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } অন্যথা যদি (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } অন্যথা যদি (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } অন্যথা {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

স্থির শূন্য U_CALLCONV deleteNorm2AllModes(শূন্য *allModes) {
    delete (Norm2AllModes *)allModes;
}

স্থির UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#যদি !NORM2_HARDCODE_NFC_তথ্য
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
    ফেরত সত্য;
}

U_CDECL_END

#যদি !NORM2_HARDCODE_NFC_তথ্য
ধ্রুবক Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ফেরত nfcSingleton;
}
#endif

ধ্রুবক Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ফেরত nfkcSingleton;
}

ধ্রুবক Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ফেরত nfkc_cfSingleton;
}

ধ্রুবক Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { ফেরত nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ফেরত nfkc_scfSingleton;
}

#যদি !NORM2_HARDCODE_NFC_তথ্য
ধ্রুবক Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->decomp : nullptr;
}

ধ্রুবক Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->fcd : nullptr;
}

ধ্রুবক Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->fcc : nullptr;
}

ধ্রুবক Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ফেরত allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ধ্রুবক Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->decomp : nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ফেরত allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2::getInstance(ধ্রুবক অক্ষর *packageName,
                         ধ্রুবক অক্ষর *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    যদি(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত nullptr;
    }
    ধ্রুবক Norm2AllModes *allModes=nullptr;
    যদি(packageName==nullptr) {
        যদি(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } অন্যথা যদি(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } অন্যথা যদি(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } অন্যথা যদি(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    যদি(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            যদি(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        যদি(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            যদি(U_SUCCESS(errorCode)) {
                Mutex lock;
                যদি(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    যদি(U_FAILURE(errorCode)) {
                        ফেরত nullptr;
                    }
                    uhash_set_চাবি_Deleter(cache, uprv_free);
                    uhash_set_মান_Deleter(cache, deleteNorm2AllModes);
                }
                শূন্য *অস্থায়ী=uhash_get(cache, name);
                যদি(অস্থায়ী==nullptr) {
                    int32_t চাবি_দৈর্ঘ্য= static_cast<int32_t>(uprv_strlen(name)+1);
                    অক্ষর* nameCopy = static_cast<অক্ষর*>(uprv_malloc(চাবি_দৈর্ঘ্য));
                    যদি(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ফেরত nullptr;
                    }
                    uprv_memcpy(nameCopy, name, চাবি_দৈর্ঘ্য);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } অন্যথা {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(অস্থায়ী);
                }
            }
        }
    }
    যদি(allModes!=nullptr && U_SUCCESS(errorCode)) {
        নির্বাচন(mode) {
        ক্ষেত্র UNORM2_COMPOSE:
            ফেরত &allModes->comp;
        ক্ষেত্র UNORM2_DECOMPOSE:
            ফেরত &allModes->decomp;
        ক্ষেত্র UNORM2_FCD:
            ফেরত &allModes->fcd;
        ক্ষেত্র UNORM2_COMPOSE_CONTIGUOUS:
            ফেরত &allModes->fcc;
        পূর্বনির্ধারিত:
            বিরতি;  // do nothing
        }
    }
    ফেরত nullptr;
}

ধ্রুবক Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        ফেরত nullptr;
    }
    নির্বাচন(mode) {
    ক্ষেত্র UNORM_NFD:
        ফেরত Normalizer2::getNFDInstance(errorCode);
    ক্ষেত্র UNORM_NFKD:
        ফেরত Normalizer2::getNFKDInstance(errorCode);
    ক্ষেত্র UNORM_NFC:
        ফেরত Normalizer2::getNFCInstance(errorCode);
    ক্ষেত্র UNORM_NFKC:
        ফেরত Normalizer2::getNFKCInstance(errorCode);
    ক্ষেত্র UNORM_FCD:
        ফেরত getFCDInstance(errorCode);
    পূর্বনির্ধারিত:  // UNORM_NONE
        ফেরত getNoopInstance(errorCode);
    }
}

ধ্রুবক Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ফেরত allModes!=nullptr ? allModes->impl : nullptr;
}

ধ্রুবক Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ধ্রুবক Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ফেরত allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ধ্রুবক UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ফেরত (ধ্রুবক UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ধ্রুবক UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ফেরত (ধ্রুবক UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ধ্রুবক UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ফেরত (ধ্রুবক UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ধ্রুবক UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ফেরত (ধ্রুবক UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ধ্রুবক UNormalizer2 * U_EXPORT2
unorm2_getInstance(ধ্রুবক অক্ষর *packageName,
                   ধ্রুবক অক্ষর *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ফেরত (ধ্রুবক UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalization_Check_ফলাফল
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    যদি(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ফেরত UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুবক Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    যদি(U_SUCCESS(errorCode)) {
        ফেরত ((ধ্রুবক Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } অন্যথা {
        ফেরত UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
