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
typedef ga_nanaa {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_KEY_VALUE = 4
} SpecialType;

sa_nracanaa LocExtKeyData : public icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

sa_nracanaa LocExtType : public icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

sa_nracanaa TypeAlias : public icu::UMemory {
    std::string_view from;
};

static icu::MemoryPool<icu::FixedString>* gKeyTypeStringPool = nullptr;
static icu::MemoryPool<LocExtKeyData>* gLocExtKeyDataEntries = nullptr;
static icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
static icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

static UBool U_CALLCONV
uloc_key_type_cleanup() {
    ຖ້າ (gLocExtKeyMap != nullptr) {
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
    ສົ່ງຄືນ true;
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

    ຖ້າ (U_FAILURE(sts)) {
        ສົ່ງຄືນ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    gKeyTypeStringPool = new icu::MemoryPool<icu::FixedString>;
    ຖ້າ (gKeyTypeStringPool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    gLocExtKeyDataEntries = new icu::MemoryPool<LocExtKeyData>;
    ຖ້າ (gLocExtKeyDataEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    gLocExtTypeEntries = new icu::MemoryPool<LocExtType>;
    ຖ້າ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }
    gTypeAliasEntries = new icu::MemoryPool<TypeAlias>;
    ຖ້າ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ສົ່ງຄືນ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer keyMapEntry;

    ໃນຂະນະທີ່ (ures_hasNext(keyMapRes.getAlias())) {
        keyMapEntry.adoptInstead(ures_getNextResource(keyMapRes.getAlias(), keyMapEntry.orphan(), &sts));
        ຖ້າ (U_FAILURE(sts)) {
            bha_nga;
        }
        const var_na* legacyKeyId = ures_getKey(keyMapEntry.getAlias());
        UnicodeString uBcpKeyId = ures_getUnicodeString(keyMapEntry.getAlias(), &sts);
        ຖ້າ (U_FAILURE(sts)) {
            bha_nga;
        }

        // empty value indicates that BCP key is same with the legacy key.
        const var_na* bcpKeyId = legacyKeyId;
        ຖ້າ (!uBcpKeyId.isEmpty()) {
            icu::FixedString* bcpKeyIdBuf = gKeyTypeStringPool->create();
            ຖ້າ (bcpKeyIdBuf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                bha_nga;
            }
            copyInvariantChars(uBcpKeyId, *bcpKeyIdBuf, sts);
            ຖ້າ (U_FAILURE(sts)) {
                bha_nga;
            }
            bcpKeyId = bcpKeyIdBuf->data();
        }

        bool isTZ = uprv_strcmp(legacyKeyId, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        ຖ້າ (U_FAILURE(sts)) {
            bha_nga;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer typeAliasResByKey;
        LocalUResourceBundlePointer bcpTypeAliasResByKey;

        ຖ້າ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            typeAliasResByKey.adoptInstead(ures_getByKey(typeAliasRes.getAlias(), legacyKeyId, nullptr, &tmpSts));
            ຖ້າ (U_FAILURE(tmpSts)) {
                typeAliasResByKey.orphan();
            }
        }
        ຖ້າ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcpTypeAliasResByKey.adoptInstead(ures_getByKey(bcpTypeAliasRes.getAlias(), bcpKeyId, nullptr, &tmpSts));
            ຖ້າ (U_FAILURE(tmpSts)) {
                bcpTypeAliasResByKey.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer typeMapResByKey(ures_getByKey(typeMapRes.getAlias(), legacyKeyId, nullptr, &sts));
        ຖ້າ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ບໍ່ດັ່ງນັ້ນ {
            LocalUResourceBundlePointer typeMapEntry;

            ໃນຂະນະທີ່ (ures_hasNext(typeMapResByKey.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(typeMapResByKey.getAlias(), typeMapEntry.orphan(), &sts));
                ຖ້າ (U_FAILURE(sts)) {
                    bha_nga;
                }
                const var_na* legacyTypeId = ures_getKey(typeMapEntry.getAlias());

                // special types
                ຖ້າ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    continue;
                }
                ຖ້າ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    continue;
                }
                ຖ້າ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_KEY_VALUE;
                    continue;
                }

                ຖ້າ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ຖ້າ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = gKeyTypeStringPool->create(legacyTypeIdView);
                        ຖ້າ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            bha_nga;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                ຖ້າ (U_FAILURE(sts)) {
                    bha_nga;
                }

                // empty value indicates that BCP type is same with the legacy type.
                const var_na* bcpTypeId = legacyTypeId;
                ຖ້າ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = gKeyTypeStringPool->create();
                    ຖ້າ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        bha_nga;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ຖ້າ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ຖ້າ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    bha_nga;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                ຖ້າ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                ຖ້າ (U_FAILURE(sts)) {
                    bha_nga;
                }

                // also put aliases in the map
                ຖ້າ (typeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(typeAliasResByKey.getAlias());
                    ໃນຂະນະທີ່ (ures_hasNext(typeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(typeAliasResByKey.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        ຖ້າ (U_FAILURE(sts)) {
                            bha_nga;
                        }
                        // check if this is an alias of canonical legacy type
                        ຖ້າ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            const var_na* from = ures_getKey(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ຖ້າ (isTZ) {
                                // replace colon with slash if necessary
                                ຖ້າ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = gKeyTypeStringPool->create(fromView);
                                    ຖ້າ (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        bha_nga;
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
                    ຖ້າ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                }

                ຖ້າ (bcpTypeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcpTypeAliasResByKey.getAlias());
                    ໃນຂະນະທີ່ (ures_hasNext(bcpTypeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcpTypeAliasResByKey.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        ຖ້າ (U_FAILURE(sts)) {
                            bha_nga;
                        }
                        // check if this is an alias of bcp type
                        ຖ້າ (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            const var_na* from = ures_getKey(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    ຖ້າ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                }
            }
        }
        ຖ້າ (U_FAILURE(sts)) {
            bha_nga;
        }

        LocExtKeyData* keyData = gLocExtKeyDataEntries->create();
        ຖ້າ (keyData == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            bha_nga;
        }
        keyData->bcpId = bcpKeyId;
        keyData->legacyId = legacyKeyId;
        keyData->specialTypes = specialTypes;
        keyData->typeMap.adoptInstead(typeDataMap);

        uhash_put(gLocExtKeyMap, &keyData->legacyId, keyData, &sts);
        ຖ້າ (legacyKeyId != bcpKeyId) {
            // different key value
            uhash_put(gLocExtKeyMap, &keyData->bcpId, keyData, &sts);
        }
        ຖ້າ (U_FAILURE(sts)) {
            bha_nga;
        }
    }
}

bool
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(gLocExtKeyMapInitOnce, &initFromResourceBundle, sts);
    ຖ້າ (U_FAILURE(sts)) {
        ສົ່ງຄືນ false;
    }
    ສົ່ງຄືນ true;
}

bool
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    ສຳລັບ (var_na c : val) {
        ຖ້າ (c == '-') {
            ຖ້າ (subtagLen < 4 || subtagLen > 6) {
                ສົ່ງຄືນ false;
            }
            subtagLen = 0;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ false;
        }
    }
    ສົ່ງຄືນ subtagLen >= 4 && subtagLen <= 6;
}

bool
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    ສຳລັບ (var_na c : val) {
        ຖ້າ (c == '-') {
            ຖ້າ (subtagLen < 3 || subtagLen > 8) {
                ສົ່ງຄືນ false;
            }
            subtagLen = 0;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ false;
        }
    }
    ສົ່ງຄືນ subtagLen >= 3 && subtagLen <= 8;
}

bool
isSpecialTypeRgKeyValue(std::string_view val) {
    int32_t subtagLen = 0;
    ສຳລັບ (var_na c : val) {
        ຖ້າ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ບໍ່ດັ່ງນັ້ນ {
            ສົ່ງຄືນ false;
        }
    }
    ສົ່ງຄືນ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpKey(std::string_view key) {
    ຖ້າ (!init()) {
        ສົ່ງຄືນ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ຖ້າ (keyData != nullptr) {
        ສົ່ງຄືນ keyData->bcpId;
    }

    ສົ່ງຄືນ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyKey(std::string_view key) {
    ຖ້າ (!init()) {
        ສົ່ງຄືນ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ຖ້າ (keyData != nullptr) {
        ສົ່ງຄືນ keyData->legacyId;
    }

    ສົ່ງຄືນ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ຖ້າ (!init()) {
        ສົ່ງຄືນ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ຖ້າ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ຖ້າ (t != nullptr) {
            ສົ່ງຄືນ t->bcpId;
        }
        ຖ້າ (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            ຖ້າ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ຖ້າ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ຖ້າ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ຖ້າ (matched) {
                ສົ່ງຄືນ type;
            }
        }
    }

    ສົ່ງຄືນ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ຖ້າ (!init()) {
        ສົ່ງຄືນ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ຖ້າ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ຖ້າ (t != nullptr) {
            ສົ່ງຄືນ t->legacyId;
        }
        ຖ້າ (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            ຖ້າ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ຖ້າ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ຖ້າ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ຖ້າ (matched) {
                ສົ່ງຄືນ type;
            }
        }
    }

    ສົ່ງຄືນ std::nullopt;
}
