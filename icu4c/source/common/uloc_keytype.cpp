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

ساكن UHashtable* g_Loc_Ext_کلید_Map = اشاره_تهی;
ساكن icu::UInitOnce g_Loc_Ext_کلید_Map_Init_Once {};

// bit flags for special types
تعريفنوع تعداد {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_کد = 2,
    SPECIALTYPE_RG_کلید_مقدار = 4
} SpecialType;

بنية Loc_Ext_کلید_داده : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_اشاره_گر typeMap;
    uint32_t        specialTypes;
};

بنية LocExtType : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

بنية TypeAlias : عام icu::UMemory {
    std::string_view from;
};

ساكن icu::حافظه_Pool<icu::FixedString>* g_کلید_Type_String_Pool = اشاره_تهی;
ساكن icu::حافظه_Pool<Loc_Ext_کلید_داده>* g_Loc_Ext_کلید_داده_Entries = اشاره_تهی;
ساكن icu::حافظه_Pool<LocExtType>* gLocExtTypeEntries = اشاره_تهی;
ساكن icu::حافظه_Pool<TypeAlias>* gTypeAliasEntries = اشاره_تهی;

U_CDECL_BEGIN

ساكن UBool U_CALLCONV
uloc_کلید_type_cleanup() {
    إذا (g_Loc_Ext_کلید_Map != اشاره_تهی) {
        uhash_close(g_Loc_Ext_کلید_Map);
        g_Loc_Ext_کلید_Map = اشاره_تهی;
    }

    حذف g_Loc_Ext_کلید_داده_Entries;
    g_Loc_Ext_کلید_داده_Entries = اشاره_تهی;

    حذف gLocExtTypeEntries;
    gLocExtTypeEntries = اشاره_تهی;

    حذف gTypeAliasEntries;
    gTypeAliasEntries = اشاره_تهی;

    حذف g_کلید_Type_String_Pool;
    g_کلید_Type_String_Pool = اشاره_تهی;

    g_Loc_Ext_کلید_Map_Init_Once.reset();
    أرجع درست;
}

U_CDECL_انتها

