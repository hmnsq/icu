// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ضمن <utility>
#ضمن "unicode/utypes.h"
#ضمن "unicode/bytestrie.h"
#ضمن "unicode/localpointer.h"
#ضمن "unicode/locid.h"
#ضمن "unicode/uobject.h"
#ضمن "unicode/ures.h"
#ضمن "unicode/uscript.h"
#ضمن "charstr.h"
#ضمن "cstring.h"
#ضمن "loclikelysubtags.h"
#ضمن "lsr.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"
#ضمن "uhash.h"
#ضمن "uinvchar.h"
#ضمن "umutex.h"
#ضمن "uniquecharstr.h"
#ضمن "uresdata.h"
#ضمن "uresimp.h"
#ضمن "uvector.h"

U_NAMESPACE_BEGIN

نطاق {

constexpr حرف PSEUDO_ACCENTS_مختاړی = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_مختاړی = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_مختاړی = ',';  // -XC, -PSCRACK

}  // namespace

Locale_واټن_معلومات::Locale_واټن_معلومات(Locale_واټن_معلومات &&معلومات) :
        واټن_Trie_بایټونه(معلومات.واټن_Trie_بایټونه),
        region_منزل_Partitions(معلومات.region_منزل_Partitions),
        partitions(معلومات.partitions),
        paradigms(معلومات.paradigms), paradigms_اوږدتیا(معلومات.paradigms_اوږدتیا),
        distances(معلومات.distances) {
    معلومات.partitions = تش_اشاره;
    معلومات.paradigms = تش_اشاره;
}

Locale_واټن_معلومات::~Locale_واټن_معلومات() {
    uprv_free(partitions);
    ړنګول[] paradigms;
}

