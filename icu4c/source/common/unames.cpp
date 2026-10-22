// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 1999-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  unames.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 1999oct04
*   created by: Markus W. Scherer
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "unicode/uchar.h"
#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"
#ಅನ್ತರ್ಭಾವ "udataswp.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ದತ್ತಾಂಶ_NAME[] = "unames";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ದತ್ತಾಂಶ_TYPE[] = "icu";

#ವ್ಯಾಖ್ಯಾನಿಸು ಗುಂಪು_ಸರಿಸು 5
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು (1L<<ಗುಂಪು_ಸರಿಸು)
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುಂಪು_ಮುಖವಾಡ (ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು-1)

/*
 * This struct was replaced by explicitly accessing equivalent
 * fields from triples of uint16_t.
 * The Group struct was padded to 8 bytes on compilers for early ARM CPUs,
 * which broke the assumption that sizeof(Group)==6 and that the ++ operator
 * would advance by 6 bytes (3 uint16_t).
 *
 * We can't just change the data structure because it's loaded from a data file,
 * and we don't want to make it less compact, so we changed the access code.
 *
 * For details see ICU tickets 6331 and 6008.
typedef struct {
    uint16_t groupMSB,
             offsetHigh, offsetLow; / * avoid padding * /
} Group;
 */
ಎಣಿಕೆ {
    ಗುಂಪು_MSB,
    ಗುಂಪು_ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ,
    ಗುಂಪು_ಸ್ಥಾನಾಂತರ_ಕೆಳಗಿನ,
    ಗುಂಪು_ಉದ್ದಳತೆ
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#ವ್ಯಾಖ್ಯಾನಿಸು GET_ಗುಂಪು_ಸ್ಥಾನಾಂತರ(ಗುಂಪು) ((int32_t)(ಗುಂಪು)[ಗುಂಪು_ಸ್ಥಾನಾಂತರ_ಮೇಲಿನ]<<16|(ಗುಂಪು)[ಗುಂಪು_ಸ್ಥಾನಾಂತರ_ಕೆಳಗಿನ])

#ವ್ಯಾಖ್ಯಾನಿಸು ಮುಂದಿನ_ಗುಂಪು(ಗುಂಪು) ((ಗುಂಪು)+ಗುಂಪು_ಉದ್ದಳತೆ)
#ವ್ಯಾಖ್ಯಾನಿಸು PREV_ಗುಂಪು(ಗುಂಪು) ((ಗುಂಪು)-ಗುಂಪು_ಉದ್ದಳತೆ)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    uint32_t ಆರಂಭ, ಕೊನೆ;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    uint32_t token_String_ಸ್ಥಾನಾಂತರ, groups_ಸ್ಥಾನಾಂತರ, ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ, alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ;
} UChar_ಹೆಸರುಗಳು;

