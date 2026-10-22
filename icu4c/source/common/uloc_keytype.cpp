// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#অন্তর্ভাব <algorithm>
#অন্তর্ভাব <optional>
#অন্তর্ভাব <string_view>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/uobject.h"

#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "fixedstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "uhash.h"
#অন্তর্ভাব "ulocimp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uresimp.h"
#অন্তর্ভাব "uvector.h"
#অন্তর্ভাব "udataswp.h" /* for InvChar functions */

স্থির UHashtable* g_Loc_Ext_চাবি_Map = nullptr;
স্থির icu::UInitOnce g_Loc_Ext_চাবি_Map_Init_Once {};

// bit flags for special types
প্রকারনির্দেশ গণনা {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_চাবি_মান = 4
} SpecialType;

সংরচনা Loc_Ext_চাবি_Data : সার্বজনিক icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

সংরচনা LocExtType : সার্বজনিক icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

সংরচনা TypeAlias : সার্বজনিক icu::UMemory {
    std::string_view from;
};

স্থির icu::MemoryPool<icu::FixedString>* g_চাবি_Type_String_Pool = nullptr;
স্থির icu::MemoryPool<Loc_Ext_চাবি_Data>* g_Loc_Ext_চাবি_Data_Entries = nullptr;
স্থির icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
স্থির icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

স্থির UBool U_CALLCONV
uloc_চাবি_type_cleanup() {
    যদি (g_Loc_Ext_চাবি_Map != nullptr) {
        uhash_close(g_Loc_Ext_চাবি_Map);
        g_Loc_Ext_চাবি_Map = nullptr;
    }

    delete g_Loc_Ext_চাবি_Data_Entries;
    g_Loc_Ext_চাবি_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_চাবি_Type_String_Pool;
    g_চাবি_Type_String_Pool = nullptr;

    g_Loc_Ext_চাবি_Map_Init_Once.reset();
    প্রত্যাগম সত্য;
}

U_CDECL_END

