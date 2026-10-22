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

    शून्य load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UErrorCode &errorCode);

गुप्त:
    स्थिर UBool U_CALLCONV
    isAcceptable(शून्य *context, ध्रुव वर्ण *type, ध्रुव वर्ण *name, ध्रुव UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(शून्य * /*context*/,
                                    ध्रुव वर्ण * /* type */, ध्रुव वर्ण * /*name*/,
                                    ध्रुव UDataInfo *pInfo) {
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
        प्रत्यागम सत्य;
    } अन्यथा {
        प्रत्यागम असत्य;
    }
}

शून्य
LoadedNormalizer2Impl::load(ध्रुव वर्ण *packageName, ध्रुव वर्ण *name, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, एतत्, &errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }
    ध्रुव uint8_t* inBytes = static_cast<ध्रुव uint8_t*>(udata_getMemory(memory));
    ध्रुव int32_t* inIndexes = reinterpret_cast<ध्रुव int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    यदि(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        प्रत्यागम;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_द्विमान(UCPTRIE_TYPE_FAST, UCPTRIE_मूल्य_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    ध्रुव uint16_t* inExtraData = reinterpret_cast<ध्रुव uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    ध्रुव uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(ध्रुव वर्ण *packageName,
                              ध्रुव वर्ण *name,
                              UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    LoadedNormalizer2Impl *impl=नूतन LoadedNormalizer2Impl;
    यदि(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम nullptr;
    }
    impl->load(packageName, name, errorCode);
    प्रत्यागम createInstance(impl, errorCode);
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
स्थिर शून्य U_CALLCONV initSingletons(ध्रुव वर्ण *what, UErrorCode &errorCode) {
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
    प्रत्यागम सत्य;
}

U_CDECL_END

#यदि !NORM2_HARDCODE_NFC_DATA
ध्रुव Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    प्रत्यागम nfcSingleton;
}
#endif

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    प्रत्यागम nfkcSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    प्रत्यागम nfkc_cfSingleton;
}

ध्रुव Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) { प्रत्यागम nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    प्रत्यागम nfkc_scfSingleton;
}

#यदि !NORM2_HARDCODE_NFC_DATA
ध्रुव Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->decomp : nullptr;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->fcd : nullptr;
}

ध्रुव Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->fcc : nullptr;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

ध्रुव Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->decomp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? &allModes->comp : nullptr;
}

ध्रुव Normalizer2 *
Normalizer2::getInstance(ध्रुव वर्ण *packageName,
                         ध्रुव वर्ण *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    यदि(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम nullptr;
    }
    ध्रुव Norm2AllModes *allModes=nullptr;
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
                        प्रत्यागम nullptr;
                    }
                    uhash_set_किल्ली_Deleter(cache, uprv_free);
                    uhash_set_मूल्य_Deleter(cache, deleteNorm2AllModes);
                }
                शून्य *temp=uhash_get(cache, name);
                यदि(temp==nullptr) {
                    int32_t किल्ली_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    वर्ण* nameCopy = static_cast<वर्ण*>(uprv_malloc(किल्ली_Length));
                    यदि(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        प्रत्यागम nullptr;
                    }
                    uprv_memcpy(nameCopy, name, किल्ली_Length);
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
            प्रत्यागम &allModes->comp;
        पक्ष UNORM2_DECOMPOSE:
            प्रत्यागम &allModes->decomp;
        पक्ष UNORM2_FCD:
            प्रत्यागम &allModes->fcd;
        पक्ष UNORM2_COMPOSE_CONTIGUOUS:
            प्रत्यागम &allModes->fcc;
        स्वभाव:
            भङ्ग;  // do nothing
        }
    }
    प्रत्यागम nullptr;
}

ध्रुव Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    यदि(U_FAILURE(errorCode)) {
        प्रत्यागम nullptr;
    }
    विकल्प(mode) {
    पक्ष UNORM_NFD:
        प्रत्यागम Normalizer2::getNFDInstance(errorCode);
    पक्ष UNORM_NFKD:
        प्रत्यागम Normalizer2::getNFKDInstance(errorCode);
    पक्ष UNORM_NFC:
        प्रत्यागम Normalizer2::getNFCInstance(errorCode);
    पक्ष UNORM_NFKC:
        प्रत्यागम Normalizer2::getNFKCInstance(errorCode);
    पक्ष UNORM_FCD:
        प्रत्यागम getFCDInstance(errorCode);
    स्वभाव:  // UNORM_NONE
        प्रत्यागम getNoopInstance(errorCode);
    }
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? allModes->impl : nullptr;
}

ध्रुव Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    ध्रुव Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    प्रत्यागम allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI ध्रुव UNormalizer2 * U_EXPORT2
unorm2_getInstance(ध्रुव वर्ण *packageName,
                   ध्रुव वर्ण *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    प्रत्यागम (ध्रुव UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    यदि(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        प्रत्यागम UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    ध्रुव Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    यदि(U_SUCCESS(errorCode)) {
        प्रत्यागम ((ध्रुव Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } अन्यथा {
        प्रत्यागम UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
