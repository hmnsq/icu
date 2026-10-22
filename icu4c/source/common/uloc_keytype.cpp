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

ಸ್ಥಿರ UHashtable* g_Loc_Ext_ಕೀಲಿ_Map = nullptr;
ಸ್ಥಿರ icu::UInitOnce g_Loc_Ext_ಕೀಲಿ_Map_Init_Once {};

// bit flags for special types
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ = 4
} SpecialType;

ಸಂರಚನಾ Loc_Ext_ಕೀಲಿ_Data : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

ಸಂರಚನಾ LocExtType : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ಸಂರಚನಾ TypeAlias : ಸಾರ್ವಜನಿಕ icu::UMemory {
    std::string_view from;
};

ಸ್ಥಿರ icu::MemoryPool<icu::FixedString>* g_ಕೀಲಿ_Type_String_Pool = nullptr;
ಸ್ಥಿರ icu::MemoryPool<Loc_Ext_ಕೀಲಿ_Data>* g_Loc_Ext_ಕೀಲಿ_Data_Entries = nullptr;
ಸ್ಥಿರ icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
ಸ್ಥಿರ icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

ಸ್ಥಿರ UBool U_CALLCONV
uloc_ಕೀಲಿ_type_cleanup() {
    ಯದಿ (g_Loc_Ext_ಕೀಲಿ_Map != nullptr) {
        uhash_close(g_Loc_Ext_ಕೀಲಿ_Map);
        g_Loc_Ext_ಕೀಲಿ_Map = nullptr;
    }

    delete g_Loc_Ext_ಕೀಲಿ_Data_Entries;
    g_Loc_Ext_ಕೀಲಿ_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_ಕೀಲಿ_Type_String_Pool;
    g_ಕೀಲಿ_Type_String_Pool = nullptr;

    g_Loc_Ext_ಕೀಲಿ_Map_Init_Once.reset();
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_CDECL_END

ನಾಮಾಕಾಶ {

ಶೂನ್ಯ U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ಕೀಲಿ_TYPE, uloc_ಕೀಲಿ_type_cleanup);

    g_Loc_Ext_ಕೀಲಿ_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer ಕೀಲಿ_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer ಕೀಲಿ_Map_Res(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    ಯದಿ (U_FAILURE(sts)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_ಕೀಲಿ(ಕೀಲಿ_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ಕೀಲಿ_Type_String_Pool = ನೂತನ icu::MemoryPool<icu::FixedString>;
    ಯದಿ (g_ಕೀಲಿ_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    g_Loc_Ext_ಕೀಲಿ_Data_Entries = ನೂತನ icu::MemoryPool<Loc_Ext_ಕೀಲಿ_Data>;
    ಯದಿ (g_Loc_Ext_ಕೀಲಿ_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    gLocExtTypeEntries = ನೂತನ icu::MemoryPool<LocExtType>;
    ಯದಿ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }
    gTypeAliasEntries = ನೂತನ icu::MemoryPool<TypeAlias>;
    ಯದಿ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer ಕೀಲಿ_Map_Entry;

    ಯಾವತ್ (ures_hasNext(ಕೀಲಿ_Map_Res.getAlias())) {
        ಕೀಲಿ_Map_Entry.adoptInstead(ures_getNextResource(ಕೀಲಿ_Map_Res.getAlias(), ಕೀಲಿ_Map_Entry.orphan(), &sts));
        ಯದಿ (U_FAILURE(sts)) {
            ಭಙ್ಗ;
        }
        ಧ್ರುವ ವರ್ಣ* legacy_ಕೀಲಿ_Id = ures_get_ಕೀಲಿ(ಕೀಲಿ_Map_Entry.getAlias());
        UnicodeString u_Bcp_ಕೀಲಿ_Id = ures_getUnicodeString(ಕೀಲಿ_Map_Entry.getAlias(), &sts);
        ಯದಿ (U_FAILURE(sts)) {
            ಭಙ್ಗ;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ಧ್ರುವ ವರ್ಣ* bcp_ಕೀಲಿ_Id = legacy_ಕೀಲಿ_Id;
        ಯದಿ (!u_Bcp_ಕೀಲಿ_Id.isEmpty()) {
            icu::FixedString* bcp_ಕೀಲಿ_Id_Buf = g_ಕೀಲಿ_Type_String_Pool->create();
            ಯದಿ (bcp_ಕೀಲಿ_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                ಭಙ್ಗ;
            }
            copyInvariantChars(u_Bcp_ಕೀಲಿ_Id, *bcp_ಕೀಲಿ_Id_Buf, sts);
            ಯದಿ (U_FAILURE(sts)) {
                ಭಙ್ಗ;
            }
            bcp_ಕೀಲಿ_Id = bcp_ಕೀಲಿ_Id_Buf->data();
        }

        ತಾರ್ಕಿಕ isTZ = uprv_strcmp(legacy_ಕೀಲಿ_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        ಯದಿ (U_FAILURE(sts)) {
            ಭಙ್ಗ;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_ಕೀಲಿ;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_ಕೀಲಿ;

        ಯದಿ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ಕೀಲಿ.adoptInstead(ures_get_By_ಕೀಲಿ(typeAliasRes.getAlias(), legacy_ಕೀಲಿ_Id, nullptr, &tmpSts));
            ಯದಿ (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ಕೀಲಿ.orphan();
            }
        }
        ಯದಿ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ಕೀಲಿ.adoptInstead(ures_get_By_ಕೀಲಿ(bcpTypeAliasRes.getAlias(), bcp_ಕೀಲಿ_Id, nullptr, &tmpSts));
            ಯದಿ (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ಕೀಲಿ.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_ಕೀಲಿ(ures_get_By_ಕೀಲಿ(typeMapRes.getAlias(), legacy_ಕೀಲಿ_Id, nullptr, &sts));
        ಯದಿ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ಅನ್ಯಥಾ {
            LocalUResourceBundlePointer typeMapEntry;

            ಯಾವತ್ (ures_hasNext(type_Map_Res_By_ಕೀಲಿ.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_ಕೀಲಿ.getAlias(), typeMapEntry.orphan(), &sts));
                ಯದಿ (U_FAILURE(sts)) {
                    ಭಙ್ಗ;
                }
                ಧ್ರುವ ವರ್ಣ* legacyTypeId = ures_get_ಕೀಲಿ(typeMapEntry.getAlias());

                // special types
                ಯದಿ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ಅನುವರ್ತ;
                }
                ಯದಿ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    ಅನುವರ್ತ;
                }
                ಯದಿ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ;
                    ಅನುವರ್ತ;
                }

                ಯದಿ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ಯದಿ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ಕೀಲಿ_Type_String_Pool->create(legacyTypeIdView);
                        ಯದಿ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            ಭಙ್ಗ;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                ಯದಿ (U_FAILURE(sts)) {
                    ಭಙ್ಗ;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ಧ್ರುವ ವರ್ಣ* bcpTypeId = legacyTypeId;
                ಯದಿ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_ಕೀಲಿ_Type_String_Pool->create();
                    ಯದಿ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        ಭಙ್ಗ;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ಯದಿ (U_FAILURE(sts)) {
                        ಭಙ್ಗ;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ಯದಿ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    ಭಙ್ಗ;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                ಯದಿ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                ಯದಿ (U_FAILURE(sts)) {
                    ಭಙ್ಗ;
                }

                // also put aliases in the map
                ಯದಿ (type_Alias_Res_By_ಕೀಲಿ.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_ಕೀಲಿ.getAlias());
                    ಯಾವತ್ (ures_hasNext(type_Alias_Res_By_ಕೀಲಿ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_ಕೀಲಿ.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ಧ್ರುವ char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        ಯದಿ (U_FAILURE(sts)) {
                            ಭಙ್ಗ;
                        }
                        // check if this is an alias of canonical legacy type
                        ಯದಿ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ಧ್ರುವ ವರ್ಣ* from = ures_get_ಕೀಲಿ(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ಯದಿ (isTZ) {
                                // replace colon with slash if necessary
                                ಯದಿ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ಕೀಲಿ_Type_String_Pool->create(fromView);
                                    ಯದಿ (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        ಭಙ್ಗ;
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
                    ಯದಿ (U_FAILURE(sts)) {
                        ಭಙ್ಗ;
                    }
                }

                ಯದಿ (bcp_Type_Alias_Res_By_ಕೀಲಿ.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias());
                    ಯಾವತ್ (ures_hasNext(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_ಕೀಲಿ.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ಧ್ರುವ char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        ಯದಿ (U_FAILURE(sts)) {
                            ಭಙ್ಗ;
                        }
                        // check if this is an alias of bcp type
                        ಯದಿ (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ಧ್ರುವ ವರ್ಣ* from = ures_get_ಕೀಲಿ(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    ಯದಿ (U_FAILURE(sts)) {
                        ಭಙ್ಗ;
                    }
                }
            }
        }
        ಯದಿ (U_FAILURE(sts)) {
            ಭಙ್ಗ;
        }

        Loc_Ext_ಕೀಲಿ_Data* ಕೀಲಿ_Data = g_Loc_Ext_ಕೀಲಿ_Data_Entries->create();
        ಯದಿ (ಕೀಲಿ_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            ಭಙ್ಗ;
        }
        ಕೀಲಿ_Data->bcpId = bcp_ಕೀಲಿ_Id;
        ಕೀಲಿ_Data->legacyId = legacy_ಕೀಲಿ_Id;
        ಕೀಲಿ_Data->specialTypes = specialTypes;
        ಕೀಲಿ_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ_Data->legacyId, ಕೀಲಿ_Data, &sts);
        ಯದಿ (legacy_ಕೀಲಿ_Id != bcp_ಕೀಲಿ_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ_Data->bcpId, ಕೀಲಿ_Data, &sts);
        }
        ಯದಿ (U_FAILURE(sts)) {
            ಭಙ್ಗ;
        }
    }
}

ತಾರ್ಕಿಕ
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ಕೀಲಿ_Map_Init_Once, &initFromResourceBundle, sts);
    ಯದಿ (U_FAILURE(sts)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    ಪ್ರತಿ (ವರ್ಣ c : val) {
        ಯದಿ (c == '-') {
            ಯದಿ (subtagLen < 4 || subtagLen > 6) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            subtagLen = 0;
        } ಅನ್ಯಥಾ ಯದಿ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ subtagLen >= 4 && subtagLen <= 6;
}

ತಾರ್ಕಿಕ
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    ಪ್ರತಿ (ವರ್ಣ c : val) {
        ಯದಿ (c == '-') {
            ಯದಿ (subtagLen < 3 || subtagLen > 8) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            subtagLen = 0;
        } ಅನ್ಯಥಾ ಯದಿ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ subtagLen >= 3 && subtagLen <= 8;
}

ತಾರ್ಕಿಕ
is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(std::string_view val) {
    int32_t subtagLen = 0;
    ಪ್ರತಿ (ವರ್ಣ c : val) {
        ಯದಿ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
    }
    ಪ್ರತ್ಯಾಗಮ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_ಕೀಲಿ(std::string_view ಕೀಲಿ) {
    ಯದಿ (!init()) {
        ಪ್ರತ್ಯಾಗಮ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_Data* ಕೀಲಿ_Data = static_cast<Loc_Ext_ಕೀಲಿ_Data*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಯದಿ (ಕೀಲಿ_Data != nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_Data->bcpId;
    }

    ಪ್ರತ್ಯಾಗಮ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_ಕೀಲಿ(std::string_view ಕೀಲಿ) {
    ಯದಿ (!init()) {
        ಪ್ರತ್ಯಾಗಮ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_Data* ಕೀಲಿ_Data = static_cast<Loc_Ext_ಕೀಲಿ_Data*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಯದಿ (ಕೀಲಿ_Data != nullptr) {
        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ_Data->legacyId;
    }

    ಪ್ರತ್ಯಾಗಮ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND) {
    ಯದಿ (!init()) {
        ಪ್ರತ್ಯಾಗಮ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_Data* ಕೀಲಿ_Data = static_cast<Loc_Ext_ಕೀಲಿ_Data*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಯದಿ (ಕೀಲಿ_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ಕೀಲಿ_Data->typeMap.getAlias(), &type));
        ಯದಿ (t != nullptr) {
            ಪ್ರತ್ಯಾಗಮ t->bcpId;
        }
        ಯದಿ (ಕೀಲಿ_Data->specialTypes != SPECIALTYPE_NONE) {
            ತಾರ್ಕಿಕ matched = ಅಸತ್ಯ;
            ಯದಿ (ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ಯದಿ (!matched && ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ಯದಿ (!matched && ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ) {
                matched = is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(type);
            }
            ಯದಿ (matched) {
                ಪ್ರತ್ಯಾಗಮ type;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view ಕೀಲಿ, std::string_view type U_LIFETIME_BOUND) {
    ಯದಿ (!init()) {
        ಪ್ರತ್ಯಾಗಮ std::nullopt;
    }

    Loc_Ext_ಕೀಲಿ_Data* ಕೀಲಿ_Data = static_cast<Loc_Ext_ಕೀಲಿ_Data*>(uhash_get(g_Loc_Ext_ಕೀಲಿ_Map, &ಕೀಲಿ));
    ಯದಿ (ಕೀಲಿ_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ಕೀಲಿ_Data->typeMap.getAlias(), &type));
        ಯದಿ (t != nullptr) {
            ಪ್ರತ್ಯಾಗಮ t->legacyId;
        }
        ಯದಿ (ಕೀಲಿ_Data->specialTypes != SPECIALTYPE_NONE) {
            ತಾರ್ಕಿಕ matched = ಅಸತ್ಯ;
            ಯದಿ (ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ಯದಿ (!matched && ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ಯದಿ (!matched && ಕೀಲಿ_Data->specialTypes & SPECIALTYPE_RG_ಕೀಲಿ_ಮೌಲ್ಯ) {
                matched = is_Special_Type_Rg_ಕೀಲಿ_ಮೌಲ್ಯ(type);
            }
            ಯದಿ (matched) {
                ಪ್ರತ್ಯಾಗಮ type;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ std::nullopt;
}
