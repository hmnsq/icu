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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/normalizer2.h"
#अन्तर्भाव "unicode/ucptrie.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "mutex.h"
#अन्तर्भाव "norm2allmodes.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"

U_NAMESPACE_BEGIN

वर्ग LoadedNormalizer2Impl : सार्वजनिक Normalizer2Impl {
सार्वजनिक:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    आभास ~LoadedNormalizer2Impl();

    शून्य load(अचर वर्ण *packageName, अचर वर्ण *name, UErrorCode &errorCode);

गुप्त:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, अचर वर्ण *type, अचर वर्ण *name, अचर UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(शून्य * /*context*/,
                                    अचर वर्ण * /* type */, अचर वर्ण * /*name*/,
                                    अचर UDataInfo *pInfo) {
    यदि(
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
        फर्काऊ सत्य;
    } अन्यथा {
        फर्काऊ असत्य;
    }
}

शून्य
LoadedNormalizer2Impl::load(अचर वर्ण *packageName, अचर वर्ण *name, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, एतत्, &errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }
    अचर uint8_t* inBytes = static_cast<अचर uint8_t*>(udata_getMemory(memory));
    अचर int32_t* inIndexes = reinterpret_cast<अचर int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    यदि(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        फर्काऊ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_द्विआधारी(UCPTRIE_TYPE_FAST, UCPTRIE_मान_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    अचर uint16_t* inExtraData = reinterpret_cast<अचर uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    अचर uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(अचर वर्ण *packageName,
                              अचर वर्ण *name,
                              UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    LoadedNormalizer2Impl *impl=नूतन LoadedNormalizer2Impl;
    यदि(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        फर्काऊ nullptr;
    }
    impl->load(packageName, name, errorCode);
    फर्काऊ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#यदि !NORM2_HARDCODE_NFC_DATA
स्थिर Norm2AllModes *nfcSingleton;
स्थिर icu::UInitOnce nfcInitOnce {};
#endif

स्थिर Norm2AllModes *nfkcSingleton;
स्थिर icu::UInitOnce nfkcInitOnce {};

स्थिर Norm2AllModes *nfkc_cfSingleton;
स्थिर icu::UInitOnce nfkc_cfInitOnce {};

स्थिर Norm2AllModes *nfkc_scfSingleton;
स्थिर icu::UInitOnce nfkc_scfInitOnce {};

स्थिर UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
स्थिर शून्य U_CALLCONV initSingletons(अचर वर्ण *what, UErrorCode &errorCode) {
#यदि !NORM2_HARDCODE_NFC_DATA
    यदि (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } अन्यथा
#endif
    यदि (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } अन्यथा यदि (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } अन्यथा यदि (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } अन्यथा {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

स्थिर शून्य U_CALLCONV deleteNorm2AllModes(शून्य *allModes) {
    delete (Norm2AllModes *)allModes;
}

स्थिर UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#यदि !NORM2_HARDCODE_NFC_DATA
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
    फर्काऊ सत्य;
}

U_CDECL_END

#यदि !NORM2_HARDCODE_NFC_DATA
अचर Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    फर्काऊ nfcSingleton;
}
#endif

अचर Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    फर्काऊ nfkcSingleton;
}

अचर Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    फर्काऊ nfkc_cfSingleton;
}

अचर Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { फर्काऊ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    फर्काऊ nfkc_scfSingleton;
}

#यदि !NORM2_HARDCODE_NFC_DATA
अचर Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->comp : nullptr;
}

अचर Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->decomp : nullptr;
}

अचर Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->fcd : nullptr;
}

अचर Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->fcc : nullptr;
}

अचर Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

अचर Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->comp : nullptr;
}

अचर Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->decomp : nullptr;
}

अचर Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->comp : nullptr;
}

अचर Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    फर्काऊ allModes!=nullptr ? &allModes->comp : nullptr;
}

अचर Normalizer2 *
Normalizer2::getInstance(अचर वर्ण *packageName,
                         अचर वर्ण *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    यदि(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ nullptr;
    }
    अचर Norm2AllModes *allModes=nullptr;
    यदि(packageName==nullptr) {
        यदि(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } अन्यथा यदि(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    यदि(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            यदि(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        यदि(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            यदि(U_SUCCESS(errorCode)) {
                Mutex lock;
                यदि(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    यदि(U_FAILURE(errorCode)) {
                        फर्काऊ nullptr;
                    }
                    uhash_set_साँचो_Deleter(cache, uprv_free);
                    uhash_set_मान_Deleter(cache, deleteNorm2AllModes);
                }
                शून्य *temp=uhash_get(cache, name);
                यदि(temp==nullptr) {
                    int32_t साँचो_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    वर्ण* nameCopy = static_cast<वर्ण*>(uprv_malloc(साँचो_Length));
                    यदि(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        फर्काऊ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, साँचो_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } अन्यथा {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    यदि(allModes!=nullptr && U_SUCCESS(errorCode)) {
        विकल्प(mode) {
        पक्ष UNORM2_COMPOSE:
            फर्काऊ &allModes->comp;
        पक्ष UNORM2_DECOMPOSE:
            फर्काऊ &allModes->decomp;
        पक्ष UNORM2_FCD:
            फर्काऊ &allModes->fcd;
        पक्ष UNORM2_COMPOSE_CONTIGUOUS:
            फर्काऊ &allModes->fcc;
        स्वभाव:
            रोक;  // do nothing
        }
    }
    फर्काऊ nullptr;
}

अचर Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        फर्काऊ nullptr;
    }
    विकल्प(mode) {
    पक्ष UNORM_NFD:
        फर्काऊ Normalizer2::getNFDInstance(errorCode);
    पक्ष UNORM_NFKD:
        फर्काऊ Normalizer2::getNFKDInstance(errorCode);
    पक्ष UNORM_NFC:
        फर्काऊ Normalizer2::getNFCInstance(errorCode);
    पक्ष UNORM_NFKC:
        फर्काऊ Normalizer2::getNFKCInstance(errorCode);
    पक्ष UNORM_FCD:
        फर्काऊ getFCDInstance(errorCode);
    स्वभाव:  // UNORM_NONE
        फर्काऊ getNoopInstance(errorCode);
    }
}

अचर Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    फर्काऊ allModes!=nullptr ? allModes->impl : nullptr;
}

अचर Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    अचर Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    फर्काऊ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI अचर UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    फर्काऊ (अचर UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI अचर UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    फर्काऊ (अचर UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI अचर UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    फर्काऊ (अचर UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI अचर UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    फर्काऊ (अचर UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI अचर UNormalizer2 * U_EXPORT2
unorm2_getInstance(अचर वर्ण *packageName,
                   अचर वर्ण *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    फर्काऊ (अचर UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    यदि(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        फर्काऊ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    अचर Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    यदि(U_SUCCESS(errorCode)) {
        फर्काऊ ((अचर Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } अन्यथा {
        फर्काऊ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
