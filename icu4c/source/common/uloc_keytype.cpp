// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#అన్తర్భావ <algorithm>
#అన్తర్భావ <optional>
#అన్తర్భావ <string_view>

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/unistr.h"
#అన్తర్భావ "unicode/uobject.h"

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "fixedstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "ulocimp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "uvector.h"
#అన్తర్భావ "udataswp.h" /* for InvChar functions */

స్థిర UHashtable* g_Loc_Ext_కీలకం_Map = శూన్యసూచిక;
స్థిర icu::UInitOnce g_Loc_Ext_కీలకం_Map_Init_Once {};

// bit flags for special types
రకనిర్వచనం గణన {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_సంకేతం = 2,
    SPECIALTYPE_RG_కీలకం_విలువ = 4
} SpecialType;

నిర్మాణం Loc_Ext_కీలకం_దత్తాంశం : బహిరంగ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::Local_UHashtable_సూచిక typeMap;
    uint32_t        specialTypes;
};

నిర్మాణం LocExtType : బహిరంగ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

నిర్మాణం TypeAlias : బహిరంగ icu::UMemory {
    std::string_view from;
};

స్థిర icu::జ్ఞాపకం_Pool<icu::FixedString>* g_కీలకం_Type_String_Pool = శూన్యసూచిక;
స్థిర icu::జ్ఞాపకం_Pool<Loc_Ext_కీలకం_దత్తాంశం>* g_Loc_Ext_కీలకం_దత్తాంశం_Entries = శూన్యసూచిక;
స్థిర icu::జ్ఞాపకం_Pool<LocExtType>* gLocExtTypeEntries = శూన్యసూచిక;
స్థిర icu::జ్ఞాపకం_Pool<TypeAlias>* gTypeAliasEntries = శూన్యసూచిక;

U_CDECL_BEGIN

స్థిర UBool U_CALLCONV
uloc_కీలకం_type_cleanup() {
    ఒకవేళ (g_Loc_Ext_కీలకం_Map != శూన్యసూచిక) {
        uhash_close(g_Loc_Ext_కీలకం_Map);
        g_Loc_Ext_కీలకం_Map = శూన్యసూచిక;
    }

    తొలగించు g_Loc_Ext_కీలకం_దత్తాంశం_Entries;
    g_Loc_Ext_కీలకం_దత్తాంశం_Entries = శూన్యసూచిక;

    తొలగించు gLocExtTypeEntries;
    gLocExtTypeEntries = శూన్యసూచిక;

    తొలగించు gTypeAliasEntries;
    gTypeAliasEntries = శూన్యసూచిక;

    తొలగించు g_కీలకం_Type_String_Pool;
    g_కీలకం_Type_String_Pool = శూన్యసూచిక;

    g_Loc_Ext_కీలకం_Map_Init_Once.reset();
    తిరిగి నిజం;
}

U_CDECL_చివర

