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

ساكن UHashtable* g_Loc_Ext_ڪنجي_Map = خالي_اشارو;
ساكن icu::UInitOnce g_Loc_Ext_ڪنجي_Map_Init_Once {};

// bit flags for special types
تعريفنوع تعداد {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_ڪوڊ = 2,
    SPECIALTYPE_RG_ڪنجي_قدر = 4
} SpecialType;

بنية Loc_Ext_ڪنجي_معلومات : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_اشارو typeMap;
    uint32_t        specialTypes;
};

بنية LocExtType : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

بنية TypeAlias : عام icu::UMemory {
    std::string_view from;
};

ساكن icu::يادگيري_Pool<icu::FixedString>* g_ڪنجي_Type_String_Pool = خالي_اشارو;
ساكن icu::يادگيري_Pool<Loc_Ext_ڪنجي_معلومات>* g_Loc_Ext_ڪنجي_معلومات_Entries = خالي_اشارو;
ساكن icu::يادگيري_Pool<LocExtType>* gLocExtTypeEntries = خالي_اشارو;
ساكن icu::يادگيري_Pool<TypeAlias>* gTypeAliasEntries = خالي_اشارو;

U_CDECL_BEGIN

ساكن UBool U_CALLCONV
uloc_ڪنجي_type_cleanup() {
    إذا (g_Loc_Ext_ڪنجي_Map != خالي_اشارو) {
        uhash_close(g_Loc_Ext_ڪنجي_Map);
        g_Loc_Ext_ڪنجي_Map = خالي_اشارو;
    }

    ميٽيو g_Loc_Ext_ڪنجي_معلومات_Entries;
    g_Loc_Ext_ڪنجي_معلومات_Entries = خالي_اشارو;

    ميٽيو gLocExtTypeEntries;
    gLocExtTypeEntries = خالي_اشارو;

    ميٽيو gTypeAliasEntries;
    gTypeAliasEntries = خالي_اشارو;

    ميٽيو g_ڪنجي_Type_String_Pool;
    g_ڪنجي_Type_String_Pool = خالي_اشارو;

    g_Loc_Ext_ڪنجي_Map_Init_Once.reset();
    أرجع سچ;
}

U_CDECL_پڇاڙي