نطاق {

فراغ U_CALLCONV
initFromResourceBundle(UError_کد& sts) {
    U_NAMESPACE_USE
    ucln_common_ثبات_Cleanup(UCLN_COMMON_LOCALE_کلید_TYPE, uloc_کلید_type_cleanup);

    g_Loc_Ext_کلید_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, اشاره_تهی, &sts);

    Local_UResource_Bundle_اشاره_گر کلید_Type_داده_Res(ures_open_مستقیم(اشاره_تهی, "keyTypeData", &sts));
    Local_UResource_Bundle_اشاره_گر کلید_Map_Res(ures_get_By_کلید(کلید_Type_داده_Res.getAlias(), "keyMap", اشاره_تهی, &sts));
    Local_UResource_Bundle_اشاره_گر typeMapRes(ures_get_By_کلید(کلید_Type_داده_Res.getAlias(), "typeMap", اشاره_تهی, &sts));

    إذا (U_FAILURE(sts)) {
        أرجع;
    }

    UError_کد tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_گر typeAliasRes(ures_get_By_کلید(کلید_Type_داده_Res.getAlias(), "typeAlias", اشاره_تهی, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_گر bcpTypeAliasRes(ures_get_By_کلید(کلید_Type_داده_Res.getAlias(), "bcpTypeAlias", اشاره_تهی, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_کلید_Type_String_Pool = جديد icu::حافظه_Pool<icu::FixedString>;
    إذا (g_کلید_Type_String_Pool == اشاره_تهی) {
        sts = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    g_Loc_Ext_کلید_داده_Entries = جديد icu::حافظه_Pool<Loc_Ext_کلید_داده>;
    إذا (g_Loc_Ext_کلید_داده_Entries == اشاره_تهی) {
        sts = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    gLocExtTypeEntries = جديد icu::حافظه_Pool<LocExtType>;
    إذا (gLocExtTypeEntries == اشاره_تهی) {
        sts = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
    gTypeAliasEntries = جديد icu::حافظه_Pool<TypeAlias>;
    إذا (gTypeAliasEntries == اشاره_تهی) {
        sts = U_حافظه_تخصیص_ERROR;
        أرجع;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_اشاره_گر کلید_Map_مدخل;

    طالما (ures_has_بعدی(کلید_Map_Res.getAlias())) {
        کلید_Map_مدخل.adoptInstead(ures_get_بعدی_Resource(کلید_Map_Res.getAlias(), کلید_Map_مدخل.orphan(), &sts));
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        ثابت حرف* legacy_کلید_Id = ures_get_کلید(کلید_Map_مدخل.getAlias());
        UnicodeString u_Bcp_کلید_Id = ures_getUnicodeString(کلید_Map_مدخل.getAlias(), &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcp_کلید_Id = legacy_کلید_Id;
        إذا (!u_Bcp_کلید_Id.is_خالی()) {
            icu::FixedString* bcp_کلید_Id_Buf = g_کلید_Type_String_Pool->create();
            إذا (bcp_کلید_Id_Buf == اشاره_تهی) {
                sts = U_حافظه_تخصیص_ERROR;
                اكسر;
            }
            copyInvariantChars(u_Bcp_کلید_Id, *bcp_کلید_Id_Buf, sts);
            إذا (U_FAILURE(sts)) {
                اكسر;
            }
            bcp_کلید_Id = bcp_کلید_Id_Buf->داده();
        }

        منطقي isTZ = uprv_strcmp(legacy_کلید_Id, "timezone") == 0;

        UHashtable* type_داده_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, اشاره_تهی, &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_اشاره_گر type_Alias_Res_By_کلید;
        Local_UResource_Bundle_اشاره_گر bcp_Type_Alias_Res_By_کلید;

        إذا (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(typeAliasRes.getAlias(), legacy_کلید_Id, اشاره_تهی, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_کلید.orphan();
            }
        }
        إذا (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(bcpTypeAliasRes.getAlias(), bcp_کلید_Id, اشاره_تهی, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_کلید.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_اشاره_گر type_Map_Res_By_کلید(ures_get_By_کلید(typeMapRes.getAlias(), legacy_کلید_Id, اشاره_تهی, &sts));
        إذا (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } وإلا {
            Local_UResource_Bundle_اشاره_گر type_Map_مدخل;

            طالما (ures_has_بعدی(type_Map_Res_By_کلید.getAlias())) {
                type_Map_مدخل.adoptInstead(ures_get_بعدی_Resource(type_Map_Res_By_کلید.getAlias(), type_Map_مدخل.orphan(), &sts));
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }
                ثابت حرف* legacyTypeId = ures_get_کلید(type_Map_مدخل.getAlias());

                // special types
                إذا (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_کد;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_کلید_مقدار;
                    استمر;
                }

                إذا (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    إذا (uprv_strchr(legacyTypeId, ':') != اشاره_تهی) {
                        U_ASSERT(legacyTypeId != اشاره_تهی && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_کلید_Type_String_Pool->create(legacyTypeIdView);
                        إذا (legacyTypeIdBuf == اشاره_تهی || legacyTypeIdBuf->is_خالی()) {
                            sts = U_حافظه_تخصیص_ERROR;
                            اكسر;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->داده();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_مدخل.getAlias(), &sts);
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                إذا (!uBcpTypeId.is_خالی()) {
                    icu::FixedString* bcpTypeIdBuf = g_کلید_Type_String_Pool->create();
                    إذا (bcpTypeIdBuf == اشاره_تهی) {
                        sts = U_حافظه_تخصیص_ERROR;
                        اكسر;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                    bcpTypeId = bcpTypeIdBuf->داده();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                إذا (t == اشاره_تهی) {
                    sts = U_حافظه_تخصیص_ERROR;
                    اكسر;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_داده_Map, &t->legacyId, t, &sts);
                إذا (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_داده_Map, &t->bcpId, t, &sts);
                }
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // also put aliases in the map
                إذا (type_Alias_Res_By_کلید.isValid()) {
                    Local_UResource_Bundle_اشاره_گر type_Alias_داده_مدخل;

                    ures_resetIterator(type_Alias_Res_By_کلید.getAlias());
                    طالما (ures_has_بعدی(type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t مقصد_Len;
                        type_Alias_داده_مدخل.adoptInstead(ures_get_بعدی_Resource(type_Alias_Res_By_کلید.getAlias(), type_Alias_داده_مدخل.orphan(), &sts));
                        ثابت char16_t* مقصد = ures_getString(type_Alias_داده_مدخل.getAlias(), &مقصد_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of canonical legacy type
                        إذا (uprv_compareInvWithUChar(
                                اشاره_تهی,
                                t->legacyId.داده(),
                                static_cast<int32_t>(t->legacyId.size()),
                                مقصد,
                                مقصد_Len) == 0) {
                            ثابت حرف* from = ures_get_کلید(type_Alias_داده_مدخل.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            إذا (isTZ) {
                                // replace colon with slash if necessary
                                إذا (uprv_strchr(from, ':') != اشاره_تهی) {
                                    U_ASSERT(from != اشاره_تهی && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_کلید_Type_String_Pool->create(fromView);
                                    إذا (fromBuf == اشاره_تهی || fromBuf->is_خالی()) {
                                        sts = U_حافظه_تخصیص_ERROR;
                                        اكسر;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->داده(), fromView.length()};
                                }
                            }
                            uhash_put(type_داده_Map, &alias->from, t, &sts);
                        }
                    }
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                }

                إذا (bcp_Type_Alias_Res_By_کلید.isValid()) {
                    Local_UResource_Bundle_اشاره_گر bcp_Type_Alias_داده_مدخل;

                    ures_resetIterator(bcp_Type_Alias_Res_By_کلید.getAlias());
                    طالما (ures_has_بعدی(bcp_Type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t مقصد_Len;
                        bcp_Type_Alias_داده_مدخل.adoptInstead(ures_get_بعدی_Resource(bcp_Type_Alias_Res_By_کلید.getAlias(), bcp_Type_Alias_داده_مدخل.orphan(), &sts));
                        ثابت char16_t* مقصد = ures_getString(bcp_Type_Alias_داده_مدخل.getAlias(), &مقصد_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of bcp type
                        إذا (uprv_compareInvWithUChar(
                                اشاره_تهی,
                                t->bcpId.داده(),
                                static_cast<int32_t>(t->bcpId.size()),
                                مقصد,
                                مقصد_Len) == 0) {
                            ثابت حرف* from = ures_get_کلید(bcp_Type_Alias_داده_مدخل.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_داده_Map, &alias->from, t, &sts);
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

        Loc_Ext_کلید_داده* کلید_داده = g_Loc_Ext_کلید_داده_Entries->create();
        إذا (کلید_داده == اشاره_تهی) {
            sts = U_حافظه_تخصیص_ERROR;
            اكسر;
        }
        کلید_داده->bcpId = bcp_کلید_Id;
        کلید_داده->legacyId = legacy_کلید_Id;
        کلید_داده->specialTypes = specialTypes;
        کلید_داده->typeMap.adoptInstead(type_داده_Map);

        uhash_put(g_Loc_Ext_کلید_Map, &کلید_داده->legacyId, کلید_داده, &sts);
        إذا (legacy_کلید_Id != bcp_کلید_Id) {
            // different key value
            uhash_put(g_Loc_Ext_کلید_Map, &کلید_داده->bcpId, کلید_داده, &sts);
        }
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
    }
}

منطقي
init() {
    UError_کد sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_کلید_Map_Init_Once, &initFromResourceBundle, sts);
    إذا (U_FAILURE(sts)) {
        أرجع خطأ;
    }
    أرجع درست;
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
is_Special_Type_Reorder_کد(std::string_view val) {
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
is_Special_Type_Rg_کلید_مقدار(std::string_view val) {
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
ulocimp_مقصد_Bcp_کلید(std::string_view کلید) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_داده* کلید_داده = static_cast<Loc_Ext_کلید_داده*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_داده != اشاره_تهی) {
        أرجع کلید_داده->bcpId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_کلید(std::string_view کلید) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_داده* کلید_داده = static_cast<Loc_Ext_کلید_داده*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_داده != اشاره_تهی) {
        أرجع کلید_داده->legacyId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Bcp_Type(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_داده* کلید_داده = static_cast<Loc_Ext_کلید_داده*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_داده != اشاره_تهی) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_داده->typeMap.getAlias(), &type));
        إذا (t != اشاره_تهی) {
            أرجع t->bcpId;
        }
        إذا (کلید_داده->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلید_داده->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلید_داده->specialTypes & SPECIALTYPE_REORDER_کد) {
                matched = is_Special_Type_Reorder_کد(type);
            }
            إذا (!matched && کلید_داده->specialTypes & SPECIALTYPE_RG_کلید_مقدار) {
                matched = is_Special_Type_Rg_کلید_مقدار(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_Type(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلید_داده* کلید_داده = static_cast<Loc_Ext_کلید_داده*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    إذا (کلید_داده != اشاره_تهی) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_داده->typeMap.getAlias(), &type));
        إذا (t != اشاره_تهی) {
            أرجع t->legacyId;
        }
        إذا (کلید_داده->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلید_داده->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلید_داده->specialTypes & SPECIALTYPE_REORDER_کد) {
                matched = is_Special_Type_Reorder_کد(type);
            }
            إذا (!matched && کلید_داده->specialTypes & SPECIALTYPE_RG_کلید_مقدار) {
                matched = is_Special_Type_Rg_کلید_مقدار(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}
