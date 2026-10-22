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

#만약 !UCONFIG_NO_NORMALIZATION

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

    void load(const char *packageName, const char *name, UErrorCode &errorCode);

private:
    static UBool U_CALLCONV
    isAcceptable(void *context, const char *type, const char *name, const UDataInfo *pInfo);

    UDataMemory *memory;
    UCPTrie *ownedTrie;
};

LoadedNormalizer2Impl::~LoadedNormalizer2Impl() {
    udata_close(memory);
    ucptrie_close(ownedTrie);
}

UBool U_CALLCONV
LoadedNormalizer2Impl::isAcceptable(void * /*context*/,
                                    const char * /* type */, const char * /*name*/,
                                    const UDataInfo *pInfo) {
    만약(
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
        반환 true;
    } 아니면 {
        반환 false;
    }
}

void
LoadedNormalizer2Impl::load(const char *packageName, const char *name, UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) {
        반환;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, this, &errorCode);
    만약(U_FAILURE(errorCode)) {
        반환;
    }
    const uint8_t* inBytes = static_cast<const uint8_t*>(udata_getMemory(memory));
    const int32_t* inIndexes = reinterpret_cast<const int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    만약(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        반환;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    만약(U_FAILURE(errorCode)) {
        반환;
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
Norm2AllModes::createInstance(const char *packageName,
                              const char *name,
                              UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) {
        반환 nullptr;
    }
    LoadedNormalizer2Impl *impl=new LoadedNormalizer2Impl;
    만약(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        반환 nullptr;
    }
    impl->load(packageName, name, errorCode);
    반환 createInstance(impl, errorCode);
}

U_CDECL_BEGIN
static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#만약 !NORM2_HARDCODE_NFC_DATA
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
static void U_CALLCONV initSingletons(const char *what, UErrorCode &errorCode) {
#만약 !NORM2_HARDCODE_NFC_DATA
    만약 (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } 아니면
#endif
    만약 (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } 아니면 만약 (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } 아니면 만약 (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } 아니면 {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

static void U_CALLCONV deleteNorm2AllModes(void *allModes) {
    delete (Norm2AllModes *)allModes;
}

static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#만약 !NORM2_HARDCODE_NFC_DATA
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
    반환 true;
}

U_CDECL_END

#만약 !NORM2_HARDCODE_NFC_DATA
const Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) { 반환 nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    반환 nfcSingleton;
}
#endif

const Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) { 반환 nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    반환 nfkcSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) { 반환 nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    반환 nfkc_cfSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) { 반환 nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    반환 nfkc_scfSingleton;
}

#만약 !NORM2_HARDCODE_NFC_DATA
const Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->fcd : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->fcc : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    반환 allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

const Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    반환 allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getInstance(const char *packageName,
                         const char *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) {
        반환 nullptr;
    }
    만약(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        반환 nullptr;
    }
    const Norm2AllModes *allModes=nullptr;
    만약(packageName==nullptr) {
        만약(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } 아니면 만약(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } 아니면 만약(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } 아니면 만약(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    만약(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            만약(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        만약(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            만약(U_SUCCESS(errorCode)) {
                Mutex lock;
                만약(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    만약(U_FAILURE(errorCode)) {
                        반환 nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                void *temp=uhash_get(cache, name);
                만약(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    char* nameCopy = static_cast<char*>(uprv_malloc(keyLength));
                    만약(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        반환 nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } 아니면 {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    만약(allModes!=nullptr && U_SUCCESS(errorCode)) {
        switch(mode) {
        case UNORM2_COMPOSE:
            반환 &allModes->comp;
        case UNORM2_DECOMPOSE:
            반환 &allModes->decomp;
        case UNORM2_FCD:
            반환 &allModes->fcd;
        case UNORM2_COMPOSE_CONTIGUOUS:
            반환 &allModes->fcc;
        default:
            break;  // do nothing
        }
    }
    반환 nullptr;
}

const Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    만약(U_FAILURE(errorCode)) {
        반환 nullptr;
    }
    switch(mode) {
    case UNORM_NFD:
        반환 Normalizer2::getNFDInstance(errorCode);
    case UNORM_NFKD:
        반환 Normalizer2::getNFKDInstance(errorCode);
    case UNORM_NFC:
        반환 Normalizer2::getNFCInstance(errorCode);
    case UNORM_NFKC:
        반환 Normalizer2::getNFKCInstance(errorCode);
    case UNORM_FCD:
        반환 getFCDInstance(errorCode);
    default:  // UNORM_NONE
        반환 getNoopInstance(errorCode);
    }
}

const Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    반환 allModes!=nullptr ? allModes->impl : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    반환 allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    반환 (const UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    반환 (const UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    반환 (const UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    반환 (const UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getInstance(const char *packageName,
                   const char *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    반환 (const UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    만약(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        반환 UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    만약(U_SUCCESS(errorCode)) {
        반환 ((const Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } 아니면 {
        반환 UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
