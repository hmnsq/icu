// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#अन्तर्भाव <algorithm>
#अन्तर्भाव <optional>
#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/uobject.h"

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "fixedstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "ulocimp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvector.h"
#अन्तर्भाव "udataswp.h" /* for InvChar functions */

स्थिर UHashtable* g_Loc_Ext_कुञ्चिका_Map = शून्यसूचकः;
स्थिर icu::UInitOnce g_Loc_Ext_कुञ्चिका_Map_Init_Once {};

// bit flags for special types
प्रकारनिर्देश गणना {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_संहिता = 2,
    SPECIALTYPE_RG_कुञ्चिका_मानम् = 4
} SpecialType;

संरचना Loc_Ext_कुञ्चिका_दत्त : सार्वजनिक icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

संरचना LocExtType : सार्वजनिक icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

संरचना TypeAlias : सार्वजनिक icu::UMemory {
    std::string_view from;
};

स्थिर icu::स्मृतिः_Pool<icu::FixedString>* g_कुञ्चिका_Type_String_Pool = शून्यसूचकः;
स्थिर icu::स्मृतिः_Pool<Loc_Ext_कुञ्चिका_दत्त>* g_Loc_Ext_कुञ्चिका_दत्त_Entries = शून्यसूचकः;
स्थिर icu::स्मृतिः_Pool<LocExtType>* gLocExtTypeEntries = शून्यसूचकः;
स्थिर icu::स्मृतिः_Pool<TypeAlias>* gTypeAliasEntries = शून्यसूचकः;

U_CDECL_BEGIN

स्थिर UBool U_CALLCONV
uloc_कुञ्चिका_type_cleanup() {
    यदि (g_Loc_Ext_कुञ्चिका_Map != शून्यसूचकः) {
        uhash_close(g_Loc_Ext_कुञ्चिका_Map);
        g_Loc_Ext_कुञ्चिका_Map = शून्यसूचकः;
    }

    विलोपय g_Loc_Ext_कुञ्चिका_दत्त_Entries;
    g_Loc_Ext_कुञ्चिका_दत्त_Entries = शून्यसूचकः;

    विलोपय gLocExtTypeEntries;
    gLocExtTypeEntries = शून्यसूचकः;

    विलोपय gTypeAliasEntries;
    gTypeAliasEntries = शून्यसूचकः;

    विलोपय g_कुञ्चिका_Type_String_Pool;
    g_कुञ्चिका_Type_String_Pool = शून्यसूचकः;

    g_Loc_Ext_कुञ्चिका_Map_Init_Once.reset();
    प्रत्यागम सत्य;
}

U_CDECL_अन्तः

