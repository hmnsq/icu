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

ساكن UHashtable* g_Loc_Ext_کلی_Map = تش_اشاره;
ساكن icu::UInitOnce g_Loc_Ext_کلی_Map_Init_Once {};

// bit flags for special types
تعريفنوع تعداد {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_کوډ = 2,
    SPECIALTYPE_RG_کلی_ارزښت = 4
} SpecialType;

بنية Loc_Ext_کلی_معلومات : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_اشاره_ګر typeMap;
    uint32_t        specialTypes;
};

بنية LocExtType : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

بنية TypeAlias : عام icu::UMemory {
    std::string_view from;
};

ساكن icu::حافظه_Pool<icu::FixedString>* g_کلی_Type_String_Pool = تش_اشاره;
ساكن icu::حافظه_Pool<Loc_Ext_کلی_معلومات>* g_Loc_Ext_کلی_معلومات_Entries = تش_اشاره;
ساكن icu::حافظه_Pool<LocExtType>* gLocExtTypeEntries = تش_اشاره;
ساكن icu::حافظه_Pool<TypeAlias>* gTypeAliasEntries = تش_اشاره;

U_CDECL_BEGIN

ساكن UBool U_CALLCONV
uloc_کلی_type_cleanup() {
    إذا (g_Loc_Ext_کلی_Map != تش_اشاره) {
        uhash_close(g_Loc_Ext_کلی_Map);
        g_Loc_Ext_کلی_Map = تش_اشاره;
    }

    ړنګول g_Loc_Ext_کلی_معلومات_Entries;
    g_Loc_Ext_کلی_معلومات_Entries = تش_اشاره;

    ړنګول gLocExtTypeEntries;
    gLocExtTypeEntries = تش_اشاره;

    ړنګول gTypeAliasEntries;
    gTypeAliasEntries = تش_اشاره;

    ړنګول g_کلی_Type_String_Pool;
    g_کلی_Type_String_Pool = تش_اشاره;

    g_Loc_Ext_کلی_Map_Init_Once.reset();
    أرجع ریښتیا;
}

U_CDECL_پای_ټکی

