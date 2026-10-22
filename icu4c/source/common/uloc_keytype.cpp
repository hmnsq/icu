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

स्थिर UHashtable* g_Loc_Ext_कुंजी_Map = nullptr;
स्थिर icu::UInitOnce g_Loc_Ext_कुंजी_Map_Init_Once {};

// bit flags for special types
प्रकारनिर्देश गणना {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_कुंजी_मूल्य = 4
} SpecialType;

संरचना Loc_Ext_कुंजी_Data : सार्वजनिक icu::UMemory {
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

स्थिर icu::MemoryPool<icu::FixedString>* g_कुंजी_Type_String_Pool = nullptr;
स्थिर icu::MemoryPool<Loc_Ext_कुंजी_Data>* g_Loc_Ext_कुंजी_Data_Entries = nullptr;
स्थिर icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
स्थिर icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

स्थिर UBool U_CALLCONV
uloc_कुंजी_type_cleanup() {
    यदि (g_Loc_Ext_कुंजी_Map != nullptr) {
        uhash_close(g_Loc_Ext_कुंजी_Map);
        g_Loc_Ext_कुंजी_Map = nullptr;
    }

    delete g_Loc_Ext_कुंजी_Data_Entries;
    g_Loc_Ext_कुंजी_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_कुंजी_Type_String_Pool;
    g_कुंजी_Type_String_Pool = nullptr;

    g_Loc_Ext_कुंजी_Map_Init_Once.reset();
    प्रत्यागम सत्य;
}

U_CDECL_END

नामाकाश {

शून्य U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_कुंजी_TYPE, uloc_कुंजी_type_cleanup);

    g_Loc_Ext_कुंजी_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer कुंजी_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer कुंजी_Map_Res(ures_get_By_कुंजी(कुंजी_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_कुंजी(कुंजी_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    यदि (U_FAILURE(sts)) {
        प्रत्यागम;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_कुंजी(कुंजी_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_कुंजी(कुंजी_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_कुंजी_Type_String_Pool = नूतन icu::MemoryPool<icu::FixedString>;
    यदि (g_कुंजी_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    g_Loc_Ext_कुंजी_Data_Entries = नूतन icu::MemoryPool<Loc_Ext_कुंजी_Data>;
    यदि (g_Loc_Ext_कुंजी_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    gLocExtTypeEntries = नूतन icu::MemoryPool<LocExtType>;
    यदि (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }
    gTypeAliasEntries = नूतन icu::MemoryPool<TypeAlias>;
    यदि (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        प्रत्यागम;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer कुंजी_Map_Entry;

    यावत् (ures_hasNext(कुंजी_Map_Res.getAlias())) {
        कुंजी_Map_Entry.adoptInstead(ures_getNextResource(कुंजी_Map_Res.getAlias(), कुंजी_Map_Entry.orphan(), &sts));
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
        ध्रुव वर्ण* legacy_कुंजी_Id = ures_get_कुंजी(कुंजी_Map_Entry.getAlias());
        UnicodeString u_Bcp_कुंजी_Id = ures_getUnicodeString(कुंजी_Map_Entry.getAlias(), &sts);
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ध्रुव वर्ण* bcp_कुंजी_Id = legacy_कुंजी_Id;
        यदि (!u_Bcp_कुंजी_Id.isEmpty()) {
            icu::FixedString* bcp_कुंजी_Id_Buf = g_कुंजी_Type_String_Pool->create();
            यदि (bcp_कुंजी_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                भङ्ग;
            }
            copyInvariantChars(u_Bcp_कुंजी_Id, *bcp_कुंजी_Id_Buf, sts);
            यदि (U_FAILURE(sts)) {
                भङ्ग;
            }
            bcp_कुंजी_Id = bcp_कुंजी_Id_Buf->data();
        }

        तार्किक isTZ = uprv_strcmp(legacy_कुंजी_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_कुंजी;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_कुंजी;

        यदि (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_कुंजी.adoptInstead(ures_get_By_कुंजी(typeAliasRes.getAlias(), legacy_कुंजी_Id, nullptr, &tmpSts));
            यदि (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_कुंजी.orphan();
            }
        }
        यदि (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_कुंजी.adoptInstead(ures_get_By_कुंजी(bcpTypeAliasRes.getAlias(), bcp_कुंजी_Id, nullptr, &tmpSts));
            यदि (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_कुंजी.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_कुंजी(ures_get_By_कुंजी(typeMapRes.getAlias(), legacy_कुंजी_Id, nullptr, &sts));
        यदि (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } अन्यथा {
            LocalUResourceBundlePointer typeMapEntry;

            यावत् (ures_hasNext(type_Map_Res_By_कुंजी.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_कुंजी.getAlias(), typeMapEntry.orphan(), &sts));
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }
                ध्रुव वर्ण* legacyTypeId = ures_get_कुंजी(typeMapEntry.getAlias());

                // special types
                यदि (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    अनुवर्त;
                }
                यदि (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    अनुवर्त;
                }
                यदि (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_कुंजी_मूल्य;
                    अनुवर्त;
                }

                यदि (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    यदि (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_कुंजी_Type_String_Pool->create(legacyTypeIdView);
                        यदि (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
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
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ध्रुव वर्ण* bcpTypeId = legacyTypeId;
                यदि (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_कुंजी_Type_String_Pool->create();
                    यदि (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        भङ्ग;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                यदि (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    भङ्ग;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                यदि (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // also put aliases in the map
                यदि (type_Alias_Res_By_कुंजी.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_कुंजी.getAlias());
                    यावत् (ures_hasNext(type_Alias_Res_By_कुंजी.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_कुंजी.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ध्रुव char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        यदि (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of canonical legacy type
                        यदि (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुंजी(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            यदि (isTZ) {
                                // replace colon with slash if necessary
                                यदि (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_कुंजी_Type_String_Pool->create(fromView);
                                    यदि (fromBuf == nullptr || fromBuf->isEmpty()) {
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
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }

                यदि (bcp_Type_Alias_Res_By_कुंजी.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_कुंजी.getAlias());
                    यावत् (ures_hasNext(bcp_Type_Alias_Res_By_कुंजी.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_कुंजी.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ध्रुव char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        यदि (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of bcp type
                        यदि (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुंजी(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }
            }
        }
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }

        Loc_Ext_कुंजी_Data* कुंजी_Data = g_Loc_Ext_कुंजी_Data_Entries->create();
        यदि (कुंजी_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            भङ्ग;
        }
        कुंजी_Data->bcpId = bcp_कुंजी_Id;
        कुंजी_Data->legacyId = legacy_कुंजी_Id;
        कुंजी_Data->specialTypes = specialTypes;
        कुंजी_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_कुंजी_Map, &कुंजी_Data->legacyId, कुंजी_Data, &sts);
        यदि (legacy_कुंजी_Id != bcp_कुंजी_Id) {
            // different key value
            uhash_put(g_Loc_Ext_कुंजी_Map, &कुंजी_Data->bcpId, कुंजी_Data, &sts);
        }
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
    }
}

तार्किक
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_कुंजी_Map_Init_Once, &initFromResourceBundle, sts);
    यदि (U_FAILURE(sts)) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}

तार्किक
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि (c == '-') {
            यदि (subtagLen < 4 || subtagLen > 6) {
                प्रत्यागम असत्य;
            }
            subtagLen = 0;
        } अन्यथा यदि ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen >= 4 && subtagLen <= 6;
}

तार्किक
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि (c == '-') {
            यदि (subtagLen < 3 || subtagLen > 8) {
                प्रत्यागम असत्य;
            }
            subtagLen = 0;
        } अन्यथा यदि (uprv_isASCIILetter(c)) {
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen >= 3 && subtagLen <= 8;
}

तार्किक
is_Special_Type_Rg_कुंजी_मूल्य(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_कुंजी(std::string_view कुंजी) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुंजी_Data* कुंजी_Data = static_cast<Loc_Ext_कुंजी_Data*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    यदि (कुंजी_Data != nullptr) {
        प्रत्यागम कुंजी_Data->bcpId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_कुंजी(std::string_view कुंजी) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुंजी_Data* कुंजी_Data = static_cast<Loc_Ext_कुंजी_Data*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    यदि (कुंजी_Data != nullptr) {
        प्रत्यागम कुंजी_Data->legacyId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view कुंजी, std::string_view type U_LIFETIME_BOUND) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुंजी_Data* कुंजी_Data = static_cast<Loc_Ext_कुंजी_Data*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    यदि (कुंजी_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुंजी_Data->typeMap.getAlias(), &type));
        यदि (t != nullptr) {
            प्रत्यागम t->bcpId;
        }
        यदि (कुंजी_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            यदि (कुंजी_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            यदि (!matched && कुंजी_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            यदि (!matched && कुंजी_Data->specialTypes & SPECIALTYPE_RG_कुंजी_मूल्य) {
                matched = is_Special_Type_Rg_कुंजी_मूल्य(type);
            }
            यदि (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view कुंजी, std::string_view type U_LIFETIME_BOUND) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुंजी_Data* कुंजी_Data = static_cast<Loc_Ext_कुंजी_Data*>(uhash_get(g_Loc_Ext_कुंजी_Map, &कुंजी));
    यदि (कुंजी_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुंजी_Data->typeMap.getAlias(), &type));
        यदि (t != nullptr) {
            प्रत्यागम t->legacyId;
        }
        यदि (कुंजी_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            यदि (कुंजी_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            यदि (!matched && कुंजी_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            यदि (!matched && कुंजी_Data->specialTypes & SPECIALTYPE_RG_कुंजी_मूल्य) {
                matched = is_Special_Type_Rg_कुंजी_मूल्य(type);
            }
            यदि (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}
