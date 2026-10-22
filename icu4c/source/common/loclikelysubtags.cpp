// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ಅನ್ತರ್ಭಾವ <utility>
#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/bytestrie.h"
#ಅನ್ತರ್ಭಾವ "unicode/localpointer.h"
#ಅನ್ತರ್ಭಾವ "unicode/locid.h"
#ಅನ್ತರ್ಭಾವ "unicode/uobject.h"
#ಅನ್ತರ್ಭಾವ "unicode/ures.h"
#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "charstr.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "loclikelysubtags.h"
#ಅನ್ತರ್ಭಾವ "lsr.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "uhash.h"
#ಅನ್ತರ್ಭಾವ "uinvchar.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uniquecharstr.h"
#ಅನ್ತರ್ಭಾವ "uresdata.h"
#ಅನ್ತರ್ಭಾವ "uresimp.h"
#ಅನ್ತರ್ಭಾವ "uvector.h"

U_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ {

constexpr ಅಕ್ಷರ PSEUDO_ACCENTS_ಪೂರ್ವಪ್ರತ್ಯಯ = '\'';  // -XA, -PSACCENT
constexpr ಅಕ್ಷರ PSEUDO_BIDI_ಪೂರ್ವಪ್ರತ್ಯಯ = '+';  // -XB, -PSBIDI
constexpr ಅಕ್ಷರ PSEUDO_CRACKED_ಪೂರ್ವಪ್ರತ್ಯಯ = ',';  // -XC, -PSCRACK

}  // namespace

Locale_ದೂರ_ದತ್ತಾಂಶ::Locale_ದೂರ_ದತ್ತಾಂಶ(Locale_ದೂರ_ದತ್ತಾಂಶ &&ದತ್ತಾಂಶ) :
        ದೂರ_Trie_ಬೈಟುಗಳು(ದತ್ತಾಂಶ.ದೂರ_Trie_ಬೈಟುಗಳು),
        region_ಗಮ್ಯ_Partitions(ದತ್ತಾಂಶ.region_ಗಮ್ಯ_Partitions),
        partitions(ದತ್ತಾಂಶ.partitions),
        paradigms(ದತ್ತಾಂಶ.paradigms), paradigms_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ.paradigms_ಉದ್ದಳತೆ),
        distances(ದತ್ತಾಂಶ.distances) {
    ದತ್ತಾಂಶ.partitions = ಶೂನ್ಯಸೂಚಕ;
    ದತ್ತಾಂಶ.paradigms = ಶೂನ್ಯಸೂಚಕ;
}

Locale_ದೂರ_ದತ್ತಾಂಶ::~Locale_ದೂರ_ದತ್ತಾಂಶ() {
    uprv_free(partitions);
    ಅಳಿಸು[] paradigms;
}

