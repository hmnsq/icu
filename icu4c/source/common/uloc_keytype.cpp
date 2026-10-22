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

static UHashtable* g_Loc_Ext_सुसि_Map = nullptr;
static icu::UInitOnce g_Loc_Ext_सुसि_Map_Init_Once {};

// bit flags for special types
typedef enum {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_सुसि_हालागा = 4
} SpecialType;

struct Loc_Ext_सुसि_Data : public icu::UMemory {
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

static icu::MemoryPool<icu::FixedString>* g_सुसि_Type_String_Pool = nullptr;
static icu::MemoryPool<Loc_Ext_सुसि_Data>* g_Loc_Ext_सुसि_Data_Entries = nullptr;
static icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
static icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

static UBool U_CALLCONV
uloc_सुसि_type_cleanup() {
    if (g_Loc_Ext_सुसि_Map != nullptr) {
        uhash_close(g_Loc_Ext_सुसि_Map);
        g_Loc_Ext_सुसि_Map = nullptr;
    }

    delete g_Loc_Ext_सुसि_Data_Entries;
    g_Loc_Ext_सुसि_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_सुसि_Type_String_Pool;
    g_सुसि_Type_String_Pool = nullptr;

    g_Loc_Ext_सुसि_Map_Init_Once.reset();
    return true;
}

U_CDECL_END

namespace {

void U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_सुसि_TYPE, uloc_सुसि_type_cleanup);

