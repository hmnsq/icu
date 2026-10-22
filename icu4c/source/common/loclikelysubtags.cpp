// © 2019 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html

// loclikelysubtags.cpp
// created: 2019may08 Markus W. Scherer

#ਅਨ੍ਤਰ੍ਭਾਵ <utility>
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/bytestrie.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/localpointer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/locid.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uobject.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ures.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "charstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "loclikelysubtags.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lsr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uhash.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uinvchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uniquecharstr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresdata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uresimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uvector.h"

U_NAMESPACE_BEGIN

ਨਾਮਾਕਾਸ਼ {

constexpr ਅੱਖਰ PSEUDO_ACCENTS_ਅਗੇਤਰ = '\'';  // -XA, -PSACCENT
constexpr ਅੱਖਰ PSEUDO_BIDI_ਅਗੇਤਰ = '+';  // -XB, -PSBIDI
constexpr ਅੱਖਰ PSEUDO_CRACKED_ਅਗੇਤਰ = ',';  // -XC, -PSCRACK

}  // namespace

Locale_ਦੂਰੀ_ਅੰਕੜੇ::Locale_ਦੂਰੀ_ਅੰਕੜੇ(Locale_ਦੂਰੀ_ਅੰਕੜੇ &&ਅੰਕੜੇ) :
        ਦੂਰੀ_Trie_ਬਾਈਟਾਂ(ਅੰਕੜੇ.ਦੂਰੀ_Trie_ਬਾਈਟਾਂ),
        region_ਮੰਜ਼ਿਲ_Partitions(ਅੰਕੜੇ.region_ਮੰਜ਼ਿਲ_Partitions),
        partitions(ਅੰਕੜੇ.partitions),
        paradigms(ਅੰਕੜੇ.paradigms), paradigms_ਲੰਮਾਈ(ਅੰਕੜੇ.paradigms_ਲੰਮਾਈ),
        distances(ਅੰਕੜੇ.distances) {
    ਅੰਕੜੇ.partitions = ਖਾਲੀ_ਸੂਚਕ;
    ਅੰਕੜੇ.paradigms = ਖਾਲੀ_ਸੂਚਕ;
}

Locale_ਦੂਰੀ_ਅੰਕੜੇ::~Locale_ਦੂਰੀ_ਅੰਕੜੇ() {
    uprv_free(partitions);
    ਮਿਟਾਓ[] paradigms;
}

