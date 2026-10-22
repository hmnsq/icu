// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 2014-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*/
#અન્તર્ભાવ <algorithm>
#અન્તર્ભાવ <optional>
#અન્તર્ભાવ <string_view>

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/unistr.h"
#અન્તર્ભાવ "unicode/uobject.h"

#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "fixedstring.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "uhash.h"
#અન્તર્ભાવ "ulocimp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "uresimp.h"
#અન્તર્ભાવ "uvector.h"
#અન્તર્ભાવ "udataswp.h" /* for InvChar functions */

સ્થિર UHashtable* g_Loc_Ext_ચાવી_Map = nullptr;
સ્થિર icu::UInitOnce g_Loc_Ext_ચાવી_Map_Init_Once {};

// bit flags for special types
પ્રકારવ્યાખ્યા ગણના {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_ચાવી_કિંમત = 4
} SpecialType;

સંરચના Loc_Ext_ચાવી_Data : જાહેર icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

સંરચના LocExtType : જાહેર icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

સંરચના TypeAlias : જાહેર icu::UMemory {
    std::string_view from;
};

સ્થિર icu::MemoryPool<icu::FixedString>* g_ચાવી_Type_String_Pool = nullptr;
સ્થિર icu::MemoryPool<Loc_Ext_ચાવી_Data>* g_Loc_Ext_ચાવી_Data_Entries = nullptr;
સ્થિર icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
સ્થિર icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

સ્થિર UBool U_CALLCONV
uloc_ચાવી_type_cleanup() {
    જો (g_Loc_Ext_ચાવી_Map != nullptr) {
        uhash_close(g_Loc_Ext_ચાવી_Map);
        g_Loc_Ext_ચાવી_Map = nullptr;
    }

    delete g_Loc_Ext_ચાવી_Data_Entries;
    g_Loc_Ext_ચાવી_Data_Entries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete g_ચાવી_Type_String_Pool;
    g_ચાવી_Type_String_Pool = nullptr;

    g_Loc_Ext_ચાવી_Map_Init_Once.reset();
    પાછા સાચું;
}

U_CDECL_END

