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

ساکن UHashtable* g_Loc_Ext_کلید_Map = خالی_اشاریہ;
ساکن icu::UInitOnce g_Loc_Ext_کلید_Map_Init_Once {};

// bit flags for special types
قسمتعریف شمار {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_رمز = 2,
    SPECIALTYPE_RG_کلید_قدر = 4
} SpecialType;

ساخت Loc_Ext_کلید_ڈیٹا : عوامی icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_اشارہ_گر typeMap;
    uint32_t        specialTypes;
};

ساخت LocExtType : عوامی icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ساخت TypeAlias : عوامی icu::UMemory {
    std::string_view from;
};

ساکن icu::حافظہ_Pool<icu::FixedString>* g_کلید_Type_String_Pool = خالی_اشاریہ;
ساکن icu::حافظہ_Pool<Loc_Ext_کلید_ڈیٹا>* g_Loc_Ext_کلید_ڈیٹا_Entries = خالی_اشاریہ;
ساکن icu::حافظہ_Pool<LocExtType>* gLocExtTypeEntries = خالی_اشاریہ;
ساکن icu::حافظہ_Pool<TypeAlias>* gTypeAliasEntries = خالی_اشاریہ;

U_CDECL_BEGIN

ساکن UBool U_CALLCONV
uloc_کلید_type_cleanup() {
    اگر (g_Loc_Ext_کلید_Map != خالی_اشاریہ) {
        uhash_close(g_Loc_Ext_کلید_Map);
        g_Loc_Ext_کلید_Map = خالی_اشاریہ;
    }

    حذف g_Loc_Ext_کلید_ڈیٹا_Entries;
    g_Loc_Ext_کلید_ڈیٹا_Entries = خالی_اشاریہ;

    حذف gLocExtTypeEntries;
    gLocExtTypeEntries = خالی_اشاریہ;

    حذف gTypeAliasEntries;
    gTypeAliasEntries = خالی_اشاریہ;

    حذف g_کلید_Type_String_Pool;
    g_کلید_Type_String_Pool = خالی_اشاریہ;

    g_Loc_Ext_کلید_Map_Init_Once.reset();
    واپس سچ;
}

U_CDECL_انتہا