ਬਣਤਰ Likely_Subtags_ਅੰਕੜੇ {
    UResourceBundle *langInfoBundle = ਖਾਲੀ_ਸੂਚਕ;
    Unique_Char_ਲੜੀਆਂ ਲੜੀਆਂ;
    CharStringMap languageAliases;
    CharStringMap regionAliases;
    ਅਚਲ uint8_t *trie_ਬਾਈਟਾਂ = ਖਾਲੀ_ਸੂਚਕ;
    LSR *lsrs = ਖਾਲੀ_ਸੂਚਕ;
    int32_t lsrs_ਲੰਮਾਈ = 0;

    Locale_ਦੂਰੀ_ਅੰਕੜੇ ਦੂਰੀ_ਅੰਕੜੇ;

    Likely_Subtags_ਅੰਕੜੇ(UError_ਕੋਡ &error_ਕੋਡ) : ਲੜੀਆਂ(error_ਕੋਡ) {}

    ~Likely_Subtags_ਅੰਕੜੇ() {
        ures_close(langInfoBundle);
        ਮਿਟਾਓ[] lsrs;
    }

    ਖਾਲੀ load(UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        langInfoBundle = ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ, "langInfo", &error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
        ਢੇਰ_UResource_Bundle ਢੇਰ_ਆਰਜ਼ੀ_Bundle;
        Resource_ਅੰਕੜੇ_ਮੁੱਲ value;
        ures_get_ਮੁੱਲ_With_Fallback(langInfoBundle, "likely", ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(),
                                  value, error_ਕੋਡ);
        Resource_ਸਾਰਣੀ likely_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }

        // Read all strings in the resource bundle and convert them to invariant char *.
        Local_ਸਮ੍ਰਿਤੀ<int32_t> languageIndexes, regionIndexes, lsrSubtagIndexes;
        int32_t languages_ਲੰਮਾਈ = 0, regions_ਲੰਮਾਈ = 0, lsr_Subtags_ਲੰਮਾਈ = 0;
        ResourceArray m49Array;
        ਜੇ (likely_ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ("m49", value)) {
            m49Array = value.getArray(error_ਕੋਡ);
        } ਨਹੀਂਤਾਂ {
            error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
            ਵਾਪਸ;
        }
        ਜੇ (!read_ਲੜੀਆਂ(likely_ਸਾਰਣੀ, "languageAliases", value,
                         languageIndexes, languages_ਲੰਮਾਈ, error_ਕੋਡ) ||
                !read_ਲੜੀਆਂ(likely_ਸਾਰਣੀ, "regionAliases", value,
                             regionIndexes, regions_ਲੰਮਾਈ, error_ਕੋਡ) ||
                !read_LSREncoded_ਲੜੀਆਂ(likely_ਸਾਰਣੀ, "lsrnum", value, m49Array,
                             lsrSubtagIndexes,lsr_Subtags_ਲੰਮਾਈ, error_ਕੋਡ)) {
            ਵਾਪਸ;
        }
        ਜੇ ((languages_ਲੰਮਾਈ & 1) != 0 ||
                (regions_ਲੰਮਾਈ & 1) != 0 ||
                (lsr_Subtags_ਲੰਮਾਈ % 3) != 0) {
            error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
            ਵਾਪਸ;
        }
        ਜੇ (lsr_Subtags_ਲੰਮਾਈ == 0) {
            error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
            ਵਾਪਸ;
        }

        ਜੇ (!likely_ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ("trie", value)) {
            error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
            ਵਾਪਸ;
        }
        int32_t length;
        trie_ਬਾਈਟਾਂ = value.get_ਦੁਆਧਾਰੀ(length, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }

        // Also read distance/matcher data if available,
        // to open & keep only one resource bundle pointer
        // and to use one single UniqueCharStrings.
        UError_ਕੋਡ ਮੇਲ_Error_ਕੋਡ = U_ZERO_ERROR;
        ures_get_ਮੁੱਲ_With_Fallback(langInfoBundle, "match", ਢੇਰ_ਆਰਜ਼ੀ_Bundle.getAlias(),
                                  value, ਮੇਲ_Error_ਕੋਡ);
        Local_ਸਮ੍ਰਿਤੀ<int32_t> ਹਿੱਸੇ_ਕਰੋ_Indexes, paradigmSubtagIndexes;
        int32_t partitions_ਲੰਮਾਈ = 0, paradigm_Subtags_ਲੰਮਾਈ = 0;
        ਜੇ (U_SUCCESS(ਮੇਲ_Error_ਕੋਡ)) {
            Resource_ਸਾਰਣੀ ਮੇਲ_ਸਾਰਣੀ = value.get_ਸਾਰਣੀ(error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }

            ਜੇ (ਮੇਲ_ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ("trie", value)) {
                ਦੂਰੀ_ਅੰਕੜੇ.ਦੂਰੀ_Trie_ਬਾਈਟਾਂ = value.get_ਦੁਆਧਾਰੀ(length, error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
            }

            ਜੇ (ਮੇਲ_ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ("regionToPartitions", value)) {
                ਦੂਰੀ_ਅੰਕੜੇ.region_ਮੰਜ਼ਿਲ_Partitions = value.get_ਦੁਆਧਾਰੀ(length, error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
                ਜੇ (length < LSR::REGION_ਸੂਚਕਾਂਕ_ਹੱਦਬੰਦੀ) {
                    error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
                    ਵਾਪਸ;
                }
            }

            ਜੇ (!read_ਲੜੀਆਂ(ਮੇਲ_ਸਾਰਣੀ, "partitions", value,
                             ਹਿੱਸੇ_ਕਰੋ_Indexes, partitions_ਲੰਮਾਈ, error_ਕੋਡ) ||
                    !read_LSREncoded_ਲੜੀਆਂ(ਮੇਲ_ਸਾਰਣੀ, "paradigmnum", value, m49Array,
                                 paradigmSubtagIndexes, paradigm_Subtags_ਲੰਮਾਈ, error_ਕੋਡ)) {
                ਵਾਪਸ;
            }
            ਜੇ ((paradigm_Subtags_ਲੰਮਾਈ % 3) != 0) {
                error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
                ਵਾਪਸ;
            }

            ਜੇ (ਮੇਲ_ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ("distances", value)) {
                ਦੂਰੀ_ਅੰਕੜੇ.distances = value.get_Int_ਸਦਿਸ਼(length, error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
                ਜੇ (length < 4) {  // LocaleDistance IX_LIMIT
                    error_ਕੋਡ = U_INVALID_FORMAT_ERROR;
                    ਵਾਪਸ;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਮੇਲ_Error_ਕੋਡ == U_MISSING_RESOURCE_ERROR) {
            // ok for likely subtags
        } ਨਹੀਂਤਾਂ {  // error other than missing resource
            error_ਕੋਡ = ਮੇਲ_Error_ਕੋਡ;
            ਵਾਪਸ;
        }

        // Fetch & store invariant-character versions of strings
        // only after we have collected and de-duplicated all of them.
        ਲੜੀਆਂ.freeze();

        languageAliases = CharStringMap(languages_ਲੰਮਾਈ / 2, error_ਕੋਡ);
        ਲਈ (int32_t i = 0; i < languages_ਲੰਮਾਈ; i += 2) {
            languageAliases.put(ਲੜੀਆਂ.get(languageIndexes[i]),
                                ਲੜੀਆਂ.get(languageIndexes[i + 1]), error_ਕੋਡ);
        }

        regionAliases = CharStringMap(regions_ਲੰਮਾਈ / 2, error_ਕੋਡ);
        ਲਈ (int32_t i = 0; i < regions_ਲੰਮਾਈ; i += 2) {
            regionAliases.put(ਲੜੀਆਂ.get(regionIndexes[i]),
                              ਲੜੀਆਂ.get(regionIndexes[i + 1]), error_ਕੋਡ);
        }
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }

        lsrs_ਲੰਮਾਈ = lsr_Subtags_ਲੰਮਾਈ / 3;
        lsrs = ਨਵਾਂ LSR[lsrs_ਲੰਮਾਈ];
        ਜੇ (lsrs == ਖਾਲੀ_ਸੂਚਕ) {
            error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ;
        }
        ਲਈ (int32_t i = 0, j = 0; i < lsr_Subtags_ਲੰਮਾਈ; i += 3, ++j) {
            lsrs[j] = LSR(ਲੜੀਆਂ.get(lsrSubtagIndexes[i]),
                          ਲੜੀਆਂ.get(lsrSubtagIndexes[i + 1]),
                          ਲੜੀਆਂ.get(lsrSubtagIndexes[i + 2]),
                          LSR::IMPLICIT_LSR);
        }

        ਜੇ (partitions_ਲੰਮਾਈ > 0) {
            ਦੂਰੀ_ਅੰਕੜੇ.partitions = static_cast<ਅਚਲ ਅੱਖਰ **>(
                uprv_malloc(partitions_ਲੰਮਾਈ * ਆਕਾਰ(ਅਚਲ ਅੱਖਰ *)));
            ਜੇ (ਦੂਰੀ_ਅੰਕੜੇ.partitions == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            ਲਈ (int32_t i = 0; i < partitions_ਲੰਮਾਈ; ++i) {
                ਦੂਰੀ_ਅੰਕੜੇ.partitions[i] = ਲੜੀਆਂ.get(ਹਿੱਸੇ_ਕਰੋ_Indexes[i]);
            }
        }

        ਜੇ (paradigm_Subtags_ਲੰਮਾਈ > 0) {
            ਦੂਰੀ_ਅੰਕੜੇ.paradigms_ਲੰਮਾਈ = paradigm_Subtags_ਲੰਮਾਈ / 3;
            LSR *paradigms = ਨਵਾਂ LSR[ਦੂਰੀ_ਅੰਕੜੇ.paradigms_ਲੰਮਾਈ];
            ਜੇ (paradigms == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ;
            }
            ਲਈ (int32_t i = 0, j = 0; i < paradigm_Subtags_ਲੰਮਾਈ; i += 3, ++j) {
                paradigms[j] = LSR(ਲੜੀਆਂ.get(paradigmSubtagIndexes[i]),
                                   ਲੜੀਆਂ.get(paradigmSubtagIndexes[i + 1]),
                                   ਲੜੀਆਂ.get(paradigmSubtagIndexes[i + 2]),
                                   LSR::DONT_CARE_FLAGS);
            }
            ਦੂਰੀ_ਅੰਕੜੇ.paradigms = paradigms;
        }
    }

ਨਿਜੀ:
    ਤਾਰਕਿਕ read_ਲੜੀਆਂ(ਅਚਲ Resource_ਸਾਰਣੀ &ਸਾਰਣੀ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, Resource_ਮੁੱਲ &value,
                     Local_ਸਮ੍ਰਿਤੀ<int32_t> &indexes, int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
        ਜੇ (ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ(ਕੁੰਜੀ, value)) {
            ResourceArray stringArray = value.getArray(error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
            length = stringArray.get_ਮਾਪ();
            ਜੇ (length == 0) { ਵਾਪਸ ਸੱਚ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length);
            ਜੇ (rawIndexes == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਝੂਠ;
            }
            ਲਈ (ਪੂਰਨ i = 0; i < length; ++i) {
                ਜੇ (stringArray.get_ਮੁੱਲ(i, value)) {  // returns true because i < length
                    int32_t str_ਲੰਮਾਈ = 0;
                    rawIndexes[i] = ਲੜੀਆਂ.ਜੋੜੋ(value.getString(str_ਲੰਮਾਈ, error_ਕੋਡ), error_ਕੋਡ);
                    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
                }
            }
        }
        ਵਾਪਸ ਸੱਚ;
    }
    UnicodeString ਮੰਜ਼ਿਲ_Language(ਪੂਰਨ encoded) {
        ਜੇ (encoded == 0) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("");
        }
        ਜੇ (encoded == 1) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("skip");
        }
        encoded &= 0x00ffffff;
        encoded %= 27*27*27;
        ਅੱਖਰ lang[3];
        lang[0] = 'a' + ((encoded % 27) - 1);
        lang[1] = 'a' + (((encoded / 27 ) % 27) - 1);
        ਜੇ (encoded / (27 * 27) == 0) {
            ਵਾਪਸ UnicodeString(lang, 2, US_INV);
        }
        lang[2] = 'a' + ((encoded / (27 * 27)) - 1);
        ਵਾਪਸ UnicodeString(lang, 3, US_INV);
    }
    UnicodeString ਮੰਜ਼ਿਲ_Script(ਪੂਰਨ encoded) {
        ਜੇ (encoded == 0) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("");
        }
        ਜੇ (encoded == 1) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("script");
        }
        encoded = (encoded >> 24) & 0x000000ff;
        ਅਚਲ ਅੱਖਰ* script = uscript_getShortName(static_cast<UScript_ਕੋਡ>(encoded));
        ਜੇ (script == ਖਾਲੀ_ਸੂਚਕ) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("");
        }
        U_ASSERT(uprv_strlen(script) == 4);
        ਵਾਪਸ UnicodeString(script, 4, US_INV);
    }
    UnicodeString m49_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੋਡ(ਅਚਲ ResourceArray &m49Array, Resource_ਮੁੱਲ &value, ਪੂਰਨ ਸੂਚਕਾਂਕ, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("");
        }
        ਜੇ (m49Array.get_ਮੁੱਲ(ਸੂਚਕਾਂਕ, value)) {
            ਵਾਪਸ value.getUnicodeString(error_ਕੋਡ);
        }
        // "m49" does not include the index.
        error_ਕੋਡ = U_MISSING_RESOURCE_ERROR;
        ਵਾਪਸ UNICODE_STRING_SIMPLE("");
    }

    UnicodeString ਮੰਜ਼ਿਲ_Region(ਅਚਲ ResourceArray& m49Array, Resource_ਮੁੱਲ &value, ਪੂਰਨ encoded, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ) || encoded == 0 || encoded == 1) {
            ਵਾਪਸ UNICODE_STRING_SIMPLE("");
        }
        encoded &= 0x00ffffff;
        encoded /= 27 * 27 * 27;
        encoded %= 27 * 27;
        ਜੇ (encoded < 27) {
            // Selected M49 code index, find the code from "m49" resource.
            ਵਾਪਸ  m49_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਕੋਡ(m49Array, value, encoded, error_ਕੋਡ);
        }
        ਅੱਖਰ region[2];
        region[0] = 'A' + ((encoded % 27) - 1);
        region[1] = 'A' + (((encoded / 27) % 27) - 1);
        ਵਾਪਸ UnicodeString(region, 2, US_INV);
    }

    ਤਾਰਕਿਕ read_LSREncoded_ਲੜੀਆਂ(ਅਚਲ Resource_ਸਾਰਣੀ &ਸਾਰਣੀ, ਅਚਲ ਅੱਖਰ* ਕੁੰਜੀ, Resource_ਮੁੱਲ &value, ਅਚਲ ResourceArray& m49Array,
                     Local_ਸਮ੍ਰਿਤੀ<int32_t> &indexes, int32_t &length, UError_ਕੋਡ &error_ਕੋਡ) {
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
        ਜੇ (ਸਾਰਣੀ.ਲੱਭੋ_ਮੁੱਲ(ਕੁੰਜੀ, value)) {
            ਅਚਲ int32_t* vectors = value.get_Int_ਸਦਿਸ਼(length, error_ਕੋਡ);
            ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
            ਜੇ (length == 0) { ਵਾਪਸ ਸੱਚ; }
            int32_t *rawIndexes = indexes.allocateInsteadAndCopy(length * 3);
            ਜੇ (rawIndexes == ਖਾਲੀ_ਸੂਚਕ) {
                error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
                ਵਾਪਸ ਝੂਠ;
            }
            ਲਈ (ਪੂਰਨ i = 0; i < length; ++i) {
                rawIndexes[i*3] = ਲੜੀਆਂ.ਜੋੜੋ_By_ਮੁੱਲ(ਮੰਜ਼ਿਲ_Language(vectors[i]), error_ਕੋਡ);
                rawIndexes[i*3+1] = ਲੜੀਆਂ.ਜੋੜੋ_By_ਮੁੱਲ(ਮੰਜ਼ਿਲ_Script(vectors[i]), error_ਕੋਡ);
                rawIndexes[i*3+2] = ਲੜੀਆਂ.ਜੋੜੋ_By_ਮੁੱਲ(
                    ਮੰਜ਼ਿਲ_Region(m49Array, value, vectors[i], error_ਕੋਡ), error_ਕੋਡ);
                ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
            }
            length *= 3;
        }
        ਵਾਪਸ ਸੱਚ;
    }
};

