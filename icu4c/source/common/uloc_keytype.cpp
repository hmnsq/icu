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

स्थिर UHashtable* g_Loc_Ext_किल्ली_Map = nullptr;
स्थिर icu::UInitOnce g_Loc_Ext_किल्ली_Map_Init_Once {};

// bit flags for special types
प्रकारव्याख्या गणना {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_किल्ली_मूल्य = 4
} SpecialType;

संरचना Loc_Ext_किल्ली_Data : सार्वजनिक icu::UMemory {
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

स्थिर icu::MemoryPool<icu::FixedString>* g_किल्ली_Type_String_Pool = nullptr;
स्थिर icu::MemoryPool<Loc_Ext_किल्ली_Data>* g_Loc_Ext_किल्ली_Data_Entries = nullptr;
स्थिर icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
स्थिर icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

स्थिर UBool U_CALLCONV
uloc_किल्ली_type_cleanup() {
    जर (g_Loc_Ext_किल्ली_Map != nullptr) {
        uhash_close(g_Loc_Ext_किल्ली_Map);
        g_Loc_Ext_किल्ली_Map = nullptr;
    }

    delete g_Loc_Ext_किल्ली_Data_Entries;
    g_Loc_Ext_किल्ली_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_किल्ली_Type_String_Pool;
    g_किल्ली_Type_String_Pool = nullptr;

    g_Loc_Ext_किल्ली_Map_Init_Once.reset();
    परत सत्य;
}

U_CDECL_END

नामाकाश {

शून्य U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_किल्ली_TYPE, uloc_किल्ली_type_cleanup);

    g_Loc_Ext_किल्ली_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer किल्ली_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer किल्ली_Map_Res(ures_get_By_किल्ली(किल्ली_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_किल्ली(किल्ली_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    जर (U_FAILURE(sts)) {
        परत;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_किल्ली(किल्ली_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_किल्ली(किल्ली_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_किल्ली_Type_String_Pool = नवीन icu::MemoryPool<icu::FixedString>;
    जर (g_किल्ली_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    g_Loc_Ext_किल्ली_Data_Entries = नवीन icu::MemoryPool<Loc_Ext_किल्ली_Data>;
    जर (g_Loc_Ext_किल्ली_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    gLocExtTypeEntries = नवीन icu::MemoryPool<LocExtType>;
    जर (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }
    gTypeAliasEntries = नवीन icu::MemoryPool<TypeAlias>;
    जर (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        परत;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer किल्ली_Map_Entry;

    पर्यंत (ures_hasNext(किल्ली_Map_Res.getAlias())) {
        किल्ली_Map_Entry.adoptInstead(ures_getNextResource(किल्ली_Map_Res.getAlias(), किल्ली_Map_Entry.orphan(), &sts));
        जर (U_FAILURE(sts)) {
            थांबा;
        }
        स्थिरांक अक्षर* legacy_किल्ली_Id = ures_get_किल्ली(किल्ली_Map_Entry.getAlias());
        UnicodeString u_Bcp_किल्ली_Id = ures_getUnicodeString(किल्ली_Map_Entry.getAlias(), &sts);
        जर (U_FAILURE(sts)) {
            थांबा;
        }

        // empty value indicates that BCP key is same with the legacy key.
        स्थिरांक अक्षर* bcp_किल्ली_Id = legacy_किल्ली_Id;
        जर (!u_Bcp_किल्ली_Id.isEmpty()) {
            icu::FixedString* bcp_किल्ली_Id_Buf = g_किल्ली_Type_String_Pool->create();
            जर (bcp_किल्ली_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                थांबा;
            }
            copyInvariantChars(u_Bcp_किल्ली_Id, *bcp_किल्ली_Id_Buf, sts);
            जर (U_FAILURE(sts)) {
                थांबा;
            }
            bcp_किल्ली_Id = bcp_किल्ली_Id_Buf->data();
        }

        तार्किक isTZ = uprv_strcmp(legacy_किल्ली_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        जर (U_FAILURE(sts)) {
            थांबा;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_किल्ली;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_किल्ली;

        जर (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_किल्ली.adoptInstead(ures_get_By_किल्ली(typeAliasRes.getAlias(), legacy_किल्ली_Id, nullptr, &tmpSts));
            जर (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_किल्ली.orphan();
            }
        }
        जर (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_किल्ली.adoptInstead(ures_get_By_किल्ली(bcpTypeAliasRes.getAlias(), bcp_किल्ली_Id, nullptr, &tmpSts));
            जर (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_किल्ली.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_किल्ली(ures_get_By_किल्ली(typeMapRes.getAlias(), legacy_किल्ली_Id, nullptr, &sts));
        जर (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } अन्यथा {
            LocalUResourceBundlePointer typeMapEntry;

            पर्यंत (ures_hasNext(type_Map_Res_By_किल्ली.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_किल्ली.getAlias(), typeMapEntry.orphan(), &sts));
                जर (U_FAILURE(sts)) {
                    थांबा;
                }
                स्थिरांक अक्षर* legacyTypeId = ures_get_किल्ली(typeMapEntry.getAlias());

                // special types
                जर (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    सुरूठेवा;
                }
                जर (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    सुरूठेवा;
                }
                जर (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_किल्ली_मूल्य;
                    सुरूठेवा;
                }

                जर (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    जर (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_किल्ली_Type_String_Pool->create(legacyTypeIdView);
                        जर (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            थांबा;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                जर (U_FAILURE(sts)) {
                    थांबा;
                }

                // empty value indicates that BCP type is same with the legacy type.
                स्थिरांक अक्षर* bcpTypeId = legacyTypeId;
                जर (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_किल्ली_Type_String_Pool->create();
                    जर (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        थांबा;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    जर (U_FAILURE(sts)) {
                        थांबा;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                जर (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    थांबा;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                जर (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                जर (U_FAILURE(sts)) {
                    थांबा;
                }

                // also put aliases in the map
                जर (type_Alias_Res_By_किल्ली.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_किल्ली.getAlias());
                    पर्यंत (ures_hasNext(type_Alias_Res_By_किल्ली.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_किल्ली.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        स्थिरांक char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        जर (U_FAILURE(sts)) {
                            थांबा;
                        }
                        // check if this is an alias of canonical legacy type
                        जर (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            स्थिरांक अक्षर* from = ures_get_किल्ली(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            जर (isTZ) {
                                // replace colon with slash if necessary
                                जर (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_किल्ली_Type_String_Pool->create(fromView);
                                    जर (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        थांबा;
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
                    जर (U_FAILURE(sts)) {
                        थांबा;
                    }
                }

                जर (bcp_Type_Alias_Res_By_किल्ली.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_किल्ली.getAlias());
                    पर्यंत (ures_hasNext(bcp_Type_Alias_Res_By_किल्ली.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_किल्ली.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        स्थिरांक char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        जर (U_FAILURE(sts)) {
                            थांबा;
                        }
                        // check if this is an alias of bcp type
                        जर (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            स्थिरांक अक्षर* from = ures_get_किल्ली(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    जर (U_FAILURE(sts)) {
                        थांबा;
                    }
                }
            }
        }
        जर (U_FAILURE(sts)) {
            थांबा;
        }

        Loc_Ext_किल्ली_Data* किल्ली_Data = g_Loc_Ext_किल्ली_Data_Entries->create();
        जर (किल्ली_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            थांबा;
        }
        किल्ली_Data->bcpId = bcp_किल्ली_Id;
        किल्ली_Data->legacyId = legacy_किल्ली_Id;
        किल्ली_Data->specialTypes = specialTypes;
        किल्ली_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_किल्ली_Map, &किल्ली_Data->legacyId, किल्ली_Data, &sts);
        जर (legacy_किल्ली_Id != bcp_किल्ली_Id) {
            // different key value
            uhash_put(g_Loc_Ext_किल्ली_Map, &किल्ली_Data->bcpId, किल्ली_Data, &sts);
        }
        जर (U_FAILURE(sts)) {
            थांबा;
        }
    }
}

तार्किक
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_किल्ली_Map_Init_Once, &initFromResourceBundle, sts);
    जर (U_FAILURE(sts)) {
        परत असत्य;
    }
    परत सत्य;
}

तार्किक
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    साठी (अक्षर c : val) {
        जर (c == '-') {
            जर (subtagLen < 4 || subtagLen > 6) {
                परत असत्य;
            }
            subtagLen = 0;
        } अन्यथा जर ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } अन्यथा {
            परत असत्य;
        }
    }
    परत subtagLen >= 4 && subtagLen <= 6;
}

तार्किक
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    साठी (अक्षर c : val) {
        जर (c == '-') {
            जर (subtagLen < 3 || subtagLen > 8) {
                परत असत्य;
            }
            subtagLen = 0;
        } अन्यथा जर (uprv_isASCIILetter(c)) {
            subtagLen++;
        } अन्यथा {
            परत असत्य;
        }
    }
    परत subtagLen >= 3 && subtagLen <= 8;
}

तार्किक
is_Special_Type_Rg_किल्ली_मूल्य(std::string_view val) {
    int32_t subtagLen = 0;
    साठी (अक्षर c : val) {
        जर ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } अन्यथा {
            परत असत्य;
        }
    }
    परत subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_किल्ली(std::string_view किल्ली) {
    जर (!init()) {
        परत std::nullopt;
    }

    Loc_Ext_किल्ली_Data* किल्ली_Data = static_cast<Loc_Ext_किल्ली_Data*>(uhash_get(g_Loc_Ext_किल्ली_Map, &किल्ली));
    जर (किल्ली_Data != nullptr) {
        परत किल्ली_Data->bcpId;
    }

    परत std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_किल्ली(std::string_view किल्ली) {
    जर (!init()) {
        परत std::nullopt;
    }

    Loc_Ext_किल्ली_Data* किल्ली_Data = static_cast<Loc_Ext_किल्ली_Data*>(uhash_get(g_Loc_Ext_किल्ली_Map, &किल्ली));
    जर (किल्ली_Data != nullptr) {
        परत किल्ली_Data->legacyId;
    }

    परत std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view किल्ली, std::string_view type U_LIFETIME_BOUND) {
    जर (!init()) {
        परत std::nullopt;
    }

    Loc_Ext_किल्ली_Data* किल्ली_Data = static_cast<Loc_Ext_किल्ली_Data*>(uhash_get(g_Loc_Ext_किल्ली_Map, &किल्ली));
    जर (किल्ली_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(किल्ली_Data->typeMap.getAlias(), &type));
        जर (t != nullptr) {
            परत t->bcpId;
        }
        जर (किल्ली_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            जर (किल्ली_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            जर (!matched && किल्ली_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            जर (!matched && किल्ली_Data->specialTypes & SPECIALTYPE_RG_किल्ली_मूल्य) {
                matched = is_Special_Type_Rg_किल्ली_मूल्य(type);
            }
            जर (matched) {
                परत type;
            }
        }
    }

    परत std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view किल्ली, std::string_view type U_LIFETIME_BOUND) {
    जर (!init()) {
        परत std::nullopt;
    }

    Loc_Ext_किल्ली_Data* किल्ली_Data = static_cast<Loc_Ext_किल्ली_Data*>(uhash_get(g_Loc_Ext_किल्ली_Map, &किल्ली));
    जर (किल्ली_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(किल्ली_Data->typeMap.getAlias(), &type));
        जर (t != nullptr) {
            परत t->legacyId;
        }
        जर (किल्ली_Data->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            जर (किल्ली_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            जर (!matched && किल्ली_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            जर (!matched && किल्ली_Data->specialTypes & SPECIALTYPE_RG_किल्ली_मूल्य) {
                matched = is_Special_Type_Rg_किल्ली_मूल्य(type);
            }
            जर (matched) {
                परत type;
            }
        }
    }

    परत std::nullopt;
}
