// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ਅਨ੍ਤਰ੍ਭਾਵ <algorithm>
#ਅਨ੍ਤਰ੍ਭਾਵ <optional>
#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fixedstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ulocimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h" /* for InvChar functions */

ਸਥਿਰ UHashtable* g_Loc_Ext_ਕੁੰਜੀ_Map = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce g_Loc_Ext_ਕੁੰਜੀ_Map_Init_Once {};

// bit flags for special types
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_ਕੋਡ = 2,
    SPECIALTYPE_RG_ਕੁੰਜੀ_ਮੁੱਲ = 4
} SpecialType;

ਬਣਤਰ Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ : ਜਨਤਕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_ਸੂਚਕ typeMap;
    uint32_t        specialTypes;
};

ਬਣਤਰ LocExtType : ਜਨਤਕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ਬਣਤਰ TypeAlias : ਜਨਤਕ icu::UMemory {
    std::string_view from;
};

ਸਥਿਰ icu::ਸਮ੍ਰਿਤੀ_Pool<icu::FixedString>* g_ਕੁੰਜੀ_Type_String_Pool = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::ਸਮ੍ਰਿਤੀ_Pool<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ>* g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::ਸਮ੍ਰਿਤੀ_Pool<LocExtType>* gLocExtTypeEntries = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::ਸਮ੍ਰਿਤੀ_Pool<TypeAlias>* gTypeAliasEntries = ਖਾਲੀ_ਸੂਚਕ;

U_CDECL_BEGIN

ਸਥਿਰ UBool U_CALLCONV
uloc_ਕੁੰਜੀ_type_cleanup() {
    ਜੇ (g_Loc_Ext_ਕੁੰਜੀ_Map != ਖਾਲੀ_ਸੂਚਕ) {
        uhash_close(g_Loc_Ext_ਕੁੰਜੀ_Map);
        g_Loc_Ext_ਕੁੰਜੀ_Map = ਖਾਲੀ_ਸੂਚਕ;
    }

    ਮਿਟਾਓ g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries;
    g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ gLocExtTypeEntries;
    gLocExtTypeEntries = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ gTypeAliasEntries;
    gTypeAliasEntries = ਖਾਲੀ_ਸੂਚਕ;

    ਮਿਟਾਓ g_ਕੁੰਜੀ_Type_String_Pool;
    g_ਕੁੰਜੀ_Type_String_Pool = ਖਾਲੀ_ਸੂਚਕ;

    g_Loc_Ext_ਕੁੰਜੀ_Map_Init_Once.reset();
    ਵਾਪਸ ਸੱਚ;
}

U_CDECL_ਸਿਰਾ

