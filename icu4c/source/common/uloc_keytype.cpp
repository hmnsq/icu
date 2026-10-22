// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ضمن <algorithm>
#ضمن <optional>
#ضمن <string_view>

#ضمن "unicode/utypes.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/uobject.h"

#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "fixedstring.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"
#ضمن "ulocimp.h"
#ضمن "umutex.h"
#ضمن "uresimp.h"
#ضمن "uvector.h"
#ضمن "udataswp.h" /* for InvChar functions */

ساكن UHashtable* g_Loc_Ext_کلید_Map = nullptr;
ساكن icu::UInitOnce g_Loc_Ext_کلید_Map_Init_Once {};

// bit flags for special types
تعريفنوع تعداد {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_کلید_قدر = 4
} SpecialType;

بنية Loc_Ext_کلید_Data : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

بنية LocExtType : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

بنية TypeAlias : عام icu::UMemory {
    std::string_view from;
};

ساكن icu::MemoryPool<icu::FixedString>* g_کلید_Type_String_Pool = nullptr;
ساكن icu::MemoryPool<Loc_Ext_کلید_Data>* g_Loc_Ext_کلید_Data_Entries = nullptr;
ساكن icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
ساكن icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

ساكن UBool U_CALLCONV
uloc_کلید_type_cleanup() {
    إذا (g_Loc_Ext_کلید_Map != nullptr) {
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
    أرجع true;
}

U_CDECL_END

نطاق {

فراغ U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_کلید_TYPE, uloc_کلید_type_cleanup);

    g_Loc_Ext_کلید_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer کلید_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer کلید_Map_Res(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    إذا (U_FAILURE(sts)) {
        أرجع;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_کلید(کلید_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_کلید_Type_String_Pool = جديد icu::MemoryPool<icu::FixedString>;
    إذا (g_کلید_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    g_Loc_Ext_کلید_Data_Entries = جديد icu::MemoryPool<Loc_Ext_کلید_Data>;
    إذا (g_Loc_Ext_کلید_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    gLocExtTypeEntries = جديد icu::MemoryPool<LocExtType>;
    إذا (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }
    gTypeAliasEntries = جديد icu::MemoryPool<TypeAlias>;
    إذا (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        أرجع;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer کلید_Map_Entry;

    طالما (ures_hasNext(کلید_Map_Res.getAlias())) {
        کلید_Map_Entry.adoptInstead(ures_getNextResource(کلید_Map_Res.getAlias(), کلید_Map_Entry.orphan(), &sts));
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        ثابت حرف* legacy_کلید_Id = ures_get_کلید(کلید_Map_Entry.getAlias());
        UnicodeString u_Bcp_کلید_Id = ures_getUnicodeString(کلید_Map_Entry.getAlias(), &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcp_کلید_Id = legacy_کلید_Id;
        إذا (!u_Bcp_کلید_Id.isEmpty()) {
            icu::FixedString* bcp_کلید_Id_Buf = g_کلید_Type_String_Pool->create();
            إذا (bcp_کلید_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                اكسر;
            }
            copyInvariantChars(u_Bcp_کلید_Id, *bcp_کلید_Id_Buf, sts);
            إذا (U_FAILURE(sts)) {
                اكسر;
            }
            bcp_کلید_Id = bcp_کلید_Id_Buf->data();
        }

        منطقي isTZ = uprv_strcmp(legacy_کلید_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_کلید;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_کلید;

        إذا (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(typeAliasRes.getAlias(), legacy_کلید_Id, nullptr, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_کلید.orphan();
            }
        }
        إذا (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(bcpTypeAliasRes.getAlias(), bcp_کلید_Id, nullptr, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_کلید.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_کلید(ures_get_By_کلید(typeMapRes.getAlias(), legacy_کلید_Id, nullptr, &sts));
        إذا (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } وإلا {
            LocalUResourceBundlePointer typeMapEntry;

            طالما (ures_hasNext(type_Map_Res_By_کلید.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_کلید.getAlias(), typeMapEntry.orphan(), &sts));
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }
                ثابت حرف* legacyTypeId = ures_get_کلید(typeMapEntry.getAlias());

                // special types
                إذا (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_کلید_قدر;
                    استمر;
                }

                إذا (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    إذا (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_کلید_Type_String_Pool->create(legacyTypeIdView);
                        إذا (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            اكسر;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                إذا (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_کلید_Type_String_Pool->create();
                    إذا (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        اكسر;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                إذا (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    اكسر;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                إذا (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // also put aliases in the map
                إذا (type_Alias_Res_By_کلید.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_کلید.getAlias());
                    طالما (ures_hasNext(type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_کلید.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of canonical legacy type
                        إذا (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ثابت حرف* from = ures_get_کلید(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            إذا (isTZ) {
                                // replace colon with slash if necessary
                                إذا (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_کلید_Type_String_Pool->create(fromView);
                                    إذا (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        اكسر;
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
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                }

                إذا (bcp_Type_Alias_Res_By_کلید.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_کلید.getAlias());
                    طالما (ures_hasNext(bcp_Type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_کلید.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of bcp type
                        إذا (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ثابت حرف* from = ures_get_کلید(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                }
            }
        }
        إذا (U_FAILURE(sts)) {
            اكسر;
        }

        Loc_Ext_کلید_Data* کلید_Data = g_Loc_Ext_کلید_Data_Entries->create();
        إذا (کلید_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            اكسر;
        }
        کلید_Data->bcpId = bcp_کلید_Id;
        کلید_Data->legacyId = legacy_کلید_Id;
        کلید_Data->specialTypes = specialTypes;
        کلید_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_کلید_Map, &کلید_Data->legacyId, کلید_Data, &sts);
        إذا (legacy_کلید_Id != bcp_کلید_Id) {
            // different key value
            uhash_put(g_Loc_Ext_کلید_Map, &کلید_Data->bcpId, کلید_Data, &sts);
        }
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
    }
}

منطقي
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_کلید_Map_Init_Once, &initFromResourceBundle, sts);
    إذا (U_FAILURE(sts)) {
        أرجع خطأ;
    }
    أرجع true;
}

منطقي
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        إذا (c == '-') {
            إذا (subtagLen < 4 || subtagLen > 6) {
                أرجع خطأ;
            }
            subtagLen = 0;
        } وإلا إذا ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع subtagLen >= 4 && subtagLen <= 6;
}

منطقي
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        إذا (c == '-') {
            إذا (subtagLen < 3 || subtagLen > 8) {
                أرجع خطأ;
            }
            subtagLen = 0;
        } وإلا إذا (uprv_isASCIILetter(c)) {
            subtagLen++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع subtagLen >= 3 && subtagLen <= 8;
}

منطقي
is_Special_Type_Rg_کلید_قدر(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        إذا ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } وإلا {
            أرجع خطأ;
        }
    }
    أرجع subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_کلید(std::string_view کلید) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_Data != nullptr) {
        أرجع کلید_Data->bcpId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_کلید(std::string_view کلید) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_Data != nullptr) {
        أرجع کلید_Data->legacyId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_Data->typeMap.getAlias(), &type));
        إذا (t != nullptr) {
            أرجع t->bcpId;
        }
        إذا (کلید_Data->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلید_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلید_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            إذا (!matched && کلید_Data->specialTypes & SPECIALTYPE_RG_کلید_قدر) {
                matched = is_Special_Type_Rg_کلید_قدر(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_Data* کلید_Data = static_cast<Loc_Ext_کلید_Data*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_Data->typeMap.getAlias(), &type));
        إذا (t != nullptr) {
            أرجع t->legacyId;
        }
        إذا (کلید_Data->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلید_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلید_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            إذا (!matched && کلید_Data->specialTypes & SPECIALTYPE_RG_کلید_قدر) {
                matched = is_Special_Type_Rg_کلید_قدر(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}
