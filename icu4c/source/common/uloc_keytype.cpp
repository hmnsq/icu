// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#अन्तर्भाव <algorithm>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "fixedstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "udataswp.h" /* for InvChar functions */

स्थिर UHashtable* g_Loc_Ext_कुञ्चिका_Map = nullptr;
स्थिर icu::UInitOnce g_Loc_Ext_कुञ्चिका_Map_Init_Once {};

// bit flags for special types
प्रकारनिर्देश गणना {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_कुञ्चिका_मानम् = 4
} SpecialType;

संरचना Loc_Ext_कुञ्चिका_Data : सार्वजनिक icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

संरचना LocExtType : सार्वजनिक icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

संरचना TypeAlias : सार्वजनिक icu::UMemory {
    std::string_view from;
};

स्थिर icu::MemoryPool<icu::FixedString>* g_कुञ्चिका_Type_String_Pool = nullptr;
स्थिर icu::MemoryPool<Loc_Ext_कुञ्चिका_Data>* g_Loc_Ext_कुञ्चिका_Data_Entries = nullptr;
स्थिर icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
स्थिर icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

स्थिर UBool U_CALLCONV
uloc_कुञ्चिका_type_cleanup() {
    जइ (g_Loc_Ext_कुञ्चिका_Map != nullptr) {
        uhash_close(g_Loc_Ext_कुञ्चिका_Map);
        g_Loc_Ext_कुञ्चिका_Map = nullptr;
    }

    delete g_Loc_Ext_कुञ्चिका_Data_Entries;
    g_Loc_Ext_कुञ्चिका_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_कुञ्चिका_Type_String_Pool;
    g_कुञ्चिका_Type_String_Pool = nullptr;

    g_Loc_Ext_कुञ्चिका_Map_Init_Once.reset();
    प्रत्यागम सच्च;
}

U_CDECL_END

