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
    ᬬᬾᬦ᭄ (gLocExtKeyMap != nullptr) {
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
    ᬯᬮᬶ true;
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

    ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
        ᬯᬮᬶ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    gKeyTypeStringPool = new icu::MemoryPool<icu::FixedString>;
    ᬬᬾᬦ᭄ (gKeyTypeStringPool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ;
    }
    gLocExtKeyDataEntries = new icu::MemoryPool<LocExtKeyData>;
    ᬬᬾᬦ᭄ (gLocExtKeyDataEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ;
    }
    gLocExtTypeEntries = new icu::MemoryPool<LocExtType>;
    ᬬᬾᬦ᭄ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ;
    }
    gTypeAliasEntries = new icu::MemoryPool<TypeAlias>;
    ᬬᬾᬦ᭄ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ᬯᬮᬶ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer keyMapEntry;

    ᬲᭂᬤᭂᬮ (ures_hasNext(keyMapRes.getAlias())) {
        keyMapEntry.adoptInstead(ures_getNextResource(keyMapRes.getAlias(), keyMapEntry.orphan(), &sts));
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            bha_nga;
        }
        const var_na* legacyKeyId = ures_getKey(keyMapEntry.getAlias());
        UnicodeString uBcpKeyId = ures_getUnicodeString(keyMapEntry.getAlias(), &sts);
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            bha_nga;
        }

        // empty value indicates that BCP key is same with the legacy key.
        const var_na* bcpKeyId = legacyKeyId;
        ᬬᬾᬦ᭄ (!uBcpKeyId.isEmpty()) {
            icu::FixedString* bcpKeyIdBuf = gKeyTypeStringPool->create();
            ᬬᬾᬦ᭄ (bcpKeyIdBuf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                bha_nga;
            }
            copyInvariantChars(uBcpKeyId, *bcpKeyIdBuf, sts);
            ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                bha_nga;
            }
            bcpKeyId = bcpKeyIdBuf->data();
        }

        bool isTZ = uprv_strcmp(legacyKeyId, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            bha_nga;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer typeAliasResByKey;
        LocalUResourceBundlePointer bcpTypeAliasResByKey;

        ᬬᬾᬦ᭄ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            typeAliasResByKey.adoptInstead(ures_getByKey(typeAliasRes.getAlias(), legacyKeyId, nullptr, &tmpSts));
            ᬬᬾᬦ᭄ (U_FAILURE(tmpSts)) {
                typeAliasResByKey.orphan();
            }
        }
        ᬬᬾᬦ᭄ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcpTypeAliasResByKey.adoptInstead(ures_getByKey(bcpTypeAliasRes.getAlias(), bcpKeyId, nullptr, &tmpSts));
            ᬬᬾᬦ᭄ (U_FAILURE(tmpSts)) {
                bcpTypeAliasResByKey.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer typeMapResByKey(ures_getByKey(typeMapRes.getAlias(), legacyKeyId, nullptr, &sts));
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ᬮᬶᬬ {
            LocalUResourceBundlePointer typeMapEntry;

            ᬲᭂᬤᭂᬮ (ures_hasNext(typeMapResByKey.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(typeMapResByKey.getAlias(), typeMapEntry.orphan(), &sts));
                ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                    bha_nga;
                }
                const var_na* legacyTypeId = ures_getKey(typeMapEntry.getAlias());

                // special types
                ᬬᬾᬦ᭄ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    continue;
                }
                ᬬᬾᬦ᭄ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    continue;
                }
                ᬬᬾᬦ᭄ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_KEY_VALUE;
                    continue;
                }

                ᬬᬾᬦ᭄ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ᬬᬾᬦ᭄ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = gKeyTypeStringPool->create(legacyTypeIdView);
                        ᬬᬾᬦ᭄ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
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
                ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                    bha_nga;
                }

                // empty value indicates that BCP type is same with the legacy type.
                const var_na* bcpTypeId = legacyTypeId;
                ᬬᬾᬦ᭄ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = gKeyTypeStringPool->create();
                    ᬬᬾᬦ᭄ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        bha_nga;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ᬬᬾᬦ᭄ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    bha_nga;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                ᬬᬾᬦ᭄ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                    bha_nga;
                }

                // also put aliases in the map
                ᬬᬾᬦ᭄ (typeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(typeAliasResByKey.getAlias());
                    ᬲᭂᬤᭂᬮ (ures_hasNext(typeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(typeAliasResByKey.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                            bha_nga;
                        }
                        // check if this is an alias of canonical legacy type
                        ᬬᬾᬦ᭄ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            const var_na* from = ures_getKey(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ᬬᬾᬦ᭄ (isTZ) {
                                // replace colon with slash if necessary
                                ᬬᬾᬦ᭄ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = gKeyTypeStringPool->create(fromView);
                                    ᬬᬾᬦ᭄ (fromBuf == nullptr || fromBuf->isEmpty()) {
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
                    ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                }

                ᬬᬾᬦ᭄ (bcpTypeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcpTypeAliasResByKey.getAlias());
                    ᬲᭂᬤᭂᬮ (ures_hasNext(bcpTypeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcpTypeAliasResByKey.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                            bha_nga;
                        }
                        // check if this is an alias of bcp type
                        ᬬᬾᬦ᭄ (uprv_compareInvWithUChar(
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
                    ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
                        bha_nga;
                    }
                }
            }
        }
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            bha_nga;
        }

        LocExtKeyData* keyData = gLocExtKeyDataEntries->create();
        ᬬᬾᬦ᭄ (keyData == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            bha_nga;
        }
        keyData->bcpId = bcpKeyId;
        keyData->legacyId = legacyKeyId;
        keyData->specialTypes = specialTypes;
        keyData->typeMap.adoptInstead(typeDataMap);

        uhash_put(gLocExtKeyMap, &keyData->legacyId, keyData, &sts);
        ᬬᬾᬦ᭄ (legacyKeyId != bcpKeyId) {
            // different key value
            uhash_put(gLocExtKeyMap, &keyData->bcpId, keyData, &sts);
        }
        ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
            bha_nga;
        }
    }
}

