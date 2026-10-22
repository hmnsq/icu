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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/localpointer.h"
#அந்தர்பாவ "unicode/normalizer2.h"
#அந்தர்பாவ "unicode/ucptrie.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/unorm.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "mutex.h"
#அந்தர்பாவ "norm2allmodes.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uhash.h"

U_NAMESPACE_BEGIN

வகுப்பு LoadedNormalizer2Impl : பொது Normalizer2Impl {
பொது:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    மெய்நிகர் ~LoadedNormalizer2Impl();

    வெறுமை load(மாறிலி எழுத்து *packageName, மாறிலி எழுத்து *name, UErrorCode &errorCode);

தனிப்பட்ட:
    நிலையான UBool U_CALLCONV
    isAcceptable(வெறுமை *context, மாறிலி எழுத்து *type, மாறிலி எழுத்து *name, மாறிலி UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(வெறுமை * /*context*/,
                                    மாறிலி எழுத்து * /* type */, மாறிலி எழுத்து * /*name*/,
                                    மாறிலி UDataInfo *pInfo) {
    எனில்(
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
        திரும்பு உண்மை;
    } இல்லையெனில் {
        திரும்பு பொய்;
    }
}

வெறுமை
LoadedNormalizer2Impl::load(மாறிலி எழுத்து *packageName, மாறிலி எழுத்து *name, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, இது, &errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }
    மாறிலி uint8_t* inBytes = static_cast<மாறிலி uint8_t*>(udata_getMemory(memory));
    மாறிலி int32_t* inIndexes = reinterpret_cast<மாறிலி int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    எனில்(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        திரும்பு;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_இருமம்(UCPTRIE_TYPE_FAST, UCPTRIE_மதிப்பு_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    மாறிலி uint16_t* inExtraData = reinterpret_cast<மாறிலி uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    மாறிலி uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(மாறிலி எழுத்து *packageName,
                              மாறிலி எழுத்து *name,
                              UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    LoadedNormalizer2Impl *impl=புதிய LoadedNormalizer2Impl;
    எனில்(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }
    impl->load(packageName, name, errorCode);
    திரும்பு createInstance(impl, errorCode);
}

U_CDECL_BEGIN
நிலையான UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#எனில் !NORM2_HARDCODE_NFC_DATA
நிலையான Norm2AllModes *nfcSingleton;
நிலையான icu::UInitOnce nfcInitOnce {};
#endif

நிலையான Norm2AllModes *nfkcSingleton;
நிலையான icu::UInitOnce nfkcInitOnce {};

நிலையான Norm2AllModes *nfkc_cfSingleton;
நிலையான icu::UInitOnce nfkc_cfInitOnce {};

நிலையான Norm2AllModes *nfkc_scfSingleton;
நிலையான icu::UInitOnce nfkc_scfInitOnce {};

நிலையான UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
நிலையான வெறுமை U_CALLCONV initSingletons(மாறிலி எழுத்து *what, UErrorCode &errorCode) {
#எனில் !NORM2_HARDCODE_NFC_DATA
    எனில் (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } இல்லையெனில்
#endif
    எனில் (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } இல்லையெனில் எனில் (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } இல்லையெனில் எனில் (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } இல்லையெனில் {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

நிலையான வெறுமை U_CALLCONV deleteNorm2AllModes(வெறுமை *allModes) {
    delete (Norm2AllModes *)allModes;
}

நிலையான UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#எனில் !NORM2_HARDCODE_NFC_DATA
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
    திரும்பு உண்மை;
}

U_CDECL_END

#எனில் !NORM2_HARDCODE_NFC_DATA
மாறிலி Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    திரும்பு nfcSingleton;
}
#endif

மாறிலி Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    திரும்பு nfkcSingleton;
}

மாறிலி Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    திரும்பு nfkc_cfSingleton;
}

மாறிலி Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) { திரும்பு nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    திரும்பு nfkc_scfSingleton;
}

#எனில் !NORM2_HARDCODE_NFC_DATA
மாறிலி Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->comp : nullptr;
}

