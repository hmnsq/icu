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

#ຖ້າ !UCONFIG_NO_NORMALIZATION

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
    ຖ້າ(
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
        ສົ່ງຄືນ true;
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ false;
    }
}

void
LoadedNormalizer2Impl::load(const var_na *packageName, const var_na *name, UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, this, &errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ;
    }
    const uint8_t* inBytes = static_cast<const uint8_t*>(udata_getMemory(memory));
    const int32_t* inIndexes = reinterpret_cast<const int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    ຖ້າ(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        ສົ່ງຄືນ;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ;
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
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ nullptr;
    }
    LoadedNormalizer2Impl *impl=new LoadedNormalizer2Impl;
    ຖ້າ(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    impl->load(packageName, name, errorCode);
    ສົ່ງຄືນ createInstance(impl, errorCode);
}

U_CDECL_BEGIN
static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#ຖ້າ !NORM2_HARDCODE_NFC_DATA
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
#ຖ້າ !NORM2_HARDCODE_NFC_DATA
    ຖ້າ (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } ບໍ່ດັ່ງນັ້ນ
#endif
    ຖ້າ (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } ບໍ່ດັ່ງນັ້ນ {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

static void U_CALLCONV deleteNorm2AllModes(void *allModes) {
    delete (Norm2AllModes *)allModes;
}

static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#ຖ້າ !NORM2_HARDCODE_NFC_DATA
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
    ສົ່ງຄືນ true;
}

U_CDECL_END

#ຖ້າ !NORM2_HARDCODE_NFC_DATA
const Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    ສົ່ງຄືນ nfcSingleton;
}
#endif

const Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    ສົ່ງຄືນ nfkcSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    ສົ່ງຄືນ nfkc_cfSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) { ສົ່ງຄືນ nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    ສົ່ງຄືນ nfkc_scfSingleton;
}

#ຖ້າ !NORM2_HARDCODE_NFC_DATA
const Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->fcd : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->fcc : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

const Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getInstance(const var_na *packageName,
                         const var_na *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ nullptr;
    }
    ຖ້າ(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ສົ່ງຄືນ nullptr;
    }
    const Norm2AllModes *allModes=nullptr;
    ຖ້າ(packageName==nullptr) {
        ຖ້າ(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    ຖ້າ(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            ຖ້າ(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        ຖ້າ(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            ຖ້າ(U_SUCCESS(errorCode)) {
                Mutex lock;
                ຖ້າ(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    ຖ້າ(U_FAILURE(errorCode)) {
                        ສົ່ງຄືນ nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                void *temp=uhash_get(cache, name);
                ຖ້າ(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    var_na* nameCopy = static_cast<var_na*>(uprv_malloc(keyLength));
                    ຖ້າ(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        ສົ່ງຄືນ nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } ບໍ່ດັ່ງນັ້ນ {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    ຖ້າ(allModes!=nullptr && U_SUCCESS(errorCode)) {
        switch(mode) {
        pak_sa UNORM2_COMPOSE:
            ສົ່ງຄືນ &allModes->comp;
        pak_sa UNORM2_DECOMPOSE:
            ສົ່ງຄືນ &allModes->decomp;
        pak_sa UNORM2_FCD:
            ສົ່ງຄືນ &allModes->fcd;
        pak_sa UNORM2_COMPOSE_CONTIGUOUS:
            ສົ່ງຄືນ &allModes->fcc;
        default:
            bha_nga;  // do nothing
        }
    }
    ສົ່ງຄືນ nullptr;
}

const Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    ຖ້າ(U_FAILURE(errorCode)) {
        ສົ່ງຄືນ nullptr;
    }
    switch(mode) {
    pak_sa UNORM_NFD:
        ສົ່ງຄືນ Normalizer2::getNFDInstance(errorCode);
    pak_sa UNORM_NFKD:
        ສົ່ງຄືນ Normalizer2::getNFKDInstance(errorCode);
    pak_sa UNORM_NFC:
        ສົ່ງຄືນ Normalizer2::getNFCInstance(errorCode);
    pak_sa UNORM_NFKC:
        ສົ່ງຄືນ Normalizer2::getNFKCInstance(errorCode);
    pak_sa UNORM_FCD:
        ສົ່ງຄືນ getFCDInstance(errorCode);
    default:  // UNORM_NONE
        ສົ່ງຄືນ getNoopInstance(errorCode);
    }
}

const Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? allModes->impl : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    ສົ່ງຄືນ allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    ສົ່ງຄືນ (const UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    ສົ່ງຄືນ (const UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    ສົ່ງຄືນ (const UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    ສົ່ງຄືນ (const UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getInstance(const var_na *packageName,
                   const var_na *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    ສົ່ງຄືນ (const UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    ຖ້າ(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        ສົ່ງຄືນ UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    ຖ້າ(U_SUCCESS(errorCode)) {
        ສົ່ງຄືນ ((const Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } ບໍ່ດັ່ງນັ້ນ {
        ສົ່ງຄືນ UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
