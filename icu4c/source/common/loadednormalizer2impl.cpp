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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/localpointer.h"
#અન્તર્ભાવ "unicode/normalizer2.h"
#અન્તર્ભાવ "unicode/ucptrie.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/unorm.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "mutex.h"
#અન્તર્ભાવ "norm2allmodes.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uhash.h"

U_NAMESPACE_BEGIN

વર્ગ LoadedNormalizer2Impl : જાહેર Normalizer2Impl {
જાહેર:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    આભાસી ~LoadedNormalizer2Impl();

    શૂન્ય load(અચલ અક્ષર *packageName, અચલ અક્ષર *name, UErrorCode &errorCode);

ખાનગી:
    સ્થિર UBool U_CALLCONV
    isAcceptable(શૂન્ય *context, અચલ અક્ષર *type, અચલ અક્ષર *name, અચલ UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(શૂન્ય * /*context*/,
                                    અચલ અક્ષર * /* type */, અચલ અક્ષર * /*name*/,
                                    અચલ UDataInfo *pInfo) {
    જો(
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
        પાછા સાચું;
    } નહીંતર {
        પાછા ખોટું;
    }
}

શૂન્ય
LoadedNormalizer2Impl::load(અચલ અક્ષર *packageName, અચલ અક્ષર *name, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, આ, &errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }
    અચલ uint8_t* inBytes = static_cast<અચલ uint8_t*>(udata_getMemory(memory));
    અચલ int32_t* inIndexes = reinterpret_cast<અચલ int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    જો(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        પાછા;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_open_From_દ્વિઅંકી(UCPTRIE_TYPE_FAST, UCPTRIE_કિંમત_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    જો(U_FAILURE(errorCode)) {
        પાછા;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    અચલ uint16_t* inExtraData = reinterpret_cast<અચલ uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    અચલ uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(અચલ અક્ષર *packageName,
                              અચલ અક્ષર *name,
                              UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    LoadedNormalizer2Impl *impl=નવું LoadedNormalizer2Impl;
    જો(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }
    impl->load(packageName, name, errorCode);
    પાછા createInstance(impl, errorCode);
}

U_CDECL_BEGIN
સ્થિર UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#જો !NORM2_HARDCODE_NFC_DATA
સ્થિર Norm2AllModes *nfcSingleton;
સ્થિર icu::UInitOnce nfcInitOnce {};
#endif

સ્થિર Norm2AllModes *nfkcSingleton;
સ્થિર icu::UInitOnce nfkcInitOnce {};

સ્થિર Norm2AllModes *nfkc_cfSingleton;
સ્થિર icu::UInitOnce nfkc_cfInitOnce {};

સ્થિર Norm2AllModes *nfkc_scfSingleton;
સ્થિર icu::UInitOnce nfkc_scfInitOnce {};

સ્થિર UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
સ્થિર શૂન્ય U_CALLCONV initSingletons(અચલ અક્ષર *what, UErrorCode &errorCode) {
#જો !NORM2_HARDCODE_NFC_DATA
    જો (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } નહીંતર
#endif
    જો (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } નહીંતર જો (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } નહીંતર જો (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } નહીંતર {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

સ્થિર શૂન્ય U_CALLCONV deleteNorm2AllModes(શૂન્ય *allModes) {
    delete (Norm2AllModes *)allModes;
}

સ્થિર UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#જો !NORM2_HARDCODE_NFC_DATA
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
    પાછા સાચું;
}

U_CDECL_END

#જો !NORM2_HARDCODE_NFC_DATA
અચલ Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    પાછા nfcSingleton;
}
#endif

અચલ Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    પાછા nfkcSingleton;
}

અચલ Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    પાછા nfkc_cfSingleton;
}

અચલ Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) { પાછા nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    પાછા nfkc_scfSingleton;
}

#જો !NORM2_HARDCODE_NFC_DATA
અચલ Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->comp : nullptr;
}

અચલ Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->decomp : nullptr;
}

અચલ Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->fcd : nullptr;
}

અચલ Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->fcc : nullptr;
}

અચલ Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    પાછા allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

અચલ Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->comp : nullptr;
}

અચલ Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->decomp : nullptr;
}

અચલ Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->comp : nullptr;
}

અચલ Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    પાછા allModes!=nullptr ? &allModes->comp : nullptr;
}

અચલ Normalizer2 *
Normalizer2::getInstance(અચલ અક્ષર *packageName,
                         અચલ અક્ષર *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    જો(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા nullptr;
    }
    અચલ Norm2AllModes *allModes=nullptr;
    જો(packageName==nullptr) {
        જો(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } નહીંતર જો(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } નહીંતર જો(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } નહીંતર જો(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    જો(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            જો(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        જો(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            જો(U_SUCCESS(errorCode)) {
                Mutex lock;
                જો(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    જો(U_FAILURE(errorCode)) {
                        પાછા nullptr;
                    }
                    uhash_set_ચાવી_Deleter(cache, uprv_free);
                    uhash_set_કિંમત_Deleter(cache, deleteNorm2AllModes);
                }
                શૂન્ય *temp=uhash_get(cache, name);
                જો(temp==nullptr) {
                    int32_t ચાવી_Length= static_cast<int32_t>(uprv_strlen(name)+1);
                    અક્ષર* nameCopy = static_cast<અક્ષર*>(uprv_malloc(ચાવી_Length));
                    જો(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        પાછા nullptr;
                    }
                    uprv_memcpy(nameCopy, name, ચાવી_Length);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } નહીંતર {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    જો(allModes!=nullptr && U_SUCCESS(errorCode)) {
        પસંદગી(mode) {
        સ્થિતિ UNORM2_COMPOSE:
            પાછા &allModes->comp;
        સ્થિતિ UNORM2_DECOMPOSE:
            પાછા &allModes->decomp;
        સ્થિતિ UNORM2_FCD:
            પાછા &allModes->fcd;
        સ્થિતિ UNORM2_COMPOSE_CONTIGUOUS:
            પાછા &allModes->fcc;
        મૂળભૂત:
            તોડો;  // do nothing
        }
    }
    પાછા nullptr;
}

અચલ Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    જો(U_FAILURE(errorCode)) {
        પાછા nullptr;
    }
    પસંદગી(mode) {
    સ્થિતિ UNORM_NFD:
        પાછા Normalizer2::getNFDInstance(errorCode);
    સ્થિતિ UNORM_NFKD:
        પાછા Normalizer2::getNFKDInstance(errorCode);
    સ્થિતિ UNORM_NFC:
        પાછા Normalizer2::getNFCInstance(errorCode);
    સ્થિતિ UNORM_NFKC:
        પાછા Normalizer2::getNFKCInstance(errorCode);
    સ્થિતિ UNORM_FCD:
        પાછા getFCDInstance(errorCode);
    મૂળભૂત:  // UNORM_NONE
        પાછા getNoopInstance(errorCode);
    }
}

અચલ Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    પાછા allModes!=nullptr ? allModes->impl : nullptr;
}

અચલ Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    અચલ Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    પાછા allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI અચલ UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    પાછા (અચલ UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI અચલ UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    પાછા (અચલ UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI અચલ UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    પાછા (અચલ UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI અચલ UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    પાછા (અચલ UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI અચલ UNormalizer2 * U_EXPORT2
unorm2_getInstance(અચલ અક્ષર *packageName,
                   અચલ અક્ષર *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    પાછા (અચલ UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    જો(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        પાછા UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    અચલ Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    જો(U_SUCCESS(errorCode)) {
        પાછા ((અચલ Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } નહીંતર {
        પાછા UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
