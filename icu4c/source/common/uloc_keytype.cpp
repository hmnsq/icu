// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ಅನ್ತರ್ಭಾವ <algorithm>
#ಅನ್ತರ್ಭಾವ <optional>
#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "fixedstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "ulocimp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h" /* for InvChar functions */

ಸ್ಥಿರ UHashtable* g_Loc_Ext_ಕೀಲಿ_Map = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce g_Loc_Ext_ಕೀಲಿ_Map_Init_Once {};

// bit flags for special types
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_ಸಂಕೇತ = 2,
    SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ = 4
} SpecialType;

ರಚನೆ Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_ಸೂಚಕ typeMap;
    uint32_t        specialTypes;
};

ರಚನೆ LocExtType : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ರಚನೆ TypeAlias : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view from;
};

ಸ್ಥಿರ icu::ಸ್ಮರಣೆ_Pool<icu::FixedString>* g_ಕೀಲಿ_Type_String_Pool = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::ಸ್ಮರಣೆ_Pool<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ>* g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::ಸ್ಮರಣೆ_Pool<LocExtType>* gLocExtTypeEntries = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::ಸ್ಮರಣೆ_Pool<TypeAlias>* gTypeAliasEntries = ಶೂನ್ಯಸೂಚಕ;

U_CDECL_BEGIN