மாறிலி Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->decomp : nullptr;
}

மாறிலி Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->fcd : nullptr;
}

மாறிலி Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->fcc : nullptr;
}

மாறிலி Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    திரும்பு allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

மாறிலி Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->comp : nullptr;
}

மாறிலி Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->decomp : nullptr;
}

மாறிலி Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->comp : nullptr;
}

மாறிலி Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    திரும்பு allModes!=nullptr ? &allModes->comp : nullptr;
}

மாறிலி Normalizer2 *
Normalizer2::getInstance(மாறிலி எழுத்து *packageName,
                         மாறிலி எழுத்து *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    எனில்(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு nullptr;
    }
    மாறிலி Norm2AllModes *allModes=nullptr;
    எனில்(packageName==nullptr) {
        எனில்(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } இல்லையெனில் எனில்(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } இல்லையெனில் எனில்(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } இல்லையெனில் எனில்(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    எனில்(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            எனில்(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        எனில்(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            எனில்(U_SUCCESS(errorCode)) {
                Mutex lock;
                எனில்(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    எனில்(U_FAILURE(errorCode)) {
                        திரும்பு nullptr;
                    }
                    uhash_set_சாவி_Deleter(cache, uprv_free);
                    uhash_set_மதிப்பு_Deleter(cache, deleteNorm2AllModes);
                }
                வெறுமை *temp=uhash_get(cache, name);
                எனில்(temp==nullptr) {
                    int32_t சாவி_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    எழுத்து* nameCopy = static_cast<எழுத்து*>(uprv_malloc(சாவி_Length));
                    எனில்(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        திரும்பு nullptr;
                    }
                    uprv_memcpy(nameCopy, name, சாவி_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } இல்லையெனில் {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    எனில்(allModes!=nullptr && U_SUCCESS(errorCode)) {
        தேர்வு(mode) {
        நிலை UNORM2_COMPOSE:
            திரும்பு &allModes->comp;
        நிலை UNORM2_DECOMPOSE:
            திரும்பு &allModes->decomp;
        நிலை UNORM2_FCD:
            திரும்பு &allModes->fcd;
        நிலை UNORM2_COMPOSE_CONTIGUOUS:
            திரும்பு &allModes->fcc;
        இயல்பு:
            நிறுத்து;  // do nothing
        }
    }
    திரும்பு nullptr;
}

மாறிலி Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    எனில்(U_FAILURE(errorCode)) {
        திரும்பு nullptr;
    }
    தேர்வு(mode) {
    நிலை UNORM_NFD:
        திரும்பு Normalizer2::getNFDInstance(errorCode);
    நிலை UNORM_NFKD:
        திரும்பு Normalizer2::getNFKDInstance(errorCode);
    நிலை UNORM_NFC:
        திரும்பு Normalizer2::getNFCInstance(errorCode);
    நிலை UNORM_NFKC:
        திரும்பு Normalizer2::getNFKCInstance(errorCode);
    நிலை UNORM_FCD:
        திரும்பு getFCDInstance(errorCode);
    இயல்பு:  // UNORM_NONE
        திரும்பு getNoopInstance(errorCode);
    }
}

மாறிலி Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    திரும்பு allModes!=nullptr ? allModes->impl : nullptr;
}

மாறிலி Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    மாறிலி Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    திரும்பு allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI மாறிலி UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    திரும்பு (மாறிலி UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI மாறிலி UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    திரும்பு (மாறிலி UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI மாறிலி UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    திரும்பு (மாறிலி UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI மாறிலி UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    திரும்பு (மாறிலி UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI மாறிலி UNormalizer2 * U_EXPORT2
unorm2_getInstance(மாறிலி எழுத்து *packageName,
                   மாறிலி எழுத்து *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    திரும்பு (மாறிலி UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    எனில்(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        திரும்பு UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    மாறிலி Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    எனில்(U_SUCCESS(errorCode)) {
        திரும்பு ((மாறிலி Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } இல்லையெனில் {
        திரும்பு UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
