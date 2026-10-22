// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#అన్తర్భావ <utility>
#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/bytestrie.h"
#అన్తర్భావ "unicode/localpointer.h"
#అన్తర్భావ "unicode/locid.h"
#అన్తర్భావ "unicode/uobject.h"
#అన్తర్భావ "unicode/ures.h"
#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "charstr.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "loclikelysubtags.h"
#అన్తర్భావ "lsr.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "uhash.h"
#అన్తర్భావ "uinvchar.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "uniquecharstr.h"
#అన్తర్భావ "uresdata.h"
#అన్తర్భావ "uresimp.h"
#అన్తర్భావ "uvector.h"

U_NAMESPACE_BEGIN

నామాకాశ {

constexpr అక్షరం PSEUDO_ACCENTS_ఉపసర్గ = '\'';  // -XA, -PSACCENT
constexpr అక్షరం PSEUDO_BIDI_ఉపసర్గ = '+';  // -XB, -PSBIDI
constexpr అక్షరం PSEUDO_CRACKED_ఉపసర్గ = ',';  // -XC, -PSCRACK

}  // namespace

Locale_దూరం_దత్తాంశం::Locale_దూరం_దత్తాంశం(Locale_దూరం_దత్తాంశం &&దత్తాంశం) :
        దూరం_Trie_బైట్లు(దత్తాంశం.దూరం_Trie_బైట్లు),
        region_గమ్యం_Partitions(దత్తాంశం.region_గమ్యం_Partitions),
        partitions(దత్తాంశం.partitions),
        paradigms(దత్తాంశం.paradigms), paradigms_నిడివి(దత్తాంశం.paradigms_నిడివి),
        distances(దత్తాంశం.distances) {
    దత్తాంశం.partitions = శూన్యసూచిక;
    దత్తాంశం.paradigms = శూన్యసూచిక;
}

Locale_దూరం_దత్తాంశం::~Locale_దూరం_దత్తాంశం() {
    uprv_free(partitions);
    తొలగించు[] paradigms;
}