નામાકાશ {

શૂન્ય U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_ચાવી_TYPE, uloc_ચાવી_type_cleanup);

    g_Loc_Ext_ચાવી_Map = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer ચાવી_Type_Data_Res(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer ચાવી_Map_Res(ures_get_By_ચાવી(ચાવી_Type_Data_Res.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_get_By_ચાવી(ચાવી_Type_Data_Res.getAlias(), "typeMap", nullptr, &sts));

    જો (U_FAILURE(sts)) {
        પાછા;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_get_By_ચાવી(ચાવી_Type_Data_Res.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_get_By_ચાવી(ચાવી_Type_Data_Res.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    g_ચાવી_Type_String_Pool = નવું icu::MemoryPool<icu::FixedString>;
    જો (g_ચાવી_Type_String_Pool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    g_Loc_Ext_ચાવી_Data_Entries = નવું icu::MemoryPool<Loc_Ext_ચાવી_Data>;
    જો (g_Loc_Ext_ચાવી_Data_Entries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    gLocExtTypeEntries = નવું icu::MemoryPool<LocExtType>;
    જો (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }
    gTypeAliasEntries = નવું icu::MemoryPool<TypeAlias>;
    જો (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        પાછા;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer ચાવી_Map_Entry;

    જ્યારે (ures_hasNext(ચાવી_Map_Res.getAlias())) {
        ચાવી_Map_Entry.adoptInstead(ures_getNextResource(ચાવી_Map_Res.getAlias(), ચાવી_Map_Entry.orphan(), &sts));
        જો (U_FAILURE(sts)) {
            તોડો;
        }
        અચલ અક્ષર* legacy_ચાવી_Id = ures_get_ચાવી(ચાવી_Map_Entry.getAlias());
        UnicodeString u_Bcp_ચાવી_Id = ures_getUnicodeString(ચાવી_Map_Entry.getAlias(), &sts);
        જો (U_FAILURE(sts)) {
            તોડો;
        }

        // empty value indicates that BCP key is same with the legacy key.
        અચલ અક્ષર* bcp_ચાવી_Id = legacy_ચાવી_Id;
        જો (!u_Bcp_ચાવી_Id.isEmpty()) {
            icu::FixedString* bcp_ચાવી_Id_Buf = g_ચાવી_Type_String_Pool->create();
            જો (bcp_ચાવી_Id_Buf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                તોડો;
            }
            copyInvariantChars(u_Bcp_ચાવી_Id, *bcp_ચાવી_Id_Buf, sts);
            જો (U_FAILURE(sts)) {
                તોડો;
            }
            bcp_ચાવી_Id = bcp_ચાવી_Id_Buf->data();
        }

        તાર્કિક isTZ = uprv_strcmp(legacy_ચાવી_Id, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        જો (U_FAILURE(sts)) {
            તોડો;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer type_Alias_Res_By_ચાવી;
        LocalUResourceBundlePointer bcp_Type_Alias_Res_By_ચાવી;

        જો (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            type_Alias_Res_By_ચાવી.adoptInstead(ures_get_By_ચાવી(typeAliasRes.getAlias(), legacy_ચાવી_Id, nullptr, &tmpSts));
            જો (U_FAILURE(tmpSts)) {
                type_Alias_Res_By_ચાવી.orphan();
            }
        }
        જો (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcp_Type_Alias_Res_By_ચાવી.adoptInstead(ures_get_By_ચાવી(bcpTypeAliasRes.getAlias(), bcp_ચાવી_Id, nullptr, &tmpSts));
            જો (U_FAILURE(tmpSts)) {
                bcp_Type_Alias_Res_By_ચાવી.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer type_Map_Res_By_ચાવી(ures_get_By_ચાવી(typeMapRes.getAlias(), legacy_ચાવી_Id, nullptr, &sts));
        જો (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } નહીંતર {
            LocalUResourceBundlePointer typeMapEntry;

            જ્યારે (ures_hasNext(type_Map_Res_By_ચાવી.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(type_Map_Res_By_ચાવી.getAlias(), typeMapEntry.orphan(), &sts));
                જો (U_FAILURE(sts)) {
                    તોડો;
                }
                અચલ અક્ષર* legacyTypeId = ures_get_ચાવી(typeMapEntry.getAlias());

                // special types
                જો (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    ચાલુ;
                }
                જો (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    ચાલુ;
                }
                જો (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_ચાવી_કિંમત;
                    ચાલુ;
                }

                જો (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    જો (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = g_ચાવી_Type_String_Pool->create(legacyTypeIdView);
                        જો (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            તોડો;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                જો (U_FAILURE(sts)) {
                    તોડો;
                }

                // empty value indicates that BCP type is same with the legacy type.
                અચલ અક્ષર* bcpTypeId = legacyTypeId;
                જો (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = g_ચાવી_Type_String_Pool->create();
                    જો (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        તોડો;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    જો (U_FAILURE(sts)) {
                        તોડો;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                જો (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    તોડો;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                જો (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                જો (U_FAILURE(sts)) {
                    તોડો;
                }

                // also put aliases in the map
                જો (type_Alias_Res_By_ચાવી.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(type_Alias_Res_By_ચાવી.getAlias());
                    જ્યારે (ures_hasNext(type_Alias_Res_By_ચાવી.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(type_Alias_Res_By_ચાવી.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        અચલ char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        જો (U_FAILURE(sts)) {
                            તોડો;
                        }
                        // check if this is an alias of canonical legacy type
                        જો (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            અચલ અક્ષર* from = ures_get_ચાવી(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            જો (isTZ) {
                                // replace colon with slash if necessary
                                જો (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = g_ચાવી_Type_String_Pool->create(fromView);
                                    જો (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        તોડો;
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
                    જો (U_FAILURE(sts)) {
                        તોડો;
                    }
                }

                જો (bcp_Type_Alias_Res_By_ચાવી.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcp_Type_Alias_Res_By_ચાવી.getAlias());
                    જ્યારે (ures_hasNext(bcp_Type_Alias_Res_By_ચાવી.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcp_Type_Alias_Res_By_ચાવી.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        અચલ char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        જો (U_FAILURE(sts)) {
                            તોડો;
                        }
                        // check if this is an alias of bcp type
                        જો (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            અચલ અક્ષર* from = ures_get_ચાવી(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    જો (U_FAILURE(sts)) {
                        તોડો;
                    }
                }
            }
        }
        જો (U_FAILURE(sts)) {
            તોડો;
        }

        Loc_Ext_ચાવી_Data* ચાવી_Data = g_Loc_Ext_ચાવી_Data_Entries->create();
        જો (ચાવી_Data == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            તોડો;
        }
        ચાવી_Data->bcpId = bcp_ચાવી_Id;
        ચાવી_Data->legacyId = legacy_ચાવી_Id;
        ચાવી_Data->specialTypes = specialTypes;
        ચાવી_Data->typeMap.adoptInstead(typeDataMap);

        uhash_put(g_Loc_Ext_ચાવી_Map, &ચાવી_Data->legacyId, ચાવી_Data, &sts);
        જો (legacy_ચાવી_Id != bcp_ચાવી_Id) {
            // different key value
            uhash_put(g_Loc_Ext_ચાવી_Map, &ચાવી_Data->bcpId, ચાવી_Data, &sts);
        }
        જો (U_FAILURE(sts)) {
            તોડો;
        }
    }
}

તાર્કિક
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(g_Loc_Ext_ચાવી_Map_Init_Once, &initFromResourceBundle, sts);
    જો (U_FAILURE(sts)) {
        પાછા ખોટું;
    }
    પાછા સાચું;
}

તાર્કિક
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    માટે (અક્ષર c : val) {
        જો (c == '-') {
            જો (subtagLen < 4 || subtagLen > 6) {
                પાછા ખોટું;
            }
            subtagLen = 0;
        } નહીંતર જો ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } નહીંતર {
            પાછા ખોટું;
        }
    }
    પાછા subtagLen >= 4 && subtagLen <= 6;
}

તાર્કિક
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    માટે (અક્ષર c : val) {
        જો (c == '-') {
            જો (subtagLen < 3 || subtagLen > 8) {
                પાછા ખોટું;
            }
            subtagLen = 0;
        } નહીંતર જો (uprv_isASCIILetter(c)) {
            subtagLen++;
        } નહીંતર {
            પાછા ખોટું;
        }
    }
    પાછા subtagLen >= 3 && subtagLen <= 8;
}

તાર્કિક
is_Special_Type_Rg_ચાવી_કિંમત(std::string_view val) {
    int32_t subtagLen = 0;
    માટે (અક્ષર c : val) {
        જો ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } નહીંતર {
            પાછા ખોટું;
        }
    }
    પાછા subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_to_Bcp_ચાવી(std::string_view ચાવી) {
    જો (!init()) {
        પાછા std::nullopt;
    }

    Loc_Ext_ચાવી_Data* ચાવી_Data = static_cast<Loc_Ext_ચાવી_Data*>(uhash_get(g_Loc_Ext_ચાવી_Map, &ચાવી));
    જો (ચાવી_Data != nullptr) {
        પાછા ચાવી_Data->bcpId;
    }

    પાછા std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_to_Legacy_ચાવી(std::string_view ચાવી) {
    જો (!init()) {
        પાછા std::nullopt;
    }

    Loc_Ext_ચાવી_Data* ચાવી_Data = static_cast<Loc_Ext_ચાવી_Data*>(uhash_get(g_Loc_Ext_ચાવી_Map, &ચાવી));
    જો (ચાવી_Data != nullptr) {
        પાછા ચાવી_Data->legacyId;
    }

    પાછા std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view ચાવી, std::string_view type U_LIFETIME_BOUND) {
    જો (!init()) {
        પાછા std::nullopt;
    }

    Loc_Ext_ચાવી_Data* ચાવી_Data = static_cast<Loc_Ext_ચાવી_Data*>(uhash_get(g_Loc_Ext_ચાવી_Map, &ચાવી));
    જો (ચાવી_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ચાવી_Data->typeMap.getAlias(), &type));
        જો (t != nullptr) {
            પાછા t->bcpId;
        }
        જો (ચાવી_Data->specialTypes != SPECIALTYPE_NONE) {
            તાર્કિક matched = ખોટું;
            જો (ચાવી_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            જો (!matched && ચાવી_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            જો (!matched && ચાવી_Data->specialTypes & SPECIALTYPE_RG_ચાવી_કિંમત) {
                matched = is_Special_Type_Rg_ચાવી_કિંમત(type);
            }
            જો (matched) {
                પાછા type;
            }
        }
    }

    પાછા std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view ચાવી, std::string_view type U_LIFETIME_BOUND) {
    જો (!init()) {
        પાછા std::nullopt;
    }

    Loc_Ext_ચાવી_Data* ચાવી_Data = static_cast<Loc_Ext_ચાવી_Data*>(uhash_get(g_Loc_Ext_ચાવી_Map, &ચાવી));
    જો (ચાવી_Data != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(ચાવી_Data->typeMap.getAlias(), &type));
        જો (t != nullptr) {
            પાછા t->legacyId;
        }
        જો (ચાવી_Data->specialTypes != SPECIALTYPE_NONE) {
            તાર્કિક matched = ખોટું;
            જો (ચાવી_Data->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            જો (!matched && ચાવી_Data->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            જો (!matched && ચાવી_Data->specialTypes & SPECIALTYPE_RG_ચાવી_કિંમત) {
                matched = is_Special_Type_Rg_ચાવી_કિંમત(type);
            }
            જો (matched) {
                પાછા type;
            }
        }
    }

    પાછા std::nullopt;
}
