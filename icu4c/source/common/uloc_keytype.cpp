// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#समावेश <algorithm>
#समावेश <optional>
#समावेश <string_view>

#समावेश "unicode/utypes.h"
#समावेश "unicode/unistr.h"
#समावेश "unicode/uobject.h"

#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "fixedstring.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"
#समावेश "uhash.h"
#समावेश "ulocimp.h"
#समावेश "umutex.h"
#समावेश "uresimp.h"
#समावेश "uvector.h"
#समावेश "udataswp.h" /* for InvChar functions */

अटल UHashtable* g_Loc_Ext_कुंजी_Map = nullptr;
अटल icu::UInitOnce g_Loc_Ext_कुंजी_Map_Init_Once {};

// bit flags for special types
प्रकार क्रमागत {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_कुंजी_मूल्य = 4
} SpecialType;

काष्ठा Loc_Ext_कुंजी_तथ्य : खुला icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

काष्ठा LocExtType : खुला icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

काष्ठा TypeAlias : खुला icu::UMemory {
    std::string_view from;
};

अटल icu::MemoryPool<icu::FixedString>* g_कुंजी_Type_String_Pool = nullptr;
अटल icu::MemoryPool<Loc_Ext_कुंजी_तथ्य>* g_Loc_Ext_कुंजी_तथ्य_Entries = nullptr;
अटल icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
अटल icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

अटल UBool U_CALLCONV
uloc_कुंजी_type_cleanup() {
    अगर (g_Loc_Ext_कुंजी_Map != nullptr) {
        uhash_close(g_Loc_Ext_कुंजी_Map);
        g_Loc_Ext_कुंजी_Map = nullptr;
    }

    मिटाओ g_Loc_Ext_कुंजी_तथ्य_Entries;
    g_Loc_Ext_कुंजी_तथ्य_Entries = nullptr;

    मिटाओ gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    मिटाओ gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    मिटाओ g_कुंजी_Type_String_Pool;
    g_कुंजी_Type_String_Pool = nullptr;

    g_Loc_Ext_कुंजी_Map_Init_Once.reset();
    वापस true;
}

U_CDECL_END