نامخلا {

خالی U_CALLCONV
initFromResourceBundle(UError_رمز& sts) {
    U_NAMESPACE_USE
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LOCALE_کلید_TYPE, uloc_کلید_type_cleanup);

    g_Loc_Ext_کلید_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, خالی_اشاریہ, &sts);

    Local_UResource_Bundle_اشارہ_گر کلید_Type_ڈیٹا_Res(ures_open_براہ_راست(خالی_اشاریہ, "keyTypeData", &sts));
    Local_UResource_Bundle_اشارہ_گر کلید_Map_Res(ures_get_By_کلید(کلید_Type_ڈیٹا_Res.getAlias(), "keyMap", خالی_اشاریہ, &sts));
    Local_UResource_Bundle_اشارہ_گر typeMapRes(ures_get_By_کلید(کلید_Type_ڈیٹا_Res.getAlias(), "typeMap", خالی_اشاریہ, &sts));

    اگر (U_FAILURE(sts)) {
        واپس;
    }

    UError_رمز tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر typeAliasRes(ures_get_By_کلید(کلید_Type_ڈیٹا_Res.getAlias(), "typeAlias", خالی_اشاریہ, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_اشارہ_گر bcpTypeAliasRes(ures_get_By_کلید(کلید_Type_ڈیٹا_Res.getAlias(), "bcpTypeAlias", خالی_اشاریہ, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_کلید_Type_String_Pool = نیا icu::حافظہ_Pool<icu::FixedString>;
    اگر (g_کلید_Type_String_Pool == خالی_اشاریہ) {
        sts = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    g_Loc_Ext_کلید_ڈیٹا_Entries = نیا icu::حافظہ_Pool<Loc_Ext_کلید_ڈیٹا>;
    اگر (g_Loc_Ext_کلید_ڈیٹا_Entries == خالی_اشاریہ) {
        sts = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    gLocExtTypeEntries = نیا icu::حافظہ_Pool<LocExtType>;
    اگر (gLocExtTypeEntries == خالی_اشاریہ) {
        sts = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
    gTypeAliasEntries = نیا icu::حافظہ_Pool<TypeAlias>;
    اگر (gTypeAliasEntries == خالی_اشاریہ) {
        sts = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_اشارہ_گر کلید_Map_اندراج;

    جب (ures_has_اگلا(کلید_Map_Res.getAlias())) {
        کلید_Map_اندراج.adoptInstead(ures_get_اگلا_Resource(کلید_Map_Res.getAlias(), کلید_Map_اندراج.orphan(), &sts));
        اگر (U_FAILURE(sts)) {
            توڑو;
        }
        ثابت حرف* legacy_کلید_Id = ures_get_کلید(کلید_Map_اندراج.getAlias());
        UnicodeString u_Bcp_کلید_Id = ures_getUnicodeString(کلید_Map_اندراج.getAlias(), &sts);
        اگر (U_FAILURE(sts)) {
            توڑو;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcp_کلید_Id = legacy_کلید_Id;
        اگر (!u_Bcp_کلید_Id.is_خالی_خانے()) {
            icu::FixedString* bcp_کلید_Id_Buf = g_کلید_Type_String_Pool->create();
            اگر (bcp_کلید_Id_Buf == خالی_اشاریہ) {
                sts = U_حافظہ_تخصیص_ERROR;
                توڑو;
            }
            copyInvariantChars(u_Bcp_کلید_Id, *bcp_کلید_Id_Buf, sts);
            اگر (U_FAILURE(sts)) {
                توڑو;
            }
            bcp_کلید_Id = bcp_کلید_Id_Buf->ڈیٹا();
        }

        منطقی isTZ = uprv_strcmp(legacy_کلید_Id, "timezone") == 0;

        UHashtable* type_ڈیٹا_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, خالی_اشاریہ, &sts);
        اگر (U_FAILURE(sts)) {
            توڑو;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_اشارہ_گر type_Alias_Res_By_کلید;
        Local_UResource_Bundle_اشارہ_گر bcp_Type_Alias_Res_By_کلید;

        اگر (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(typeAliasRes.getAlias(), legacy_کلید_Id, خالی_اشاریہ, &tmpSts));
            اگر (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_کلید.orphan();
            }
        }
        اگر (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_کلید.adoptInstead(ures_get_By_کلید(bcpTypeAliasRes.getAlias(), bcp_کلید_Id, خالی_اشاریہ, &tmpSts));
            اگر (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_کلید.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_اشارہ_گر type_Map_Res_By_کلید(ures_get_By_کلید(typeMapRes.getAlias(), legacy_کلید_Id, خالی_اشاریہ, &sts));
        اگر (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ورنہ {
            Local_UResource_Bundle_اشارہ_گر type_Map_اندراج;

            جب (ures_has_اگلا(type_Map_Res_By_کلید.getAlias())) {
                type_Map_اندراج.adoptInstead(ures_get_اگلا_Resource(type_Map_Res_By_کلید.getAlias(), type_Map_اندراج.orphan(), &sts));
                اگر (U_FAILURE(sts)) {
                    توڑو;
                }
                ثابت حرف* legacyTypeId = ures_get_کلید(type_Map_اندراج.getAlias());

                // special types
                اگر (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    جاری;
                }
                اگر (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_رمز;
                    جاری;
                }
                اگر (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_کلید_قدر;
                    جاری;
                }

                اگر (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    اگر (uprv_strchr(legacyTypeId, ':') != خالی_اشاریہ) {
                        U_ASSERT(legacyTypeId != خالی_اشاریہ && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_کلید_Type_String_Pool->create(legacyTypeIdView);
                        اگر (legacyTypeIdBuf == خالی_اشاریہ || legacyTypeIdBuf->is_خالی_خانے()) {
                            sts = U_حافظہ_تخصیص_ERROR;
                            توڑو;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->ڈیٹا();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_اندراج.getAlias(), &sts);
                اگر (U_FAILURE(sts)) {
                    توڑو;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                اگر (!uBcpTypeId.is_خالی_خانے()) {
                    icu::FixedString* bcpTypeIdBuf = g_کلید_Type_String_Pool->create();
                    اگر (bcpTypeIdBuf == خالی_اشاریہ) {
                        sts = U_حافظہ_تخصیص_ERROR;
                        توڑو;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    اگر (U_FAILURE(sts)) {
                        توڑو;
                    }
                    bcpTypeId = bcpTypeIdBuf->ڈیٹا();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                اگر (t == خالی_اشاریہ) {
                    sts = U_حافظہ_تخصیص_ERROR;
                    توڑو;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_ڈیٹا_Map, &t->legacyId, t, &sts);
                اگر (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_ڈیٹا_Map, &t->bcpId, t, &sts);
                }
                اگر (U_FAILURE(sts)) {
                    توڑو;
                }

                // also put aliases in the map
                اگر (type_Alias_Res_By_کلید.isValid()) {
                    Local_UResource_Bundle_اشارہ_گر type_Alias_ڈیٹا_اندراج;

                    ures_resetIterator(type_Alias_Res_By_کلید.getAlias());
                    جب (ures_has_اگلا(type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t مقصد_Len;
                        type_Alias_ڈیٹا_اندراج.adoptInstead(ures_get_اگلا_Resource(type_Alias_Res_By_کلید.getAlias(), type_Alias_ڈیٹا_اندراج.orphan(), &sts));
                        ثابت char16_t* مقصد = ures_getString(type_Alias_ڈیٹا_اندراج.getAlias(), &مقصد_Len, &sts);
                        اگر (U_FAILURE(sts)) {
                            توڑو;
                        }
                        // check if this is an alias of canonical legacy type
                        اگر (uprv_compareInvWithUChar(
                                خالی_اشاریہ,
                                t->legacyId.ڈیٹا(),
                                static_cast<int32_t>(t->legacyId.size()),
                                مقصد,
                                مقصد_Len) == 0) {
                            ثابت حرف* from = ures_get_کلید(type_Alias_ڈیٹا_اندراج.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            اگر (isTZ) {
                                // replace colon with slash if necessary
                                اگر (uprv_strchr(from, ':') != خالی_اشاریہ) {
                                    U_ASSERT(from != خالی_اشاریہ && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_کلید_Type_String_Pool->create(fromView);
                                    اگر (fromBuf == خالی_اشاریہ || fromBuf->is_خالی_خانے()) {
                                        sts = U_حافظہ_تخصیص_ERROR;
                                        توڑو;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->ڈیٹا(), fromView.length()};
                                }
                            }
                            uhash_put(type_ڈیٹا_Map, &alias->from, t, &sts);
                        }
                    }
                    اگر (U_FAILURE(sts)) {
                        توڑو;
                    }
                }

                اگر (bcp_Type_Alias_Res_By_کلید.isValid()) {
                    Local_UResource_Bundle_اشارہ_گر bcp_Type_Alias_ڈیٹا_اندراج;

                    ures_resetIterator(bcp_Type_Alias_Res_By_کلید.getAlias());
                    جب (ures_has_اگلا(bcp_Type_Alias_Res_By_کلید.getAlias()) && U_SUCCESS(sts)) {
                        int32_t مقصد_Len;
                        bcp_Type_Alias_ڈیٹا_اندراج.adoptInstead(ures_get_اگلا_Resource(bcp_Type_Alias_Res_By_کلید.getAlias(), bcp_Type_Alias_ڈیٹا_اندراج.orphan(), &sts));
                        ثابت char16_t* مقصد = ures_getString(bcp_Type_Alias_ڈیٹا_اندراج.getAlias(), &مقصد_Len, &sts);
                        اگر (U_FAILURE(sts)) {
                            توڑو;
                        }
                        // check if this is an alias of bcp type
                        اگر (uprv_compareInvWithUChar(
                                خالی_اشاریہ,
                                t->bcpId.ڈیٹا(),
                                static_cast<int32_t>(t->bcpId.size()),
                                مقصد,
                                مقصد_Len) == 0) {
                            ثابت حرف* from = ures_get_کلید(bcp_Type_Alias_ڈیٹا_اندراج.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_ڈیٹا_Map, &alias->from, t, &sts);
                        }
                    }
                    اگر (U_FAILURE(sts)) {
                        توڑو;
                    }
                }
            }
        }
        اگر (U_FAILURE(sts)) {
            توڑو;
        }

        Loc_Ext_کلید_ڈیٹا* کلید_ڈیٹا = g_Loc_Ext_کلید_ڈیٹا_Entries->create();
        اگر (کلید_ڈیٹا == خالی_اشاریہ) {
            sts = U_حافظہ_تخصیص_ERROR;
            توڑو;
        }
        کلید_ڈیٹا->bcpId = bcp_کلید_Id;
        کلید_ڈیٹا->legacyId = legacy_کلید_Id;
        کلید_ڈیٹا->specialTypes = specialTypes;
        کلید_ڈیٹا->typeMap.adoptInstead(type_ڈیٹا_Map);

        uhash_put(g_Loc_Ext_کلید_Map, &کلید_ڈیٹا->legacyId, کلید_ڈیٹا, &sts);
        اگر (legacy_کلید_Id != bcp_کلید_Id) {
            // different key value
            uhash_put(g_Loc_Ext_کلید_Map, &کلید_ڈیٹا->bcpId, کلید_ڈیٹا, &sts);
        }
        اگر (U_FAILURE(sts)) {
            توڑو;
        }
    }
}

منطقی
init() {
    UError_رمز sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_کلید_Map_Init_Once, &initFromResourceBundle, sts);
    اگر (U_FAILURE(sts)) {
        واپس جھوٹ;
    }
    واپس سچ;
}

منطقی
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    برائے (حرف c : val) {
        اگر (c == '-') {
            اگر (subtagLen < 4 || subtagLen > 6) {
                واپس جھوٹ;
            }
            subtagLen = 0;
        } ورنہ اگر ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ورنہ {
            واپس جھوٹ;
        }
    }
    واپس subtagLen >= 4 && subtagLen <= 6;
}

منطقی
is_Special_Type_Reorder_رمز(std::string_view val) {
    int32_t subtagLen = 0;
    برائے (حرف c : val) {
        اگر (c == '-') {
            اگر (subtagLen < 3 || subtagLen > 8) {
                واپس جھوٹ;
            }
            subtagLen = 0;
        } ورنہ اگر (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ورنہ {
            واپس جھوٹ;
        }
    }
    واپس subtagLen >= 3 && subtagLen <= 8;
}

منطقی
is_Special_Type_Rg_کلید_قدر(std::string_view val) {
    int32_t subtagLen = 0;
    برائے (حرف c : val) {
        اگر ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ورنہ {
            واپس جھوٹ;
        }
    }
    واپس subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Bcp_کلید(std::string_view کلید) {
    اگر (!init()) {
        واپس std::nullopt;
    }

    Loc_Ext_کلید_ڈیٹا* کلید_ڈیٹا = static_cast<Loc_Ext_کلید_ڈیٹا*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_ڈیٹا != خالی_اشاریہ) {
        واپس کلید_ڈیٹا->bcpId;
    }

    واپس std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_کلید(std::string_view کلید) {
    اگر (!init()) {
        واپس std::nullopt;
    }

    Loc_Ext_کلید_ڈیٹا* کلید_ڈیٹا = static_cast<Loc_Ext_کلید_ڈیٹا*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_ڈیٹا != خالی_اشاریہ) {
        واپس کلید_ڈیٹا->legacyId;
    }

    واپس std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Bcp_Type(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    اگر (!init()) {
        واپس std::nullopt;
    }

    Loc_Ext_کلید_ڈیٹا* کلید_ڈیٹا = static_cast<Loc_Ext_کلید_ڈیٹا*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_ڈیٹا != خالی_اشاریہ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_ڈیٹا->typeMap.getAlias(), &type));
        اگر (t != خالی_اشاریہ) {
            واپس t->bcpId;
        }
        اگر (کلید_ڈیٹا->specialTypes != SPECIALTYPE_NONE) {
            منطقی matched = جھوٹ;
            اگر (کلید_ڈیٹا->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            اگر (!matched && کلید_ڈیٹا->specialTypes & SPECIALTYPE_REORDER_رمز) {
                matched = is_Special_Type_Reorder_رمز(type);
            }
            اگر (!matched && کلید_ڈیٹا->specialTypes & SPECIALTYPE_RG_کلید_قدر) {
                matched = is_Special_Type_Rg_کلید_قدر(type);
            }
            اگر (matched) {
                واپس type;
            }
        }
    }

    واپس std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_مقصد_Legacy_Type(std::string_view کلید, std::string_view type U_LIFETIME_BOUND) {
    اگر (!init()) {
        واپس std::nullopt;
    }

    Loc_Ext_کلید_ڈیٹا* کلید_ڈیٹا = static_cast<Loc_Ext_کلید_ڈیٹا*>(uhash_get(g_Loc_Ext_کلید_Map, &کلید));
    اگر (کلید_ڈیٹا != خالی_اشاریہ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(کلید_ڈیٹا->typeMap.getAlias(), &type));
        اگر (t != خالی_اشاریہ) {
            واپس t->legacyId;
        }
        اگر (کلید_ڈیٹا->specialTypes != SPECIALTYPE_NONE) {
            منطقی matched = جھوٹ;
            اگر (کلید_ڈیٹا->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            اگر (!matched && کلید_ڈیٹا->specialTypes & SPECIALTYPE_REORDER_رمز) {
                matched = is_Special_Type_Reorder_رمز(type);
            }
            اگر (!matched && کلید_ڈیٹا->specialTypes & SPECIALTYPE_RG_کلید_قدر) {
                matched = is_Special_Type_Rg_کلید_قدر(type);
            }
            اگر (matched) {
                واپس type;
            }
        }
    }

    واپس std::nullopt;
}
