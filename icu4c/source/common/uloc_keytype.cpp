// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#شامل <algorithm>
#شامل <optional>
#شامل <string_view>

#شامل "unicode/utypes.h"
#شامل "unicode/unistr.h"
#شامل "unicode/uobject.h"

#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "fixedstring.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"
#شامل "ulocimp.h"
#شامل "umutex.h"
#شامل "uresimp.h"
#شامل "uvector.h"
#شامل "udataswp.h" /* for InvChar functions */

ایستا UHashtable* g_Loc_Ext_کلید_Map = nullptr;
ایستا icu::UInitOnce g_Loc_Ext_کلید_Map_Init_Once {};

// bit flags for special types
تعریفنوع شمارش {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_کلید_مقدار = 4
} SpecialType;

ساختار Loc_Ext_کلید_Data : عمومی icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

ساختار LocExtType : عمومی icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ساختار TypeAlias : عمومی icu::UMemory {
    std::string_view from;
};

ایستا icu::MemoryPool<icu::FixedString>* g_کلید_Type_String_Pool = nullptr;
ایستا icu::MemoryPool<Loc_Ext_کلید_Data>* g_Loc_Ext_کلید_Data_Entries = nullptr;
ایستا icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
ایستا icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

ایستا UBool U_CALLCONV
uloc_کلید_type_cleanup() {
    اگر (g_Loc_Ext_کلید_Map != nullptr) {
        uhash_close(g_Loc_Ext_کلید_Map);
        g_Loc_Ext_کلید_Map = nullptr;
    }

    delete g_Loc_Ext_کلید_Data_Entries;
    g_Loc_Ext_کلید_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_کلید_Type_String_Pool;
    g_کلید_Type_String_Pool = nullptr;

    g_Loc_Ext_کلید_Map_Init_Once.reset();
    بازگشت درست;
}

U_CDECL_END