ಸ್ಥಿರ UBool U_CALLCONV
uloc_ಕೀಲಿ_type_cleanup() {
    ಆದರೆ (g_Loc_Ext_ಕೀಲಿ_Map != ಶೂನ್ಯಸೂಚಕ) {
        uhash_close(g_Loc_Ext_ಕೀಲಿ_Map);
        g_Loc_Ext_ಕೀಲಿ_Map = ಶೂನ್ಯಸೂಚಕ;
    }

    ಅಳಿಸು g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries;
    g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು gLocExtTypeEntries;
    gLocExtTypeEntries = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು gTypeAliasEntries;
    gTypeAliasEntries = ಶೂನ್ಯಸೂಚಕ;

    ಅಳಿಸು g_ಕೀಲಿ_Type_String_Pool;
    g_ಕೀಲಿ_Type_String_Pool = ಶೂನ್ಯಸೂಚಕ;

    g_Loc_Ext_ಕೀಲಿ_Map_Init_Once.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_CDECL_ಕೊನೆ

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ U_CALLCONV
initFromResourceBundle(UError_ಸಂಕೇತ& sts) {
    U_NAMESPACE_USE
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LOCALE_ಕೀಲಿ_TYPE, uloc_ಕೀಲಿ_type_cleanup);

    g_Loc_Ext_ಕೀಲಿ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, ಶೂನ್ಯಸೂಚಕ, &sts);

    Local_UResource_Bundle_ಸೂಚಕ ಕೀಲಿ_Type_ದತ್ತಾಂಶ_Res(ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, "keyTypeData", &sts));
    Local_UResource_Bundle_ಸೂಚಕ ಕೀಲಿ_Map_Res(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_ದತ್ತಾಂಶ_Res.getAlias(), "keyMap", ಶೂನ್ಯಸೂಚಕ, &sts));
    Local_UResource_Bundle_ಸೂಚಕ typeMapRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_ದತ್ತಾಂಶ_Res.getAlias(), "typeMap", ಶೂನ್ಯಸೂಚಕ, &sts));

    ಆದರೆ (U_FAILURE(sts)) {
        ಹಿಂತಿರುಗಿ;
    }

    UError_ಸಂಕೇತ tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_ಸೂಚಕ typeAliasRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_ದತ್ತಾಂಶ_Res.getAlias(), "typeAlias", ಶೂನ್ಯಸೂಚಕ, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_ಸೂಚಕ bcpTypeAliasRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_ದತ್ತಾಂಶ_Res.getAlias(), "bcpTypeAlias", ಶೂನ್ಯಸೂಚಕ, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ಕೀಲಿ_Type_String_Pool = ಹೊಸ icu::ಸ್ಮರಣೆ_Pool<icu::FixedString>;
    ಆದರೆ (g_ಕೀಲಿ_Type_String_Pool == ಶೂನ್ಯಸೂಚಕ) {
        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries = ಹೊಸ icu::ಸ್ಮರಣೆ_Pool<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ>;
    ಆದರೆ (g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries == ಶೂನ್ಯಸೂಚಕ) {
        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    gLocExtTypeEntries = ಹೊಸ icu::ಸ್ಮರಣೆ_Pool<LocExtType>;
    ಆದರೆ (gLocExtTypeEntries == ಶೂನ್ಯಸೂಚಕ) {
        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
    gTypeAliasEntries = ಹೊಸ icu::ಸ್ಮರಣೆ_Pool<TypeAlias>;
    ಆದರೆ (gTypeAliasEntries == ಶೂನ್ಯಸೂಚಕ) {
        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_ಸೂಚಕ ಕೀಲಿ_Map_ನಮೂದು;

    ಆಗ (ures_has_ಮುಂದಿನ(ಕೀಲಿ_Map_Res.getAlias())) {
        ಕೀಲಿ_Map_ನಮೂದು.adoptInstead(ures_get_ಮುಂದಿನ_Resource(ಕೀಲಿ_Map_Res.getAlias(), ಕೀಲಿ_Map_ನಮೂದು.orphan(), &sts));
        ಆದರೆ (U_FAILURE(sts)) {
            ನಿಲ್ಲಿಸು;
        }
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* legacy_ಕೀಲಿ_Id = ures_get_ಕೀಲಿ(ಕೀಲಿ_Map_ನಮೂದು.getAlias());
        UnicodeString u_Bcp_ಕೀಲಿ_Id = ures_getUnicodeString(ಕೀಲಿ_Map_ನಮೂದು.getAlias(), &sts);
        ಆದರೆ (U_FAILURE(sts)) {
            ನಿಲ್ಲಿಸು;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* bcp_ಕೀಲಿ_Id = legacy_ಕೀಲಿ_Id;
        ಆದರೆ (!u_Bcp_ಕೀಲಿ_Id.is_ಖಾಲಿಗಳು()) {
            icu::FixedString* bcp_ಕೀಲಿ_Id_Buf = g_ಕೀಲಿ_Type_String_Pool->create();
            ಆದರೆ (bcp_ಕೀಲಿ_Id_Buf == ಶೂನ್ಯಸೂಚಕ) {
                sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            copyInvariantChars(u_Bcp_ಕೀಲಿ_Id, *bcp_ಕೀಲಿ_Id_Buf, sts);
            ಆದರೆ (U_FAILURE(sts)) {
                ನಿಲ್ಲಿಸು;
            }
            bcp_ಕೀಲಿ_Id = bcp_ಕೀಲಿ_Id_Buf->ದತ್ತಾಂಶ();
        }

        ತಾರ್ಕಿಕ isTZ = uprv_strcmp(legacy_ಕೀಲಿ_Id, "timezone") == 0;

        UHashtable* type_ದತ್ತಾಂಶ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, ಶೂನ್ಯಸೂಚಕ, &sts);
        ಆದರೆ (U_FAILURE(sts)) {
            ನಿಲ್ಲಿಸು;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_ಸೂಚಕ type_Alias_Res_By_ಕೀಲಿ;
        Local_UResource_Bundle_ಸೂಚಕ bcp_Type_Alias_Res_By_ಕೀಲಿ;

        ಆದರೆ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ಕೀಲಿ.adoptInstead(ures_get_By_ಕೀಲಿ(typeAliasRes.getAlias(), legacy_ಕೀಲಿ_Id, ಶೂನ್ಯಸೂಚಕ, &tmpSts));
            ಆದರೆ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ಕೀಲಿ.orphan();
            }
        }
        ಆದರೆ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ಕೀಲಿ.adoptInstead(ures_get_By_ಕೀಲಿ(bcpTypeAliasRes.getAlias(), bcp_ಕೀಲಿ_Id, ಶೂನ್ಯಸೂಚಕ, &tmpSts));
            ಆದರೆ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ಕೀಲಿ.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_ಸೂಚಕ type_Map_Res_By_ಕೀಲಿ(ures_get_By_ಕೀಲಿ(typeMapRes.getAlias(), legacy_ಕೀಲಿ_Id, ಶೂನ್ಯಸೂಚಕ, &sts));
        ಆದರೆ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ಇಲ್ಲದಿದ್ದರೆ {
            Local_UResource_Bundle_ಸೂಚಕ type_Map_ನಮೂದು;

            ಆಗ (ures_has_ಮುಂದಿನ(type_Map_Res_By_ಕೀಲಿ.getAlias())) {
                type_Map_ನಮೂದು.adoptInstead(ures_get_ಮುಂದಿನ_Resource(type_Map_Res_By_ಕೀಲಿ.getAlias(), type_Map_ನಮೂದು.orphan(), &sts));
                ಆದರೆ (U_FAILURE(sts)) {
                    ನಿಲ್ಲಿಸು;
                }
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* legacyTypeId = ures_get_ಕೀಲಿ(type_Map_ನಮೂದು.getAlias());

                // special types
                ಆದರೆ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ಮುಂದುವರಿ;
                }
                ಆದರೆ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_ಸಂಕೇತ;
                    ಮುಂದುವರಿ;
                }
                ಆದರೆ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ;
                    ಮುಂದುವರಿ;
                }

                ಆದರೆ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ಆದರೆ (uprv_strchr(legacyTypeId, ':') != ಶೂನ್ಯಸೂಚಕ) {
                        U_ASSERT(legacyTypeId != ಶೂನ್ಯಸೂಚಕ && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ಕೀಲಿ_Type_String_Pool->create(legacyTypeIdView);
                        ಆದರೆ (legacyTypeIdBuf == ಶೂನ್ಯಸೂಚಕ || legacyTypeIdBuf->is_ಖಾಲಿಗಳು()) {
                            sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                            ನಿಲ್ಲಿಸು;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->ದತ್ತಾಂಶ();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_ನಮೂದು.getAlias(), &sts);
                ಆದರೆ (U_FAILURE(sts)) {
                    ನಿಲ್ಲಿಸು;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* bcpTypeId = legacyTypeId;
                ಆದರೆ (!uBcpTypeId.is_ಖಾಲಿಗಳು()) {
                    icu::FixedString* bcpTypeIdBuf = g_ಕೀಲಿ_Type_String_Pool->create();
                    ಆದರೆ (bcpTypeIdBuf == ಶೂನ್ಯಸೂಚಕ) {
                        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                        ನಿಲ್ಲಿಸು;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ಆದರೆ (U_FAILURE(sts)) {
                        ನಿಲ್ಲಿಸು;
                    }
                    bcpTypeId = bcpTypeIdBuf->ದತ್ತಾಂಶ();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ಆದರೆ (t == ಶೂನ್ಯಸೂಚಕ) {
                    sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                    ನಿಲ್ಲಿಸು;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_ದತ್ತಾಂಶ_Map, &t->legacyId, t, &sts);
                ಆದರೆ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_ದತ್ತಾಂಶ_Map, &t->bcpId, t, &sts);
                }
                ಆದರೆ (U_FAILURE(sts)) {
                    ನಿಲ್ಲಿಸು;
                }

                // also put aliases in the map
                ಆದರೆ (type_Alias_Res_By_ಕೀಲಿ.isValid()) {
                    Local_UResource_Bundle_ಸೂಚಕ type_Alias_ದತ್ತಾಂಶ_ನಮೂದು;

                    ures_resetIterator(type_Alias_Res_By_ಕೀಲಿ.getAlias());
                    ಆಗ (ures_has_ಮುಂದಿನ(type_Alias_Res_By_ಕೀಲಿ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t ಗಮ್ಯ_Len;
                        type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.adoptInstead(ures_get_ಮುಂದಿನ_Resource(type_Alias_Res_By_ಕೀಲಿ.getAlias(), type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.orphan(), &sts));
                        ಸ್ಥಿರಾಂಕ char16_t* ಗಮ್ಯ = ures_getString(type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.getAlias(), &ಗಮ್ಯ_Len, &sts);
                        ಆದರೆ (U_FAILURE(sts)) {
                            ನಿಲ್ಲಿಸು;
                        }
                        // check if this is an alias of canonical legacy type
                        ಆದರೆ (uprv_compareInvWithUChar(
                                ಶೂನ್ಯಸೂಚಕ,
                                t->legacyId.ದತ್ತಾಂಶ(),
                                static_cast<int32_t>(t->legacyId.size()),
                                ಗಮ್ಯ,
                                ಗಮ್ಯ_Len) == 0) {
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* from = ures_get_ಕೀಲಿ(type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ಆದರೆ (isTZ) {
                                // replace colon with slash if necessary
                                ಆದರೆ (uprv_strchr(from, ':') != ಶೂನ್ಯಸೂಚಕ) {
                                    U_ASSERT(from != ಶೂನ್ಯಸೂಚಕ && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ಕೀಲಿ_Type_String_Pool->create(fromView);
                                    ಆದರೆ (fromBuf == ಶೂನ್ಯಸೂಚಕ || fromBuf->is_ಖಾಲಿಗಳು()) {
                                        sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                                        ನಿಲ್ಲಿಸು;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->ದತ್ತಾಂಶ(), fromView.length()};
                                }
                            }
                            uhash_put(type_ದತ್ತಾಂಶ_Map, &alias->from, t, &sts);
                        }
                    }
                    ಆದರೆ (U_FAILURE(sts)) {
                        ನಿಲ್ಲಿಸು;
                    }
                }

                ಆದರೆ (bcp_Type_Alias_Res_By_ಕೀಲಿ.isValid()) {
                    Local_UResource_Bundle_ಸೂಚಕ bcp_Type_Alias_ದತ್ತಾಂಶ_ನಮೂದು;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias());
                    ಆಗ (ures_has_ಮುಂದಿನ(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t ಗಮ್ಯ_Len;
                        bcp_Type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.adoptInstead(ures_get_ಮುಂದಿನ_Resource(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias(), bcp_Type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.orphan(), &sts));
                        ಸ್ಥಿರಾಂಕ char16_t* ಗಮ್ಯ = ures_getString(bcp_Type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.getAlias(), &ಗಮ್ಯ_Len, &sts);
                        ಆದರೆ (U_FAILURE(sts)) {
                            ನಿಲ್ಲಿಸು;
                        }
                        // check if this is an alias of bcp type
                        ಆದರೆ (uprv_compareInvWithUChar(
                                ಶೂನ್ಯಸೂಚಕ,
                                t->bcpId.ದತ್ತಾಂಶ(),
                                static_cast<int32_t>(t->bcpId.size()),
                                ಗಮ್ಯ,
                                ಗಮ್ಯ_Len) == 0) {
                            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* from = ures_get_ಕೀಲಿ(bcp_Type_Alias_ದತ್ತಾಂಶ_ನಮೂದು.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_ದತ್ತಾಂಶ_Map, &alias->from, t, &sts);
                        }
                    }
                    ಆದರೆ (U_FAILURE(sts)) {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
        }
        ಆದರೆ (U_FAILURE(sts)) {
            ನಿಲ್ಲಿಸು;
        }

        Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ* ಕೀಲಿ_ದತ್ತಾಂಶ = g_Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ_Entries->create();
        ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
            sts = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
        ಕೀಲಿ_ದತ್ತಾಂಶ->bcpId = bcp_ಕೀಲಿ_Id;
        ಕೀಲಿ_ದತ್ತಾಂಶ->legacyId = legacy_ಕೀಲಿ_Id;
        ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes = specialTypes;
        ಕೀಲಿ_ದತ್ತಾಂಶ->typeMap.adoptInstead(type_ದತ್ತಾಂಶ_Map);

        uhash_put(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ_ದತ್ತಾಂಶ->legacyId, ಕೀಲಿ_ದತ್ತಾಂಶ, &sts);
        ಆದರೆ (legacy_ಕೀಲಿ_Id != bcp_ಕೀಲಿ_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ_ದತ್ತಾಂಶ->bcpId, ಕೀಲಿ_ದತ್ತಾಂಶ, &sts);
        }
        ಆದರೆ (U_FAILURE(sts)) {
            ನಿಲ್ಲಿಸು;
        }
    }
}

ತಾರ್ಕಿಕ
init() {
    UError_ಸಂಕೇತ sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ಕೀಲಿ_Map_Init_Once, &initFromResourceBundle, sts);
    ಆದರೆ (U_FAILURE(sts)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    ಗಾಗಿ (ಅಕ್ಷರ c : val) {
        ಆದರೆ (c == '-') {
            ಆದರೆ (subtagLen < 4 || subtagLen > 6) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            subtagLen = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ subtagLen >= 4 && subtagLen <= 6;
}

ತಾರ್ಕಿಕ
is_Special_Type_Reorder_ಸಂಕೇತ(std::string_view val) {
    int32_t subtagLen = 0;
    ಗಾಗಿ (ಅಕ್ಷರ c : val) {
        ಆದರೆ (c == '-') {
            ಆದರೆ (subtagLen < 3 || subtagLen > 8) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            subtagLen = 0;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ subtagLen >= 3 && subtagLen <= 8;
}

ತಾರ್ಕಿಕ
is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(std::string_view val) {
    int32_t subtagLen = 0;
    ಗಾಗಿ (ಅಕ್ಷರ c : val) {
        ಆದರೆ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
    }
    ಹಿಂತಿರುಗಿ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_ಕೀಲಿ(std::string_view ಕೀಲಿ) {
    ಆದರೆ (!init()) {
        ಹಿಂತಿರುಗಿ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ* ಕೀಲಿ_ದತ್ತಾಂಶ = static_cast<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಕೀಲಿ_ದತ್ತಾಂಶ->bcpId;
    }

    ಹಿಂತಿರುಗಿ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_ಕೀಲಿ(std::string_view ಕೀಲಿ) {
    ಆದರೆ (!init()) {
        ಹಿಂತಿರುಗಿ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ* ಕೀಲಿ_ದತ್ತಾಂಶ = static_cast<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಕೀಲಿ_ದತ್ತಾಂಶ->legacyId;
    }

    ಹಿಂತಿರುಗಿ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Bcp_Type(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND) {
    ಆದರೆ (!init()) {
        ಹಿಂತಿರುಗಿ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ* ಕೀಲಿ_ದತ್ತಾಂಶ = static_cast<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ಕೀಲಿ_ದತ್ತಾಂಶ->typeMap.getAlias(), &type));
        ಆದರೆ (t != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ t->bcpId;
        }
        ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes != SPECIALTYPE_NONE) {
            ತಾರ್ಕಿಕ matched = ಸುಳ್ಳು;
            ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ಆದರೆ (!matched && ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_REORDER_ಸಂಕೇತ) {
                matched = is_Special_Type_Reorder_ಸಂಕೇತ(type);
            }
            ಆದರೆ (!matched && ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ) {
                matched = is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(type);
            }
            ಆದರೆ (matched) {
                ಹಿಂತಿರುಗಿ type;
            }
        }
    }

    ಹಿಂತಿರುಗಿ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_ಗಮ್ಯ_Legacy_Type(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND) {
    ಆದರೆ (!init()) {
        ಹಿಂತಿರುಗಿ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ* ಕೀಲಿ_ದತ್ತಾಂಶ = static_cast<Loc_Ext_ಕೀಲಿ_ದತ್ತಾಂಶ*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ಕೀಲಿ_ದತ್ತಾಂಶ->typeMap.getAlias(), &type));
        ಆದರೆ (t != ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ t->legacyId;
        }
        ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes != SPECIALTYPE_NONE) {
            ತಾರ್ಕಿಕ matched = ಸುಳ್ಳು;
            ಆದರೆ (ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ಆದರೆ (!matched && ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_REORDER_ಸಂಕೇತ) {
                matched = is_Special_Type_Reorder_ಸಂಕೇತ(type);
            }
            ಆದರೆ (!matched && ಕೀಲಿ_ದತ್ತಾಂಶ->specialTypes & SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ) {
                matched = is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(type);
            }
            ಆದರೆ (matched) {
                ಹಿಂತಿರುಗಿ type;
            }
        }
    }

    ಹಿಂತಿರುಗಿ std::nullopt;
}