    g_Loc_Ext_सुसि_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer सुसि_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer सुसि_Map_Res(ures_get_By_सुसि(सुसि_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_सुसि(सुसि_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    if (U_FAILURE(sts)) {
        return;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_सुसि(सुसि_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_सुसि(सुसि_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_सुसि_Type_String_Pool = new icu::MemoryPool<icu::FixedString>;
    if (g_सुसि_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    g_Loc_Ext_सुसि_Data_Entries = new icu::MemoryPool<Loc_Ext_सुसि_Data>;
    if (g_Loc_Ext_सुसि_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    gLocExtTypeEntries = new icu::MemoryPool<LocExtType>;
    if (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        return;
    }
    gTypeAliasEntries = new icu::MemoryPool<TypeAlias>;
    if (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        return;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer सुसि_Map_Entry;

    while (ures_hasNext(सुसि_Map_Res.getAlias())) {
        सुसि_Map_Entry.adoptInstead(ures_getNextResource(सुसि_Map_Res.getAlias(), सुसि_Map_Entry.orphan(), &sts));
        if (U_FAILURE(sts)) {
            break;
        }
        const char* legacy_सुसि_Id = ures_get_सुसि(सुसि_Map_Entry.getAlias());
        UnicodeString u_Bcp_सुसि_Id = ures_getUnicodeString(सुसि_Map_Entry.getAlias(), &sts);
        if (U_FAILURE(sts)) {
            break;
        }

        // empty value indicates that BCP key is same with the legacy key.
        const char* bcp_सुसि_Id = legacy_सुसि_Id;
        if (!u_Bcp_सुसि_Id.isEmpty()) {
            icu::FixedString* bcp_सुसि_Id_Buf = g_सुसि_Type_String_Pool->create();
            if (bcp_सुसि_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                break;
            }
            copyInvariantChars(u_Bcp_सुसि_Id, *bcp_सुसि_Id_Buf, sts);
            if (U_FAILURE(sts)) {
                break;
            }
            bcp_सुसि_Id = bcp_सुसि_Id_Buf->data();
        }

        bool isTZ = uprv_strcmp(legacy_सुसि_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        if (U_FAILURE(sts)) {
            break;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_सुसि;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_सुसि;

        if (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_सुसि.adoptInstead(ures_get_By_सुसि(typeAliasRes.getAlias(), legacy_सुसि_Id, nullptr, &tmpSts));
            if (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_सुसि.orphan();
            }
        }
        if (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_सुसि.adoptInstead(ures_get_By_सुसि(bcpTypeAliasRes.getAlias(), bcp_सुसि_Id, nullptr, &tmpSts));
            if (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_सुसि.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_सुसि(ures_get_By_सुसि(typeMapRes.getAlias(), legacy_सुसि_Id, nullptr, &sts));
        if (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } else {
            LocalUResourceBundlePointer typeMapEntry;

            while (ures_hasNext(type_Map_Res_By_सुसि.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_सुसि.getAlias(), typeMapEntry.orphan(), &sts));
                if (U_FAILURE(sts)) {
                    break;
                }
                const char* legacyTypeId = ures_get_सुसि(typeMapEntry.getAlias());

                // special types
                if (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    continue;
                }
                if (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    continue;
                }
                if (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_सुसि_हालागा;
                    continue;
                }

                if (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    if (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_सुसि_Type_String_Pool->create(legacyTypeIdView);
                        if (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
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
                if (U_FAILURE(sts)) {
                    break;
                }

                // empty value indicates that BCP type is same with the legacy type.
                const char* bcpTypeId = legacyTypeId;
                if (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_सुसि_Type_String_Pool->create();
                    if (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        break;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    if (U_FAILURE(sts)) {
                        break;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                if (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    break;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                if (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                if (U_FAILURE(sts)) {
                    break;
                }

                // also put aliases in the map
                if (type_Alias_Res_By_सुसि.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_सुसि.getAlias());
                    while (ures_hasNext(type_Alias_Res_By_सुसि.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_सुसि.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        if (U_FAILURE(sts)) {
                            break;
                        }
                        // check if this is an alias of canonical legacy type
                        if (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            const char* from = ures_get_सुसि(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            if (isTZ) {
                                // replace colon with slash if necessary
                                if (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_सुसि_Type_String_Pool->create(fromView);
                                    if (fromBuf == nullptr || fromBuf->isEmpty()) {
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
                    if (U_FAILURE(sts)) {
                        break;
                    }
                }

                if (bcp_Type_Alias_Res_By_सुसि.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_सुसि.getAlias());
                    while (ures_hasNext(bcp_Type_Alias_Res_By_सुसि.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_सुसि.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        const char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        if (U_FAILURE(sts)) {
                            break;
                        }
                        // check if this is an alias of bcp type
                        if (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            const char* from = ures_get_सुसि(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    if (U_FAILURE(sts)) {
                        break;
                    }
                }
            }
        }
        if (U_FAILURE(sts)) {
            break;
        }

        Loc_Ext_सुसि_Data* सुसि_Data = g_Loc_Ext_सुसि_Data_Entries->create();
        if (सुसि_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            break;
        }
        सुसि_Data->bcpId = bcp_सुसि_Id;
        सुसि_Data->legacyId = legacy_सुसि_Id;
        सुसि_Data->specialTypes = specialTypes;
        सुसि_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_सुसि_Map, &सुसि_Data->legacyId, सुसि_Data, &sts);
        if (legacy_सुसि_Id != bcp_सुसि_Id) {
            // different key value
            uhash_put(g_Loc_Ext_सुसि_Map, &सुसि_Data->bcpId, सुसि_Data, &sts);
        }
        if (U_FAILURE(sts)) {
            break;
        }
    }
}

bool
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_सुसि_Map_Init_Once, &initFromResourceBundle, sts);
    if (U_FAILURE(sts)) {
        return false;
    }
    return true;
}

bool
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    for (char c : val) {
        if (c == '-') {
            if (subtagLen < 4 || subtagLen > 6) {
                return false;
            }
            subtagLen = 0;
        } else if ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } else {
            return false;
        }
    }
    return subtagLen >= 4 && subtagLen <= 6;
}

bool
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    for (char c : val) {
        if (c == '-') {
            if (subtagLen < 3 || subtagLen > 8) {
                return false;
            }
            subtagLen = 0;
        } else if (uprv_isASCIILetter(c)) {
            subtagLen++;
        } else {
            return false;
        }
    }
    return subtagLen >= 3 && subtagLen <= 8;
}

bool
is_Special_Type_Rg_सुसि_हालागा(std::string_view val) {
    int32_t subtagLen = 0;
    for (char c : val) {
        if ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } else {
            return false;
        }
    }
    return subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_सुसि(std::string_view सुसि) {
    if (!init()) {
        return std::nullopt;
    }

    Loc_Ext_सुसि_Data* सुसि_Data = static_cast<Loc_Ext_सुसि_Data*>(uhash_get(g_Loc_Ext_सुसि_Map, &सुसि));
    if (सुसि_Data != nullptr) {
        return सुसि_Data->bcpId;
    }

    return std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_सुसि(std::string_view सुसि) {
    if (!init()) {
        return std::nullopt;
    }

    Loc_Ext_सुसि_Data* सुसि_Data = static_cast<Loc_Ext_सुसि_Data*>(uhash_get(g_Loc_Ext_सुसि_Map, &सुसि));
    if (सुसि_Data != nullptr) {
        return सुसि_Data->legacyId;
    }

    return std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view सुसि, std::string_view type U_LIFETIME_BOUND) {
    if (!init()) {
        return std::nullopt;
    }

    Loc_Ext_सुसि_Data* सुसि_Data = static_cast<Loc_Ext_सुसि_Data*>(uhash_get(g_Loc_Ext_सुसि_Map, &सुसि));
    if (सुसि_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(सुसि_Data->typeMap.getAlias(), &type));
        if (t != nullptr) {
            return t->bcpId;
        }
        if (सुसि_Data->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            if (सुसि_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            if (!matched && सुसि_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            if (!matched && सुसि_Data->specialTypes & SPECIALTYPE_RG_सुसि_हालागा) {
                matched = is_Special_Type_Rg_सुसि_हालागा(type);
            }
            if (matched) {
                return type;
            }
        }
    }

    return std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view सुसि, std::string_view type U_LIFETIME_BOUND) {
    if (!init()) {
        return std::nullopt;
    }

    Loc_Ext_सुसि_Data* सुसि_Data = static_cast<Loc_Ext_सुसि_Data*>(uhash_get(g_Loc_Ext_सुसि_Map, &सुसि));
    if (सुसि_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(सुसि_Data->typeMap.getAlias(), &type));
        if (t != nullptr) {
            return t->legacyId;
        }
        if (सुसि_Data->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            if (सुसि_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            if (!matched && सुसि_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            if (!matched && सुसि_Data->specialTypes & SPECIALTYPE_RG_सुसि_हालागा) {
                matched = is_Special_Type_Rg_सुसि_हालागा(type);
            }
            if (matched) {
                return type;
            }
        }
    }

    return std::nullopt;
}
