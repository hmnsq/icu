// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#شامل <utility>
#شامل "unicode/utypes.h"
#شامل "unicode/bytestrie.h"
#شامل "unicode/localpointer.h"
#شامل "unicode/locid.h"
#شامل "unicode/uobject.h"
#شامل "unicode/ures.h"
#شامل "unicode/uscript.h"
#شامل "charstr.h"
#شامل "cstring.h"
#شامل "loclikelysubtags.h"
#شامل "lsr.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"
#شامل "uhash.h"
#شامل "uinvchar.h"
#شامل "umutex.h"
#شامل "uniquecharstr.h"
#شامل "uresdata.h"
#شامل "uresimp.h"
#شامل "uvector.h"

U_NAMESPACE_BEGIN

نامخلا {

constexpr حرف PSEUDO_ACCENTS_سابقہ = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_سابقہ = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_سابقہ = ',';  // -XC, -PSCRACK

}  // namespace

Locale_فاصلہ_ڈیٹا::Locale_فاصلہ_ڈیٹا(Locale_فاصلہ_ڈیٹا &&ڈیٹا) :
        فاصلہ_Trie_بائٹیں(ڈیٹا.فاصلہ_Trie_بائٹیں),
        region_مقصد_Partitions(ڈیٹا.region_مقصد_Partitions),
        partitions(ڈیٹا.partitions),
        paradigms(ڈیٹا.paradigms), paradigms_لمبائی(ڈیٹا.paradigms_لمبائی),
        distances(ڈیٹا.distances) {
    ڈیٹا.partitions = خالی_اشاریہ;
    ڈیٹا.paradigms = خالی_اشاریہ;
}

Locale_فاصلہ_ڈیٹا::~Locale_فاصلہ_ڈیٹا() {
    uprv_free(partitions);
    حذف[] paradigms;
}

