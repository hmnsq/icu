// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#அந்தர்பாவ <algorithm>
#அந்தர்பாவ <optional>
#அந்தர்பாவ <string_view>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/uobject.h"

#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "fixedstring.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "uhash.h"
#அந்தர்பாவ "ulocimp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uresimp.h"
#அந்தர்பாவ "uvector.h"
#அந்தர்பாவ "udataswp.h" /* for InvChar functions */

நிலையான UHashtable* g_Loc_Ext_சாவி_Map = nullptr;
நிலையான icu::UInitOnce g_Loc_Ext_சாவி_Map_Init_Once {};

// bit flags for special types
வகைவரை எண்ணிக்கை {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_சாவி_மதிப்பு = 4
} SpecialType;

கட்டமைப்பு Loc_Ext_சாவி_Data : பொது icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

கட்டமைப்பு LocExtType : பொது icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

கட்டமைப்பு TypeAlias : பொது icu::UMemory {
    std::string_view from;
};

நிலையான icu::MemoryPool<icu::FixedString>* g_சாவி_Type_String_Pool = nullptr;
நிலையான icu::MemoryPool<Loc_Ext_சாவி_Data>* g_Loc_Ext_சாவி_Data_Entries = nullptr;
நிலையான icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
நிலையான icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

நிலையான UBool U_CALLCONV
uloc_சாவி_type_cleanup() {
    எனில் (g_Loc_Ext_சாவி_Map != nullptr) {
        uhash_close(g_Loc_Ext_சாவி_Map);
        g_Loc_Ext_சாவி_Map = nullptr;
    }

    delete g_Loc_Ext_சாவி_Data_Entries;
    g_Loc_Ext_சாவி_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_சாவி_Type_String_Pool;
    g_சாவி_Type_String_Pool = nullptr;

    g_Loc_Ext_சாவி_Map_Init_Once.reset();
    திரும்பு உண்மை;
}

U_CDECL_END