ನಾಮಾಕಾಶ {
// For a character name `name`, a query string `query` matches `name` under UAX44-LM2 if and only if
// the following returns true:
//     auto matcher = CharacterNameQuery(query).matcher();
//     for (const char c : name) {
//         if (!matcher.consistentWith(c)) {
//             return false;
//         }
//     }
//     return matcher.matches();
// `name` must be an exact character name, in particular, all uppercase, with no underscores, no
// double hyphens, no isolated hyphens.  There are no constraints on `query`; for instance, this
// class will match name="ZANABAZAR SQUARE LETTER -A" with query="Zanabazar-square_letter_-A".
// `Matcher::consistentWith` is not retryable: once it returns false, the `Matcher` should not be
// used again.
ವರ್ಗ CharacterNameQuery {
  ಸಾರ್ವಜನಿಕ:
    ವರ್ಗ Matcher {
      ಸಾರ್ವಜನಿಕ:
        // `query` must outlive the constructed object.
        Matcher(ಸ್ಥಿರಾಂಕ CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        ತಾರ್ಕಿಕ consistentWith(ಅಕ್ಷರ c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            ಆದರೆ (skeletonIterator == query.skeleton.ಕೊನೆ()) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಆದರೆ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                ಆದರೆ (ಕೊನೆಯ_Char == '-') {
                    ಆದರೆ (*skeletonIterator++ != ಕೊನೆಯ_Char) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                }
                ಕೊನೆಯ_Char = c;
                ಹಿಂತಿರುಗಿ ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '-') {
                ಆದರೆ (ಕೊನೆಯ_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.ಕೊನೆ() - 2)) {
                    // If lastChar == ' ', this is a word-initial hyphen, so we know it is
                    // significant.  Check that we are expecting it.
                    // If we are looking for U+1180 and we matched everything but the trailing -E,
                    // this could be that hyphen; move past it.  This could turn out to be a
                    // different character name if there is something other than E afterwards, in
                    // which case we should in principle have ignored the hyphen; but then since the
                    // suffixes will differ, we will return false anyway.
                    // For example, when searching for HANGUL JUNGSEONG O-E and checking consistency
                    // with HANGUL JUNGSEONG O-U, if we computed both skeleta and compared them,
                    // the skeleta would be HANGULJUNGSEONGO-E and HANGULJUNGSEONGOU, and the
                    // comparison would fail on - vs. U.
                    // Here, while feeding HANGUL JUNGSEONG O-U character-by-character, we assume
                    // its skeleton would be HANGULJUNGSEONGO-, and fail on E vs. U.
                    ಆದರೆ (*skeletonIterator++ != c) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                ಕೊನೆಯ_Char = c;
                ಹಿಂತಿರುಗಿ ನಿಜ;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (*skeletonIterator++ != c) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                ಕೊನೆಯ_Char = c;
                ಹಿಂತಿರುಗಿ ನಿಜ;
            }
        }

        ತಾರ್ಕಿಕ consistentWith(ಸ್ಥಿರಾಂಕ std::string_view substring) {
            ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c : substring) {
                ಆದರೆ (!consistentWith(c)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        ತಾರ್ಕಿಕ matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(ಕೊನೆಯ_Char != '-');
            ಹಿಂತಿರುಗಿ skeletonIterator == query.skeleton.ಕೊನೆ();
        }

        std::string_view ಉಳಿದ_Significant_Characters() ಸ್ಥಿರಾಂಕ {
            ಹಿಂತಿರುಗಿ query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      ಖಾಸಗಿ:
        ಸ್ಥಿರಾಂಕ CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        ಅಕ್ಷರ ಕೊನೆಯ_Char = ' ';
    };

    ಸ್ಪಷ್ಟ CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        ಅಕ್ಷರ *skeleton_ಎಲ್ಲೆ = skeleton_ದತ್ತಾಂಶ;
        ಗಾಗಿ (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_ಎಲ್ಲೆ < skeleton_ದತ್ತಾಂಶ + ಗಾತ್ರ(skeleton_ದತ್ತಾಂಶ));
            ಆದರೆ (skeleton_ಎಲ್ಲೆ >= skeleton_ದತ್ತಾಂಶ + ಗಾತ್ರ(skeleton_ದತ್ತಾಂಶ)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_ಎಲ್ಲೆ = skeleton_ದತ್ತಾಂಶ;
                ನಿಲ್ಲಿಸು;
            }
            ಆದರೆ (query[i] == ' ' || query[i] == '_') {
                ಮುಂದುವರಿ;
            }
            ಆದರೆ (query[i] == '-') {
                ತಾರ್ಕಿಕ isMedial;
                ತಾರ್ಕಿಕ is1180MedialHyphen = ಸುಳ್ಳು;
                ಆದರೆ (i == 0 || i == query.length() - 1) {
                    isMedial = ಸುಳ್ಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                ಆದರೆ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_ದತ್ತಾಂಶ, skeleton_ಎಲ್ಲೆ - skeleton_ದತ್ತಾಂಶ) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = ನಿಜ;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    ಗಾಗಿ (std::size_t j = i + 2; j < query.length(); ++j) {
                        ಆದರೆ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = ಸುಳ್ಳು;
                        }
                    }
                }
                ಆದರೆ (!isMedial || is1180MedialHyphen) {
                    *skeleton_ಎಲ್ಲೆ++ = query[i];
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                *skeleton_ಎಲ್ಲೆ++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_ದತ್ತಾಂಶ, skeleton_ಎಲ್ಲೆ - skeleton_ದತ್ತಾಂಶ);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ಸ್ಥಿರಾಂಕ {
        ಹಿಂತಿರುಗಿ Matcher(*ಇದು);
    }

ಖಾಸಗಿ:
    ಸ್ಥಿರ ತಾರ್ಕಿಕ isASCIILetterOrDigit(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ c) {
        ಹಿಂತಿರುಗಿ uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    ಅಕ್ಷರ skeleton_ದತ್ತಾಂಶ[120];
    std::string_view skeleton;
    ತಾರ್ಕಿಕ is1180;
};
} // namespace

/*
 * Get the groups table from a UCharNames struct.
 * The groups table consists of one uint16_t groupCount followed by
 * groupCount groups. Each group is a triple of uint16_t, see GROUP_LENGTH
 * and the comment for the old struct Group above.
 *
 * @param names (const UCharNames *) pointer to the UCharNames indexes
 * @return (const uint16_t *) pointer to the groups table
 */
#ವ್ಯಾಖ್ಯಾನಿಸು GET_GROUPS(ಹೆಸರುಗಳು) (ಸ್ಥಿರಾಂಕ uint16_t *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಹೆಸರುಗಳು+ಹೆಸರುಗಳು->groups_ಸ್ಥಾನಾಂತರ)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ CharacterNameQuery *otherName;
    UChar32 ಸಂಕೇತ_2;
} ಕಂಡುಹಿಡಿ_Name;

#ವ್ಯಾಖ್ಯಾನಿಸು DO_ಕಂಡುಹಿಡಿ_NAME ಶೂನ್ಯಸೂಚಕ

ಸ್ಥಿರ UData_ಸ್ಮರಣೆ *u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ=ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ UChar_ಹೆಸರುಗಳು *u_Char_ಹೆಸರುಗಳು=ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ icu::UInitOnce g_Char_ಹೆಸರುಗಳು_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ಸ್ಥಿರ int32_t g_Max_Name_ಉದ್ದಳತೆ=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ಸ್ಥಿರ uint32_t g_Name_ಗಣ[8]={ 0 };

#ವ್ಯಾಖ್ಯಾನಿಸು U_NONCHARACTER_ಸಂಕೇತ_ಬಿಂದು U_CHAR_CATEGORY_ಎಣಿಕೆ
#ವ್ಯಾಖ್ಯಾನಿಸು U_LEAD_SURROGATE U_CHAR_CATEGORY_ಎಣಿಕೆ + 1
#ವ್ಯಾಖ್ಯಾನಿಸು U_TRAIL_SURROGATE U_CHAR_CATEGORY_ಎಣಿಕೆ + 2

#ವ್ಯಾಖ್ಯಾನಿಸು U_CHAR_EXTENDED_CATEGORY_ಎಣಿಕೆ (U_CHAR_CATEGORY_ಎಣಿಕೆ + 3)

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ char_Cat_ಹೆಸರುಗಳು[U_CHAR_EXTENDED_CATEGORY_ಎಣಿಕೆ] = {
    "unassigned",
    "uppercase letter",
    "lowercase letter",
    "titlecase letter",
    "modifier letter",
    "other letter",
    "non spacing mark",
    "enclosing mark",
    "combining spacing mark",
    "decimal digit number",
    "letter number",
    "other number",
    "space separator",
    "line separator",
    "paragraph separator",
    "control",
    "format",
    "private use area",
    "surrogate",
    "dash punctuation",   
    "start punctuation",
    "end punctuation",
    "connector punctuation",
    "other punctuation",
    "math symbol",
    "currency symbol",
    "modifier symbol",
    "other symbol",
    "initial punctuation",
    "final punctuation",
    "noncharacter",
    "lead surrogate",
    "trail surrogate"
};

/* implementation ----------------------------------------------------------- */

ಸ್ಥಿರ UBool U_CALLCONV unames_cleanup()
{
    ಆದರೆ(u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ) {
        udata_close(u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ);
        u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(u_Char_ಹೆಸರುಗಳು) {
        u_Char_ಹೆಸರುಗಳು = ಶೂನ್ಯಸೂಚಕ;
    }
    g_Char_ಹೆಸರುಗಳು_Init_Once.reset();
    g_Max_Name_ಉದ್ದಳತೆ=0;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ * /*context*/,
             ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*type*/, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * /*name*/,
             ಸ್ಥಿರಾಂಕ UDataInfo *pInfo) {
    ಹಿಂತಿರುಗಿ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x6e &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x61 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x6d &&
        pInfo->format_ಆವೃತ್ತಿ[0]==1;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ತುಂಬು_Char_ಹೆಸರುಗಳು(UError_ಸಂಕೇತ &ಸ್ಥಿತಿ_2) {
    U_ASSERT(u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ);
    U_ASSERT(u_Char_ಹೆಸರುಗಳು == ಶೂನ್ಯಸೂಚಕ);

    u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ = udata_openChoice(ಶೂನ್ಯಸೂಚಕ, ದತ್ತಾಂಶ_TYPE, ದತ್ತಾಂಶ_NAME, isAcceptable, ಶೂನ್ಯಸೂಚಕ, &ಸ್ಥಿತಿ_2);
    ಆದರೆ(U_FAILURE(ಸ್ಥಿತಿ_2)) {
        u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        u_Char_ಹೆಸರುಗಳು = (UChar_ಹೆಸರುಗಳು *)udata_get_ಸ್ಮರಣೆ(u_Char_ಹೆಸರುಗಳು_ದತ್ತಾಂಶ);
    }
    ucln_common_ರಿಜಿಸ್ಟರ್_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ಸ್ಥಿರ UBool
is_ದತ್ತಾಂಶ_Loaded(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    umtx_initOnce(g_Char_ಹೆಸರುಗಳು_Init_Once, &ತುಂಬು_Char_ಹೆಸರುಗಳು, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ U_SUCCESS(*p_Error_ಸಂಕೇತ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c) UPRV_ಖಂಡ_MACRO_BEGIN { \
    ಆದರೆ((ಬಫರ್_ಉದ್ದಳತೆ)>0) { \
        *(ಬಫರ್)++=c; \
        --(ಬಫರ್_ಉದ್ದಳತೆ); \
    } \
    ++(ಬಫರ್_Pos); \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ

#ವ್ಯಾಖ್ಯಾನಿಸು U_ISO_COMMENT U_CHAR_NAME_CHOICE_ಎಣಿಕೆ

/*
 * Important: expandName() and compareName() are almost the same -
 * apply fixes to both.
 *
 * UnicodeData.txt uses ';' as a field separator, so no
 * field can contain ';' as part of its contents.
 * In unames.dat, it is marked as token[';']==-1 only if the
 * semicolon is used in the data file - which is iff we
 * have Unicode 1.0 names or ISO comments or aliases.
 * So, it will be token[';']==-1 if we store U1.0 names/ISO comments/aliases
 * although we know that it will never be part of a name.
 */
ಸ್ಥಿರ uint16_t
expandName(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು,
           ಸ್ಥಿರಾಂಕ uint8_t *name, uint16_t name_ಉದ್ದಳತೆ, UCharNameChoice nameChoice,
           ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    uint16_t* ಟೋಕನುಗಳು = reinterpret_cast<uint16_t*>(ಹೆಸರುಗಳು) + 8;
    uint16_t token, token_ಎಣಿಕೆ=*ಟೋಕನುಗಳು++, ಬಫರ್_Pos=0;
    uint8_t* token_ಅಕ್ಷರಸರಣಿಗಳು = reinterpret_cast<uint8_t*>(ಹೆಸರುಗಳು) + ಹೆಸರುಗಳು->token_String_ಸ್ಥಾನಾಂತರ;
    uint8_t c;

    ಆದರೆ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ಆದರೆ (static_cast<uint8_t>(';') >= token_ಎಣಿಕೆ || ಟೋಕನುಗಳು[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ಪೂರ್ಣಾಂಕ ಕ್ಷೇತ್ರ_ಸೂಚ್ಯಂಕ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ಮಾಡು {
                ಆಗ(name_ಉದ್ದಳತೆ>0) {
                    --name_ಉದ್ದಳತೆ;
                    ಆದರೆ(*name++==';') {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಆಗ(--ಕ್ಷೇತ್ರ_ಸೂಚ್ಯಂಕ>0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ಉದ್ದಳತೆ=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    ಆಗ(name_ಉದ್ದಳತೆ>0) {
        --name_ಉದ್ದಳತೆ;
        c=*name++;

        ಆದರೆ(c>=token_ಎಣಿಕೆ) {
            ಆದರೆ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*ಪೂರ್ಣಗೊಂಡ*/
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            token=ಟೋಕನುಗಳು[c];
            ಆದರೆ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ಟೋಕನುಗಳು[c<<8|*name++];
                --name_ಉದ್ದಳತೆ;
            }
            ಆದರೆ (token == static_cast<uint16_t>(-1)) {
                ಆದರೆ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*ಪೂರ್ಣಗೊಂಡ*/
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* write token word */
                uint8_t *tokenString=token_ಅಕ್ಷರಸರಣಿಗಳು+token;
                ಆಗ((c=*tokenString++)!=0) {
                    WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    ಆದರೆ(ಬಫರ್_ಉದ್ದಳತೆ>0) {
        *ಬಫರ್=0;
    }

    ಹಿಂತಿರುಗಿ ಬಫರ್_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ಸ್ಥಿರ UBool
compareName(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು,
            ಸ್ಥಿರಾಂಕ uint8_t *name, uint16_t name_ಉದ್ದಳತೆ, UCharNameChoice nameChoice,
            ಸ್ಥಿರಾಂಕ CharacterNameQuery& otherName) {
    uint16_t* ಟೋಕನುಗಳು = reinterpret_cast<uint16_t*>(ಹೆಸರುಗಳು) + 8;
    uint16_t token, token_ಎಣಿಕೆ=*ಟೋಕನುಗಳು++;
    uint8_t* token_ಅಕ್ಷರಸರಣಿಗಳು = reinterpret_cast<uint8_t*>(ಹೆಸರುಗಳು) + ಹೆಸರುಗಳು->token_String_ಸ್ಥಾನಾಂತರ;
    uint8_t c;
    ಸ್ವಯಂಚಾಲಿತ matcher = otherName.matcher();

    ಆದರೆ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ಆದರೆ (static_cast<uint8_t>(';') >= token_ಎಣಿಕೆ || ಟೋಕನುಗಳು[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ಪೂರ್ಣಾಂಕ ಕ್ಷೇತ್ರ_ಸೂಚ್ಯಂಕ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ಮಾಡು {
                ಆಗ(name_ಉದ್ದಳತೆ>0) {
                    --name_ಉದ್ದಳತೆ;
                    ಆದರೆ(*name++==';') {
                        ನಿಲ್ಲಿಸು;
                    }
                }
            } ಆಗ(--ಕ್ಷೇತ್ರ_ಸೂಚ್ಯಂಕ>0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ಉದ್ದಳತೆ=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    ಆಗ(name_ಉದ್ದಳತೆ>0) {
        --name_ಉದ್ದಳತೆ;
        c=*name++;

        ಆದರೆ(c>=token_ಎಣಿಕೆ) {
            ಆದರೆ(c!=';') {
                /* implicit letter */
                ಆದರೆ (!matcher.consistentWith(c)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /*ಪೂರ್ಣಗೊಂಡ*/
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            token=ಟೋಕನುಗಳು[c];
            ಆದರೆ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ಟೋಕನುಗಳು[c<<8|*name++];
                --name_ಉದ್ದಳತೆ;
            }
            ಆದರೆ (token == static_cast<uint16_t>(-1)) {
                ಆದರೆ(c!=';') {
                    /* explicit letter */
                    ಆದರೆ (!matcher.consistentWith(c)) {
                        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /*ಪೂರ್ಣಗೊಂಡ*/
                    ನಿಲ್ಲಿಸು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* write token word */
                ಆದರೆ (!matcher.consistentWith(reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *>(token_ಅಕ್ಷರಸರಣಿಗಳು + token))) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
        }
    }

    /* complete match? */
    ಹಿಂತಿರುಗಿ matcher.matches();
}

ಸ್ಥಿರ uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    ಆದರೆ (U_IS_UNICODE_NONCHAR(cp)) {
        ಹಿಂತಿರುಗಿ U_NONCHARACTER_ಸಂಕೇತ_ಬಿಂದು;
    }

    ಆದರೆ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    ಹಿಂತಿರುಗಿ cat;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    ಆದರೆ (cat >= UPRV_LENGTHOF(char_Cat_ಹೆಸರುಗಳು)) {
        ಹಿಂತಿರುಗಿ "unknown";
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ char_Cat_ಹೆಸರುಗಳು[cat];
    }
}

ಸ್ಥಿರ uint16_t getExtName(uint32_t ಸಂಕೇತ_2, ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *catname = getCharCatName(ಸಂಕೇತ_2);
    uint16_t length = 0;

    UChar32 cp;
    ಪೂರ್ಣಾಂಕ ndigits, i;
    
    WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, length, '<');
    ಆಗ (catname[length - 1]) {
        WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, length, catname[length - 1]);
    }
    WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, length, '-');
    ಗಾಗಿ (cp = ಸಂಕೇತ_2, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    ಆದರೆ (ndigits < 4)
        ndigits = 4;
    ಗಾಗಿ (cp = ಸಂಕೇತ_2, i = ndigits; (cp || i > 0) && ಬಫರ್_ಉದ್ದಳತೆ; cp >>= 4, ಬಫರ್_ಉದ್ದಳತೆ--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        ಬಫರ್[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    ಬಫರ್ += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, length, '>');

    ಹಿಂತಿರುಗಿ length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint16_t *
get_ಗುಂಪು(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು, uint32_t ಸಂಕೇತ_2) {
    ಸ್ಥಿರಾಂಕ uint16_t *groups=GET_GROUPS(ಹೆಸರುಗಳು);
    uint16_t ಗುಂಪು_MSB_2 = static_cast<uint16_t>(ಸಂಕೇತ_2 >> ಗುಂಪು_ಸರಿಸು),
             ಆರಂಭ=0,
             ಎಲ್ಲೆ=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    ಆಗ(ಆರಂಭ<ಎಲ್ಲೆ-1) {
        number = static_cast<uint16_t>((ಆರಂಭ + ಎಲ್ಲೆ) / 2);
        ಆದರೆ(ಗುಂಪು_MSB_2<groups[number*ಗುಂಪು_ಉದ್ದಳತೆ+ಗುಂಪು_MSB]) {
            ಎಲ್ಲೆ=number;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆರಂಭ=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    ಹಿಂತಿರುಗಿ groups+ಆರಂಭ*ಗುಂಪು_ಉದ್ದಳತೆ;
}

/*
 * expandGroupLengths() reads a block of compressed lengths of 32 strings and
 * expands them into offsets and lengths for each string.
 * Lengths are stored with a variable-width encoding in consecutive nibbles:
 * If a nibble<0xc, then it is the length itself (0=empty string).
 * If a nibble>=0xc, then it forms a length value with the following nibble.
 * Calculation see below.
 * The offsets and lengths arrays must be at least 33 (one more) long because
 * there is no check here at the end if the last nibble is still used.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint8_t *
expand_ಗುಂಪು_Lengths(ಸ್ಥಿರಾಂಕ uint8_t *s,
                   uint16_t offsets[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+1], uint16_t lengths[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, ಸ್ಥಾನಾಂತರ=0, length=0;
    uint8_t ಉದ್ದಳತೆ_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    ಆಗ(i<ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು) {
        ಉದ್ದಳತೆ_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        ಆದರೆ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | ಉದ್ದಳತೆ_Byte >> 4) + 12);
            ಉದ್ದಳತೆ_Byte&=0xf;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಉದ್ದಳತೆ_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((ಉದ್ದಳತೆ_Byte & 0x3f) + 12);
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(ಉದ್ದಳತೆ_Byte >> 4);
            ಉದ್ದಳತೆ_Byte&=0xf;
        }

        *offsets++=ಸ್ಥಾನಾಂತರ;
        *lengths++=length;

        ಸ್ಥಾನಾಂತರ+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        ಆದರೆ((ಉದ್ದಳತೆ_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=ಉದ್ದಳತೆ_Byte;
            ಆದರೆ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=ಸ್ಥಾನಾಂತರ;
                *lengths++=length;

                ಸ್ಥಾನಾಂತರ+=length;
                ++i;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    ಹಿಂತಿರುಗಿ s;
}

ಸ್ಥಿರ uint16_t
expand_ಗುಂಪು_Name(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು, ಸ್ಥಿರಾಂಕ uint16_t *ಗುಂಪು,
                uint16_t ಸಾಲು_ಸಂಖ್ಯೆ, UCharNameChoice nameChoice,
                ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    uint16_t offsets[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2], lengths[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2];
    ಸ್ಥಿರಾಂಕ uint8_t* s = reinterpret_cast<uint8_t*>(ಹೆಸರುಗಳು) + ಹೆಸರುಗಳು->ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ + GET_ಗುಂಪು_ಸ್ಥಾನಾಂತರ(ಗುಂಪು);
    s=expand_ಗುಂಪು_Lengths(s, offsets, lengths);
    ಹಿಂತಿರುಗಿ expandName(ಹೆಸರುಗಳು, s+offsets[ಸಾಲು_ಸಂಖ್ಯೆ], lengths[ಸಾಲು_ಸಂಖ್ಯೆ], nameChoice,
                      ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ);
}

ಸ್ಥಿರ uint16_t
getName(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು, uint32_t ಸಂಕೇತ_2, UCharNameChoice nameChoice,
        ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    ಸ್ಥಿರಾಂಕ uint16_t *ಗುಂಪು=get_ಗುಂಪು(ಹೆಸರುಗಳು, ಸಂಕೇತ_2);
    ಆದರೆ (static_cast<uint16_t>(ಸಂಕೇತ_2 >> ಗುಂಪು_ಸರಿಸು) == ಗುಂಪು[ಗುಂಪು_MSB]) {
        ಹಿಂತಿರುಗಿ expand_ಗುಂಪು_Name(ಹೆಸರುಗಳು, ಗುಂಪು, static_cast<uint16_t>(ಸಂಕೇತ_2 & ಗುಂಪು_ಮುಖವಾಡ), nameChoice,
                               ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* group not found */
        /* zero-terminate */
        ಆದರೆ(ಬಫರ್_ಉದ್ದಳತೆ>0) {
            *ಬಫರ್=0;
        }
        ಹಿಂತಿರುಗಿ 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ಸ್ಥಿರ UBool
enum_ಗುಂಪು_ಹೆಸರುಗಳು(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು, ಸ್ಥಿರಾಂಕ uint16_t *ಗುಂಪು,
               UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
               UEnum_Char_ಹೆಸರುಗಳು_Fn *fn, ಶೂನ್ಯ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2], lengths[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2];
    ಸ್ಥಿರಾಂಕ uint8_t* s = reinterpret_cast<uint8_t*>(ಹೆಸರುಗಳು) + ಹೆಸರುಗಳು->ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ + GET_ಗುಂಪು_ಸ್ಥಾನಾಂತರ(ಗುಂಪು);

    s=expand_ಗುಂಪು_Lengths(s, offsets, lengths);
    ಆದರೆ(fn!=DO_ಕಂಡುಹಿಡಿ_NAME) {
        ಅಕ್ಷರ ಬಫರ್[200];
        uint16_t length;

        ಆಗ(ಆರಂಭ<=ಕೊನೆ) {
            length=expandName(ಹೆಸರುಗಳು, s+offsets[ಆರಂಭ&ಗುಂಪು_ಮುಖವಾಡ], lengths[ಆರಂಭ&ಗುಂಪು_ಮುಖವಾಡ], nameChoice, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
            ಆದರೆ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                ಬಫರ್[length = getExtName(ಆರಂಭ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            ಆದರೆ(length>0) {
                ಆದರೆ(!fn(context, ಆರಂಭ, nameChoice, ಬಫರ್, length)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ++ಆರಂಭ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ CharacterNameQuery& otherName = *static_cast<ಕಂಡುಹಿಡಿ_Name*>(context)->otherName;
        ಆಗ(ಆರಂಭ<=ಕೊನೆ) {
            ಆದರೆ(compareName(ಹೆಸರುಗಳು, s+offsets[ಆರಂಭ&ಗುಂಪು_ಮುಖವಾಡ], lengths[ಆರಂಭ&ಗುಂಪು_ಮುಖವಾಡ], nameChoice, otherName)) {
                static_cast<ಕಂಡುಹಿಡಿ_Name*>(context)->ಸಂಕೇತ_2 = ಆರಂಭ;
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ++ಆರಂಭ;
        }
    }
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ಸ್ಥಿರ UBool
enum_Ext_ಹೆಸರುಗಳು(UChar32 ಆರಂಭ, UChar32 ಕೊನೆ,
             UEnum_Char_ಹೆಸರುಗಳು_Fn *fn, ಶೂನ್ಯ *context)
{
    ಆದರೆ(fn!=DO_ಕಂಡುಹಿಡಿ_NAME) {
        ಅಕ್ಷರ ಬಫರ್[200];
        uint16_t length;
        
        ಆಗ(ಆರಂಭ<=ಕೊನೆ) {
            ಬಫರ್[length = getExtName(ಆರಂಭ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್))] = 0;
            /* here, we assume that the buffer is large enough */
            ಆದರೆ(length>0) {
                ಆದರೆ(!fn(context, ಆರಂಭ, U_EXTENDED_CHAR_NAME, ಬಫರ್, length)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ++ಆರಂಭ;
        }
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ UBool
enum_ಹೆಸರುಗಳು(UChar_ಹೆಸರುಗಳು *ಹೆಸರುಗಳು,
          UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
          UEnum_Char_ಹೆಸರುಗಳು_Fn *fn, ಶೂನ್ಯ *context,
          UCharNameChoice nameChoice) {
    uint16_t ಆರಂಭ_ಗುಂಪು_MSB, ಕೊನೆ_ಗುಂಪು_MSB, ಗುಂಪು_ಎಣಿಕೆ;
    ಸ್ಥಿರಾಂಕ uint16_t *ಗುಂಪು, *ಗುಂಪು_ಎಲ್ಲೆ;

    ಆರಂಭ_ಗುಂಪು_MSB = static_cast<uint16_t>(ಆರಂಭ >> ಗುಂಪು_ಸರಿಸು);
    ಕೊನೆ_ಗುಂಪು_MSB = static_cast<uint16_t>((ಎಲ್ಲೆ - 1) >> ಗುಂಪು_ಸರಿಸು);

    /* find the group that contains start, or the highest before it */
    ಗುಂಪು=get_ಗುಂಪು(ಹೆಸರುಗಳು, ಆರಂಭ);

    ಆದರೆ(ಆರಂಭ_ಗುಂಪು_MSB<ಗುಂಪು[ಗುಂಪು_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_ಎಲ್ಲೆ = static_cast<UChar32>(ಗುಂಪು[ಗುಂಪು_MSB]) << ಗುಂಪು_ಸರಿಸು;
        ಆದರೆ(ext_ಎಲ್ಲೆ>ಎಲ್ಲೆ) {
            ext_ಎಲ್ಲೆ=ಎಲ್ಲೆ;
        }
        ಆದರೆ(!enum_Ext_ಹೆಸರುಗಳು(ಆರಂಭ, ext_ಎಲ್ಲೆ-1, fn, context)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }
        ಆರಂಭ=ext_ಎಲ್ಲೆ;
    }

    ಆದರೆ(ಆರಂಭ_ಗುಂಪು_MSB==ಕೊನೆ_ಗುಂಪು_MSB) {
        ಆದರೆ(ಆರಂಭ_ಗುಂಪು_MSB==ಗುಂಪು[ಗುಂಪು_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            ಹಿಂತಿರುಗಿ enum_ಗುಂಪು_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, ಗುಂಪು, ಆರಂಭ, ಎಲ್ಲೆ-1, fn, context, nameChoice);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಸ್ಥಿರಾಂಕ uint16_t *groups=GET_GROUPS(ಹೆಸರುಗಳು);
        ಗುಂಪು_ಎಣಿಕೆ=*groups++;
        ಗುಂಪು_ಎಲ್ಲೆ=groups+ಗುಂಪು_ಎಣಿಕೆ*ಗುಂಪು_ಉದ್ದಳತೆ;

        ಆದರೆ(ಆರಂಭ_ಗುಂಪು_MSB==ಗುಂಪು[ಗುಂಪು_MSB]) {
            /* enumerate characters in the partial start group */
            ಆದರೆ((ಆರಂಭ&ಗುಂಪು_ಮುಖವಾಡ)!=0) {
                ಆದರೆ(!enum_ಗುಂಪು_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, ಗುಂಪು,
                                   ಆರಂಭ, (static_cast<UChar32>(ಆರಂಭ_ಗುಂಪು_MSB) << ಗುಂಪು_ಸರಿಸು) + ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು - 1,
                                   fn, context, nameChoice)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
                ಗುಂಪು=ಮುಂದಿನ_ಗುಂಪು(ಗುಂಪು); /* continue with the next group */
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಆರಂಭ_ಗುಂಪು_MSB>ಗುಂಪು[ಗುಂಪು_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ಸ್ಥಿರಾಂಕ uint16_t *ಮುಂದಿನ_ಗುಂಪು_2=ಮುಂದಿನ_ಗುಂಪು(ಗುಂಪು);
            ಆದರೆ (ಮುಂದಿನ_ಗುಂಪು_2 < ಗುಂಪು_ಎಲ್ಲೆ && ಮುಂದಿನ_ಗುಂಪು_2[ಗುಂಪು_MSB] > ಆರಂಭ_ಗುಂಪು_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 ಕೊನೆ = ಮುಂದಿನ_ಗುಂಪು_2[ಗುಂಪು_MSB] << ಗುಂಪು_ಸರಿಸು;
                ಆದರೆ (ಕೊನೆ > ಎಲ್ಲೆ) {
                    ಕೊನೆ = ಎಲ್ಲೆ;
                }
                ಆದರೆ (!enum_Ext_ಹೆಸರುಗಳು(ಆರಂಭ, ಕೊನೆ - 1, fn, context)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ಗುಂಪು=ಮುಂದಿನ_ಗುಂಪು_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        ಆಗ(ಗುಂಪು<ಗುಂಪು_ಎಲ್ಲೆ && ಗುಂಪು[ಗುಂಪು_MSB]<ಕೊನೆ_ಗುಂಪು_MSB) {
            ಸ್ಥಿರಾಂಕ uint16_t *ಮುಂದಿನ_ಗುಂಪು_2;
            ಆರಂಭ = static_cast<UChar32>(ಗುಂಪು[ಗುಂಪು_MSB]) << ಗುಂಪು_ಸರಿಸು;
            ಆದರೆ(!enum_ಗುಂಪು_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, ಗುಂಪು, ಆರಂಭ, ಆರಂಭ+ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು-1, fn, context, nameChoice)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
            ಮುಂದಿನ_ಗುಂಪು_2=ಮುಂದಿನ_ಗುಂಪು(ಗುಂಪು);
            ಆದರೆ (ಮುಂದಿನ_ಗುಂಪು_2 < ಗುಂಪು_ಎಲ್ಲೆ && ಮುಂದಿನ_ಗುಂಪು_2[ಗುಂಪು_MSB] > ಗುಂಪು[ಗುಂಪು_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 ಕೊನೆ = ಮುಂದಿನ_ಗುಂಪು_2[ಗುಂಪು_MSB] << ಗುಂಪು_ಸರಿಸು;
                ಆದರೆ (ಕೊನೆ > ಎಲ್ಲೆ) {
                    ಕೊನೆ = ಎಲ್ಲೆ;
                }
                ಆದರೆ (!enum_Ext_ಹೆಸರುಗಳು((ಗುಂಪು[ಗುಂಪು_MSB] + 1) << ಗುಂಪು_ಸರಿಸು, ಕೊನೆ - 1, fn, context)) {
                    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
                }
            }
            ಗುಂಪು=ಮುಂದಿನ_ಗುಂಪು_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        ಆದರೆ(ಗುಂಪು<ಗುಂಪು_ಎಲ್ಲೆ && ಗುಂಪು[ಗುಂಪು_MSB]==ಕೊನೆ_ಗುಂಪು_MSB) {
            ಹಿಂತಿರುಗಿ enum_ಗುಂಪು_ಹೆಸರುಗಳು(ಹೆಸರುಗಳು, ಗುಂಪು, (ಎಲ್ಲೆ-1)&~ಗುಂಪು_ಮುಖವಾಡ, ಎಲ್ಲೆ-1, fn, context, nameChoice);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nameChoice == U_EXTENDED_CHAR_NAME && ಗುಂಪು == ಗುಂಪು_ಎಲ್ಲೆ) {
            UChar32 next = (PREV_ಗುಂಪು(ಗುಂಪು)[ಗುಂಪು_MSB] + 1) << ಗುಂಪು_ಸರಿಸು;
            ಆದರೆ (next > ಆರಂಭ) {
                ಆರಂಭ = next;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    ಆದರೆ (nameChoice == U_EXTENDED_CHAR_NAME) {
        ಆದರೆ (ಎಲ್ಲೆ > UCHAR_MAX_ಮೌಲ್ಯ + 1) {
            ಎಲ್ಲೆ = UCHAR_MAX_ಮೌಲ್ಯ + 1;
        }
        ಹಿಂತಿರುಗಿ enum_Ext_ಹೆಸರುಗಳು(ಆರಂಭ, ಎಲ್ಲೆ - 1, fn, context);
    }
    
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

ಸ್ಥಿರ uint16_t
write_ಗುಣಕ_Suffix(ಸ್ಥಿರಾಂಕ uint16_t *factors, uint16_t count,
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, /* suffix elements */
                  uint32_t ಸಂಕೇತ_2,
                  uint16_t indexes[8], /* output fields from here */
                  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *elementBases[8], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *elements[8],
                  ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    uint16_t i, ಗುಣಕ, ಬಫರ್_Pos=0;
    ಅಕ್ಷರ c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    ಗಾಗಿ(i=count; i>0; --i) {
        ಗುಣಕ=factors[i];
        indexes[i] = static_cast<uint16_t>(ಸಂಕೇತ_2 % ಗುಣಕ);
        ಸಂಕೇತ_2/=ಗುಣಕ;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(ಸಂಕೇತ_2);

    /* write each element */
    ಗಾಗಿ(;;) {
        ಆದರೆ(elementBases!=ಶೂನ್ಯಸೂಚಕ) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ಗುಣಕ=indexes[i];
        ಆಗ(ಗುಣಕ>0) {
            ಆಗ(*s++!=0) {}
            --ಗುಣಕ;
        }
        ಆದರೆ(elements!=ಶೂನ್ಯಸೂಚಕ) {
            *elements++=s;
        }

        /* write element */
        ಆಗ((c=*s++)!=0) {
            WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        ಆದರೆ(i>=count) {
            ನಿಲ್ಲಿಸು;
        }

        /* skip the rest of the strings for this factors[i] */
        ಗುಣಕ = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        ಆಗ(ಗುಣಕ>0) {
            ಆಗ(*s++!=0) {}
            --ಗುಣಕ;
        }

        ++i;
    }

    /* zero-terminate */
    ಆದರೆ(ಬಫರ್_ಉದ್ದಳತೆ>0) {
        *ಬಫರ್=0;
    }

    ಹಿಂತಿರುಗಿ ಬಫರ್_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ಸ್ಥಿರ uint16_t
getAlgName(AlgorithmicRange *range, uint32_t ಸಂಕೇತ_2, UCharNameChoice nameChoice,
        ಅಕ್ಷರ *ಬಫರ್, uint16_t ಬಫರ್_ಉದ್ದಳತೆ) {
    uint16_t ಬಫರ್_Pos=0;

    /* Only the normative character name can be algorithmic. */
    ಆದರೆ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        ಆದರೆ(ಬಫರ್_ಉದ್ದಳತೆ>0) {
            *ಬಫರ್=0;
        }
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆಯ್ಕೆ(range->type) {
    ಸ್ಥಿತಿ 0: {
        /* name = prefix hex-digits */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(range + 1);
        ಅಕ್ಷರ c;

        uint16_t i, count;

        /* copy prefix */
        ಆಗ((c=*s++)!=0) {
            WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        ಆದರೆ(count<ಬಫರ್_ಉದ್ದಳತೆ) {
            ಬಫರ್[count]=0;
        }

        ಗಾಗಿ(i=count; i>0;) {
            ಆದರೆ(--i<ಬಫರ್_ಉದ್ದಳತೆ) {
                c = static_cast<ಅಕ್ಷರ>(ಸಂಕೇತ_2 & 0xf);
                ಆದರೆ(c<10) {
                    c+='0';
                } ಇಲ್ಲದಿದ್ದರೆ {
                    c+='A'-10;
                }
                ಬಫರ್[i]=c;
            }
            ಸಂಕೇತ_2>>=4;
        }

        ಬಫರ್_Pos+=count;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ಸ್ಥಿರಾಂಕ uint16_t* factors = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(factors + count);
        ಅಕ್ಷರ c;

        /* copy prefix */
        ಆಗ((c=*s++)!=0) {
            WRITE_CHAR(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, ಬಫರ್_Pos, c);
        }

        ಬಫರ್_Pos+=write_ಗುಣಕ_Suffix(factors, count,
                                     s, ಸಂಕೇತ_2-range->ಆರಂಭ, indexes, ಶೂನ್ಯಸೂಚಕ, ಶೂನ್ಯಸೂಚಕ, ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ);
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* undefined type */
        /* zero-terminate */
        ಆದರೆ(ಬಫರ್_ಉದ್ದಳತೆ>0) {
            *ಬಫರ್=0;
        }
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ಬಫರ್_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ಸ್ಥಿರ UBool
enum_Alg_ಹೆಸರುಗಳು(AlgorithmicRange *range,
             UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
             UEnum_Char_ಹೆಸರುಗಳು_Fn *fn, ಶೂನ್ಯ *context,
             UCharNameChoice nameChoice) {
    ಅಕ್ಷರ ಬಫರ್[200];
    uint16_t length;

    ಆದರೆ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆಯ್ಕೆ(range->type) {
    ಸ್ಥಿತಿ 0: {
        ಅಕ್ಷರ *s, *ಕೊನೆ;
        ಅಕ್ಷರ c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(ಆರಂಭ), nameChoice, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
        ಆದರೆ(length<=0) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }

        /* call the enumerator function with this first character */
        ಆದರೆ(!fn(context, ಆರಂಭ, nameChoice, ಬಫರ್, length)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* go to the end of the name; all these names have the same length */
        ಕೊನೆ=ಬಫರ್;
        ಆಗ(*ಕೊನೆ!=0) {
            ++ಕೊನೆ;
        }

        /* enumerate the rest of the names */
        ಆಗ(++ಆರಂಭ<ಎಲ್ಲೆ) {
            /* increment the hexadecimal number on a character-basis */
            s=ಕೊನೆ;
            ಗಾಗಿ (;;) {
                c=*--s;
                ಆದರೆ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<ಅಕ್ಷರ>(c + 1);
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c=='9') {
                    *s='A';
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c=='F') {
                    *s='0';
                }
            }

            ಆದರೆ(!fn(context, ಆರಂಭ, nameChoice, ಬಫರ್, length)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ 1: {
        uint16_t indexes[8];
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *elementBases[8], *elements[8];
        ಸ್ಥಿರಾಂಕ uint16_t* factors = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(factors + count);
        ಅಕ್ಷರ *suffix, *t;
        uint16_t ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ, i, idx;

        ಅಕ್ಷರ c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=ಬಫರ್;
        ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ=0;
        ಆಗ((c=*s++)!=0) {
            *suffix++=c;
            ++ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ + write_ಗುಣಕ_Suffix(factors, count,
                                              s, static_cast<uint32_t>(ಆರಂಭ) - range->ಆರಂಭ,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(ಗಾತ್ರ(ಬಫರ್) - ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ)));

        /* call the enumerator function with this first character */
        ಆದರೆ(!fn(context, ಆರಂಭ, nameChoice, ಬಫರ್, length)) {
            ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
        }

        /* enumerate the rest of the names */
        ಆಗ(++ಆರಂಭ<ಎಲ್ಲೆ) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ಗಾಗಿ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ಆದರೆ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ಆಗ(*s++!=0) {
                    }
                    elements[i]=s;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=ಪೂರ್ವಪ್ರತ್ಯಯ_ಉದ್ದಳತೆ;
            ಗಾಗಿ(i=0; i<count; ++i) {
                s=elements[i];
                ಆಗ((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            ಆದರೆ(!fn(context, ಆರಂಭ, nameChoice, ಬಫರ್, length)) {
                ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* undefined type */
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ಸ್ಥಿರ UChar32
ಕಂಡುಹಿಡಿ_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 ಸಂಕೇತ_2;
    ಸ್ವಯಂಚಾಲಿತ matcher = query.matcher();

    ಆದರೆ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ಹಿಂತಿರುಗಿ 0xffff;
    }

    ಆಯ್ಕೆ(range->type) {
    ಸ್ಥಿತಿ 0: {
        /* name = prefix hex-digits */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(range + 1);

        /* compare prefix */
        ಆದರೆ (!matcher.consistentWith(s)) {
            ಹಿಂತಿರುಗಿ 0xffff;
        }

        /* read hexadecimal code point value */
        ಸಂಕೇತ_2=0;
        ಗಾಗಿ(ಅಕ್ಷರ c : matcher.ಉಳಿದ_Significant_Characters()) {
            ಆದರೆ('0'<=c && c<='9') {
                ಸಂಕೇತ_2=(ಸಂಕೇತ_2<<4)|(c-'0');
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ('A'<=c && c<='F') {
                ಸಂಕೇತ_2=(ಸಂಕೇತ_2<<4)|(c-'A'+10);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ 0xffff;
            }
        }

        /* does it fit into the range? */
        ಆದರೆ (range->ಆರಂಭ <= static_cast<uint32_t>(ಸಂಕೇತ_2) && static_cast<uint32_t>(ಸಂಕೇತ_2) <= range->ಕೊನೆ) {
            ಹಿಂತಿರುಗಿ ಸಂಕೇತ_2;
        }
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ 1: {
        ಅಕ್ಷರ ಬಫರ್[64];
        uint16_t indexes[8];
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *elementBases[8], *elements[8];
        ಸ್ಥಿರಾಂಕ uint16_t* factors = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(factors + count);
        UChar32 ಆರಂಭ, ಎಲ್ಲೆ;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view ಪೂರ್ವಪ್ರತ್ಯಯ = s;
        ಆದರೆ (!matcher.consistentWith(ಪೂರ್ವಪ್ರತ್ಯಯ)) {
            ಹಿಂತಿರುಗಿ 0xffff;
        }

        s += ಪೂರ್ವಪ್ರತ್ಯಯ.length() + 1;

        ಆರಂಭ = static_cast<UChar32>(range->ಆರಂಭ);
        ಎಲ್ಲೆ = static_cast<UChar32>(range->ಕೊನೆ + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.ಉಳಿದ_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ಗುಣಕ_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));

        /* compare the first suffix */
        ಆದರೆ(querySuffix == ಬಫರ್) {
            ಹಿಂತಿರುಗಿ ಆರಂಭ;
        }

        /* enumerate and compare the rest of the suffixes */
        ಆಗ(++ಆರಂಭ<ಎಲ್ಲೆ) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ಗಾಗಿ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ಆದರೆ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ಆಗ(*s++!=0) {}
                    elements[i]=s;
                    ನಿಲ್ಲಿಸು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            ಸ್ವಯಂಚಾಲಿತ it = querySuffix.begin();
            ಗಾಗಿ(i=0; i<count; ++i) {
                s=elements[i];
                ಅಕ್ಷರ c;
                ಆಗ((c=*s++)!=0) {
                    ಆದರೆ(it == querySuffix.ಕೊನೆ() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            ಆದರೆ(i<99 && it == querySuffix.ಕೊನೆ()) {
                ಹಿಂತಿರುಗಿ ಆರಂಭ;
            }
        }
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* undefined type */
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ಕೂಡಿಸು_3(ಗಣ, c) ((ಗಣ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#ವ್ಯಾಖ್ಯಾನಿಸು ಗಣ_ಒಳಗೊಂಡಿದೆ(ಗಣ, c) (((ಗಣ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ಸ್ಥಿರ int32_t
calc_String_ಗಣ_ಉದ್ದಳತೆ(uint32_t ಗಣ[8], ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) {
    int32_t length=0;
    ಅಕ್ಷರ c;

    ಆಗ((c=*s++)!=0) {
        ಗಣ_ಕೂಡಿಸು_3(ಗಣ, c);
        ++length;
    }
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ int32_t
calcAlgNameSetsLengths(int32_t max_Name_ಉದ್ದಳತೆ) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_ಎಣಿಕೆ;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_ಹೆಸರುಗಳು) + u_Char_ಹೆಸರುಗಳು->alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ);
    range_ಎಣಿಕೆ=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    ಆಗ(range_ಎಣಿಕೆ>0) {
        ಆಯ್ಕೆ(range->type) {
        ಸ್ಥಿತಿ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*ಪೂರ್ವಪ್ರತ್ಯಯ*/
            length = calc_String_ಗಣ_ಉದ್ದಳತೆ(g_Name_ಗಣ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(range + 1)) + range->variant;
            ಆದರೆ(length>max_Name_ಉದ್ದಳತೆ) {
                max_Name_ಉದ್ದಳತೆ=length;
            }
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 1: {
            /* name = prefix factorized-elements */
            ಸ್ಥಿರಾಂಕ uint16_t* factors = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint16_t*>(range + 1);
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s;
            int32_t i, count=range->variant, ಗುಣಕ, ಗುಣಕ_ಉದ್ದಳತೆ, max_ಗುಣಕ_ಉದ್ದಳತೆ;

            /* prefix length */
            s = reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(factors + count);
            length=calc_String_ಗಣ_ಉದ್ದಳತೆ(g_Name_ಗಣ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            ಗಾಗಿ(i=0; i<count; ++i) {
                max_ಗುಣಕ_ಉದ್ದಳತೆ=0;
                ಗಾಗಿ(ಗುಣಕ=factors[i]; ಗುಣಕ>0; --ಗುಣಕ) {
                    ಗುಣಕ_ಉದ್ದಳತೆ=calc_String_ಗಣ_ಉದ್ದಳತೆ(g_Name_ಗಣ, s);
                    s+=ಗುಣಕ_ಉದ್ದಳತೆ+1;
                    ಆದರೆ(ಗುಣಕ_ಉದ್ದಳತೆ>max_ಗುಣಕ_ಉದ್ದಳತೆ) {
                        max_ಗುಣಕ_ಉದ್ದಳತೆ=ಗುಣಕ_ಉದ್ದಳತೆ;
                    }
                }
                length+=max_ಗುಣಕ_ಉದ್ದಳತೆ;
            }

            ಆದರೆ(length>max_Name_ಉದ್ದಳತೆ) {
                max_Name_ಉದ್ದಳತೆ=length;
            }
            ನಿಲ್ಲಿಸು;
        }
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* unknown type */
            ನಿಲ್ಲಿಸು;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_ಎಣಿಕೆ;
    }
    ಹಿಂತಿರುಗಿ max_Name_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರ int32_t
calcExtNameSetsLengths(int32_t max_Name_ಉದ್ದಳತೆ) {
    int32_t i, length;

    ಗಾಗಿ(i=0; i<UPRV_LENGTHOF(char_Cat_ಹೆಸರುಗಳು); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_ಗಣ_ಉದ್ದಳತೆ(g_Name_ಗಣ, char_Cat_ಹೆಸರುಗಳು[i]);
        ಆದರೆ(length>max_Name_ಉದ್ದಳತೆ) {
            max_Name_ಉದ್ದಳತೆ=length;
        }
    }
    ಹಿಂತಿರುಗಿ max_Name_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರ int32_t
calc_Name_ಗಣ_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ uint16_t *ಟೋಕನುಗಳು, uint16_t token_ಎಣಿಕೆ, ಸ್ಥಿರಾಂಕ uint8_t *token_ಅಕ್ಷರಸರಣಿಗಳು, int8_t *tokenLengths,
                  uint32_t ಗಣ[8],
                  ಸ್ಥಿರಾಂಕ uint8_t **p_ಸಾಲು, ಸ್ಥಿರಾಂಕ uint8_t *ಸಾಲು_ಎಲ್ಲೆ) {
    ಸ್ಥಿರಾಂಕ uint8_t *ಸಾಲು=*p_ಸಾಲು;
    int32_t length=0, token_ಉದ್ದಳತೆ;
    uint16_t c, token;

    ಆಗ (ಸಾಲು != ಸಾಲು_ಎಲ್ಲೆ && (c = *ಸಾಲು++) != static_cast<uint8_t>(';')) {
        ಆದರೆ(c>=token_ಎಣಿಕೆ) {
            /* implicit letter */
            ಗಣ_ಕೂಡಿಸು_3(ಗಣ, c);
            ++length;
        } ಇಲ್ಲದಿದ್ದರೆ {
            token=ಟೋಕನುಗಳು[c];
            ಆದರೆ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*ಸಾಲು++;
                token=ಟೋಕನುಗಳು[c];
            }
            ಆದರೆ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                ಗಣ_ಕೂಡಿಸು_3(ಗಣ, c);
                ++length;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* count token word */
                ಆದರೆ(tokenLengths!=ಶೂನ್ಯಸೂಚಕ) {
                    /* use cached token length */
                    token_ಉದ್ದಳತೆ=tokenLengths[c];
                    ಆದರೆ(token_ಉದ್ದಳತೆ==0) {
                        token_ಉದ್ದಳತೆ = calc_String_ಗಣ_ಉದ್ದಳತೆ(ಗಣ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(token_ಅಕ್ಷರಸರಣಿಗಳು) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_ಉದ್ದಳತೆ);
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    token_ಉದ್ದಳತೆ = calc_String_ಗಣ_ಉದ್ದಳತೆ(ಗಣ, reinterpret_cast<ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*>(token_ಅಕ್ಷರಸರಣಿಗಳು) + token);
                }
                length+=token_ಉದ್ದಳತೆ;
            }
        }
    }

    *p_ಸಾಲು=ಸಾಲು;
    ಹಿಂತಿರುಗಿ length;
}

ಸ್ಥಿರ ಶೂನ್ಯ
calc_ಗುಂಪು_Name_Sets_Lengths(int32_t max_Name_ಉದ್ದಳತೆ) {
    uint16_t offsets[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2], lengths[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+2];

    uint16_t* ಟೋಕನುಗಳು = reinterpret_cast<uint16_t*>(u_Char_ಹೆಸರುಗಳು) + 8;
    uint16_t token_ಎಣಿಕೆ=*ಟೋಕನುಗಳು++;
    uint8_t* token_ಅಕ್ಷರಸರಣಿಗಳು = reinterpret_cast<uint8_t*>(u_Char_ಹೆಸರುಗಳು) + u_Char_ಹೆಸರುಗಳು->token_String_ಸ್ಥಾನಾಂತರ;

    int8_t *tokenLengths;

    ಸ್ಥಿರಾಂಕ uint16_t *ಗುಂಪು;
    ಸ್ಥಿರಾಂಕ uint8_t *s, *ಸಾಲು, *ಸಾಲು_ಎಲ್ಲೆ;

    int32_t ಗುಂಪು_ಎಣಿಕೆ, ಸಾಲು_ಸಂಖ್ಯೆ, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_ಎಣಿಕೆ));
    ಆದರೆ(tokenLengths!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_memset(tokenLengths, 0, token_ಎಣಿಕೆ);
    }

    ಗುಂಪು=GET_GROUPS(u_Char_ಹೆಸರುಗಳು);
    ಗುಂಪು_ಎಣಿಕೆ=*ಗುಂಪು++;

    /* enumerate all groups */
    ಆಗ(ಗುಂಪು_ಎಣಿಕೆ>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_ಹೆಸರುಗಳು) + u_Char_ಹೆಸರುಗಳು->ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ + GET_ಗುಂಪು_ಸ್ಥಾನಾಂತರ(ಗುಂಪು);
        s=expand_ಗುಂಪು_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        ಗಾಗಿ(ಸಾಲು_ಸಂಖ್ಯೆ=0; ಸಾಲು_ಸಂಖ್ಯೆ<ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು; ++ಸಾಲು_ಸಂಖ್ಯೆ) {
            ಸಾಲು=s+offsets[ಸಾಲು_ಸಂಖ್ಯೆ];
            length=lengths[ಸಾಲು_ಸಂಖ್ಯೆ];
            ಆದರೆ(length==0) {
                ಮುಂದುವರಿ;
            }

            ಸಾಲು_ಎಲ್ಲೆ=ಸಾಲು+length;

            /* read regular name */
            length=calc_Name_ಗಣ_ಉದ್ದಳತೆ(ಟೋಕನುಗಳು, token_ಎಣಿಕೆ, token_ಅಕ್ಷರಸರಣಿಗಳು, tokenLengths, g_Name_ಗಣ, &ಸಾಲು, ಸಾಲು_ಎಲ್ಲೆ);
            ಆದರೆ(length>max_Name_ಉದ್ದಳತೆ) {
                max_Name_ಉದ್ದಳತೆ=length;
            }
            ಆದರೆ(ಸಾಲು==ಸಾಲು_ಎಲ್ಲೆ) {
                ಮುಂದುವರಿ;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_ಗಣ_ಉದ್ದಳತೆ(ಟೋಕನುಗಳು, token_ಎಣಿಕೆ, token_ಅಕ್ಷರಸರಣಿಗಳು, tokenLengths, g_Name_ಗಣ, &ಸಾಲು, ಸಾಲು_ಎಲ್ಲೆ);
            ಆದರೆ(length>max_Name_ಉದ್ದಳತೆ) {
                max_Name_ಉದ್ದಳತೆ=length;
            }
            ಆದರೆ(ಸಾಲು==ಸಾಲು_ಎಲ್ಲೆ) {
                ಮುಂದುವರಿ;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        ಗುಂಪು=ಮುಂದಿನ_ಗುಂಪು(ಗುಂಪು);
        --ಗುಂಪು_ಎಣಿಕೆ;
    }

    ಆದರೆ(tokenLengths!=ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_ಉದ್ದಳತೆ=max_Name_ಉದ್ದಳತೆ;
}

ಸ್ಥಿರ UBool
calcNameSetsLengths(UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_ಉದ್ದಳತೆ;

    ಆದರೆ(g_Max_Name_ಉದ್ದಳತೆ!=0) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }

    ಆದರೆ(!is_ದತ್ತಾಂಶ_Loaded(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    ಗಾಗಿ (i = 0; i < static_cast<int32_t>(ಗಾತ್ರ(extChars)) - 1; ++i) {
        ಗಣ_ಕೂಡಿಸು_3(g_Name_ಗಣ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_ಉದ್ದಳತೆ=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_ಉದ್ದಳತೆ=calcExtNameSetsLengths(max_Name_ಉದ್ದಳತೆ);

    /* set sets and lengths from group names, set global maximum values */
    calc_ಗುಂಪು_Name_Sets_Lengths(max_Name_ಉದ್ದಳತೆ);

    ಹಿಂತಿರುಗಿ ನಿಜ;
}

U_NAMESPACE_ಕೊನೆ

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 ಸಂಕೇತ_2, UCharNameChoice nameChoice,
           ಅಕ್ಷರ *ಬಫರ್, int32_t ಬಫರ್_ಉದ್ದಳತೆ,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ ||
              ಬಫರ್_ಉದ್ದಳತೆ<0 || (ಬಫರ್_ಉದ್ದಳತೆ>0 && ಬಫರ್==ಶೂನ್ಯಸೂಚಕ)
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ((uint32_t)ಸಂಕೇತ_2>UCHAR_MAX_ಮೌಲ್ಯ || !is_ದತ್ತಾಂಶ_Loaded(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ u_terminateChars(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, 0, p_Error_ಸಂಕೇತ);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_ಹೆಸರುಗಳು+u_Char_ಹೆಸರುಗಳು->alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಆಗ(i>0) {
        ಆದರೆ(algRange->ಆರಂಭ<=(uint32_t)ಸಂಕೇತ_2 && (uint32_t)ಸಂಕೇತ_2<=algRange->ಕೊನೆ) {
            length=getAlgName(algRange, (uint32_t)ಸಂಕೇತ_2, nameChoice, ಬಫರ್, (uint16_t)ಬಫರ್_ಉದ್ದಳತೆ);
            ನಿಲ್ಲಿಸು;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    ಆದರೆ(i==0) {
        ಆದರೆ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_ಹೆಸರುಗಳು, (uint32_t )ಸಂಕೇತ_2, U_EXTENDED_CHAR_NAME, ಬಫರ್, (uint16_t) ಬಫರ್_ಉದ್ದಳತೆ);
            ಆದರೆ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) ಸಂಕೇತ_2, ಬಫರ್, (uint16_t) ಬಫರ್_ಉದ್ದಳತೆ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* normal character name */
            length=getName(u_Char_ಹೆಸರುಗಳು, (uint32_t)ಸಂಕೇತ_2, nameChoice, ಬಫರ್, (uint16_t)ಬಫರ್_ಉದ್ದಳತೆ);
        }
    }

    ಹಿಂತಿರುಗಿ u_terminateChars(ಬಫರ್, ಬಫರ್_ಉದ್ದಳತೆ, length, p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                ಅಕ್ಷರ *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* check the argument values */
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest_ಸಾಮರ್ಥ್ಯ>0 && dest==ಶೂನ್ಯಸೂಚಕ)) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಹಿಂತಿರುಗಿ u_terminateChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ name,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಅಕ್ಷರ ಅಧೋ[120] = {0};
    ಕಂಡುಹಿಡಿ_Name ಕಂಡುಹಿಡಿ_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    ಅಕ್ಷರ c0;
    ಸ್ಥಿರ constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ error;
    }

    ಆದರೆ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ || name==ಶೂನ್ಯಸೂಚಕ || *name==0) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ error;
    }

    ಆದರೆ(!is_ದತ್ತಾಂಶ_Loaded(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ error;
    }

    /* construct the lowercase of the name first */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* nameChar = name;
    ಗಾಗಿ(i=0; i<ಗಾತ್ರ(ಅಧೋ); ++i) {
        ಆದರೆ((c0=*nameChar++)!=0) {
            ಅಧೋ[i]=uprv_tolower(c0);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಆದರೆ(i==ಗಾತ್ರ(ಅಧೋ)) {
        /* name too long, there is no such character */
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    ಆದರೆ (ಅಧೋ[0] == '<') {
        ಆದರೆ (nameChoice == U_EXTENDED_CHAR_NAME && ಅಧೋ[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t ಎಲ್ಲೆ = i;
            ಆಗ (i >= 3 && ಅಧೋ[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_ಉದ್ದಳತೆ = ಎಲ್ಲೆ - (i + 1);
            ಆದರೆ (i >= 2 && ಅಧೋ[i] == '-' && 1 <= hex_ಉದ್ದಳತೆ && hex_ಉದ್ದಳತೆ <= 8) {
                uint32_t cIdx;

                ಅಧೋ[i] = 0;

                ಗಾಗಿ (++i; i < ಎಲ್ಲೆ; ++i) {
                    ಆದರೆ (ಅಧೋ[i] >= '0' && ಅಧೋ[i] <= '9') {
                        cp = (cp << 4) + ಅಧೋ[i] - '0';
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಧೋ[i] >= 'a' && ಅಧೋ[i] <= 'f') {
                        cp = (cp << 4) + ಅಧೋ[i] - 'a' + 10;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    ಆದರೆ (cp > UCHAR_MAX_ಮೌಲ್ಯ) {
                        *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                        ಹಿಂತಿರುಗಿ error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                ಗಾಗಿ (ಅಧೋ[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_ಹೆಸರುಗಳು); ++cIdx) {

                    ಆದರೆ (!uprv_strcmp(ಅಧೋ + 1, char_Cat_ಹೆಸರುಗಳು[cIdx])) {
                        ಆದರೆ (cat == cIdx) {
                            ಹಿಂತಿರುಗಿ cp;
                        }
                        ನಿಲ್ಲಿಸು;
                    }
                }
            }
        }

        *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
        ಹಿಂತಿರುಗಿ error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_ಹೆಸರುಗಳು+u_Char_ಹೆಸರುಗಳು->alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಆಗ(i>0) {
        ಆದರೆ((cp=ಕಂಡುಹಿಡಿ_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            ಹಿಂತಿರುಗಿ cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    ಕಂಡುಹಿಡಿ_Name_2.otherName=&query;
    ಕಂಡುಹಿಡಿ_Name_2.ಸಂಕೇತ_2=error;
    enum_ಹೆಸರುಗಳು(u_Char_ಹೆಸರುಗಳು, 0, UCHAR_MAX_ಮೌಲ್ಯ + 1, DO_ಕಂಡುಹಿಡಿ_NAME, &ಕಂಡುಹಿಡಿ_Name_2, nameChoice);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_Name_2.ಸಂಕೇತ_2 == error) {
         *p_Error_ಸಂಕೇತ = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
    }
    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_Name_2.ಸಂಕೇತ_2;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_enum_Char_ಹೆಸರುಗಳು(UChar32 ಆರಂಭ, UChar32 ಎಲ್ಲೆ,
                UEnum_Char_ಹೆಸರುಗಳು_Fn *fn,
                ಶೂನ್ಯ *context,
                UCharNameChoice nameChoice,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ || fn==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ((uint32_t) ಎಲ್ಲೆ > UCHAR_MAX_ಮೌಲ್ಯ + 1) {
        ಎಲ್ಲೆ = UCHAR_MAX_ಮೌಲ್ಯ + 1;
    }
    ಆದರೆ((uint32_t)ಆರಂಭ>=(uint32_t)ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(!is_ದತ್ತಾಂಶ_Loaded(p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_ಹೆಸರುಗಳು+u_Char_ಹೆಸರುಗಳು->alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಆಗ(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        ಆದರೆ((uint32_t)ಆರಂಭ<algRange->ಆರಂಭ) {
            ಆದರೆ((uint32_t)ಎಲ್ಲೆ<=algRange->ಆರಂಭ) {
                enum_ಹೆಸರುಗಳು(u_Char_ಹೆಸರುಗಳು, ಆರಂಭ, ಎಲ್ಲೆ, fn, context, nameChoice);
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ(!enum_ಹೆಸರುಗಳು(u_Char_ಹೆಸರುಗಳು, ಆರಂಭ, (UChar32)algRange->ಆರಂಭ, fn, context, nameChoice)) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆರಂಭ=(UChar32)algRange->ಆರಂಭ;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        ಆದರೆ((uint32_t)ಆರಂಭ<=algRange->ಕೊನೆ) {
            ಆದರೆ((uint32_t)ಎಲ್ಲೆ<=(algRange->ಕೊನೆ+1)) {
                enum_Alg_ಹೆಸರುಗಳು(algRange, ಆರಂಭ, ಎಲ್ಲೆ, fn, context, nameChoice);
                ಹಿಂತಿರುಗಿ;
            }
            ಆದರೆ(!enum_Alg_ಹೆಸರುಗಳು(algRange, ಆರಂಭ, (UChar32)algRange->ಕೊನೆ+1, fn, context, nameChoice)) {
                ಹಿಂತಿರುಗಿ;
            }
            ಆರಂಭ=(UChar32)algRange->ಕೊನೆ+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_ಹೆಸರುಗಳು(u_Char_ಹೆಸರುಗಳು, ಆರಂಭ, ಎಲ್ಲೆ, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_ಉದ್ದಳತೆ() {
    UError_ಸಂಕೇತ error_ಸಂಕೇತ=U_ZERO_ERROR;
    ಆದರೆ(calcNameSetsLengths(&error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ g_Max_Name_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
char_ಗಣ_ಗಮ್ಯ_USet(uint32_t cset[8], ಸ್ಥಿರಾಂಕ USetAdder *sa) {
    char16_t us[256];
    ಅಕ್ಷರ cs[256];

    int32_t i, length;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ;

    error_ಸಂಕೇತ=U_ZERO_ERROR;

    ಆದರೆ(!calcNameSetsLengths(&error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    ಗಾಗಿ(i=0; i<256; ++i) {
        ಆದರೆ(ಗಣ_ಒಳಗೊಂಡಿದೆ(cset, i)) {
            cs[length++] = static_cast<ಅಕ್ಷರ>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_ಗಮ್ಯ_UChars(cs, us, length);

    /* add each char16_t to the USet */
    ಗಾಗಿ(i=0; i<length; ++i) {
        ಆದರೆ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->ಕೂಡಿಸು(sa->ಗಣ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_getCharNameCharacters(ಸ್ಥಿರಾಂಕ USetAdder *sa) {
    char_ಗಣ_ಗಮ್ಯ_USet(g_Name_ಗಣ, sa);
}

/* data swapping ------------------------------------------------------------ */

/*
 * The token table contains non-negative entries for token bytes,
 * and -1 for bytes that represent themselves in the data file's charset.
 * -2 entries are used for lead bytes.
 *
 * Direct bytes (-1 entries) must be translated from the input charset family
 * to the output charset family.
 * makeTokenMap() writes a permutation mapping for this.
 * Use it once for single-/lead-byte tokens and once more for all trail byte
 * tokens. (';' is an unused trail byte marked with -1.)
 */
ಸ್ಥಿರ ಶೂನ್ಯ
makeTokenMap(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
             int16_t ಟೋಕನುಗಳು[], uint16_t token_ಎಣಿಕೆ,
             uint8_t map[256],
             UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UBool ಬಳಸಿದ_ಹೊರಹೋಗು_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(ds->ಒಳ_Charset==ds->ಹೊರಹೋಗು_Charset) {
        /* Same charset family: identity permutation */
        ಗಾಗಿ(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        uprv_memset(map, 0, 256);
        uprv_memset(ಬಳಸಿದ_ಹೊರಹೋಗು_Char, 0, 256);

        ಆದರೆ(token_ಎಣಿಕೆ>256) {
            token_ಎಣಿಕೆ=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        ಗಾಗಿ(i=1; i<token_ಎಣಿಕೆ; ++i) {
            ಆದರೆ(ಟೋಕನುಗಳು[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->ಅದಲುಬದಲು_Inv_Chars(ds, &c1, 1, &c2, p_Error_ಸಂಕೇತ);
                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->ಒಳ_Charset);
                    ಹಿಂತಿರುಗಿ;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                ಬಳಸಿದ_ಹೊರಹೋಗು_Char[c2]=ನಿಜ;
            }
        }

        /* set the mappings for the rest of the permutation */
        ಗಾಗಿ(i=j=1; i<token_ಎಣಿಕೆ; ++i) {
            /* set mappings that were not set for direct bytes */
            ಆದರೆ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                ಆಗ(ಬಳಸಿದ_ಹೊರಹೋಗು_Char[j]) {
                    ++j;
                }
                map[i] = static_cast<uint8_t>(j++);
            }
        }

        /*
         * leave mappings at tokenCount and above unset if tokenCount<256
         * because they won't be used
         */
    }
}

U_CAPI int32_t U_EXPORT2
uchar_ಅದಲುಬದಲು_ಹೆಸರುಗಳು(ಸ್ಥಿರಾಂಕ UDataSwapper *ds,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಒಳ_ದತ್ತಾಂಶ, int32_t length, ಶೂನ್ಯ *ಹೊರಹೋಗು_ದತ್ತಾಂಶ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ UDataInfo *pInfo;
    int32_t ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;

    ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಬೈಟುಗಳು;
    uint8_t *ಹೊರಹೋಗು_ಬೈಟುಗಳು;

    uint32_t token_String_ಸ್ಥಾನಾಂತರ, groups_ಸ್ಥಾನಾಂತರ, ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ, alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ,
             ಸ್ಥಾನಾಂತರ, i, count, ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ;

    ಸ್ಥಿರಾಂಕ AlgorithmicRange *ಒಳ_Range;
    AlgorithmicRange *ಹೊರಹೋಗು_Range;

    /* udata_swapDataHeader checks the arguments */
    ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ=udata_ಅದಲುಬದಲು_ದತ್ತಾಂಶ_ಶೀರ್ಷಿಕೆ(ds, ಒಳ_ದತ್ತಾಂಶ, length, ಹೊರಹೋಗು_ದತ್ತಾಂಶ, p_Error_ಸಂಕೇತ);
    ಆದರೆ(p_Error_ಸಂಕೇತ==ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check data format and format version */
    pInfo=(ಸ್ಥಿರಾಂಕ UDataInfo *)((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಒಳ_ದತ್ತಾಂಶ+4);
    ಆದರೆ(!(
        pInfo->ದತ್ತಾಂಶ_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ದತ್ತಾಂಶ_Format[1]==0x6e &&
        pInfo->ದತ್ತಾಂಶ_Format[2]==0x61 &&
        pInfo->ದತ್ತಾಂಶ_Format[3]==0x6d &&
        pInfo->format_ಆವೃತ್ತಿ[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->ದತ್ತಾಂಶ_Format[0], pInfo->ದತ್ತಾಂಶ_Format[1],
                         pInfo->ದತ್ತಾಂಶ_Format[2], pInfo->ದತ್ತಾಂಶ_Format[3],
                         pInfo->format_ಆವೃತ್ತಿ[0]);
        *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಒಳ_ಬೈಟುಗಳು=(ಸ್ಥಿರಾಂಕ uint8_t *)ಒಳ_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಹೊರಹೋಗು_ಬೈಟುಗಳು=(ಹೊರಹೋಗು_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) ? ಶೂನ್ಯಸೂಚಕ : (uint8_t *)ಹೊರಹೋಗು_ದತ್ತಾಂಶ+ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
    ಆದರೆ(length<0) {
        alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ=ds->readUInt32(((ಸ್ಥಿರಾಂಕ uint32_t *)ಒಳ_ಬೈಟುಗಳು)[3]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        length-=ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ;
        ಆದರೆ( length<20 ||
            (uint32_t)length<(alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ=ds->readUInt32(((ಸ್ಥಿರಾಂಕ uint32_t *)ಒಳ_ಬೈಟುಗಳು)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }
    }

    ಆದರೆ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        ಸ್ಥಾನಾಂತರ=alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ;
        count=ds->readUInt32(*((ಸ್ಥಿರಾಂಕ uint32_t *)(ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ)));
        ಸ್ಥಾನಾಂತರ+=4;

        ಗಾಗಿ(i=0; i<count; ++i) {
            ಒಳ_Range=(ಸ್ಥಿರಾಂಕ AlgorithmicRange *)(ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ);
            ಸ್ಥಾನಾಂತರ+=ds->readUInt16(ಒಳ_Range->size);
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* swap data */
        ಸ್ಥಿರಾಂಕ uint16_t *p;
        uint16_t *q, *ತಾತ್ಕಾಲಿಕ;

        int16_t ಟೋಕನುಗಳು[512];
        uint16_t token_ಎಣಿಕೆ;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        ಆದರೆ(ಒಳ_ಬೈಟುಗಳು!=ಹೊರಹೋಗು_ಬೈಟುಗಳು) {
            uprv_memcpy(ಹೊರಹೋಗು_ಬೈಟುಗಳು, ಒಳ_ಬೈಟುಗಳು, length);
        }

        /* the initial 4 offsets first */
        token_String_ಸ್ಥಾನಾಂತರ=ds->readUInt32(((ಸ್ಥಿರಾಂಕ uint32_t *)ಒಳ_ಬೈಟುಗಳು)[0]);
        groups_ಸ್ಥಾನಾಂತರ=ds->readUInt32(((ಸ್ಥಿರಾಂಕ uint32_t *)ಒಳ_ಬೈಟುಗಳು)[1]);
        ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ=ds->readUInt32(((ಸ್ಥಿರಾಂಕ uint32_t *)ಒಳ_ಬೈಟುಗಳು)[2]);
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು, 16, ಹೊರಹೋಗು_ಬೈಟುಗಳು, p_Error_ಸಂಕೇತ);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ಸ್ಥಿರಾಂಕ uint16_t *)(ಒಳ_ಬೈಟುಗಳು+16);
        q=(uint16_t *)(ಹೊರಹೋಗು_ಬೈಟುಗಳು+16);

        /* read and swap the tokenCount */
        token_ಎಣಿಕೆ=ds->readUInt16(*p);
        ds->ಅದಲುಬದಲು_Array16(ds, p, 2, q, p_Error_ಸಂಕೇತ);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        ಆದರೆ(token_ಎಣಿಕೆ<=512) {
            count=token_ಎಣಿಕೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            count=512;
        }
        ಗಾಗಿ(i=0; i<count; ++i) {
            ಟೋಕನುಗಳು[i]=udata_readInt16(ds, p[i]);
        }
        ಗಾಗಿ(; i<512; ++i) {
            ಟೋಕನುಗಳು[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, ಟೋಕನುಗಳು, token_ಎಣಿಕೆ, map, p_Error_ಸಂಕೇತ);
        makeTokenMap(ds, ಟೋಕನುಗಳು+256, (uint16_t)(token_ಎಣಿಕೆ>256 ? token_ಎಣಿಕೆ-256 : 0), trailMap, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        ತಾತ್ಕಾಲಿಕ=(uint16_t *)uprv_malloc(token_ಎಣಿಕೆ*2);
        ಆದರೆ(ತಾತ್ಕಾಲಿಕ==ಶೂನ್ಯಸೂಚಕ) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_ಎಣಿಕೆ);
            *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
            ಹಿಂತಿರುಗಿ 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        ಗಾಗಿ(i=0; i<token_ಎಣಿಕೆ && i<256; ++i) {
            ds->ಅದಲುಬದಲು_Array16(ds, p+i, 2, ತಾತ್ಕಾಲಿಕ+map[i], p_Error_ಸಂಕೇತ);
        }

        /* swap and permutate trail-byte tokens */
        ಗಾಗಿ(; i<token_ಎಣಿಕೆ; ++i) {
            ds->ಅದಲುಬದಲು_Array16(ds, p+i, 2, ತಾತ್ಕಾಲಿಕ+(i&0xffffff00)+trailMap[i&0xff], p_Error_ಸಂಕೇತ);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, ತಾತ್ಕಾಲಿಕ, token_ಎಣಿಕೆ*2);
        uprv_free(ತಾತ್ಕಾಲಿಕ);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_ಅದಲುಬದಲು_Inv_String_ಖಂಡ(ds, ಒಳ_ಬೈಟುಗಳು+token_String_ಸ್ಥಾನಾಂತರ, (int32_t)(groups_ಸ್ಥಾನಾಂತರ-token_String_ಸ್ಥಾನಾಂತರ),
                                    ಹೊರಹೋಗು_ಬೈಟುಗಳು+token_String_ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            ಹಿಂತಿರುಗಿ 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ಸ್ಥಿರಾಂಕ uint16_t *)(ಒಳ_ಬೈಟುಗಳು+groups_ಸ್ಥಾನಾಂತರ)));
        ds->ಅದಲುಬದಲು_Array16(ds, ಒಳ_ಬೈಟುಗಳು+groups_ಸ್ಥಾನಾಂತರ, (int32_t)((1+count*3)*2),
                           ಹೊರಹೋಗು_ಬೈಟುಗಳು+groups_ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        ಆದರೆ(ds->ಒಳ_Charset!=ds->ಹೊರಹೋಗು_Charset) {
            uint16_t offsets[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+1], lengths[ಸಾಲುಗಳು_ಪ್ರತಿ_ಗುಂಪು+1];

            ಸ್ಥಿರಾಂಕ uint8_t *ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು, *ಮುಂದಿನ_ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು;
            uint8_t *ಹೊರಹೋಗು_ಅಕ್ಷರಸರಣಿಗಳು;

            uint8_t c;

            ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು=ಒಳ_ಬೈಟುಗಳು+ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ;
            ಹೊರಹೋಗು_ಅಕ್ಷರಸರಣಿಗಳು=ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ;

            ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ=alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ-ಗುಂಪು_String_ಸ್ಥಾನಾಂತರ;

            /* iterate through string groups until only a few padding bytes are left */
            ಆಗ(ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ>32) {
                ಮುಂದಿನ_ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು=expand_ಗುಂಪು_Lengths(ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು, offsets, lengths);

                /* move past the length bytes */
                ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ-=(uint32_t)(ಮುಂದಿನ_ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು-ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು);
                ಹೊರಹೋಗು_ಅಕ್ಷರಸರಣಿಗಳು+=ಮುಂದಿನ_ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು-ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು;
                ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು=ಮುಂದಿನ_ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ-=count;

                /* swap the string bytes using map[] and trailMap[] */
                ಆಗ(count>0) {
                    c=*ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು++;
                    *ಹೊರಹೋಗು_ಅಕ್ಷರಸರಣಿಗಳು++=map[c];
                    ಆದರೆ(ಟೋಕನುಗಳು[c]!=-2) {
                        --count;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* token lead byte: swap the trail byte, too */
                        *ಹೊರಹೋಗು_ಅಕ್ಷರಸರಣಿಗಳು++=trailMap[*ಒಳ_ಅಕ್ಷರಸರಣಿಗಳು++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        ಸ್ಥಾನಾಂತರ=alg_ಹೆಸರುಗಳು_ಸ್ಥಾನಾಂತರ;
        count=ds->readUInt32(*((ಸ್ಥಿರಾಂಕ uint32_t *)(ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ)));
        ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, 4, ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ, p_Error_ಸಂಕೇತ);
        ಸ್ಥಾನಾಂತರ+=4;

        ಗಾಗಿ(i=0; i<count; ++i) {
            ಆದರೆ(ಸ್ಥಾನಾಂತರ>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_ಸಂಕೇತ=U_ಸೂಚ್ಯಂಕ_OUTOFBOUNDS_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }

            ಒಳ_Range=(ಸ್ಥಿರಾಂಕ AlgorithmicRange *)(ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ);
            ಹೊರಹೋಗು_Range=(AlgorithmicRange *)(ಹೊರಹೋಗು_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ);
            ಸ್ಥಾನಾಂತರ+=ds->readUInt16(ಒಳ_Range->size);

            ds->ಅದಲುಬದಲು_Array32(ds, ಒಳ_Range, 8, ಹೊರಹೋಗು_Range, p_Error_ಸಂಕೇತ);
            ds->ಅದಲುಬದಲು_Array16(ds, &ಒಳ_Range->size, 2, &ಹೊರಹೋಗು_Range->size, p_Error_ಸಂಕೇತ);
            ಆಯ್ಕೆ(ಒಳ_Range->type) {
            ಸ್ಥಿತಿ 0:
                /* swap prefix string */
                ds->ಅದಲುಬದಲು_Inv_Chars(ds, ಒಳ_Range+1, (int32_t)uprv_strlen((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಒಳ_Range+1)),
                                    ಹೊರಹೋಗು_Range+1, p_Error_ಸಂಕೇತ);
                ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    ಹಿಂತಿರುಗಿ 0;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_ಎಣಿಕೆ;

                    factors_ಎಣಿಕೆ=ಒಳ_Range->variant;
                    p=(ಸ್ಥಿರಾಂಕ uint16_t *)(ಒಳ_Range+1);
                    q=(uint16_t *)(ಹೊರಹೋಗು_Range+1);
                    ds->ಅದಲುಬದಲು_Array16(ds, p, (int32_t)(factors_ಎಣಿಕೆ*2), q, p_Error_ಸಂಕೇತ);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_ಎಣಿಕೆ;
                    q+=factors_ಎಣಿಕೆ;
                    ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ=(uint32_t)((ಒಳ_ಬೈಟುಗಳು+ಸ್ಥಾನಾಂತರ)-(ಸ್ಥಿರಾಂಕ uint8_t *)p);
                    ಆಗ(ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ>0 && ((ಸ್ಥಿರಾಂಕ uint8_t *)p)[ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ-1]!=0) {
                        --ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ;
                    }
                    ds->ಅದಲುಬದಲು_Inv_Chars(ds, p, (int32_t)ಅಕ್ಷರಸರಣಿಗಳು_ಎಣಿಕೆ, q, p_Error_ಸಂಕೇತ);
                }
                ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 ಒಳ_Range->type, i);
                *p_Error_ಸಂಕೇತ=U_UNSUPPORTED_ERROR;
                ಹಿಂತಿರುಗಿ 0;
            }
        }
    }

    ಹಿಂತಿರುಗಿ ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ+(int32_t)ಸ್ಥಾನಾಂತರ;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
