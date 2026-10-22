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
প্রকারলেখ গণনা {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_চাবি_মান = 4
} SpecialType;

কাঠামো Loc_Ext_চাবি_তথ্য : সর্বজনীন icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

কাঠামো LocExtType : সর্বজনীন icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

কাঠামো TypeAlias : সর্বজনীন icu::UMemory {
    std::string_view from;
};

স্থির icu::MemoryPool<icu::FixedString>* g_চাবি_Type_String_Pool = nullptr;
স্থির icu::MemoryPool<Loc_Ext_চাবি_তথ্য>* g_Loc_Ext_চাবি_তথ্য_Entries = nullptr;
স্থির icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
স্থির icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

স্থির UBool U_CALLCONV
uloc_চাবি_type_cleanup() {
    যদি (g_Loc_Ext_চাবি_Map != nullptr) {
        uhash_close(g_Loc_Ext_চাবি_Map);
        g_Loc_Ext_চাবি_Map = nullptr;
    }

    delete g_Loc_Ext_চাবি_তথ্য_Entries;
    g_Loc_Ext_চাবি_তথ্য_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_চাবি_Type_String_Pool;
    g_চাবি_Type_String_Pool = nullptr;

    g_Loc_Ext_চাবি_Map_Init_Once.reset();
    ফেরত সত্য;
}

U_CDECL_END