నామాకాశ {

శూన్య U_CALLCONV
initFromResourceBundle(UError_సంకేతం& sts) {
    U_NAMESPACE_USE
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LOCALE_కీలకం_TYPE, uloc_కీలకం_type_cleanup);

    g_Loc_Ext_కీలకం_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, శూన్యసూచిక, &sts);

    Local_UResource_Bundle_సూచిక కీలకం_Type_దత్తాంశం_Res(ures_open_ప్రత్యక్ష(శూన్యసూచిక, "keyTypeData", &sts));
    Local_UResource_Bundle_సూచిక కీలకం_Map_Res(ures_get_By_కీలకం(కీలకం_Type_దత్తాంశం_Res.getAlias(), "keyMap", శూన్యసూచిక, &sts));
    Local_UResource_Bundle_సూచిక typeMapRes(ures_get_By_కీలకం(కీలకం_Type_దత్తాంశం_Res.getAlias(), "typeMap", శూన్యసూచిక, &sts));

    ఒకవేళ (U_FAILURE(sts)) {
        తిరిగి;
    }

    UError_సంకేతం tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_సూచిక typeAliasRes(ures_get_By_కీలకం(కీలకం_Type_దత్తాంశం_Res.getAlias(), "typeAlias", శూన్యసూచిక, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    Local_UResource_Bundle_సూచిక bcpTypeAliasRes(ures_get_By_కీలకం(కీలకం_Type_దత్తాంశం_Res.getAlias(), "bcpTypeAlias", శూన్యసూచిక, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_కీలకం_Type_String_Pool = కొత్త icu::జ్ఞాపకం_Pool<icu::FixedString>;
    ఒకవేళ (g_కీలకం_Type_String_Pool == శూన్యసూచిక) {
        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    g_Loc_Ext_కీలకం_దత్తాంశం_Entries = కొత్త icu::జ్ఞాపకం_Pool<Loc_Ext_కీలకం_దత్తాంశం>;
    ఒకవేళ (g_Loc_Ext_కీలకం_దత్తాంశం_Entries == శూన్యసూచిక) {
        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    gLocExtTypeEntries = కొత్త icu::జ్ఞాపకం_Pool<LocExtType>;
    ఒకవేళ (gLocExtTypeEntries == శూన్యసూచిక) {
        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
    gTypeAliasEntries = కొత్త icu::జ్ఞాపకం_Pool<TypeAlias>;
    ఒకవేళ (gTypeAliasEntries == శూన్యసూచిక) {
        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    // iterate through keyMap resource
    Local_UResource_Bundle_సూచిక కీలకం_Map_నమోదు;

    అయితే (ures_has_తదుపరి(కీలకం_Map_Res.getAlias())) {
        కీలకం_Map_నమోదు.adoptInstead(ures_get_తదుపరి_Resource(కీలకం_Map_Res.getAlias(), కీలకం_Map_నమోదు.orphan(), &sts));
        ఒకవేళ (U_FAILURE(sts)) {
            ఆపు;
        }
        స్థిరరాశి అక్షరం* legacy_కీలకం_Id = ures_get_కీలకం(కీలకం_Map_నమోదు.getAlias());
        UnicodeString u_Bcp_కీలకం_Id = ures_getUnicodeString(కీలకం_Map_నమోదు.getAlias(), &sts);
        ఒకవేళ (U_FAILURE(sts)) {
            ఆపు;
        }

        // empty value indicates that BCP key is same with the legacy key.
        స్థిరరాశి అక్షరం* bcp_కీలకం_Id = legacy_కీలకం_Id;
        ఒకవేళ (!u_Bcp_కీలకం_Id.is_ఖాళీలు()) {
            icu::FixedString* bcp_కీలకం_Id_Buf = g_కీలకం_Type_String_Pool->create();
            ఒకవేళ (bcp_కీలకం_Id_Buf == శూన్యసూచిక) {
                sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                ఆపు;
            }
            copyInvariantChars(u_Bcp_కీలకం_Id, *bcp_కీలకం_Id_Buf, sts);
            ఒకవేళ (U_FAILURE(sts)) {
                ఆపు;
            }
            bcp_కీలకం_Id = bcp_కీలకం_Id_Buf->దత్తాంశం();
        }

        తార్కిక isTZ = uprv_strcmp(legacy_కీలకం_Id, "timezone") == 0;

        UHashtable* type_దత్తాంశం_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, శూన్యసూచిక, &sts);
        ఒకవేళ (U_FAILURE(sts)) {
            ఆపు;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        Local_UResource_Bundle_సూచిక type_Alias_Res_By_కీలకం;
        Local_UResource_Bundle_సూచిక bcp_Type_Alias_Res_By_కీలకం;

        ఒకవేళ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_కీలకం.adoptInstead(ures_get_By_కీలకం(typeAliasRes.getAlias(), legacy_కీలకం_Id, శూన్యసూచిక, &tmpSts));
            ఒకవేళ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_కీలకం.orphan();
            }
        }
        ఒకవేళ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_కీలకం.adoptInstead(ures_get_By_కీలకం(bcpTypeAliasRes.getAlias(), bcp_కీలకం_Id, శూన్యసూచిక, &tmpSts));
            ఒకవేళ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_కీలకం.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        Local_UResource_Bundle_సూచిక type_Map_Res_By_కీలకం(ures_get_By_కీలకం(typeMapRes.getAlias(), legacy_కీలకం_Id, శూన్యసూచిక, &sts));
        ఒకవేళ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } లేకపోతే {
            Local_UResource_Bundle_సూచిక type_Map_నమోదు;

            అయితే (ures_has_తదుపరి(type_Map_Res_By_కీలకం.getAlias())) {
                type_Map_నమోదు.adoptInstead(ures_get_తదుపరి_Resource(type_Map_Res_By_కీలకం.getAlias(), type_Map_నమోదు.orphan(), &sts));
                ఒకవేళ (U_FAILURE(sts)) {
                    ఆపు;
                }
                స్థిరరాశి అక్షరం* legacyTypeId = ures_get_కీలకం(type_Map_నమోదు.getAlias());

                // special types
                ఒకవేళ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    కొనసాగు;
                }
                ఒకవేళ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_సంకేతం;
                    కొనసాగు;
                }
                ఒకవేళ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_కీలకం_విలువ;
                    కొనసాగు;
                }

                ఒకవేళ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ఒకవేళ (uprv_strchr(legacyTypeId, ':') != శూన్యసూచిక) {
                        U_ASSERT(legacyTypeId != శూన్యసూచిక && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_కీలకం_Type_String_Pool->create(legacyTypeIdView);
                        ఒకవేళ (legacyTypeIdBuf == శూన్యసూచిక || legacyTypeIdBuf->is_ఖాళీలు()) {
                            sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                            ఆపు;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->దత్తాంశం();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_నమోదు.getAlias(), &sts);
                ఒకవేళ (U_FAILURE(sts)) {
                    ఆపు;
                }

                // empty value indicates that BCP type is same with the legacy type.
                స్థిరరాశి అక్షరం* bcpTypeId = legacyTypeId;
                ఒకవేళ (!uBcpTypeId.is_ఖాళీలు()) {
                    icu::FixedString* bcpTypeIdBuf = g_కీలకం_Type_String_Pool->create();
                    ఒకవేళ (bcpTypeIdBuf == శూన్యసూచిక) {
                        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                        ఆపు;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ఒకవేళ (U_FAILURE(sts)) {
                        ఆపు;
                    }
                    bcpTypeId = bcpTypeIdBuf->దత్తాంశం();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ఒకవేళ (t == శూన్యసూచిక) {
                    sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                    ఆపు;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_దత్తాంశం_Map, &t->legacyId, t, &sts);
                ఒకవేళ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_దత్తాంశం_Map, &t->bcpId, t, &sts);
                }
                ఒకవేళ (U_FAILURE(sts)) {
                    ఆపు;
                }

                // also put aliases in the map
                ఒకవేళ (type_Alias_Res_By_కీలకం.isValid()) {
                    Local_UResource_Bundle_సూచిక type_Alias_దత్తాంశం_నమోదు;

                    ures_resetIterator(type_Alias_Res_By_కీలకం.getAlias());
                    అయితే (ures_has_తదుపరి(type_Alias_Res_By_కీలకం.getAlias()) && U_SUCCESS(sts)) {
                        int32_t గమ్యం_Len;
                        type_Alias_దత్తాంశం_నమోదు.adoptInstead(ures_get_తదుపరి_Resource(type_Alias_Res_By_కీలకం.getAlias(), type_Alias_దత్తాంశం_నమోదు.orphan(), &sts));
                        స్థిరరాశి char16_t* గమ్యం = ures_getString(type_Alias_దత్తాంశం_నమోదు.getAlias(), &గమ్యం_Len, &sts);
                        ఒకవేళ (U_FAILURE(sts)) {
                            ఆపు;
                        }
                        // check if this is an alias of canonical legacy type
                        ఒకవేళ (uprv_compareInvWithUChar(
                                శూన్యసూచిక,
                                t->legacyId.దత్తాంశం(),
                                static_cast<int32_t>(t->legacyId.size()),
                                గమ్యం,
                                గమ్యం_Len) == 0) {
                            స్థిరరాశి అక్షరం* from = ures_get_కీలకం(type_Alias_దత్తాంశం_నమోదు.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ఒకవేళ (isTZ) {
                                // replace colon with slash if necessary
                                ఒకవేళ (uprv_strchr(from, ':') != శూన్యసూచిక) {
                                    U_ASSERT(from != శూన్యసూచిక && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_కీలకం_Type_String_Pool->create(fromView);
                                    ఒకవేళ (fromBuf == శూన్యసూచిక || fromBuf->is_ఖాళీలు()) {
                                        sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                                        ఆపు;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->దత్తాంశం(), fromView.length()};
                                }
                            }
                            uhash_put(type_దత్తాంశం_Map, &alias->from, t, &sts);
                        }
                    }
                    ఒకవేళ (U_FAILURE(sts)) {
                        ఆపు;
                    }
                }

                ఒకవేళ (bcp_Type_Alias_Res_By_కీలకం.isValid()) {
                    Local_UResource_Bundle_సూచిక bcp_Type_Alias_దత్తాంశం_నమోదు;

                    ures_resetIterator(bcp_Type_Alias_Res_By_కీలకం.getAlias());
                    అయితే (ures_has_తదుపరి(bcp_Type_Alias_Res_By_కీలకం.getAlias()) && U_SUCCESS(sts)) {
                        int32_t గమ్యం_Len;
                        bcp_Type_Alias_దత్తాంశం_నమోదు.adoptInstead(ures_get_తదుపరి_Resource(bcp_Type_Alias_Res_By_కీలకం.getAlias(), bcp_Type_Alias_దత్తాంశం_నమోదు.orphan(), &sts));
                        స్థిరరాశి char16_t* గమ్యం = ures_getString(bcp_Type_Alias_దత్తాంశం_నమోదు.getAlias(), &గమ్యం_Len, &sts);
                        ఒకవేళ (U_FAILURE(sts)) {
                            ఆపు;
                        }
                        // check if this is an alias of bcp type
                        ఒకవేళ (uprv_compareInvWithUChar(
                                శూన్యసూచిక,
                                t->bcpId.దత్తాంశం(),
                                static_cast<int32_t>(t->bcpId.size()),
                                గమ్యం,
                                గమ్యం_Len) == 0) {
                            స్థిరరాశి అక్షరం* from = ures_get_కీలకం(bcp_Type_Alias_దత్తాంశం_నమోదు.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_దత్తాంశం_Map, &alias->from, t, &sts);
                        }
                    }
                    ఒకవేళ (U_FAILURE(sts)) {
                        ఆపు;
                    }
                }
            }
        }
        ఒకవేళ (U_FAILURE(sts)) {
            ఆపు;
        }

        Loc_Ext_కీలకం_దత్తాంశం* కీలకం_దత్తాంశం = g_Loc_Ext_కీలకం_దత్తాంశం_Entries->create();
        ఒకవేళ (కీలకం_దత్తాంశం == శూన్యసూచిక) {
            sts = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            ఆపు;
        }
        కీలకం_దత్తాంశం->bcpId = bcp_కీలకం_Id;
        కీలకం_దత్తాంశం->legacyId = legacy_కీలకం_Id;
        కీలకం_దత్తాంశం->specialTypes = specialTypes;
        కీలకం_దత్తాంశం->typeMap.adoptInstead(type_దత్తాంశం_Map);

        uhash_put(g_Loc_Ext_కీలకం_Map, &కీలకం_దత్తాంశం->legacyId, కీలకం_దత్తాంశం, &sts);
        ఒకవేళ (legacy_కీలకం_Id != bcp_కీలకం_Id) {
            // different key value
            uhash_put(g_Loc_Ext_కీలకం_Map, &కీలకం_దత్తాంశం->bcpId, కీలకం_దత్తాంశం, &sts);
        }
        ఒకవేళ (U_FAILURE(sts)) {
            ఆపు;
        }
    }
}

తార్కిక
init() {
    UError_సంకేతం sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_కీలకం_Map_Init_Once, &initFromResourceBundle, sts);
    ఒకవేళ (U_FAILURE(sts)) {
        తిరిగి అబద్ధం;
    }
    తిరిగి నిజం;
}

తార్కిక
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    కొరకు (అక్షరం c : val) {
        ఒకవేళ (c == '-') {
            ఒకవేళ (subtagLen < 4 || subtagLen > 6) {
                తిరిగి అబద్ధం;
            }
            subtagLen = 0;
        } లేకపోతే ఒకవేళ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి subtagLen >= 4 && subtagLen <= 6;
}

తార్కిక
is_Special_Type_Reorder_సంకేతం(std::string_view val) {
    int32_t subtagLen = 0;
    కొరకు (అక్షరం c : val) {
        ఒకవేళ (c == '-') {
            ఒకవేళ (subtagLen < 3 || subtagLen > 8) {
                తిరిగి అబద్ధం;
            }
            subtagLen = 0;
        } లేకపోతే ఒకవేళ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి subtagLen >= 3 && subtagLen <= 8;
}

తార్కిక
is_Special_Type_Rg_కీలకం_విలువ(std::string_view val) {
    int32_t subtagLen = 0;
    కొరకు (అక్షరం c : val) {
        ఒకవేళ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } లేకపోతే {
            తిరిగి అబద్ధం;
        }
    }
    తిరిగి subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_కీలకం(std::string_view కీలకం) {
    ఒకవేళ (!init()) {
        తిరిగి std::nullopt;
    }

    Loc_Ext_కీలకం_దత్తాంశం* కీలకం_దత్తాంశం = static_cast<Loc_Ext_కీలకం_దత్తాంశం*>(uhash_get(g_Loc_Ext_కీలకం_Map, &కీలకం));
    ఒకవేళ (కీలకం_దత్తాంశం != శూన్యసూచిక) {
        తిరిగి కీలకం_దత్తాంశం->bcpId;
    }

    తిరిగి std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_కీలకం(std::string_view కీలకం) {
    ఒకవేళ (!init()) {
        తిరిగి std::nullopt;
    }

    Loc_Ext_కీలకం_దత్తాంశం* కీలకం_దత్తాంశం = static_cast<Loc_Ext_కీలకం_దత్తాంశం*>(uhash_get(g_Loc_Ext_కీలకం_Map, &కీలకం));
    ఒకవేళ (కీలకం_దత్తాంశం != శూన్యసూచిక) {
        తిరిగి కీలకం_దత్తాంశం->legacyId;
    }

    తిరిగి std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Bcp_Type(std::string_view కీలకం, std::string_view type U_LIFETIME_BOUND) {
    ఒకవేళ (!init()) {
        తిరిగి std::nullopt;
    }

    Loc_Ext_కీలకం_దత్తాంశం* కీలకం_దత్తాంశం = static_cast<Loc_Ext_కీలకం_దత్తాంశం*>(uhash_get(g_Loc_Ext_కీలకం_Map, &కీలకం));
    ఒకవేళ (కీలకం_దత్తాంశం != శూన్యసూచిక) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(కీలకం_దత్తాంశం->typeMap.getAlias(), &type));
        ఒకవేళ (t != శూన్యసూచిక) {
            తిరిగి t->bcpId;
        }
        ఒకవేళ (కీలకం_దత్తాంశం->specialTypes != SPECIALTYPE_NONE) {
            తార్కిక matched = అబద్ధం;
            ఒకవేళ (కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ఒకవేళ (!matched && కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_REORDER_సంకేతం) {
                matched = is_Special_Type_Reorder_సంకేతం(type);
            }
            ఒకవేళ (!matched && కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_RG_కీలకం_విలువ) {
                matched = is_Special_Type_Rg_కీలకం_విలువ(type);
            }
            ఒకవేళ (matched) {
                తిరిగి type;
            }
        }
    }

    తిరిగి std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_గమ్యం_Legacy_Type(std::string_view కీలకం, std::string_view type U_LIFETIME_BOUND) {
    ఒకవేళ (!init()) {
        తిరిగి std::nullopt;
    }

    Loc_Ext_కీలకం_దత్తాంశం* కీలకం_దత్తాంశం = static_cast<Loc_Ext_కీలకం_దత్తాంశం*>(uhash_get(g_Loc_Ext_కీలకం_Map, &కీలకం));
    ఒకవేళ (కీలకం_దత్తాంశం != శూన్యసూచిక) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(కీలకం_దత్తాంశం->typeMap.getAlias(), &type));
        ఒకవేళ (t != శూన్యసూచిక) {
            తిరిగి t->legacyId;
        }
        ఒకవేళ (కీలకం_దత్తాంశం->specialTypes != SPECIALTYPE_NONE) {
            తార్కిక matched = అబద్ధం;
            ఒకవేళ (కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ఒకవేళ (!matched && కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_REORDER_సంకేతం) {
                matched = is_Special_Type_Reorder_సంకేతం(type);
            }
            ఒకవేళ (!matched && కీలకం_దత్తాంశం->specialTypes & SPECIALTYPE_RG_కీలకం_విలువ) {
                matched = is_Special_Type_Rg_కీలకం_విలువ(type);
            }
            ఒకవేళ (matched) {
                తిరిగి type;
            }
        }
    }

    తిరిగి std::nullopt;
}
