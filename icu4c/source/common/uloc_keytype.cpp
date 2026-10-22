// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#ଅନ୍ତର୍ଭାଵ <algorithm>
#ଅନ୍ତର୍ଭାଵ <optional>
#ଅନ୍ତର୍ଭାଵ <string_view>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uobject.h"

#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "fixedstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"
#ଅନ୍ତର୍ଭାଵ "uhash.h"
#ଅନ୍ତର୍ଭାଵ "ulocimp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uresimp.h"
#ଅନ୍ତର୍ଭାଵ "uvector.h"
#ଅନ୍ତର୍ଭାଵ "udataswp.h" /* for InvChar functions */

ସ୍ଥିର UHashtable* g_Loc_Ext_ଚାବି_Map = nullptr;
ସ୍ଥିର icu::UInitOnce g_Loc_Ext_ଚାବି_Map_Init_Once {};

// bit flags for special types
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_ଚାବି_ମୂଲ୍ୟ = 4
} SpecialType;

ସଂରଚନା Loc_Ext_ଚାବି_Data : ସର୍ବସାଧାରଣ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

ସଂରଚନା LocExtType : ସର୍ବସାଧାରଣ icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

ସଂରଚନା TypeAlias : ସର୍ବସାଧାରଣ icu::UMemory {
    std::string_view from;
};

ସ୍ଥିର icu::MemoryPool<icu::FixedString>* g_ଚାବି_Type_String_Pool = nullptr;
ସ୍ଥିର icu::MemoryPool<Loc_Ext_ଚାବି_Data>* g_Loc_Ext_ଚାବି_Data_Entries = nullptr;
ସ୍ଥିର icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
ସ୍ଥିର icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

