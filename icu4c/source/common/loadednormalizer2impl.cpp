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

#include "unicode/utypes.h"

#ᬬᬾᬦ᭄ !UCONFIG_NO_NORMALIZATION

#include "unicode/udata.h"
#include "unicode/localpointer.h"
#include "unicode/normalizer2.h"
#include "unicode/ucptrie.h"
#include "unicode/unistr.h"
#include "unicode/unorm.h"
#include "cstring.h"
#include "mutex.h"
#include "norm2allmodes.h"
#include "normalizer2impl.h"
#include "uassert.h"
#include "ucln_cmn.h"
#include "uhash.h"

U_NAMESPACE_BEGIN

class LoadedNormalizer2Impl : public Normalizer2Impl {
public:
    LoadedNormalizer2Impl() : memory(nullptr), ownedTrie(nullptr) {}
    virtual ~LoadedNormalizer2Impl();

    void load(const var_na *packageName, const var_na *name, UErrorCode &errorCode);

private:
    static UBool U_CALLCONV
    isAcceptable(void *context, const var_na *type, const var_na *name, const UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(void * /*context*/,
                                    const var_na * /* type */, const var_na * /*name*/,
                                    const UDataInfo *pInfo) {
    ᬬᬾᬦ᭄(
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
        ᬯᬮᬶ true;
    } ᬮᬶᬬ {
        ᬯᬮᬶ false;
    }
}

void
LoadedNormalizer2Impl::load(const var_na *packageName, const var_na *name, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, this, &errorCode);
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }
    const uint8_t* inBytes = static_cast<const uint8_t*>(udata_getMemory(memory));
    const int32_t* inIndexes = reinterpret_cast<const int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    ᬬᬾᬦ᭄(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ᬯᬮᬶ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ;
    }

    offset=nextOffset;
    nextOffset=inIndexes[IX_SMALL_FCD_OFFSET];
    const uint16_t* inExtraData = reinterpret_cast<const uint16_t*>(inBytes + offset);

    // smallFCD: new in formatVersion 2
    offset=nextOffset;
    const uint8_t *inSmallFCD=inBytes+offset;

    init(inIndexes, ownedTrie, inExtraData, inSmallFCD);
}

// instance cache ---------------------------------------------------------- ***

Norm2AllModes *
Norm2AllModes::createInstance(const var_na *packageName,
                              const var_na *name,
                              UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ nullptr;
    }
    LoadedNormalizer2Impl *impl=new LoadedNormalizer2Impl;
    ᬬᬾᬦ᭄(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ nullptr;
    }
    impl->load(packageName, name, errorCode);
    ᬯᬮᬶ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#ᬬᬾᬦ᭄ !NORM2_HARDCODE_NFC_DATA
static Norm2AllModes *nfcSingleton;
static icu::UInitOnce nfcInitOnce {};
#endif

static Norm2AllModes *nfkcSingleton;
static icu::UInitOnce nfkcInitOnce {};

static Norm2AllModes *nfkc_cfSingleton;
static icu::UInitOnce nfkc_cfInitOnce {};

static Norm2AllModes *nfkc_scfSingleton;
static icu::UInitOnce nfkc_scfInitOnce {};

static UHashtable    *cache=nullptr;

// UInitOnce singleton initialization function
static void U_CALLCONV initSingletons(const var_na *what, UErrorCode &errorCode) {
#ᬬᬾᬦ᭄ !NORM2_HARDCODE_NFC_DATA
    ᬬᬾᬦ᭄ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } ᬮᬶᬬ
#endif
    ᬬᬾᬦ᭄ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } ᬮᬶᬬ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

static void U_CALLCONV deleteNorm2AllModes(void *allModes) {
    delete (Norm2AllModes *)allModes;
}

static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ᬬᬾᬦ᭄ !NORM2_HARDCODE_NFC_DATA
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
    ᬯᬮᬶ true;
}

U_CDECL_END

#ᬬᬾᬦ᭄ !NORM2_HARDCODE_NFC_DATA
const Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ᬯᬮᬶ nfcSingleton;
}
#endif

const Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ᬯᬮᬶ nfkcSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ᬯᬮᬶ nfkc_cfSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) { ᬯᬮᬶ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ᬯᬮᬶ nfkc_scfSingleton;
}

#ᬬᬾᬦ᭄ !NORM2_HARDCODE_NFC_DATA
const Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->fcd : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->fcc : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

const Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getInstance(const var_na *packageName,
                         const var_na *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ nullptr;
    }
    ᬬᬾᬦ᭄(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᬯᬮᬶ nullptr;
    }
    const Norm2AllModes *allModes=nullptr;
    ᬬᬾᬦ᭄(packageName==nullptr) {
        ᬬᬾᬦ᭄(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } ᬮᬶᬬ ᬬᬾᬦ᭄(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    ᬬᬾᬦ᭄(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            ᬬᬾᬦ᭄(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ᬬᬾᬦ᭄(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            ᬬᬾᬦ᭄(U_SUCCESS(errorCode)) {
                Mutex lock;
                ᬬᬾᬦ᭄(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
                        ᬯᬮᬶ nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                void *temp=uhash_get(cache, name);
                ᬬᬾᬦ᭄(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    var_na* nameCopy = static_cast<var_na*>(uprv_malloc(keyLength));
                    ᬬᬾᬦ᭄(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ᬯᬮᬶ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } ᬮᬶᬬ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    ᬬᬾᬦ᭄(allModes!=nullptr && U_SUCCESS(errorCode)) {
        switch(mode) {
        pak_sa UNORM2_COMPOSE:
            ᬯᬮᬶ &allModes->comp;
        pak_sa UNORM2_DECOMPOSE:
            ᬯᬮᬶ &allModes->decomp;
        pak_sa UNORM2_FCD:
            ᬯᬮᬶ &allModes->fcd;
        pak_sa UNORM2_COMPOSE_CONTIGUOUS:
            ᬯᬮᬶ &allModes->fcc;
        default:
            bha_nga;  // do nothing
        }
    }
    ᬯᬮᬶ nullptr;
}

const Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    ᬬᬾᬦ᭄(U_FAILURE(errorCode)) {
        ᬯᬮᬶ nullptr;
    }
    switch(mode) {
    pak_sa UNORM_NFD:
        ᬯᬮᬶ Normalizer2::getNFDInstance(errorCode);
    pak_sa UNORM_NFKD:
        ᬯᬮᬶ Normalizer2::getNFKDInstance(errorCode);
    pak_sa UNORM_NFC:
        ᬯᬮᬶ Normalizer2::getNFCInstance(errorCode);
    pak_sa UNORM_NFKC:
        ᬯᬮᬶ Normalizer2::getNFKCInstance(errorCode);
    pak_sa UNORM_FCD:
        ᬯᬮᬶ getFCDInstance(errorCode);
    default:  // UNORM_NONE
        ᬯᬮᬶ getNoopInstance(errorCode);
    }
}

const Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? allModes->impl : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ᬯᬮᬶ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ᬯᬮᬶ (const UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ᬯᬮᬶ (const UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ᬯᬮᬶ (const UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ᬯᬮᬶ (const UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getInstance(const var_na *packageName,
                   const var_na *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ᬯᬮᬶ (const UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    ᬬᬾᬦ᭄(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ᬯᬮᬶ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    ᬬᬾᬦ᭄(U_SUCCESS(errorCode)) {
        ᬯᬮᬶ ((const Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } ᬮᬶᬬ {
        ᬯᬮᬶ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
