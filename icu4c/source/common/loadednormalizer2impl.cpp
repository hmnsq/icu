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

#если !UCONFIG_NO_NORMALIZATION

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
    если(
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
        вернуть true;
    } иначе {
        вернуть false;
    }
}

void
LoadedNormalizer2Impl::load(const char *packageName, const char *name, UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть;
    }
    memory=udata_openChoice(packageName, "nrm", name, isAcceptable, this, &errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть;
    }
    const uint8_t* inBytes = static_cast<const uint8_t*>(udata_getMemory(memory));
    const int32_t* inIndexes = reinterpret_cast<const int32_t*>(inBytes);
    int32_t indexesLength=inIndexes[IX_NORM_TRIE_OFFSET]/4;
    если(indexesLength<=IX_MIN_LCCC_CP) {
        errorCode=U_INVALID_FORMAT_ERROR;  // Not enough indexes.
        вернуть;
    }

    int32_t offset=inIndexes[IX_NORM_TRIE_OFFSET];
    int32_t nextOffset=inIndexes[IX_EXTRA_DATA_OFFSET];
    ownedTrie=ucptrie_openFromBinary(UCPTRIE_TYPE_FAST, UCPTRIE_VALUE_BITS_16,
                                     inBytes+offset, nextOffset-offset, nullptr,
                                     &errorCode);
    если(U_FAILURE(errorCode)) {
        вернуть;
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
    если(U_FAILURE(errorCode)) {
        вернуть nullptr;
    }
    LoadedNormalizer2Impl *impl=new LoadedNormalizer2Impl;
    если(impl==nullptr) {
        errorCode=U_MEMORY_ALLOCATION_ERROR;
        вернуть nullptr;
    }
    impl->load(packageName, name, errorCode);
    вернуть createInstance(impl, errorCode);
}

U_CDECL_BEGIN
static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup();
U_CDECL_END

#если !NORM2_HARDCODE_NFC_DATA
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
#если !NORM2_HARDCODE_NFC_DATA
    если (uprv_strcmp(what, "nfc") == 0) {
        nfcSingleton    = Norm2AllModes::createInstance(nullptr, "nfc", errorCode);
    } иначе
#endif
    если (uprv_strcmp(what, "nfkc") == 0) {
        nfkcSingleton    = Norm2AllModes::createInstance(nullptr, "nfkc", errorCode);
    } иначе если (uprv_strcmp(what, "nfkc_cf") == 0) {
        nfkc_cfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_cf", errorCode);
    } иначе если (uprv_strcmp(what, "nfkc_scf") == 0) {
        nfkc_scfSingleton = Norm2AllModes::createInstance(nullptr, "nfkc_scf", errorCode);
    } иначе {
        UPRV_UNREACHABLE_EXIT;   // Unknown singleton
    }
    ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
}

U_CDECL_BEGIN

static void U_CALLCONV deleteNorm2AllModes(void *allModes) {
    delete (Norm2AllModes *)allModes;
}

static UBool U_CALLCONV uprv_loaded_normalizer2_cleanup() {
#если !NORM2_HARDCODE_NFC_DATA
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
    вернуть true;
}

U_CDECL_END

#если !NORM2_HARDCODE_NFC_DATA
const Norm2AllModes *
Norm2AllModes::getNFCInstance(UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) { вернуть nullptr; }
    umtx_initOnce(nfcInitOnce, &initSingletons, "nfc", errorCode);
    вернуть nfcSingleton;
}
#endif

const Norm2AllModes *
Norm2AllModes::getNFKCInstance(UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) { вернуть nullptr; }
    umtx_initOnce(nfkcInitOnce, &initSingletons, "nfkc", errorCode);
    вернуть nfkcSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_CFInstance(UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) { вернуть nullptr; }
    umtx_initOnce(nfkc_cfInitOnce, &initSingletons, "nfkc_cf", errorCode);
    вернуть nfkc_cfSingleton;
}

const Norm2AllModes *
Norm2AllModes::getNFKC_SCFInstance(UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) { вернуть nullptr; }
    umtx_initOnce(nfkc_scfInitOnce, &initSingletons, "nfkc_scf", errorCode);
    вернуть nfkc_scfSingleton;
}

#если !NORM2_HARDCODE_NFC_DATA
const Normalizer2 *
Normalizer2::getNFCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->fcd : nullptr;
}

const Normalizer2 *Normalizer2Factory::getFCCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->fcc : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFCInstance(errorCode);
    вернуть allModes!=nullptr ? allModes->impl : nullptr;
}
#endif

const Normalizer2 *
Normalizer2::getNFKCInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKDInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->decomp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getNFKCSimpleCasefoldInstance(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
    вернуть allModes!=nullptr ? &allModes->comp : nullptr;
}