ਨਾਮਾਕਾਸ਼ {

LikelySubtags *gLikelySubtags = ਖਾਲੀ_ਸੂਚਕ;
UVector *gMacroregions = ਖਾਲੀ_ਸੂਚਕ;
UInitOnce gInitOnce {};

UBool U_CALLCONV cleanup() {
    ਮਿਟਾਓ gLikelySubtags;
    gLikelySubtags = ਖਾਲੀ_ਸੂਚਕ;
    ਮਿਟਾਓ gMacroregions;
    gMacroregions = ਖਾਲੀ_ਸੂਚਕ;
    gInitOnce.reset();
    ਵਾਪਸ ਸੱਚ;
}

constexpr ਅਚਲ char16_t* MACROREGION_HARDCODE[] = {
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
ਖਾਲੀ processMacroregionRange(ਅਚਲ UnicodeString& regionName, UVector* newMacroRegions, UError_ਕੋਡ& ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ; }
    int32_t rangeMarkerLocation = regionName.ਸੂਚਕਾਂਕ_ਦਾ(RANGE_MARKER);
    char16_t buf[6];
    regionName.extract(buf,6,ਸਥਿਤੀ);
    ਜੇ ( rangeMarkerLocation > 0 ) {
        char16_t ਸਿਰਾ_Range = regionName.charAt(rangeMarkerLocation+1);
        buf[rangeMarkerLocation] = 0;
        ਜਦੋਂ ( buf[rangeMarkerLocation-1] <= ਸਿਰਾ_Range && U_SUCCESS(ਸਥਿਤੀ)) {
            Local_ਸੂਚਕ<UnicodeString> newRegion(ਨਵਾਂ UnicodeString(buf), ਸਥਿਤੀ);
            newMacroRegions->adoptElement(newRegion.orphan(),ਸਥਿਤੀ);
            buf[rangeMarkerLocation-1]++;
        }
    } ਨਹੀਂਤਾਂ {
        Local_ਸੂਚਕ<UnicodeString> newRegion(ਨਵਾਂ UnicodeString(regionName), ਸਥਿਤੀ);
        newMacroRegions->adoptElement(newRegion.orphan(),ਸਥਿਤੀ);
    }
}

#ਜੇ U_DEBUG
UVector* ਲੱਦੋ_Macroregions(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    Local_ਸੂਚਕ<UVector> newMacroRegions(ਨਵਾਂ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ਸਥਿਤੀ), ਸਥਿਤੀ);

    Local_UResource_Bundle_ਸੂਚਕ supplemental_ਅੰਕੜੇ(ures_open_ਸਿੱਧਾ(ਖਾਲੀ_ਸੂਚਕ,"supplementalData",&ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ idValidity(ures_get_By_ਕੁੰਜੀ(supplemental_ਅੰਕੜੇ.getAlias(),"idValidity",ਖਾਲੀ_ਸੂਚਕ,&ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ regionList(ures_get_By_ਕੁੰਜੀ(idValidity.getAlias(),"region",ਖਾਲੀ_ਸੂਚਕ,&ਸਥਿਤੀ));
    Local_UResource_Bundle_ਸੂਚਕ regionMacro(ures_get_By_ਕੁੰਜੀ(regionList.getAlias(),"macroregion",ਖਾਲੀ_ਸੂਚਕ,&ਸਥਿਤੀ));

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜਦੋਂ (ures_has_ਅਗਲਾ(regionMacro.getAlias())) {
        UnicodeString regionName = ures_get_ਅਗਲਾ_Unicode_String(regionMacro.getAlias(),ਖਾਲੀ_ਸੂਚਕ,&ਸਥਿਤੀ);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਵਾਪਸ newMacroRegions.orphan();
}
#ਜੇ_ਅੰਤ // U_DEBUG

UVector* getStaticMacroregions(UError_ਕੋਡ &ਸਥਿਤੀ) {
    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    Local_ਸੂਚਕ<UVector> newMacroRegions(ਨਵਾਂ UVector(uprv_deleteUObject, uhash_compareUnicodeString, ਸਥਿਤੀ), ਸਥਿਤੀ);

    ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ *region : MACROREGION_HARDCODE) {
        UnicodeString regionName(region);
        processMacroregionRange(regionName, newMacroRegions.getAlias(), ਸਥਿਤੀ);
        ਜੇ (U_FAILURE(ਸਥਿਤੀ)) {
            ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
        }
    }

    ਵਾਪਸ newMacroRegions.orphan();
}

}  // namespace

ਖਾਲੀ U_CALLCONV LikelySubtags::initLikelySubtags(UError_ਕੋਡ &error_ਕੋਡ) {
    // This function is invoked only via umtx_initOnce().
    U_ASSERT(gLikelySubtags == ਖਾਲੀ_ਸੂਚਕ);
    Likely_Subtags_ਅੰਕੜੇ ਅੰਕੜੇ(error_ਕੋਡ);
    ਅੰਕੜੇ.load(error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ; }
    gLikelySubtags = ਨਵਾਂ LikelySubtags(ਅੰਕੜੇ);
    gMacroregions = getStaticMacroregions(error_ਕੋਡ);
#ਜੇ U_DEBUG
    ਸਵੈਚਾਲਿਤ macroregions_From_ਅੰਕੜੇ = ਲੱਦੋ_Macroregions(error_ਕੋਡ);
    U_ASSERT((*gMacroregions) == (*macroregions_From_ਅੰਕੜੇ));
    ਮਿਟਾਓ macroregions_From_ਅੰਕੜੇ;
#ਜੇ_ਅੰਤ
    ਜੇ (U_FAILURE(error_ਕੋਡ) || gLikelySubtags == ਖਾਲੀ_ਸੂਚਕ || gMacroregions == ਖਾਲੀ_ਸੂਚਕ) {
        ਮਿਟਾਓ gLikelySubtags;
        ਮਿਟਾਓ gMacroregions;
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }

    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_LIKELY_SUBTAGS, cleanup);
}

ਅਚਲ LikelySubtags *LikelySubtags::getSingleton(UError_ਕੋਡ &error_ਕੋਡ) {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ਕੋਡ);
    ਵਾਪਸ gLikelySubtags;
}

LikelySubtags::LikelySubtags(Likely_Subtags_ਅੰਕੜੇ &ਅੰਕੜੇ) :
        langInfoBundle(ਅੰਕੜੇ.langInfoBundle),
        ਲੜੀਆਂ(ਅੰਕੜੇ.ਲੜੀਆਂ.orphan_Char_ਲੜੀਆਂ()),
        languageAliases(std::move(ਅੰਕੜੇ.languageAliases)),
        regionAliases(std::move(ਅੰਕੜੇ.regionAliases)),
        trie(ਅੰਕੜੇ.trie_ਬਾਈਟਾਂ),
        lsrs(ਅੰਕੜੇ.lsrs),
#ਜੇ U_DEBUG
        lsrs_ਲੰਮਾਈ(ਅੰਕੜੇ.lsrs_ਲੰਮਾਈ),
#ਜੇ_ਅੰਤ // U_DEBUG
        ਦੂਰੀ_ਅੰਕੜੇ(std::move(ਅੰਕੜੇ.ਦੂਰੀ_ਅੰਕੜੇ)) {
    ਅੰਕੜੇ.langInfoBundle = ਖਾਲੀ_ਸੂਚਕ;
    ਅੰਕੜੇ.lsrs = ਖਾਲੀ_ਸੂਚਕ;

    // Cache the result of looking up language="und" encoded as "*", and "und-Zzzz" ("**").
    UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ));
    trieUndState = trie.getState64();
    ਨਤੀਜਾ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ਅਗਲਾ(ਨਤੀਜਾ));
    trieUndZzzzState = trie.getState64();
    ਨਤੀਜਾ = trie.next(u'*');
    U_ASSERT(USTRINGTRIE_HAS_ਮੁੱਲ(ਨਤੀਜਾ));
    default_Lsr_ਸੂਚਕਾਂਕ = trie.get_ਮੁੱਲ();
    trie.reset();

    ਲਈ (char16_t c = u'a'; c <= u'z'; ++c) {
        ਨਤੀਜਾ = trie.next(c);
        ਜੇ (ਨਤੀਜਾ == USTRINGTRIE_NO_ਮੁੱਲ) {
            trie_ਪਹਿਲਾ_Letter_ਹਾਲਤਾਂ[c - u'a'] = trie.getState64();
        }
        trie.reset();
    }
}