नामाकाश {

शून्य U_CALLCONV
initFromResourceBundle(UError_संहिता& sts) {
    U_NAMESPACE_USE
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LOCALE_कुञ्चिका_TYPE, uloc_कुञ्चिका_type_cleanup);

    g_Loc_Ext_कुञ्चिका_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, शून्यसूचकः, &sts);

    LocalUResourceBundlePointer कुञ्चिका_Type_दत्त_Res(ures_open_प्रत्यक्षम्(शून्यसूचकः, "keyTypeData", &sts));
    LocalUResourceBundlePointer कुञ्चिका_Map_Res(ures_get_By_कुञ्चिका(कुञ्चिका_Type_दत्त_Res.getAlias(), "keyMap", शून्यसूचकः, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_दत्त_Res.getAlias(), "typeMap", शून्यसूचकः, &sts));

    यदि (U_FAILURE(sts)) {
        प्रत्यागम;
    }

    UError_संहिता tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_दत्त_Res.getAlias(), "typeAlias", शून्यसूचकः, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_कुञ्चिका(कुञ्चिका_Type_दत्त_Res.getAlias(), "bcpTypeAlias", शून्यसूचकः, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_कुञ्चिका_Type_String_Pool = नूतन icu::स्मृतिः_Pool<icu::FixedString>;
    यदि (g_कुञ्चिका_Type_String_Pool == शून्यसूचकः) {
        sts = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    g_Loc_Ext_कुञ्चिका_दत्त_Entries = नूतन icu::स्मृतिः_Pool<Loc_Ext_कुञ्चिका_दत्त>;
    यदि (g_Loc_Ext_कुञ्चिका_दत्त_Entries == शून्यसूचकः) {
        sts = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    gLocExtTypeEntries = नूतन icu::स्मृतिः_Pool<LocExtType>;
    यदि (gLocExtTypeEntries == शून्यसूचकः) {
        sts = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
    gTypeAliasEntries = नूतन icu::स्मृतिः_Pool<TypeAlias>;
    यदि (gTypeAliasEntries == शून्यसूचकः) {
        sts = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer कुञ्चिका_Map_प्रविष्टिः;

    यावत् (ures_has_अग्र(कुञ्चिका_Map_Res.getAlias())) {
        कुञ्चिका_Map_प्रविष्टिः.adoptInstead(ures_get_अग्र_Resource(कुञ्चिका_Map_Res.getAlias(), कुञ्चिका_Map_प्रविष्टिः.orphan(), &sts));
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
        ध्रुव वर्ण* legacy_कुञ्चिका_Id = ures_get_कुञ्चिका(कुञ्चिका_Map_प्रविष्टिः.getAlias());
        UnicodeString u_Bcp_कुञ्चिका_Id = ures_getUnicodeString(कुञ्चिका_Map_प्रविष्टिः.getAlias(), &sts);
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ध्रुव वर्ण* bcp_कुञ्चिका_Id = legacy_कुञ्चिका_Id;
        यदि (!u_Bcp_कुञ्चिका_Id.is_रिक्तानि()) {
            icu::FixedString* bcp_कुञ्चिका_Id_Buf = g_कुञ्चिका_Type_String_Pool->create();
            यदि (bcp_कुञ्चिका_Id_Buf == शून्यसूचकः) {
                sts = U_स्मृतिः_आवण्टनम्_ERROR;
                भङ्ग;
            }
            copyInvariantChars(u_Bcp_कुञ्चिका_Id, *bcp_कुञ्चिका_Id_Buf, sts);
            यदि (U_FAILURE(sts)) {
                भङ्ग;
            }
            bcp_कुञ्चिका_Id = bcp_कुञ्चिका_Id_Buf->दत्त();
        }

        तार्किक isTZ = uprv_strcmp(legacy_कुञ्चिका_Id, "timezone") == 0;

        UHashtable* type_दत्त_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, शून्यसूचकः, &sts);
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_कुञ्चिका;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_कुञ्चिका;

        यदि (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_कुञ्चिका.adoptInstead(ures_get_By_कुञ्चिका(typeAliasRes.getAlias(), legacy_कुञ्चिका_Id, शून्यसूचकः, &tmpSts));
            यदि (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_कुञ्चिका.orphan();
            }
        }
        यदि (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_कुञ्चिका.adoptInstead(ures_get_By_कुञ्चिका(bcpTypeAliasRes.getAlias(), bcp_कुञ्चिका_Id, शून्यसूचकः, &tmpSts));
            यदि (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_कुञ्चिका.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_कुञ्चिका(ures_get_By_कुञ्चिका(typeMapRes.getAlias(), legacy_कुञ्चिका_Id, शून्यसूचकः, &sts));
        यदि (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } अन्यथा {
            LocalUResourceBundlePointer type_Map_प्रविष्टिः;

            यावत् (ures_has_अग्र(type_Map_Res_By_कुञ्चिका.getAlias())) {
                type_Map_प्रविष्टिः.adoptInstead(ures_get_अग्र_Resource(type_Map_Res_By_कुञ्चिका.getAlias(), type_Map_प्रविष्टिः.orphan(), &sts));
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }
                ध्रुव वर्ण* legacyTypeId = ures_get_कुञ्चिका(type_Map_प्रविष्टिः.getAlias());

                // special types
                यदि (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    अनुवर्त;
                }
                यदि (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_संहिता;
                    अनुवर्त;
                }
                यदि (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_कुञ्चिका_मानम्;
                    अनुवर्त;
                }

                यदि (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    यदि (uprv_strchr(legacyTypeId, ':') != शून्यसूचकः) {
                        U_ASSERT(legacyTypeId != शून्यसूचकः && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_कुञ्चिका_Type_String_Pool->create(legacyTypeIdView);
                        यदि (legacyTypeIdBuf == शून्यसूचकः || legacyTypeIdBuf->is_रिक्तानि()) {
                            sts = U_स्मृतिः_आवण्टनम्_ERROR;
                            भङ्ग;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->दत्त();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(type_Map_प्रविष्टिः.getAlias(), &sts);
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ध्रुव वर्ण* bcpTypeId = legacyTypeId;
                यदि (!uBcpTypeId.is_रिक्तानि()) {
                    icu::FixedString* bcpTypeIdBuf = g_कुञ्चिका_Type_String_Pool->create();
                    यदि (bcpTypeIdBuf == शून्यसूचकः) {
                        sts = U_स्मृतिः_आवण्टनम्_ERROR;
                        भङ्ग;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                    bcpTypeId = bcpTypeIdBuf->दत्त();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                यदि (t == शून्यसूचकः) {
                    sts = U_स्मृतिः_आवण्टनम्_ERROR;
                    भङ्ग;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(type_दत्त_Map, &t->legacyId, t, &sts);
                यदि (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(type_दत्त_Map, &t->bcpId, t, &sts);
                }
                यदि (U_FAILURE(sts)) {
                    भङ्ग;
                }

                // also put aliases in the map
                यदि (type_Alias_Res_By_कुञ्चिका.isValid()) {
                    LocalUResourceBundlePointer type_Alias_दत्त_प्रविष्टिः;

                    ures_resetIterator(type_Alias_Res_By_कुञ्चिका.getAlias());
                    यावत् (ures_has_अग्र(type_Alias_Res_By_कुञ्चिका.getAlias()) && U_SUCCESS(sts)) {
                        int32_t गन्तव्यम्_Len;
                        type_Alias_दत्त_प्रविष्टिः.adoptInstead(ures_get_अग्र_Resource(type_Alias_Res_By_कुञ्चिका.getAlias(), type_Alias_दत्त_प्रविष्टिः.orphan(), &sts));
                        ध्रुव char16_t* गन्तव्यम् = ures_getString(type_Alias_दत्त_प्रविष्टिः.getAlias(), &गन्तव्यम्_Len, &sts);
                        यदि (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of canonical legacy type
                        यदि (uprv_compareInvWithUChar(
                                शून्यसूचकः,
                                t->legacyId.दत्त(),
                                static_cast<int32_t>(t->legacyId.size()),
                                गन्तव्यम्,
                                गन्तव्यम्_Len) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुञ्चिका(type_Alias_दत्त_प्रविष्टिः.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            यदि (isTZ) {
                                // replace colon with slash if necessary
                                यदि (uprv_strchr(from, ':') != शून्यसूचकः) {
                                    U_ASSERT(from != शून्यसूचकः && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_कुञ्चिका_Type_String_Pool->create(fromView);
                                    यदि (fromBuf == शून्यसूचकः || fromBuf->is_रिक्तानि()) {
                                        sts = U_स्मृतिः_आवण्टनम्_ERROR;
                                        भङ्ग;
                                    }
                                    std::replace(
                                            fromBuf->getAlias(),
                                            fromBuf->getAlias() + fromView.length(),
                                            ':', '/');
                                    alias->from = {fromBuf->दत्त(), fromView.length()};
                                }
                            }
                            uhash_put(type_दत्त_Map, &alias->from, t, &sts);
                        }
                    }
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }

                यदि (bcp_Type_Alias_Res_By_कुञ्चिका.isValid()) {
                    LocalUResourceBundlePointer bcp_Type_Alias_दत्त_प्रविष्टिः;

                    ures_resetIterator(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias());
                    यावत् (ures_has_अग्र(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias()) && U_SUCCESS(sts)) {
                        int32_t गन्तव्यम्_Len;
                        bcp_Type_Alias_दत्त_प्रविष्टिः.adoptInstead(ures_get_अग्र_Resource(bcp_Type_Alias_Res_By_कुञ्चिका.getAlias(), bcp_Type_Alias_दत्त_प्रविष्टिः.orphan(), &sts));
                        ध्रुव char16_t* गन्तव्यम् = ures_getString(bcp_Type_Alias_दत्त_प्रविष्टिः.getAlias(), &गन्तव्यम्_Len, &sts);
                        यदि (U_FAILURE(sts)) {
                            भङ्ग;
                        }
                        // check if this is an alias of bcp type
                        यदि (uprv_compareInvWithUChar(
                                शून्यसूचकः,
                                t->bcpId.दत्त(),
                                static_cast<int32_t>(t->bcpId.size()),
                                गन्तव्यम्,
                                गन्तव्यम्_Len) == 0) {
                            ध्रुव वर्ण* from = ures_get_कुञ्चिका(bcp_Type_Alias_दत्त_प्रविष्टिः.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(type_दत्त_Map, &alias->from, t, &sts);
                        }
                    }
                    यदि (U_FAILURE(sts)) {
                        भङ्ग;
                    }
                }
            }
        }
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }

        Loc_Ext_कुञ्चिका_दत्त* कुञ्चिका_दत्त = g_Loc_Ext_कुञ्चिका_दत्त_Entries->create();
        यदि (कुञ्चिका_दत्त == शून्यसूचकः) {
            sts = U_स्मृतिः_आवण्टनम्_ERROR;
            भङ्ग;
        }
        कुञ्चिका_दत्त->bcpId = bcp_कुञ्चिका_Id;
        कुञ्चिका_दत्त->legacyId = legacy_कुञ्चिका_Id;
        कुञ्चिका_दत्त->specialTypes = specialTypes;
        कुञ्चिका_दत्त->typeMap.adoptInstead(type_दत्त_Map);

        uhash_put(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका_दत्त->legacyId, कुञ्चिका_दत्त, &sts);
        यदि (legacy_कुञ्चिका_Id != bcp_कुञ्चिका_Id) {
            // different key value
            uhash_put(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका_दत्त->bcpId, कुञ्चिका_दत्त, &sts);
        }
        यदि (U_FAILURE(sts)) {
            भङ्ग;
        }
    }
}

तार्किक
init() {
    UError_संहिता sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_कुञ्चिका_Map_Init_Once, &initFromResourceBundle, sts);
    यदि (U_FAILURE(sts)) {
        प्रत्यागम असत्य;
    }
    प्रत्यागम सत्य;
}

तार्किक
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि (c == '-') {
            यदि (subtagLen < 4 || subtagLen > 6) {
                प्रत्यागम असत्य;
            }
            subtagLen = 0;
        } अन्यथा यदि ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen >= 4 && subtagLen <= 6;
}

तार्किक
is_Special_Type_Reorder_संहिता(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि (c == '-') {
            यदि (subtagLen < 3 || subtagLen > 8) {
                प्रत्यागम असत्य;
            }
            subtagLen = 0;
        } अन्यथा यदि (uprv_isASCIILetter(c)) {
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen >= 3 && subtagLen <= 8;
}

तार्किक
is_Special_Type_Rg_कुञ्चिका_मानम्(std::string_view val) {
    int32_t subtagLen = 0;
    प्रति (वर्ण c : val) {
        यदि ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } अन्यथा {
            प्रत्यागम असत्य;
        }
    }
    प्रत्यागम subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_कुञ्चिका(std::string_view कुञ्चिका) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_दत्त* कुञ्चिका_दत्त = static_cast<Loc_Ext_कुञ्चिका_दत्त*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    यदि (कुञ्चिका_दत्त != शून्यसूचकः) {
        प्रत्यागम कुञ्चिका_दत्त->bcpId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_कुञ्चिका(std::string_view कुञ्चिका) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_दत्त* कुञ्चिका_दत्त = static_cast<Loc_Ext_कुञ्चिका_दत्त*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    यदि (कुञ्चिका_दत्त != शून्यसूचकः) {
        प्रत्यागम कुञ्चिका_दत्त->legacyId;
    }

    प्रत्यागम std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Bcp_Type(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_दत्त* कुञ्चिका_दत्त = static_cast<Loc_Ext_कुञ्चिका_दत्त*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    यदि (कुञ्चिका_दत्त != शून्यसूचकः) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुञ्चिका_दत्त->typeMap.getAlias(), &type));
        यदि (t != शून्यसूचकः) {
            प्रत्यागम t->bcpId;
        }
        यदि (कुञ्चिका_दत्त->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            यदि (कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            यदि (!matched && कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_REORDER_संहिता) {
                matched = is_Special_Type_Reorder_संहिता(type);
            }
            यदि (!matched && कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_RG_कुञ्चिका_मानम्) {
                matched = is_Special_Type_Rg_कुञ्चिका_मानम्(type);
            }
            यदि (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_गन्तव्यम्_Legacy_Type(std::string_view कुञ्चिका, std::string_view type U_LIFETIME_BOUND) {
    यदि (!init()) {
        प्रत्यागम std::nullopt;
    }

    Loc_Ext_कुञ्चिका_दत्त* कुञ्चिका_दत्त = static_cast<Loc_Ext_कुञ्चिका_दत्त*>(uhash_get(g_Loc_Ext_कुञ्चिका_Map, &कुञ्चिका));
    यदि (कुञ्चिका_दत्त != शून्यसूचकः) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(कुञ्चिका_दत्त->typeMap.getAlias(), &type));
        यदि (t != शून्यसूचकः) {
            प्रत्यागम t->legacyId;
        }
        यदि (कुञ्चिका_दत्त->specialTypes != SPECIALTYPE_NONE) {
            तार्किक matched = असत्य;
            यदि (कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            यदि (!matched && कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_REORDER_संहिता) {
                matched = is_Special_Type_Reorder_संहिता(type);
            }
            यदि (!matched && कुञ्चिका_दत्त->specialTypes & SPECIALTYPE_RG_कुञ्चिका_मानम्) {
                matched = is_Special_Type_Rg_कुञ्चिका_मानम्(type);
            }
            यदि (matched) {
                प्रत्यागम type;
            }
        }
    }

    प्रत्यागम std::nullopt;
}