bool
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(gLocExtKeyMapInitOnce, &initFromResourceBundle, sts);
    ᬬᬾᬦ᭄ (U_FAILURE(sts)) {
        ᬯᬮᬶ false;
    }
    ᬯᬮᬶ true;
}

bool
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    for (var_na c : val) {
        ᬬᬾᬦ᭄ (c == '-') {
            ᬬᬾᬦ᭄ (subtagLen < 4 || subtagLen > 6) {
                ᬯᬮᬶ false;
            }
            subtagLen = 0;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ᬮᬶᬬ {
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ subtagLen >= 4 && subtagLen <= 6;
}

bool
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    for (var_na c : val) {
        ᬬᬾᬦ᭄ (c == '-') {
            ᬬᬾᬦ᭄ (subtagLen < 3 || subtagLen > 8) {
                ᬯᬮᬶ false;
            }
            subtagLen = 0;
        } ᬮᬶᬬ ᬬᬾᬦ᭄ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ᬮᬶᬬ {
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ subtagLen >= 3 && subtagLen <= 8;
}

bool
isSpecialTypeRgKeyValue(std::string_view val) {
    int32_t subtagLen = 0;
    for (var_na c : val) {
        ᬬᬾᬦ᭄ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ᬮᬶᬬ {
            ᬯᬮᬶ false;
        }
    }
    ᬯᬮᬶ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpKey(std::string_view key) {
    ᬬᬾᬦ᭄ (!init()) {
        ᬯᬮᬶ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ᬬᬾᬦ᭄ (keyData != nullptr) {
        ᬯᬮᬶ keyData->bcpId;
    }

    ᬯᬮᬶ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyKey(std::string_view key) {
    ᬬᬾᬦ᭄ (!init()) {
        ᬯᬮᬶ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ᬬᬾᬦ᭄ (keyData != nullptr) {
        ᬯᬮᬶ keyData->legacyId;
    }

    ᬯᬮᬶ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ᬬᬾᬦ᭄ (!init()) {
        ᬯᬮᬶ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ᬬᬾᬦ᭄ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ᬬᬾᬦ᭄ (t != nullptr) {
            ᬯᬮᬶ t->bcpId;
        }
        ᬬᬾᬦ᭄ (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            ᬬᬾᬦ᭄ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ᬬᬾᬦ᭄ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ᬬᬾᬦ᭄ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ᬬᬾᬦ᭄ (matched) {
                ᬯᬮᬶ type;
            }
        }
    }

    ᬯᬮᬶ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ᬬᬾᬦ᭄ (!init()) {
        ᬯᬮᬶ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ᬬᬾᬦ᭄ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ᬬᬾᬦ᭄ (t != nullptr) {
            ᬯᬮᬶ t->legacyId;
        }
        ᬬᬾᬦ᭄ (keyData->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            ᬬᬾᬦ᭄ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ᬬᬾᬦ᭄ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ᬬᬾᬦ᭄ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ᬬᬾᬦ᭄ (matched) {
                ᬯᬮᬶ type;
            }
        }
    }

    ᬯᬮᬶ std::nullopt;
}
