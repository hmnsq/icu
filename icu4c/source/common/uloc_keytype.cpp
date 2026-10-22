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

ساكن UHashtable* gLocExtKeyMap = nullptr;
ساكن icu::UInitOnce gLocExtKeyMapInitOnce {};

// bit flags for special types
تعريفنوع تعداد {
    SPECIALTYPE_NONE = 0,
    SPECIALTYPE_CODEPOINTS = 1,
    SPECIALTYPE_REORDER_CODE = 2,
    SPECIALTYPE_RG_KEY_VALUE = 4
} SpecialType;

بنية LocExtKeyData : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
    icu::LocalUHashtablePointer typeMap;
    uint32_t        specialTypes;
};

بنية LocExtType : عام icu::UMemory {
    std::string_view legacyId;
    std::string_view bcpId;
};

بنية TypeAlias : عام icu::UMemory {
    std::string_view from;
};

ساكن icu::MemoryPool<icu::FixedString>* gKeyTypeStringPool = nullptr;
ساكن icu::MemoryPool<LocExtKeyData>* gLocExtKeyDataEntries = nullptr;
ساكن icu::MemoryPool<LocExtType>* gLocExtTypeEntries = nullptr;
ساكن icu::MemoryPool<TypeAlias>* gTypeAliasEntries = nullptr;

U_CDECL_BEGIN

ساكن UBool U_CALLCONV
uloc_key_type_cleanup() {
    ܐܢ (gLocExtKeyMap != nullptr) {
        uhash_close(gLocExtKeyMap);
        gLocExtKeyMap = nullptr;
    }

    delete gLocExtKeyDataEntries;
    gLocExtKeyDataEntries = nullptr;

    delete gLocExtTypeEntries;
    gLocExtTypeEntries = nullptr;

    delete gTypeAliasEntries;
    gTypeAliasEntries = nullptr;

    delete gKeyTypeStringPool;
    gKeyTypeStringPool = nullptr;

    gLocExtKeyMapInitOnce.reset();
    ܦܢܐ ܫܪܪܐ;
}

U_CDECL_END