LikelySubtags::~LikelySubtags() {
    ures_close(langInfoBundle);
    ਮਿਟਾਓ ਲੜੀਆਂ;
    ਮਿਟਾਓ[] lsrs;
}

LSR LikelySubtags::makeMaximizedLsrFrom(ਅਚਲ Locale &locale,
                                         ਤਾਰਕਿਕ return_ਨਿਵੇਸ਼_If_Unmatch,
                                         UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    ਜੇ (locale.isBogus()) {
        error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ {};
    }
    ਅਚਲ ਅੱਖਰ *name = locale.getName();
    ਜੇ (!return_ਨਿਵੇਸ਼_If_Unmatch && uprv_isAtSign(name[0]) && name[1] == 'x' && name[2] == '=') {  // name.startsWith("@x=")
        // Private use language tag x-subtag-subtag... which CLDR changes to
        // und-x-subtag-subtag...
        ਵਾਪਸ LSR(name, "", "", LSR::EXPLICIT_LSR);
    }
    LSR max = makeMaximizedLsr(locale.getLanguage(), locale.getScript(), locale.getCountry(),
                            locale.getVariant(), return_ਨਿਵੇਸ਼_If_Unmatch, error_ਕੋਡ);

    ਜੇ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // If the provided ULocale instance is already in the maximal form, or
        // there is no data available available for maximization, it will be
        // returned.
        ਵਾਪਸ LSR(locale.getLanguage(), locale.getScript(), locale.getCountry(), LSR::EXPLICIT_LSR, error_ਕੋਡ);
    }
    ਵਾਪਸ max;
}