بنية Likely_Subtags_معلومات {
    UResourceBundle *langInfoBundle = تش_اشاره;
    Unique_Char_مزي_ګانې مزي_ګانې;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trie_بایټونه = تش_اشاره;
    LSR *lsrs = تش_اشاره;
    int32_t lsrs_اوږدتیا = 0;

    Locale_واټن_معلومات واټن_معلومات;

    Likely_Subtags_معلومات(UError_کوډ &error_کوډ) : مزي_ګانې(error_کوډ) {}

    ~Likely_Subtags_معلومات() {
        ures_close(langInfoBundle);
        ړنګول[] lsrs;
    }

    فراغ load(UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        langInfoBundle = ures_open_مستقیم(تش_اشاره, "langInfo", &error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }
        ډېری_UResource_Bundle ډېری_لنډمهاله_Bundle;
        Resource_معلومات_ارزښت value;
        ures_get_ارزښت_With_Fallback(langInfoBundle, "likely", ډېری_لنډمهاله_Bundle.getAlias(),
                                  value, error_کوډ);
        Resource_جدول likely_جدول = value.get_جدول(error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_حافظه<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_اوږدتیا = 0, regions_اوږدتیا = 0, lsr_Subtags_اوږدتیا = 0;
        ResourceArray m49Array;
        إذا (likely_جدول.پیدا_کړه_ارزښت("m49", value)) {
            m49Array = value.getArray(error_کوډ);
        } وإلا {
            error_کوډ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        إذا (!read_مزي_ګانې(likely_جدول, "languageAliases", value,
                         languageIndexes, languages_اوږدتیا, error_کوډ) ||
                !read_مزي_ګانې(likely_جدول, "regionAliases", value,
                             regionIndexes, regions_اوږدتیا, error_کوډ) ||
                !read_LSREncoded_مزي_ګانې(likely_جدول, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_اوږدتیا, error_کوډ)) {
            أرجع;
        }
        إذا ((languages_اوږدتیا & 1) != 0 ||
                (regions_اوږدتیا & 1) != 0 ||
                (lsr_Subtags_اوږدتیا % 3) != 0) {
            error_کوډ = U_INVALID_FORMAT_ERROR;
            أرجع;
        }
        إذا (lsr_Subtags_اوږدتیا == 0) {
            error_کوډ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }

        إذا (!likely_جدول.پیدا_کړه_ارزښت("trie", value)) {
            error_کوډ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        int32_t length;
        trie_بایټونه = value.get_دوګونی(length, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_کوډ سمون_Error_کوډ = U_ZERO_ERROR;
        ures_get_ارزښت_With_Fallback(langInfoBundle, "match", ډېری_لنډمهاله_Bundle.getAlias(),
                                  value, سمون_Error_کوډ);
        Local_حافظه<int32_t> برخه_کول_Indexes, paradigmSubtagIndexes;
        int32_t partitions_اوږدتیا = 0, paradigm_Subtags_اوږدتیا = 0;
        إذا (U_SUCCESS(سمون_Error_کوډ)) {
            Resource_جدول سمون_جدول = value.get_جدول(error_کوډ);
            إذا (U_FAILURE(error_کوډ)) { أرجع; }

            إذا (سمون_جدول.پیدا_کړه_ارزښت("trie", value)) {
                واټن_معلومات.واټن_Trie_بایټونه = value.get_دوګونی(length, error_کوډ);
                إذا (U_FAILURE(error_کوډ)) { أرجع; }
            }

            إذا (سمون_جدول.پیدا_کړه_ارزښت("regionToPartitions", value)) {
                واټن_معلومات.region_منزل_Partitions = value.get_دوګونی(length, error_کوډ);
                إذا (U_FAILURE(error_کوډ)) { أرجع; }
                إذا (length < LSR::REGION_شاخص_حد) {
                    error_کوډ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }

            إذا (!read_مزي_ګانې(سمون_جدول, "partitions", value,
                             برخه_کول_Indexes, partitions_اوږدتیا, error_کوډ) ||
                    !read_LSREncoded_مزي_ګانې(سمون_جدول, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_اوږدتیا, error_کوډ)) {
                أرجع;
            }
            إذا ((paradigm_Subtags_اوږدتیا % 3) != 0) {
                error_کوډ = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            إذا (سمون_جدول.پیدا_کړه_ارزښت("distances", value)) {
                واټن_معلومات.distances = value.get_Int_ویکټور(length, error_کوډ);
                إذا (U_FAILURE(error_کوډ)) { أرجع; }
                إذا (length < 4) {  // LocaleDistance IX_LIMIT
                    error_کوډ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }
        } وإلا إذا (سمون_Error_کوډ == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وإلا {  // error other than missing resource
            error_کوډ = سمون_Error_کوډ;
            أرجع;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        مزي_ګانې.freeze();

        languageAliases = CharStringMap(languages_اوږدتیا / 2, error_کوډ);
        لكل (int32_t i = 0; i < languages_اوږدتیا; i += 2) {
            languageAliases.put(مزي_ګانې.get(languageIndexes[i]),
                                مزي_ګانې.get(languageIndexes[i + 1]), error_کوډ);
        }

        regionAliases = CharStringMap(regions_اوږدتیا / 2, error_کوډ);
        لكل (int32_t i = 0; i < regions_اوږدتیا; i += 2) {
            regionAliases.put(مزي_ګانې.get(regionIndexes[i]),
                              مزي_ګانې.get(regionIndexes[i + 1]), error_کوډ);
        }
        إذا (U_FAILURE(error_کوډ)) { أرجع; }

        lsrs_اوږدتیا = lsr_Subtags_اوږدتیا / 3;
        lsrs = جديد LSR[lsrs_اوږدتیا];
        إذا (lsrs == تش_اشاره) {
            error_کوډ = U_حافظه_ویش_ERROR;
            أرجع;
        }
        لكل (int32_t i = 0, j = 0; i < lsr_Subtags_اوږدتیا; i += 3, ++j) {
            lsrs[j] = LSR(مزي_ګانې.get(lsrSubtagIndexes[i]),
                          مزي_ګانې.get(lsrSubtagIndexes[i + 1]),
                          مزي_ګانې.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        إذا (partitions_اوږدتیا > 0) {
            واټن_معلومات.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitions_اوږدتیا * حجم(ثابت حرف *)));
            إذا (واټن_معلومات.partitions == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0; i < partitions_اوږدتیا; ++i) {
                واټن_معلومات.partitions[i] = مزي_ګانې.get(برخه_کول_Indexes[i]);
            }
        }

        إذا (paradigm_Subtags_اوږدتیا > 0) {
            واټن_معلومات.paradigms_اوږدتیا = paradigm_Subtags_اوږدتیا / 3;
            LSR *paradigms = جديد LSR[واټن_معلومات.paradigms_اوږدتیا];
            إذا (paradigms == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0, j = 0; i < paradigm_Subtags_اوږدتیا; i += 3, ++j) {
                paradigms[j] = LSR(مزي_ګانې.get(paradigmSubtagIndexes[i]),
                                   مزي_ګانې.get(paradigmSubtagIndexes[i + 1]),
                                   مزي_ګانې.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            واټن_معلومات.paradigms = paradigms;
        }
    }

خاص:
    منطقي read_مزي_ګانې(ثابت Resource_جدول &جدول, ثابت حرف *کلی, Resource_ارزښت &value,
                     Local_حافظه<int32_t> &indexes, int32_t &length, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
        إذا (جدول.پیدا_کړه_ارزښت(کلی, value)) {
            ResourceArray stringArray = value.getArray(error_کوډ);
            إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
            length = stringArray.get_اندازه();
            إذا (length == 0) { أرجع ریښتیا; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            إذا (rawIndexes == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                إذا (stringArray.get_ارزښت(i, value)) {  // returns true because i < length
                    int32_t str_اوږدتیا = 0;
                    rawIndexes[i] = مزي_ګانې.جمع_کړه(value.getString(str_اوږدتیا, error_کوډ), error_کوډ);
                    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
                }
            }
        }
        أرجع ریښتیا;
    }
    UnicodeString منزل_Language(صحيح encoded) {
        إذا (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        حرف lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        إذا (encoded / (27 * 27) == 0) {
            أرجع UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        أرجع UnicodeString(lang, 3, US_INV);
    }
    UnicodeString منزل_Script(صحيح encoded) {
        إذا (encoded == 0) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ثابت حرف* script = uscript_getShortName(static_cast<UScript_کوډ>(encoded));
        إذا (script == تش_اشاره) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        أرجع UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_شاخص_منزل_کوډ(ثابت ResourceArray &m49Array, Resource_ارزښت &value, صحيح شاخص, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (m49Array.get_ارزښت(شاخص, value)) {
            أرجع value.getUnicodeString(error_کوډ);
        }
        // "m49" does not include the index.
        error_کوډ = U_MISSING_RESOURCE_ERROR;
        أرجع UNICODE_STRING_SIMPLE("");
    }

    UnicodeString منزل_Region(ثابت ResourceArray& m49Array, Resource_ارزښت &value, صحيح encoded, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ) || encoded == 0 || encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        إذا (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            أرجع  m49_شاخص_منزل_کوډ(m49Array, value, encoded, error_کوډ);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        أرجع UnicodeString(region, 2, US_INV);
    }

    منطقي read_LSREncoded_مزي_ګانې(ثابت Resource_جدول &جدول, ثابت حرف* کلی, Resource_ارزښت &value, ثابت ResourceArray& m49Array,
                     Local_حافظه<int32_t> &indexes, int32_t &length, UError_کوډ &error_کوډ) {
        إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
        إذا (جدول.پیدا_کړه_ارزښت(کلی, value)) {
            ثابت int32_t* vectors = value.get_Int_ویکټور(length, error_کوډ);
            إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
            إذا (length == 0) { أرجع ریښتیا; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            إذا (rawIndexes == تش_اشاره) {
                error_کوډ = U_حافظه_ویش_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                rawIndexes[i*3] = مزي_ګانې.جمع_کړه_By_ارزښت(منزل_Language(vectors[i]), error_کوډ);
                rawIndexes[i*3+1] = مزي_ګانې.جمع_کړه_By_ارزښت(منزل_Script(vectors[i]), error_کوډ);
                rawIndexes[i*3+2] = مزي_ګانې.جمع_کړه_By_ارزښت(
                    منزل_Region(m49Array, value, vectors[i], error_کوډ), error_کوډ);
                إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
            }
            length *= 3;
        }
        أرجع ریښتیا;
    }
};

نطاق {

LikelySubtags *gLikelySubtags = تش_اشاره;
UVector *gMacroregions = تش_اشاره;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ړنګول gLikelySubtags;
    gLikelySubtags = تش_اشاره;
    ړنګول gMacroregions;
    gMacroregions = تش_اشاره;
    gInitOnce.reset();
    أرجع ریښتیا;
}

constexpr ثابت char16_t* MACROREGION_HARDCODE[] = {
    u"001~3",
    u"005",
    u"009",
    u"011",
    u"013~5",
    u"017~9",
    u"021",
    u"029",
    u"030",
    u"034~5",
    u"039",
    u"053~4",
    u"057",
    u"061",
    u"142~3",
    u"145",
    u"150~1",
    u"154~5",
    u"202",
    u"419",
    u"EU",
    u"EZ",
    u"QO",
    u"UN",
};

constexpr char16_t RANGE_MARKER = 0x7E; /* '~' */
فراغ processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UError_کوډ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    int32_t rangeMarkerLocation = regionName.شاخص_د(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,حالت);
    إذا ( rangeMarkerLocation > 0 ) {
        char16_t پای_ټکی_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        طالما ( buf[rangeMarkerLocation-1] <= پای_ټکی_Range && U_SUCCESS(حالت)) {
            Local_اشاره_ګر<UnicodeString> newRegion(جديد UnicodeString(buf), حالت);
            newMacroRegions->adoptElement(newRegion.orphan(),حالت);
            buf[rangeMarkerLocation-1]++;
        }
    } وإلا {
        Local_اشاره_ګر<UnicodeString> newRegion(جديد UnicodeString(regionName), حالت);
        newMacroRegions->adoptElement(newRegion.orphan(),حالت);
    }
}

#إذا U_DEBUG
UVector* بارول_Macroregions(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع تش_اشاره; }
    Local_اشاره_ګر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    Local_UResource_Bundle_اشاره_ګر supplemental_معلومات(ures_open_مستقیم(تش_اشاره,"supplementalData",&حالت));
    Local_UResource_Bundle_اشاره_ګر idValidity(ures_get_By_کلی(supplemental_معلومات.getAlias(),"idValidity",تش_اشاره,&حالت));
    Local_UResource_Bundle_اشاره_ګر regionList(ures_get_By_کلی(idValidity.getAlias(),"region",تش_اشاره,&حالت));
    Local_UResource_Bundle_اشاره_ګر regionMacro(ures_get_By_کلی(regionList.getAlias(),"macroregion",تش_اشاره,&حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    طالما (ures_has_بل(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_بل_Unicode_String(regionMacro.getAlias(),تش_اشاره,&حالت);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع تش_اشاره;
        }
    }

    أرجع newMacroRegions.orphan();
}
#د_که_پای // U_DEBUG

UVector* getStaticMacroregions(UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع تش_اشاره; }
    Local_اشاره_ګر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }

    لكل (ثابت خپلکاری *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع تش_اشاره;
        }
    }

    أرجع newMacroRegions.orphan();
}

}  // namespace

فراغ U_CALLCONV LikelySubtags::initLikelySubtags(UError_کوډ &error_کوډ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == تش_اشاره);
    Likely_Subtags_معلومات معلومات(error_کوډ);
    معلومات.load(error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع; }
    gLikelySubtags = جديد LikelySubtags(معلومات);
    gMacroregions = getStaticMacroregions(error_کوډ);
#إذا U_DEBUG
    خپلکاری macroregions_From_معلومات = بارول_Macroregions(error_کوډ);
    U_ASSERT((*gMacroregions) == (*macroregions_From_معلومات));
    ړنګول macroregions_From_معلومات;
#د_که_پای
    إذا (U_FAILURE(error_کوډ) || gLikelySubtags == تش_اشاره || gMacroregions == تش_اشاره) {
        ړنګول gLikelySubtags;
        ړنګول gMacroregions;
        error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }

    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UError_کوډ &error_کوډ) {
    إذا (U_FAILURE(error_کوډ)) { أرجع تش_اشاره; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کوډ);
    أرجع gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_معلومات &معلومات) :
        langInfoBundle(معلومات.langInfoBundle),
        مزي_ګانې(معلومات.مزي_ګانې.orphan_Char_مزي_ګانې()),
        languageAliases(std::move(معلومات.languageAliases)),
        regionAliases(std::move(معلومات.regionAliases)),
        trie(معلومات.trie_بایټونه),
        lsrs(معلومات.lsrs),
#إذا U_DEBUG
        lsrs_اوږدتیا(معلومات.lsrs_اوږدتیا),
#د_که_پای // U_DEBUG
        واټن_معلومات(std::move(معلومات.واټن_معلومات)) {
    معلومات.langInfoBundle = تش_اشاره;
    معلومات.lsrs = تش_اشاره;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_پایله پایله = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_بل(پایله));
    trieUndState = trie.getState64();
    پایله = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_بل(پایله));
    trieUndZzzzState = trie.getState64();
    پایله = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ارزښت(پایله));
    default_Lsr_شاخص = trie.get_ارزښت();
    trie.reset();

    لكل (char16_t c = u'a'; c <= u'z'; ++c) {
        پایله = trie.next(c);
        إذا (پایله == USTRINGTRIE_NO_ارزښت) {
            trie_لومړی_Letter_حالتونه[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    ړنګول مزي_ګانې;
    ړنګول[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقي return_ننوت_If_Unmatch,
                                         UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    إذا (locale.isBogus()) {
        error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع {};
    }
    ثابت حرف *name = locale.getName();
    إذا (!return_ننوت_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        أرجع LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ننوت_If_Unmatch, error_کوډ);

    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        أرجع LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_کوډ);
    }
    أرجع max;
}

نطاق {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    أرجع canonical == تش_اشاره ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقي return_ننوت_If_Unmatch,
                                     UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    إذا (!return_ننوت_If_Unmatch) {
        حرف c1;
        إذا (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            بدل (c1) {
            حالة 'A':
                أرجع LSR(PSEUDO_ACCENTS_مختاړی, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوډ);
            حالة 'B':
                أرجع LSR(PSEUDO_BIDI_مختاړی, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوډ);
            حالة 'C':
                أرجع LSR(PSEUDO_CRACKED_مختاړی, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوډ);
            افتراضي:  // normal locale
                اكسر;
            }
        }

        إذا (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            إذا (uprv_strcmp(variant, "PSACCENT") == 0) {
                أرجع LSR(PSEUDO_ACCENTS_مختاړی, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_کوډ);
            } وإلا إذا (uprv_strcmp(variant, "PSBIDI") == 0) {
                أرجع LSR(PSEUDO_BIDI_مختاړی, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_کوډ);
            } وإلا إذا (uprv_strcmp(variant, "PSCRACK") == 0) {
                أرجع LSR(PSEUDO_CRACKED_مختاړی, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_کوډ);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    أرجع maximize(language, script, region, return_ننوت_If_Unmatch, error_کوډ);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقي return_ننوت_If_Unmatch,
                             UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    أرجع maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ننوت_If_Unmatch,
                    error_کوډ);
}

منطقي LikelySubtags::isMacroregion(StringPiece& region, UError_کوډ& error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع خطأ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    أرجع gMacroregions->لري((فراغ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقي return_ننوت_If_Unmatch,
                             UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    إذا (language.compare("und") == 0) {
        language = "";
    }
    إذا (script.compare("Zzzz") == 0) {
        script = "";
    }
    إذا (region.compare("ZZ") == 0) {
        region = "";
    }
    إذا (!script.empty() && !region.empty() && !language.empty()) {
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کوډ);  // already maximized
    }
    منطقي retainLanguage = خطأ;
    منطقي retainScript = خطأ;
    منطقي retainRegion = خطأ;

    بایټونه_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_ښکتنی_Ordinal(language.معلومات()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_لومړی_Letter_حالتونه[c0]) != 0) {
        value = trie_بل(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_بل(iter, language, 0);
    }
    منطقي سمون_Language = (value >= 0);
    منطقي سمون_Script = خطأ;
    إذا (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وإلا {
        retainLanguage = ریښتیا;
        iter.reset_منزل_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value >= 0 && !script.empty()) {
        سمون_Script = ریښتیا;
    }
    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وإلا {
        value = trie_بل(iter, script, 0);
        إذا (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وإلا {
            retainScript = ریښتیا;
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_بل(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقي سمون_Region = خطأ;
    إذا (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وإلا {
        value = trie_بل(iter, region, 0);
        إذا (value >= 0) {
            إذا (!region.empty() && !isMacroregion(region, error_کوډ)) {
                retainRegion = ریښتیا;
                سمون_Region = ریښتیا;
            }
        } وإلا {
            retainRegion = ریښتیا;
            إذا (state == 0) {
                value = default_Lsr_شاخص;
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_بل(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                إذا (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_منزل_State64(trieUndState);  // "und" ("*")
                    value = trie_بل(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_تشه_State = iter.getState64();
                    value = trie_بل(iter, region, 0);
                    // Fallback to und =>
                    إذا (value < 0) {
                        iter.reset_منزل_State64(trie_Und_تشه_State);
                        value = trie_بل(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_اوږدتیا);
    إذا (return_ننوت_If_Unmatch &&
        (!(سمون_Language || سمون_Script || (سمون_Region && language.empty())))) {
      أرجع LSR("", "", "", LSR::EXPLICIT_LSR, error_کوډ);  // no matching.
    }
    إذا (language.empty()) {
        language = StringPiece("und");
    }

    إذا (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ثابت LSR &matched = lsrs[value];
        أرجع LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    إذا (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    إذا (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    إذا (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_ماسک = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    أرجع LSR(language, script, region, retain_ماسک, error_کوډ);
}

int32_t LikelySubtags::compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    إذا (uprv_strcmp(lsr.language, other.language) != 0) {
        أرجع 0xfffffffc;  // negative, lsr not better than other
    }
    إذا (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t شاخص;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            شاخص = likelyInfo >> 2;
        } وإلا {
            شاخص = get_Likely_شاخص(lsr.language, "");
            likelyInfo = شاخص << 2;
        }
        ثابت LSR &likely = lsrs[شاخص];
        إذا (uprv_strcmp(lsr.script, likely.script) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    إذا (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t شاخص;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            شاخص = likelyInfo >> 2;
        } وإلا {
            شاخص = get_Likely_شاخص(lsr.language, lsr.region);
            likelyInfo = (شاخص << 2) | 2;
        }
        ثابت LSR &likely = lsrs[شاخص];
        إذا (uprv_strcmp(lsr.region, likely.region) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    أرجع likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_شاخص(ثابت حرف *language, ثابت حرف *script) ثابت {
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    إذا (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    بایټونه_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_ښکتنی_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_لومړی_Letter_حالتونه[c0]) != 0) {
        value = trie_بل(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_بل(iter, language, 0);
    }
    إذا (value >= 0) {
        state = iter.getState64();
    } وإلا {
        iter.reset_منزل_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
    } وإلا {
        value = trie_بل(iter, script, 0);
        إذا (value >= 0) {
            state = iter.getState64();
        } وإلا {
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_بل(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    إذا (value > 0) {
        // Final value from just language or language+script.
    } وإلا {
        value = trie_بل(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_اوږدتیا);
    أرجع value;
}

int32_t LikelySubtags::trie_بل(بایټونه_Trie &iter, ثابت حرف *s, int32_t i) {
    UString_Trie_پایله پایله;
    uint8_t c;
    إذا ((c = s[i]) == 0) {
        پایله = iter.next(u'*');
    } وإلا {
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            إذا (next != 0) {
                إذا (!USTRINGTRIE_HAS_بل(iter.next(c))) {
                    أرجع -1;
                }
            } وإلا {
                // last character of this subtag
                پایله = iter.next(c | 0x80);
                اكسر;
            }
            c = next;
        }
    }
    بدل (پایله) {
    حالة USTRINGTRIE_NO_سمون: أرجع -1;
    حالة USTRINGTRIE_NO_ارزښت: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_ارزښت:
        U_ASSERT(iter.get_ارزښت() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_ارزښت: أرجع iter.get_ارزښت();
    افتراضي: أرجع -1;
    }
}
int32_t LikelySubtags::trie_بل(بایټونه_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_پایله پایله;
    uint8_t c;
    إذا (s.length() == i) {
        پایله = iter.next(u'*');
    } وإلا {
        c = s.معلومات()[i];
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            إذا (i+1 != s.length()) {
                إذا (!USTRINGTRIE_HAS_بل(iter.next(c))) {
                    أرجع -1;
                }
                c = s.معلومات()[++i];
            } وإلا {
                // last character of this subtag
                پایله = iter.next(c | 0x80);
                اكسر;
            }
        }
    }
    بدل (پایله) {
    حالة USTRINGTRIE_NO_سمون: أرجع -1;
    حالة USTRINGTRIE_NO_ارزښت: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_ارزښت:
        U_ASSERT(iter.get_ارزښت() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_ارزښت: أرجع iter.get_ارزښت();
    افتراضي: أرجع -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقي favorScript,
                                    UError_کوډ &error_کوډ) ثابت {
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    LSR max = maximize(language, script, region, ریښتیا, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    // If no match, return it.
    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کوډ);
    }
    // try language
    LSR test = maximize(max.language, "", "", ریښتیا, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_کوډ);
    }

    إذا (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, ریښتیا, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کوډ);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", ریښتیا, error_کوډ);
    إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_کوډ);
    }
    إذا (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, ریښتیا, error_کوډ);
        إذا (U_FAILURE(error_کوډ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کوډ);
        }
    }
    أرجع LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_کوډ);
}

U_NAMESPACE_پای_ټکی