const Normalizer2 *
Normalizer2::getInstance(const char *packageName,
                         const char *name,
                         UNormalization2Mode mode,
                         UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть nullptr;
    }
    если(name==nullptr || *name==0) {
        errorCode=U_ILLEGAL_ARGUMENT_ERROR;
        вернуть nullptr;
    }
    const Norm2AllModes *allModes=nullptr;
    если(packageName==nullptr) {
        если(0==uprv_strcmp(name, "nfc")) {
            allModes=Norm2AllModes::getNFCInstance(errorCode);
        } иначе если(0==uprv_strcmp(name, "nfkc")) {
            allModes=Norm2AllModes::getNFKCInstance(errorCode);
        } иначе если(0==uprv_strcmp(name, "nfkc_cf")) {
            allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
        } иначе если(0==uprv_strcmp(name, "nfkc_scf")) {
            allModes=Norm2AllModes::getNFKC_SCFInstance(errorCode);
        }
    }
    если(allModes==nullptr && U_SUCCESS(errorCode)) {
        {
            Mutex lock;
            если(cache!=nullptr) {
                allModes = static_cast<Norm2AllModes*>(uhash_get(cache, name));
            }
        }
        если(allModes==nullptr) {
            ucln_common_registerCleanup(UCLN_COMMON_LOADED_NORMALIZER2, uprv_loaded_normalizer2_cleanup);
            LocalPointer<Norm2AllModes> localAllModes(
                Norm2AllModes::createInstance(packageName, name, errorCode));
            если(U_SUCCESS(errorCode)) {
                Mutex lock;
                если(cache==nullptr) {
                    cache=uhash_open(uhash_hashChars, uhash_compareChars, nullptr, &errorCode);
                    если(U_FAILURE(errorCode)) {
                        вернуть nullptr;
                    }
                    uhash_setKeyDeleter(cache, uprv_free);
                    uhash_setValueDeleter(cache, deleteNorm2AllModes);
                }
                void *temp=uhash_get(cache, name);
                если(temp==nullptr) {
                    int32_t keyLength= static_cast<int32_t>(uprv_strlen(name)+1);
                    char* nameCopy = static_cast<char*>(uprv_malloc(keyLength));
                    если(nameCopy==nullptr) {
                        errorCode=U_MEMORY_ALLOCATION_ERROR;
                        вернуть nullptr;
                    }
                    uprv_memcpy(nameCopy, name, keyLength);
                    allModes=localAllModes.getAlias();
                    uhash_put(cache, nameCopy, localAllModes.orphan(), &errorCode);
                } иначе {
                    // race condition
                    allModes = static_cast<Norm2AllModes*>(temp);
                }
            }
        }
    }
    если(allModes!=nullptr && U_SUCCESS(errorCode)) {
        switch(mode) {
        case UNORM2_COMPOSE:
            вернуть &allModes->comp;
        case UNORM2_DECOMPOSE:
            вернуть &allModes->decomp;
        case UNORM2_FCD:
            вернуть &allModes->fcd;
        case UNORM2_COMPOSE_CONTIGUOUS:
            вернуть &allModes->fcc;
        default:
            break;  // do nothing
        }
    }
    вернуть nullptr;
}

const Normalizer2 *
Normalizer2Factory::getInstance(UNormalizationMode mode, UErrorCode &errorCode) {
    если(U_FAILURE(errorCode)) {
        вернуть nullptr;
    }
    switch(mode) {
    case UNORM_NFD:
        вернуть Normalizer2::getNFDInstance(errorCode);
    case UNORM_NFKD:
        вернуть Normalizer2::getNFKDInstance(errorCode);
    case UNORM_NFC:
        вернуть Normalizer2::getNFCInstance(errorCode);
    case UNORM_NFKC:
        вернуть Normalizer2::getNFKCInstance(errorCode);
    case UNORM_FCD:
        вернуть getFCDInstance(errorCode);
    default:  // UNORM_NONE
        вернуть getNoopInstance(errorCode);
    }
}

const Normalizer2Impl *
Normalizer2Factory::getNFKCImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKCInstance(errorCode);
    вернуть allModes!=nullptr ? allModes->impl : nullptr;
}

const Normalizer2Impl *
Normalizer2Factory::getNFKC_CFImpl(UErrorCode &errorCode) {
    const Norm2AllModes *allModes=Norm2AllModes::getNFKC_CFInstance(errorCode);
    вернуть allModes!=nullptr ? allModes->impl : nullptr;
}

U_NAMESPACE_END

// C API ------------------------------------------------------------------- ***

U_NAMESPACE_USE

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCInstance(UErrorCode *pErrorCode) {
    вернуть (const UNormalizer2 *)Normalizer2::getNFKCInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKDInstance(UErrorCode *pErrorCode) {
    вернуть (const UNormalizer2 *)Normalizer2::getNFKDInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCCasefoldInstance(UErrorCode *pErrorCode) {
    вернуть (const UNormalizer2 *)Normalizer2::getNFKCCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getNFKCSimpleCasefoldInstance(UErrorCode *pErrorCode) {
    вернуть (const UNormalizer2 *)Normalizer2::getNFKCSimpleCasefoldInstance(*pErrorCode);
}

U_CAPI const UNormalizer2 * U_EXPORT2
unorm2_getInstance(const char *packageName,
                   const char *name,
                   UNormalization2Mode mode,
                   UErrorCode *pErrorCode) {
    вернуть (const UNormalizer2 *)Normalizer2::getInstance(packageName, name, mode, *pErrorCode);
}

U_CFUNC UNormalizationCheckResult
unorm_getQuickCheck(UChar32 c, UNormalizationMode mode) {
    если(mode<=UNORM_NONE || UNORM_FCD<=mode) {
        вернуть UNORM_YES;
    }
    UErrorCode errorCode=U_ZERO_ERROR;
    const Normalizer2 *norm2=Normalizer2Factory::getInstance(mode, errorCode);
    если(U_SUCCESS(errorCode)) {
        вернуть ((const Normalizer2WithImpl *)norm2)->getQuickCheck(c);
    } иначе {
        вернуть UNORM_MAYBE;
    }
}

#endif  // !UCONFIG_NO_NORMALIZATION