ସ୍ଥିର UBool U_CALLCONV
uloc_ଚାବି_type_cleanup() {
    ଯଦି (g_Loc_Ext_ଚାବି_Map != nullptr) {
        uhash_close(g_Loc_Ext_ଚାବି_Map);
        g_Loc_Ext_ଚାବି_Map = nullptr;
    }

    delete g_Loc_Ext_ଚାବି_Data_Entries;
    g_Loc_Ext_ଚାବି_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_ଚାବି_Type_String_Pool;
    g_ଚାବି_Type_String_Pool = nullptr;

    g_Loc_Ext_ଚାବି_Map_Init_Once.reset();
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

U_CDECL_END

ନାମାକାଶ {

ଶୂନ୍ୟ U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ଚାବି_TYPE, uloc_ଚାବି_type_cleanup);

    g_Loc_Ext_ଚାବି_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer ଚାବି_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer ଚାବି_Map_Res(ures_get_By_ଚାବି(ଚାବି_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_ଚାବି(ଚାବି_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    ଯଦି (U_FAILURE(sts)) {
        ଫେରନ୍ତୁ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_ଚାବି(ଚାବି_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_ଚାବି(ଚାବି_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ଚାବି_Type_String_Pool = ନୂତନ icu::MemoryPool<icu::FixedString>;
    ଯଦି (g_ଚାବି_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    g_Loc_Ext_ଚାବି_Data_Entries = ନୂତନ icu::MemoryPool<Loc_Ext_ଚାବି_Data>;
    ଯଦି (g_Loc_Ext_ଚାବି_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    gLocExtTypeEntries = ନୂତନ icu::MemoryPool<LocExtType>;
    ଯଦି (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }
    gTypeAliasEntries = ନୂତନ icu::MemoryPool<TypeAlias>;
    ଯଦି (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ଫେରନ୍ତୁ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer ଚାବି_Map_Entry;

    ଯେତେବେଳେ (ures_hasNext(ଚାବି_Map_Res.getAlias())) {
        ଚାବି_Map_Entry.adoptInstead(ures_getNextResource(ଚାବି_Map_Res.getAlias(), ଚାବି_Map_Entry.orphan(), &sts));
        ଯଦି (U_FAILURE(sts)) {
            ଭାଙ୍ଗ;
        }
        ଅଚଳ ଅକ୍ଷର* legacy_ଚାବି_Id = ures_get_ଚାବି(ଚାବି_Map_Entry.getAlias());
        UnicodeString u_Bcp_ଚାବି_Id = ures_getUnicodeString(ଚାବି_Map_Entry.getAlias(), &sts);
        ଯଦି (U_FAILURE(sts)) {
            ଭାଙ୍ଗ;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ଅଚଳ ଅକ୍ଷର* bcp_ଚାବି_Id = legacy_ଚାବି_Id;
        ଯଦି (!u_Bcp_ଚାବି_Id.isEmpty()) {
            icu::FixedString* bcp_ଚାବି_Id_Buf = g_ଚାବି_Type_String_Pool->create();
            ଯଦି (bcp_ଚାବି_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                ଭାଙ୍ଗ;
            }
            copyInvariantChars(u_Bcp_ଚାବି_Id, *bcp_ଚାବି_Id_Buf, sts);
            ଯଦି (U_FAILURE(sts)) {
                ଭାଙ୍ଗ;
            }
            bcp_ଚାବି_Id = bcp_ଚାବି_Id_Buf->data();
        }

        ତାର୍କିକ isTZ = uprv_strcmp(legacy_ଚାବି_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        ଯଦି (U_FAILURE(sts)) {
            ଭାଙ୍ଗ;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_ଚାବି;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_ଚାବି;

        ଯଦି (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ଚାବି.adoptInstead(ures_get_By_ଚାବି(typeAliasRes.getAlias(), legacy_ଚାବି_Id, nullptr, &tmpSts));
            ଯଦି (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ଚାବି.orphan();
            }
        }
        ଯଦି (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ଚାବି.adoptInstead(ures_get_By_ଚାବି(bcpTypeAliasRes.getAlias(), bcp_ଚାବି_Id, nullptr, &tmpSts));
            ଯଦି (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ଚାବି.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_ଚାବି(ures_get_By_ଚାବି(typeMapRes.getAlias(), legacy_ଚାବି_Id, nullptr, &sts));
        ଯଦି (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ଅନ୍ୟଥା {
            LocalUResourceBundlePointer typeMapEntry;

            ଯେତେବେଳେ (ures_hasNext(type_Map_Res_By_ଚାବି.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_ଚାବି.getAlias(), typeMapEntry.orphan(), &sts));
                ଯଦି (U_FAILURE(sts)) {
                    ଭାଙ୍ଗ;
                }
                ଅଚଳ ଅକ୍ଷର* legacyTypeId = ures_get_ଚାବି(typeMapEntry.getAlias());

                // special types
                ଯଦି (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ଜାରି;
                }
                ଯଦି (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    ଜାରି;
                }
                ଯଦି (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ଚାବି_ମୂଲ୍ୟ;
                    ଜାରି;
                }

                ଯଦି (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ଯଦି (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ଚାବି_Type_String_Pool->create(legacyTypeIdView);
                        ଯଦି (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            ଭାଙ୍ଗ;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                ଯଦି (U_FAILURE(sts)) {
                    ଭାଙ୍ଗ;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ଅଚଳ ଅକ୍ଷର* bcpTypeId = legacyTypeId;
                ଯଦି (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_ଚାବି_Type_String_Pool->create();
                    ଯଦି (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        ଭାଙ୍ଗ;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ଯଦି (U_FAILURE(sts)) {
                        ଭାଙ୍ଗ;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ଯଦି (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    ଭାଙ୍ଗ;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                ଯଦି (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                ଯଦି (U_FAILURE(sts)) {
                    ଭାଙ୍ଗ;
                }

                // also put aliases in the map
                ଯଦି (type_Alias_Res_By_ଚାବି.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_ଚାବି.getAlias());
                    ଯେତେବେଳେ (ures_hasNext(type_Alias_Res_By_ଚାବି.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_ଚାବି.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ଅଚଳ char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        ଯଦି (U_FAILURE(sts)) {
                            ଭାଙ୍ଗ;
                        }
                        // check if this is an alias of canonical legacy type
                        ଯଦି (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ଅଚଳ ଅକ୍ଷର* from = ures_get_ଚାବି(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ଯଦି (isTZ) {
                                // replace colon with slash if necessary
                                ଯଦି (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ଚାବି_Type_String_Pool->create(fromView);
                                    ଯଦି (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        ଭାଙ୍ଗ;
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
                    ଯଦି (U_FAILURE(sts)) {
                        ଭାଙ୍ଗ;
                    }
                }

                ଯଦି (bcp_Type_Alias_Res_By_ଚାବି.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ଚାବି.getAlias());
                    ଯେତେବେଳେ (ures_hasNext(bcp_Type_Alias_Res_By_ଚାବି.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_ଚାବି.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ଅଚଳ char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        ଯଦି (U_FAILURE(sts)) {
                            ଭାଙ୍ଗ;
                        }
                        // check if this is an alias of bcp type
                        ଯଦି (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ଅଚଳ ଅକ୍ଷର* from = ures_get_ଚାବି(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    ଯଦି (U_FAILURE(sts)) {
                        ଭାଙ୍ଗ;
                    }
                }
            }
        }
        ଯଦି (U_FAILURE(sts)) {
            ଭାଙ୍ଗ;
        }

        Loc_Ext_ଚାବି_Data* ଚାବି_Data = g_Loc_Ext_ଚାବି_Data_Entries->create();
        ଯଦି (ଚାବି_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            ଭାଙ୍ଗ;
        }
        ଚାବି_Data->bcpId = bcp_ଚାବି_Id;
        ଚାବି_Data->legacyId = legacy_ଚାବି_Id;
        ଚାବି_Data->specialTypes = specialTypes;
        ଚାବି_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_ଚାବି_Map, &ଚାବି_Data->legacyId, ଚାବି_Data, &sts);
        ଯଦି (legacy_ଚାବି_Id != bcp_ଚାବି_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ଚାବି_Map, &ଚାବି_Data->bcpId, ଚାବି_Data, &sts);
        }
        ଯଦି (U_FAILURE(sts)) {
            ଭାଙ୍ଗ;
        }
    }
}

ତାର୍କିକ
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ଚାବି_Map_Init_Once, &initFromResourceBundle, sts);
    ଯଦି (U_FAILURE(sts)) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

ତାର୍କିକ
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    ପାଇଁ (ଅକ୍ଷର c : val) {
        ଯଦି (c == '-') {
            ଯଦି (subtagLen < 4 || subtagLen > 6) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            subtagLen = 0;
        } ଅନ୍ୟଥା ଯଦି ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ subtagLen >= 4 && subtagLen <= 6;
}

ତାର୍କିକ
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    ପାଇଁ (ଅକ୍ଷର c : val) {
        ଯଦି (c == '-') {
            ଯଦି (subtagLen < 3 || subtagLen > 8) {
                ଫେରନ୍ତୁ ମିଥ୍ୟା;
            }
            subtagLen = 0;
        } ଅନ୍ୟଥା ଯଦି (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ subtagLen >= 3 && subtagLen <= 8;
}

ତାର୍କିକ
is_Special_Type_Rg_ଚାବି_ମୂଲ୍ୟ(std::string_view val) {
    int32_t subtagLen = 0;
    ପାଇଁ (ଅକ୍ଷର c : val) {
        ଯଦି ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ ମିଥ୍ୟା;
        }
    }
    ଫେରନ୍ତୁ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_ଚାବି(std::string_view ଚାବି) {
    ଯଦି (!init()) {
        ଫେରନ୍ତୁ std::nullopt;
    }

    Loc_Ext_ଚାବି_Data* ଚାବି_Data = static_cast<Loc_Ext_ଚାବି_Data*>(uhash_get(g_Loc_Ext_ଚାବି_Map, &ଚାବି));
    ଯଦି (ଚାବି_Data != nullptr) {
        ଫେରନ୍ତୁ ଚାବି_Data->bcpId;
    }

    ଫେରନ୍ତୁ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_ଚାବି(std::string_view ଚାବି) {
    ଯଦି (!init()) {
        ଫେରନ୍ତୁ std::nullopt;
    }

    Loc_Ext_ଚାବି_Data* ଚାବି_Data = static_cast<Loc_Ext_ଚାବି_Data*>(uhash_get(g_Loc_Ext_ଚାବି_Map, &ଚାବି));
    ଯଦି (ଚାବି_Data != nullptr) {
        ଫେରନ୍ତୁ ଚାବି_Data->legacyId;
    }

    ଫେରନ୍ତୁ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view ଚାବି, std::string_view type U_LIFETIME_BOUND) {
    ଯଦି (!init()) {
        ଫେରନ୍ତୁ std::nullopt;
    }

    Loc_Ext_ଚାବି_Data* ଚାବି_Data = static_cast<Loc_Ext_ଚାବି_Data*>(uhash_get(g_Loc_Ext_ଚାବି_Map, &ଚାବି));
    ଯଦି (ଚାବି_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ଚାବି_Data->typeMap.getAlias(), &type));
        ଯଦି (t != nullptr) {
            ଫେରନ୍ତୁ t->bcpId;
        }
        ଯଦି (ଚାବି_Data->specialTypes != SPECIALTYPE_NONE) {
            ତାର୍କିକ matched = ମିଥ୍ୟା;
            ଯଦି (ଚାବି_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ଯଦି (!matched && ଚାବି_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ଯଦି (!matched && ଚାବି_Data->specialTypes & SPECIALTYPE_RG_ଚାବି_ମୂଲ୍ୟ) {
                matched = is_Special_Type_Rg_ଚାବି_ମୂଲ୍ୟ(type);
            }
            ଯଦି (matched) {
                ଫେରନ୍ତୁ type;
            }
        }
    }

    ଫେରନ୍ତୁ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view ଚାବି, std::string_view type U_LIFETIME_BOUND) {
    ଯଦି (!init()) {
        ଫେରନ୍ତୁ std::nullopt;
    }

    Loc_Ext_ଚାବି_Data* ଚାବି_Data = static_cast<Loc_Ext_ଚାବି_Data*>(uhash_get(g_Loc_Ext_ଚାବି_Map, &ଚାବି));
    ଯଦି (ଚାବି_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ଚାବି_Data->typeMap.getAlias(), &type));
        ଯଦି (t != nullptr) {
            ଫେରନ୍ତୁ t->legacyId;
        }
        ଯଦି (ଚାବି_Data->specialTypes != SPECIALTYPE_NONE) {
            ତାର୍କିକ matched = ମିଥ୍ୟା;
            ଯଦି (ଚାବି_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ଯଦି (!matched && ଚାବି_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ଯଦି (!matched && ଚାବି_Data->specialTypes & SPECIALTYPE_RG_ଚାବି_ମୂଲ୍ୟ) {
                matched = is_Special_Type_Rg_ଚାବି_ମୂଲ୍ୟ(type);
            }
            ଯଦି (matched) {
                ଫେରନ୍ତୁ type;
            }
        }
    }

    ଫେରନ୍ତୁ std::nullopt;
}
