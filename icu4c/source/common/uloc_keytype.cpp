// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#include <algorithm>
#include <optional>
#include <string_view>

#include "unicode/utypes.h"
#include "unicode/unistr.h"
#include "unicode/uobject.h"

#include "cmemory.h"
#include "cstring.h"
#include "fixedstring.h"
#include "uassert.h"
#include "ucln_cmn.h"
#include "uhash.h"
#include "ulocimp.h"
#include "umutex.h"
#include "uresimp.h"
#include "uvector.h"
#include "udataswp.h" /* for InvChar functions */

static UHashtable* gLocExtKeyMap = nullptr;
static icu::UInitOnce gLocExtKeyMapInitOnce {};

// bit flags for special types
typedef enum {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_KEY_VALUE = 4
} SpecialType;

struct LocExtKeyData : public icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

struct LocExtType : public icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

struct TypeAlias : public icu::UMemory {
    std::string_view from;
};

static icu::MemoryPool<icu::FixedString>* gKeyTypeStringPool = nullptr;
static icu::MemoryPool<LocExtKeyData>* gLocExtKeyDataEntries = nullptr;
static icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
static icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

static UBool U_CALLCONV
uloc_key_type_cleanup() {
    если (gLocExtKeyMap != nullptr) {
        uhash_close(gLocExtKeyMap);
        gLocExtKeyMap = nullptr;
    }

    delete gLocExtKeyDataEntries;
    gLocExtKeyDataEntries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete gKeyTypeStringPool;
    gKeyTypeStringPool = nullptr;

    gLocExtKeyMapInitOnce.reset();
    вернуть true;
}

U_CDECL_END

