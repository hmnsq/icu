// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#अन्तर्भाव <utility>
#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/bytestrie.h"
#अन्तर्भाव "unicode/localpointer.h"
#अन्तर्भाव "unicode/locid.h"
#अन्तर्भाव "unicode/uobject.h"
#अन्तर्भाव "unicode/ures.h"
#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "charstr.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "loclikelysubtags.h"
#अन्तर्भाव "lsr.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "uhash.h"
#अन्तर्भाव "uinvchar.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uniquecharstr.h"
#अन्तर्भाव "uresdata.h"
#अन्तर्भाव "uresimp.h"
#अन्तर्भाव "uvector.h"

U_NAMESPACE_BEGIN

नामाकाश {

constexpr वर्ण PSEUDO_ACCENTS_उपसर्गः = '\'';  // -XA, -PSACCENT
constexpr वर्ण PSEUDO_BIDI_उपसर्गः = '+';  // -XB, -PSBIDI
constexpr वर्ण PSEUDO_CRACKED_उपसर्गः = ',';  // -XC, -PSCRACK

}  // namespace

Locale_Distance_दत्त::Locale_Distance_दत्त(Locale_Distance_दत्त &&दत्त) :
        distance_Trie_अष्टकानि(दत्त.distance_Trie_अष्टकानि),
        region_गन्तव्यम्_Partitions(दत्त.region_गन्तव्यम्_Partitions),
        partitions(दत्त.partitions),
        paradigms(दत्त.paradigms), paradigms_दीर्घता(दत्त.paradigms_दीर्घता),
        distances(दत्त.distances) {
    दत्त.partitions = शून्यसूचकः;
    दत्त.paradigms = शून्यसूचकः;
}

Locale_Distance_दत्त::~Locale_Distance_दत्त() {
    uprv_free(partitions);
    विलोपय[] paradigms;
}

