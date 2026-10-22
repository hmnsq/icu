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

বর্গ LoadedNormalizer2Impl : সার্বজনিক Normalizer2Impl {
সার্বজনিক:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    আভাস ~LoadedNormalizer2Impl();

    শূন্য load(ধ্রুব বর্ণ *packageName, ধ্রুব বর্ণ *name, UErrorCode &errorCode);

গুপ্ত:
    স্থির UBool U_CALLCONV
    isAcceptable(শূন্য *context, ধ্রুব বর্ণ *type, ধ্রুব বর্ণ *name, ধ্রুব UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(শূন্য * /*context*/,
                                    ধ্রুব বর্ণ * /* type */, ধ্রুব বর্ণ * /*name*/,
                                    ধ্রুব UDataInfo *pInfo) {
    যদি(
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
        প্রত্যাগম সত্য;
    } অন্যথা {
        প্রত্যাগম অসত্য;
    }
}

শূন্য
LoadedNormalizer2Impl::load(ধ্রুব বর্ণ *packageName, ধ্রুব বর্ণ *name, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, এতত্, &errorCode);
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }
    ধ্রুব uint8_t* inBytes = static_cast<ধ্রুব uint8_t*>(udata_getMemory(memory));
    ধ্রুব int32_t* inIndexes = reinterpret_cast<ধ্রুব int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    যদি(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        প্রত্যাগম;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_দ্বিমিক(UCPTRIE_TYPE_FAST, UCPTRIE_মান_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ধ্রুব uint16_t* inExtraData = reinterpret_cast<ধ্রুব uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ধ্রুব uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ধ্রুব বর্ণ *packageName,
                              ধ্রুব বর্ণ *name,
                              UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    LoadedNormalizer2Impl *impl=নূতন LoadedNormalizer2Impl;
    যদি(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম nullptr;
    }
    impl->load(packageName, name, errorCode);
    প্রত্যাগম createInstance(impl, errorCode);
}

U_CDECL_BEGIN
স্থির UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#যদি !NORM2_HARDCODE_NFC_DATA
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
স্থির শূন্য U_CALLCONV initSingletons(ধ্রুব বর্ণ *what, UErrorCode &errorCode) {
#যদি !NORM2_HARDCODE_NFC_DATA
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
#যদি !NORM2_HARDCODE_NFC_DATA
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
    প্রত্যাগম সত্য;
}

U_CDECL_END

#যদি !NORM2_HARDCODE_NFC_DATA
ধ্রুব Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    প্রত্যাগম nfcSingleton;
}
#endif

ধ্রুব Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    প্রত্যাগম nfkcSingleton;
}

ধ্রুব Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    প্রত্যাগম nfkc_cfSingleton;
}

ধ্রুব Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) { প্রত্যাগম nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    প্রত্যাগম nfkc_scfSingleton;
}

#যদি !NORM2_HARDCODE_NFC_DATA
ধ্রুব Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->decomp : nullptr;
}

ধ্রুব Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->fcd : nullptr;
}

ধ্রুব Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->fcc : nullptr;
}

ধ্রুব Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ধ্রুব Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->decomp : nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? &allModes->comp : nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2::getInstance(ধ্রুব বর্ণ *packageName,
                         ধ্রুব বর্ণ *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    যদি(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম nullptr;
    }
    ধ্রুব Norm2AllModes *allModes=nullptr;
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
                        প্রত্যাগম nullptr;
                    }
                    uhash_set_চাবি_Deleter(cache, uprv_free);
                    uhash_set_মান_Deleter(cache, deleteNorm2AllModes);
                }
                শূন্য *temp=uhash_get(cache, name);
                যদি(temp==nullptr) {
                    int32_t চাবি_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    বর্ণ* nameCopy = static_cast<বর্ণ*>(uprv_malloc(চাবি_Length));
                    যদি(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        প্রত্যাগম nullptr;
                    }
                    uprv_memcpy(nameCopy, name, চাবি_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } অন্যথা {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    যদি(allModes!=nullptr && U_SUCCESS(errorCode)) {
        বিকল্প(mode) {
        পক্ষ UNORM2_COMPOSE:
            প্রত্যাগম &allModes->comp;
        পক্ষ UNORM2_DECOMPOSE:
            প্রত্যাগম &allModes->decomp;
        পক্ষ UNORM2_FCD:
            প্রত্যাগম &allModes->fcd;
        পক্ষ UNORM2_COMPOSE_CONTIGUOUS:
            প্রত্যাগম &allModes->fcc;
        স্বভাব:
            ভঙ্গ;  // do nothing
        }
    }
    প্রত্যাগম nullptr;
}

ধ্রুব Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    যদি(U_FAILURE(errorCode)) {
        প্রত্যাগম nullptr;
    }
    বিকল্প(mode) {
    পক্ষ UNORM_NFD:
        প্রত্যাগম Normalizer2::getNFDInstance(errorCode);
    পক্ষ UNORM_NFKD:
        প্রত্যাগম Normalizer2::getNFKDInstance(errorCode);
    পক্ষ UNORM_NFC:
        প্রত্যাগম Normalizer2::getNFCInstance(errorCode);
    পক্ষ UNORM_NFKC:
        প্রত্যাগম Normalizer2::getNFKCInstance(errorCode);
    পক্ষ UNORM_FCD:
        প্রত্যাগম getFCDInstance(errorCode);
    স্বভাব:  // UNORM_NONE
        প্রত্যাগম getNoopInstance(errorCode);
    }
}

ধ্রুব Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? allModes->impl : nullptr;
}

ধ্রুব Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ধ্রুব Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    প্রত্যাগম allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ধ্রুব UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    প্রত্যাগম (ধ্রুব UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ধ্রুব UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    প্রত্যাগম (ধ্রুব UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ধ্রুব UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    প্রত্যাগম (ধ্রুব UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ধ্রুব UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    প্রত্যাগম (ধ্রুব UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ধ্রুব UNormalizer2 * U_EXPORT2
unorm2_getInstance(ধ্রুব বর্ণ *packageName,
                   ধ্রুব বর্ণ *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    প্রত্যাগম (ধ্রুব UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    যদি(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        প্রত্যাগম UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ধ্রুব Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    যদি(U_SUCCESS(errorCode)) {
        প্রত্যাগম ((ধ্রুব Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } অন্যথা {
        প্রত্যাগম UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