நாமாகாஶ {

வெறுமை U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_சாவி_TYPE, uloc_சாவி_type_cleanup);

    g_Loc_Ext_சாவி_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer சாவி_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer சாவி_Map_Res(ures_get_By_சாவி(சாவி_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_சாவி(சாவி_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    எனில் (U_FAILURE(sts)) {
        திரும்பு;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_சாவி(சாவி_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_சாவி(சாவி_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_சாவி_Type_String_Pool = புதிய icu::MemoryPool<icu::FixedString>;
    எனில் (g_சாவி_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    g_Loc_Ext_சாவி_Data_Entries = புதிய icu::MemoryPool<Loc_Ext_சாவி_Data>;
    எனில் (g_Loc_Ext_சாவி_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    gLocExtTypeEntries = புதிய icu::MemoryPool<LocExtType>;
    எனில் (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
    gTypeAliasEntries = புதிய icu::MemoryPool<TypeAlias>;
    எனில் (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer சாவி_Map_Entry;

    போது (ures_hasNext(சாவி_Map_Res.getAlias())) {
        சாவி_Map_Entry.adoptInstead(ures_getNextResource(சாவி_Map_Res.getAlias(), சாவி_Map_Entry.orphan(), &sts));
        எனில் (U_FAILURE(sts)) {
            நிறுத்து;
        }
        மாறிலி எழுத்து* legacy_சாவி_Id = ures_get_சாவி(சாவி_Map_Entry.getAlias());
        UnicodeString u_Bcp_சாவி_Id = ures_getUnicodeString(சாவி_Map_Entry.getAlias(), &sts);
        எனில் (U_FAILURE(sts)) {
            நிறுத்து;
        }

        // empty value indicates that BCP key is same with the legacy key.
        மாறிலி எழுத்து* bcp_சாவி_Id = legacy_சாவி_Id;
        எனில் (!u_Bcp_சாவி_Id.isEmpty()) {
            icu::FixedString* bcp_சாவி_Id_Buf = g_சாவி_Type_String_Pool->create();
            எனில் (bcp_சாவி_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                நிறுத்து;
            }
            copyInvariantChars(u_Bcp_சாவி_Id, *bcp_சாவி_Id_Buf, sts);
            எனில் (U_FAILURE(sts)) {
                நிறுத்து;
            }
            bcp_சாவி_Id = bcp_சாவி_Id_Buf->data();
        }

        தருக்க isTZ = uprv_strcmp(legacy_சாவி_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        எனில் (U_FAILURE(sts)) {
            நிறுத்து;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_சாவி;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_சாவி;

        எனில் (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_சாவி.adoptInstead(ures_get_By_சாவி(typeAliasRes.getAlias(), legacy_சாவி_Id, nullptr, &tmpSts));
            எனில் (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_சாவி.orphan();
            }
        }
        எனில் (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_சாவி.adoptInstead(ures_get_By_சாவி(bcpTypeAliasRes.getAlias(), bcp_சாவி_Id, nullptr, &tmpSts));
            எனில் (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_சாவி.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_சாவி(ures_get_By_சாவி(typeMapRes.getAlias(), legacy_சாவி_Id, nullptr, &sts));
        எனில் (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } இல்லையெனில் {
            LocalUResourceBundlePointer typeMapEntry;

            போது (ures_hasNext(type_Map_Res_By_சாவி.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_சாவி.getAlias(), typeMapEntry.orphan(), &sts));
                எனில் (U_FAILURE(sts)) {
                    நிறுத்து;
                }
                மாறிலி எழுத்து* legacyTypeId = ures_get_சாவி(typeMapEntry.getAlias());

                // special types
                எனில் (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    தொடர்;
                }
                எனில் (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    தொடர்;
                }
                எனில் (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_சாவி_மதிப்பு;
                    தொடர்;
                }

                எனில் (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    எனில் (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_சாவி_Type_String_Pool->create(legacyTypeIdView);
                        எனில் (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            நிறுத்து;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                எனில் (U_FAILURE(sts)) {
                    நிறுத்து;
                }

                // empty value indicates that BCP type is same with the legacy type.
                மாறிலி எழுத்து* bcpTypeId = legacyTypeId;
                எனில் (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_சாவி_Type_String_Pool->create();
                    எனில் (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        நிறுத்து;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    எனில் (U_FAILURE(sts)) {
                        நிறுத்து;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                எனில் (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    நிறுத்து;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                எனில் (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                எனில் (U_FAILURE(sts)) {
                    நிறுத்து;
                }

                // also put aliases in the map
                எனில் (type_Alias_Res_By_சாவி.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_சாவி.getAlias());
                    போது (ures_hasNext(type_Alias_Res_By_சாவி.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_சாவி.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        மாறிலி char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        எனில் (U_FAILURE(sts)) {
                            நிறுத்து;
                        }
                        // check if this is an alias of canonical legacy type
                        எனில் (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            மாறிலி எழுத்து* from = ures_get_சாவி(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            எனில் (isTZ) {
                                // replace colon with slash if necessary
                                எனில் (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_சாவி_Type_String_Pool->create(fromView);
                                    எனில் (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        நிறுத்து;
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
                    எனில் (U_FAILURE(sts)) {
                        நிறுத்து;
                    }
                }

                எனில் (bcp_Type_Alias_Res_By_சாவி.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_சாவி.getAlias());
                    போது (ures_hasNext(bcp_Type_Alias_Res_By_சாவி.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_சாவி.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        மாறிலி char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        எனில் (U_FAILURE(sts)) {
                            நிறுத்து;
                        }
                        // check if this is an alias of bcp type
                        எனில் (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            மாறிலி எழுத்து* from = ures_get_சாவி(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    எனில் (U_FAILURE(sts)) {
                        நிறுத்து;
                    }
                }
            }
        }
        எனில் (U_FAILURE(sts)) {
            நிறுத்து;
        }

        Loc_Ext_சாவி_Data* சாவி_Data = g_Loc_Ext_சாவி_Data_Entries->create();
        எனில் (சாவி_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            நிறுத்து;
        }
        சாவி_Data->bcpId = bcp_சாவி_Id;
        சாவி_Data->legacyId = legacy_சாவி_Id;
        சாவி_Data->specialTypes = specialTypes;
        சாவி_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_சாவி_Map, &சாவி_Data->legacyId, சாவி_Data, &sts);
        எனில் (legacy_சாவி_Id != bcp_சாவி_Id) {
            // different key value
            uhash_put(g_Loc_Ext_சாவி_Map, &சாவி_Data->bcpId, சாவி_Data, &sts);
        }
        எனில் (U_FAILURE(sts)) {
            நிறுத்து;
        }
    }
}

தருக்க
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_சாவி_Map_Init_Once, &initFromResourceBundle, sts);
    எனில் (U_FAILURE(sts)) {
        திரும்பு பொய்;
    }
    திரும்பு உண்மை;
}

தருக்க
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    க்கு (எழுத்து c : val) {
        எனில் (c == '-') {
            எனில் (subtagLen < 4 || subtagLen > 6) {
                திரும்பு பொய்;
            }
            subtagLen = 0;
        } இல்லையெனில் எனில் ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } இல்லையெனில் {
            திரும்பு பொய்;
        }
    }
    திரும்பு subtagLen >= 4 && subtagLen <= 6;
}

தருக்க
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    க்கு (எழுத்து c : val) {
        எனில் (c == '-') {
            எனில் (subtagLen < 3 || subtagLen > 8) {
                திரும்பு பொய்;
            }
            subtagLen = 0;
        } இல்லையெனில் எனில் (uprv_isASCIILetter(c)) {
            subtagLen++;
        } இல்லையெனில் {
            திரும்பு பொய்;
        }
    }
    திரும்பு subtagLen >= 3 && subtagLen <= 8;
}

தருக்க
is_Special_Type_Rg_சாவி_மதிப்பு(std::string_view val) {
    int32_t subtagLen = 0;
    க்கு (எழுத்து c : val) {
        எனில் ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } இல்லையெனில் {
            திரும்பு பொய்;
        }
    }
    திரும்பு subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_சாவி(std::string_view சாவி) {
    எனில் (!init()) {
        திரும்பு std::nullopt;
    }

    Loc_Ext_சாவி_Data* சாவி_Data = static_cast<Loc_Ext_சாவி_Data*>(uhash_get(g_Loc_Ext_சாவி_Map, &சாவி));
    எனில் (சாவி_Data != nullptr) {
        திரும்பு சாவி_Data->bcpId;
    }

    திரும்பு std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_சாவி(std::string_view சாவி) {
    எனில் (!init()) {
        திரும்பு std::nullopt;
    }

    Loc_Ext_சாவி_Data* சாவி_Data = static_cast<Loc_Ext_சாவி_Data*>(uhash_get(g_Loc_Ext_சாவி_Map, &சாவி));
    எனில் (சாவி_Data != nullptr) {
        திரும்பு சாவி_Data->legacyId;
    }

    திரும்பு std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view சாவி, std::string_view type U_LIFETIME_BOUND) {
    எனில் (!init()) {
        திரும்பு std::nullopt;
    }

    Loc_Ext_சாவி_Data* சாவி_Data = static_cast<Loc_Ext_சாவி_Data*>(uhash_get(g_Loc_Ext_சாவி_Map, &சாவி));
    எனில் (சாவி_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(சாவி_Data->typeMap.getAlias(), &type));
        எனில் (t != nullptr) {
            திரும்பு t->bcpId;
        }
        எனில் (சாவி_Data->specialTypes != SPECIALTYPE_NONE) {
            தருக்க matched = பொய்;
            எனில் (சாவி_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            எனில் (!matched && சாவி_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            எனில் (!matched && சாவி_Data->specialTypes & SPECIALTYPE_RG_சாவி_மதிப்பு) {
                matched = is_Special_Type_Rg_சாவி_மதிப்பு(type);
            }
            எனில் (matched) {
                திரும்பு type;
            }
        }
    }

    திரும்பு std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view சாவி, std::string_view type U_LIFETIME_BOUND) {
    எனில் (!init()) {
        திரும்பு std::nullopt;
    }

    Loc_Ext_சாவி_Data* சாவி_Data = static_cast<Loc_Ext_சாவி_Data*>(uhash_get(g_Loc_Ext_சாவி_Map, &சாவி));
    எனில் (சாவி_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(சாவி_Data->typeMap.getAlias(), &type));
        எனில் (t != nullptr) {
            திரும்பு t->legacyId;
        }
        எனில் (சாவி_Data->specialTypes != SPECIALTYPE_NONE) {
            தருக்க matched = பொய்;
            எனில் (சாவி_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            எனில் (!matched && சாவி_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            எனில் (!matched && சாவி_Data->specialTypes & SPECIALTYPE_RG_சாவி_மதிப்பு) {
                matched = is_Special_Type_Rg_சாவி_மதிப்பு(type);
            }
            எனில் (matched) {
                திரும்பு type;
            }
        }
    }

    திரும்பு std::nullopt;
}