نطاق {

فراغ U_CALLCONV
initFromResourceBundle(UError_کوډ& sts) {
    U_NAMESPACE_USE
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LOCALE_کلی_TYPE, uloc_کلی_type_cleanup);

    g_Loc_Ext_کلی_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, تش_اشاره, &sts);

    Local_UResource_Bundle_اشاره_ګر کلی_Type_معلومات_Res(ures_open_مستقیم(تش_اشاره, "keyTypeData", &sts));
    Local_UResource_Bundle_اشاره_ګر کلی_Map_Res(ures_get_By_کلی(کلی_Type_معلومات_Res.getAlias(), "keyMap", تش_اشاره, &sts));
    Local_UResource_Bundle_اشاره_ګر typeMapRes(ures_get_By_کلی(کلی_Type_معلومات_Res.getAlias(), "typeMap", تش_اشاره, &sts));

    إذا (U_FAILURE(sts)) {
        أرجع;
    }

    UError_کوډ tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_ګر typeAliasRes(ures_get_By_کلی(کلی_Type_معلومات_Res.getAlias(), "typeAlias", تش_اشاره, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشاره_ګر bcpTypeAliasRes(ures_get_By_کلی(کلی_Type_معلومات_Res.getAlias(), "bcpTypeAlias", تش_اشاره, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_کلی_Type_String_Pool = جديد icu::حافظه_Pool<icu::FixedString>;
    إذا (g_کلی_Type_String_Pool == تش_اشاره) {
        sts = U_حافظه_ویش_ERROR;
        أرجع;
    }
    g_Loc_Ext_کلی_معلومات_Entries = جديد icu::حافظه_Pool<Loc_Ext_کلی_معلومات>;
    إذا (g_Loc_Ext_کلی_معلومات_Entries == تش_اشاره) {
        sts = U_حافظه_ویش_ERROR;
        أرجع;
    }
    gLocExtTypeEntries = جديد icu::حافظه_Pool<LocExtType>;
    إذا (gLocExtTypeEntries == تش_اشاره) {
        sts = U_حافظه_ویش_ERROR;
        أرجع;
    }
    gTypeAliasEntries = جديد icu::حافظه_Pool<TypeAlias>;
    إذا (gTypeAliasEntries == تش_اشاره) {
        sts = U_حافظه_ویش_ERROR;
        أرجع;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_اشاره_ګر کلی_Map_ثبت;

    طالما (ures_has_بل(کلی_Map_Res.getAlias())) {
        کلی_Map_ثبت.adoptInstead(ures_get_بل_Resource(کلی_Map_Res.getAlias(), کلی_Map_ثبت.orphan(), &sts));
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        ثابت حرف* legacy_کلی_Id = ures_get_کلی(کلی_Map_ثبت.getAlias());
        UnicodeString u_Bcp_کلی_Id = ures_getUnicodeString(کلی_Map_ثبت.getAlias(), &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcp_کلی_Id = legacy_کلی_Id;
        إذا (!u_Bcp_کلی_Id.is_تشه()) {
            icu::FixedString* bcp_کلی_Id_Buf = g_کلی_Type_String_Pool->create();
            إذا (bcp_کلی_Id_Buf == تش_اشاره) {
                sts = U_حافظه_ویش_ERROR;
                اكسر;
            }
            copyInvariantChars(u_Bcp_کلی_Id, *bcp_کلی_Id_Buf, sts);
            إذا (U_FAILURE(sts)) {
                اكسر;
            }
            bcp_کلی_Id = bcp_کلی_Id_Buf->معلومات();
        }

        منطقي isTZ = uprv_strcmp(legacy_کلی_Id, "timezone") == 0;

        UHashtable* type_معلومات_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, تش_اشاره, &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_اشاره_ګر type_Alias_Res_By_کلی;
        Local_UResource_Bundle_اشاره_ګر bcp_Type_Alias_Res_By_کلی;

        إذا (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_کلی.adoptInstead(ures_get_By_کلی(typeAliasRes.getAlias(), legacy_کلی_Id, تش_اشاره, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_کلی.orphan();
            }
        }
        إذا (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_کلی.adoptInstead(ures_get_By_کلی(bcpTypeAliasRes.getAlias(), bcp_کلی_Id, تش_اشاره, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_کلی.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_اشاره_ګر type_Map_Res_By_کلی(ures_get_By_کلی(typeMapRes.getAlias(), legacy_کلی_Id, تش_اشاره, &sts));
        إذا (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } وإلا {
            Local_UResource_Bundle_اشاره_ګر type_Map_ثبت;

            طالما (ures_has_بل(type_Map_Res_By_کلی.getAlias())) {
                type_Map_ثبت.adoptInstead(ures_get_بل_Resource(type_Map_Res_By_کلی.getAlias(), type_Map_ثبت.orphan(), &sts));
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }
                ثابت حرف* legacyTypeId = ures_get_کلی(type_Map_ثبت.getAlias());

                // special types
                إذا (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_کوډ;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_کلی_ارزښت;
                    استمر;
                }

                إذا (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    إذا (uprv_strchr(legacyTypeId, ':') != تش_اشاره) {
                        U_ASSERT(legacyTypeId != تش_اشاره && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_کلی_Type_String_Pool->create(legacyTypeIdView);
                        إذا (legacyTypeIdBuf == تش_اشاره || legacyTypeIdBuf->is_تشه()) {
                            sts = U_حافظه_ویش_ERROR;
                            اكسر;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->معلومات();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_ثبت.getAlias(), &sts);
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                إذا (!uBcpTypeId.is_تشه()) {
                    icu::FixedString* bcpTypeIdBuf = g_کلی_Type_String_Pool->create();
                    إذا (bcpTypeIdBuf == تش_اشاره) {
                        sts = U_حافظه_ویش_ERROR;
                        اكسر;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                    bcpTypeId = bcpTypeIdBuf->معلومات();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                إذا (t == تش_اشاره) {
                    sts = U_حافظه_ویش_ERROR;
                    اكسر;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_معلومات_Map, &t->legacyId, t, &sts);
                إذا (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_معلومات_Map, &t->bcpId, t, &sts);
                }
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // also put aliases in the map
                إذا (type_Alias_Res_By_کلی.isValid()) {
                    Local_UResource_Bundle_اشاره_ګر type_Alias_معلومات_ثبت;

                    ures_resetIterator(type_Alias_Res_By_کلی.getAlias());
                    طالما (ures_has_بل(type_Alias_Res_By_کلی.getAlias()) && U_SUCCESS(sts)) {
                        int32_t منزل_Len;
                        type_Alias_معلومات_ثبت.adoptInstead(ures_get_بل_Resource(type_Alias_Res_By_کلی.getAlias(), type_Alias_معلومات_ثبت.orphan(), &sts));
                        ثابت char16_t* منزل = ures_getString(type_Alias_معلومات_ثبت.getAlias(), &منزل_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of canonical legacy type
                        إذا (uprv_compareInvWithUChar(
                                تش_اشاره,
                                t->legacyId.معلومات(),
                                static_cast<int32_t>(t->legacyId.size()),
                                منزل,
                                منزل_Len) == 0) {
                            ثابت حرف* from = ures_get_کلی(type_Alias_معلومات_ثبت.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            إذا (isTZ) {
                                // replace colon with slash if necessary
                                إذا (uprv_strchr(from, ':') != تش_اشاره) {
                                    U_ASSERT(from != تش_اشاره && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_کلی_Type_String_Pool->create(fromView);
                                    إذا (fromBuf == تش_اشاره || fromBuf->is_تشه()) {
                                        sts = U_حافظه_ویش_ERROR;
                                        اكسر;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->معلومات(), fromView.length()};
                                }
                            }
                            uhash_put(type_معلومات_Map, &alias->from, t, &sts);
                        }
                    }
                    إذا (U_FAILURE(sts)) {
                        اكسر;
                    }
                }

                إذا (bcp_Type_Alias_Res_By_کلی.isValid()) {
                    Local_UResource_Bundle_اشاره_ګر bcp_Type_Alias_معلومات_ثبت;

                    ures_resetIterator(bcp_Type_Alias_Res_By_کلی.getAlias());
                    طالما (ures_has_بل(bcp_Type_Alias_Res_By_کلی.getAlias()) && U_SUCCESS(sts)) {
                        int32_t منزل_Len;
                        bcp_Type_Alias_معلومات_ثبت.adoptInstead(ures_get_بل_Resource(bcp_Type_Alias_Res_By_کلی.getAlias(), bcp_Type_Alias_معلومات_ثبت.orphan(), &sts));
                        ثابت char16_t* منزل = ures_getString(bcp_Type_Alias_معلومات_ثبت.getAlias(), &منزل_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of bcp type
                        إذا (uprv_compareInvWithUChar(
                                تش_اشاره,
                                t->bcpId.معلومات(),
                                static_cast<int32_t>(t->bcpId.size()),
                                منزل,
                                منزل_Len) == 0) {
                            ثابت حرف* from = ures_get_کلی(bcp_Type_Alias_معلومات_ثبت.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_معلومات_Map, &alias->from, t, &sts);
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

        Loc_Ext_کلی_معلومات* کلی_معلومات = g_Loc_Ext_کلی_معلومات_Entries->create();
        إذا (کلی_معلومات == تش_اشاره) {
            sts = U_حافظه_ویش_ERROR;
            اكسر;
        }
        کلی_معلومات->bcpId = bcp_کلی_Id;
        کلی_معلومات->legacyId = legacy_کلی_Id;
        کلی_معلومات->specialTypes = specialTypes;
        کلی_معلومات->typeMap.adoptInstead(type_معلومات_Map);

        uhash_put(g_Loc_Ext_کلی_Map, &کلی_معلومات->legacyId, کلی_معلومات, &sts);
        إذا (legacy_کلی_Id != bcp_کلی_Id) {
            // different key value
            uhash_put(g_Loc_Ext_کلی_Map, &کلی_معلومات->bcpId, کلی_معلومات, &sts);
        }
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
    }
}

منطقي
init() {
    UError_کوډ sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_کلی_Map_Init_Once, &initFromResourceBundle, sts);
    إذا (U_FAILURE(sts)) {
        أرجع خطأ;
    }
    أرجع ریښتیا;
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
is_Special_Type_Reorder_کوډ(std::string_view val) {
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
is_Special_Type_Rg_کلی_ارزښت(std::string_view val) {
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
ulocimp_منزل_Bcp_کلی(std::string_view کلی) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلی_معلومات* کلی_معلومات = static_cast<Loc_Ext_کلی_معلومات*>(uhash_get(g_Loc_Ext_کلی_Map, &کلی));
    إذا (کلی_معلومات != تش_اشاره) {
        أرجع کلی_معلومات->bcpId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_کلی(std::string_view کلی) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلی_معلومات* کلی_معلومات = static_cast<Loc_Ext_کلی_معلومات*>(uhash_get(g_Loc_Ext_کلی_Map, &کلی));
    إذا (کلی_معلومات != تش_اشاره) {
        أرجع کلی_معلومات->legacyId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_Type(std::string_view کلی, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلی_معلومات* کلی_معلومات = static_cast<Loc_Ext_کلی_معلومات*>(uhash_get(g_Loc_Ext_کلی_Map, &کلی));
    إذا (کلی_معلومات != تش_اشاره) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلی_معلومات->typeMap.getAlias(), &type));
        إذا (t != تش_اشاره) {
            أرجع t->bcpId;
        }
        إذا (کلی_معلومات->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلی_معلومات->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلی_معلومات->specialTypes & SPECIALTYPE_REORDER_کوډ) {
                matched = is_Special_Type_Reorder_کوډ(type);
            }
            إذا (!matched && کلی_معلومات->specialTypes & SPECIALTYPE_RG_کلی_ارزښت) {
                matched = is_Special_Type_Rg_کلی_ارزښت(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_Type(std::string_view کلی, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_کلی_معلومات* کلی_معلومات = static_cast<Loc_Ext_کلی_معلومات*>(uhash_get(g_Loc_Ext_کلی_Map, &کلی));
    إذا (کلی_معلومات != تش_اشاره) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلی_معلومات->typeMap.getAlias(), &type));
        إذا (t != تش_اشاره) {
            أرجع t->legacyId;
        }
        إذا (کلی_معلومات->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (کلی_معلومات->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && کلی_معلومات->specialTypes & SPECIALTYPE_REORDER_کوډ) {
                matched = is_Special_Type_Reorder_کوډ(type);
            }
            إذا (!matched && کلی_معلومات->specialTypes & SPECIALTYPE_RG_کلی_ارزښت) {
                matched = is_Special_Type_Rg_کلی_ارزښت(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}