নামাকাশ {

শূন্য U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_চাবি_TYPE, uloc_চাবি_type_cleanup);

    g_Loc_Ext_চাবি_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer চাবি_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer চাবি_Map_Res(ures_get_By_চাবি(চাবি_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_চাবি(চাবি_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    যদি (U_FAILURE(sts)) {
        প্রত্যাগম;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_চাবি(চাবি_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_চাবি(চাবি_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_চাবি_Type_String_Pool = নূতন icu::MemoryPool<icu::FixedString>;
    যদি (g_চাবি_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    g_Loc_Ext_চাবি_Data_Entries = নূতন icu::MemoryPool<Loc_Ext_চাবি_Data>;
    যদি (g_Loc_Ext_চাবি_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    gLocExtTypeEntries = নূতন icu::MemoryPool<LocExtType>;
    যদি (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
    gTypeAliasEntries = নূতন icu::MemoryPool<TypeAlias>;
    যদি (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer চাবি_Map_Entry;

    যাবত্ (ures_hasNext(চাবি_Map_Res.getAlias())) {
        চাবি_Map_Entry.adoptInstead(ures_getNextResource(চাবি_Map_Res.getAlias(), চাবি_Map_Entry.orphan(), &sts));
        যদি (U_FAILURE(sts)) {
            ভঙ্গ;
        }
        ধ্রুব বর্ণ* legacy_চাবি_Id = ures_get_চাবি(চাবি_Map_Entry.getAlias());
        UnicodeString u_Bcp_চাবি_Id = ures_getUnicodeString(চাবি_Map_Entry.getAlias(), &sts);
        যদি (U_FAILURE(sts)) {
            ভঙ্গ;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ধ্রুব বর্ণ* bcp_চাবি_Id = legacy_চাবি_Id;
        যদি (!u_Bcp_চাবি_Id.isEmpty()) {
            icu::FixedString* bcp_চাবি_Id_Buf = g_চাবি_Type_String_Pool->create();
            যদি (bcp_চাবি_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                ভঙ্গ;
            }
            copyInvariantChars(u_Bcp_চাবি_Id, *bcp_চাবি_Id_Buf, sts);
            যদি (U_FAILURE(sts)) {
                ভঙ্গ;
            }
            bcp_চাবি_Id = bcp_চাবি_Id_Buf->data();
        }

        তার্কিক isTZ = uprv_strcmp(legacy_চাবি_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        যদি (U_FAILURE(sts)) {
            ভঙ্গ;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_চাবি;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_চাবি;

        যদি (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_চাবি.adoptInstead(ures_get_By_চাবি(typeAliasRes.getAlias(), legacy_চাবি_Id, nullptr, &tmpSts));
            যদি (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_চাবি.orphan();
            }
        }
        যদি (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_চাবি.adoptInstead(ures_get_By_চাবি(bcpTypeAliasRes.getAlias(), bcp_চাবি_Id, nullptr, &tmpSts));
            যদি (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_চাবি.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_চাবি(ures_get_By_চাবি(typeMapRes.getAlias(), legacy_চাবি_Id, nullptr, &sts));
        যদি (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } অন্যথা {
            LocalUResourceBundlePointer typeMapEntry;

            যাবত্ (ures_hasNext(type_Map_Res_By_চাবি.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_চাবি.getAlias(), typeMapEntry.orphan(), &sts));
                যদি (U_FAILURE(sts)) {
                    ভঙ্গ;
                }
                ধ্রুব বর্ণ* legacyTypeId = ures_get_চাবি(typeMapEntry.getAlias());

                // special types
                যদি (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    অনুবর্ত;
                }
                যদি (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    অনুবর্ত;
                }
                যদি (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_চাবি_মান;
                    অনুবর্ত;
                }

                যদি (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    যদি (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_চাবি_Type_String_Pool->create(legacyTypeIdView);
                        যদি (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            ভঙ্গ;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                যদি (U_FAILURE(sts)) {
                    ভঙ্গ;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ধ্রুব বর্ণ* bcpTypeId = legacyTypeId;
                যদি (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_চাবি_Type_String_Pool->create();
                    যদি (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        ভঙ্গ;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    যদি (U_FAILURE(sts)) {
                        ভঙ্গ;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                যদি (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    ভঙ্গ;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                যদি (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                যদি (U_FAILURE(sts)) {
                    ভঙ্গ;
                }

                // also put aliases in the map
                যদি (type_Alias_Res_By_চাবি.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_চাবি.getAlias());
                    যাবত্ (ures_hasNext(type_Alias_Res_By_চাবি.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_চাবি.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ধ্রুব char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        যদি (U_FAILURE(sts)) {
                            ভঙ্গ;
                        }
                        // check if this is an alias of canonical legacy type
                        যদি (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ধ্রুব বর্ণ* from = ures_get_চাবি(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            যদি (isTZ) {
                                // replace colon with slash if necessary
                                যদি (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_চাবি_Type_String_Pool->create(fromView);
                                    যদি (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        ভঙ্গ;
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
                    যদি (U_FAILURE(sts)) {
                        ভঙ্গ;
                    }
                }

                যদি (bcp_Type_Alias_Res_By_চাবি.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_চাবি.getAlias());
                    যাবত্ (ures_hasNext(bcp_Type_Alias_Res_By_চাবি.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_চাবি.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ধ্রুব char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        যদি (U_FAILURE(sts)) {
                            ভঙ্গ;
                        }
                        // check if this is an alias of bcp type
                        যদি (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ধ্রুব বর্ণ* from = ures_get_চাবি(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    যদি (U_FAILURE(sts)) {
                        ভঙ্গ;
                    }
                }
            }
        }
        যদি (U_FAILURE(sts)) {
            ভঙ্গ;
        }

        Loc_Ext_চাবি_Data* চাবি_Data = g_Loc_Ext_চাবি_Data_Entries->create();
        যদি (চাবি_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            ভঙ্গ;
        }
        চাবি_Data->bcpId = bcp_চাবি_Id;
        চাবি_Data->legacyId = legacy_চাবি_Id;
        চাবি_Data->specialTypes = specialTypes;
        চাবি_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_চাবি_Map, &চাবি_Data->legacyId, চাবি_Data, &sts);
        যদি (legacy_চাবি_Id != bcp_চাবি_Id) {
            // different key value
            uhash_put(g_Loc_Ext_চাবি_Map, &চাবি_Data->bcpId, চাবি_Data, &sts);
        }
        যদি (U_FAILURE(sts)) {
            ভঙ্গ;
        }
    }
}

তার্কিক
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_চাবি_Map_Init_Once, &initFromResourceBundle, sts);
    যদি (U_FAILURE(sts)) {
        প্রত্যাগম অসত্য;
    }
    প্রত্যাগম সত্য;
}

তার্কিক
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    প্রতি (বর্ণ c : val) {
        যদি (c == '-') {
            যদি (subtagLen < 4 || subtagLen > 6) {
                প্রত্যাগম অসত্য;
            }
            subtagLen = 0;
        } অন্যথা যদি ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } অন্যথা {
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম subtagLen >= 4 && subtagLen <= 6;
}

তার্কিক
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    প্রতি (বর্ণ c : val) {
        যদি (c == '-') {
            যদি (subtagLen < 3 || subtagLen > 8) {
                প্রত্যাগম অসত্য;
            }
            subtagLen = 0;
        } অন্যথা যদি (uprv_isASCIILetter(c)) {
            subtagLen++;
        } অন্যথা {
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম subtagLen >= 3 && subtagLen <= 8;
}

তার্কিক
is_Special_Type_Rg_চাবি_মান(std::string_view val) {
    int32_t subtagLen = 0;
    প্রতি (বর্ণ c : val) {
        যদি ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } অন্যথা {
            প্রত্যাগম অসত্য;
        }
    }
    প্রত্যাগম subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_চাবি(std::string_view চাবি) {
    যদি (!init()) {
        প্রত্যাগম std::nullopt;
    }

    Loc_Ext_চাবি_Data* চাবি_Data = static_cast<Loc_Ext_চাবি_Data*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_Data != nullptr) {
        প্রত্যাগম চাবি_Data->bcpId;
    }

    প্রত্যাগম std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_চাবি(std::string_view চাবি) {
    যদি (!init()) {
        প্রত্যাগম std::nullopt;
    }

    Loc_Ext_চাবি_Data* চাবি_Data = static_cast<Loc_Ext_চাবি_Data*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_Data != nullptr) {
        প্রত্যাগম চাবি_Data->legacyId;
    }

    প্রত্যাগম std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view চাবি, std::string_view type U_LIFETIME_BOUND) {
    যদি (!init()) {
        প্রত্যাগম std::nullopt;
    }

    Loc_Ext_চাবি_Data* চাবি_Data = static_cast<Loc_Ext_চাবি_Data*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(চাবি_Data->typeMap.getAlias(), &type));
        যদি (t != nullptr) {
            প্রত্যাগম t->bcpId;
        }
        যদি (চাবি_Data->specialTypes != SPECIALTYPE_NONE) {
            তার্কিক matched = অসত্য;
            যদি (চাবি_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            যদি (!matched && চাবি_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            যদি (!matched && চাবি_Data->specialTypes & SPECIALTYPE_RG_চাবি_মান) {
                matched = is_Special_Type_Rg_চাবি_মান(type);
            }
            যদি (matched) {
                প্রত্যাগম type;
            }
        }
    }

    প্রত্যাগম std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view চাবি, std::string_view type U_LIFETIME_BOUND) {
    যদি (!init()) {
        প্রত্যাগম std::nullopt;
    }

    Loc_Ext_চাবি_Data* চাবি_Data = static_cast<Loc_Ext_চাবি_Data*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(চাবি_Data->typeMap.getAlias(), &type));
        যদি (t != nullptr) {
            প্রত্যাগম t->legacyId;
        }
        যদি (চাবি_Data->specialTypes != SPECIALTYPE_NONE) {
            তার্কিক matched = অসত্য;
            যদি (চাবি_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            যদি (!matched && চাবি_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            যদি (!matched && চাবি_Data->specialTypes & SPECIALTYPE_RG_চাবি_মান) {
                matched = is_Special_Type_Rg_চাবি_মান(type);
            }
            যদি (matched) {
                প্রত্যাগম type;
            }
        }
    }

    প্রত্যাগম std::nullopt;
}