نطاق {

فراغ U_CALLCONV
initFromResourceBundle(UError_ڪوڊ& sts) {
    U_NAMESPACE_USE
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LOCALE_ڪنجي_TYPE, uloc_ڪنجي_type_cleanup);

    g_Loc_Ext_ڪنجي_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, خالي_اشارو, &sts);

    Local_UResource_Bundle_اشارو ڪنجي_Type_معلومات_Res(ures_open_سڌو(خالي_اشارو, "keyTypeData", &sts));
    Local_UResource_Bundle_اشارو ڪنجي_Map_Res(ures_get_By_ڪنجي(ڪنجي_Type_معلومات_Res.getAlias(), "keyMap", خالي_اشارو, &sts));
    Local_UResource_Bundle_اشارو typeMapRes(ures_get_By_ڪنجي(ڪنجي_Type_معلومات_Res.getAlias(), "typeMap", خالي_اشارو, &sts));

    إذا (U_FAILURE(sts)) {
        أرجع;
    }

    UError_ڪوڊ tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارو typeAliasRes(ures_get_By_ڪنجي(ڪنجي_Type_معلومات_Res.getAlias(), "typeAlias", خالي_اشارو, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارو bcpTypeAliasRes(ures_get_By_ڪنجي(ڪنجي_Type_معلومات_Res.getAlias(), "bcpTypeAlias", خالي_اشارو, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ڪنجي_Type_String_Pool = جديد icu::يادگيري_Pool<icu::FixedString>;
    إذا (g_ڪنجي_Type_String_Pool == خالي_اشارو) {
        sts = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    g_Loc_Ext_ڪنجي_معلومات_Entries = جديد icu::يادگيري_Pool<Loc_Ext_ڪنجي_معلومات>;
    إذا (g_Loc_Ext_ڪنجي_معلومات_Entries == خالي_اشارو) {
        sts = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    gLocExtTypeEntries = جديد icu::يادگيري_Pool<LocExtType>;
    إذا (gLocExtTypeEntries == خالي_اشارو) {
        sts = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
    gTypeAliasEntries = جديد icu::يادگيري_Pool<TypeAlias>;
    إذا (gTypeAliasEntries == خالي_اشارو) {
        sts = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_اشارو ڪنجي_Map_داخلا;

    طالما (ures_has_ايندڙ(ڪنجي_Map_Res.getAlias())) {
        ڪنجي_Map_داخلا.adoptInstead(ures_get_ايندڙ_Resource(ڪنجي_Map_Res.getAlias(), ڪنجي_Map_داخلا.orphan(), &sts));
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        ثابت حرف* legacy_ڪنجي_Id = ures_get_ڪنجي(ڪنجي_Map_داخلا.getAlias());
        UnicodeString u_Bcp_ڪنجي_Id = ures_getUnicodeString(ڪنجي_Map_داخلا.getAlias(), &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcp_ڪنجي_Id = legacy_ڪنجي_Id;
        إذا (!u_Bcp_ڪنجي_Id.is_خالي_جايون()) {
            icu::FixedString* bcp_ڪنجي_Id_Buf = g_ڪنجي_Type_String_Pool->create();
            إذا (bcp_ڪنجي_Id_Buf == خالي_اشارو) {
                sts = U_يادگيري_ورهاست_ERROR;
                اكسر;
            }
            copyInvariantChars(u_Bcp_ڪنجي_Id, *bcp_ڪنجي_Id_Buf, sts);
            إذا (U_FAILURE(sts)) {
                اكسر;
            }
            bcp_ڪنجي_Id = bcp_ڪنجي_Id_Buf->معلومات();
        }

        منطقي isTZ = uprv_strcmp(legacy_ڪنجي_Id, "timezone") == 0;

        UHashtable* type_معلومات_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, خالي_اشارو, &sts);
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_اشارو type_Alias_Res_By_ڪنجي;
        Local_UResource_Bundle_اشارو bcp_Type_Alias_Res_By_ڪنجي;

        إذا (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ڪنجي.adoptInstead(ures_get_By_ڪنجي(typeAliasRes.getAlias(), legacy_ڪنجي_Id, خالي_اشارو, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ڪنجي.orphan();
            }
        }
        إذا (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ڪنجي.adoptInstead(ures_get_By_ڪنجي(bcpTypeAliasRes.getAlias(), bcp_ڪنجي_Id, خالي_اشارو, &tmpSts));
            إذا (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ڪنجي.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_اشارو type_Map_Res_By_ڪنجي(ures_get_By_ڪنجي(typeMapRes.getAlias(), legacy_ڪنجي_Id, خالي_اشارو, &sts));
        إذا (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } وإلا {
            Local_UResource_Bundle_اشارو type_Map_داخلا;

            طالما (ures_has_ايندڙ(type_Map_Res_By_ڪنجي.getAlias())) {
                type_Map_داخلا.adoptInstead(ures_get_ايندڙ_Resource(type_Map_Res_By_ڪنجي.getAlias(), type_Map_داخلا.orphan(), &sts));
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }
                ثابت حرف* legacyTypeId = ures_get_ڪنجي(type_Map_داخلا.getAlias());

                // special types
                إذا (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_ڪوڊ;
                    استمر;
                }
                إذا (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ڪنجي_قدر;
                    استمر;
                }

                إذا (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    إذا (uprv_strchr(legacyTypeId, ':') != خالي_اشارو) {
                        U_ASSERT(legacyTypeId != خالي_اشارو && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ڪنجي_Type_String_Pool->create(legacyTypeIdView);
                        إذا (legacyTypeIdBuf == خالي_اشارو || legacyTypeIdBuf->is_خالي_جايون()) {
                            sts = U_يادگيري_ورهاست_ERROR;
                            اكسر;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->معلومات();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_داخلا.getAlias(), &sts);
                إذا (U_FAILURE(sts)) {
                    اكسر;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                إذا (!uBcpTypeId.is_خالي_جايون()) {
                    icu::FixedString* bcpTypeIdBuf = g_ڪنجي_Type_String_Pool->create();
                    إذا (bcpTypeIdBuf == خالي_اشارو) {
                        sts = U_يادگيري_ورهاست_ERROR;
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
                إذا (t == خالي_اشارو) {
                    sts = U_يادگيري_ورهاست_ERROR;
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
                إذا (type_Alias_Res_By_ڪنجي.isValid()) {
                    Local_UResource_Bundle_اشارو type_Alias_معلومات_داخلا;

                    ures_resetIterator(type_Alias_Res_By_ڪنجي.getAlias());
                    طالما (ures_has_ايندڙ(type_Alias_Res_By_ڪنجي.getAlias()) && U_SUCCESS(sts)) {
                        int32_t منزل_Len;
                        type_Alias_معلومات_داخلا.adoptInstead(ures_get_ايندڙ_Resource(type_Alias_Res_By_ڪنجي.getAlias(), type_Alias_معلومات_داخلا.orphan(), &sts));
                        ثابت char16_t* منزل = ures_getString(type_Alias_معلومات_داخلا.getAlias(), &منزل_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of canonical legacy type
                        إذا (uprv_compareInvWithUChar(
                                خالي_اشارو,
                                t->legacyId.معلومات(),
                                static_cast<int32_t>(t->legacyId.size()),
                                منزل,
                                منزل_Len) == 0) {
                            ثابت حرف* from = ures_get_ڪنجي(type_Alias_معلومات_داخلا.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            إذا (isTZ) {
                                // replace colon with slash if necessary
                                إذا (uprv_strchr(from, ':') != خالي_اشارو) {
                                    U_ASSERT(from != خالي_اشارو && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ڪنجي_Type_String_Pool->create(fromView);
                                    إذا (fromBuf == خالي_اشارو || fromBuf->is_خالي_جايون()) {
                                        sts = U_يادگيري_ورهاست_ERROR;
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

                إذا (bcp_Type_Alias_Res_By_ڪنجي.isValid()) {
                    Local_UResource_Bundle_اشارو bcp_Type_Alias_معلومات_داخلا;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ڪنجي.getAlias());
                    طالما (ures_has_ايندڙ(bcp_Type_Alias_Res_By_ڪنجي.getAlias()) && U_SUCCESS(sts)) {
                        int32_t منزل_Len;
                        bcp_Type_Alias_معلومات_داخلا.adoptInstead(ures_get_ايندڙ_Resource(bcp_Type_Alias_Res_By_ڪنجي.getAlias(), bcp_Type_Alias_معلومات_داخلا.orphan(), &sts));
                        ثابت char16_t* منزل = ures_getString(bcp_Type_Alias_معلومات_داخلا.getAlias(), &منزل_Len, &sts);
                        إذا (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of bcp type
                        إذا (uprv_compareInvWithUChar(
                                خالي_اشارو,
                                t->bcpId.معلومات(),
                                static_cast<int32_t>(t->bcpId.size()),
                                منزل,
                                منزل_Len) == 0) {
                            ثابت حرف* from = ures_get_ڪنجي(bcp_Type_Alias_معلومات_داخلا.getAlias());
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

        Loc_Ext_ڪنجي_معلومات* ڪنجي_معلومات = g_Loc_Ext_ڪنجي_معلومات_Entries->create();
        إذا (ڪنجي_معلومات == خالي_اشارو) {
            sts = U_يادگيري_ورهاست_ERROR;
            اكسر;
        }
        ڪنجي_معلومات->bcpId = bcp_ڪنجي_Id;
        ڪنجي_معلومات->legacyId = legacy_ڪنجي_Id;
        ڪنجي_معلومات->specialTypes = specialTypes;
        ڪنجي_معلومات->typeMap.adoptInstead(type_معلومات_Map);

        uhash_put(g_Loc_Ext_ڪنجي_Map, &ڪنجي_معلومات->legacyId, ڪنجي_معلومات, &sts);
        إذا (legacy_ڪنجي_Id != bcp_ڪنجي_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ڪنجي_Map, &ڪنجي_معلومات->bcpId, ڪنجي_معلومات, &sts);
        }
        إذا (U_FAILURE(sts)) {
            اكسر;
        }
    }
}

منطقي
init() {
    UError_ڪوڊ sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ڪنجي_Map_Init_Once, &initFromResourceBundle, sts);
    إذا (U_FAILURE(sts)) {
        أرجع خطأ;
    }
    أرجع سچ;
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
is_Special_Type_Reorder_ڪوڊ(std::string_view val) {
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
is_Special_Type_Rg_ڪنجي_قدر(std::string_view val) {
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
ulocimp_منزل_Bcp_ڪنجي(std::string_view ڪنجي) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_ڪنجي_معلومات* ڪنجي_معلومات = static_cast<Loc_Ext_ڪنجي_معلومات*>(uhash_get(g_Loc_Ext_ڪنجي_Map, &ڪنجي));
    إذا (ڪنجي_معلومات != خالي_اشارو) {
        أرجع ڪنجي_معلومات->bcpId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_ڪنجي(std::string_view ڪنجي) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_ڪنجي_معلومات* ڪنجي_معلومات = static_cast<Loc_Ext_ڪنجي_معلومات*>(uhash_get(g_Loc_Ext_ڪنجي_Map, &ڪنجي));
    إذا (ڪنجي_معلومات != خالي_اشارو) {
        أرجع ڪنجي_معلومات->legacyId;
    }

    أرجع std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Bcp_Type(std::string_view ڪنجي, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_ڪنجي_معلومات* ڪنجي_معلومات = static_cast<Loc_Ext_ڪنجي_معلومات*>(uhash_get(g_Loc_Ext_ڪنجي_Map, &ڪنجي));
    إذا (ڪنجي_معلومات != خالي_اشارو) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ڪنجي_معلومات->typeMap.getAlias(), &type));
        إذا (t != خالي_اشارو) {
            أرجع t->bcpId;
        }
        إذا (ڪنجي_معلومات->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (ڪنجي_معلومات->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && ڪنجي_معلومات->specialTypes & SPECIALTYPE_REORDER_ڪوڊ) {
                matched = is_Special_Type_Reorder_ڪوڊ(type);
            }
            إذا (!matched && ڪنجي_معلومات->specialTypes & SPECIALTYPE_RG_ڪنجي_قدر) {
                matched = is_Special_Type_Rg_ڪنجي_قدر(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_منزل_Legacy_Type(std::string_view ڪنجي, std::string_view type U_LIFETIME_BOUND) {
    إذا (!init()) {
        أرجع std::nullopt;
    }

    Loc_Ext_ڪنجي_معلومات* ڪنجي_معلومات = static_cast<Loc_Ext_ڪنجي_معلومات*>(uhash_get(g_Loc_Ext_ڪنجي_Map, &ڪنجي));
    إذا (ڪنجي_معلومات != خالي_اشارو) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ڪنجي_معلومات->typeMap.getAlias(), &type));
        إذا (t != خالي_اشارو) {
            أرجع t->legacyId;
        }
        إذا (ڪنجي_معلومات->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = خطأ;
            إذا (ڪنجي_معلومات->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            إذا (!matched && ڪنجي_معلومات->specialTypes & SPECIALTYPE_REORDER_ڪوڊ) {
                matched = is_Special_Type_Reorder_ڪوڊ(type);
            }
            إذا (!matched && ڪنجي_معلومات->specialTypes & SPECIALTYPE_RG_ڪنجي_قدر) {
                matched = is_Special_Type_Rg_ڪنجي_قدر(type);
            }
            إذا (matched) {
                أرجع type;
            }
        }
    }

    أرجع std::nullopt;
}
