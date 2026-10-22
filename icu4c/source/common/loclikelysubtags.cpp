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

constexpr حرف PSEUDO_ACCENTS_اڳياڙي = '\'';  // -XA, -PSACCENT
constexpr حرف PSEUDO_BIDI_اڳياڙي = '+';  // -XB, -PSBIDI
constexpr حرف PSEUDO_CRACKED_اڳياڙي = ',';  // -XC, -PSCRACK

}  // namespace

Locale_مفاصلو_معلومات::Locale_مفاصلو_معلومات(Locale_مفاصلو_معلومات &&معلومات) :
        مفاصلو_Trie_بائيٽون(معلومات.مفاصلو_Trie_بائيٽون),
        region_منزل_Partitions(معلومات.region_منزل_Partitions),
        partitions(معلومات.partitions),
        paradigms(معلومات.paradigms), paradigms_ڊيگهائي(معلومات.paradigms_ڊيگهائي),
        distances(معلومات.distances) {
    معلومات.partitions = خالي_اشارو;
    معلومات.paradigms = خالي_اشارو;
}

Locale_مفاصلو_معلومات::~Locale_مفاصلو_معلومات() {
    uprv_free(partitions);
    ميٽيو[] paradigms;
}

بنية Likely_Subtags_معلومات {
    UResourceBundle *langInfoBundle = خالي_اشارو;
    Unique_Char_لڙيون لڙيون;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ثابت uint8_t *trie_بائيٽون = خالي_اشارو;
    LSR *lsrs = خالي_اشارو;
    int32_t lsrs_ڊيگهائي = 0;

    Locale_مفاصلو_معلومات مفاصلو_معلومات;

    Likely_Subtags_معلومات(UError_ڪوڊ &error_ڪوڊ) : لڙيون(error_ڪوڊ) {}

    ~Likely_Subtags_معلومات() {
        ures_close(langInfoBundle);
        ميٽيو[] lsrs;
    }

    فراغ load(UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        langInfoBundle = ures_open_سڌو(خالي_اشارو, "langInfo", &error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
        ڍير_UResource_Bundle ڍير_عارضي_Bundle;
        Resource_معلومات_قدر value;
        ures_get_قدر_With_Fallback(langInfoBundle, "likely", ڍير_عارضي_Bundle.getAlias(),
                                  value, error_ڪوڊ);
        Resource_جدول likely_جدول = value.get_جدول(error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_يادگيري<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_ڊيگهائي = 0, regions_ڊيگهائي = 0, lsr_Subtags_ڊيگهائي = 0;
        ResourceArray m49Array;
        إذا (likely_جدول.ڳوليو_قدر("m49", value)) {
            m49Array = value.getArray(error_ڪوڊ);
        } وإلا {
            error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        إذا (!read_لڙيون(likely_جدول, "languageAliases", value,
                         languageIndexes, languages_ڊيگهائي, error_ڪوڊ) ||
                !read_لڙيون(likely_جدول, "regionAliases", value,
                             regionIndexes, regions_ڊيگهائي, error_ڪوڊ) ||
                !read_LSREncoded_لڙيون(likely_جدول, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_ڊيگهائي, error_ڪوڊ)) {
            أرجع;
        }
        إذا ((languages_ڊيگهائي & 1) != 0 ||
                (regions_ڊيگهائي & 1) != 0 ||
                (lsr_Subtags_ڊيگهائي % 3) != 0) {
            error_ڪوڊ = U_INVALID_FORMAT_ERROR;
            أرجع;
        }
        إذا (lsr_Subtags_ڊيگهائي == 0) {
            error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }

        إذا (!likely_جدول.ڳوليو_قدر("trie", value)) {
            error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
            أرجع;
        }
        int32_t length;
        trie_بائيٽون = value.get_ٻٽو(length, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_ڪوڊ ميلاپ_Error_ڪوڊ = U_ZERO_ERROR;
        ures_get_قدر_With_Fallback(langInfoBundle, "match", ڍير_عارضي_Bundle.getAlias(),
                                  value, ميلاپ_Error_ڪوڊ);
        Local_يادگيري<int32_t> حصا_ڪريو_Indexes, paradigmSubtagIndexes;
        int32_t partitions_ڊيگهائي = 0, paradigm_Subtags_ڊيگهائي = 0;
        إذا (U_SUCCESS(ميلاپ_Error_ڪوڊ)) {
            Resource_جدول ميلاپ_جدول = value.get_جدول(error_ڪوڊ);
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }

            إذا (ميلاپ_جدول.ڳوليو_قدر("trie", value)) {
                مفاصلو_معلومات.مفاصلو_Trie_بائيٽون = value.get_ٻٽو(length, error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
            }

            إذا (ميلاپ_جدول.ڳوليو_قدر("regionToPartitions", value)) {
                مفاصلو_معلومات.region_منزل_Partitions = value.get_ٻٽو(length, error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
                إذا (length < LSR::REGION_اشاريو_حد_بندي) {
                    error_ڪوڊ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }

            إذا (!read_لڙيون(ميلاپ_جدول, "partitions", value,
                             حصا_ڪريو_Indexes, partitions_ڊيگهائي, error_ڪوڊ) ||
                    !read_LSREncoded_لڙيون(ميلاپ_جدول, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_ڊيگهائي, error_ڪوڊ)) {
                أرجع;
            }
            إذا ((paradigm_Subtags_ڊيگهائي % 3) != 0) {
                error_ڪوڊ = U_INVALID_FORMAT_ERROR;
                أرجع;
            }

            إذا (ميلاپ_جدول.ڳوليو_قدر("distances", value)) {
                مفاصلو_معلومات.distances = value.get_Int_ويڪٽر(length, error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
                إذا (length < 4) {  // LocaleDistance IX_LIMIT
                    error_ڪوڊ = U_INVALID_FORMAT_ERROR;
                    أرجع;
                }
            }
        } وإلا إذا (ميلاپ_Error_ڪوڊ == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } وإلا {  // error other than missing resource
            error_ڪوڊ = ميلاپ_Error_ڪوڊ;
            أرجع;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        لڙيون.freeze();

        languageAliases = CharStringMap(languages_ڊيگهائي / 2, error_ڪوڊ);
        لكل (int32_t i = 0; i < languages_ڊيگهائي; i += 2) {
            languageAliases.put(لڙيون.get(languageIndexes[i]),
                                لڙيون.get(languageIndexes[i + 1]), error_ڪوڊ);
        }

        regionAliases = CharStringMap(regions_ڊيگهائي / 2, error_ڪوڊ);
        لكل (int32_t i = 0; i < regions_ڊيگهائي; i += 2) {
            regionAliases.put(لڙيون.get(regionIndexes[i]),
                              لڙيون.get(regionIndexes[i + 1]), error_ڪوڊ);
        }
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }

        lsrs_ڊيگهائي = lsr_Subtags_ڊيگهائي / 3;
        lsrs = جديد LSR[lsrs_ڊيگهائي];
        إذا (lsrs == خالي_اشارو) {
            error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
            أرجع;
        }
        لكل (int32_t i = 0, j = 0; i < lsr_Subtags_ڊيگهائي; i += 3, ++j) {
            lsrs[j] = LSR(لڙيون.get(lsrSubtagIndexes[i]),
                          لڙيون.get(lsrSubtagIndexes[i + 1]),
                          لڙيون.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        إذا (partitions_ڊيگهائي > 0) {
            مفاصلو_معلومات.partitions = static_cast<ثابت حرف **>(
                uprv_malloc(partitions_ڊيگهائي * حجم(ثابت حرف *)));
            إذا (مفاصلو_معلومات.partitions == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0; i < partitions_ڊيگهائي; ++i) {
                مفاصلو_معلومات.partitions[i] = لڙيون.get(حصا_ڪريو_Indexes[i]);
            }
        }

        إذا (paradigm_Subtags_ڊيگهائي > 0) {
            مفاصلو_معلومات.paradigms_ڊيگهائي = paradigm_Subtags_ڊيگهائي / 3;
            LSR *paradigms = جديد LSR[مفاصلو_معلومات.paradigms_ڊيگهائي];
            إذا (paradigms == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع;
            }
            لكل (int32_t i = 0, j = 0; i < paradigm_Subtags_ڊيگهائي; i += 3, ++j) {
                paradigms[j] = LSR(لڙيون.get(paradigmSubtagIndexes[i]),
                                   لڙيون.get(paradigmSubtagIndexes[i + 1]),
                                   لڙيون.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            مفاصلو_معلومات.paradigms = paradigms;
        }
    }

خاص:
    منطقي read_لڙيون(ثابت Resource_جدول &جدول, ثابت حرف *ڪنجي, Resource_قدر &value,
                     Local_يادگيري<int32_t> &indexes, int32_t &length, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
        إذا (جدول.ڳوليو_قدر(ڪنجي, value)) {
            ResourceArray stringArray = value.getArray(error_ڪوڊ);
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
            length = stringArray.get_سائيز();
            إذا (length == 0) { أرجع سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            إذا (rawIndexes == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                إذا (stringArray.get_قدر(i, value)) {  // returns true because i < length
                    int32_t str_ڊيگهائي = 0;
                    rawIndexes[i] = لڙيون.جوڙيو(value.getString(str_ڊيگهائي, error_ڪوڊ), error_ڪوڊ);
                    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
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
        ثابت حرف* script = uscript_getShortName(static_cast<UScript_ڪوڊ>(encoded));
        إذا (script == خالي_اشارو) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        أرجع UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_اشاريو_منزل_ڪوڊ(ثابت ResourceArray &m49Array, Resource_قدر &value, صحيح اشاريو, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        إذا (m49Array.get_قدر(اشاريو, value)) {
            أرجع value.getUnicodeString(error_ڪوڊ);
        }
        // "m49" does not include the index.
        error_ڪوڊ = U_MISSING_RESOURCE_ERROR;
        أرجع UNICODE_STRING_SIMPLE("");
    }

    UnicodeString منزل_Region(ثابت ResourceArray& m49Array, Resource_قدر &value, صحيح encoded, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ) || encoded == 0 || encoded == 1) {
            أرجع UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        إذا (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            أرجع  m49_اشاريو_منزل_ڪوڊ(m49Array, value, encoded, error_ڪوڊ);
        }
        حرف region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        أرجع UnicodeString(region, 2, US_INV);
    }

    منطقي read_LSREncoded_لڙيون(ثابت Resource_جدول &جدول, ثابت حرف* ڪنجي, Resource_قدر &value, ثابت ResourceArray& m49Array,
                     Local_يادگيري<int32_t> &indexes, int32_t &length, UError_ڪوڊ &error_ڪوڊ) {
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
        إذا (جدول.ڳوليو_قدر(ڪنجي, value)) {
            ثابت int32_t* vectors = value.get_Int_ويڪٽر(length, error_ڪوڊ);
            إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
            إذا (length == 0) { أرجع سچ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            إذا (rawIndexes == خالي_اشارو) {
                error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
                أرجع خطأ;
            }
            لكل (صحيح i = 0; i < length; ++i) {
                rawIndexes[i*3] = لڙيون.جوڙيو_By_قدر(منزل_Language(vectors[i]), error_ڪوڊ);
                rawIndexes[i*3+1] = لڙيون.جوڙيو_By_قدر(منزل_Script(vectors[i]), error_ڪوڊ);
                rawIndexes[i*3+2] = لڙيون.جوڙيو_By_قدر(
                    منزل_Region(m49Array, value, vectors[i], error_ڪوڊ), error_ڪوڊ);
                إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
            }
            length *= 3;
        }
        أرجع سچ;
    }
};

نطاق {

LikelySubtags *gLikelySubtags = خالي_اشارو;
UVector *gMacroregions = خالي_اشارو;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ميٽيو gLikelySubtags;
    gLikelySubtags = خالي_اشارو;
    ميٽيو gMacroregions;
    gMacroregions = خالي_اشارو;
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
فراغ processMacroregionRange(ثابت UnicodeString& regionName, UVector* newMacroRegions, UError_ڪوڊ& حالت) {
    إذا (U_FAILURE(حالت)) { أرجع; }
    int32_t rangeMarkerLocation = regionName.اشاريو_جو(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,حالت);
    إذا ( rangeMarkerLocation > 0 ) {
        char16_t پڇاڙي_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        طالما ( buf[rangeMarkerLocation-1] <= پڇاڙي_Range && U_SUCCESS(حالت)) {
            Local_اشارو<UnicodeString> newRegion(جديد UnicodeString(buf), حالت);
            newMacroRegions->adoptElement(newRegion.orphan(),حالت);
            buf[rangeMarkerLocation-1]++;
        }
    } وإلا {
        Local_اشارو<UnicodeString> newRegion(جديد UnicodeString(regionName), حالت);
        newMacroRegions->adoptElement(newRegion.orphan(),حالت);
    }
}

#إذا U_DEBUG
UVector* ڀريو_Macroregions(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالي_اشارو; }
    Local_اشارو<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    Local_UResource_Bundle_اشارو supplemental_معلومات(ures_open_سڌو(خالي_اشارو,"supplementalData",&حالت));
    Local_UResource_Bundle_اشارو idValidity(ures_get_By_ڪنجي(supplemental_معلومات.getAlias(),"idValidity",خالي_اشارو,&حالت));
    Local_UResource_Bundle_اشارو regionList(ures_get_By_ڪنجي(idValidity.getAlias(),"region",خالي_اشارو,&حالت));
    Local_UResource_Bundle_اشارو regionMacro(ures_get_By_ڪنجي(regionList.getAlias(),"macroregion",خالي_اشارو,&حالت));

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    طالما (ures_has_ايندڙ(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_ايندڙ_Unicode_String(regionMacro.getAlias(),خالي_اشارو,&حالت);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خالي_اشارو;
        }
    }

    أرجع newMacroRegions.orphan();
}
#جيڪڏهن_پورو // U_DEBUG

UVector* getStaticMacroregions(UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) { أرجع خالي_اشارو; }
    Local_اشارو<UVector> newMacroRegions(جديد UVector(uprv_deleteUObject, uhash_compareUnicodeString, حالت), حالت);

    إذا (U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }

    لكل (ثابت خودڪار *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), حالت);
        إذا (U_FAILURE(حالت)) {
            أرجع خالي_اشارو;
        }
    }

    أرجع newMacroRegions.orphan();
}

}  // namespace

فراغ U_CALLCONV LikelySubtags::initLikelySubtags(UError_ڪوڊ &error_ڪوڊ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == خالي_اشارو);
    Likely_Subtags_معلومات معلومات(error_ڪوڊ);
    معلومات.load(error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع; }
    gLikelySubtags = جديد LikelySubtags(معلومات);
    gMacroregions = getStaticMacroregions(error_ڪوڊ);
#إذا U_DEBUG
    خودڪار macroregions_From_معلومات = ڀريو_Macroregions(error_ڪوڊ);
    U_ASSERT((*gMacroregions) == (*macroregions_From_معلومات));
    ميٽيو macroregions_From_معلومات;
#جيڪڏهن_پورو
    إذا (U_FAILURE(error_ڪوڊ) || gLikelySubtags == خالي_اشارو || gMacroregions == خالي_اشارو) {
        ميٽيو gLikelySubtags;
        ميٽيو gMacroregions;
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }

    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ثابت LikelySubtags *LikelySubtags::getSingleton(UError_ڪوڊ &error_ڪوڊ) {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خالي_اشارو; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ڪوڊ);
    أرجع gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_معلومات &معلومات) :
        langInfoBundle(معلومات.langInfoBundle),
        لڙيون(معلومات.لڙيون.orphan_Char_لڙيون()),
        languageAliases(std::move(معلومات.languageAliases)),
        regionAliases(std::move(معلومات.regionAliases)),
        trie(معلومات.trie_بائيٽون),
        lsrs(معلومات.lsrs),
#إذا U_DEBUG
        lsrs_ڊيگهائي(معلومات.lsrs_ڊيگهائي),
#جيڪڏهن_پورو // U_DEBUG
        مفاصلو_معلومات(std::move(معلومات.مفاصلو_معلومات)) {
    معلومات.langInfoBundle = خالي_اشارو;
    معلومات.lsrs = خالي_اشارو;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_نتيجو نتيجو = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ايندڙ(نتيجو));
    trieUndState = trie.getState64();
    نتيجو = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ايندڙ(نتيجو));
    trieUndZzzzState = trie.getState64();
    نتيجو = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_قدر(نتيجو));
    default_Lsr_اشاريو = trie.get_قدر();
    trie.reset();

    لكل (char16_t c = u'a'; c <= u'z'; ++c) {
        نتيجو = trie.next(c);
        إذا (نتيجو == USTRINGTRIE_NO_قدر) {
            trie_پهريون_Letter_حالتون[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    ميٽيو لڙيون;
    ميٽيو[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ثابت Locale &locale,
                                         منطقي return_داخلا_If_Unmatch,
                                         UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    إذا (locale.isBogus()) {
        error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع {};
    }
    ثابت حرف *name = locale.getName();
    إذا (!return_داخلا_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        أرجع LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_داخلا_If_Unmatch, error_ڪوڊ);

    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        أرجع LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_ڪوڊ);
    }
    أرجع max;
}

نطاق {

ثابت حرف *getCanonical(ثابت CharStringMap &aliases, ثابت حرف *alias) {
    ثابت حرف *canonical = aliases.get(alias);
    أرجع canonical == خالي_اشارو ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                                     ثابت حرف *variant,
                                     منطقي return_داخلا_If_Unmatch,
                                     UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    إذا (!return_داخلا_If_Unmatch) {
        حرف c1;
        إذا (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            بدل (c1) {
            حالة 'A':
                أرجع LSR(PSEUDO_ACCENTS_اڳياڙي, language, script, region,
                           LSR::EXPLICIT_LSR, error_ڪوڊ);
            حالة 'B':
                أرجع LSR(PSEUDO_BIDI_اڳياڙي, language, script, region,
                           LSR::EXPLICIT_LSR, error_ڪوڊ);
            حالة 'C':
                أرجع LSR(PSEUDO_CRACKED_اڳياڙي, language, script, region,
                           LSR::EXPLICIT_LSR, error_ڪوڊ);
            افتراضي:  // normal locale
                اكسر;
            }
        }

        إذا (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            إذا (uprv_strcmp(variant, "PSACCENT") == 0) {
                أرجع LSR(PSEUDO_ACCENTS_اڳياڙي, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_ڪوڊ);
            } وإلا إذا (uprv_strcmp(variant, "PSBIDI") == 0) {
                أرجع LSR(PSEUDO_BIDI_اڳياڙي, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_ڪوڊ);
            } وإلا إذا (uprv_strcmp(variant, "PSCRACK") == 0) {
                أرجع LSR(PSEUDO_CRACKED_اڳياڙي, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_ڪوڊ);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    أرجع maximize(language, script, region, return_داخلا_If_Unmatch, error_ڪوڊ);
}

LSR LikelySubtags::maximize(ثابت حرف *language, ثابت حرف *script, ثابت حرف *region,
                             منطقي return_داخلا_If_Unmatch,
                             UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    أرجع maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_داخلا_If_Unmatch,
                    error_ڪوڊ);
}

منطقي LikelySubtags::isMacroregion(StringPiece& region, UError_ڪوڊ& error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع خطأ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    أرجع gMacroregions->شامل_آهي((فراغ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             منطقي return_داخلا_If_Unmatch,
                             UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
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
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_ڪوڊ);  // already maximized
    }
    منطقي retainLanguage = خطأ;
    منطقي retainScript = خطأ;
    منطقي retainRegion = خطأ;

    بائيٽون_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_هيٺاهون_Ordinal(language.معلومات()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_پهريون_Letter_حالتون[c0]) != 0) {
        value = trie_ايندڙ(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_ايندڙ(iter, language, 0);
    }
    منطقي ميلاپ_Language = (value >= 0);
    منطقي ميلاپ_Script = خطأ;
    إذا (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } وإلا {
        retainLanguage = سچ;
        iter.reset_منزل_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    إذا (value >= 0 && !script.empty()) {
        ميلاپ_Script = سچ;
    }
    إذا (value > 0) {
        // Intermediate or final value from just language.
        إذا (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } وإلا {
        value = trie_ايندڙ(iter, script, 0);
        إذا (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } وإلا {
            retainScript = سچ;
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_ايندڙ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    منطقي ميلاپ_Region = خطأ;
    إذا (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } وإلا {
        value = trie_ايندڙ(iter, region, 0);
        إذا (value >= 0) {
            إذا (!region.empty() && !isMacroregion(region, error_ڪوڊ)) {
                retainRegion = سچ;
                ميلاپ_Region = سچ;
            }
        } وإلا {
            retainRegion = سچ;
            إذا (state == 0) {
                value = default_Lsr_اشاريو;
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_ايندڙ(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                إذا (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_منزل_State64(trieUndState);  // "und" ("*")
                    value = trie_ايندڙ(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_خالي_جايون_State = iter.getState64();
                    value = trie_ايندڙ(iter, region, 0);
                    // Fallback to und =>
                    إذا (value < 0) {
                        iter.reset_منزل_State64(trie_Und_خالي_جايون_State);
                        value = trie_ايندڙ(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_ڊيگهائي);
    إذا (return_داخلا_If_Unmatch &&
        (!(ميلاپ_Language || ميلاپ_Script || (ميلاپ_Region && language.empty())))) {
      أرجع LSR("", "", "", LSR::EXPLICIT_LSR, error_ڪوڊ);  // no matching.
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
    أرجع LSR(language, script, region, retain_نقاب, error_ڪوڊ);
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
        int32_t اشاريو;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            اشاريو = likelyInfo >> 2;
        } وإلا {
            اشاريو = get_Likely_اشاريو(lsr.language, "");
            likelyInfo = اشاريو << 2;
        }
        ثابت LSR &likely = lsrs[اشاريو];
        إذا (uprv_strcmp(lsr.script, likely.script) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    إذا (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t اشاريو;
        إذا (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            اشاريو = likelyInfo >> 2;
        } وإلا {
            اشاريو = get_Likely_اشاريو(lsr.language, lsr.region);
            likelyInfo = (اشاريو << 2) | 2;
        }
        ثابت LSR &likely = lsrs[اشاريو];
        إذا (uprv_strcmp(lsr.region, likely.region) == 0) {
            أرجع likelyInfo | 1;
        } وإلا {
            أرجع likelyInfo & ~1;
        }
    }
    أرجع likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_اشاريو(ثابت حرف *language, ثابت حرف *script) ثابت {
    إذا (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    إذا (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    بائيٽون_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    إذا (0 <= (c0 = uprv_هيٺاهون_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_پهريون_Letter_حالتون[c0]) != 0) {
        value = trie_ايندڙ(iter.reset_منزل_State64(state), language, 1);
    } وإلا {
        value = trie_ايندڙ(iter, language, 0);
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
        value = trie_ايندڙ(iter, script, 0);
        إذا (value >= 0) {
            state = iter.getState64();
        } وإلا {
            إذا (state == 0) {
                iter.reset_منزل_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } وإلا {
                iter.reset_منزل_State64(state);
                value = trie_ايندڙ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    إذا (value > 0) {
        // Final value from just language or language+script.
    } وإلا {
        value = trie_ايندڙ(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_ڊيگهائي);
    أرجع value;
}

int32_t LikelySubtags::trie_ايندڙ(بائيٽون_Trie &iter, ثابت حرف *s, int32_t i) {
    UString_Trie_نتيجو نتيجو;
    uint8_t c;
    إذا ((c = s[i]) == 0) {
        نتيجو = iter.next(u'*');
    } وإلا {
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            إذا (next != 0) {
                إذا (!USTRINGTRIE_HAS_ايندڙ(iter.next(c))) {
                    أرجع -1;
                }
            } وإلا {
                // last character of this subtag
                نتيجو = iter.next(c | 0x80);
                اكسر;
            }
            c = next;
        }
    }
    بدل (نتيجو) {
    حالة USTRINGTRIE_NO_ميلاپ: أرجع -1;
    حالة USTRINGTRIE_NO_قدر: أرجع 0;
    حالة USTRINGTRIE_INTERMEDIATE_قدر:
        U_ASSERT(iter.get_قدر() == SKIP_SCRIPT);
        أرجع SKIP_SCRIPT;
    حالة USTRINGTRIE_FINAL_قدر: أرجع iter.get_قدر();
    افتراضي: أرجع -1;
    }
}
int32_t LikelySubtags::trie_ايندڙ(بائيٽون_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_نتيجو نتيجو;
    uint8_t c;
    إذا (s.length() == i) {
        نتيجو = iter.next(u'*');
    } وإلا {
        c = s.معلومات()[i];
        لكل (;;) {
            c = uprv_inv_Char_منزل_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            إذا (i+1 != s.length()) {
                إذا (!USTRINGTRIE_HAS_ايندڙ(iter.next(c))) {
                    أرجع -1;
                }
                c = s.معلومات()[++i];
            } وإلا {
                // last character of this subtag
                نتيجو = iter.next(c | 0x80);
                اكسر;
            }
        }
    }
    بدل (نتيجو) {
    حالة USTRINGTRIE_NO_ميلاپ: أرجع -1;
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
                                    UError_ڪوڊ &error_ڪوڊ) ثابت {
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    LSR max = maximize(language, script, region, سچ, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    // If no match, return it.
    إذا (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        أرجع LSR(language, script, region, LSR::EXPLICIT_LSR, error_ڪوڊ);
    }
    // try language
    LSR test = maximize(max.language, "", "", سچ, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_ڪوڊ);
    }

    إذا (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ڪوڊ);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", سچ, error_ڪوڊ);
    إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
    إذا (test.is_Equivalent_منزل(max)) {
        أرجع LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_ڪوڊ);
    }
    إذا (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, سچ, error_ڪوڊ);
        إذا (U_FAILURE(error_ڪوڊ)) { أرجع {}; }
        إذا (test.is_Equivalent_منزل(max)) {
            أرجع LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ڪوڊ);
        }
    }
    أرجع LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_ڪوڊ);
}

U_NAMESPACE_پڇاڙي
