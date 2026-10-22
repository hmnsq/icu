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

constexpr حرف PSEUDO_ACCENTS_اگیتر = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_اگیتر = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_اگیتر = ',';  // -XC, -PSCRACK

}  // namespace

Locale_فاصلہ_انکڑے::Locale_فاصلہ_انکڑے(Locale_فاصلہ_انکڑے &&انکڑے) :
        فاصلہ_Trie_بائٹاں(انکڑے.فاصلہ_Trie_بائٹاں),
        region_منزل_Partitions(انکڑے.region_منزل_Partitions),
        partitions(انکڑے.partitions),
        paradigms(انکڑے.paradigms), paradigms_لمائی(انکڑے.paradigms_لمائی),
        distances(انکڑے.distances) {
    انکڑے.partitions = خالی_سوچک;
    انکڑے.paradigms = خالی_سوچک;
}

Locale_فاصلہ_انکڑے::~Locale_فاصلہ_انکڑے() {
    uprv_free(partitions);
    مٹاؤ[] paradigms;
}

بنية Likely_Subtags_انکڑے {
    UResourceBundle *langInfoBundle = خالی_سوچک;
    Unique_Char_لڑیاں لڑیاں;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trie_بائٹاں = خالی_سوچک;
    LSR *lsrs = خالی_سوچک;
    int32_t lsrs_لمائی = 0;

    Locale_فاصلہ_انکڑے فاصلہ_انکڑے;

    Likely_Subtags_انکڑے(UError_کوڈ &error_کوڈ) : لڑیاں(error_کوڈ) {}

    ~Likely_Subtags_انکڑے() {
        ures_close(langInfoBundle);
        مٹاؤ[] lsrs;
    }

    فراغ load(UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        langInfoBundle = ures_open_سدھا(خالی_سوچک, "langInfo", &error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }
        ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
        Resource_انکڑے_قدر value;
        ures_get_قدر_With_Fallback(langInfoBundle, "likely", ڈھیر_عارضی_Bundle.getAlias(),
                                  value, error_کوڈ);
        Resource_جدول likely_جدول = value.get_جدول(error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_حافظہ<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_لمائی = 0, regions_لمائی = 0, lsr_Subtags_لمائی = 0;
        ResourceArray m49Array;
        إذا (likely_جدول.لبھو_قدر("m49", value)) {
            m49Array = value.getArray(error_کوڈ);
        } وإلا {
            error_کوڈ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        إذا (!read_لڑیاں(likely_جدول, "languageAliases", value,
                         languageIndexes, languages_لمائی, error_کوڈ) ||
                !read_لڑیاں(likely_جدول, "regionAliases", value,
                             regionIndexes, regions_لمائی, error_کوڈ) ||
                !read_LSREncoded_لڑیاں(likely_جدول, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_لمائی, error_کوڈ)) {
            أرجع;
        }
        إذا ((languages_لمائی & 1) != 0 ||
                (regions_لمائی & 1) != 0 ||
                (lsr_Subtags_لمائی % 3) != 0) {
            error_کوڈ = U_INVALID_FORMAT_ERROR;
            أرجع;
        }
        إذا (lsr_Subtags_لمائی == 0) {
            error_کوڈ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }

        إذا (!likely_جدول.لبھو_قدر("trie", value)) {
            error_کوڈ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        int32_t length;
        trie_بائٹاں = value.get_ثنائی(length, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_کوڈ میل_Error_کوڈ = U_ZERO_ERROR;
        ures_get_قدر_With_Fallback(langInfoBundle, "match", ڈھیر_عارضی_Bundle.getAlias(),
                                  value, میل_Error_کوڈ);
        Local_حافظہ<int32_t> حصے_کرو_Indexes, paradigmSubtagIndexes;
        int32_t partitions_لمائی = 0, paradigm_Subtags_لمائی = 0;
        إذا (U_SUCCESS(میل_Error_کوڈ)) {
            Resource_جدول میل_جدول = value.get_جدول(error_کوڈ);
            إذا (U_FAILURE(error_کوڈ)) { أرجع; }

            إذا (میل_جدول.لبھو_قدر("trie", value)) {
                فاصلہ_انکڑے.فاصلہ_Trie_بائٹاں = value.get_ثنائی(length, error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) { أرجع; }
            }

            إذا (میل_جدول.لبھو_قدر("regionToPartitions", value)) {
                فاصلہ_انکڑے.region_منزل_Partitions = value.get_ثنائی(length, error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) { أرجع; }
                إذا (length < LSR::REGION_اشاریہ_حد_بندی) {
                    error_کوڈ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }

            إذا (!read_لڑیاں(میل_جدول, "partitions", value,
                             حصے_کرو_Indexes, partitions_لمائی, error_کوڈ) ||
                    !read_LSREncoded_لڑیاں(میل_جدول, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_لمائی, error_کوڈ)) {
                أرجع;
            }
            إذا ((paradigm_Subtags_لمائی % 3) != 0) {
                error_کوڈ = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            إذا (میل_جدول.لبھو_قدر("distances", value)) {
                فاصلہ_انکڑے.distances = value.get_Int_سمتیہ(length, error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) { أرجع; }
                إذا (length < 4) {  // LocaleDistance IX_LIMIT
                    error_کوڈ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }
        } وإلا إذا (میل_Error_کوڈ == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وإلا {  // error other than missing resource
            error_کوڈ = میل_Error_کوڈ;
            أرجع;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        لڑیاں.freeze();

        languageAliases = CharStringMap(languages_لمائی / 2, error_کوڈ);
        لكل (int32_t i = 0; i < languages_لمائی; i += 2) {
            languageAliases.put(لڑیاں.get(languageIndexes[i]),
                                لڑیاں.get(languageIndexes[i + 1]), error_کوڈ);
        }

        regionAliases = CharStringMap(regions_لمائی / 2, error_کوڈ);
        لكل (int32_t i = 0; i < regions_لمائی; i += 2) {
            regionAliases.put(لڑیاں.get(regionIndexes[i]),
                              لڑیاں.get(regionIndexes[i + 1]), error_کوڈ);
        }
        إذا (U_FAILURE(error_کوڈ)) { أرجع; }

        lsrs_لمائی = lsr_Subtags_لمائی / 3;
        lsrs = جديد LSR[lsrs_لمائی];
        إذا (lsrs == خالی_سوچک) {
            error_کوڈ = U_حافظہ_ونڈ_ERROR;
            أرجع;
        }
        لكل (int32_t i = 0, j = 0; i < lsr_Subtags_لمائی; i += 3, ++j) {
            lsrs[j] = LSR(لڑیاں.get(lsrSubtagIndexes[i]),
                          لڑیاں.get(lsrSubtagIndexes[i + 1]),
                          لڑیاں.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        إذا (partitions_لمائی > 0) {
            فاصلہ_انکڑے.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitions_لمائی * حجم(ثابت حرف *)));
            إذا (فاصلہ_انکڑے.partitions == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0; i < partitions_لمائی; ++i) {
                فاصلہ_انکڑے.partitions[i] = لڑیاں.get(حصے_کرو_Indexes[i]);
            }
        }

        إذا (paradigm_Subtags_لمائی > 0) {
            فاصلہ_انکڑے.paradigms_لمائی = paradigm_Subtags_لمائی / 3;
            LSR *paradigms = جديد LSR[فاصلہ_انکڑے.paradigms_لمائی];
            إذا (paradigms == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0, j = 0; i < paradigm_Subtags_لمائی; i += 3, ++j) {
                paradigms[j] = LSR(لڑیاں.get(paradigmSubtagIndexes[i]),
                                   لڑیاں.get(paradigmSubtagIndexes[i + 1]),
                                   لڑیاں.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            فاصلہ_انکڑے.paradigms = paradigms;
        }
    }

خاص:
    منطقي read_لڑیاں(ثابت Resource_جدول &جدول, ثابت حرف *کلید, Resource_قدر &value,
                     Local_حافظہ<int32_t> &indexes, int32_t &length, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
        إذا (جدول.لبھو_قدر(کلید, value)) {
            ResourceArray stringArray = value.getArray(error_کوڈ);
            إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
            length = stringArray.get_ناپ();
            إذا (length == 0) { أرجع سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            إذا (rawIndexes == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                إذا (stringArray.get_قدر(i, value)) {  // returns true because i < length
                    int32_t str_لمائی = 0;
                    rawIndexes[i] = لڑیاں.جوڑو(value.getString(str_لمائی, error_کوڈ), error_کوڈ);
                    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
                }
            }
        }
        أرجع سچ;
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
        ثابت حرف* script = uscript_getShortName(static_cast<UScript_کوڈ>(encoded));
        إذا (script == خالی_سوچک) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        أرجع UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_اشاریہ_منزل_کوڈ(ثابت ResourceArray &m49Array, Resource_قدر &value, صحيح اشاریہ, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (m49Array.get_قدر(اشاریہ, value)) {
            أرجع value.getUnicodeString(error_کوڈ);
        }
        // "m49" does not include the index.
        error_کوڈ = U_MISSING_RESOURCE_ERROR;
        أرجع UNICODE_STRING_SIMPLE("");
    }

    UnicodeString منزل_Region(ثابت ResourceArray& m49Array, Resource_قدر &value, صحيح encoded, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ) || encoded == 0 || encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        إذا (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            أرجع  m49_اشاریہ_منزل_کوڈ(m49Array, value, encoded, error_کوڈ);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        أرجع UnicodeString(region, 2, US_INV);
    }

    منطقي read_LSREncoded_لڑیاں(ثابت Resource_جدول &جدول, ثابت حرف* کلید, Resource_قدر &value, ثابت ResourceArray& m49Array,
                     Local_حافظہ<int32_t> &indexes, int32_t &length, UError_کوڈ &error_کوڈ) {
        إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
        إذا (جدول.لبھو_قدر(کلید, value)) {
            ثابت int32_t* vectors = value.get_Int_سمتیہ(length, error_کوڈ);
            إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
            إذا (length == 0) { أرجع سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            إذا (rawIndexes == خالی_سوچک) {
                error_کوڈ = U_حافظہ_ونڈ_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                rawIndexes[i*3] = لڑیاں.جوڑو_By_قدر(منزل_Language(vectors[i]), error_کوڈ);
                rawIndexes[i*3+1] = لڑیاں.جوڑو_By_قدر(منزل_Script(vectors[i]), error_کوڈ);
                rawIndexes[i*3+2] = لڑیاں.جوڑو_By_قدر(
                    منزل_Region(m49Array, value, vectors[i], error_کوڈ), error_کوڈ);
                إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
            }
            length *= 3;
        }
        أرجع سچ;
    }
};

نطاق {

LikelySubtags *gLikelySubtags = خالی_سوچک;
UVector *gMacroregions = خالی_سوچک;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    مٹاؤ gLikelySubtags;
    gLikelySubtags = خالی_سوچک;
    مٹاؤ gMacroregions;
    gMacroregions = خالی_سوچک;
    gInitOnce.reset();
    أرجع سچ;
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
فراغ processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UError_کوڈ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    int32_t rangeMarkerLocation = regionName.اشاریہ_دا(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,حالت);
    إذا ( rangeMarkerLocation > 0 ) {
        char16_t سرا_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        طالما ( buf[rangeMarkerLocation-1] <= سرا_Range && U_SUCCESS(حالت)) {
            Local_اشارہ_گر<UnicodeString> newRegion(جديد UnicodeString(buf), حالت);
            newMacroRegions->adoptElement(newRegion.orphan(),حالت);
            buf[rangeMarkerLocation-1]++;
        }
    } وإلا {
        Local_اشارہ_گر<UnicodeString> newRegion(جديد UnicodeString(regionName), حالت);
        newMacroRegions->adoptElement(newRegion.orphan(),حالت);
    }
}

#إذا U_DEBUG
UVector* لدو_Macroregions(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالی_سوچک; }
    Local_اشارہ_گر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    Local_UResource_Bundle_اشارہ_گر supplemental_انکڑے(ures_open_سدھا(خالی_سوچک,"supplementalData",&حالت));
    Local_UResource_Bundle_اشارہ_گر idValidity(ures_get_By_کلید(supplemental_انکڑے.getAlias(),"idValidity",خالی_سوچک,&حالت));
    Local_UResource_Bundle_اشارہ_گر regionList(ures_get_By_کلید(idValidity.getAlias(),"region",خالی_سوچک,&حالت));
    Local_UResource_Bundle_اشارہ_گر regionMacro(ures_get_By_کلید(regionList.getAlias(),"macroregion",خالی_سوچک,&حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    طالما (ures_has_اگلا(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_اگلا_Unicode_String(regionMacro.getAlias(),خالی_سوچک,&حالت);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خالی_سوچک;
        }
    }

    أرجع newMacroRegions.orphan();
}
#جے_انت // U_DEBUG

UVector* getStaticMacroregions(UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالی_سوچک; }
    Local_اشارہ_گر<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }

    لكل (ثابت خودکار *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خالی_سوچک;
        }
    }

    أرجع newMacroRegions.orphan();
}

}  // namespace

فراغ U_CALLCONV LikelySubtags::initLikelySubtags(UError_کوڈ &error_کوڈ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == خالی_سوچک);
    Likely_Subtags_انکڑے انکڑے(error_کوڈ);
    انکڑے.load(error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع; }
    gLikelySubtags = جديد LikelySubtags(انکڑے);
    gMacroregions = getStaticMacroregions(error_کوڈ);
#إذا U_DEBUG
    خودکار macroregions_From_انکڑے = لدو_Macroregions(error_کوڈ);
    U_ASSERT((*gMacroregions) == (*macroregions_From_انکڑے));
    مٹاؤ macroregions_From_انکڑے;
#جے_انت
    إذا (U_FAILURE(error_کوڈ) || gLikelySubtags == خالی_سوچک || gMacroregions == خالی_سوچک) {
        مٹاؤ gLikelySubtags;
        مٹاؤ gMacroregions;
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }

    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UError_کوڈ &error_کوڈ) {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خالی_سوچک; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کوڈ);
    أرجع gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_انکڑے &انکڑے) :
        langInfoBundle(انکڑے.langInfoBundle),
        لڑیاں(انکڑے.لڑیاں.orphan_Char_لڑیاں()),
        languageAliases(std::move(انکڑے.languageAliases)),
        regionAliases(std::move(انکڑے.regionAliases)),
        trie(انکڑے.trie_بائٹاں),
        lsrs(انکڑے.lsrs),
#إذا U_DEBUG
        lsrs_لمائی(انکڑے.lsrs_لمائی),
#جے_انت // U_DEBUG
        فاصلہ_انکڑے(std::move(انکڑے.فاصلہ_انکڑے)) {
    انکڑے.langInfoBundle = خالی_سوچک;
    انکڑے.lsrs = خالی_سوچک;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_نتیجہ نتیجہ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_اگلا(نتیجہ));
    trieUndState = trie.getState64();
    نتیجہ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_اگلا(نتیجہ));
    trieUndZzzzState = trie.getState64();
    نتیجہ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_قدر(نتیجہ));
    default_Lsr_اشاریہ = trie.get_قدر();
    trie.reset();

    لكل (char16_t c = u'a'; c <= u'z'; ++c) {
        نتیجہ = trie.next(c);
        إذا (نتیجہ == USTRINGTRIE_NO_قدر) {
            trie_پہلا_Letter_حالتاں[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    مٹاؤ لڑیاں;
    مٹاؤ[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقي return_داخلہ_If_Unmatch,
                                         UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    إذا (locale.isBogus()) {
        error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع {};
    }
    ثابت حرف *name = locale.getName();
    إذا (!return_داخلہ_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        أرجع LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_داخلہ_If_Unmatch, error_کوڈ);

    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        أرجع LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_کوڈ);
    }
    أرجع max;
}

نطاق {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    أرجع canonical == خالی_سوچک ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقي return_داخلہ_If_Unmatch,
                                     UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    إذا (!return_داخلہ_If_Unmatch) {
        حرف c1;
        إذا (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            بدل (c1) {
            حالة 'A':
                أرجع LSR(PSEUDO_ACCENTS_اگیتر, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوڈ);
            حالة 'B':
                أرجع LSR(PSEUDO_BIDI_اگیتر, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوڈ);
            حالة 'C':
                أرجع LSR(PSEUDO_CRACKED_اگیتر, language, script, region,
                           LSR::EXPLICIT_LSR, error_کوڈ);
            افتراضي:  // normal locale
                اكسر;
            }
        }

        إذا (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            إذا (uprv_strcmp(variant, "PSACCENT") == 0) {
                أرجع LSR(PSEUDO_ACCENTS_اگیتر, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_کوڈ);
            } وإلا إذا (uprv_strcmp(variant, "PSBIDI") == 0) {
                أرجع LSR(PSEUDO_BIDI_اگیتر, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_کوڈ);
            } وإلا إذا (uprv_strcmp(variant, "PSCRACK") == 0) {
                أرجع LSR(PSEUDO_CRACKED_اگیتر, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_کوڈ);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    أرجع maximize(language, script, region, return_داخلہ_If_Unmatch, error_کوڈ);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقي return_داخلہ_If_Unmatch,
                             UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    أرجع maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_داخلہ_If_Unmatch,
                    error_کوڈ);
}

منطقي LikelySubtags::isMacroregion(StringPiece& region, UError_کوڈ& error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع خطأ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    أرجع gMacroregions->شامل_اے((فراغ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقي return_داخلہ_If_Unmatch,
                             UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
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
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کوڈ);  // already maximized
    }
    منطقي retainLanguage = خطأ;
    منطقي retainScript = خطأ;
    منطقي retainRegion = خطأ;

    بائٹاں_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_ہیٹھلا_Ordinal(language.انکڑے()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_پہلا_Letter_حالتاں[c0]) != 0) {
        value = trie_اگلا(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_اگلا(iter, language, 0);
    }
    منطقي میل_Language = (value >= 0);
    منطقي میل_Script = خطأ;
    إذا (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وإلا {
        retainLanguage = سچ;
        iter.reset_منزل_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value >= 0 && !script.empty()) {
        میل_Script = سچ;
    }
    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وإلا {
        value = trie_اگلا(iter, script, 0);
        إذا (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وإلا {
            retainScript = سچ;
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقي میل_Region = خطأ;
    إذا (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وإلا {
        value = trie_اگلا(iter, region, 0);
        إذا (value >= 0) {
            إذا (!region.empty() && !isMacroregion(region, error_کوڈ)) {
                retainRegion = سچ;
                میل_Region = سچ;
            }
        } وإلا {
            retainRegion = سچ;
            إذا (state == 0) {
                value = default_Lsr_اشاریہ;
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                إذا (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_منزل_State64(trieUndState);  // "und" ("*")
                    value = trie_اگلا(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_خالی_تھاواں_State = iter.getState64();
                    value = trie_اگلا(iter, region, 0);
                    // Fallback to und =>
                    إذا (value < 0) {
                        iter.reset_منزل_State64(trie_Und_خالی_تھاواں_State);
                        value = trie_اگلا(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_لمائی);
    إذا (return_داخلہ_If_Unmatch &&
        (!(میل_Language || میل_Script || (میل_Region && language.empty())))) {
      أرجع LSR("", "", "", LSR::EXPLICIT_LSR, error_کوڈ);  // no matching.
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
    int32_t retain_نقاب = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    أرجع LSR(language, script, region, retain_نقاب, error_کوڈ);
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
        int32_t اشاریہ;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            اشاریہ = likelyInfo >> 2;
        } وإلا {
            اشاریہ = get_Likely_اشاریہ(lsr.language, "");
            likelyInfo = اشاریہ << 2;
        }
        ثابت LSR &likely = lsrs[اشاریہ];
        إذا (uprv_strcmp(lsr.script, likely.script) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    إذا (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t اشاریہ;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            اشاریہ = likelyInfo >> 2;
        } وإلا {
            اشاریہ = get_Likely_اشاریہ(lsr.language, lsr.region);
            likelyInfo = (اشاریہ << 2) | 2;
        }
        ثابت LSR &likely = lsrs[اشاریہ];
        إذا (uprv_strcmp(lsr.region, likely.region) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    أرجع likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_اشاریہ(ثابت حرف *language, ثابت حرف *script) ثابت {
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    إذا (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    بائٹاں_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_ہیٹھلا_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_پہلا_Letter_حالتاں[c0]) != 0) {
        value = trie_اگلا(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_اگلا(iter, language, 0);
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
        value = trie_اگلا(iter, script, 0);
        إذا (value >= 0) {
            state = iter.getState64();
        } وإلا {
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    إذا (value > 0) {
        // Final value from just language or language+script.
    } وإلا {
        value = trie_اگلا(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_لمائی);
    أرجع value;
}

int32_t LikelySubtags::trie_اگلا(بائٹاں_Trie &iter, ثابت حرف *s, int32_t i) {
    UString_Trie_نتیجہ نتیجہ;
    uint8_t c;
    إذا ((c = s[i]) == 0) {
        نتیجہ = iter.next(u'*');
    } وإلا {
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            إذا (next != 0) {
                إذا (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                    أرجع -1;
                }
            } وإلا {
                // last character of this subtag
                نتیجہ = iter.next(c | 0x80);
                اكسر;
            }
            c = next;
        }
    }
    بدل (نتیجہ) {
    حالة USTRINGTRIE_NO_میل: أرجع -1;
    حالة USTRINGTRIE_NO_قدر: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_قدر:
        U_ASSERT(iter.get_قدر() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_قدر: أرجع iter.get_قدر();
    افتراضي: أرجع -1;
    }
}
int32_t LikelySubtags::trie_اگلا(بائٹاں_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_نتیجہ نتیجہ;
    uint8_t c;
    إذا (s.length() == i) {
        نتیجہ = iter.next(u'*');
    } وإلا {
        c = s.انکڑے()[i];
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            إذا (i+1 != s.length()) {
                إذا (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                    أرجع -1;
                }
                c = s.انکڑے()[++i];
            } وإلا {
                // last character of this subtag
                نتیجہ = iter.next(c | 0x80);
                اكسر;
            }
        }
    }
    بدل (نتیجہ) {
    حالة USTRINGTRIE_NO_میل: أرجع -1;
    حالة USTRINGTRIE_NO_قدر: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_قدر:
        U_ASSERT(iter.get_قدر() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_قدر: أرجع iter.get_قدر();
    افتراضي: أرجع -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقي favorScript,
                                    UError_کوڈ &error_کوڈ) ثابت {
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    LSR max = maximize(language, script, region, سچ, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    // If no match, return it.
    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_کوڈ);
    }
    // try language
    LSR test = maximize(max.language, "", "", سچ, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_کوڈ);
    }

    إذا (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کوڈ);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", سچ, error_کوڈ);
    إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_کوڈ);
    }
    إذا (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_کوڈ);
        إذا (U_FAILURE(error_کوڈ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_کوڈ);
        }
    }
    أرجع LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_کوڈ);
}

U_NAMESPACE_سرا
