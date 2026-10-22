// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#അന്തര്ഭാവ <algorithm>
#അന്തര്ഭാവ <optional>
#അന്തര്ഭാവ <string_view>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/uobject.h"

#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "fixedstring.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "uhash.h"
#അന്തര്ഭാവ "ulocimp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uresimp.h"
#അന്തര്ഭാവ "uvector.h"
#അന്തര്ഭാവ "udataswp.h" /* for InvChar functions */

സ്ഥിതം UHashtable* g_Loc_Ext_താക്കോൽ_Map = nullptr;
സ്ഥിതം icu::UInitOnce g_Loc_Ext_താക്കോൽ_Map_Init_Once {};

// bit flags for special types
തരനിർവചനം എണ്ണൽ {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_താക്കോൽ_മൂല്യം = 4
} SpecialType;

ഘടന Loc_Ext_താക്കോൽ_Data : പൊതു icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

ഘടന LocExtType : പൊതു icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ഘടന TypeAlias : പൊതു icu::UMemory {
    std::string_view from;
};

സ്ഥിതം icu::MemoryPool<icu::FixedString>* g_താക്കോൽ_Type_String_Pool = nullptr;
സ്ഥിതം icu::MemoryPool<Loc_Ext_താക്കോൽ_Data>* g_Loc_Ext_താക്കോൽ_Data_Entries = nullptr;
സ്ഥിതം icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
സ്ഥിതം icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

സ്ഥിതം UBool U_CALLCONV
uloc_താക്കോൽ_type_cleanup() {
    എങ്കിൽ (g_Loc_Ext_താക്കോൽ_Map != nullptr) {
        uhash_close(g_Loc_Ext_താക്കോൽ_Map);
        g_Loc_Ext_താക്കോൽ_Map = nullptr;
    }

    delete g_Loc_Ext_താക്കോൽ_Data_Entries;
    g_Loc_Ext_താക്കോൽ_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_താക്കോൽ_Type_String_Pool;
    g_താക്കോൽ_Type_String_Pool = nullptr;

    g_Loc_Ext_താക്കോൽ_Map_Init_Once.reset();
    തിരികെ സത്യം;
}

U_CDECL_END