ساخت Likely_Subtags_ڈیٹا {
    UResourceBundle *langInfoBundle = خالی_اشاریہ;
    Unique_Char_لڑیاں لڑیاں;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trie_بائٹیں = خالی_اشاریہ;
    LSR *lsrs = خالی_اشاریہ;
    int32_t lsrs_لمبائی = 0;

    Locale_فاصلہ_ڈیٹا فاصلہ_ڈیٹا;

    Likely_Subtags_ڈیٹا(UError_رمز &error_رمز) : لڑیاں(error_رمز) {}

    ~Likely_Subtags_ڈیٹا() {
        ures_close(langInfoBundle);
        حذف[] lsrs;
    }

    خالی load(UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس; }
        langInfoBundle = ures_open_براہ_راست(خالی_اشاریہ, "langInfo", &error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }
        ڈھیر_UResource_Bundle ڈھیر_عارضی_Bundle;
        Resource_ڈیٹا_قدر value;
        ures_get_قدر_With_Fallback(langInfoBundle, "likely", ڈھیر_عارضی_Bundle.getAlias(),
                                  value, error_رمز);
        Resource_جدول likely_جدول = value.get_جدول(error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_حافظہ<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_لمبائی = 0, regions_لمبائی = 0, lsr_Subtags_لمبائی = 0;
        ResourceArray m49Array;
        اگر (likely_جدول.ڈھونڈو_قدر("m49", value)) {
            m49Array = value.getArray(error_رمز);
        } ورنہ {
            error_رمز = U_MISSING_RESOURCE_ERROR;
            واپس;
        }
        اگر (!read_لڑیاں(likely_جدول, "languageAliases", value,
                         languageIndexes, languages_لمبائی, error_رمز) ||
                !read_لڑیاں(likely_جدول, "regionAliases", value,
                             regionIndexes, regions_لمبائی, error_رمز) ||
                !read_LSREncoded_لڑیاں(likely_جدول, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_لمبائی, error_رمز)) {
            واپس;
        }
        اگر ((languages_لمبائی & 1) != 0 ||
                (regions_لمبائی & 1) != 0 ||
                (lsr_Subtags_لمبائی % 3) != 0) {
            error_رمز = U_INVALID_FORMAT_ERROR;
            واپس;
        }
        اگر (lsr_Subtags_لمبائی == 0) {
            error_رمز = U_MISSING_RESOURCE_ERROR;
            واپس;
        }

        اگر (!likely_جدول.ڈھونڈو_قدر("trie", value)) {
            error_رمز = U_MISSING_RESOURCE_ERROR;
            واپس;
        }
        int32_t length;
        trie_بائٹیں = value.get_ثنائی(length, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_رمز مطابقت_Error_رمز = U_ZERO_ERROR;
        ures_get_قدر_With_Fallback(langInfoBundle, "match", ڈھیر_عارضی_Bundle.getAlias(),
                                  value, مطابقت_Error_رمز);
        Local_حافظہ<int32_t> تقسیم_Indexes, paradigmSubtagIndexes;
        int32_t partitions_لمبائی = 0, paradigm_Subtags_لمبائی = 0;
        اگر (U_SUCCESS(مطابقت_Error_رمز)) {
            Resource_جدول مطابقت_جدول = value.get_جدول(error_رمز);
            اگر (U_FAILURE(error_رمز)) { واپس; }

            اگر (مطابقت_جدول.ڈھونڈو_قدر("trie", value)) {
                فاصلہ_ڈیٹا.فاصلہ_Trie_بائٹیں = value.get_ثنائی(length, error_رمز);
                اگر (U_FAILURE(error_رمز)) { واپس; }
            }

            اگر (مطابقت_جدول.ڈھونڈو_قدر("regionToPartitions", value)) {
                فاصلہ_ڈیٹا.region_مقصد_Partitions = value.get_ثنائی(length, error_رمز);
                اگر (U_FAILURE(error_رمز)) { واپس; }
                اگر (length < LSR::REGION_اشاریہ_حد_بندی) {
                    error_رمز = U_INVALID_FORMAT_ERROR;
                    واپس;
                }
            }

            اگر (!read_لڑیاں(مطابقت_جدول, "partitions", value,
                             تقسیم_Indexes, partitions_لمبائی, error_رمز) ||
                    !read_LSREncoded_لڑیاں(مطابقت_جدول, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_لمبائی, error_رمز)) {
                واپس;
            }
            اگر ((paradigm_Subtags_لمبائی % 3) != 0) {
                error_رمز = U_INVALID_FORMAT_ERROR;
                واپس;
            }

            اگر (مطابقت_جدول.ڈھونڈو_قدر("distances", value)) {
                فاصلہ_ڈیٹا.distances = value.get_Int_سمتیہ(length, error_رمز);
                اگر (U_FAILURE(error_رمز)) { واپس; }
                اگر (length < 4) {  // LocaleDistance IX_LIMIT
                    error_رمز = U_INVALID_FORMAT_ERROR;
                    واپس;
                }
            }
        } ورنہ اگر (مطابقت_Error_رمز == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ورنہ {  // error other than missing resource
            error_رمز = مطابقت_Error_رمز;
            واپس;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        لڑیاں.freeze();

        languageAliases = CharStringMap(languages_لمبائی / 2, error_رمز);
        برائے (int32_t i = 0; i < languages_لمبائی; i += 2) {
            languageAliases.put(لڑیاں.get(languageIndexes[i]),
                                لڑیاں.get(languageIndexes[i + 1]), error_رمز);
        }

        regionAliases = CharStringMap(regions_لمبائی / 2, error_رمز);
        برائے (int32_t i = 0; i < regions_لمبائی; i += 2) {
            regionAliases.put(لڑیاں.get(regionIndexes[i]),
                              لڑیاں.get(regionIndexes[i + 1]), error_رمز);
        }
        اگر (U_FAILURE(error_رمز)) { واپس; }

        lsrs_لمبائی = lsr_Subtags_لمبائی / 3;
        lsrs = نیا LSR[lsrs_لمبائی];
        اگر (lsrs == خالی_اشاریہ) {
            error_رمز = U_حافظہ_تخصیص_ERROR;
            واپس;
        }
        برائے (int32_t i = 0, j = 0; i < lsr_Subtags_لمبائی; i += 3, ++j) {
            lsrs[j] = LSR(لڑیاں.get(lsrSubtagIndexes[i]),
                          لڑیاں.get(lsrSubtagIndexes[i + 1]),
                          لڑیاں.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        اگر (partitions_لمبائی > 0) {
            فاصلہ_ڈیٹا.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitions_لمبائی * حجم(ثابت حرف *)));
            اگر (فاصلہ_ڈیٹا.partitions == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            برائے (int32_t i = 0; i < partitions_لمبائی; ++i) {
                فاصلہ_ڈیٹا.partitions[i] = لڑیاں.get(تقسیم_Indexes[i]);
            }
        }

        اگر (paradigm_Subtags_لمبائی > 0) {
            فاصلہ_ڈیٹا.paradigms_لمبائی = paradigm_Subtags_لمبائی / 3;
            LSR *paradigms = نیا LSR[فاصلہ_ڈیٹا.paradigms_لمبائی];
            اگر (paradigms == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس;
            }
            برائے (int32_t i = 0, j = 0; i < paradigm_Subtags_لمبائی; i += 3, ++j) {
                paradigms[j] = LSR(لڑیاں.get(paradigmSubtagIndexes[i]),
                                   لڑیاں.get(paradigmSubtagIndexes[i + 1]),
                                   لڑیاں.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            فاصلہ_ڈیٹا.paradigms = paradigms;
        }
    }

نجی:
    منطقی read_لڑیاں(ثابت Resource_جدول &جدول, ثابت حرف *کلید, Resource_قدر &value,
                     Local_حافظہ<int32_t> &indexes, int32_t &length, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
        اگر (جدول.ڈھونڈو_قدر(کلید, value)) {
            ResourceArray stringArray = value.getArray(error_رمز);
            اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
            length = stringArray.get_سائز();
            اگر (length == 0) { واپس سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            اگر (rawIndexes == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس جھوٹ;
            }
            برائے (صحیح i = 0; i < length; ++i) {
                اگر (stringArray.get_قدر(i, value)) {  // returns true because i < length
                    int32_t str_لمبائی = 0;
                    rawIndexes[i] = لڑیاں.جوڑو(value.getString(str_لمبائی, error_رمز), error_رمز);
                    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
                }
            }
        }
        واپس سچ;
    }
    UnicodeString مقصد_Language(صحیح encoded) {
        اگر (encoded == 0) {
            واپس UNICODE_STRING_SIMPLE("");
        }
        اگر (encoded == 1) {
            واپس UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        حرف lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        اگر (encoded / (27 * 27) == 0) {
            واپس UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        واپس UnicodeString(lang, 3, US_INV);
    }
    UnicodeString مقصد_Script(صحیح encoded) {
        اگر (encoded == 0) {
            واپس UNICODE_STRING_SIMPLE("");
        }
        اگر (encoded == 1) {
            واپس UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ثابت حرف* script = uscript_getShortName(static_cast<UScript_رمز>(encoded));
        اگر (script == خالی_اشاریہ) {
            واپس UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        واپس UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_اشاریہ_مقصد_رمز(ثابت ResourceArray &m49Array, Resource_قدر &value, صحیح اشاریہ, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) {
            واپس UNICODE_STRING_SIMPLE("");
        }
        اگر (m49Array.get_قدر(اشاریہ, value)) {
            واپس value.getUnicodeString(error_رمز);
        }
        // "m49" does not include the index.
        error_رمز = U_MISSING_RESOURCE_ERROR;
        واپس UNICODE_STRING_SIMPLE("");
    }

    UnicodeString مقصد_Region(ثابت ResourceArray& m49Array, Resource_قدر &value, صحیح encoded, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز) || encoded == 0 || encoded == 1) {
            واپس UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        اگر (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            واپس  m49_اشاریہ_مقصد_رمز(m49Array, value, encoded, error_رمز);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        واپس UnicodeString(region, 2, US_INV);
    }

    منطقی read_LSREncoded_لڑیاں(ثابت Resource_جدول &جدول, ثابت حرف* کلید, Resource_قدر &value, ثابت ResourceArray& m49Array,
                     Local_حافظہ<int32_t> &indexes, int32_t &length, UError_رمز &error_رمز) {
        اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
        اگر (جدول.ڈھونڈو_قدر(کلید, value)) {
            ثابت int32_t* vectors = value.get_Int_سمتیہ(length, error_رمز);
            اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
            اگر (length == 0) { واپس سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            اگر (rawIndexes == خالی_اشاریہ) {
                error_رمز = U_حافظہ_تخصیص_ERROR;
                واپس جھوٹ;
            }
            برائے (صحیح i = 0; i < length; ++i) {
                rawIndexes[i*3] = لڑیاں.جوڑو_By_قدر(مقصد_Language(vectors[i]), error_رمز);
                rawIndexes[i*3+1] = لڑیاں.جوڑو_By_قدر(مقصد_Script(vectors[i]), error_رمز);
                rawIndexes[i*3+2] = لڑیاں.جوڑو_By_قدر(
                    مقصد_Region(m49Array, value, vectors[i], error_رمز), error_رمز);
                اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
            }
            length *= 3;
        }
        واپس سچ;
    }
};

نامخلا {

LikelySubtags *gLikelySubtags = خالی_اشاریہ;
UVector *gMacroregions = خالی_اشاریہ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    حذف gLikelySubtags;
    gLikelySubtags = خالی_اشاریہ;
    حذف gMacroregions;
    gMacroregions = خالی_اشاریہ;
    gInitOnce.reset();
    واپس سچ;
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
خالی processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UError_رمز& حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس; }
    int32_t rangeMarkerLocation = regionName.اشاریہ_کا(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,حالت_2);
    اگر ( rangeMarkerLocation > 0 ) {
        char16_t انتہا_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        جب ( buf[rangeMarkerLocation-1] <= انتہا_Range && U_SUCCESS(حالت_2)) {
            Local_اشارہ_گر<UnicodeString> newRegion(نیا UnicodeString(buf), حالت_2);
            newMacroRegions->adoptElement(newRegion.orphan(),حالت_2);
            buf[rangeMarkerLocation-1]++;
        }
    } ورنہ {
        Local_اشارہ_گر<UnicodeString> newRegion(نیا UnicodeString(regionName), حالت_2);
        newMacroRegions->adoptElement(newRegion.orphan(),حالت_2);
    }
}

#اگر U_DEBUG
UVector* بھرو_Macroregions(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس خالی_اشاریہ; }
    Local_اشارہ_گر<UVector> newMacroRegions(نیا UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت_2), حالت_2);

    Local_UResource_Bundle_اشارہ_گر supplemental_ڈیٹا(ures_open_براہ_راست(خالی_اشاریہ,"supplementalData",&حالت_2));
    Local_UResource_Bundle_اشارہ_گر idValidity(ures_get_By_کلید(supplemental_ڈیٹا.getAlias(),"idValidity",خالی_اشاریہ,&حالت_2));
    Local_UResource_Bundle_اشارہ_گر regionList(ures_get_By_کلید(idValidity.getAlias(),"region",خالی_اشاریہ,&حالت_2));
    Local_UResource_Bundle_اشارہ_گر regionMacro(ures_get_By_کلید(regionList.getAlias(),"macroregion",خالی_اشاریہ,&حالت_2));

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    جب (ures_has_اگلا(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_اگلا_Unicode_String(regionMacro.getAlias(),خالی_اشاریہ,&حالت_2);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس خالی_اشاریہ;
        }
    }

    واپس newMacroRegions.orphan();
}
#اگر_ختم // U_DEBUG

UVector* getStaticMacroregions(UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) { واپس خالی_اشاریہ; }
    Local_اشارہ_گر<UVector> newMacroRegions(نیا UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت_2), حالت_2);

    اگر (U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }

    برائے (ثابت خودکار *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت_2);
        اگر (U_FAILURE(حالت_2)) {
            واپس خالی_اشاریہ;
        }
    }

    واپس newMacroRegions.orphan();
}

}  // namespace

خالی U_CALLCONV LikelySubtags::initLikelySubtags(UError_رمز &error_رمز) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == خالی_اشاریہ);
    Likely_Subtags_ڈیٹا ڈیٹا(error_رمز);
    ڈیٹا.load(error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس; }
    gLikelySubtags = نیا LikelySubtags(ڈیٹا);
    gMacroregions = getStaticMacroregions(error_رمز);
#اگر U_DEBUG
    خودکار macroregions_From_ڈیٹا = بھرو_Macroregions(error_رمز);
    U_ASSERT((*gMacroregions) == (*macroregions_From_ڈیٹا));
    حذف macroregions_From_ڈیٹا;
#اگر_ختم
    اگر (U_FAILURE(error_رمز) || gLikelySubtags == خالی_اشاریہ || gMacroregions == خالی_اشاریہ) {
        حذف gLikelySubtags;
        حذف gMacroregions;
        error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }

    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UError_رمز &error_رمز) {
    اگر (U_FAILURE(error_رمز)) { واپس خالی_اشاریہ; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_رمز);
    واپس gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_ڈیٹا &ڈیٹا) :
        langInfoBundle(ڈیٹا.langInfoBundle),
        لڑیاں(ڈیٹا.لڑیاں.orphan_Char_لڑیاں()),
        languageAliases(std::move(ڈیٹا.languageAliases)),
        regionAliases(std::move(ڈیٹا.regionAliases)),
        trie(ڈیٹا.trie_بائٹیں),
        lsrs(ڈیٹا.lsrs),
#اگر U_DEBUG
        lsrs_لمبائی(ڈیٹا.lsrs_لمبائی),
#اگر_ختم // U_DEBUG
        فاصلہ_ڈیٹا(std::move(ڈیٹا.فاصلہ_ڈیٹا)) {
    ڈیٹا.langInfoBundle = خالی_اشاریہ;
    ڈیٹا.lsrs = خالی_اشاریہ;

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

    برائے (char16_t c = u'a'; c <= u'z'; ++c) {
        نتیجہ = trie.next(c);
        اگر (نتیجہ == USTRINGTRIE_NO_قدر) {
            trie_پہلا_Letter_حالتیں[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    حذف لڑیاں;
    حذف[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقی return_داخلہ_If_Unmatch,
                                         UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    اگر (locale.isBogus()) {
        error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس {};
    }
    ثابت حرف *name = locale.getName();
    اگر (!return_داخلہ_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        واپس LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_داخلہ_If_Unmatch, error_رمز);

    اگر (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        واپس LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_رمز);
    }
    واپس max;
}

نامخلا {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    واپس canonical == خالی_اشاریہ ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقی return_داخلہ_If_Unmatch,
                                     UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    اگر (!return_داخلہ_If_Unmatch) {
        حرف c1;
        اگر (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            انتخاب (c1) {
            حالت 'A':
                واپس LSR(PSEUDO_ACCENTS_سابقہ, language, script, region,
                           LSR::EXPLICIT_LSR, error_رمز);
            حالت 'B':
                واپس LSR(PSEUDO_BIDI_سابقہ, language, script, region,
                           LSR::EXPLICIT_LSR, error_رمز);
            حالت 'C':
                واپس LSR(PSEUDO_CRACKED_سابقہ, language, script, region,
                           LSR::EXPLICIT_LSR, error_رمز);
            طےشدہ:  // normal locale
                توڑو;
            }
        }

        اگر (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            اگر (uprv_strcmp(variant, "PSACCENT") == 0) {
                واپس LSR(PSEUDO_ACCENTS_سابقہ, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_رمز);
            } ورنہ اگر (uprv_strcmp(variant, "PSBIDI") == 0) {
                واپس LSR(PSEUDO_BIDI_سابقہ, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_رمز);
            } ورنہ اگر (uprv_strcmp(variant, "PSCRACK") == 0) {
                واپس LSR(PSEUDO_CRACKED_سابقہ, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_رمز);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    واپس maximize(language, script, region, return_داخلہ_If_Unmatch, error_رمز);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقی return_داخلہ_If_Unmatch,
                             UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    واپس maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_داخلہ_If_Unmatch,
                    error_رمز);
}

منطقی LikelySubtags::isMacroregion(StringPiece& region, UError_رمز& error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس جھوٹ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    واپس gMacroregions->شامل_ہے((خالی *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقی return_داخلہ_If_Unmatch,
                             UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    اگر (language.compare("und") == 0) {
        language = "";
    }
    اگر (script.compare("Zzzz") == 0) {
        script = "";
    }
    اگر (region.compare("ZZ") == 0) {
        region = "";
    }
    اگر (!script.empty() && !region.empty() && !language.empty()) {
        واپس LSR(language, script, region, LSR::EXPLICIT_LSR, error_رمز);  // already maximized
    }
    منطقی retainLanguage = جھوٹ;
    منطقی retainScript = جھوٹ;
    منطقی retainRegion = جھوٹ;

    بائٹیں_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    اگر (0 <= (c0 = uprv_زیریں_Ordinal(language.ڈیٹا()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_پہلا_Letter_حالتیں[c0]) != 0) {
        value = trie_اگلا(iter.reset_مقصد_State64(state), language, 1);
    } ورنہ {
        value = trie_اگلا(iter, language, 0);
    }
    منطقی مطابقت_Language = (value >= 0);
    منطقی مطابقت_Script = جھوٹ;
    اگر (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ورنہ {
        retainLanguage = سچ;
        iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    اگر (value >= 0 && !script.empty()) {
        مطابقت_Script = سچ;
    }
    اگر (value > 0) {
        // Intermediate or final value from just language.
        اگر (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ورنہ {
        value = trie_اگلا(iter, script, 0);
        اگر (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ورنہ {
            retainScript = سچ;
            اگر (state == 0) {
                iter.reset_مقصد_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ورنہ {
                iter.reset_مقصد_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقی مطابقت_Region = جھوٹ;
    اگر (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ورنہ {
        value = trie_اگلا(iter, region, 0);
        اگر (value >= 0) {
            اگر (!region.empty() && !isMacroregion(region, error_رمز)) {
                retainRegion = سچ;
                مطابقت_Region = سچ;
            }
        } ورنہ {
            retainRegion = سچ;
            اگر (state == 0) {
                value = default_Lsr_اشاریہ;
            } ورنہ {
                iter.reset_مقصد_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                اگر (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
                    value = trie_اگلا(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_خالی_خانے_State = iter.getState64();
                    value = trie_اگلا(iter, region, 0);
                    // Fallback to und =>
                    اگر (value < 0) {
                        iter.reset_مقصد_State64(trie_Und_خالی_خانے_State);
                        value = trie_اگلا(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_لمبائی);
    اگر (return_داخلہ_If_Unmatch &&
        (!(مطابقت_Language || مطابقت_Script || (مطابقت_Region && language.empty())))) {
      واپس LSR("", "", "", LSR::EXPLICIT_LSR, error_رمز);  // no matching.
    }
    اگر (language.empty()) {
        language = StringPiece("und");
    }

    اگر (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ثابت LSR &matched = lsrs[value];
        واپس LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    اگر (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    اگر (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    اگر (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_نقاب = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    واپس LSR(language, script, region, retain_نقاب, error_رمز);
}

int32_t LikelySubtags::compareLikely(ثابت LSR &lsr, ثابت LSR &other, int32_t likelyInfo) ثابت {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    اگر (uprv_strcmp(lsr.language, other.language) != 0) {
        واپس 0xfffffffc;  // negative, lsr not better than other
    }
    اگر (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t اشاریہ;
        اگر (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            اشاریہ = likelyInfo >> 2;
        } ورنہ {
            اشاریہ = get_Likely_اشاریہ(lsr.language, "");
            likelyInfo = اشاریہ << 2;
        }
        ثابت LSR &likely = lsrs[اشاریہ];
        اگر (uprv_strcmp(lsr.script, likely.script) == 0) {
            واپس likelyInfo | 1;
        } ورنہ {
            واپس likelyInfo & ~1;
        }
    }
    اگر (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t اشاریہ;
        اگر (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            اشاریہ = likelyInfo >> 2;
        } ورنہ {
            اشاریہ = get_Likely_اشاریہ(lsr.language, lsr.region);
            likelyInfo = (اشاریہ << 2) | 2;
        }
        ثابت LSR &likely = lsrs[اشاریہ];
        اگر (uprv_strcmp(lsr.region, likely.region) == 0) {
            واپس likelyInfo | 1;
        } ورنہ {
            واپس likelyInfo & ~1;
        }
    }
    واپس likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_اشاریہ(ثابت حرف *language, ثابت حرف *script) ثابت {
    اگر (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    اگر (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    بائٹیں_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    اگر (0 <= (c0 = uprv_زیریں_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_پہلا_Letter_حالتیں[c0]) != 0) {
        value = trie_اگلا(iter.reset_مقصد_State64(state), language, 1);
    } ورنہ {
        value = trie_اگلا(iter, language, 0);
    }
    اگر (value >= 0) {
        state = iter.getState64();
    } ورنہ {
        iter.reset_مقصد_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    اگر (value > 0) {
        // Intermediate or final value from just language.
        اگر (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ورنہ {
        value = trie_اگلا(iter, script, 0);
        اگر (value >= 0) {
            state = iter.getState64();
        } ورنہ {
            اگر (state == 0) {
                iter.reset_مقصد_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ورنہ {
                iter.reset_مقصد_State64(state);
                value = trie_اگلا(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    اگر (value > 0) {
        // Final value from just language or language+script.
    } ورنہ {
        value = trie_اگلا(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_لمبائی);
    واپس value;
}

int32_t LikelySubtags::trie_اگلا(بائٹیں_Trie &iter, ثابت حرف *s, int32_t i) {
    UString_Trie_نتیجہ نتیجہ;
    uint8_t c;
    اگر ((c = s[i]) == 0) {
        نتیجہ = iter.next(u'*');
    } ورنہ {
        برائے (;;) {
            c = uprv_inv_Char_مقصد_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            اگر (next != 0) {
                اگر (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                    واپس -1;
                }
            } ورنہ {
                // last character of this subtag
                نتیجہ = iter.next(c | 0x80);
                توڑو;
            }
            c = next;
        }
    }
    انتخاب (نتیجہ) {
    حالت USTRINGTRIE_NO_مطابقت: واپس -1;
    حالت USTRINGTRIE_NO_قدر: واپس 0;
    حالت USTRINGTRIE_INTERMEDIATE_قدر:
        U_ASSERT(iter.get_قدر() == SKIP_SCRIPT);
        واپس SKIP_SCRIPT;
    حالت USTRINGTRIE_FINAL_قدر: واپس iter.get_قدر();
    طےشدہ: واپس -1;
    }
}
int32_t LikelySubtags::trie_اگلا(بائٹیں_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_نتیجہ نتیجہ;
    uint8_t c;
    اگر (s.length() == i) {
        نتیجہ = iter.next(u'*');
    } ورنہ {
        c = s.ڈیٹا()[i];
        برائے (;;) {
            c = uprv_inv_Char_مقصد_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            اگر (i+1 != s.length()) {
                اگر (!USTRINGTRIE_HAS_اگلا(iter.next(c))) {
                    واپس -1;
                }
                c = s.ڈیٹا()[++i];
            } ورنہ {
                // last character of this subtag
                نتیجہ = iter.next(c | 0x80);
                توڑو;
            }
        }
    }
    انتخاب (نتیجہ) {
    حالت USTRINGTRIE_NO_مطابقت: واپس -1;
    حالت USTRINGTRIE_NO_قدر: واپس 0;
    حالت USTRINGTRIE_INTERMEDIATE_قدر:
        U_ASSERT(iter.get_قدر() == SKIP_SCRIPT);
        واپس SKIP_SCRIPT;
    حالت USTRINGTRIE_FINAL_قدر: واپس iter.get_قدر();
    طےشدہ: واپس -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    منطقی favorScript,
                                    UError_رمز &error_رمز) ثابت {
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    LSR max = maximize(language, script, region, سچ, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    // If no match, return it.
    اگر (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        واپس LSR(language, script, region, LSR::EXPLICIT_LSR, error_رمز);
    }
    // try language
    LSR test = maximize(max.language, "", "", سچ, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    اگر (test.is_Equivalent_مقصد(max)) {
        واپس LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_رمز);
    }

    اگر (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس {}; }
        اگر (test.is_Equivalent_مقصد(max)) {
            واپس LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_رمز);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", سچ, error_رمز);
    اگر (U_FAILURE(error_رمز)) { واپس {}; }
    اگر (test.is_Equivalent_مقصد(max)) {
        واپس LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_رمز);
    }
    اگر (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_رمز);
        اگر (U_FAILURE(error_رمز)) { واپس {}; }
        اگر (test.is_Equivalent_مقصد(max)) {
            واپس LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_رمز);
        }
    }
    واپس LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_رمز);
}

U_NAMESPACE_انتہا