నిర్మాణం Likely_Subtags_దత్తాంశం {
    UResourceBundle *langInfoBundle = శూన్యసూచిక;
    Unique_Char_అక్షరశ్రేణులు అక్షరశ్రేణులు;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    స్థిరరాశి uint8_t *trie_బైట్లు = శూన్యసూచిక;
    LSR *lsrs = శూన్యసూచిక;
    int32_t lsrs_నిడివి = 0;

    Locale_దూరం_దత్తాంశం దూరం_దత్తాంశం;

    Likely_Subtags_దత్తాంశం(UError_సంకేతం &error_సంకేతం) : అక్షరశ్రేణులు(error_సంకేతం) {}

    ~Likely_Subtags_దత్తాంశం() {
        ures_close(langInfoBundle);
        తొలగించు[] lsrs;
    }

    శూన్య load(UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        langInfoBundle = ures_open_ప్రత్యక్ష(శూన్యసూచిక, "langInfo", &error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
        దొంతర_UResource_Bundle దొంతర_తాత్కాలిక_Bundle;
        Resource_దత్తాంశం_విలువ value;
        ures_get_విలువ_With_Fallback(langInfoBundle, "likely", దొంతర_తాత్కాలిక_Bundle.getAlias(),
                                  value, error_సంకేతం);
        Resource_పట్టిక likely_పట్టిక = value.get_పట్టిక(error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_జ్ఞాపకం<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_నిడివి = 0, regions_నిడివి = 0, lsr_Subtags_నిడివి = 0;
        ResourceArray m49Array;
        ఒకవేళ (likely_పట్టిక.కనుగొను_విలువ("m49", value)) {
            m49Array = value.getArray(error_సంకేతం);
        } లేకపోతే {
            error_సంకేతం = U_MISSING_RESOURCE_ERROR;
            తిరిగి;
        }
        ఒకవేళ (!read_అక్షరశ్రేణులు(likely_పట్టిక, "languageAliases", value,
                         languageIndexes, languages_నిడివి, error_సంకేతం) ||
                !read_అక్షరశ్రేణులు(likely_పట్టిక, "regionAliases", value,
                             regionIndexes, regions_నిడివి, error_సంకేతం) ||
                !read_LSREncoded_అక్షరశ్రేణులు(likely_పట్టిక, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_నిడివి, error_సంకేతం)) {
            తిరిగి;
        }
        ఒకవేళ ((languages_నిడివి & 1) != 0 ||
                (regions_నిడివి & 1) != 0 ||
                (lsr_Subtags_నిడివి % 3) != 0) {
            error_సంకేతం = U_INVALID_FORMAT_ERROR;
            తిరిగి;
        }
        ఒకవేళ (lsr_Subtags_నిడివి == 0) {
            error_సంకేతం = U_MISSING_RESOURCE_ERROR;
            తిరిగి;
        }

        ఒకవేళ (!likely_పట్టిక.కనుగొను_విలువ("trie", value)) {
            error_సంకేతం = U_MISSING_RESOURCE_ERROR;
            తిరిగి;
        }
        int32_t length;
        trie_బైట్లు = value.get_ద్విమాన(length, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_సంకేతం సరిపోలిక_Error_సంకేతం = U_ZERO_ERROR;
        ures_get_విలువ_With_Fallback(langInfoBundle, "match", దొంతర_తాత్కాలిక_Bundle.getAlias(),
                                  value, సరిపోలిక_Error_సంకేతం);
        Local_జ్ఞాపకం<int32_t> విభజన_Indexes, paradigmSubtagIndexes;
        int32_t partitions_నిడివి = 0, paradigm_Subtags_నిడివి = 0;
        ఒకవేళ (U_SUCCESS(సరిపోలిక_Error_సంకేతం)) {
            Resource_పట్టిక సరిపోలిక_పట్టిక = value.get_పట్టిక(error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }

            ఒకవేళ (సరిపోలిక_పట్టిక.కనుగొను_విలువ("trie", value)) {
                దూరం_దత్తాంశం.దూరం_Trie_బైట్లు = value.get_ద్విమాన(length, error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
            }

            ఒకవేళ (సరిపోలిక_పట్టిక.కనుగొను_విలువ("regionToPartitions", value)) {
                దూరం_దత్తాంశం.region_గమ్యం_Partitions = value.get_ద్విమాన(length, error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
                ఒకవేళ (length < LSR::REGION_సూచీ_హద్దు) {
                    error_సంకేతం = U_INVALID_FORMAT_ERROR;
                    తిరిగి;
                }
            }

            ఒకవేళ (!read_అక్షరశ్రేణులు(సరిపోలిక_పట్టిక, "partitions", value,
                             విభజన_Indexes, partitions_నిడివి, error_సంకేతం) ||
                    !read_LSREncoded_అక్షరశ్రేణులు(సరిపోలిక_పట్టిక, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_నిడివి, error_సంకేతం)) {
                తిరిగి;
            }
            ఒకవేళ ((paradigm_Subtags_నిడివి % 3) != 0) {
                error_సంకేతం = U_INVALID_FORMAT_ERROR;
                తిరిగి;
            }

            ఒకవేళ (సరిపోలిక_పట్టిక.కనుగొను_విలువ("distances", value)) {
                దూరం_దత్తాంశం.distances = value.get_Int_సదిశ(length, error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
                ఒకవేళ (length < 4) {  // LocaleDistance IX_LIMIT
                    error_సంకేతం = U_INVALID_FORMAT_ERROR;
                    తిరిగి;
                }
            }
        } లేకపోతే ఒకవేళ (సరిపోలిక_Error_సంకేతం == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } లేకపోతే {  // error other than missing resource
            error_సంకేతం = సరిపోలిక_Error_సంకేతం;
            తిరిగి;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        అక్షరశ్రేణులు.freeze();

        languageAliases = CharStringMap(languages_నిడివి / 2, error_సంకేతం);
        కొరకు (int32_t i = 0; i < languages_నిడివి; i += 2) {
            languageAliases.put(అక్షరశ్రేణులు.get(languageIndexes[i]),
                                అక్షరశ్రేణులు.get(languageIndexes[i + 1]), error_సంకేతం);
        }

        regionAliases = CharStringMap(regions_నిడివి / 2, error_సంకేతం);
        కొరకు (int32_t i = 0; i < regions_నిడివి; i += 2) {
            regionAliases.put(అక్షరశ్రేణులు.get(regionIndexes[i]),
                              అక్షరశ్రేణులు.get(regionIndexes[i + 1]), error_సంకేతం);
        }
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }

        lsrs_నిడివి = lsr_Subtags_నిడివి / 3;
        lsrs = కొత్త LSR[lsrs_నిడివి];
        ఒకవేళ (lsrs == శూన్యసూచిక) {
            error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి;
        }
        కొరకు (int32_t i = 0, j = 0; i < lsr_Subtags_నిడివి; i += 3, ++j) {
            lsrs[j] = LSR(అక్షరశ్రేణులు.get(lsrSubtagIndexes[i]),
                          అక్షరశ్రేణులు.get(lsrSubtagIndexes[i + 1]),
                          అక్షరశ్రేణులు.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ఒకవేళ (partitions_నిడివి > 0) {
            దూరం_దత్తాంశం.partitions = static_cast<స్థిరరాశి అక్షరం **>(
                uprv_malloc(partitions_నిడివి * పరిమాణం(స్థిరరాశి అక్షరం *)));
            ఒకవేళ (దూరం_దత్తాంశం.partitions == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            కొరకు (int32_t i = 0; i < partitions_నిడివి; ++i) {
                దూరం_దత్తాంశం.partitions[i] = అక్షరశ్రేణులు.get(విభజన_Indexes[i]);
            }
        }

        ఒకవేళ (paradigm_Subtags_నిడివి > 0) {
            దూరం_దత్తాంశం.paradigms_నిడివి = paradigm_Subtags_నిడివి / 3;
            LSR *paradigms = కొత్త LSR[దూరం_దత్తాంశం.paradigms_నిడివి];
            ఒకవేళ (paradigms == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి;
            }
            కొరకు (int32_t i = 0, j = 0; i < paradigm_Subtags_నిడివి; i += 3, ++j) {
                paradigms[j] = LSR(అక్షరశ్రేణులు.get(paradigmSubtagIndexes[i]),
                                   అక్షరశ్రేణులు.get(paradigmSubtagIndexes[i + 1]),
                                   అక్షరశ్రేణులు.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            దూరం_దత్తాంశం.paradigms = paradigms;
        }
    }

అంతరంగ:
    తార్కిక read_అక్షరశ్రేణులు(స్థిరరాశి Resource_పట్టిక &పట్టిక, స్థిరరాశి అక్షరం *కీలకం, Resource_విలువ &value,
                     Local_జ్ఞాపకం<int32_t> &indexes, int32_t &length, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
        ఒకవేళ (పట్టిక.కనుగొను_విలువ(కీలకం, value)) {
            ResourceArray stringArray = value.getArray(error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
            length = stringArray.get_కొలత();
            ఒకవేళ (length == 0) { తిరిగి నిజం; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ఒకవేళ (rawIndexes == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి అబద్ధం;
            }
            కొరకు (పూర్ణాంకం i = 0; i < length; ++i) {
                ఒకవేళ (stringArray.get_విలువ(i, value)) {  // returns true because i < length
                    int32_t str_నిడివి = 0;
                    rawIndexes[i] = అక్షరశ్రేణులు.కూడు(value.getString(str_నిడివి, error_సంకేతం), error_సంకేతం);
                    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
                }
            }
        }
        తిరిగి నిజం;
    }
    UnicodeString గమ్యం_Language(పూర్ణాంకం encoded) {
        ఒకవేళ (encoded == 0) {
            తిరిగి UNICODE_STRING_SIMPLE("");
        }
        ఒకవేళ (encoded == 1) {
            తిరిగి UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        అక్షరం lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ఒకవేళ (encoded / (27 * 27) == 0) {
            తిరిగి UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        తిరిగి UnicodeString(lang, 3, US_INV);
    }
    UnicodeString గమ్యం_Script(పూర్ణాంకం encoded) {
        ఒకవేళ (encoded == 0) {
            తిరిగి UNICODE_STRING_SIMPLE("");
        }
        ఒకవేళ (encoded == 1) {
            తిరిగి UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        స్థిరరాశి అక్షరం* script = uscript_getShortName(static_cast<UScript_సంకేతం>(encoded));
        ఒకవేళ (script == శూన్యసూచిక) {
            తిరిగి UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        తిరిగి UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_సూచీ_గమ్యం_సంకేతం(స్థిరరాశి ResourceArray &m49Array, Resource_విలువ &value, పూర్ణాంకం సూచీ, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) {
            తిరిగి UNICODE_STRING_SIMPLE("");
        }
        ఒకవేళ (m49Array.get_విలువ(సూచీ, value)) {
            తిరిగి value.getUnicodeString(error_సంకేతం);
        }
        // "m49" does not include the index.
        error_సంకేతం = U_MISSING_RESOURCE_ERROR;
        తిరిగి UNICODE_STRING_SIMPLE("");
    }

    UnicodeString గమ్యం_Region(స్థిరరాశి ResourceArray& m49Array, Resource_విలువ &value, పూర్ణాంకం encoded, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం) || encoded == 0 || encoded == 1) {
            తిరిగి UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ఒకవేళ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            తిరిగి  m49_సూచీ_గమ్యం_సంకేతం(m49Array, value, encoded, error_సంకేతం);
        }
        అక్షరం region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        తిరిగి UnicodeString(region, 2, US_INV);
    }

    తార్కిక read_LSREncoded_అక్షరశ్రేణులు(స్థిరరాశి Resource_పట్టిక &పట్టిక, స్థిరరాశి అక్షరం* కీలకం, Resource_విలువ &value, స్థిరరాశి ResourceArray& m49Array,
                     Local_జ్ఞాపకం<int32_t> &indexes, int32_t &length, UError_సంకేతం &error_సంకేతం) {
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
        ఒకవేళ (పట్టిక.కనుగొను_విలువ(కీలకం, value)) {
            స్థిరరాశి int32_t* vectors = value.get_Int_సదిశ(length, error_సంకేతం);
            ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
            ఒకవేళ (length == 0) { తిరిగి నిజం; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ఒకవేళ (rawIndexes == శూన్యసూచిక) {
                error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
                తిరిగి అబద్ధం;
            }
            కొరకు (పూర్ణాంకం i = 0; i < length; ++i) {
                rawIndexes[i*3] = అక్షరశ్రేణులు.కూడు_By_విలువ(గమ్యం_Language(vectors[i]), error_సంకేతం);
                rawIndexes[i*3+1] = అక్షరశ్రేణులు.కూడు_By_విలువ(గమ్యం_Script(vectors[i]), error_సంకేతం);
                rawIndexes[i*3+2] = అక్షరశ్రేణులు.కూడు_By_విలువ(
                    గమ్యం_Region(m49Array, value, vectors[i], error_సంకేతం), error_సంకేతం);
                ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
            }
            length *= 3;
        }
        తిరిగి నిజం;
    }
};

నామాకాశ {

LikelySubtags *gLikelySubtags = శూన్యసూచిక;
UVector *gMacroregions = శూన్యసూచిక;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    తొలగించు gLikelySubtags;
    gLikelySubtags = శూన్యసూచిక;
    తొలగించు gMacroregions;
    gMacroregions = శూన్యసూచిక;
    gInitOnce.reset();
    తిరిగి నిజం;
}

constexpr స్థిరరాశి char16_t* MACROREGION_HARDCODE[] = {
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
శూన్య processMacroregionRange(స్థిరరాశి UnicodeString& regionName, UVector* newMacroRegions, UError_సంకేతం& స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి; }
    int32_t rangeMarkerLocation = regionName.సూచీ_యొక్క(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,స్థితి_2);
    ఒకవేళ ( rangeMarkerLocation > 0 ) {
        char16_t చివర_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        అయితే ( buf[rangeMarkerLocation-1] <= చివర_Range && U_SUCCESS(స్థితి_2)) {
            Local_సూచిక<UnicodeString> newRegion(కొత్త UnicodeString(buf), స్థితి_2);
            newMacroRegions->adoptElement(newRegion.orphan(),స్థితి_2);
            buf[rangeMarkerLocation-1]++;
        }
    } లేకపోతే {
        Local_సూచిక<UnicodeString> newRegion(కొత్త UnicodeString(regionName), స్థితి_2);
        newMacroRegions->adoptElement(newRegion.orphan(),స్థితి_2);
    }
}

#ఒకవేళ U_DEBUG
UVector* ఎక్కించు_Macroregions(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి శూన్యసూచిక; }
    Local_సూచిక<UVector> newMacroRegions(కొత్త UVector(uprv_deleteUObject, uhash_compareUnicodeString, స్థితి_2), స్థితి_2);

    Local_UResource_Bundle_సూచిక supplemental_దత్తాంశం(ures_open_ప్రత్యక్ష(శూన్యసూచిక,"supplementalData",&స్థితి_2));
    Local_UResource_Bundle_సూచిక idValidity(ures_get_By_కీలకం(supplemental_దత్తాంశం.getAlias(),"idValidity",శూన్యసూచిక,&స్థితి_2));
    Local_UResource_Bundle_సూచిక regionList(ures_get_By_కీలకం(idValidity.getAlias(),"region",శూన్యసూచిక,&స్థితి_2));
    Local_UResource_Bundle_సూచిక regionMacro(ures_get_By_కీలకం(regionList.getAlias(),"macroregion",శూన్యసూచిక,&స్థితి_2));

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    అయితే (ures_has_తదుపరి(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_తదుపరి_Unicode_String(regionMacro.getAlias(),శూన్యసూచిక,&స్థితి_2);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
    }

    తిరిగి newMacroRegions.orphan();
}
#అయితే_ముగింపు // U_DEBUG

UVector* getStaticMacroregions(UError_సంకేతం &స్థితి_2) {
    ఒకవేళ (U_FAILURE(స్థితి_2)) { తిరిగి శూన్యసూచిక; }
    Local_సూచిక<UVector> newMacroRegions(కొత్త UVector(uprv_deleteUObject, uhash_compareUnicodeString, స్థితి_2), స్థితి_2);

    ఒకవేళ (U_FAILURE(స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }

    కొరకు (స్థిరరాశి స్వయంచాలక *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), స్థితి_2);
        ఒకవేళ (U_FAILURE(స్థితి_2)) {
            తిరిగి శూన్యసూచిక;
        }
    }

    తిరిగి newMacroRegions.orphan();
}

}  // namespace

శూన్య U_CALLCONV LikelySubtags::initLikelySubtags(UError_సంకేతం &error_సంకేతం) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == శూన్యసూచిక);
    Likely_Subtags_దత్తాంశం దత్తాంశం(error_సంకేతం);
    దత్తాంశం.load(error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి; }
    gLikelySubtags = కొత్త LikelySubtags(దత్తాంశం);
    gMacroregions = getStaticMacroregions(error_సంకేతం);
#ఒకవేళ U_DEBUG
    స్వయంచాలక macroregions_From_దత్తాంశం = ఎక్కించు_Macroregions(error_సంకేతం);
    U_ASSERT((*gMacroregions) == (*macroregions_From_దత్తాంశం));
    తొలగించు macroregions_From_దత్తాంశం;
#అయితే_ముగింపు
    ఒకవేళ (U_FAILURE(error_సంకేతం) || gLikelySubtags == శూన్యసూచిక || gMacroregions == శూన్యసూచిక) {
        తొలగించు gLikelySubtags;
        తొలగించు gMacroregions;
        error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }

    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

స్థిరరాశి LikelySubtags *LikelySubtags::getSingleton(UError_సంకేతం &error_సంకేతం) {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి శూన్యసూచిక; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_సంకేతం);
    తిరిగి gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_దత్తాంశం &దత్తాంశం) :
        langInfoBundle(దత్తాంశం.langInfoBundle),
        అక్షరశ్రేణులు(దత్తాంశం.అక్షరశ్రేణులు.orphan_Char_అక్షరశ్రేణులు()),
        languageAliases(std::move(దత్తాంశం.languageAliases)),
        regionAliases(std::move(దత్తాంశం.regionAliases)),
        trie(దత్తాంశం.trie_బైట్లు),
        lsrs(దత్తాంశం.lsrs),
#ఒకవేళ U_DEBUG
        lsrs_నిడివి(దత్తాంశం.lsrs_నిడివి),
#అయితే_ముగింపు // U_DEBUG
        దూరం_దత్తాంశం(std::move(దత్తాంశం.దూరం_దత్తాంశం)) {
    దత్తాంశం.langInfoBundle = శూన్యసూచిక;
    దత్తాంశం.lsrs = శూన్యసూచిక;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_ఫలితం ఫలితం = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_తదుపరి(ఫలితం));
    trieUndState = trie.getState64();
    ఫలితం = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_తదుపరి(ఫలితం));
    trieUndZzzzState = trie.getState64();
    ఫలితం = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_విలువ(ఫలితం));
    default_Lsr_సూచీ = trie.get_విలువ();
    trie.reset();

    కొరకు (char16_t c = u'a'; c <= u'z'; ++c) {
        ఫలితం = trie.next(c);
        ఒకవేళ (ఫలితం == USTRINGTRIE_NO_విలువ) {
            trie_మొదటి_Letter_స్థితులు[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    తొలగించు అక్షరశ్రేణులు;
    తొలగించు[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(స్థిరరాశి Locale &locale,
                                         తార్కిక return_ఆదానం_If_Unmatch,
                                         UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    ఒకవేళ (locale.isBogus()) {
        error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి {};
    }
    స్థిరరాశి అక్షరం *name = locale.getName();
    ఒకవేళ (!return_ఆదానం_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        తిరిగి LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ఆదానం_If_Unmatch, error_సంకేతం);

    ఒకవేళ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        తిరిగి LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_సంకేతం);
    }
    తిరిగి max;
}

నామాకాశ {

స్థిరరాశి అక్షరం *getCanonical(స్థిరరాశి CharStringMap &aliases, స్థిరరాశి అక్షరం *alias) {
    స్థిరరాశి అక్షరం *canonical = aliases.get(alias);
    తిరిగి canonical == శూన్యసూచిక ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script, స్థిరరాశి అక్షరం *region,
                                     స్థిరరాశి అక్షరం *variant,
                                     తార్కిక return_ఆదానం_If_Unmatch,
                                     UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ఒకవేళ (!return_ఆదానం_If_Unmatch) {
        అక్షరం c1;
        ఒకవేళ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            ఎంపిక (c1) {
            స్థితి 'A':
                తిరిగి LSR(PSEUDO_ACCENTS_ఉపసర్గ, language, script, region,
                           LSR::EXPLICIT_LSR, error_సంకేతం);
            స్థితి 'B':
                తిరిగి LSR(PSEUDO_BIDI_ఉపసర్గ, language, script, region,
                           LSR::EXPLICIT_LSR, error_సంకేతం);
            స్థితి 'C':
                తిరిగి LSR(PSEUDO_CRACKED_ఉపసర్గ, language, script, region,
                           LSR::EXPLICIT_LSR, error_సంకేతం);
            అప్రమేయ:  // normal locale
                ఆపు;
            }
        }

        ఒకవేళ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ఒకవేళ (uprv_strcmp(variant, "PSACCENT") == 0) {
                తిరిగి LSR(PSEUDO_ACCENTS_ఉపసర్గ, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_సంకేతం);
            } లేకపోతే ఒకవేళ (uprv_strcmp(variant, "PSBIDI") == 0) {
                తిరిగి LSR(PSEUDO_BIDI_ఉపసర్గ, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_సంకేతం);
            } లేకపోతే ఒకవేళ (uprv_strcmp(variant, "PSCRACK") == 0) {
                తిరిగి LSR(PSEUDO_CRACKED_ఉపసర్గ, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_సంకేతం);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    తిరిగి maximize(language, script, region, return_ఆదానం_If_Unmatch, error_సంకేతం);
}

LSR LikelySubtags::maximize(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script, స్థిరరాశి అక్షరం *region,
                             తార్కిక return_ఆదానం_If_Unmatch,
                             UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    తిరిగి maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ఆదానం_If_Unmatch,
                    error_సంకేతం);
}

తార్కిక LikelySubtags::isMacroregion(StringPiece& region, UError_సంకేతం& error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి అబద్ధం; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    తిరిగి gMacroregions->కలిగియుంది((శూన్య *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             తార్కిక return_ఆదానం_If_Unmatch,
                             UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    ఒకవేళ (language.compare("und") == 0) {
        language = "";
    }
    ఒకవేళ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ఒకవేళ (region.compare("ZZ") == 0) {
        region = "";
    }
    ఒకవేళ (!script.empty() && !region.empty() && !language.empty()) {
        తిరిగి LSR(language, script, region, LSR::EXPLICIT_LSR, error_సంకేతం);  // already maximized
    }
    తార్కిక retainLanguage = అబద్ధం;
    తార్కిక retainScript = అబద్ధం;
    తార్కిక retainRegion = అబద్ధం;

    బైట్లు_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ఒకవేళ (0 <= (c0 = uprv_క్రింది_Ordinal(language.దత్తాంశం()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_మొదటి_Letter_స్థితులు[c0]) != 0) {
        value = trie_తదుపరి(iter.reset_గమ్యం_State64(state), language, 1);
    } లేకపోతే {
        value = trie_తదుపరి(iter, language, 0);
    }
    తార్కిక సరిపోలిక_Language = (value >= 0);
    తార్కిక సరిపోలిక_Script = అబద్ధం;
    ఒకవేళ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } లేకపోతే {
        retainLanguage = నిజం;
        iter.reset_గమ్యం_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ఒకవేళ (value >= 0 && !script.empty()) {
        సరిపోలిక_Script = నిజం;
    }
    ఒకవేళ (value > 0) {
        // Intermediate or final value from just language.
        ఒకవేళ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } లేకపోతే {
        value = trie_తదుపరి(iter, script, 0);
        ఒకవేళ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } లేకపోతే {
            retainScript = నిజం;
            ఒకవేళ (state == 0) {
                iter.reset_గమ్యం_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } లేకపోతే {
                iter.reset_గమ్యం_State64(state);
                value = trie_తదుపరి(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    తార్కిక సరిపోలిక_Region = అబద్ధం;
    ఒకవేళ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } లేకపోతే {
        value = trie_తదుపరి(iter, region, 0);
        ఒకవేళ (value >= 0) {
            ఒకవేళ (!region.empty() && !isMacroregion(region, error_సంకేతం)) {
                retainRegion = నిజం;
                సరిపోలిక_Region = నిజం;
            }
        } లేకపోతే {
            retainRegion = నిజం;
            ఒకవేళ (state == 0) {
                value = default_Lsr_సూచీ;
            } లేకపోతే {
                iter.reset_గమ్యం_State64(state);
                value = trie_తదుపరి(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ఒకవేళ (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_గమ్యం_State64(trieUndState);  // "und" ("*")
                    value = trie_తదుపరి(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_ఖాళీలు_State = iter.getState64();
                    value = trie_తదుపరి(iter, region, 0);
                    // Fallback to und =>
                    ఒకవేళ (value < 0) {
                        iter.reset_గమ్యం_State64(trie_Und_ఖాళీలు_State);
                        value = trie_తదుపరి(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_నిడివి);
    ఒకవేళ (return_ఆదానం_If_Unmatch &&
        (!(సరిపోలిక_Language || సరిపోలిక_Script || (సరిపోలిక_Region && language.empty())))) {
      తిరిగి LSR("", "", "", LSR::EXPLICIT_LSR, error_సంకేతం);  // no matching.
    }
    ఒకవేళ (language.empty()) {
        language = StringPiece("und");
    }

    ఒకవేళ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        స్థిరరాశి LSR &matched = lsrs[value];
        తిరిగి LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ఒకవేళ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ఒకవేళ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ఒకవేళ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_ముసుగు = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    తిరిగి LSR(language, script, region, retain_ముసుగు, error_సంకేతం);
}

int32_t LikelySubtags::compareLikely(స్థిరరాశి LSR &lsr, స్థిరరాశి LSR &other, int32_t likelyInfo) స్థిరరాశి {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ఒకవేళ (uprv_strcmp(lsr.language, other.language) != 0) {
        తిరిగి 0xfffffffc;  // negative, lsr not better than other
    }
    ఒకవేళ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t సూచీ;
        ఒకవేళ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            సూచీ = likelyInfo >> 2;
        } లేకపోతే {
            సూచీ = get_Likely_సూచీ(lsr.language, "");
            likelyInfo = సూచీ << 2;
        }
        స్థిరరాశి LSR &likely = lsrs[సూచీ];
        ఒకవేళ (uprv_strcmp(lsr.script, likely.script) == 0) {
            తిరిగి likelyInfo | 1;
        } లేకపోతే {
            తిరిగి likelyInfo & ~1;
        }
    }
    ఒకవేళ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t సూచీ;
        ఒకవేళ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            సూచీ = likelyInfo >> 2;
        } లేకపోతే {
            సూచీ = get_Likely_సూచీ(lsr.language, lsr.region);
            likelyInfo = (సూచీ << 2) | 2;
        }
        స్థిరరాశి LSR &likely = lsrs[సూచీ];
        ఒకవేళ (uprv_strcmp(lsr.region, likely.region) == 0) {
            తిరిగి likelyInfo | 1;
        } లేకపోతే {
            తిరిగి likelyInfo & ~1;
        }
    }
    తిరిగి likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_సూచీ(స్థిరరాశి అక్షరం *language, స్థిరరాశి అక్షరం *script) స్థిరరాశి {
    ఒకవేళ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ఒకవేళ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    బైట్లు_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ఒకవేళ (0 <= (c0 = uprv_క్రింది_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_మొదటి_Letter_స్థితులు[c0]) != 0) {
        value = trie_తదుపరి(iter.reset_గమ్యం_State64(state), language, 1);
    } లేకపోతే {
        value = trie_తదుపరి(iter, language, 0);
    }
    ఒకవేళ (value >= 0) {
        state = iter.getState64();
    } లేకపోతే {
        iter.reset_గమ్యం_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ఒకవేళ (value > 0) {
        // Intermediate or final value from just language.
        ఒకవేళ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } లేకపోతే {
        value = trie_తదుపరి(iter, script, 0);
        ఒకవేళ (value >= 0) {
            state = iter.getState64();
        } లేకపోతే {
            ఒకవేళ (state == 0) {
                iter.reset_గమ్యం_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } లేకపోతే {
                iter.reset_గమ్యం_State64(state);
                value = trie_తదుపరి(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ఒకవేళ (value > 0) {
        // Final value from just language or language+script.
    } లేకపోతే {
        value = trie_తదుపరి(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_నిడివి);
    తిరిగి value;
}

int32_t LikelySubtags::trie_తదుపరి(బైట్లు_Trie &iter, స్థిరరాశి అక్షరం *s, int32_t i) {
    UString_Trie_ఫలితం ఫలితం;
    uint8_t c;
    ఒకవేళ ((c = s[i]) == 0) {
        ఫలితం = iter.next(u'*');
    } లేకపోతే {
        కొరకు (;;) {
            c = uprv_inv_Char_గమ్యం_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ఒకవేళ (next != 0) {
                ఒకవేళ (!USTRINGTRIE_HAS_తదుపరి(iter.next(c))) {
                    తిరిగి -1;
                }
            } లేకపోతే {
                // last character of this subtag
                ఫలితం = iter.next(c | 0x80);
                ఆపు;
            }
            c = next;
        }
    }
    ఎంపిక (ఫలితం) {
    స్థితి USTRINGTRIE_NO_సరిపోలిక: తిరిగి -1;
    స్థితి USTRINGTRIE_NO_విలువ: తిరిగి 0;
    స్థితి USTRINGTRIE_INTERMEDIATE_విలువ:
        U_ASSERT(iter.get_విలువ() == SKIP_SCRIPT);
        తిరిగి SKIP_SCRIPT;
    స్థితి USTRINGTRIE_FINAL_విలువ: తిరిగి iter.get_విలువ();
    అప్రమేయ: తిరిగి -1;
    }
}
int32_t LikelySubtags::trie_తదుపరి(బైట్లు_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_ఫలితం ఫలితం;
    uint8_t c;
    ఒకవేళ (s.length() == i) {
        ఫలితం = iter.next(u'*');
    } లేకపోతే {
        c = s.దత్తాంశం()[i];
        కొరకు (;;) {
            c = uprv_inv_Char_గమ్యం_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ఒకవేళ (i+1 != s.length()) {
                ఒకవేళ (!USTRINGTRIE_HAS_తదుపరి(iter.next(c))) {
                    తిరిగి -1;
                }
                c = s.దత్తాంశం()[++i];
            } లేకపోతే {
                // last character of this subtag
                ఫలితం = iter.next(c | 0x80);
                ఆపు;
            }
        }
    }
    ఎంపిక (ఫలితం) {
    స్థితి USTRINGTRIE_NO_సరిపోలిక: తిరిగి -1;
    స్థితి USTRINGTRIE_NO_విలువ: తిరిగి 0;
    స్థితి USTRINGTRIE_INTERMEDIATE_విలువ:
        U_ASSERT(iter.get_విలువ() == SKIP_SCRIPT);
        తిరిగి SKIP_SCRIPT;
    స్థితి USTRINGTRIE_FINAL_విలువ: తిరిగి iter.get_విలువ();
    అప్రమేయ: తిరిగి -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    తార్కిక favorScript,
                                    UError_సంకేతం &error_సంకేతం) స్థిరరాశి {
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    LSR max = maximize(language, script, region, నిజం, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    // If no match, return it.
    ఒకవేళ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        తిరిగి LSR(language, script, region, LSR::EXPLICIT_LSR, error_సంకేతం);
    }
    // try language
    LSR test = maximize(max.language, "", "", నిజం, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    ఒకవేళ (test.is_Equivalent_గమ్యం(max)) {
        తిరిగి LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_సంకేతం);
    }

    ఒకవేళ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, నిజం, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
        ఒకవేళ (test.is_Equivalent_గమ్యం(max)) {
            తిరిగి LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_సంకేతం);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", నిజం, error_సంకేతం);
    ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
    ఒకవేళ (test.is_Equivalent_గమ్యం(max)) {
        తిరిగి LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_సంకేతం);
    }
    ఒకవేళ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, నిజం, error_సంకేతం);
        ఒకవేళ (U_FAILURE(error_సంకేతం)) { తిరిగి {}; }
        ఒకవేళ (test.is_Equivalent_గమ్యం(max)) {
            తిరిగి LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_సంకేతం);
        }
    }
    తిరిగి LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_సంకేతం);
}

U_NAMESPACE_చివర