നാമാകാശ {

ശൂന്യം U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_താക്കോൽ_TYPE, uloc_താക്കോൽ_type_cleanup);

    g_Loc_Ext_താക്കോൽ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer താക്കോൽ_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer താക്കോൽ_Map_Res(ures_get_By_താക്കോൽ(താക്കോൽ_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_താക്കോൽ(താക്കോൽ_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    എങ്കിൽ (U_FAILURE(sts)) {
        തിരികെ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_താക്കോൽ(താക്കോൽ_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_താക്കോൽ(താക്കോൽ_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_താക്കോൽ_Type_String_Pool = പുതിയ icu::MemoryPool<icu::FixedString>;
    എങ്കിൽ (g_താക്കോൽ_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    g_Loc_Ext_താക്കോൽ_Data_Entries = പുതിയ icu::MemoryPool<Loc_Ext_താക്കോൽ_Data>;
    എങ്കിൽ (g_Loc_Ext_താക്കോൽ_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    gLocExtTypeEntries = പുതിയ icu::MemoryPool<LocExtType>;
    എങ്കിൽ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
    gTypeAliasEntries = പുതിയ icu::MemoryPool<TypeAlias>;
    എങ്കിൽ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer താക്കോൽ_Map_Entry;

    ആകുമ്പോൾ (ures_hasNext(താക്കോൽ_Map_Res.getAlias())) {
        താക്കോൽ_Map_Entry.adoptInstead(ures_getNextResource(താക്കോൽ_Map_Res.getAlias(), താക്കോൽ_Map_Entry.orphan(), &sts));
        എങ്കിൽ (U_FAILURE(sts)) {
            നിർത്തുക;
        }
        സ്ഥിരം അക്ഷരം* legacy_താക്കോൽ_Id = ures_get_താക്കോൽ(താക്കോൽ_Map_Entry.getAlias());
        UnicodeString u_Bcp_താക്കോൽ_Id = ures_getUnicodeString(താക്കോൽ_Map_Entry.getAlias(), &sts);
        എങ്കിൽ (U_FAILURE(sts)) {
            നിർത്തുക;
        }

        // empty value indicates that BCP key is same with the legacy key.
        സ്ഥിരം അക്ഷരം* bcp_താക്കോൽ_Id = legacy_താക്കോൽ_Id;
        എങ്കിൽ (!u_Bcp_താക്കോൽ_Id.isEmpty()) {
            icu::FixedString* bcp_താക്കോൽ_Id_Buf = g_താക്കോൽ_Type_String_Pool->create();
            എങ്കിൽ (bcp_താക്കോൽ_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                നിർത്തുക;
            }
            copyInvariantChars(u_Bcp_താക്കോൽ_Id, *bcp_താക്കോൽ_Id_Buf, sts);
            എങ്കിൽ (U_FAILURE(sts)) {
                നിർത്തുക;
            }
            bcp_താക്കോൽ_Id = bcp_താക്കോൽ_Id_Buf->data();
        }

        തർക്കം isTZ = uprv_strcmp(legacy_താക്കോൽ_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        എങ്കിൽ (U_FAILURE(sts)) {
            നിർത്തുക;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_താക്കോൽ;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_താക്കോൽ;

        എങ്കിൽ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_താക്കോൽ.adoptInstead(ures_get_By_താക്കോൽ(typeAliasRes.getAlias(), legacy_താക്കോൽ_Id, nullptr, &tmpSts));
            എങ്കിൽ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_താക്കോൽ.orphan();
            }
        }
        എങ്കിൽ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_താക്കോൽ.adoptInstead(ures_get_By_താക്കോൽ(bcpTypeAliasRes.getAlias(), bcp_താക്കോൽ_Id, nullptr, &tmpSts));
            എങ്കിൽ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_താക്കോൽ.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_താക്കോൽ(ures_get_By_താക്കോൽ(typeMapRes.getAlias(), legacy_താക്കോൽ_Id, nullptr, &sts));
        എങ്കിൽ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } അല്ലെങ്കിൽ {
            LocalUResourceBundlePointer typeMapEntry;

            ആകുമ്പോൾ (ures_hasNext(type_Map_Res_By_താക്കോൽ.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_താക്കോൽ.getAlias(), typeMapEntry.orphan(), &sts));
                എങ്കിൽ (U_FAILURE(sts)) {
                    നിർത്തുക;
                }
                സ്ഥിരം അക്ഷരം* legacyTypeId = ures_get_താക്കോൽ(typeMapEntry.getAlias());

                // special types
                എങ്കിൽ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    തുടരുക;
                }
                എങ്കിൽ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    തുടരുക;
                }
                എങ്കിൽ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_താക്കോൽ_മൂല്യം;
                    തുടരുക;
                }

                എങ്കിൽ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    എങ്കിൽ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_താക്കോൽ_Type_String_Pool->create(legacyTypeIdView);
                        എങ്കിൽ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            നിർത്തുക;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                എങ്കിൽ (U_FAILURE(sts)) {
                    നിർത്തുക;
                }

                // empty value indicates that BCP type is same with the legacy type.
                സ്ഥിരം അക്ഷരം* bcpTypeId = legacyTypeId;
                എങ്കിൽ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_താക്കോൽ_Type_String_Pool->create();
                    എങ്കിൽ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        നിർത്തുക;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    എങ്കിൽ (U_FAILURE(sts)) {
                        നിർത്തുക;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                എങ്കിൽ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    നിർത്തുക;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                എങ്കിൽ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                എങ്കിൽ (U_FAILURE(sts)) {
                    നിർത്തുക;
                }

                // also put aliases in the map
                എങ്കിൽ (type_Alias_Res_By_താക്കോൽ.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_താക്കോൽ.getAlias());
                    ആകുമ്പോൾ (ures_hasNext(type_Alias_Res_By_താക്കോൽ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_താക്കോൽ.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        സ്ഥിരം char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        എങ്കിൽ (U_FAILURE(sts)) {
                            നിർത്തുക;
                        }
                        // check if this is an alias of canonical legacy type
                        എങ്കിൽ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            സ്ഥിരം അക്ഷരം* from = ures_get_താക്കോൽ(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            എങ്കിൽ (isTZ) {
                                // replace colon with slash if necessary
                                എങ്കിൽ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_താക്കോൽ_Type_String_Pool->create(fromView);
                                    എങ്കിൽ (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        നിർത്തുക;
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
                    എങ്കിൽ (U_FAILURE(sts)) {
                        നിർത്തുക;
                    }
                }

                എങ്കിൽ (bcp_Type_Alias_Res_By_താക്കോൽ.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_താക്കോൽ.getAlias());
                    ആകുമ്പോൾ (ures_hasNext(bcp_Type_Alias_Res_By_താക്കോൽ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_താക്കോൽ.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        സ്ഥിരം char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        എങ്കിൽ (U_FAILURE(sts)) {
                            നിർത്തുക;
                        }
                        // check if this is an alias of bcp type
                        എങ്കിൽ (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            സ്ഥിരം അക്ഷരം* from = ures_get_താക്കോൽ(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    എങ്കിൽ (U_FAILURE(sts)) {
                        നിർത്തുക;
                    }
                }
            }
        }
        എങ്കിൽ (U_FAILURE(sts)) {
            നിർത്തുക;
        }

        Loc_Ext_താക്കോൽ_Data* താക്കോൽ_Data = g_Loc_Ext_താക്കോൽ_Data_Entries->create();
        എങ്കിൽ (താക്കോൽ_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            നിർത്തുക;
        }
        താക്കോൽ_Data->bcpId = bcp_താക്കോൽ_Id;
        താക്കോൽ_Data->legacyId = legacy_താക്കോൽ_Id;
        താക്കോൽ_Data->specialTypes = specialTypes;
        താക്കോൽ_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ_Data->legacyId, താക്കോൽ_Data, &sts);
        എങ്കിൽ (legacy_താക്കോൽ_Id != bcp_താക്കോൽ_Id) {
            // different key value
            uhash_put(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ_Data->bcpId, താക്കോൽ_Data, &sts);
        }
        എങ്കിൽ (U_FAILURE(sts)) {
            നിർത്തുക;
        }
    }
}

തർക്കം
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_താക്കോൽ_Map_Init_Once, &initFromResourceBundle, sts);
    എങ്കിൽ (U_FAILURE(sts)) {
        തിരികെ അസത്യം;
    }
    തിരികെ സത്യം;
}

തർക്കം
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    വേണ്ടി (അക്ഷരം c : val) {
        എങ്കിൽ (c == '-') {
            എങ്കിൽ (subtagLen < 4 || subtagLen > 6) {
                തിരികെ അസത്യം;
            }
            subtagLen = 0;
        } അല്ലെങ്കിൽ എങ്കിൽ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } അല്ലെങ്കിൽ {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ subtagLen >= 4 && subtagLen <= 6;
}

തർക്കം
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    വേണ്ടി (അക്ഷരം c : val) {
        എങ്കിൽ (c == '-') {
            എങ്കിൽ (subtagLen < 3 || subtagLen > 8) {
                തിരികെ അസത്യം;
            }
            subtagLen = 0;
        } അല്ലെങ്കിൽ എങ്കിൽ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } അല്ലെങ്കിൽ {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ subtagLen >= 3 && subtagLen <= 8;
}

തർക്കം
is_Special_Type_Rg_താക്കോൽ_മൂല്യം(std::string_view val) {
    int32_t subtagLen = 0;
    വേണ്ടി (അക്ഷരം c : val) {
        എങ്കിൽ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } അല്ലെങ്കിൽ {
            തിരികെ അസത്യം;
        }
    }
    തിരികെ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_താക്കോൽ(std::string_view താക്കോൽ) {
    എങ്കിൽ (!init()) {
        തിരികെ std::nullopt;
    }

    Loc_Ext_താക്കോൽ_Data* താക്കോൽ_Data = static_cast<Loc_Ext_താക്കോൽ_Data*>(uhash_get(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ));
    എങ്കിൽ (താക്കോൽ_Data != nullptr) {
        തിരികെ താക്കോൽ_Data->bcpId;
    }

    തിരികെ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_താക്കോൽ(std::string_view താക്കോൽ) {
    എങ്കിൽ (!init()) {
        തിരികെ std::nullopt;
    }

    Loc_Ext_താക്കോൽ_Data* താക്കോൽ_Data = static_cast<Loc_Ext_താക്കോൽ_Data*>(uhash_get(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ));
    എങ്കിൽ (താക്കോൽ_Data != nullptr) {
        തിരികെ താക്കോൽ_Data->legacyId;
    }

    തിരികെ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view താക്കോൽ, std::string_view type U_LIFETIME_BOUND) {
    എങ്കിൽ (!init()) {
        തിരികെ std::nullopt;
    }

    Loc_Ext_താക്കോൽ_Data* താക്കോൽ_Data = static_cast<Loc_Ext_താക്കോൽ_Data*>(uhash_get(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ));
    എങ്കിൽ (താക്കോൽ_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(താക്കോൽ_Data->typeMap.getAlias(), &type));
        എങ്കിൽ (t != nullptr) {
            തിരികെ t->bcpId;
        }
        എങ്കിൽ (താക്കോൽ_Data->specialTypes != SPECIALTYPE_NONE) {
            തർക്കം matched = അസത്യം;
            എങ്കിൽ (താക്കോൽ_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            എങ്കിൽ (!matched && താക്കോൽ_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            എങ്കിൽ (!matched && താക്കോൽ_Data->specialTypes & SPECIALTYPE_RG_താക്കോൽ_മൂല്യം) {
                matched = is_Special_Type_Rg_താക്കോൽ_മൂല്യം(type);
            }
            എങ്കിൽ (matched) {
                തിരികെ type;
            }
        }
    }

    തിരികെ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view താക്കോൽ, std::string_view type U_LIFETIME_BOUND) {
    എങ്കിൽ (!init()) {
        തിരികെ std::nullopt;
    }

    Loc_Ext_താക്കോൽ_Data* താക്കോൽ_Data = static_cast<Loc_Ext_താക്കോൽ_Data*>(uhash_get(g_Loc_Ext_താക്കോൽ_Map, &താക്കോൽ));
    എങ്കിൽ (താക്കോൽ_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(താക്കോൽ_Data->typeMap.getAlias(), &type));
        എങ്കിൽ (t != nullptr) {
            തിരികെ t->legacyId;
        }
        എങ്കിൽ (താക്കോൽ_Data->specialTypes != SPECIALTYPE_NONE) {
            തർക്കം matched = അസത്യം;
            എങ്കിൽ (താക്കോൽ_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            എങ്കിൽ (!matched && താക്കോൽ_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            എങ്കിൽ (!matched && താക്കോൽ_Data->specialTypes & SPECIALTYPE_RG_താക്കോൽ_മൂല്യം) {
                matched = is_Special_Type_Rg_താക്കോൽ_മൂല്യം(type);
            }
            എങ്കിൽ (matched) {
                തിരികെ type;
            }
        }
    }

    തിരികെ std::nullopt;
}