ਨਾਮਾਕਾਸ਼ {

ਅਚਲ ਅੱਖਰ *getCanonical(ਅਚਲ CharStringMap &aliases, ਅਚਲ ਅੱਖਰ *alias) {
    ਅਚਲ ਅੱਖਰ *canonical = aliases.get(alias);
    ਵਾਪਸ canonical == ਖਾਲੀ_ਸੂਚਕ ? alias : canonical;
}

}  // namespace

LSR LikelySubtags::makeMaximizedLsr(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script, ਅਚਲ ਅੱਖਰ *region,
                                     ਅਚਲ ਅੱਖਰ *variant,
                                     ਤਾਰਕਿਕ return_ਨਿਵੇਸ਼_If_Unmatch,
                                     UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    // Handle pseudolocales like en-XA, ar-XB, fr-PSCRACK.
    // They should match only themselves,
    // not other locales with what looks like the same language and script subtags.
    ਜੇ (!return_ਨਿਵੇਸ਼_If_Unmatch) {
        ਅੱਖਰ c1;
        ਜੇ (region[0] == 'X' && (c1 = region[1]) != 0 && region[2] == 0) {
            ਚੋਣ (c1) {
            ਹਾਲਤ 'A':
                ਵਾਪਸ LSR(PSEUDO_ACCENTS_ਅਗੇਤਰ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ਕੋਡ);
            ਹਾਲਤ 'B':
                ਵਾਪਸ LSR(PSEUDO_BIDI_ਅਗੇਤਰ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ਕੋਡ);
            ਹਾਲਤ 'C':
                ਵਾਪਸ LSR(PSEUDO_CRACKED_ਅਗੇਤਰ, language, script, region,
                           LSR::EXPLICIT_LSR, error_ਕੋਡ);
            ਮੂਲ:  // normal locale
                ਰੋਕੋ;
            }
        }

        ਜੇ (variant[0] == 'P' && variant[1] == 'S') {
            int32_t lsrFlags = *region == 0 ?
                LSR::EXPLICIT_LANGUAGE | LSR::EXPLICIT_SCRIPT : LSR::EXPLICIT_LSR;
            ਜੇ (uprv_strcmp(variant, "PSACCENT") == 0) {
                ਵਾਪਸ LSR(PSEUDO_ACCENTS_ਅਗੇਤਰ, language, script,
                           *region == 0 ? "XA" : region, lsrFlags, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(variant, "PSBIDI") == 0) {
                ਵਾਪਸ LSR(PSEUDO_BIDI_ਅਗੇਤਰ, language, script,
                           *region == 0 ? "XB" : region, lsrFlags, error_ਕੋਡ);
            } ਨਹੀਂਤਾਂ ਜੇ (uprv_strcmp(variant, "PSCRACK") == 0) {
                ਵਾਪਸ LSR(PSEUDO_CRACKED_ਅਗੇਤਰ, language, script,
                           *region == 0 ? "XC" : region, lsrFlags, error_ਕੋਡ);
            }
            // else normal locale
        }
    } // end of if (!returnInputIfUnmatch)

    language = getCanonical(languageAliases, language);
    // (We have no script mappings.)
    region = getCanonical(regionAliases, region);
    ਵਾਪਸ maximize(language, script, region, return_ਨਿਵੇਸ਼_If_Unmatch, error_ਕੋਡ);
}

LSR LikelySubtags::maximize(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script, ਅਚਲ ਅੱਖਰ *region,
                             ਤਾਰਕਿਕ return_ਨਿਵੇਸ਼_If_Unmatch,
                             UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    ਵਾਪਸ maximize({language, static_cast<int32_t>(uprv_strlen(language))},
                    {script, static_cast<int32_t>(uprv_strlen(script))},
                    {region, static_cast<int32_t>(uprv_strlen(region))},
                    return_ਨਿਵੇਸ਼_If_Unmatch,
                    error_ਕੋਡ);
}

ਤਾਰਕਿਕ LikelySubtags::isMacroregion(StringPiece& region, UError_ਕੋਡ& error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    // In Java, we use Region class. In C++, since Region is under i18n,
    // we read the same data used by Region into gMacroregions avoid dependency
    // from common to i18n/region.cpp
    umtx_initOnce(gInitOnce, &LikelySubtags::initLikelySubtags, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ ਝੂਠ; }
    UnicodeString str(UnicodeString::fromUTF8(region));
    ਵਾਪਸ gMacroregions->ਸ਼ਾਮਲ_ਹੈ((ਖਾਲੀ *)&str);
}

LSR LikelySubtags::maximize(StringPiece language, StringPiece script, StringPiece region,
                             ਤਾਰਕਿਕ return_ਨਿਵੇਸ਼_If_Unmatch,
                             UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    ਜੇ (language.compare("und") == 0) {
        language = "";
    }
    ਜੇ (script.compare("Zzzz") == 0) {
        script = "";
    }
    ਜੇ (region.compare("ZZ") == 0) {
        region = "";
    }
    ਜੇ (!script.empty() && !region.empty() && !language.empty()) {
        ਵਾਪਸ LSR(language, script, region, LSR::EXPLICIT_LSR, error_ਕੋਡ);  // already maximized
    }
    ਤਾਰਕਿਕ retainLanguage = ਝੂਠ;
    ਤਾਰਕਿਕ retainScript = ਝੂਠ;
    ਤਾਰਕਿਕ retainRegion = ਝੂਠ;

    ਬਾਈਟਾਂ_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ਜੇ (0 <= (c0 = uprv_ਹੇਠਲਾ_Ordinal(language.ਅੰਕੜੇ()[0])) && c0 <= 25 &&
            language.length() >= 2 &&
            (state = trie_ਪਹਿਲਾ_Letter_ਹਾਲਤਾਂ[c0]) != 0) {
        value = trie_ਅਗਲਾ(iter.reset_ਮੰਜ਼ਿਲ_State64(state), language, 1);
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, language, 0);
    }
    ਤਾਰਕਿਕ ਮੇਲ_Language = (value >= 0);
    ਤਾਰਕਿਕ ਮੇਲ_Script = ਝੂਠ;
    ਜੇ (value >= 0) {
        retainLanguage = !language.empty();
        state = iter.getState64();
    } ਨਹੀਂਤਾਂ {
        retainLanguage = ਸੱਚ;
        iter.reset_ਮੰਜ਼ਿਲ_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ਜੇ (value >= 0 && !script.empty()) {
        ਮੇਲ_Script = ਸੱਚ;
    }
    ਜੇ (value > 0) {
        // Intermediate or final value from just language.
        ਜੇ (value == SKIP_SCRIPT) {
            value = 0;
        }
        retainScript = !script.empty();
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, script, 0);
        ਜੇ (value >= 0) {
            retainScript = !script.empty();
            state = iter.getState64();
        } ਨਹੀਂਤਾਂ {
            retainScript = ਸੱਚ;
            ਜੇ (state == 0) {
                iter.reset_ਮੰਜ਼ਿਲ_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ਨਹੀਂਤਾਂ {
                iter.reset_ਮੰਜ਼ਿਲ_State64(state);
                value = trie_ਅਗਲਾ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ਤਾਰਕਿਕ ਮੇਲ_Region = ਝੂਠ;
    ਜੇ (value > 0) {
        // Final value from just language or language+script.
        retainRegion = !region.empty();
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, region, 0);
        ਜੇ (value >= 0) {
            ਜੇ (!region.empty() && !isMacroregion(region, error_ਕੋਡ)) {
                retainRegion = ਸੱਚ;
                ਮੇਲ_Region = ਸੱਚ;
            }
        } ਨਹੀਂਤਾਂ {
            retainRegion = ਸੱਚ;
            ਜੇ (state == 0) {
                value = default_Lsr_ਸੂਚਕਾਂਕ;
            } ਨਹੀਂਤਾਂ {
                iter.reset_ਮੰਜ਼ਿਲ_State64(state);
                value = trie_ਅਗਲਾ(iter, "", 0);
                U_ASSERT(value != 0);
                // For the case of und_Latn
                ਜੇ (value < 0) {
                    retainLanguage = !language.empty();
                    retainScript = !script.empty();
                    retainRegion = !region.empty();
                    // Fallback to und_$region =>
                    iter.reset_ਮੰਜ਼ਿਲ_State64(trieUndState);  // "und" ("*")
                    value = trie_ਅਗਲਾ(iter, "", 0);
                    U_ASSERT(value == 0);
                    int64_t trie_Und_ਖਾਲੀ_ਥਾਵਾਂ_State = iter.getState64();
                    value = trie_ਅਗਲਾ(iter, region, 0);
                    // Fallback to und =>
                    ਜੇ (value < 0) {
                        iter.reset_ਮੰਜ਼ਿਲ_State64(trie_Und_ਖਾਲੀ_ਥਾਵਾਂ_State);
                        value = trie_ਅਗਲਾ(iter, "", 0);
                        U_ASSERT(value > 0);
                    }
                }
            }
        }
    }
    U_ASSERT(value < lsrs_ਲੰਮਾਈ);
    ਜੇ (return_ਨਿਵੇਸ਼_If_Unmatch &&
        (!(ਮੇਲ_Language || ਮੇਲ_Script || (ਮੇਲ_Region && language.empty())))) {
      ਵਾਪਸ LSR("", "", "", LSR::EXPLICIT_LSR, error_ਕੋਡ);  // no matching.
    }
    ਜੇ (language.empty()) {
        language = StringPiece("und");
    }

    ਜੇ (!(retainLanguage || retainScript || retainRegion)) {
        U_ASSERT(value >= 0);
        // Quickly return a copy of the lookup-result LSR
        // without new allocation of the subtags.
        ਅਚਲ LSR &matched = lsrs[value];
        ਵਾਪਸ LSR(matched.language, matched.script, matched.region, matched.flags);
    }
    ਜੇ (!retainLanguage) {
        U_ASSERT(value >= 0);
        language = lsrs[value].language;
    }
    ਜੇ (!retainScript) {
        U_ASSERT(value >= 0);
        script = lsrs[value].script;
    }
    ਜੇ (!retainRegion) {
        U_ASSERT(value >= 0);
        region = lsrs[value].region;
    }
    int32_t retain_ਨਕਾਬ = (retainLanguage ? 4 : 0) + (retainScript ? 2 : 0) + (retainRegion ? 1 : 0);
    // retainOldMask flags = LSR explicit-subtag flags
    ਵਾਪਸ LSR(language, script, region, retain_ਨਕਾਬ, error_ਕੋਡ);
}

int32_t LikelySubtags::compareLikely(ਅਚਲ LSR &lsr, ਅਚਲ LSR &other, int32_t likelyInfo) ਅਚਲ {
    // If likelyInfo >= 0:
    // likelyInfo bit 1 is set if the previous comparison with lsr
    // was for equal language and script.
    // Otherwise the scripts differed.
    ਜੇ (uprv_strcmp(lsr.language, other.language) != 0) {
        ਵਾਪਸ 0xfffffffc;  // negative, lsr not better than other
    }
    ਜੇ (uprv_strcmp(lsr.script, other.script) != 0) {
        int32_t ਸੂਚਕਾਂਕ;
        ਜੇ (likelyInfo >= 0 && (likelyInfo & 2) == 0) {
            ਸੂਚਕਾਂਕ = likelyInfo >> 2;
        } ਨਹੀਂਤਾਂ {
            ਸੂਚਕਾਂਕ = get_Likely_ਸੂਚਕਾਂਕ(lsr.language, "");
            likelyInfo = ਸੂਚਕਾਂਕ << 2;
        }
        ਅਚਲ LSR &likely = lsrs[ਸੂਚਕਾਂਕ];
        ਜੇ (uprv_strcmp(lsr.script, likely.script) == 0) {
            ਵਾਪਸ likelyInfo | 1;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ likelyInfo & ~1;
        }
    }
    ਜੇ (uprv_strcmp(lsr.region, other.region) != 0) {
        int32_t ਸੂਚਕਾਂਕ;
        ਜੇ (likelyInfo >= 0 && (likelyInfo & 2) != 0) {
            ਸੂਚਕਾਂਕ = likelyInfo >> 2;
        } ਨਹੀਂਤਾਂ {
            ਸੂਚਕਾਂਕ = get_Likely_ਸੂਚਕਾਂਕ(lsr.language, lsr.region);
            likelyInfo = (ਸੂਚਕਾਂਕ << 2) | 2;
        }
        ਅਚਲ LSR &likely = lsrs[ਸੂਚਕਾਂਕ];
        ਜੇ (uprv_strcmp(lsr.region, likely.region) == 0) {
            ਵਾਪਸ likelyInfo | 1;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ likelyInfo & ~1;
        }
    }
    ਵਾਪਸ likelyInfo & ~1;  // lsr not better than other
}

// Subset of maximize().
int32_t LikelySubtags::get_Likely_ਸੂਚਕਾਂਕ(ਅਚਲ ਅੱਖਰ *language, ਅਚਲ ਅੱਖਰ *script) ਅਚਲ {
    ਜੇ (uprv_strcmp(language, "und") == 0) {
        language = "";
    }
    ਜੇ (uprv_strcmp(script, "Zzzz") == 0) {
        script = "";
    }

    ਬਾਈਟਾਂ_Trie iter(trie);
    uint64_t state;
    int32_t value;
    // Small optimization: Array lookup for first language letter.
    int32_t c0;
    ਜੇ (0 <= (c0 = uprv_ਹੇਠਲਾ_Ordinal(language[0])) && c0 <= 25 &&
            language[1] != 0 &&  // language.length() >= 2
            (state = trie_ਪਹਿਲਾ_Letter_ਹਾਲਤਾਂ[c0]) != 0) {
        value = trie_ਅਗਲਾ(iter.reset_ਮੰਜ਼ਿਲ_State64(state), language, 1);
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, language, 0);
    }
    ਜੇ (value >= 0) {
        state = iter.getState64();
    } ਨਹੀਂਤਾਂ {
        iter.reset_ਮੰਜ਼ਿਲ_State64(trieUndState);  // "und" ("*")
        state = 0;
    }

    ਜੇ (value > 0) {
        // Intermediate or final value from just language.
        ਜੇ (value == SKIP_SCRIPT) {
            value = 0;
        }
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, script, 0);
        ਜੇ (value >= 0) {
            state = iter.getState64();
        } ਨਹੀਂਤਾਂ {
            ਜੇ (state == 0) {
                iter.reset_ਮੰਜ਼ਿਲ_State64(trieUndZzzzState);  // "und-Zzzz" ("**")
            } ਨਹੀਂਤਾਂ {
                iter.reset_ਮੰਜ਼ਿਲ_State64(state);
                value = trie_ਅਗਲਾ(iter, "", 0);
                U_ASSERT(value >= 0);
                state = iter.getState64();
            }
        }
    }

    ਜੇ (value > 0) {
        // Final value from just language or language+script.
    } ਨਹੀਂਤਾਂ {
        value = trie_ਅਗਲਾ(iter, "", 0);
        U_ASSERT(value > 0);
    }
    U_ASSERT(value < lsrs_ਲੰਮਾਈ);
    ਵਾਪਸ value;
}

int32_t LikelySubtags::trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, ਅਚਲ ਅੱਖਰ *s, int32_t i) {
    UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ;
    uint8_t c;
    ਜੇ ((c = s[i]) == 0) {
        ਨਤੀਜਾ = iter.next(u'*');
    } ਨਹੀਂਤਾਂ {
        ਲਈ (;;) {
            c = uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            uint8_t next = s[++i];
            ਜੇ (next != 0) {
                ਜੇ (!USTRINGTRIE_HAS_ਅਗਲਾ(iter.next(c))) {
                    ਵਾਪਸ -1;
                }
            } ਨਹੀਂਤਾਂ {
                // last character of this subtag
                ਨਤੀਜਾ = iter.next(c | 0x80);
                ਰੋਕੋ;
            }
            c = next;
        }
    }
    ਚੋਣ (ਨਤੀਜਾ) {
    ਹਾਲਤ USTRINGTRIE_NO_ਮੇਲ: ਵਾਪਸ -1;
    ਹਾਲਤ USTRINGTRIE_NO_ਮੁੱਲ: ਵਾਪਸ 0;
    ਹਾਲਤ USTRINGTRIE_INTERMEDIATE_ਮੁੱਲ:
        U_ASSERT(iter.get_ਮੁੱਲ() == SKIP_SCRIPT);
        ਵਾਪਸ SKIP_SCRIPT;
    ਹਾਲਤ USTRINGTRIE_FINAL_ਮੁੱਲ: ਵਾਪਸ iter.get_ਮੁੱਲ();
    ਮੂਲ: ਵਾਪਸ -1;
    }
}
int32_t LikelySubtags::trie_ਅਗਲਾ(ਬਾਈਟਾਂ_Trie &iter, StringPiece s, int32_t i) {
    UString_Trie_ਨਤੀਜਾ ਨਤੀਜਾ;
    uint8_t c;
    ਜੇ (s.length() == i) {
        ਨਤੀਜਾ = iter.next(u'*');
    } ਨਹੀਂਤਾਂ {
        c = s.ਅੰਕੜੇ()[i];
        ਲਈ (;;) {
            c = uprv_inv_Char_ਮੰਜ਼ਿਲ_Ascii(c);
            // EBCDIC: If s[i] is not an invariant character,
            // then c is now 0 and will simply not match anything, which is harmless.
            ਜੇ (i+1 != s.length()) {
                ਜੇ (!USTRINGTRIE_HAS_ਅਗਲਾ(iter.next(c))) {
                    ਵਾਪਸ -1;
                }
                c = s.ਅੰਕੜੇ()[++i];
            } ਨਹੀਂਤਾਂ {
                // last character of this subtag
                ਨਤੀਜਾ = iter.next(c | 0x80);
                ਰੋਕੋ;
            }
        }
    }
    ਚੋਣ (ਨਤੀਜਾ) {
    ਹਾਲਤ USTRINGTRIE_NO_ਮੇਲ: ਵਾਪਸ -1;
    ਹਾਲਤ USTRINGTRIE_NO_ਮੁੱਲ: ਵਾਪਸ 0;
    ਹਾਲਤ USTRINGTRIE_INTERMEDIATE_ਮੁੱਲ:
        U_ASSERT(iter.get_ਮੁੱਲ() == SKIP_SCRIPT);
        ਵਾਪਸ SKIP_SCRIPT;
    ਹਾਲਤ USTRINGTRIE_FINAL_ਮੁੱਲ: ਵਾਪਸ iter.get_ਮੁੱਲ();
    ਮੂਲ: ਵਾਪਸ -1;
    }
}