नामाकाश {

शून्य U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_कुञ्चिका_TYPE, uloc_कुञ्चिका_type_cleanup);

    g_Loc_Ext_कुञ्चिका_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer कुञ्चिका_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer कुञ्चिका_Map_Res(ures_get_By_कुञ्चिका(कुञ्चिका_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    जइ (U_FAILURE(sts)) {
        प्रत्यागम;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_कुञ्चिका_Type_String_Pool = नूतन icu::MemoryPool<icu::FixedString>;
    जइ (g_कुञ्चिका_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    g_Loc_Ext_कुञ्चिका_Data_Entries = नूतन icu::MemoryPool<Loc_Ext_कुञ्चिका_Data>;
    जइ (g_Loc_Ext_कुञ्चिका_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    gLocExtTypeEntries = नूतन icu::MemoryPool<LocExtType>;
    जइ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    gTypeAliasEntries = नूतन icu::MemoryPool<TypeAlias>;
    जइ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer कुञ्चिका_Map_Entry;

    जाव (ures_hasNext(कुञ्चिका_Map_Res.getAlias())) {
        कुञ्चिका_Map_Entry.adoptInstead(ures_getNextResource(कुञ्चिका_Map_Res.getAlias(), कुञ्चिका_Map_Entry.orphan(), &sts));
        जइ (U_FAILURE(sts)) {
            भङ्ग;
        }
        ध्रुव वर्ण* legacy_कुञ्चिका_Id = ures_get_कुञ्चिका(कुञ्चिका_Map_Entry.getAlias());
        UnicodeString u_Bcp_कुञ्चिका_Id = ures_getUnicodeString(कुञ्चिका_Map_Entry.getAlias(), &sts);
        जइ (U_FAILURE(sts)) {
            भङ्ग;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ध्रुव वर्ण* bcp_कुञ्चिका_Id = legacy_कुञ्चिका_Id;
        जइ (!u_Bcp_कुञ्चिका_Id.isEmpty()) {
            icu::FixedString* bcp_कुञ्चिका_Id_Buf = g_कुञ्चिका_Type_String_Pool->create();
            जइ (bcp_कुञ्चिका_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                भङ्ग;
            }
            copyInvariantChars(u_Bcp_कुञ्चिका_Id, *bcp_कुञ्चिका_Id_Buf, sts);
            जइ (U_FAILURE(sts)) {
                भङ्ग;
            }
            bcp_कुञ्चिका_Id = bcp_कुञ्चिका_Id_Buf->data();
        }

        तार्किक isTZ = uprv_strcmp(legacy_कुञ्चिका_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        जइ (U_FAILURE(sts)) {
            भङ्ग;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_कुञ्चिका;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_कुञ्चिका;

        जइ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_कुञ्चिका.adoptInstead(ures_get_By_कुञ्चिका(typeAliasRes.getAlias(), legacy_कुञ्चिका_Id, nullptr, &tmpSts));
            जइ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_कुञ्चिका.orphan();
            }
        }
        जइ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_कुञ्चिका.adoptInstead(ures_get_By_कुञ्चिका(bcpTypeAliasRes.getAlias(), bcp_कुञ्चिका_Id, nullptr, &tmpSts));
            जइ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_कुञ्चिका.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_कुञ्चिका(ures_get_By_कुञ्चिका(typeMapRes.getAlias(), legacy_कुञ्चिका_Id, nullptr, &sts));
        जइ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } अण्णहा {
            LocalUResourceBundlePointer typeMapEntry;

            जाव (ures_hasNext(type_Map_Res_By_कुञ्चिका.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_कुञ्चिका.getAlias(), typeMapEntry.orphan(), &sts));
                जइ (U_FAILURE(sts)) {
                    भङ्ग;
                }
                ध्रुव वर्ण* legacyTypeId = ures_get_कुञ्चिका(typeMapEntry.getAlias());

                // special types
                जइ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    अनुवर्त;
                }
                जइ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    अनुवर्त;
                }
                जइ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_कुञ्चिका_मानम्;
                    अनुवर्त;
                }

                जइ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    जइ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_कुञ्चिका_Type_String_Pool->create(legacyTypeIdView);
                        जइ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            भङ्ग;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                जइ (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ध्रुव वर्ण* bcpTypeId = legacyTypeId;
                जइ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_कुञ्चिका_Type_String_Pool->create();
                    जइ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        भङ्ग;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    जइ (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                जइ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    भङ्ग;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                जइ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                जइ (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // also put aliases in the map
                जइ (type_Alias_Res_By_कुञ्चिका.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_कुञ्चिका.getAlias());
                    जाव (ures_hasNext(type_Alias_Res_By_कुञ्चिका.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_कुञ्चिका.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ध्रुव char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        जइ (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of canonical legacy type
                        जइ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुञ्चिका(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            जइ (isTZ) {
                                // replace colon with slash if necessary
                                जइ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_कुञ्चिका_Type_String_Pool->create(fromView);
                                    जइ (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        भङ्ग;
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
                    जइ (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }

                जइ (bcp_Type_Alias_Res_By_कुञ्चिका.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias());
                    जाव (ures_hasNext(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ध्रुव char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        जइ (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of bcp type
                        जइ (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुञ्चिका(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    जइ (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }
            }
        }
        जइ (U_FAILURE(sts)) {
            भङ्ग;
        }

        Loc_Ext_कुञ्चिका_Data* कुञ्चिका_Data = g_Loc_Ext_कुञ्चिका_Data_Entries->create();
        जइ (कुञ्चिका_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            भङ्ग;
        }
        कुञ्चिका_Data->bcpId = bcp_कुञ्चिका_Id;
        कुञ्चिका_Data->legacyId = legacy_कुञ्चिका_Id;
        कुञ्चिका_Data->specialTypes = specialTypes;
        कुञ्चिका_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका_Data->legacyId, कुञ्चिका_Data, &sts);
        जइ (legacy_कुञ्चिका_Id != bcp_कुञ्चिका_Id) {
            // different key value
            uhash_put(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका_Data->bcpId, कुञ्चिका_Data, &sts);
        }
        जइ (U_FAILURE(sts)) {
            भङ्ग;
        }
    }
}

तार्किक
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_कुञ्चिका_Map_Init_Once, &initFromResourceBundle, sts);
    जइ (U_FAILURE(sts)) {
        प्रत्यागम मिच्छ;
    }
    प्रत्यागम सच्च;
}

तार्किक
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        जइ (c == '-') {
            जइ (subtagLen < 4 || subtagLen > 6) {
                प्रत्यागम मिच्छ;
            }
            subtagLen = 0;
        } अण्णहा जइ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } अण्णहा {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम subtagLen >= 4 && subtagLen <= 6;
}

तार्किक
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        जइ (c == '-') {
            जइ (subtagLen < 3 || subtagLen > 8) {
                प्रत्यागम मिच्छ;
            }
            subtagLen = 0;
        } अण्णहा जइ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } अण्णहा {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम subtagLen >= 3 && subtagLen <= 8;
}

तार्किक
is_Special_Type_Rg_कुञ्चिका_मानम्(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        जइ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } अण्णहा {
            प्रत्यागम मिच्छ;
        }
    }
    प्रत्यागम subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_कुञ्चिका(std::string_view कुञ्चिका) {
    जइ (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_Data* कुञ्चिका_Data = static_cast<Loc_Ext_कुञ्चिका_Data*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    जइ (कुञ्चिका_Data != nullptr) {
        प्रत्यागम कुञ्चिका_Data->bcpId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_कुञ्चिका(std::string_view कुञ्चिका) {
    जइ (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_Data* कुञ्चिका_Data = static_cast<Loc_Ext_कुञ्चिका_Data*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    जइ (कुञ्चिका_Data != nullptr) {
        प्रत्यागम कुञ्चिका_Data->legacyId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND) {
    जइ (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_Data* कुञ्चिका_Data = static_cast<Loc_Ext_कुञ्चिका_Data*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    जइ (कुञ्चिका_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुञ्चिका_Data->typeMap.getAlias(), &type));
        जइ (t != nullptr) {
            प्रत्यागम t->bcpId;
        }
        जइ (कुञ्चिका_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = मिच्छ;
            जइ (कुञ्चिका_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            जइ (!matched && कुञ्चिका_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            जइ (!matched && कुञ्चिका_Data->specialTypes & SPECIALTYPE_RG_कुञ्चिका_मानम्) {
                matched = is_Special_Type_Rg_कुञ्चिका_मानम्(type);
            }
            जइ (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND) {
    जइ (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_Data* कुञ्चिका_Data = static_cast<Loc_Ext_कुञ्चिका_Data*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    जइ (कुञ्चिका_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुञ्चिका_Data->typeMap.getAlias(), &type));
        जइ (t != nullptr) {
            प्रत्यागम t->legacyId;
        }
        जइ (कुञ्चिका_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = मिच्छ;
            जइ (कुञ्चिका_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            जइ (!matched && कुञ्चिका_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            जइ (!matched && कुञ्चिका_Data->specialTypes & SPECIALTYPE_RG_कुञ्चिका_मानम्) {
                matched = is_Special_Type_Rg_कुञ्चिका_मानम्(type);
            }
            जइ (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}