فضاینام {

تهی U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_کلید_TYPE, uloc_کلید_type_cleanup);

    g_Loc_Ext_کلید_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer کلید_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer کلید_Map_Res(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    اگر (U_FAILURE(sts)) {
        بازگشت;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_کلید_Type_String_Pool = جدید icu::MemoryPool<icu::FixedString>;
    اگر (g_کلید_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    g_Loc_Ext_کلید_Data_Entries = جدید icu::MemoryPool<Loc_Ext_کلید_Data>;
    اگر (g_Loc_Ext_کلید_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    gLocExtTypeEntries = جدید icu::MemoryPool<LocExtType>;
    اگر (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }
    gTypeAliasEntries = جدید icu::MemoryPool<TypeAlias>;
    اگر (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        بازگشت;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer کلید_Map_Entry;

    وقتی (ures_hasNext(کلید_Map_Res.getAlias())) {
        کلید_Map_Entry.adoptInstead(ures_getNextResource(کلید_Map_Res.getAlias(), کلید_Map_Entry.orphan(), &sts));
        اگر (U_FAILURE(sts)) {
            بشکن;
        }
        ثابت نویسه* legacy_کلید_Id = ures_get_کلید(کلید_Map_Entry.getAlias());
        UnicodeString u_Bcp_کلید_Id = ures_getUnicodeString(کلید_Map_Entry.getAlias(), &sts);
        اگر (U_FAILURE(sts)) {
            بشکن;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت نویسه* bcp_کلید_Id = legacy_کلید_Id;
        اگر (!u_Bcp_کلید_Id.isEmpty()) {
            icu::FixedString* bcp_کلید_Id_Buf = g_کلید_Type_String_Pool->create();
            اگر (bcp_کلید_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                بشکن;
            }
            copyInvariantChars(u_Bcp_کلید_Id, *bcp_کلید_Id_Buf, sts);
            اگر (U_FAILURE(sts)) {
                بشکن;
            }
            bcp_کلید_Id = bcp_کلید_Id_Buf->data();
        }

        منطقی isTZ = uprv_strcmp(legacy_کلید_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        اگر (U_FAILURE(sts)) {
            بشکن;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_کلید;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_کلید;

        اگر (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(typeAliasRes.getAlias(), legacy_کلید_Id, nullptr, &tmpSts));
            اگر (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_کلید.orphan();
            }
        }
        اگر (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(bcpTypeAliasRes.getAlias(), bcp_کلید_Id, nullptr, &tmpSts));
            اگر (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_کلید.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_کلید(ures_get_By_کلید(typeMapRes.getAlias(), legacy_کلید_Id, nullptr, &sts));
        اگر (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } وگرنه {
            LocalUResourceBundlePointer typeMapEntry;

            وقتی (ures_hasNext(type_Map_Res_By_کلید.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_کلید.getAlias(), typeMapEntry.orphan(), &sts));
                اگر (U_FAILURE(sts)) {
                    بشکن;
                }
                ثابت نویسه* legacyTypeId = ures_get_کلید(typeMapEntry.getAlias());

                // special types
                اگر (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ادامه;
                }
                اگر (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    ادامه;
                }
                اگر (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_کلید_مقدار;
                    ادامه;
                }

                اگر (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    اگر (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_کلید_Type_String_Pool->create(legacyTypeIdView);
                        اگر (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            بشکن;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                اگر (U_FAILURE(sts)) {
                    بشکن;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت نویسه* bcpTypeId = legacyTypeId;
                اگر (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_کلید_Type_String_Pool->create();
                    اگر (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        بشکن;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    اگر (U_FAILURE(sts)) {
                        بشکن;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                اگر (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    بشکن;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                اگر (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                اگر (U_FAILURE(sts)) {
                    بشکن;
                }

                // also put aliases in the map
                اگر (type_Alias_Res_By_کلید.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_کلید.getAlias());
                    وقتی (ures_hasNext(type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_کلید.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        اگر (U_FAILURE(sts)) {
                            بشکن;
                        }
                        // check if this is an alias of canonical legacy type
                        اگر (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ثابت نویسه* from = ures_get_کلید(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            اگر (isTZ) {
                                // replace colon with slash if necessary
                                اگر (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_کلید_Type_String_Pool->create(fromView);
                                    اگر (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        بشکن;
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
                    اگر (U_FAILURE(sts)) {
                        بشکن;
                    }
                }

                اگر (bcp_Type_Alias_Res_By_کلید.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_کلید.getAlias());
                    وقتی (ures_hasNext(bcp_Type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_کلید.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        اگر (U_FAILURE(sts)) {
                            بشکن;
                        }
                        // check if this is an alias of bcp type
                        اگر (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ثابت نویسه* from = ures_get_کلید(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    اگر (U_FAILURE(sts)) {
                        بشکن;
                    }
                }
            }
        }
        اگر (U_FAILURE(sts)) {
            بشکن;
        }

        Loc_Ext_کلید_Data* کلید_Data = g_Loc_Ext_کلید_Data_Entries->create();
        اگر (کلید_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            بشکن;
        }
        کلید_Data->bcpId = bcp_کلید_Id;
        کلید_Data->legacyId = legacy_کلید_Id;
        کلید_Data->specialTypes = specialTypes;
        کلید_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_کلید_Map, &کلید_Data->legacyId, کلید_Data, &sts);
        اگر (legacy_کلید_Id != bcp_کلید_Id) {
            // different key value
            uhash_put(g_Loc_Ext_کلید_Map, &کلید_Data->bcpId, کلید_Data, &sts);
        }
        اگر (U_FAILURE(sts)) {
            بشکن;
        }
    }
}

منطقی
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_کلید_Map_Init_Once, &initFromResourceBundle, sts);
    اگر (U_FAILURE(sts)) {
        بازگشت نادرست;
    }
    بازگشت درست;
}

منطقی
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    برای (نویسه c : val) {
        اگر (c == '-') {
            اگر (subtagLen < 4 || subtagLen > 6) {
                بازگشت نادرست;
            }
            subtagLen = 0;
        } وگرنه اگر ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } وگرنه {
            بازگشت نادرست;
        }
    }
    بازگشت subtagLen >= 4 && subtagLen <= 6;
}

منطقی
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    برای (نویسه c : val) {
        اگر (c == '-') {
            اگر (subtagLen < 3 || subtagLen > 8) {
                بازگشت نادرست;
            }
            subtagLen = 0;
        } وگرنه اگر (uprv_isASCIILetter(c)) {
            subtagLen++;
        } وگرنه {
            بازگشت نادرست;
        }
    }
    بازگشت subtagLen >= 3 && subtagLen <= 8;
}

منطقی
is_Special_Type_Rg_کلید_مقدار(std::string_view val) {
    int32_t subtagLen = 0;
    برای (نویسه c : val) {
        اگر ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } وگرنه {
            بازگشت نادرست;
        }
    }
    بازگشت subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_کلید(std::string_view کلید) {
    اگر (!init()) {
        بازگشت std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_Data != nullptr) {
        بازگشت کلید_Data->bcpId;
    }

    بازگشت std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_کلید(std::string_view کلید) {
    اگر (!init()) {
        بازگشت std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_Data != nullptr) {
        بازگشت کلید_Data->legacyId;
    }

    بازگشت std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    اگر (!init()) {
        بازگشت std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_Data->typeMap.getAlias(), &type));
        اگر (t != nullptr) {
            بازگشت t->bcpId;
        }
        اگر (کلید_Data->specialTypes != SPECIALTYPE_NONE) {
            منطقی matched = نادرست;
            اگر (کلید_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            اگر (!matched && کلید_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            اگر (!matched && کلید_Data->specialTypes & SPECIALTYPE_RG_کلید_مقدار) {
                matched = is_Special_Type_Rg_کلید_مقدار(type);
            }
            اگر (matched) {
                بازگشت type;
            }
        }
    }

    بازگشت std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    اگر (!init()) {
        بازگشت std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_Data->typeMap.getAlias(), &type));
        اگر (t != nullptr) {
            بازگشت t->legacyId;
        }
        اگر (کلید_Data->specialTypes != SPECIALTYPE_NONE) {
            منطقی matched = نادرست;
            اگر (کلید_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            اگر (!matched && کلید_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            اگر (!matched && کلید_Data->specialTypes & SPECIALTYPE_RG_کلید_مقدار) {
                matched = is_Special_Type_Rg_کلید_مقدار(type);
            }
            اگر (matched) {
                بازگشت type;
            }
        }
    }

    بازگشت std::nullopt;
}