LSR LikelySubtags::minimizeSubtags(StringPiece language, StringPiece script,
                                    StringPiece region,
                                    ਤਾਰਕਿਕ favorScript,
                                    UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    LSR max = maximize(language, script, region, ਸੱਚ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    // If no match, return it.
    ਜੇ (uprv_strlen(max.language) == 0 &&
        uprv_strlen(max.script) == 0 &&
        uprv_strlen(max.region) == 0) {
        // No match. ICU API mandate us to
        // "If this Locale is already in the minimal form, or not valid, or
        // there is no data available for minimization, the Locale will be
        // unchanged."
        ਵਾਪਸ LSR(language, script, region, LSR::EXPLICIT_LSR, error_ਕੋਡ);
    }
    // try language
    LSR test = maximize(max.language, "", "", ਸੱਚ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    ਜੇ (test.is_Equivalent_ਮੰਜ਼ਿਲ(max)) {
        ਵਾਪਸ LSR(max.language, "", "", LSR::DONT_CARE_FLAGS, error_ਕੋਡ);
    }

    ਜੇ (!favorScript) {
        // favor Region
        // try language and region
        test = maximize(max.language, "", max.region, ਸੱਚ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
        ਜੇ (test.is_Equivalent_ਮੰਜ਼ਿਲ(max)) {
            ਵਾਪਸ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ਕੋਡ);
        }
    }
    // try language and script
    test = maximize(max.language, max.script, "", ਸੱਚ, error_ਕੋਡ);
    ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
    ਜੇ (test.is_Equivalent_ਮੰਜ਼ਿਲ(max)) {
        ਵਾਪਸ LSR(max.language, max.script, "", LSR::DONT_CARE_FLAGS, error_ਕੋਡ);
    }
    ਜੇ (favorScript) {
        // try language and region
        test = maximize(max.language, "", max.region, ਸੱਚ, error_ਕੋਡ);
        ਜੇ (U_FAILURE(error_ਕੋਡ)) { ਵਾਪਸ {}; }
        ਜੇ (test.is_Equivalent_ਮੰਜ਼ਿਲ(max)) {
            ਵਾਪਸ LSR(max.language, "", max.region, LSR::DONT_CARE_FLAGS, error_ਕੋਡ);
        }
    }
    ਵਾਪਸ LSR(max.language, max.script, max.region, LSR::DONT_CARE_FLAGS, error_ਕੋਡ);
}

U_NAMESPACE_ਸਿਰਾ