namespace {

व्योम U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_कुंजी_TYPE, uloc_कुंजी_type_cleanup);

    g_Loc_Ext_कुंजी_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer कुंजी_Type_तथ्य_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer कुंजी_Map_Res(ures_get_By_कुंजी(कुंजी_Type_तथ्य_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_कुंजी(कुंजी_Type_तथ्य_Res.getAlias(), "typeMap", nullptr, &sts));

    अगर (U_FAILURE(sts)) {
        वापस;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_कुंजी(कुंजी_Type_तथ्य_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_कुंजी(कुंजी_Type_तथ्य_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_कुंजी_Type_String_Pool = नया icu::MemoryPool<icu::FixedString>;
    अगर (g_कुंजी_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    g_Loc_Ext_कुंजी_तथ्य_Entries = नया icu::MemoryPool<Loc_Ext_कुंजी_तथ्य>;
    अगर (g_Loc_Ext_कुंजी_तथ्य_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    gLocExtTypeEntries = नया icu::MemoryPool<LocExtType>;
    अगर (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
    gTypeAliasEntries = नया icu::MemoryPool<TypeAlias>;
    अगर (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer कुंजी_Map_Entry;

    जबतक (ures_has_अग्र(कुंजी_Map_Res.getAlias())) {
        कुंजी_Map_Entry.adoptInstead(ures_get_अग्र_Resource(कुंजी_Map_Res.getAlias(), कुंजी_Map_Entry.orphan(), &sts));
        अगर (U_FAILURE(sts)) {
            अवरोध;
        }
        स्थिर अक्षर* legacy_कुंजी_Id = ures_get_कुंजी(कुंजी_Map_Entry.getAlias());
        UnicodeString u_Bcp_कुंजी_Id = ures_getUnicodeString(कुंजी_Map_Entry.getAlias(), &sts);
        अगर (U_FAILURE(sts)) {
            अवरोध;
        }

        // empty value indicates that BCP key is same with the legacy key.
        स्थिर अक्षर* bcp_कुंजी_Id = legacy_कुंजी_Id;
        अगर (!u_Bcp_कुंजी_Id.isEmpty()) {
            icu::FixedString* bcp_कुंजी_Id_Buf = g_कुंजी_Type_String_Pool->create();
            अगर (bcp_कुंजी_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                अवरोध;
            }
            copyInvariantChars(u_Bcp_कुंजी_Id, *bcp_कुंजी_Id_Buf, sts);
            अगर (U_FAILURE(sts)) {
                अवरोध;
            }
            bcp_कुंजी_Id = bcp_कुंजी_Id_Buf->तथ्य();
        }

        bool isTZ = uprv_strcmp(legacy_कुंजी_Id, "timezone") == 0;

        UHashtable* type_तथ्य_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        अगर (U_FAILURE(sts)) {
            अवरोध;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_कुंजी;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_कुंजी;

        अगर (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_कुंजी.adoptInstead(ures_get_By_कुंजी(typeAliasRes.getAlias(), legacy_कुंजी_Id, nullptr, &tmpSts));
            अगर (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_कुंजी.orphan();
            }
        }
        अगर (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_कुंजी.adoptInstead(ures_get_By_कुंजी(bcpTypeAliasRes.getAlias(), bcp_कुंजी_Id, nullptr, &tmpSts));
            अगर (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_कुंजी.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_कुंजी(ures_get_By_कुंजी(typeMapRes.getAlias(), legacy_कुंजी_Id, nullptr, &sts));
        अगर (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } अन्यथा {
            LocalUResourceBundlePointer typeMapEntry;

            जबतक (ures_has_अग्र(type_Map_Res_By_कुंजी.getAlias())) {
                typeMapEntry.adoptInstead(ures_get_अग्र_Resource(type_Map_Res_By_कुंजी.getAlias(), typeMapEntry.orphan(), &sts));
                अगर (U_FAILURE(sts)) {
                    अवरोध;
                }
                स्थिर अक्षर* legacyTypeId = ures_get_कुंजी(typeMapEntry.getAlias());

                // special types
                अगर (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    जारी;
                }
                अगर (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    जारी;
                }
                अगर (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_कुंजी_मूल्य;
                    जारी;
                }

                अगर (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    अगर (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_कुंजी_Type_String_Pool->create(legacyTypeIdView);
                        अगर (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            अवरोध;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->तथ्य();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                अगर (U_FAILURE(sts)) {
                    अवरोध;
                }

                // empty value indicates that BCP type is same with the legacy type.
                स्थिर अक्षर* bcpTypeId = legacyTypeId;
                अगर (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_कुंजी_Type_String_Pool->create();
                    अगर (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        अवरोध;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    अगर (U_FAILURE(sts)) {
                        अवरोध;
                    }
                    bcpTypeId = bcpTypeIdBuf->तथ्य();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                अगर (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    अवरोध;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_तथ्य_Map, &t->legacyId, t, &sts);
                अगर (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_तथ्य_Map, &t->bcpId, t, &sts);
                }
                अगर (U_FAILURE(sts)) {
                    अवरोध;
                }

                // also put aliases in the map
                अगर (type_Alias_Res_By_कुंजी.isValid()) {
                    LocalUResourceBundlePointer type_Alias_तथ्य_Entry;

                    ures_resetIterator(type_Alias_Res_By_कुंजी.getAlias());
                    जबतक (ures_has_अग्र(type_Alias_Res_By_कुंजी.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        type_Alias_तथ्य_Entry.adoptInstead(ures_get_अग्र_Resource(type_Alias_Res_By_कुंजी.getAlias(), type_Alias_तथ्य_Entry.orphan(), &sts));
                        स्थिर char16_t* to = ures_getString(type_Alias_तथ्य_Entry.getAlias(), &toLen, &sts);
                        अगर (U_FAILURE(sts)) {
                            अवरोध;
                        }
                        // check if this is an alias of canonical legacy type
                        अगर (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.तथ्य(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            स्थिर अक्षर* from = ures_get_कुंजी(type_Alias_तथ्य_Entry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            अगर (isTZ) {
                                // replace colon with slash if necessary
                                अगर (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_कुंजी_Type_String_Pool->create(fromView);
                                    अगर (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        अवरोध;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->तथ्य(), fromView.length()};
                                }
                            }
                            uhash_put(type_तथ्य_Map, &alias->from, t, &sts);
                        }
                    }
                    अगर (U_FAILURE(sts)) {
                        अवरोध;
                    }
                }

                अगर (bcp_Type_Alias_Res_By_कुंजी.isValid()) {
                    LocalUResourceBundlePointer bcp_Type_Alias_तथ्य_Entry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_कुंजी.getAlias());
                    जबतक (ures_has_अग्र(bcp_Type_Alias_Res_By_कुंजी.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcp_Type_Alias_तथ्य_Entry.adoptInstead(ures_get_अग्र_Resource(bcp_Type_Alias_Res_By_कुंजी.getAlias(), bcp_Type_Alias_तथ्य_Entry.orphan(), &sts));
                        स्थिर char16_t* to = ures_getString(bcp_Type_Alias_तथ्य_Entry.getAlias(), &toLen, &sts);
                        अगर (U_FAILURE(sts)) {
                            अवरोध;
                        }
                        // check if this is an alias of bcp type
                        अगर (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.तथ्य(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            स्थिर अक्षर* from = ures_get_कुंजी(bcp_Type_Alias_तथ्य_Entry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_तथ्य_Map, &alias->from, t, &sts);
                        }
                    }
                    अगर (U_FAILURE(sts)) {
                        अवरोध;
                    }
                }
            }
        }
        अगर (U_FAILURE(sts)) {
            अवरोध;
        }

        Loc_Ext_कुंजी_तथ्य* कुंजी_तथ्य = g_Loc_Ext_कुंजी_तथ्य_Entries->create();
        अगर (कुंजी_तथ्य == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            अवरोध;
        }
        कुंजी_तथ्य->bcpId = bcp_कुंजी_Id;
        कुंजी_तथ्य->legacyId = legacy_कुंजी_Id;
        कुंजी_तथ्य->specialTypes = specialTypes;
        कुंजी_तथ्य->typeMap.adoptInstead(type_तथ्य_Map);

        uhash_put(g_Loc_Ext_कुंजी_Map, &कुंजी_तथ्य->legacyId, कुंजी_तथ्य, &sts);
        अगर (legacy_कुंजी_Id != bcp_कुंजी_Id) {
            // different key value
            uhash_put(g_Loc_Ext_कुंजी_Map, &कुंजी_तथ्य->bcpId, कुंजी_तथ्य, &sts);
        }
        अगर (U_FAILURE(sts)) {
            अवरोध;
        }
    }
}

bool
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_कुंजी_Map_Init_Once, &initFromResourceBundle, sts);
    अगर (U_FAILURE(sts)) {
        वापस false;
    }
    वापस true;
}

bool
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    क्रम (अक्षर c : val) {
        अगर (c == '-') {
            अगर (subtagLen < 4 || subtagLen > 6) {
                वापस false;
            }
            subtagLen = 0;
        } अन्यथा अगर ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } अन्यथा {
            वापस false;
        }
    }
    वापस subtagLen >= 4 && subtagLen <= 6;
}

bool
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    क्रम (अक्षर c : val) {
        अगर (c == '-') {
            अगर (subtagLen < 3 || subtagLen > 8) {
                वापस false;
            }
            subtagLen = 0;
        } अन्यथा अगर (uprv_isASCIILetter(c)) {
            subtagLen++;
        } अन्यथा {
            वापस false;
        }
    }
    वापस subtagLen >= 3 && subtagLen <= 8;
}

bool
is_Special_Type_Rg_कुंजी_मूल्य(std::string_view val) {
    int32_t subtagLen = 0;
    क्रम (अक्षर c : val) {
        अगर ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } अन्यथा {
            वापस false;
        }
    }
    वापस subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_कुंजी(std::string_view कुंजी) {
    अगर (!init()) {
        वापस std::nullopt;
    }

    Loc_Ext_कुंजी_तथ्य* कुंजी_तथ्य = static_cast<Loc_Ext_कुंजी_तथ्य*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    अगर (कुंजी_तथ्य != nullptr) {
        वापस कुंजी_तथ्य->bcpId;
    }

    वापस std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_कुंजी(std::string_view कुंजी) {
    अगर (!init()) {
        वापस std::nullopt;
    }

    Loc_Ext_कुंजी_तथ्य* कुंजी_तथ्य = static_cast<Loc_Ext_कुंजी_तथ्य*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    अगर (कुंजी_तथ्य != nullptr) {
        वापस कुंजी_तथ्य->legacyId;
    }

    वापस std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view कुंजी, std::string_view type U_LIFETIME_BOUND) {
    अगर (!init()) {
        वापस std::nullopt;
    }

    Loc_Ext_कुंजी_तथ्य* कुंजी_तथ्य = static_cast<Loc_Ext_कुंजी_तथ्य*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    अगर (कुंजी_तथ्य != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुंजी_तथ्य->typeMap.getAlias(), &type));
        अगर (t != nullptr) {
            वापस t->bcpId;
        }
        अगर (कुंजी_तथ्य->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            अगर (कुंजी_तथ्य->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            अगर (!matched && कुंजी_तथ्य->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            अगर (!matched && कुंजी_तथ्य->specialTypes & SPECIALTYPE_RG_कुंजी_मूल्य) {
                matched = is_Special_Type_Rg_कुंजी_मूल्य(type);
            }
            अगर (matched) {
                वापस type;
            }
        }
    }

    वापस std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view कुंजी, std::string_view type U_LIFETIME_BOUND) {
    अगर (!init()) {
        वापस std::nullopt;
    }

    Loc_Ext_कुंजी_तथ्य* कुंजी_तथ्य = static_cast<Loc_Ext_कुंजी_तथ्य*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    अगर (कुंजी_तथ्य != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुंजी_तथ्य->typeMap.getAlias(), &type));
        अगर (t != nullptr) {
            वापस t->legacyId;
        }
        अगर (कुंजी_तथ्य->specialTypes != SPECIALTYPE_NONE) {
            bool matched = false;
            अगर (कुंजी_तथ्य->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            अगर (!matched && कुंजी_तथ्य->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            अगर (!matched && कुंजी_तथ्य->specialTypes & SPECIALTYPE_RG_कुंजी_मूल्य) {
                matched = is_Special_Type_Rg_कुंजी_मूल्य(type);
            }
            अगर (matched) {
                वापस type;
            }
        }
    }

    वापस std::nullopt;
}