संरचना Likely_Subtags_दत्त {
    UResourceBundle *langInfoBundle = शून्यसूचकः;
    Unique_Char_वाक्यानि वाक्यानि;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ध्रुव uint8_t *trie_अष्टकानि = शून्यसूचकः;
    LSR *lsrs = शून्यसूचकः;
    int32_t lsrs_दीर्घता = 0;

    Locale_Distance_दत्त distance_दत्त;

    Likely_Subtags_दत्त(UError_संहिता &error_संहिता) : वाक्यानि(error_संहिता) {}

    ~Likely_Subtags_दत्त() {
        ures_close(langInfoBundle);
        विलोपय[] lsrs;
    }

    शून्य load(UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        langInfoBundle = ures_open_प्रत्यक्षम्(शून्यसूचकः, "langInfo", &error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
        राशिः_UResource_Bundle राशिः_अस्थायी_Bundle;
        Resource_दत्त_मानम् value;
        ures_get_मानम्_With_Fallback(langInfoBundle, "likely", राशिः_अस्थायी_Bundle.getAlias(),
                                  value, error_संहिता);
        Resource_सारणी likely_सारणी = value.get_सारणी(error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_स्मृतिः<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_दीर्घता = 0, regions_दीर्घता = 0, lsr_Subtags_दीर्घता = 0;
        ResourceArray m49Array;
        यदि (likely_सारणी.find_मानम्("m49", value)) {
            m49Array = value.getArray(error_संहिता);
        } अन्यथा {
            error_संहिता = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        यदि (!read_वाक्यानि(likely_सारणी, "languageAliases", value,
                         languageIndexes, languages_दीर्घता, error_संहिता) ||
                !read_वाक्यानि(likely_सारणी, "regionAliases", value,
                             regionIndexes, regions_दीर्घता, error_संहिता) ||
                !read_LSREncoded_वाक्यानि(likely_सारणी, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_दीर्घता, error_संहिता)) {
            प्रत्यागम;
        }
        यदि ((languages_दीर्घता & 1) != 0 ||
                (regions_दीर्घता & 1) != 0 ||
                (lsr_Subtags_दीर्घता % 3) != 0) {
            error_संहिता = U_INVALID_FORMAT_ERROR;
            प्रत्यागम;
        }
        यदि (lsr_Subtags_दीर्घता == 0) {
            error_संहिता = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }

        यदि (!likely_सारणी.find_मानम्("trie", value)) {
            error_संहिता = U_MISSING_RESOURCE_ERROR;
            प्रत्यागम;
        }
        int32_t length;
        trie_अष्टकानि = value.get_द्विमान(length, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_संहिता मेलनम्_Error_संहिता = U_ZERO_ERROR;
        ures_get_मानम्_With_Fallback(langInfoBundle, "match", राशिः_अस्थायी_Bundle.getAlias(),
                                  value, मेलनम्_Error_संहिता);
        Local_स्मृतिः<int32_t> partitionIndexes, paradigmSubtagIndexes;
        int32_t partitions_दीर्घता = 0, paradigm_Subtags_दीर्घता = 0;
        यदि (U_SUCCESS(मेलनम्_Error_संहिता)) {
            Resource_सारणी मेलनम्_सारणी = value.get_सारणी(error_संहिता);
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }

            यदि (मेलनम्_सारणी.find_मानम्("trie", value)) {
                distance_दत्त.distance_Trie_अष्टकानि = value.get_द्विमान(length, error_संहिता);
                यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
            }

            यदि (मेलनम्_सारणी.find_मानम्("regionToPartitions", value)) {
                distance_दत्त.region_गन्तव्यम्_Partitions = value.get_द्विमान(length, error_संहिता);
                यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
                यदि (length < LSR::REGION_सूचकांक_सीमा) {
                    error_संहिता = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }

            यदि (!read_वाक्यानि(मेलनम्_सारणी, "partitions", value,
                             partitionIndexes, partitions_दीर्घता, error_संहिता) ||
                    !read_LSREncoded_वाक्यानि(मेलनम्_सारणी, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_दीर्घता, error_संहिता)) {
                प्रत्यागम;
            }
            यदि ((paradigm_Subtags_दीर्घता % 3) != 0) {
                error_संहिता = U_INVALID_FORMAT_ERROR;
                प्रत्यागम;
            }

            यदि (मेलनम्_सारणी.find_मानम्("distances", value)) {
                distance_दत्त.distances = value.get_Int_सदिशम्(length, error_संहिता);
                यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
                यदि (length < 4) {  // LocaleDistance IX_LIMIT
                    error_संहिता = U_INVALID_FORMAT_ERROR;
                    प्रत्यागम;
                }
            }
        } अन्यथा यदि (मेलनम्_Error_संहिता == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } अन्यथा {  // error other than missing resource
            error_संहिता = मेलनम्_Error_संहिता;
            प्रत्यागम;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        वाक्यानि.freeze();

        languageAliases = CharStringMap(languages_दीर्घता / 2, error_संहिता);
        प्रति (int32_t i = 0; i < languages_दीर्घता; i += 2) {
            languageAliases.put(वाक्यानि.get(languageIndexes[i]),
                                वाक्यानि.get(languageIndexes[i + 1]), error_संहिता);
        }

        regionAliases = CharStringMap(regions_दीर्घता / 2, error_संहिता);
        प्रति (int32_t i = 0; i < regions_दीर्घता; i += 2) {
            regionAliases.put(वाक्यानि.get(regionIndexes[i]),
                              वाक्यानि.get(regionIndexes[i + 1]), error_संहिता);
        }
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }

        lsrs_दीर्घता = lsr_Subtags_दीर्घता / 3;
        lsrs = नूतन LSR[lsrs_दीर्घता];
        यदि (lsrs == शून्यसूचकः) {
            error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम;
        }
        प्रति (int32_t i = 0, j = 0; i < lsr_Subtags_दीर्घता; i += 3, ++j) {
            lsrs[j] = LSR(वाक्यानि.get(lsrSubtagIndexes[i]),
                          वाक्यानि.get(lsrSubtagIndexes[i + 1]),
                          वाक्यानि.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        यदि (partitions_दीर्घता > 0) {
            distance_दत्त.partitions = static_cast<ध्रुव वर्ण **>(
                uprv_malloc(partitions_दीर्घता * परिमाण(ध्रुव वर्ण *)));
            यदि (distance_दत्त.partitions == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t i = 0; i < partitions_दीर्घता; ++i) {
                distance_दत्त.partitions[i] = वाक्यानि.get(partitionIndexes[i]);
            }
        }

        यदि (paradigm_Subtags_दीर्घता > 0) {
            distance_दत्त.paradigms_दीर्घता = paradigm_Subtags_दीर्घता / 3;
            LSR *paradigms = नूतन LSR[distance_दत्त.paradigms_दीर्घता];
            यदि (paradigms == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम;
            }
            प्रति (int32_t i = 0, j = 0; i < paradigm_Subtags_दीर्घता; i += 3, ++j) {
                paradigms[j] = LSR(वाक्यानि.get(paradigmSubtagIndexes[i]),
                                   वाक्यानि.get(paradigmSubtagIndexes[i + 1]),
                                   वाक्यानि.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            distance_दत्त.paradigms = paradigms;
        }
    }

गुप्त:
    तार्किक read_वाक्यानि(ध्रुव Resource_सारणी &सारणी, ध्रुव वर्ण *कुञ्चिका, Resource_मानम् &value,
                     Local_स्मृतिः<int32_t> &indexes, int32_t &length, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
        यदि (सारणी.find_मानम्(कुञ्चिका, value)) {
            ResourceArray stringArray = value.getArray(error_संहिता);
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
            length = stringArray.get_आकार();
            यदि (length == 0) { प्रत्यागम सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            यदि (rawIndexes == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम असत्य;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                यदि (stringArray.get_मानम्(i, value)) {  // returns true because i < length
                    int32_t str_दीर्घता = 0;
                    rawIndexes[i] = वाक्यानि.सङ्कलय(value.getString(str_दीर्घता, error_संहिता), error_संहिता);
                    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
                }
            }
        }
        प्रत्यागम सत्य;
    }
    UnicodeString गन्तव्यम्_Language(पूर्णाङ्क encoded) {
        यदि (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        वर्ण lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        यदि (encoded / (27 * 27) == 0) {
            प्रत्यागम UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        प्रत्यागम UnicodeString(lang, 3, US_INV);
    }
    UnicodeString गन्तव्यम्_Script(पूर्णाङ्क encoded) {
        यदि (encoded == 0) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ध्रुव वर्ण* script = uscript_getShortName(static_cast<UScript_संहिता>(encoded));
        यदि (script == शून्यसूचकः) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        प्रत्यागम UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_सूचकांक_गन्तव्यम्_संहिता(ध्रुव ResourceArray &m49Array, Resource_मानम् &value, पूर्णाङ्क सूचकांक, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        यदि (m49Array.get_मानम्(सूचकांक, value)) {
            प्रत्यागम value.getUnicodeString(error_संहिता);
        }
        // "m49" does not include the index.
        error_संहिता = U_MISSING_RESOURCE_ERROR;
        प्रत्यागम UNICODE_STRING_SIMPLE("");
    }

    UnicodeString गन्तव्यम्_Region(ध्रुव ResourceArray& m49Array, Resource_मानम् &value, पूर्णाङ्क encoded, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता) || encoded == 0 || encoded == 1) {
            प्रत्यागम UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        यदि (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            प्रत्यागम  m49_सूचकांक_गन्तव्यम्_संहिता(m49Array, value, encoded, error_संहिता);
        }
        वर्ण region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        प्रत्यागम UnicodeString(region, 2, US_INV);
    }

    तार्किक read_LSREncoded_वाक्यानि(ध्रुव Resource_सारणी &सारणी, ध्रुव वर्ण* कुञ्चिका, Resource_मानम् &value, ध्रुव ResourceArray& m49Array,
                     Local_स्मृतिः<int32_t> &indexes, int32_t &length, UError_संहिता &error_संहिता) {
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
        यदि (सारणी.find_मानम्(कुञ्चिका, value)) {
            ध्रुव int32_t* vectors = value.get_Int_सदिशम्(length, error_संहिता);
            यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
            यदि (length == 0) { प्रत्यागम सत्य; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            यदि (rawIndexes == शून्यसूचकः) {
                error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
                प्रत्यागम असत्य;
            }
            प्रति (पूर्णाङ्क i = 0; i < length; ++i) {
                rawIndexes[i*3] = वाक्यानि.सङ्कलय_By_मानम्(गन्तव्यम्_Language(vectors[i]), error_संहिता);
                rawIndexes[i*3+1] = वाक्यानि.सङ्कलय_By_मानम्(गन्तव्यम्_Script(vectors[i]), error_संहिता);
                rawIndexes[i*3+2] = वाक्यानि.सङ्कलय_By_मानम्(
                    गन्तव्यम्_Region(m49Array, value, vectors[i], error_संहिता), error_संहिता);
                यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
            }
            length *= 3;
        }
        प्रत्यागम सत्य;
    }
};

नामाकाश {

LikelySubtags *gLikelySubtags = शून्यसूचकः;
UVector *gMacroregions = शून्यसूचकः;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    विलोपय gLikelySubtags;
    gLikelySubtags = शून्यसूचकः;
    विलोपय gMacroregions;
    gMacroregions = शून्यसूचकः;
    gInitOnce.reset();
    प्रत्यागम सत्य;
}

constexpr ध्रुव char16_t* MACROREGION_HARDCODE[] = {
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
शून्य processMacroregionRange(ध्रुव UnicodeString& regionName, UVector* newMacroRegions, UError_संहिता& स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम; }
    int32_t rangeMarkerLocation = regionName.सूचकांक_सम्बन्धि(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,स्थितिः);
    यदि ( rangeMarkerLocation > 0 ) {
        char16_t अन्तः_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        यावत् ( buf[rangeMarkerLocation-1] <= अन्तः_Range && U_SUCCESS(स्थितिः)) {
            LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(buf), स्थितिः);
            newMacroRegions->adoptElement(newRegion.orphan(),स्थितिः);
            buf[rangeMarkerLocation-1]++;
        }
    } अन्यथा {
        LocalPointer<UnicodeString> newRegion(नूतन UnicodeString(regionName), स्थितिः);
        newMacroRegions->adoptElement(newRegion.orphan(),स्थितिः);
    }
}

#यदि U_DEBUG
UVector* आरोपय_Macroregions(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम शून्यसूचकः; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, स्थितिः), स्थितिः);

    LocalUResourceBundlePointer supplemental_दत्त(ures_open_प्रत्यक्षम्(शून्यसूचकः,"supplementalData",&स्थितिः));
    LocalUResourceBundlePointer idValidity(ures_get_By_कुञ्चिका(supplemental_दत्त.getAlias(),"idValidity",शून्यसूचकः,&स्थितिः));
    LocalUResourceBundlePointer regionList(ures_get_By_कुञ्चिका(idValidity.getAlias(),"region",शून्यसूचकः,&स्थितिः));
    LocalUResourceBundlePointer regionMacro(ures_get_By_कुञ्चिका(regionList.getAlias(),"macroregion",शून्यसूचकः,&स्थितिः));

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    यावत् (ures_has_अग्र(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_अग्र_Unicode_String(regionMacro.getAlias(),शून्यसूचकः,&स्थितिः);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
    }

    प्रत्यागम newMacroRegions.orphan();
}
#यदि_अन्तः // U_DEBUG

UVector* getStaticMacroregions(UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) { प्रत्यागम शून्यसूचकः; }
    LocalPointer<UVector> newMacroRegions(नूतन UVector(uprv_deleteUObject, uhash_compareUnicodeString, स्थितिः), स्थितिः);

    यदि (U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }

    प्रति (ध्रुव स्वचालित *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), स्थितिः);
        यदि (U_FAILURE(स्थितिः)) {
            प्रत्यागम शून्यसूचकः;
        }
    }

    प्रत्यागम newMacroRegions.orphan();
}

}  // namespace

शून्य U_CALLCONV LikelySubtags::initLikelySubtags(UError_संहिता &error_संहिता) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == शून्यसूचकः);
    Likely_Subtags_दत्त दत्त(error_संहिता);
    दत्त.load(error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम; }
    gLikelySubtags = नूतन LikelySubtags(दत्त);
    gMacroregions = getStaticMacroregions(error_संहिता);
#यदि U_DEBUG
    स्वचालित macroregions_From_दत्त = आरोपय_Macroregions(error_संहिता);
    U_ASSERT((*gMacroregions) == (*macroregions_From_दत्त));
    विलोपय macroregions_From_दत्त;
#यदि_अन्तः
    यदि (U_FAILURE(error_संहिता) || gLikelySubtags == शून्यसूचकः || gMacroregions == शून्यसूचकः) {
        विलोपय gLikelySubtags;
        विलोपय gMacroregions;
        error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }

    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ध्रुव LikelySubtags *LikelySubtags::getSingleton(UError_संहिता &error_संहिता) {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम शून्यसूचकः; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_संहिता);
    प्रत्यागम gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_दत्त &दत्त) :
        langInfoBundle(दत्त.langInfoBundle),
        वाक्यानि(दत्त.वाक्यानि.orphan_Char_वाक्यानि()),
        languageAliases(std::move(दत्त.languageAliases)),
        regionAliases(std::move(दत्त.regionAliases)),
        trie(दत्त.trie_अष्टकानि),
        lsrs(दत्त.lsrs),
#यदि U_DEBUG
        lsrs_दीर्घता(दत्त.lsrs_दीर्घता),
#यदि_अन्तः // U_DEBUG
        distance_दत्त(std::move(दत्त.distance_दत्त)) {
    दत्त.langInfoBundle = शून्यसूचकः;
    दत्त.lsrs = शून्यसूचकः;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_परिणाम परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_अग्र(परिणाम));
    trieUndState = trie.getState64();
    परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_अग्र(परिणाम));
    trieUndZzzzState = trie.getState64();
    परिणाम = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_मानम्(परिणाम));
    default_Lsr_सूचकांक = trie.get_मानम्();
    trie.reset();

    प्रति (char16_t c = u'a'; c <= u'z'; ++c) {
        परिणाम = trie.next(c);
        यदि (परिणाम == USTRINGTRIE_NO_मानम्) {
            trie_First_Letter_अवस्थाः[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    विलोपय वाक्यानि;
    विलोपय[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ध्रुव Locale &locale,
                                         तार्किक return_निवेशः_If_Unmatch,
                                         UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    यदि (locale.isBogus()) {
        error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम {};
    }
    ध्रुव वर्ण *name = locale.getName();
    यदि (!return_निवेशः_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        प्रत्यागम LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_निवेशः_If_Unmatch, error_संहिता);

    यदि (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        प्रत्यागम LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_संहिता);
    }
    प्रत्यागम max;
}

नामाकाश {

ध्रुव वर्ण *getCanonical(ध्रुव CharStringMap &aliases, ध्रुव वर्ण *alias) {
    ध्रुव वर्ण *canonical = aliases.get(alias);
    प्रत्यागम canonical == शून्यसूचकः ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                                     ध्रुव वर्ण *variant,
                                     तार्किक return_निवेशः_If_Unmatch,
                                     UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    यदि (!return_निवेशः_If_Unmatch) {
        वर्ण c1;
        यदि (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            विकल्प (c1) {
            पक्ष 'A':
                प्रत्यागम LSR(PSEUDO_ACCENTS_उपसर्गः, language, script, region,
                           LSR::EXPLICIT_LSR, error_संहिता);
            पक्ष 'B':
                प्रत्यागम LSR(PSEUDO_BIDI_उपसर्गः, language, script, region,
                           LSR::EXPLICIT_LSR, error_संहिता);
            पक्ष 'C':
                प्रत्यागम LSR(PSEUDO_CRACKED_उपसर्गः, language, script, region,
                           LSR::EXPLICIT_LSR, error_संहिता);
            स्वभाव:  // normal locale
                भङ्ग;
            }
        }

        यदि (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            यदि (uprv_strcmp(variant, "PSACCENT") == 0) {
                प्रत्यागम LSR(PSEUDO_ACCENTS_उपसर्गः, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_संहिता);
            } अन्यथा यदि (uprv_strcmp(variant, "PSBIDI") == 0) {
                प्रत्यागम LSR(PSEUDO_BIDI_उपसर्गः, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_संहिता);
            } अन्यथा यदि (uprv_strcmp(variant, "PSCRACK") == 0) {
                प्रत्यागम LSR(PSEUDO_CRACKED_उपसर्गः, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_संहिता);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    प्रत्यागम maximize(language, script, region, return_निवेशः_If_Unmatch, error_संहिता);
}

LSR LikelySubtags::maximize(ध्रुव वर्ण *language, ध्रुव वर्ण *script, ध्रुव वर्ण *region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    प्रत्यागम maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_निवेशः_If_Unmatch,
                    error_संहिता);
}

तार्किक LikelySubtags::isMacroregion(StringPiece& region, UError_संहिता& error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम असत्य; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    प्रत्यागम gMacroregions->अन्तर्भवति((शून्य *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             तार्किक return_निवेशः_If_Unmatch,
                             UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    यदि (language.compare("und") == 0) {
        language = "";
    }
    यदि (script.compare("Zzzz") == 0) {
        script = "";
    }
    यदि (region.compare("ZZ") == 0) {
        region = "";
    }
    यदि (!script.empty() && !region.empty() && !language.empty()) {
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, error_संहिता);  // already maximized
    }
    तार्किक retainLanguage = असत्य;
    तार्किक retainScript = असत्य;
    तार्किक retainRegion = असत्य;

    अष्टकानि_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    यदि (0 <= (c0 = uprv_अधस्तनम्_Ordinal(language.दत्त()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_First_Letter_अवस्थाः[c0]) != 0) {
        value = trie_अग्र(iter.reset_गन्तव्यम्_State64(state), language, 1);
    } अन्यथा {
        value = trie_अग्र(iter, language, 0);
    }
    तार्किक मेलनम्_Language = (value >= 0);
    तार्किक मेलनम्_Script = असत्य;
    यदि (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } अन्यथा {
        retainLanguage = सत्य;
        iter.reset_गन्तव्यम्_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    यदि (value >= 0 && !script.empty()) {
        मेलनम्_Script = सत्य;
    }
    यदि (value > 0) {
        // Intermediate or final value from just language.
        यदि (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } अन्यथा {
        value = trie_अग्र(iter, script, 0);
        यदि (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } अन्यथा {
            retainScript = सत्य;
            यदि (state == 0) {
                iter.reset_गन्तव्यम्_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.reset_गन्तव्यम्_State64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    तार्किक मेलनम्_Region = असत्य;
    यदि (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } अन्यथा {
        value = trie_अग्र(iter, region, 0);
        यदि (value >= 0) {
            यदि (!region.empty() && !isMacroregion(region, error_संहिता)) {
                retainRegion = सत्य;
                मेलनम्_Region = सत्य;
            }
        } अन्यथा {
            retainRegion = सत्य;
            यदि (state == 0) {
                value = default_Lsr_सूचकांक;
            } अन्यथा {
                iter.reset_गन्तव्यम्_State64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                यदि (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_गन्तव्यम्_State64(trieUndState);  // "und" ("*")
                    value = trie_अग्र(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_रिक्तानि_State = iter.getState64();
                    value = trie_अग्र(iter, region, 0);
                    // Fallback to und =>
                    यदि (value < 0) {
                        iter.reset_गन्तव्यम्_State64(trie_Und_रिक्तानि_State);
                        value = trie_अग्र(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_दीर्घता);
    यदि (return_निवेशः_If_Unmatch &&
        (!(मेलनम्_Language || मेलनम्_Script || (मेलनम्_Region && language.empty())))) {
      प्रत्यागम LSR("", "", "", LSR::EXPLICIT_LSR, error_संहिता);  // no matching.
    }
    यदि (language.empty()) {
        language = StringPiece("und");
    }

    यदि (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ध्रुव LSR &matched = lsrs[value];
        प्रत्यागम LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    यदि (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    यदि (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    यदि (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_आवरणम् = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    प्रत्यागम LSR(language, script, region, retain_आवरणम्, error_संहिता);
}

int32_t LikelySubtags::compareLikely(ध्रुव LSR &lsr, ध्रुव LSR &other, int32_t likelyInfo) ध्रुव {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    यदि (uprv_strcmp(lsr.language, other.language) != 0) {
        प्रत्यागम 0xfffffffc;  // negative, lsr not better than other
    }
    यदि (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t सूचकांक;
        यदि (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            सूचकांक = likelyInfo >> 2;
        } अन्यथा {
            सूचकांक = get_Likely_सूचकांक(lsr.language, "");
            likelyInfo = सूचकांक << 2;
        }
        ध्रुव LSR &likely = lsrs[सूचकांक];
        यदि (uprv_strcmp(lsr.script, likely.script) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अन्यथा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    यदि (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t सूचकांक;
        यदि (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            सूचकांक = likelyInfo >> 2;
        } अन्यथा {
            सूचकांक = get_Likely_सूचकांक(lsr.language, lsr.region);
            likelyInfo = (सूचकांक << 2) | 2;
        }
        ध्रुव LSR &likely = lsrs[सूचकांक];
        यदि (uprv_strcmp(lsr.region, likely.region) == 0) {
            प्रत्यागम likelyInfo | 1;
        } अन्यथा {
            प्रत्यागम likelyInfo & ~1;
        }
    }
    प्रत्यागम likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_सूचकांक(ध्रुव वर्ण *language, ध्रुव वर्ण *script) ध्रुव {
    यदि (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    यदि (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    अष्टकानि_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    यदि (0 <= (c0 = uprv_अधस्तनम्_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_First_Letter_अवस्थाः[c0]) != 0) {
        value = trie_अग्र(iter.reset_गन्तव्यम्_State64(state), language, 1);
    } अन्यथा {
        value = trie_अग्र(iter, language, 0);
    }
    यदि (value >= 0) {
        state = iter.getState64();
    } अन्यथा {
        iter.reset_गन्तव्यम्_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    यदि (value > 0) {
        // Intermediate or final value from just language.
        यदि (value == SKIP_SCRIPT) {
            value = 0;
        }
    } अन्यथा {
        value = trie_अग्र(iter, script, 0);
        यदि (value >= 0) {
            state = iter.getState64();
        } अन्यथा {
            यदि (state == 0) {
                iter.reset_गन्तव्यम्_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } अन्यथा {
                iter.reset_गन्तव्यम्_State64(state);
                value = trie_अग्र(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    यदि (value > 0) {
        // Final value from just language or language+script.
    } अन्यथा {
        value = trie_अग्र(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_दीर्घता);
    प्रत्यागम value;
}

int32_t LikelySubtags::trie_अग्र(अष्टकानि_Trie &iter, ध्रुव वर्ण *s, int32_t i) {
    UString_Trie_परिणाम परिणाम;
    uint8_t c;
    यदि ((c = s[i]) == 0) {
        परिणाम = iter.next(u'*');
    } अन्यथा {
        प्रति (;;) {
            c = uprv_inv_Char_गन्तव्यम्_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            यदि (next != 0) {
                यदि (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                    प्रत्यागम -1;
                }
            } अन्यथा {
                // last character of this subtag
                परिणाम = iter.next(c | 0x80);
                भङ्ग;
            }
            c = next;
        }
    }
    विकल्प (परिणाम) {
    पक्ष USTRINGTRIE_NO_मेलनम्: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मानम्: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: प्रत्यागम iter.get_मानम्();
    स्वभाव: प्रत्यागम -1;
    }
}
int32_t LikelySubtags::trie_अग्र(अष्टकानि_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_परिणाम परिणाम;
    uint8_t c;
    यदि (s.length() == i) {
        परिणाम = iter.next(u'*');
    } अन्यथा {
        c = s.दत्त()[i];
        प्रति (;;) {
            c = uprv_inv_Char_गन्तव्यम्_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            यदि (i+1 != s.length()) {
                यदि (!USTRINGTRIE_HAS_अग्र(iter.next(c))) {
                    प्रत्यागम -1;
                }
                c = s.दत्त()[++i];
            } अन्यथा {
                // last character of this subtag
                परिणाम = iter.next(c | 0x80);
                भङ्ग;
            }
        }
    }
    विकल्प (परिणाम) {
    पक्ष USTRINGTRIE_NO_मेलनम्: प्रत्यागम -1;
    पक्ष USTRINGTRIE_NO_मानम्: प्रत्यागम 0;
    पक्ष USTRINGTRIE_INTERMEDIATE_मानम्:
        U_ASSERT(iter.get_मानम्() == SKIP_SCRIPT);
        प्रत्यागम SKIP_SCRIPT;
    पक्ष USTRINGTRIE_FINAL_मानम्: प्रत्यागम iter.get_मानम्();
    स्वभाव: प्रत्यागम -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    तार्किक favorScript,
                                    UError_संहिता &error_संहिता) ध्रुव {
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    LSR max = maximize(language, script, region, सत्य, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    // If no match, return it.
    यदि (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        प्रत्यागम LSR(language, script, region, LSR::EXPLICIT_LSR, error_संहिता);
    }
    // try language
    LSR test = maximize(max.language, "", "", सत्य, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    यदि (test.is_Equivalent_गन्तव्यम्(max)) {
        प्रत्यागम LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_संहिता);
    }

    यदि (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
        यदि (test.is_Equivalent_गन्तव्यम्(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_संहिता);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", सत्य, error_संहिता);
    यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
    यदि (test.is_Equivalent_गन्तव्यम्(max)) {
        प्रत्यागम LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_संहिता);
    }
    यदि (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, सत्य, error_संहिता);
        यदि (U_FAILURE(error_संहिता)) { प्रत्यागम {}; }
        यदि (test.is_Equivalent_गन्तव्यम्(max)) {
            प्रत्यागम LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_संहिता);
        }
    }
    प्रत्यागम LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_संहिता);
}

U_NAMESPACE_अन्तः