namespace {

void U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KEY_TYPE, uloc_key_type_cleanup);

    gLocExtKeyMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer keyTypeDataRes(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer keyMapRes(ures_getByKey(keyTypeDataRes.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeMap", nullptr, &sts));

    если (U_FAILURE(sts)) {
        вернуть;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    gKeyTypeStringPool = new icu::MemoryPool<icu::FixedString>;
    если (gKeyTypeStringPool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        вернуть;
    }
    gLocExtKeyDataEntries = new icu::MemoryPool<LocExtKeyData>;
    если (gLocExtKeyDataEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        вернуть;
    }
    gLocExtTypeEntries = new icu::MemoryPool<LocExtType>;
    если (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        вернуть;
    }
    gTypeAliasEntries = new icu::MemoryPool<TypeAlias>;
    если (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        вернуть;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer keyMapEntry;

    пока (ures_hasNext(keyMapRes.getAlias())) {
        keyMapEntry.adoptInstead(ures_getNextResource(keyMapRes.getAlias(), keyMapEntry.orphan(), &sts));
        если (U_FAILURE(sts)) {
            break;
        }
        const char* legacyKeyId = ures_getKey(keyMapEntry.getAlias());
        UnicodeString uBcpKeyId = ures_getUnicodeString(keyMapEntry.getAlias(), &sts);
        если (U_FAILURE(sts)) {
            break;
        }

        // empty value indicates that BCP key is same with the legacy key.
        const char* bcpKeyId = legacyKeyId;
        если (!uBcpKeyId.isEmpty()) {
            icu::FixedString* bcpKeyIdBuf = gKeyTypeStringPool->create();
            если (bcpKeyIdBuf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                break;
            }
            copyInvariantChars(uBcpKeyId, *bcpKeyIdBuf, sts);
            если (U_FAILURE(sts)) {
                break;
            }
            bcpKeyId = bcpKeyIdBuf->data();
        }

        bool isTZ = uprv_strcmp(legacyKeyId, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        если (U_FAILURE(sts)) {
            break;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer typeAliasResByKey;
        LocalUResourceBundlePointer bcpTypeAliasResByKey;

        если (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            typeAliasResByKey.adoptInstead(ures_getByKey(typeAliasRes.getAlias(), legacyKeyId, nullptr, &tmpSts));
            если (U_FAILURE(tmpSts)) {
                typeAliasResByKey.orphan();
            }
        }
        если (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcpTypeAliasResByKey.adoptInstead(ures_getByKey(bcpTypeAliasRes.getAlias(), bcpKeyId, nullptr, &tmpSts));
            если (U_FAILURE(tmpSts)) {
                bcpTypeAliasResByKey.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer typeMapResByKey(ures_getByKey(typeMapRes.getAlias(), legacyKeyId, nullptr, &sts));
        если (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } иначе {
            LocalUResourceBundlePointer typeMapEntry;

            пока (ures_hasNext(typeMapResByKey.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(typeMapResByKey.getAlias(), typeMapEntry.orphan(), &sts));
                если (U_FAILURE(sts)) {
                    break;
                }
                const char* legacyTypeId = ures_getKey(typeMapEntry.getAlias());

                // special types
                если (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    continue;
                }
                если (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    continue;
                }
                если (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_KEY_VALUE;
                    continue;
                }

                если (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    если (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = gKeyTypeStringPool->create(legacyTypeIdView);
                        если (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            break;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                если (U_FAILURE(sts)) {
                    break;
                }

                // empty value indicates that BCP type is same with the legacy type.
                const char* bcpTypeId = legacyTypeId;
                если (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = gKeyTypeStringPool->create();
                    если (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        break;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    если (U_FAILURE(sts)) {
                        break;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                если (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    break;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                если (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                если (U_FAILURE(sts)) {
                    break;
                }

                // also put aliases in the map
                если (typeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(typeAliasResByKey.getAlias());
                    пока (ures_hasNext(typeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(typeAliasResByKey.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        если (U_FAILURE(sts)) {
                            break;
                        }
                        // check if this is an alias of canonical legacy type
                        если (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            const char* from = ures_getKey(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            если (isTZ) {
                                // replace colon with slash if necessary
                                если (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = gKeyTypeStringPool->create(fromView);
                                    если (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        break;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->data(), fromView.length()};
                                }
                            }
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    если (U_FAILURE(sts)) {
                        break;
                    }
                }

                если (bcpTypeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcpTypeAliasResByKey.getAlias());
                    пока (ures_hasNext(bcpTypeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcpTypeAliasResByKey.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        если (U_FAILURE(sts)) {
                            break;
                        }
                        // check if this is an alias of bcp type
                        если (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            const char* from = ures_getKey(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    если (U_FAILURE(sts)) {
                        break;
                    }
                }
            }
        }
        если (U_FAILURE(sts)) {
            break;
        }

        LocExtKeyData* keyData = gLocExtKeyDataEntries->create();
        если (keyData == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            break;
        }
        keyData->bcpId = bcpKeyId;
        keyData->legacyId = legacyKeyId;
        keyData->specialTypes = specialTypes;
        keyData->typeMap.adoptInstead(typeDataMap);

        uhash_put(gLocExtKeyMap, &keyData->legacyId, keyData, &sts);
        если (legacyKeyId != bcpKeyId) {
            // different key value
            uhash_put(gLocExtKeyMap, &keyData->bcpId, keyData, &sts);
        }
        если (U_FAILURE(sts)) {
            break;
        }
    }
}

bool
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(gLocExtKeyMapInitOnce, &initFromResourceBundle, sts);
    если (U_FAILURE(sts)) {
        вернуть false;
    }
    вернуть true;
}

bool
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    для (char c : val) {
        если (c == '-') {
            если (subtagLen < 4 || subtagLen > 6) {
                вернуть false;
            }
            subtagLen = 0;
        } иначе если ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } иначе {
            вернуть false;
        }
    }
    вернуть subtagLen >= 4 && subtagLen <= 6;
}

bool
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    для (char c : val) {
        если (c == '-') {
            если (subtagLen < 3 || subtagLen > 8) {
                вернуть false;
            }
            subtagLen = 0;
        } иначе если (uprv_isASCIILetter(c)) {
            subtagLen++;
        } иначе {
            вернуть false;
        }
    }
    вернуть subtagLen >= 3 && subtagLen <= 8;
}

bool
isSpecialTypeRgKeyValue(std::string_view val) {
    int32_t subtagLen = 0;
    для (char c : val) {
        если ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } иначе {
            вернуть false;
        }
    }
    вернуть subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpKey(std::string_view key) {
    если (!init()) {
        вернуть std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    если (keyData != nullptr) {
        вернуть keyData->bcpId;
    }

    вернуть std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyKey(std::string_view key) {
    если (!init()) {
        вернуть std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    если (keyData != nullptr) {
        вернуть keyData->legacyId;
    }

    вернуть std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    если (!init()) {
        вернуть std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    если (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        если (t != nullptr) {
            вернуть t->bcpId;
        }
        если (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            если (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            если (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            если (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            если (matched) {
                вернуть type;
            }
        }
    }

    вернуть std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    если (!init()) {
        вернуть std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    если (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        если (t != nullptr) {
            вернуть t->legacyId;
        }
        если (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            если (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            если (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            если (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            если (matched) {
                вернуть type;
            }
        }
    }

    вернуть std::nullopt;
}