নামাকাশ {

শূন্য U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_চাবি_TYPE, uloc_চাবি_type_cleanup);

    g_Loc_Ext_চাবি_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer চাবি_Type_তথ্য_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer চাবি_Map_Res(ures_get_By_চাবি(চাবি_Type_তথ্য_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_চাবি(চাবি_Type_তথ্য_Res.getAlias(), "typeMap", nullptr, &sts));

    যদি (U_FAILURE(sts)) {
        ফেরত;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_চাবি(চাবি_Type_তথ্য_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_চাবি(চাবি_Type_তথ্য_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_চাবি_Type_String_Pool = নতুন icu::MemoryPool<icu::FixedString>;
    যদি (g_চাবি_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    g_Loc_Ext_চাবি_তথ্য_Entries = নতুন icu::MemoryPool<Loc_Ext_চাবি_তথ্য>;
    যদি (g_Loc_Ext_চাবি_তথ্য_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    gLocExtTypeEntries = নতুন icu::MemoryPool<LocExtType>;
    যদি (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
    gTypeAliasEntries = নতুন icu::MemoryPool<TypeAlias>;
    যদি (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer চাবি_Map_Entry;

    যখন (ures_has_পরবর্তী(চাবি_Map_Res.getAlias())) {
        চাবি_Map_Entry.adoptInstead(ures_get_পরবর্তী_Resource(চাবি_Map_Res.getAlias(), চাবি_Map_Entry.orphan(), &sts));
        যদি (U_FAILURE(sts)) {
            বিরতি;
        }
        ধ্রুবক অক্ষর* legacy_চাবি_Id = ures_get_চাবি(চাবি_Map_Entry.getAlias());
        UnicodeString u_Bcp_চাবি_Id = ures_getUnicodeString(চাবি_Map_Entry.getAlias(), &sts);
        যদি (U_FAILURE(sts)) {
            বিরতি;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ধ্রুবক অক্ষর* bcp_চাবি_Id = legacy_চাবি_Id;
        যদি (!u_Bcp_চাবি_Id.isEmpty()) {
            icu::FixedString* bcp_চাবি_Id_Buf = g_চাবি_Type_String_Pool->create();
            যদি (bcp_চাবি_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                বিরতি;
            }
            copyInvariantChars(u_Bcp_চাবি_Id, *bcp_চাবি_Id_Buf, sts);
            যদি (U_FAILURE(sts)) {
                বিরতি;
            }
            bcp_চাবি_Id = bcp_চাবি_Id_Buf->তথ্য();
        }

        যৌক্তিক isTZ = uprv_strcmp(legacy_চাবি_Id, "timezone") == 0;

        UHashtable* type_তথ্য_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        যদি (U_FAILURE(sts)) {
            বিরতি;
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

            যখন (ures_has_পরবর্তী(type_Map_Res_By_চাবি.getAlias())) {
                typeMapEntry.adoptInstead(ures_get_পরবর্তী_Resource(type_Map_Res_By_চাবি.getAlias(), typeMapEntry.orphan(), &sts));
                যদি (U_FAILURE(sts)) {
                    বিরতি;
                }
                ধ্রুবক অক্ষর* legacyTypeId = ures_get_চাবি(typeMapEntry.getAlias());

                // special types
                যদি (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    চালু;
                }
                যদি (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    চালু;
                }
                যদি (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_চাবি_মান;
                    চালু;
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
                            বিরতি;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->তথ্য();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                যদি (U_FAILURE(sts)) {
                    বিরতি;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ধ্রুবক অক্ষর* bcpTypeId = legacyTypeId;
                যদি (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_চাবি_Type_String_Pool->create();
                    যদি (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        বিরতি;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    যদি (U_FAILURE(sts)) {
                        বিরতি;
                    }
                    bcpTypeId = bcpTypeIdBuf->তথ্য();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                যদি (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    বিরতি;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_তথ্য_Map, &t->legacyId, t, &sts);
                যদি (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_তথ্য_Map, &t->bcpId, t, &sts);
                }
                যদি (U_FAILURE(sts)) {
                    বিরতি;
                }

                // also put aliases in the map
                যদি (type_Alias_Res_By_চাবি.isValid()) {
                    LocalUResourceBundlePointer type_Alias_তথ্য_Entry;

                    ures_resetIterator(type_Alias_Res_By_চাবি.getAlias());
                    যখন (ures_has_পরবর্তী(type_Alias_Res_By_চাবি.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        type_Alias_তথ্য_Entry.adoptInstead(ures_get_পরবর্তী_Resource(type_Alias_Res_By_চাবি.getAlias(), type_Alias_তথ্য_Entry.orphan(), &sts));
                        ধ্রুবক char16_t* to = ures_getString(type_Alias_তথ্য_Entry.getAlias(), &toLen, &sts);
                        যদি (U_FAILURE(sts)) {
                            বিরতি;
                        }
                        // check if this is an alias of canonical legacy type
                        যদি (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.তথ্য(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ধ্রুবক অক্ষর* from = ures_get_চাবি(type_Alias_তথ্য_Entry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            যদি (isTZ) {
                                // replace colon with slash if necessary
                                যদি (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_চাবি_Type_String_Pool->create(fromView);
                                    যদি (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        বিরতি;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->তথ্য(), fromView.length()};
                                }
                            }
                            uhash_put(type_তথ্য_Map, &alias->from, t, &sts);
                        }
                    }
                    যদি (U_FAILURE(sts)) {
                        বিরতি;
                    }
                }

                যদি (bcp_Type_Alias_Res_By_চাবি.isValid()) {
                    LocalUResourceBundlePointer bcp_Type_Alias_তথ্য_Entry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_চাবি.getAlias());
                    যখন (ures_has_পরবর্তী(bcp_Type_Alias_Res_By_চাবি.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcp_Type_Alias_তথ্য_Entry.adoptInstead(ures_get_পরবর্তী_Resource(bcp_Type_Alias_Res_By_চাবি.getAlias(), bcp_Type_Alias_তথ্য_Entry.orphan(), &sts));
                        ধ্রুবক char16_t* to = ures_getString(bcp_Type_Alias_তথ্য_Entry.getAlias(), &toLen, &sts);
                        যদি (U_FAILURE(sts)) {
                            বিরতি;
                        }
                        // check if this is an alias of bcp type
                        যদি (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.তথ্য(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ধ্রুবক অক্ষর* from = ures_get_চাবি(bcp_Type_Alias_তথ্য_Entry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_তথ্য_Map, &alias->from, t, &sts);
                        }
                    }
                    যদি (U_FAILURE(sts)) {
                        বিরতি;
                    }
                }
            }
        }
        যদি (U_FAILURE(sts)) {
            বিরতি;
        }

        Loc_Ext_চাবি_তথ্য* চাবি_তথ্য = g_Loc_Ext_চাবি_তথ্য_Entries->create();
        যদি (চাবি_তথ্য == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            বিরতি;
        }
        চাবি_তথ্য->bcpId = bcp_চাবি_Id;
        চাবি_তথ্য->legacyId = legacy_চাবি_Id;
        চাবি_তথ্য->specialTypes = specialTypes;
        চাবি_তথ্য->typeMap.adoptInstead(type_তথ্য_Map);

        uhash_put(g_Loc_Ext_চাবি_Map, &চাবি_তথ্য->legacyId, চাবি_তথ্য, &sts);
        যদি (legacy_চাবি_Id != bcp_চাবি_Id) {
            // different key value
            uhash_put(g_Loc_Ext_চাবি_Map, &চাবি_তথ্য->bcpId, চাবি_তথ্য, &sts);
        }
        যদি (U_FAILURE(sts)) {
            বিরতি;
        }
    }
}

যৌক্তিক
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_চাবি_Map_Init_Once, &initFromResourceBundle, sts);
    যদি (U_FAILURE(sts)) {
        ফেরত মিথ্যা;
    }
    ফেরত সত্য;
}

যৌক্তিক
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    জন্য (অক্ষর c : val) {
        যদি (c == '-') {
            যদি (subtagLen < 4 || subtagLen > 6) {
                ফেরত মিথ্যা;
            }
            subtagLen = 0;
        } অন্যথা যদি ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } অন্যথা {
            ফেরত মিথ্যা;
        }
    }
    ফেরত subtagLen >= 4 && subtagLen <= 6;
}

যৌক্তিক
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    জন্য (অক্ষর c : val) {
        যদি (c == '-') {
            যদি (subtagLen < 3 || subtagLen > 8) {
                ফেরত মিথ্যা;
            }
            subtagLen = 0;
        } অন্যথা যদি (uprv_isASCIILetter(c)) {
            subtagLen++;
        } অন্যথা {
            ফেরত মিথ্যা;
        }
    }
    ফেরত subtagLen >= 3 && subtagLen <= 8;
}

যৌক্তিক
is_Special_Type_Rg_চাবি_মান(std::string_view val) {
    int32_t subtagLen = 0;
    জন্য (অক্ষর c : val) {
        যদি ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } অন্যথা {
            ফেরত মিথ্যা;
        }
    }
    ফেরত subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_চাবি(std::string_view চাবি) {
    যদি (!init()) {
        ফেরত std::nullopt;
    }

    Loc_Ext_চাবি_তথ্য* চাবি_তথ্য = static_cast<Loc_Ext_চাবি_তথ্য*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_তথ্য != nullptr) {
        ফেরত চাবি_তথ্য->bcpId;
    }

    ফেরত std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_চাবি(std::string_view চাবি) {
    যদি (!init()) {
        ফেরত std::nullopt;
    }

    Loc_Ext_চাবি_তথ্য* চাবি_তথ্য = static_cast<Loc_Ext_চাবি_তথ্য*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_তথ্য != nullptr) {
        ফেরত চাবি_তথ্য->legacyId;
    }

    ফেরত std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view চাবি, std::string_view type U_LIFETIME_BOUND) {
    যদি (!init()) {
        ফেরত std::nullopt;
    }

    Loc_Ext_চাবি_তথ্য* চাবি_তথ্য = static_cast<Loc_Ext_চাবি_তথ্য*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_তথ্য != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(চাবি_তথ্য->typeMap.getAlias(), &type));
        যদি (t != nullptr) {
            ফেরত t->bcpId;
        }
        যদি (চাবি_তথ্য->specialTypes != SPECIALTYPE_NONE) {
            যৌক্তিক matched = মিথ্যা;
            যদি (চাবি_তথ্য->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            যদি (!matched && চাবি_তথ্য->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            যদি (!matched && চাবি_তথ্য->specialTypes & SPECIALTYPE_RG_চাবি_মান) {
                matched = is_Special_Type_Rg_চাবি_মান(type);
            }
            যদি (matched) {
                ফেরত type;
            }
        }
    }

    ফেরত std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view চাবি, std::string_view type U_LIFETIME_BOUND) {
    যদি (!init()) {
        ফেরত std::nullopt;
    }

    Loc_Ext_চাবি_তথ্য* চাবি_তথ্য = static_cast<Loc_Ext_চাবি_তথ্য*>(uhash_get(g_Loc_Ext_চাবি_Map, &চাবি));
    যদি (চাবি_তথ্য != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(চাবি_তথ্য->typeMap.getAlias(), &type));
        যদি (t != nullptr) {
            ফেরত t->legacyId;
        }
        যদি (চাবি_তথ্য->specialTypes != SPECIALTYPE_NONE) {
            যৌক্তিক matched = মিথ্যা;
            যদি (চাবি_তথ্য->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            যদি (!matched && চাবি_তথ্য->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            যদি (!matched && চাবি_তথ্য->specialTypes & SPECIALTYPE_RG_চাবি_মান) {
                matched = is_Special_Type_Rg_চাবি_মান(type);
            }
            যদি (matched) {
                ফেরত type;
            }
        }
    }

    ফেরত std::nullopt;
}