نطاق {

فراغ U_CALLCONV
initFromResourceBundle(UErrorCode& sts) {
    U_NAMESPACE_USE
    ucln_common_registerCleanup(UCLN_COMMON_LOCALE_KEY_TYPE, uloc_key_type_cleanup);

    gLocExtKeyMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);

    LocalUResourceBundlePointer keyTypeDataRes(ures_openDirect(nullptr, "keyTypeData", &sts));
    LocalUResourceBundlePointer keyMapRes(ures_getByKey(keyTypeDataRes.getAlias(), "keyMap", nullptr, &sts));
    LocalUResourceBundlePointer typeMapRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeMap", nullptr, &sts));

    ܐܢ (U_FAILURE(sts)) {
        ܦܢܐ;
    }

    UErrorCode tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer typeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "typeAlias", nullptr, &tmpSts));
    tmpSts = U_ZERO_ERROR;
    LocalUResourceBundlePointer bcpTypeAliasRes(ures_getByKey(keyTypeDataRes.getAlias(), "bcpTypeAlias", nullptr, &tmpSts));

    // initialize pools storing dynamically allocated objects
    gKeyTypeStringPool = جديد icu::MemoryPool<icu::FixedString>;
    ܐܢ (gKeyTypeStringPool == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    gLocExtKeyDataEntries = جديد icu::MemoryPool<LocExtKeyData>;
    ܐܢ (gLocExtKeyDataEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    gLocExtTypeEntries = جديد icu::MemoryPool<LocExtType>;
    ܐܢ (gLocExtTypeEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }
    gTypeAliasEntries = جديد icu::MemoryPool<TypeAlias>;
    ܐܢ (gTypeAliasEntries == nullptr) {
        sts = U_MEMORY_ALLOCATION_ERROR;
        ܦܢܐ;
    }

    // iterate through keyMap resource
    LocalUResourceBundlePointer keyMapEntry;

    ܟܡܐ (ures_hasNext(keyMapRes.getAlias())) {
        keyMapEntry.adoptInstead(ures_getNextResource(keyMapRes.getAlias(), keyMapEntry.orphan(), &sts));
        ܐܢ (U_FAILURE(sts)) {
            اكسر;
        }
        ثابت حرف* legacyKeyId = ures_getKey(keyMapEntry.getAlias());
        UnicodeString uBcpKeyId = ures_getUnicodeString(keyMapEntry.getAlias(), &sts);
        ܐܢ (U_FAILURE(sts)) {
            اكسر;
        }

        // empty value indicates that BCP key is same with the legacy key.
        ثابت حرف* bcpKeyId = legacyKeyId;
        ܐܢ (!uBcpKeyId.isEmpty()) {
            icu::FixedString* bcpKeyIdBuf = gKeyTypeStringPool->create();
            ܐܢ (bcpKeyIdBuf == nullptr) {
                sts = U_MEMORY_ALLOCATION_ERROR;
                اكسر;
            }
            copyInvariantChars(uBcpKeyId, *bcpKeyIdBuf, sts);
            ܐܢ (U_FAILURE(sts)) {
                اكسر;
            }
            bcpKeyId = bcpKeyIdBuf->data();
        }

        منطقي isTZ = uprv_strcmp(legacyKeyId, "timezone") == 0;

        UHashtable* typeDataMap = uhash_open(uhash_hashIStringView, uhash_compareIStringView, nullptr, &sts);
        ܐܢ (U_FAILURE(sts)) {
            اكسر;
        }
        uint32_t specialTypes = SPECIALTYPE_NONE;

        LocalUResourceBundlePointer typeAliasResByKey;
        LocalUResourceBundlePointer bcpTypeAliasResByKey;

        ܐܢ (typeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            typeAliasResByKey.adoptInstead(ures_getByKey(typeAliasRes.getAlias(), legacyKeyId, nullptr, &tmpSts));
            ܐܢ (U_FAILURE(tmpSts)) {
                typeAliasResByKey.orphan();
            }
        }
        ܐܢ (bcpTypeAliasRes.isValid()) {
            tmpSts = U_ZERO_ERROR;
            bcpTypeAliasResByKey.adoptInstead(ures_getByKey(bcpTypeAliasRes.getAlias(), bcpKeyId, nullptr, &tmpSts));
            ܐܢ (U_FAILURE(tmpSts)) {
                bcpTypeAliasResByKey.orphan();
            }
        }

        // look up type map for the key, and walk through the mapping data
        LocalUResourceBundlePointer typeMapResByKey(ures_getByKey(typeMapRes.getAlias(), legacyKeyId, nullptr, &sts));
        ܐܢ (U_FAILURE(sts)) {
            // We fail here if typeMap does not have an entry corresponding to every entry in keyMap (should
            // not happen for valid keyTypeData), or if ures_getByKeyfails fails for some other reason
            // (e.g. data file cannot be loaded, using stubdata, over-aggressive data filtering has removed
            // something like timezoneTypes.res, etc.). Error code is already set. See ICU-21669.
            UPRV_UNREACHABLE_ASSERT;
        } ܐܚܪܢܐ {
            LocalUResourceBundlePointer typeMapEntry;

            ܟܡܐ (ures_hasNext(typeMapResByKey.getAlias())) {
                typeMapEntry.adoptInstead(ures_getNextResource(typeMapResByKey.getAlias(), typeMapEntry.orphan(), &sts));
                ܐܢ (U_FAILURE(sts)) {
                    اكسر;
                }
                ثابت حرف* legacyTypeId = ures_getKey(typeMapEntry.getAlias());

                // special types
                ܐܢ (uprv_strcmp(legacyTypeId, "CODEPOINTS") == 0) {
                    specialTypes |= SPECIALTYPE_CODEPOINTS;
                    استمر;
                }
                ܐܢ (uprv_strcmp(legacyTypeId, "REORDER_CODE") == 0) {
                    specialTypes |= SPECIALTYPE_REORDER_CODE;
                    استمر;
                }
                ܐܢ (uprv_strcmp(legacyTypeId, "RG_KEY_VALUE") == 0) {
                    specialTypes |= SPECIALTYPE_RG_KEY_VALUE;
                    استمر;
                }

                ܐܢ (isTZ) {
                    // a timezone key uses a colon instead of a slash in the resource.
                    // e.g. America:Los_Angeles
                    ܐܢ (uprv_strchr(legacyTypeId, ':') != nullptr) {
                        U_ASSERT(legacyTypeId != nullptr && *legacyTypeId != '\0');
                        std::string_view legacyTypeIdView = legacyTypeId;
                        icu::FixedString* legacyTypeIdBuf = gKeyTypeStringPool->create(legacyTypeIdView);
                        ܐܢ (legacyTypeIdBuf == nullptr || legacyTypeIdBuf->isEmpty()) {
                            sts = U_MEMORY_ALLOCATION_ERROR;
                            اكسر;
                        }
                        std::replace(
                                legacyTypeIdBuf->getAlias(),
                                legacyTypeIdBuf->getAlias() + legacyTypeIdView.length(),
                                ':', '/');
                        legacyTypeId = legacyTypeIdBuf->data();
                    }
                }

                UnicodeString uBcpTypeId = ures_getUnicodeString(typeMapEntry.getAlias(), &sts);
                ܐܢ (U_FAILURE(sts)) {
                    اكسر;
                }

                // empty value indicates that BCP type is same with the legacy type.
                ثابت حرف* bcpTypeId = legacyTypeId;
                ܐܢ (!uBcpTypeId.isEmpty()) {
                    icu::FixedString* bcpTypeIdBuf = gKeyTypeStringPool->create();
                    ܐܢ (bcpTypeIdBuf == nullptr) {
                        sts = U_MEMORY_ALLOCATION_ERROR;
                        اكسر;
                    }
                    copyInvariantChars(uBcpTypeId, *bcpTypeIdBuf, sts);
                    ܐܢ (U_FAILURE(sts)) {
                        اكسر;
                    }
                    bcpTypeId = bcpTypeIdBuf->data();
                }

                // Note: legacy type value should never be
                // equivalent to bcp type value of a different
                // type under the same key. So we use a single
                // map for lookup.
                LocExtType* t = gLocExtTypeEntries->create();
                ܐܢ (t == nullptr) {
                    sts = U_MEMORY_ALLOCATION_ERROR;
                    اكسر;
                }
                t->bcpId = bcpTypeId;
                t->legacyId = legacyTypeId;

                uhash_put(typeDataMap, &t->legacyId, t, &sts);
                ܐܢ (bcpTypeId != legacyTypeId) {
                    // different type value
                    uhash_put(typeDataMap, &t->bcpId, t, &sts);
                }
                ܐܢ (U_FAILURE(sts)) {
                    اكسر;
                }

                // also put aliases in the map
                ܐܢ (typeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer typeAliasDataEntry;

                    ures_resetIterator(typeAliasResByKey.getAlias());
                    ܟܡܐ (ures_hasNext(typeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        typeAliasDataEntry.adoptInstead(ures_getNextResource(typeAliasResByKey.getAlias(), typeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(typeAliasDataEntry.getAlias(), &toLen, &sts);
                        ܐܢ (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of canonical legacy type
                        ܐܢ (uprv_compareInvWithUChar(
                                nullptr,
                                t->legacyId.data(),
                                static_cast<int32_t>(t->legacyId.size()),
                                to,
                                toLen) == 0) {
                            ثابت حرف* from = ures_getKey(typeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            ܐܢ (isTZ) {
                                // replace colon with slash if necessary
                                ܐܢ (uprv_strchr(from, ':') != nullptr) {
                                    U_ASSERT(from != nullptr && *from != '\0');
                                    std::string_view fromView = from;
                                    icu::FixedString* fromBuf = gKeyTypeStringPool->create(fromView);
                                    ܐܢ (fromBuf == nullptr || fromBuf->isEmpty()) {
                                        sts = U_MEMORY_ALLOCATION_ERROR;
                                        اكسر;
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
                    ܐܢ (U_FAILURE(sts)) {
                        اكسر;
                    }
                }

                ܐܢ (bcpTypeAliasResByKey.isValid()) {
                    LocalUResourceBundlePointer bcpTypeAliasDataEntry;

                    ures_resetIterator(bcpTypeAliasResByKey.getAlias());
                    ܟܡܐ (ures_hasNext(bcpTypeAliasResByKey.getAlias()) && U_SUCCESS(sts)) {
                        int32_t toLen;
                        bcpTypeAliasDataEntry.adoptInstead(ures_getNextResource(bcpTypeAliasResByKey.getAlias(), bcpTypeAliasDataEntry.orphan(), &sts));
                        ثابت char16_t* to = ures_getString(bcpTypeAliasDataEntry.getAlias(), &toLen, &sts);
                        ܐܢ (U_FAILURE(sts)) {
                            اكسر;
                        }
                        // check if this is an alias of bcp type
                        ܐܢ (uprv_compareInvWithUChar(
                                nullptr,
                                t->bcpId.data(),
                                static_cast<int32_t>(t->bcpId.size()),
                                to,
                                toLen) == 0) {
                            ثابت حرف* from = ures_getKey(bcpTypeAliasDataEntry.getAlias());
                            TypeAlias* alias = gTypeAliasEntries->create(TypeAlias{{}, from});
                            uhash_put(typeDataMap, &alias->from, t, &sts);
                        }
                    }
                    ܐܢ (U_FAILURE(sts)) {
                        اكسر;
                    }
                }
            }
        }
        ܐܢ (U_FAILURE(sts)) {
            اكسر;
        }

        LocExtKeyData* keyData = gLocExtKeyDataEntries->create();
        ܐܢ (keyData == nullptr) {
            sts = U_MEMORY_ALLOCATION_ERROR;
            اكسر;
        }
        keyData->bcpId = bcpKeyId;
        keyData->legacyId = legacyKeyId;
        keyData->specialTypes = specialTypes;
        keyData->typeMap.adoptInstead(typeDataMap);

        uhash_put(gLocExtKeyMap, &keyData->legacyId, keyData, &sts);
        ܐܢ (legacyKeyId != bcpKeyId) {
            // different key value
            uhash_put(gLocExtKeyMap, &keyData->bcpId, keyData, &sts);
        }
        ܐܢ (U_FAILURE(sts)) {
            اكسر;
        }
    }
}

منطقي
init() {
    UErrorCode sts = U_ZERO_ERROR;
    umtx_initOnce(gLocExtKeyMapInitOnce, &initFromResourceBundle, sts);
    ܐܢ (U_FAILURE(sts)) {
        ܦܢܐ ܕܓܠܐ;
    }
    ܦܢܐ ܫܪܪܐ;
}

منطقي
isSpecialTypeCodepoints(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        ܐܢ (c == '-') {
            ܐܢ (subtagLen < 4 || subtagLen > 6) {
                ܦܢܐ ܕܓܠܐ;
            }
            subtagLen = 0;
        } ܐܚܪܢܐ ܐܢ ((c >= '0' && c <= '9') ||
                    (c >= 'A' && c <= 'F') || // A-F/a-f are contiguous
                    (c >= 'a' && c <= 'f')) { // also in EBCDIC
            subtagLen++;
        } ܐܚܪܢܐ {
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ subtagLen >= 4 && subtagLen <= 6;
}

منطقي
isSpecialTypeReorderCode(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        ܐܢ (c == '-') {
            ܐܢ (subtagLen < 3 || subtagLen > 8) {
                ܦܢܐ ܕܓܠܐ;
            }
            subtagLen = 0;
        } ܐܚܪܢܐ ܐܢ (uprv_isASCIILetter(c)) {
            subtagLen++;
        } ܐܚܪܢܐ {
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ subtagLen >= 3 && subtagLen <= 8;
}

منطقي
isSpecialTypeRgKeyValue(std::string_view val) {
    int32_t subtagLen = 0;
    لكل (حرف c : val) {
        ܐܢ ((subtagLen < 2 && uprv_isASCIILetter(c)) ||
            (subtagLen >= 2 && (c == 'Z' || c == 'z'))) {
            subtagLen++;
        } ܐܚܪܢܐ {
            ܦܢܐ ܕܓܠܐ;
        }
    }
    ܦܢܐ subtagLen == 6;
}

}  // namespace

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpKey(std::string_view key) {
    ܐܢ (!init()) {
        ܦܢܐ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ܐܢ (keyData != nullptr) {
        ܦܢܐ keyData->bcpId;
    }

    ܦܢܐ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyKey(std::string_view key) {
    ܐܢ (!init()) {
        ܦܢܐ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ܐܢ (keyData != nullptr) {
        ܦܢܐ keyData->legacyId;
    }

    ܦܢܐ std::nullopt;
}

U_EXPORT std::optional<std::string_view>
ulocimp_toBcpType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ܐܢ (!init()) {
        ܦܢܐ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ܐܢ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ܐܢ (t != nullptr) {
            ܦܢܐ t->bcpId;
        }
        ܐܢ (keyData->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = ܕܓܠܐ;
            ܐܢ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ܐܢ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ܐܢ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ܐܢ (matched) {
                ܦܢܐ type;
            }
        }
    }

    ܦܢܐ std::nullopt;
}


U_EXPORT std::optional<std::string_view>
ulocimp_toLegacyType(std::string_view key, std::string_view type U_LIFETIME_BOUND) {
    ܐܢ (!init()) {
        ܦܢܐ std::nullopt;
    }

    LocExtKeyData* keyData = static_cast<LocExtKeyData*>(uhash_get(gLocExtKeyMap, &key));
    ܐܢ (keyData != nullptr) {
        LocExtType* t = static_cast<LocExtType*>(uhash_get(keyData->typeMap.getAlias(), &type));
        ܐܢ (t != nullptr) {
            ܦܢܐ t->legacyId;
        }
        ܐܢ (keyData->specialTypes != SPECIALTYPE_NONE) {
            منطقي matched = ܕܓܠܐ;
            ܐܢ (keyData->specialTypes & SPECIALTYPE_CODEPOINTS) {
                matched = isSpecialTypeCodepoints(type);
            }
            ܐܢ (!matched && keyData->specialTypes & SPECIALTYPE_REORDER_CODE) {
                matched = isSpecialTypeReorderCode(type);
            }
            ܐܢ (!matched && keyData->specialTypes & SPECIALTYPE_RG_KEY_VALUE) {
                matched = isSpecialTypeRgKeyValue(type);
            }
            ܐܢ (matched) {
                ܦܢܐ type;
            }
        }
    }

    ܦܢܐ std::nullopt;
}