ರಚನೆ Likely_Subtags_ದತ್ತಾಂಶ {
    UResourceBundle *langInfoBundle = ಶೂನ್ಯಸೂಚಕ;
    Unique_Char_ಅಕ್ಷರಸರಣಿಗಳು ಅಕ್ಷರಸರಣಿಗಳು;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ಸ್ಥಿರಾಂಕ uint8_t *trie_ಬೈಟುಗಳು = ಶೂನ್ಯಸೂಚಕ;
    LSR *lsrs = ಶೂನ್ಯಸೂಚಕ;
    int32_t lsrs_ಉದ್ದಳತೆ = 0;

    Locale_ದೂರ_ದತ್ತಾಂಶ ದೂರ_ದತ್ತಾಂಶ;

    Likely_Subtags_ದತ್ತಾಂಶ(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) : ಅಕ್ಷರಸರಣಿಗಳು(error_ಸಂಕೇತ) {}

    ~Likely_Subtags_ದತ್ತಾಂಶ() {
        ures_close(langInfoBundle);
        ಅಳಿಸು[] lsrs;
    }

    ಶೂನ್ಯ load(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        langInfoBundle = ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ, "langInfo", &error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
        ರಾಶಿ_UResource_Bundle ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle;
        Resource_ದತ್ತಾಂಶ_ಮೌಲ್ಯ value;
        ures_get_ಮೌಲ್ಯ_With_Fallback(langInfoBundle, "likely", ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(),
                                  value, error_ಸಂಕೇತ);
        Resource_ಕೋಷ್ಟಕ likely_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_ಸ್ಮರಣೆ<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_ಉದ್ದಳತೆ = 0, regions_ಉದ್ದಳತೆ = 0, lsr_Subtags_ಉದ್ದಳತೆ = 0;
        ResourceArray m49Array;
        ಆದರೆ (likely_ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ("m49", value)) {
            m49Array = value.getArray(error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (!read_ಅಕ್ಷರಸರಣಿಗಳು(likely_ಕೋಷ್ಟಕ, "languageAliases", value,
                         languageIndexes, languages_ಉದ್ದಳತೆ, error_ಸಂಕೇತ) ||
                !read_ಅಕ್ಷರಸರಣಿಗಳು(likely_ಕೋಷ್ಟಕ, "regionAliases", value,
                             regionIndexes, regions_ಉದ್ದಳತೆ, error_ಸಂಕೇತ) ||
                !read_LSREncoded_ಅಕ್ಷರಸರಣಿಗಳು(likely_ಕೋಷ್ಟಕ, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_ಉದ್ದಳತೆ, error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ ((languages_ಉದ್ದಳತೆ & 1) != 0 ||
                (regions_ಉದ್ದಳತೆ & 1) != 0 ||
                (lsr_Subtags_ಉದ್ದಳತೆ % 3) != 0) {
            error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (lsr_Subtags_ಉದ್ದಳತೆ == 0) {
            error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
            ಹಿಂತಿರುಗಿ;
        }

        ಆದರೆ (!likely_ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ("trie", value)) {
            error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        int32_t length;
        trie_ಬೈಟುಗಳು = value.get_ದ್ವಿಮಾನ(length, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_ಸಂಕೇತ ಹೊಂದಿಕೆ_Error_ಸಂಕೇತ = U_ZERO_ERROR;
        ures_get_ಮೌಲ್ಯ_With_Fallback(langInfoBundle, "match", ರಾಶಿ_ತಾತ್ಕಾಲಿಕ_Bundle.getAlias(),
                                  value, ಹೊಂದಿಕೆ_Error_ಸಂಕೇತ);
        Local_ಸ್ಮರಣೆ<int32_t> ವಿಭಜನೆ_Indexes, paradigmSubtagIndexes;
        int32_t partitions_ಉದ್ದಳತೆ = 0, paradigm_Subtags_ಉದ್ದಳತೆ = 0;
        ಆದರೆ (U_SUCCESS(ಹೊಂದಿಕೆ_Error_ಸಂಕೇತ)) {
            Resource_ಕೋಷ್ಟಕ ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ = value.get_ಕೋಷ್ಟಕ(error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }

            ಆದರೆ (ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ("trie", value)) {
                ದೂರ_ದತ್ತಾಂಶ.ದೂರ_Trie_ಬೈಟುಗಳು = value.get_ದ್ವಿಮಾನ(length, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
            }

            ಆದರೆ (ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ("regionToPartitions", value)) {
                ದೂರ_ದತ್ತಾಂಶ.region_ಗಮ್ಯ_Partitions = value.get_ದ್ವಿಮಾನ(length, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
                ಆದರೆ (length < LSR::REGION_ಸೂಚ್ಯಂಕ_ಎಲ್ಲೆ) {
                    error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
                    ಹಿಂತಿರುಗಿ;
                }
            }

            ಆದರೆ (!read_ಅಕ್ಷರಸರಣಿಗಳು(ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ, "partitions", value,
                             ವಿಭಜನೆ_Indexes, partitions_ಉದ್ದಳತೆ, error_ಸಂಕೇತ) ||
                    !read_LSREncoded_ಅಕ್ಷರಸರಣಿಗಳು(ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_ಉದ್ದಳತೆ, error_ಸಂಕೇತ)) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ ((paradigm_Subtags_ಉದ್ದಳತೆ % 3) != 0) {
                error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
                ಹಿಂತಿರುಗಿ;
            }

            ಆದರೆ (ಹೊಂದಿಕೆ_ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ("distances", value)) {
                ದೂರ_ದತ್ತಾಂಶ.distances = value.get_Int_ಸದಿಶ(length, error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
                ಆದರೆ (length < 4) {  // LocaleDistance IX_LIMIT
                    error_ಸಂಕೇತ = U_INVALID_FORMAT_ERROR;
                    ಹಿಂತಿರುಗಿ;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹೊಂದಿಕೆ_Error_ಸಂಕೇತ == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ಇಲ್ಲದಿದ್ದರೆ {  // error other than missing resource
            error_ಸಂಕೇತ = ಹೊಂದಿಕೆ_Error_ಸಂಕೇತ;
            ಹಿಂತಿರುಗಿ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        ಅಕ್ಷರಸರಣಿಗಳು.freeze();

        languageAliases = CharStringMap(languages_ಉದ್ದಳತೆ / 2, error_ಸಂಕೇತ);
        ಗಾಗಿ (int32_t i = 0; i < languages_ಉದ್ದಳತೆ; i += 2) {
            languageAliases.put(ಅಕ್ಷರಸರಣಿಗಳು.get(languageIndexes[i]),
                                ಅಕ್ಷರಸರಣಿಗಳು.get(languageIndexes[i + 1]), error_ಸಂಕೇತ);
        }

        regionAliases = CharStringMap(regions_ಉದ್ದಳತೆ / 2, error_ಸಂಕೇತ);
        ಗಾಗಿ (int32_t i = 0; i < regions_ಉದ್ದಳತೆ; i += 2) {
            regionAliases.put(ಅಕ್ಷರಸರಣಿಗಳು.get(regionIndexes[i]),
                              ಅಕ್ಷರಸರಣಿಗಳು.get(regionIndexes[i + 1]), error_ಸಂಕೇತ);
        }
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }

        lsrs_ಉದ್ದಳತೆ = lsr_Subtags_ಉದ್ದಳತೆ / 3;
        lsrs = ಹೊಸ LSR[lsrs_ಉದ್ದಳತೆ];
        ಆದರೆ (lsrs == ಶೂನ್ಯಸೂಚಕ) {
            error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ;
        }
        ಗಾಗಿ (int32_t i = 0, j = 0; i < lsr_Subtags_ಉದ್ದಳತೆ; i += 3, ++j) {
            lsrs[j] = LSR(ಅಕ್ಷರಸರಣಿಗಳು.get(lsrSubtagIndexes[i]),
                          ಅಕ್ಷರಸರಣಿಗಳು.get(lsrSubtagIndexes[i + 1]),
                          ಅಕ್ಷರಸರಣಿಗಳು.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ಆದರೆ (partitions_ಉದ್ದಳತೆ > 0) {
            ದೂರ_ದತ್ತಾಂಶ.partitions = static_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **>(
                uprv_malloc(partitions_ಉದ್ದಳತೆ * ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)));
            ಆದರೆ (ದೂರ_ದತ್ತಾಂಶ.partitions == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಗಾಗಿ (int32_t i = 0; i < partitions_ಉದ್ದಳತೆ; ++i) {
                ದೂರ_ದತ್ತಾಂಶ.partitions[i] = ಅಕ್ಷರಸರಣಿಗಳು.get(ವಿಭಜನೆ_Indexes[i]);
            }
        }

        ಆದರೆ (paradigm_Subtags_ಉದ್ದಳತೆ > 0) {
            ದೂರ_ದತ್ತಾಂಶ.paradigms_ಉದ್ದಳತೆ = paradigm_Subtags_ಉದ್ದಳತೆ / 3;
            LSR *paradigms = ಹೊಸ LSR[ದೂರ_ದತ್ತಾಂಶ.paradigms_ಉದ್ದಳತೆ];
            ಆದರೆ (paradigms == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ;
            }
            ಗಾಗಿ (int32_t i = 0, j = 0; i < paradigm_Subtags_ಉದ್ದಳತೆ; i += 3, ++j) {
                paradigms[j] = LSR(ಅಕ್ಷರಸರಣಿಗಳು.get(paradigmSubtagIndexes[i]),
                                   ಅಕ್ಷರಸರಣಿಗಳು.get(paradigmSubtagIndexes[i + 1]),
                                   ಅಕ್ಷರಸರಣಿಗಳು.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            ದೂರ_ದತ್ತಾಂಶ.paradigms = paradigms;
        }
    }

ಖಾಸಗಿ:
    ತಾರ್ಕಿಕ read_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ Resource_ಕೋಷ್ಟಕ &ಕೋಷ್ಟಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value,
                     Local_ಸ್ಮರಣೆ<int32_t> &indexes, int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
        ಆದರೆ (ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಕೀಲಿ, value)) {
            ResourceArray stringArray = value.getArray(error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
            length = stringArray.get_ಗಾತ್ರ();
            ಆದರೆ (length == 0) { ಹಿಂತಿರುಗಿ ನಿಜ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ಆದರೆ (rawIndexes == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < length; ++i) {
                ಆದರೆ (stringArray.get_ಮೌಲ್ಯ(i, value)) {  // returns true because i < length
                    int32_t str_ಉದ್ದಳತೆ = 0;
                    rawIndexes[i] = ಅಕ್ಷರಸರಣಿಗಳು.ಕೂಡಿಸು(value.getString(str_ಉದ್ದಳತೆ, error_ಸಂಕೇತ), error_ಸಂಕೇತ);
                    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
                }
            }
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    UnicodeString ಗಮ್ಯ_Language(ಪೂರ್ಣಾಂಕ encoded) {
        ಆದರೆ (encoded == 0) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
        }
        ಆದರೆ (encoded == 1) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        ಅಕ್ಷರ lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ಆದರೆ (encoded / (27 * 27) == 0) {
            ಹಿಂತಿರುಗಿ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ಹಿಂತಿರುಗಿ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString ಗಮ್ಯ_Script(ಪೂರ್ಣಾಂಕ encoded) {
        ಆದರೆ (encoded == 0) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
        }
        ಆದರೆ (encoded == 1) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* script = uscript_getShortName(static_cast<UScript_ಸಂಕೇತ>(encoded));
        ಆದರೆ (script == ಶೂನ್ಯಸೂಚಕ) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ಹಿಂತಿರುಗಿ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಸಂಕೇತ(ಸ್ಥಿರಾಂಕ ResourceArray &m49Array, Resource_ಮೌಲ್ಯ &value, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
        }
        ಆದರೆ (m49Array.get_ಮೌಲ್ಯ(ಸೂಚ್ಯಂಕ, value)) {
            ಹಿಂತಿರುಗಿ value.getUnicodeString(error_ಸಂಕೇತ);
        }
        // "m49" does not include the index.
        error_ಸಂಕೇತ = U_MISSING_RESOURCE_ERROR;
        ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString ಗಮ್ಯ_Region(ಸ್ಥಿರಾಂಕ ResourceArray& m49Array, Resource_ಮೌಲ್ಯ &value, ಪೂರ್ಣಾಂಕ encoded, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || encoded == 0 || encoded == 1) {
            ಹಿಂತಿರುಗಿ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ಆದರೆ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ಹಿಂತಿರುಗಿ  m49_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಸಂಕೇತ(m49Array, value, encoded, error_ಸಂಕೇತ);
        }
        ಅಕ್ಷರ region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ಹಿಂತಿರುಗಿ UnicodeString(region, 2, US_INV);
    }

    ತಾರ್ಕಿಕ read_LSREncoded_ಅಕ್ಷರಸರಣಿಗಳು(ಸ್ಥಿರಾಂಕ Resource_ಕೋಷ್ಟಕ &ಕೋಷ್ಟಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಕೀಲಿ, Resource_ಮೌಲ್ಯ &value, ಸ್ಥಿರಾಂಕ ResourceArray& m49Array,
                     Local_ಸ್ಮರಣೆ<int32_t> &indexes, int32_t &length, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
        ಆದರೆ (ಕೋಷ್ಟಕ.ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಕೀಲಿ, value)) {
            ಸ್ಥಿರಾಂಕ int32_t* vectors = value.get_Int_ಸದಿಶ(length, error_ಸಂಕೇತ);
            ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
            ಆದರೆ (length == 0) { ಹಿಂತಿರುಗಿ ನಿಜ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ಆದರೆ (rawIndexes == ಶೂನ್ಯಸೂಚಕ) {
                error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < length; ++i) {
                rawIndexes[i*3] = ಅಕ್ಷರಸರಣಿಗಳು.ಕೂಡಿಸು_By_ಮೌಲ್ಯ(ಗಮ್ಯ_Language(vectors[i]), error_ಸಂಕೇತ);
                rawIndexes[i*3+1] = ಅಕ್ಷರಸರಣಿಗಳು.ಕೂಡಿಸು_By_ಮೌಲ್ಯ(ಗಮ್ಯ_Script(vectors[i]), error_ಸಂಕೇತ);
                rawIndexes[i*3+2] = ಅಕ್ಷರಸರಣಿಗಳು.ಕೂಡಿಸು_By_ಮೌಲ್ಯ(
                    ಗಮ್ಯ_Region(m49Array, value, vectors[i], error_ಸಂಕೇತ), error_ಸಂಕೇತ);
                ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
            }
            length *= 3;
        }
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
};

ನಾಮಾಕಾಶ {

LikelySubtags *gLikelySubtags = ಶೂನ್ಯಸೂಚಕ;
UVector *gMacroregions = ಶೂನ್ಯಸೂಚಕ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ಅಳಿಸು gLikelySubtags;
    gLikelySubtags = ಶೂನ್ಯಸೂಚಕ;
    ಅಳಿಸು gMacroregions;
    gMacroregions = ಶೂನ್ಯಸೂಚಕ;
    gInitOnce.reset();
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

constexpr ಸ್ಥಿರಾಂಕ char16_t* MACROREGION_HARDCODE[] = {
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
ಶೂನ್ಯ processMacroregionRange(ಸ್ಥಿರಾಂಕ UnicodeString& regionName, UVector* newMacroRegions, UError_ಸಂಕೇತ& ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ; }
    int32_t rangeMarkerLocation = regionName.ಸೂಚ್ಯಂಕ_ದ(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,ಸ್ಥಿತಿ_2);
    ಆದರೆ ( rangeMarkerLocation > 0 ) {
        char16_t ಕೊನೆ_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ಆಗ ( buf[rangeMarkerLocation-1] <= ಕೊನೆ_Range && U_SUCCESS(ಸ್ಥಿತಿ_2)) {
            Local_ಸೂಚಕ<UnicodeString> newRegion(ಹೊಸ UnicodeString(buf), ಸ್ಥಿತಿ_2);
            newMacroRegions->adoptElement(newRegion.orphan(),ಸ್ಥಿತಿ_2);
            buf[rangeMarkerLocation-1]++;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        Local_ಸೂಚಕ<UnicodeString> newRegion(ಹೊಸ UnicodeString(regionName), ಸ್ಥಿತಿ_2);
        newMacroRegions->adoptElement(newRegion.orphan(),ಸ್ಥಿತಿ_2);
    }
}

#ಆದರೆ U_DEBUG
UVector* ತುಂಬು_Macroregions(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    Local_ಸೂಚಕ<UVector> newMacroRegions(ಹೊಸ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);

    Local_UResource_Bundle_ಸೂಚಕ supplemental_ದತ್ತಾಂಶ(ures_open_ನೇರ(ಶೂನ್ಯಸೂಚಕ,"supplementalData",&ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ idValidity(ures_get_By_ಕೀಲಿ(supplemental_ದತ್ತಾಂಶ.getAlias(),"idValidity",ಶೂನ್ಯಸೂಚಕ,&ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ regionList(ures_get_By_ಕೀಲಿ(idValidity.getAlias(),"region",ಶೂನ್ಯಸೂಚಕ,&ಸ್ಥಿತಿ_2));
    Local_UResource_Bundle_ಸೂಚಕ regionMacro(ures_get_By_ಕೀಲಿ(regionList.getAlias(),"macroregion",ಶೂನ್ಯಸೂಚಕ,&ಸ್ಥಿತಿ_2));

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆಗ (ures_has_ಮುಂದಿನ(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_ಮುಂದಿನ_Unicode_String(regionMacro.getAlias(),ಶೂನ್ಯಸೂಚಕ,&ಸ್ಥಿತಿ_2);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಹಿಂತಿರುಗಿ newMacroRegions.orphan();
}
#ಆದರೆ_ಅಂತ್ಯ // U_DEBUG

UVector* getStaticMacroregions(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    Local_ಸೂಚಕ<UVector> newMacroRegions(ಹೊಸ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ಸ್ಥಿತಿ_2), ಸ್ಥಿತಿ_2);

    ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), ಸ್ಥಿತಿ_2);
        ಆದರೆ (U_FAILURE(ಸ್ಥಿತಿ_2)) {
            ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
        }
    }

    ಹಿಂತಿರುಗಿ newMacroRegions.orphan();
}

}  // namespace

ಶೂನ್ಯ U_CALLCONV LikelySubtags::initLikelySubtags(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == ಶೂನ್ಯಸೂಚಕ);
    Likely_Subtags_ದತ್ತಾಂಶ ದತ್ತಾಂಶ(error_ಸಂಕೇತ);
    ದತ್ತಾಂಶ.load(error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ; }
    gLikelySubtags = ಹೊಸ LikelySubtags(ದತ್ತಾಂಶ);
    gMacroregions = getStaticMacroregions(error_ಸಂಕೇತ);
#ಆದರೆ U_DEBUG
    ಸ್ವಯಂಚಾಲಿತ macroregions_From_ದತ್ತಾಂಶ = ತುಂಬು_Macroregions(error_ಸಂಕೇತ);
    U_ASSERT((*gMacroregions) == (*macroregions_From_ದತ್ತಾಂಶ));
    ಅಳಿಸು macroregions_From_ದತ್ತಾಂಶ;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ) || gLikelySubtags == ಶೂನ್ಯಸೂಚಕ || gMacroregions == ಶೂನ್ಯಸೂಚಕ) {
        ಅಳಿಸು gLikelySubtags;
        ಅಳಿಸು gMacroregions;
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ಸ್ಥಿರಾಂಕ LikelySubtags *LikelySubtags::getSingleton(UError_ಸಂಕೇತ &error_ಸಂಕೇತ) {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_ದತ್ತಾಂಶ &ದತ್ತಾಂಶ) :
        langInfoBundle(ದತ್ತಾಂಶ.langInfoBundle),
        ಅಕ್ಷರಸರಣಿಗಳು(ದತ್ತಾಂಶ.ಅಕ್ಷರಸರಣಿಗಳು.orphan_Char_ಅಕ್ಷರಸರಣಿಗಳು()),
        languageAliases(std::move(ದತ್ತಾಂಶ.languageAliases)),
        regionAliases(std::move(ದತ್ತಾಂಶ.regionAliases)),
        trie(ದತ್ತಾಂಶ.trie_ಬೈಟುಗಳು),
        lsrs(ದತ್ತಾಂಶ.lsrs),
#ಆದರೆ U_DEBUG
        lsrs_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ.lsrs_ಉದ್ದಳತೆ),
#ಆದರೆ_ಅಂತ್ಯ // U_DEBUG
        ದೂರ_ದತ್ತಾಂಶ(std::move(ದತ್ತಾಂಶ.ದೂರ_ದತ್ತಾಂಶ)) {
    ದತ್ತಾಂಶ.langInfoBundle = ಶೂನ್ಯಸೂಚಕ;
    ದತ್ತಾಂಶ.lsrs = ಶೂನ್ಯಸೂಚಕ;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ));
    trieUndState = trie.getState64();
    ಫಲಿತಾಂಶ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ಮುಂದಿನ(ಫಲಿತಾಂಶ));
    trieUndZzzzState = trie.getState64();
    ಫಲಿತಾಂಶ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ಮೌಲ್ಯ(ಫಲಿತಾಂಶ));
    default_Lsr_ಸೂಚ್ಯಂಕ = trie.get_ಮೌಲ್ಯ();
    trie.reset();

    ಗಾಗಿ (char16_t c = u'a'; c <= u'z'; ++c) {
        ಫಲಿತಾಂಶ = trie.next(c);
        ಆದರೆ (ಫಲಿತಾಂಶ == USTRINGTRIE_NO_ಮೌಲ್ಯ) {
            trie_ಮೊದಲ_Letter_ಸ್ಥಿತಿಗಳು[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    ಅಳಿಸು ಅಕ್ಷರಸರಣಿಗಳು;
    ಅಳಿಸು[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ಸ್ಥಿರಾಂಕ Locale &locale,
                                         ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                                         UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    ಆದರೆ (locale.isBogus()) {
        error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ {};
    }
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = locale.getName();
    ಆದರೆ (!return_ಒಳಹರಿವು_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ಹಿಂತಿರುಗಿ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ಒಳಹರಿವು_If_Unmatch, error_ಸಂಕೇತ);

    ಆದರೆ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ಹಿಂತಿರುಗಿ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ max;
}

ನಾಮಾಕಾಶ {

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getCanonical(ಸ್ಥಿರಾಂಕ CharStringMap &aliases, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *alias) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *canonical = aliases.get(alias);
    ಹಿಂತಿರುಗಿ canonical == ಶೂನ್ಯಸೂಚಕ ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variant,
                                     ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                                     UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ಆದರೆ (!return_ಒಳಹರಿವು_If_Unmatch) {
        ಅಕ್ಷರ c1;
        ಆದರೆ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            ಆಯ್ಕೆ (c1) {
            ಸ್ಥಿತಿ 'A':
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_ACCENTS_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);
            ಸ್ಥಿತಿ 'B':
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_BIDI_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);
            ಸ್ಥಿತಿ 'C':
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_CRACKED_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);
            ಪೂರ್ವನಿಯೋಜಿತ:  // normal locale
                ನಿಲ್ಲಿಸು;
            }
        }

        ಆದರೆ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ಆದರೆ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_ACCENTS_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_BIDI_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ಹಿಂತಿರುಗಿ LSR(PSEUDO_CRACKED_ಪೂರ್ವಪ್ರತ್ಯಯ, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_ಸಂಕೇತ);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ಹಿಂತಿರುಗಿ maximize(language, script, region, return_ಒಳಹರಿವು_If_Unmatch, error_ಸಂಕೇತ);
}

LSR LikelySubtags::maximize(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *region,
                             ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    ಹಿಂತಿರುಗಿ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ಒಳಹರಿವು_If_Unmatch,
                    error_ಸಂಕೇತ);
}

ತಾರ್ಕಿಕ LikelySubtags::isMacroregion(StringPiece& region, UError_ಸಂಕೇತ& error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ ಸುಳ್ಳು; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ಹಿಂತಿರುಗಿ gMacroregions->ಒಳಗೊಂಡಿದೆ((ಶೂನ್ಯ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             ತಾರ್ಕಿಕ return_ಒಳಹರಿವು_If_Unmatch,
                             UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    ಆದರೆ (language.compare("und") == 0) {
        language = "";
    }
    ಆದರೆ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ಆದರೆ (region.compare("ZZ") == 0) {
        region = "";
    }
    ಆದರೆ (!script.empty() && !region.empty() && !language.empty()) {
        ಹಿಂತಿರುಗಿ LSR(language, script, region, LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);  // already maximized
    }
    ತಾರ್ಕಿಕ retainLanguage = ಸುಳ್ಳು;
    ತಾರ್ಕಿಕ retainScript = ಸುಳ್ಳು;
    ತಾರ್ಕಿಕ retainRegion = ಸುಳ್ಳು;

    ಬೈಟುಗಳು_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ಆದರೆ (0 <= (c0 = uprv_ಅಧೋ_Ordinal(language.ದತ್ತಾಂಶ()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_ಮೊದಲ_Letter_ಸ್ಥಿತಿಗಳು[c0]) != 0) {
        value = trie_ಮುಂದಿನ(iter.reset_ಗಮ್ಯ_State64(state), language, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, language, 0);
    }
    ತಾರ್ಕಿಕ ಹೊಂದಿಕೆ_Language = (value >= 0);
    ತಾರ್ಕಿಕ ಹೊಂದಿಕೆ_Script = ಸುಳ್ಳು;
    ಆದರೆ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ಇಲ್ಲದಿದ್ದರೆ {
        retainLanguage = ನಿಜ;
        iter.reset_ಗಮ್ಯ_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ಆದರೆ (value >= 0 && !script.empty()) {
        ಹೊಂದಿಕೆ_Script = ನಿಜ;
    }
    ಆದರೆ (value > 0) {
        // Intermediate or final value from just language.
        ಆದರೆ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, script, 0);
        ಆದರೆ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ಇಲ್ಲದಿದ್ದರೆ {
            retainScript = ನಿಜ;
            ಆದರೆ (state == 0) {
                iter.reset_ಗಮ್ಯ_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter.reset_ಗಮ್ಯ_State64(state);
                value = trie_ಮುಂದಿನ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ತಾರ್ಕಿಕ ಹೊಂದಿಕೆ_Region = ಸುಳ್ಳು;
    ಆದರೆ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, region, 0);
        ಆದರೆ (value >= 0) {
            ಆದರೆ (!region.empty() && !isMacroregion(region, error_ಸಂಕೇತ)) {
                retainRegion = ನಿಜ;
                ಹೊಂದಿಕೆ_Region = ನಿಜ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            retainRegion = ನಿಜ;
            ಆದರೆ (state == 0) {
                value = default_Lsr_ಸೂಚ್ಯಂಕ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter.reset_ಗಮ್ಯ_State64(state);
                value = trie_ಮುಂದಿನ(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ಆದರೆ (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_ಗಮ್ಯ_State64(trieUndState);  // "und" ("*")
                    value = trie_ಮುಂದಿನ(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_ಖಾಲಿಗಳು_State = iter.getState64();
                    value = trie_ಮುಂದಿನ(iter, region, 0);
                    // Fallback to und =>
                    ಆದರೆ (value < 0) {
                        iter.reset_ಗಮ್ಯ_State64(trie_Und_ಖಾಲಿಗಳು_State);
                        value = trie_ಮುಂದಿನ(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_ಉದ್ದಳತೆ);
    ಆದರೆ (return_ಒಳಹರಿವು_If_Unmatch &&
        (!(ಹೊಂದಿಕೆ_Language || ಹೊಂದಿಕೆ_Script || (ಹೊಂದಿಕೆ_Region && language.empty())))) {
      ಹಿಂತಿರುಗಿ LSR("", "", "", LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);  // no matching.
    }
    ಆದರೆ (language.empty()) {
        language = StringPiece("und");
    }

    ಆದರೆ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ಸ್ಥಿರಾಂಕ LSR &matched = lsrs[value];
        ಹಿಂತಿರುಗಿ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ಆದರೆ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ಆದರೆ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ಆದರೆ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_ಮುಖವಾಡ = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ಹಿಂತಿರುಗಿ LSR(language, script, region, retain_ಮುಖವಾಡ, error_ಸಂಕೇತ);
}

int32_t LikelySubtags::compareLikely(ಸ್ಥಿರಾಂಕ LSR &lsr, ಸ್ಥಿರಾಂಕ LSR &other, int32_t likelyInfo) ಸ್ಥಿರಾಂಕ {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ಆದರೆ (uprv_strcmp(lsr.language, other.language) != 0) {
        ಹಿಂತಿರುಗಿ 0xfffffffc;  // negative, lsr not better than other
    }
    ಆದರೆ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t ಸೂಚ್ಯಂಕ;
        ಆದರೆ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            ಸೂಚ್ಯಂಕ = likelyInfo >> 2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸೂಚ್ಯಂಕ = get_Likely_ಸೂಚ್ಯಂಕ(lsr.language, "");
            likelyInfo = ಸೂಚ್ಯಂಕ << 2;
        }
        ಸ್ಥಿರಾಂಕ LSR &likely = lsrs[ಸೂಚ್ಯಂಕ];
        ಆದರೆ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ಹಿಂತಿರುಗಿ likelyInfo | 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ likelyInfo & ~1;
        }
    }
    ಆದರೆ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t ಸೂಚ್ಯಂಕ;
        ಆದರೆ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            ಸೂಚ್ಯಂಕ = likelyInfo >> 2;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಸೂಚ್ಯಂಕ = get_Likely_ಸೂಚ್ಯಂಕ(lsr.language, lsr.region);
            likelyInfo = (ಸೂಚ್ಯಂಕ << 2) | 2;
        }
        ಸ್ಥಿರಾಂಕ LSR &likely = lsrs[ಸೂಚ್ಯಂಕ];
        ಆದರೆ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ಹಿಂತಿರುಗಿ likelyInfo | 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ likelyInfo & ~1;
        }
    }
    ಹಿಂತಿರುಗಿ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_ಸೂಚ್ಯಂಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *language, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *script) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ಆದರೆ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    ಬೈಟುಗಳು_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ಆದರೆ (0 <= (c0 = uprv_ಅಧೋ_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_ಮೊದಲ_Letter_ಸ್ಥಿತಿಗಳು[c0]) != 0) {
        value = trie_ಮುಂದಿನ(iter.reset_ಗಮ್ಯ_State64(state), language, 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, language, 0);
    }
    ಆದರೆ (value >= 0) {
        state = iter.getState64();
    } ಇಲ್ಲದಿದ್ದರೆ {
        iter.reset_ಗಮ್ಯ_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ಆದರೆ (value > 0) {
        // Intermediate or final value from just language.
        ಆದರೆ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, script, 0);
        ಆದರೆ (value >= 0) {
            state = iter.getState64();
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (state == 0) {
                iter.reset_ಗಮ್ಯ_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ಇಲ್ಲದಿದ್ದರೆ {
                iter.reset_ಗಮ್ಯ_State64(state);
                value = trie_ಮುಂದಿನ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ಆದರೆ (value > 0) {
        // Final value from just language or language+script.
    } ಇಲ್ಲದಿದ್ದರೆ {
        value = trie_ಮುಂದಿನ(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ value;
}

int32_t LikelySubtags::trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, int32_t i) {
    UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ;
    uint8_t c;
    ಆದರೆ ((c = s[i]) == 0) {
        ಫಲಿತಾಂಶ = iter.next(u'*');
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ (;;) {
            c = uprv_inv_Char_ಗಮ್ಯ_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ಆದರೆ (next != 0) {
                ಆದರೆ (!USTRINGTRIE_HAS_ಮುಂದಿನ(iter.next(c))) {
                    ಹಿಂತಿರುಗಿ -1;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                // last character of this subtag
                ಫಲಿತಾಂಶ = iter.next(c | 0x80);
                ನಿಲ್ಲಿಸು;
            }
            c = next;
        }
    }
    ಆಯ್ಕೆ (ಫಲಿತಾಂಶ) {
    ಸ್ಥಿತಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ: ಹಿಂತಿರುಗಿ -1;
    ಸ್ಥಿತಿ USTRINGTRIE_NO_ಮೌಲ್ಯ: ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ:
        U_ASSERT(iter.get_ಮೌಲ್ಯ() == SKIP_SCRIPT);
        ಹಿಂತಿರುಗಿ SKIP_SCRIPT;
    ಸ್ಥಿತಿ USTRINGTRIE_FINAL_ಮೌಲ್ಯ: ಹಿಂತಿರುಗಿ iter.get_ಮೌಲ್ಯ();
    ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ -1;
    }
}
int32_t LikelySubtags::trie_ಮುಂದಿನ(ಬೈಟುಗಳು_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ;
    uint8_t c;
    ಆದರೆ (s.length() == i) {
        ಫಲಿತಾಂಶ = iter.next(u'*');
    } ಇಲ್ಲದಿದ್ದರೆ {
        c = s.ದತ್ತಾಂಶ()[i];
        ಗಾಗಿ (;;) {
            c = uprv_inv_Char_ಗಮ್ಯ_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ಆದರೆ (i+1 != s.length()) {
                ಆದರೆ (!USTRINGTRIE_HAS_ಮುಂದಿನ(iter.next(c))) {
                    ಹಿಂತಿರುಗಿ -1;
                }
                c = s.ದತ್ತಾಂಶ()[++i];
            } ಇಲ್ಲದಿದ್ದರೆ {
                // last character of this subtag
                ಫಲಿತಾಂಶ = iter.next(c | 0x80);
                ನಿಲ್ಲಿಸು;
            }
        }
    }
    ಆಯ್ಕೆ (ಫಲಿತಾಂಶ) {
    ಸ್ಥಿತಿ USTRINGTRIE_NO_ಹೊಂದಿಕೆ: ಹಿಂತಿರುಗಿ -1;
    ಸ್ಥಿತಿ USTRINGTRIE_NO_ಮೌಲ್ಯ: ಹಿಂತಿರುಗಿ 0;
    ಸ್ಥಿತಿ USTRINGTRIE_INTERMEDIATE_ಮೌಲ್ಯ:
        U_ASSERT(iter.get_ಮೌಲ್ಯ() == SKIP_SCRIPT);
        ಹಿಂತಿರುಗಿ SKIP_SCRIPT;
    ಸ್ಥಿತಿ USTRINGTRIE_FINAL_ಮೌಲ್ಯ: ಹಿಂತಿರುಗಿ iter.get_ಮೌಲ್ಯ();
    ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    ತಾರ್ಕಿಕ favorScript,
                                    UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    LSR max = maximize(language, script, region, ನಿಜ, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    // If no match, return it.
    ಆದರೆ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ಹಿಂತಿರುಗಿ LSR(language, script, region, LSR::EXPLICIT_LSR, error_ಸಂಕೇತ);
    }
    // try language
    LSR test = maximize(max.language, "", "", ನಿಜ, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    ಆದರೆ (test.is_Equivalent_ಗಮ್ಯ(max)) {
        ಹಿಂತಿರುಗಿ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_ಸಂಕೇತ);
    }

    ಆದರೆ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, ನಿಜ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
        ಆದರೆ (test.is_Equivalent_ಗಮ್ಯ(max)) {
            ಹಿಂತಿರುಗಿ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ಸಂಕೇತ);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", ನಿಜ, error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
    ಆದರೆ (test.is_Equivalent_ಗಮ್ಯ(max)) {
        ಹಿಂತಿರುಗಿ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_ಸಂಕೇತ);
    }
    ಆದರೆ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, ನಿಜ, error_ಸಂಕೇತ);
        ಆದರೆ (U_FAILURE(error_ಸಂಕೇತ)) { ಹಿಂತಿರುಗಿ {}; }
        ಆದರೆ (test.is_Equivalent_ಗಮ್ಯ(max)) {
            ಹಿಂತಿರುಗಿ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ಸಂಕೇತ);
        }
    }
    ಹಿಂತಿರುಗಿ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_ಸಂಕೇತ);
}

U_NAMESPACE_ಕೊನೆ