ਨਾਮਾਕਾਸ਼ {

ਖਾਲੀ U_CALLCONV
initFromResourceBundle(UError_ਕੋਡ& sts) {
    U_NAMESPACE_USE
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LOCALE_ਕੁੰਜੀ_TYPE, uloc_ਕੁੰਜੀ_type_cleanup);

    g_Loc_Ext_ਕੁੰਜੀ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, ਖਾਲੀ_ਸੂਚਕ, &sts);

    Local_UResource_Bundle_ਸੂਚਕ ਕੁੰਜੀ_Type_ਅੰਕੜੇ_Res(ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, "keyTypeData", &sts));
    Local_UResource_Bundle_ਸੂਚਕ ਕੁੰਜੀ_Map_Res(ures_get_By_ਕੁੰਜੀ(ਕੁੰਜੀ_Type_ਅੰਕੜੇ_Res.getAlias(), "keyMap", ਖਾਲੀ_ਸੂਚਕ, &sts));
    Local_UResource_Bundle_ਸੂਚਕ typeMapRes(ures_get_By_ਕੁੰਜੀ(ਕੁੰਜੀ_Type_ਅੰਕੜੇ_Res.getAlias(), "typeMap", ਖਾਲੀ_ਸੂਚਕ, &sts));

    ਜੇ (U_FAILURE(sts)) {
        ਵਾਪਸ;
    }

    UError_ਕੋਡ tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_ਸੂਚਕ typeAliasRes(ures_get_By_ਕੁੰਜੀ(ਕੁੰਜੀ_Type_ਅੰਕੜੇ_Res.getAlias(), "typeAlias", ਖਾਲੀ_ਸੂਚਕ, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_ਸੂਚਕ bcpTypeAliasRes(ures_get_By_ਕੁੰਜੀ(ਕੁੰਜੀ_Type_ਅੰਕੜੇ_Res.getAlias(), "bcpTypeAlias", ਖਾਲੀ_ਸੂਚਕ, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ਕੁੰਜੀ_Type_String_Pool = ਨਵਾਂ icu::ਸਮ੍ਰਿਤੀ_Pool<icu::FixedString>;
    ਜੇ (g_ਕੁੰਜੀ_Type_String_Pool == ਖਾਲੀ_ਸੂਚਕ) {
        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries = ਨਵਾਂ icu::ਸਮ੍ਰਿਤੀ_Pool<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ>;
    ਜੇ (g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries == ਖਾਲੀ_ਸੂਚਕ) {
        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    gLocExtTypeEntries = ਨਵਾਂ icu::ਸਮ੍ਰਿਤੀ_Pool<LocExtType>;
    ਜੇ (gLocExtTypeEntries == ਖਾਲੀ_ਸੂਚਕ) {
        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
    gTypeAliasEntries = ਨਵਾਂ icu::ਸਮ੍ਰਿਤੀ_Pool<TypeAlias>;
    ਜੇ (gTypeAliasEntries == ਖਾਲੀ_ਸੂਚਕ) {
        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_ਸੂਚਕ ਕੁੰਜੀ_Map_ਇੰਦਰਾਜ;

    ਜਦੋਂ (ures_has_ਅਗਲਾ(ਕੁੰਜੀ_Map_Res.getAlias())) {
        ਕੁੰਜੀ_Map_ਇੰਦਰਾਜ.adoptInstead(ures_get_ਅਗਲਾ_Resource(ਕੁੰਜੀ_Map_Res.getAlias(), ਕੁੰਜੀ_Map_ਇੰਦਰਾਜ.orphan(), &sts));
        ਜੇ (U_FAILURE(sts)) {
            ਰੋਕੋ;
        }
        ਅਚਲ ਅੱਖਰ* legacy_ਕੁੰਜੀ_Id = ures_get_ਕੁੰਜੀ(ਕੁੰਜੀ_Map_ਇੰਦਰਾਜ.getAlias());
        UnicodeString u_Bcp_ਕੁੰਜੀ_Id = ures_getUnicodeString(ਕੁੰਜੀ_Map_ਇੰਦਰਾਜ.getAlias(), &sts);
        ਜੇ (U_FAILURE(sts)) {
            ਰੋਕੋ;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ਅਚਲ ਅੱਖਰ* bcp_ਕੁੰਜੀ_Id = legacy_ਕੁੰਜੀ_Id;
        ਜੇ (!u_Bcp_ਕੁੰਜੀ_Id.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
            icu::FixedString* bcp_ਕੁੰਜੀ_Id_Buf = g_ਕੁੰਜੀ_Type_String_Pool->create();
            ਜੇ (bcp_ਕੁੰਜੀ_Id_Buf == ਖਾਲੀ_ਸੂਚਕ) {
                sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਰੋਕੋ;
            }
            copyInvariantChars(u_Bcp_ਕੁੰਜੀ_Id, *bcp_ਕੁੰਜੀ_Id_Buf, sts);
            ਜੇ (U_FAILURE(sts)) {
                ਰੋਕੋ;
            }
            bcp_ਕੁੰਜੀ_Id = bcp_ਕੁੰਜੀ_Id_Buf->ਅੰਕੜੇ();
        }

        ਤਾਰਕਿਕ isTZ = uprv_strcmp(legacy_ਕੁੰਜੀ_Id, "timezone") == 0;

        UHashtable* type_ਅੰਕੜੇ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, ਖਾਲੀ_ਸੂਚਕ, &sts);
        ਜੇ (U_FAILURE(sts)) {
            ਰੋਕੋ;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_ਸੂਚਕ type_Alias_Res_By_ਕੁੰਜੀ;
        Local_UResource_Bundle_ਸੂਚਕ bcp_Type_Alias_Res_By_ਕੁੰਜੀ;

        ਜੇ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ਕੁੰਜੀ.adoptInstead(ures_get_By_ਕੁੰਜੀ(typeAliasRes.getAlias(), legacy_ਕੁੰਜੀ_Id, ਖਾਲੀ_ਸੂਚਕ, &tmpSts));
            ਜੇ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ਕੁੰਜੀ.orphan();
            }
        }
        ਜੇ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ਕੁੰਜੀ.adoptInstead(ures_get_By_ਕੁੰਜੀ(bcpTypeAliasRes.getAlias(), bcp_ਕੁੰਜੀ_Id, ਖਾਲੀ_ਸੂਚਕ, &tmpSts));
            ਜੇ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ਕੁੰਜੀ.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_ਸੂਚਕ type_Map_Res_By_ਕੁੰਜੀ(ures_get_By_ਕੁੰਜੀ(typeMapRes.getAlias(), legacy_ਕੁੰਜੀ_Id, ਖਾਲੀ_ਸੂਚਕ, &sts));
        ਜੇ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ਨਹੀਂਤਾਂ {
            Local_UResource_Bundle_ਸੂਚਕ type_Map_ਇੰਦਰਾਜ;

            ਜਦੋਂ (ures_has_ਅਗਲਾ(type_Map_Res_By_ਕੁੰਜੀ.getAlias())) {
                type_Map_ਇੰਦਰਾਜ.adoptInstead(ures_get_ਅਗਲਾ_Resource(type_Map_Res_By_ਕੁੰਜੀ.getAlias(), type_Map_ਇੰਦਰਾਜ.orphan(), &sts));
                ਜੇ (U_FAILURE(sts)) {
                    ਰੋਕੋ;
                }
                ਅਚਲ ਅੱਖਰ* legacyTypeId = ures_get_ਕੁੰਜੀ(type_Map_ਇੰਦਰਾਜ.getAlias());

                // special types
                ਜੇ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ਜਾਰੀ;
                }
                ਜੇ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_ਕੋਡ;
                    ਜਾਰੀ;
                }
                ਜੇ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ਕੁੰਜੀ_ਮੁੱਲ;
                    ਜਾਰੀ;
                }

                ਜੇ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ਜੇ (uprv_strchr(legacyTypeId, ':') != ਖਾਲੀ_ਸੂਚਕ) {
                        U_ASSERT(legacyTypeId != ਖਾਲੀ_ਸੂਚਕ && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ਕੁੰਜੀ_Type_String_Pool->create(legacyTypeIdView);
                        ਜੇ (legacyTypeIdBuf == ਖਾਲੀ_ਸੂਚਕ || legacyTypeIdBuf->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                            sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                            ਰੋਕੋ;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->ਅੰਕੜੇ();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_ਇੰਦਰਾਜ.getAlias(), &sts);
                ਜੇ (U_FAILURE(sts)) {
                    ਰੋਕੋ;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ਅਚਲ ਅੱਖਰ* bcpTypeId = legacyTypeId;
                ਜੇ (!uBcpTypeId.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                    icu::FixedString* bcpTypeIdBuf = g_ਕੁੰਜੀ_Type_String_Pool->create();
                    ਜੇ (bcpTypeIdBuf == ਖਾਲੀ_ਸੂਚਕ) {
                        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                        ਰੋਕੋ;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ਜੇ (U_FAILURE(sts)) {
                        ਰੋਕੋ;
                    }
                    bcpTypeId = bcpTypeIdBuf->ਅੰਕੜੇ();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ਜੇ (t == ਖਾਲੀ_ਸੂਚਕ) {
                    sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                    ਰੋਕੋ;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_ਅੰਕੜੇ_Map, &t->legacyId, t, &sts);
                ਜੇ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_ਅੰਕੜੇ_Map, &t->bcpId, t, &sts);
                }
                ਜੇ (U_FAILURE(sts)) {
                    ਰੋਕੋ;
                }

                // also put aliases in the map
                ਜੇ (type_Alias_Res_By_ਕੁੰਜੀ.isValid()) {
                    Local_UResource_Bundle_ਸੂਚਕ type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ;

                    ures_resetIterator(type_Alias_Res_By_ਕੁੰਜੀ.getAlias());
                    ਜਦੋਂ (ures_has_ਅਗਲਾ(type_Alias_Res_By_ਕੁੰਜੀ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t ਮੰਜ਼ਿਲ_Len;
                        type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.adoptInstead(ures_get_ਅਗਲਾ_Resource(type_Alias_Res_By_ਕੁੰਜੀ.getAlias(), type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.orphan(), &sts));
                        ਅਚਲ char16_t* ਮੰਜ਼ਿਲ = ures_getString(type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.getAlias(), &ਮੰਜ਼ਿਲ_Len, &sts);
                        ਜੇ (U_FAILURE(sts)) {
                            ਰੋਕੋ;
                        }
                        // check if this is an alias of canonical legacy type
                        ਜੇ (uprv_compareInvWithUChar(
                                ਖਾਲੀ_ਸੂਚਕ,
                                t->legacyId.ਅੰਕੜੇ(),
                                static_cast<int32_t>(t->legacyId.size()),
                                ਮੰਜ਼ਿਲ,
                                ਮੰਜ਼ਿਲ_Len) == 0) {
                            ਅਚਲ ਅੱਖਰ* from = ures_get_ਕੁੰਜੀ(type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ਜੇ (isTZ) {
                                // replace colon with slash if necessary
                                ਜੇ (uprv_strchr(from, ':') != ਖਾਲੀ_ਸੂਚਕ) {
                                    U_ASSERT(from != ਖਾਲੀ_ਸੂਚਕ && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ਕੁੰਜੀ_Type_String_Pool->create(fromView);
                                    ਜੇ (fromBuf == ਖਾਲੀ_ਸੂਚਕ || fromBuf->is_ਖਾਲੀ_ਥਾਵਾਂ()) {
                                        sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                                        ਰੋਕੋ;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->ਅੰਕੜੇ(), fromView.length()};
                                }
                            }
                            uhash_put(type_ਅੰਕੜੇ_Map, &alias->from, t, &sts);
                        }
                    }
                    ਜੇ (U_FAILURE(sts)) {
                        ਰੋਕੋ;
                    }
                }

                ਜੇ (bcp_Type_Alias_Res_By_ਕੁੰਜੀ.isValid()) {
                    Local_UResource_Bundle_ਸੂਚਕ bcp_Type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ਕੁੰਜੀ.getAlias());
                    ਜਦੋਂ (ures_has_ਅਗਲਾ(bcp_Type_Alias_Res_By_ਕੁੰਜੀ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t ਮੰਜ਼ਿਲ_Len;
                        bcp_Type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.adoptInstead(ures_get_ਅਗਲਾ_Resource(bcp_Type_Alias_Res_By_ਕੁੰਜੀ.getAlias(), bcp_Type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.orphan(), &sts));
                        ਅਚਲ char16_t* ਮੰਜ਼ਿਲ = ures_getString(bcp_Type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.getAlias(), &ਮੰਜ਼ਿਲ_Len, &sts);
                        ਜੇ (U_FAILURE(sts)) {
                            ਰੋਕੋ;
                        }
                        // check if this is an alias of bcp type
                        ਜੇ (uprv_compareInvWithUChar(
                                ਖਾਲੀ_ਸੂਚਕ,
                                t->bcpId.ਅੰਕੜੇ(),
                                static_cast<int32_t>(t->bcpId.size()),
                                ਮੰਜ਼ਿਲ,
                                ਮੰਜ਼ਿਲ_Len) == 0) {
                            ਅਚਲ ਅੱਖਰ* from = ures_get_ਕੁੰਜੀ(bcp_Type_Alias_ਅੰਕੜੇ_ਇੰਦਰਾਜ.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_ਅੰਕੜੇ_Map, &alias->from, t, &sts);
                        }
                    }
                    ਜੇ (U_FAILURE(sts)) {
                        ਰੋਕੋ;
                    }
                }
            }
        }
        ਜੇ (U_FAILURE(sts)) {
            ਰੋਕੋ;
        }

        Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ* ਕੁੰਜੀ_ਅੰਕੜੇ = g_Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ_Entries->create();
        ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
            sts = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਰੋਕੋ;
        }
        ਕੁੰਜੀ_ਅੰਕੜੇ->bcpId = bcp_ਕੁੰਜੀ_Id;
        ਕੁੰਜੀ_ਅੰਕੜੇ->legacyId = legacy_ਕੁੰਜੀ_Id;
        ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes = specialTypes;
        ਕੁੰਜੀ_ਅੰਕੜੇ->typeMap.adoptInstead(type_ਅੰਕੜੇ_Map);

        uhash_put(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ_ਅੰਕੜੇ->legacyId, ਕੁੰਜੀ_ਅੰਕੜੇ, &sts);
        ਜੇ (legacy_ਕੁੰਜੀ_Id != bcp_ਕੁੰਜੀ_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ_ਅੰਕੜੇ->bcpId, ਕੁੰਜੀ_ਅੰਕੜੇ, &sts);
        }
        ਜੇ (U_FAILURE(sts)) {
            ਰੋਕੋ;
        }
    }
}

ਤਾਰਕਿਕ
init() {
    UError_ਕੋਡ sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ਕੁੰਜੀ_Map_Init_Once, &initFromResourceBundle, sts);
    ਜੇ (U_FAILURE(sts)) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    ਲਈ (ਅੱਖਰ c : val) {
        ਜੇ (c == '-') {
            ਜੇ (subtagLen < 4 || subtagLen > 6) {
                ਵਾਪਸ ਝੂਠ;
            }
            subtagLen = 0;
        } ਨਹੀਂਤਾਂ ਜੇ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ subtagLen >= 4 && subtagLen <= 6;
}

ਤਾਰਕਿਕ
is_Special_Type_Reorder_ਕੋਡ(std::string_view val) {
    int32_t subtagLen = 0;
    ਲਈ (ਅੱਖਰ c : val) {
        ਜੇ (c == '-') {
            ਜੇ (subtagLen < 3 || subtagLen > 8) {
                ਵਾਪਸ ਝੂਠ;
            }
            subtagLen = 0;
        } ਨਹੀਂਤਾਂ ਜੇ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ subtagLen >= 3 && subtagLen <= 8;
}

ਤਾਰਕਿਕ
is_Special_Type_Rg_ਕੁੰਜੀ_ਮੁੱਲ(std::string_view val) {
    int32_t subtagLen = 0;
    ਲਈ (ਅੱਖਰ c : val) {
        ਜੇ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਝੂਠ;
        }
    }
    ਵਾਪਸ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_ਕੁੰਜੀ(std::string_view ਕੁੰਜੀ) {
    ਜੇ (!init()) {
        ਵਾਪਸ std::nullopt;
    }

    Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ* ਕੁੰਜੀ_ਅੰਕੜੇ = static_cast<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ*>(uhash_get(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ));
    ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਕੁੰਜੀ_ਅੰਕੜੇ->bcpId;
    }

    ਵਾਪਸ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_ਕੁੰਜੀ(std::string_view ਕੁੰਜੀ) {
    ਜੇ (!init()) {
        ਵਾਪਸ std::nullopt;
    }

    Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ* ਕੁੰਜੀ_ਅੰਕੜੇ = static_cast<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ*>(uhash_get(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ));
    ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਕੁੰਜੀ_ਅੰਕੜੇ->legacyId;
    }

    ਵਾਪਸ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Bcp_Type(std::string_view ਕੁੰਜੀ, std::string_view type U_LIFETIME_BOUND) {
    ਜੇ (!init()) {
        ਵਾਪਸ std::nullopt;
    }

    Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ* ਕੁੰਜੀ_ਅੰਕੜੇ = static_cast<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ*>(uhash_get(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ));
    ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ਕੁੰਜੀ_ਅੰਕੜੇ->typeMap.getAlias(), &type));
        ਜੇ (t != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ t->bcpId;
        }
        ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes != SPECIALTYPE_NONE) {
            ਤਾਰਕਿਕ matched = ਝੂਠ;
            ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ਜੇ (!matched && ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_REORDER_ਕੋਡ) {
                matched = is_Special_Type_Reorder_ਕੋਡ(type);
            }
            ਜੇ (!matched && ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_RG_ਕੁੰਜੀ_ਮੁੱਲ) {
                matched = is_Special_Type_Rg_ਕੁੰਜੀ_ਮੁੱਲ(type);
            }
            ਜੇ (matched) {
                ਵਾਪਸ type;
            }
        }
    }

    ਵਾਪਸ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_ਮੰਜ਼ਿਲ_Legacy_Type(std::string_view ਕੁੰਜੀ, std::string_view type U_LIFETIME_BOUND) {
    ਜੇ (!init()) {
        ਵਾਪਸ std::nullopt;
    }

    Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ* ਕੁੰਜੀ_ਅੰਕੜੇ = static_cast<Loc_Ext_ਕੁੰਜੀ_ਅੰਕੜੇ*>(uhash_get(g_Loc_Ext_ਕੁੰਜੀ_Map, &ਕੁੰਜੀ));
    ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ਕੁੰਜੀ_ਅੰਕੜੇ->typeMap.getAlias(), &type));
        ਜੇ (t != ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ t->legacyId;
        }
        ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes != SPECIALTYPE_NONE) {
            ਤਾਰਕਿਕ matched = ਝੂਠ;
            ਜੇ (ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ਜੇ (!matched && ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_REORDER_ਕੋਡ) {
                matched = is_Special_Type_Reorder_ਕੋਡ(type);
            }
            ਜੇ (!matched && ਕੁੰਜੀ_ਅੰਕੜੇ->specialTypes & SPECIALTYPE_RG_ਕੁੰਜੀ_ਮੁੱਲ) {
                matched = is_Special_Type_Rg_ਕੁੰਜੀ_ਮੁੱਲ(type);
            }
            ਜੇ (matched) {
                ਵਾਪਸ type;
            }
        }
    }

    ਵਾਪਸ std::nullopt;
}
