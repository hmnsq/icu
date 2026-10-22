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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchar.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "udataswp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅੰਕੜੇ_NAME[] = "unames";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ ਅੰਕੜੇ_TYPE[] = "icu";

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮੂਹ_ਖਿਸਕਾਓ 5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ (1L<<ਸਮੂਹ_ਖਿਸਕਾਓ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮੂਹ_ਨਕਾਬ (ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ-1)

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
ਗਿਣਤੀ {
    ਸਮੂਹ_MSB,
    ਸਮੂਹ_ਵਿੱਥ_ਉੱਚਾ,
    ਸਮੂਹ_ਵਿੱਥ_ਨੀਵਾਂ,
    ਸਮੂਹ_ਲੰਮਾਈ
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_ਸਮੂਹ_ਵਿੱਥ(ਸਮੂਹ) ((int32_t)(ਸਮੂਹ)[ਸਮੂਹ_ਵਿੱਥ_ਉੱਚਾ]<<16|(ਸਮੂਹ)[ਸਮੂਹ_ਵਿੱਥ_ਨੀਵਾਂ])

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਗਲਾ_ਸਮੂਹ(ਸਮੂਹ) ((ਸਮੂਹ)+ਸਮੂਹ_ਲੰਮਾਈ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PREV_ਸਮੂਹ(ਸਮੂਹ) ((ਸਮੂਹ)-ਸਮੂਹ_ਲੰਮਾਈ)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    uint32_t ਸ਼ੁਰੂ, ਸਿਰਾ;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    uint32_t token_String_ਵਿੱਥ, groups_ਵਿੱਥ, ਸਮੂਹ_String_ਵਿੱਥ, alg_ਨਾਂ_ਵਿੱਥ;
} UChar_ਨਾਂ;

ਨਾਮਾਕਾਸ਼ {
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
ਵਰਗ CharacterNameQuery {
  ਜਨਤਕ:
    ਵਰਗ Matcher {
      ਜਨਤਕ:
        // `query` must outlive the constructed object.
        Matcher(ਅਚਲ CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        ਤਾਰਕਿਕ consistentWith(ਅੱਖਰ c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            ਜੇ (skeletonIterator == query.skeleton.ਸਿਰਾ()) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਜੇ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                ਜੇ (ਆਖਰੀ_Char == '-') {
                    ਜੇ (*skeletonIterator++ != ਆਖਰੀ_Char) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                }
                ਆਖਰੀ_Char = c;
                ਵਾਪਸ ਸੱਚ;
            } ਨਹੀਂਤਾਂ ਜੇ (c == '-') {
                ਜੇ (ਆਖਰੀ_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.ਸਿਰਾ() - 2)) {
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
                    ਜੇ (*skeletonIterator++ != c) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                ਆਖਰੀ_Char = c;
                ਵਾਪਸ ਸੱਚ;
            } ਨਹੀਂਤਾਂ {
                ਜੇ (*skeletonIterator++ != c) {
                    ਵਾਪਸ ਝੂਠ;
                }
                ਆਖਰੀ_Char = c;
                ਵਾਪਸ ਸੱਚ;
            }
        }

        ਤਾਰਕਿਕ consistentWith(ਅਚਲ std::string_view substring) {
            ਲਈ (ਅਚਲ ਅੱਖਰ c : substring) {
                ਜੇ (!consistentWith(c)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ਵਾਪਸ ਸੱਚ;
        }

        ਤਾਰਕਿਕ matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(ਆਖਰੀ_Char != '-');
            ਵਾਪਸ skeletonIterator == query.skeleton.ਸਿਰਾ();
        }

        std::string_view ਬਚਿਆ_Significant_Characters() ਅਚਲ {
            ਵਾਪਸ query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      ਨਿਜੀ:
        ਅਚਲ CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        ਅੱਖਰ ਆਖਰੀ_Char = ' ';
    };

    ਸਪਸ਼ਟ CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        ਅੱਖਰ *skeleton_ਹੱਦਬੰਦੀ = skeleton_ਅੰਕੜੇ;
        ਲਈ (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_ਹੱਦਬੰਦੀ < skeleton_ਅੰਕੜੇ + ਆਕਾਰ(skeleton_ਅੰਕੜੇ));
            ਜੇ (skeleton_ਹੱਦਬੰਦੀ >= skeleton_ਅੰਕੜੇ + ਆਕਾਰ(skeleton_ਅੰਕੜੇ)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_ਹੱਦਬੰਦੀ = skeleton_ਅੰਕੜੇ;
                ਰੋਕੋ;
            }
            ਜੇ (query[i] == ' ' || query[i] == '_') {
                ਜਾਰੀ;
            }
            ਜੇ (query[i] == '-') {
                ਤਾਰਕਿਕ isMedial;
                ਤਾਰਕਿਕ is1180MedialHyphen = ਝੂਠ;
                ਜੇ (i == 0 || i == query.length() - 1) {
                    isMedial = ਝੂਠ;
                } ਨਹੀਂਤਾਂ {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                ਜੇ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_ਅੰਕੜੇ, skeleton_ਹੱਦਬੰਦੀ - skeleton_ਅੰਕੜੇ) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = ਸੱਚ;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    ਲਈ (std::size_t j = i + 2; j < query.length(); ++j) {
                        ਜੇ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = ਝੂਠ;
                        }
                    }
                }
                ਜੇ (!isMedial || is1180MedialHyphen) {
                    *skeleton_ਹੱਦਬੰਦੀ++ = query[i];
                }
            } ਨਹੀਂਤਾਂ {
                *skeleton_ਹੱਦਬੰਦੀ++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_ਅੰਕੜੇ, skeleton_ਹੱਦਬੰਦੀ - skeleton_ਅੰਕੜੇ);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ਅਚਲ {
        ਵਾਪਸ Matcher(*ਇਹ);
    }

ਨਿਜੀ:
    ਸਥਿਰ ਤਾਰਕਿਕ isASCIILetterOrDigit(ਅਚਲ ਅੱਖਰ c) {
        ਵਾਪਸ uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    ਅੱਖਰ skeleton_ਅੰਕੜੇ[120];
    std::string_view skeleton;
    ਤਾਰਕਿਕ is1180;
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_GROUPS(ਨਾਂ) (ਅਚਲ uint16_t *)((ਅਚਲ ਅੱਖਰ *)ਨਾਂ+ਨਾਂ->groups_ਵਿੱਥ)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ CharacterNameQuery *otherName;
    UChar32 ਕੋਡ;
} ਲੱਭੋ_Name;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DO_ਲੱਭੋ_NAME ਖਾਲੀ_ਸੂਚਕ

ਸਥਿਰ UData_ਸਮ੍ਰਿਤੀ *u_Char_ਨਾਂ_ਅੰਕੜੇ=ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ UChar_ਨਾਂ *u_Char_ਨਾਂ=ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ icu::UInitOnce g_Char_ਨਾਂ_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ਸਥਿਰ int32_t g_Max_Name_ਲੰਮਾਈ=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ਸਥਿਰ uint32_t g_Name_ਸੈੱਟ[8]={ 0 };

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_NONCHARACTER_ਕੋਡ_ਬਿੰਦੂ U_CHAR_CATEGORY_ਗਿਣਤੀ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_LEAD_SURROGATE U_CHAR_CATEGORY_ਗਿਣਤੀ + 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_TRAIL_SURROGATE U_CHAR_CATEGORY_ਗਿਣਤੀ + 2

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_CHAR_EXTENDED_CATEGORY_ਗਿਣਤੀ (U_CHAR_CATEGORY_ਗਿਣਤੀ + 3)

ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ char_Cat_ਨਾਂ[U_CHAR_EXTENDED_CATEGORY_ਗਿਣਤੀ] = {
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

ਸਥਿਰ UBool U_CALLCONV unames_cleanup()
{
    ਜੇ(u_Char_ਨਾਂ_ਅੰਕੜੇ) {
        udata_close(u_Char_ਨਾਂ_ਅੰਕੜੇ);
        u_Char_ਨਾਂ_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    }
    ਜੇ(u_Char_ਨਾਂ) {
        u_Char_ਨਾਂ = ਖਾਲੀ_ਸੂਚਕ;
    }
    g_Char_ਨਾਂ_Init_Once.reset();
    g_Max_Name_ਲੰਮਾਈ=0;
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UBool U_CALLCONV
isAcceptable(ਖਾਲੀ * /*context*/,
             ਅਚਲ ਅੱਖਰ * /*type*/, ਅਚਲ ਅੱਖਰ * /*name*/,
             ਅਚਲ UDataInfo *pInfo) {
    ਵਾਪਸ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ਅੰਕੜੇ_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x6e &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x61 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x6d &&
        pInfo->format_ਸੰਸਕਰਣ[0]==1;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
ਲੱਦੋ_Char_ਨਾਂ(UError_ਕੋਡ &ਸਥਿਤੀ) {
    U_ASSERT(u_Char_ਨਾਂ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ);
    U_ASSERT(u_Char_ਨਾਂ == ਖਾਲੀ_ਸੂਚਕ);

    u_Char_ਨਾਂ_ਅੰਕੜੇ = udata_openChoice(ਖਾਲੀ_ਸੂਚਕ, ਅੰਕੜੇ_TYPE, ਅੰਕੜੇ_NAME, isAcceptable, ਖਾਲੀ_ਸੂਚਕ, &ਸਥਿਤੀ);
    ਜੇ(U_FAILURE(ਸਥਿਤੀ)) {
        u_Char_ਨਾਂ_ਅੰਕੜੇ = ਖਾਲੀ_ਸੂਚਕ;
    } ਨਹੀਂਤਾਂ {
        u_Char_ਨਾਂ = (UChar_ਨਾਂ *)udata_get_ਸਮ੍ਰਿਤੀ(u_Char_ਨਾਂ_ਅੰਕੜੇ);
    }
    ucln_common_ਰਜਿਸਟਰ_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ਸਥਿਰ UBool
is_ਅੰਕੜੇ_Loaded(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    umtx_initOnce(g_Char_ਨਾਂ_Init_Once, &ਲੱਦੋ_Char_ਨਾਂ, *p_Error_ਕੋਡ);
    ਵਾਪਸ U_SUCCESS(*p_Error_ਕੋਡ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c) UPRV_ਖੰਡ_MACRO_BEGIN { \
    ਜੇ((ਬਫਰ_ਲੰਮਾਈ)>0) { \
        *(ਬਫਰ)++=c; \
        --(ਬਫਰ_ਲੰਮਾਈ); \
    } \
    ++(ਬਫਰ_Pos); \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ U_ISO_COMMENT U_CHAR_NAME_CHOICE_ਗਿਣਤੀ

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
ਸਥਿਰ uint16_t
expandName(UChar_ਨਾਂ *ਨਾਂ,
           ਅਚਲ uint8_t *name, uint16_t name_ਲੰਮਾਈ, UCharNameChoice nameChoice,
           ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    uint16_t* ਟੋਕਨ = reinterpret_cast<uint16_t*>(ਨਾਂ) + 8;
    uint16_t token, token_ਗਿਣਤੀ=*ਟੋਕਨ++, ਬਫਰ_Pos=0;
    uint8_t* token_ਲੜੀਆਂ = reinterpret_cast<uint8_t*>(ਨਾਂ) + ਨਾਂ->token_String_ਵਿੱਥ;
    uint8_t c;

    ਜੇ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ਜੇ (static_cast<uint8_t>(';') >= token_ਗਿਣਤੀ || ਟੋਕਨ[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ਪੂਰਨ ਖੇਤਰ_ਸੂਚਕਾਂਕ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ਕਰੋ {
                ਜਦੋਂ(name_ਲੰਮਾਈ>0) {
                    --name_ਲੰਮਾਈ;
                    ਜੇ(*name++==';') {
                        ਰੋਕੋ;
                    }
                }
            } ਜਦੋਂ(--ਖੇਤਰ_ਸੂਚਕਾਂਕ>0);
        } ਨਹੀਂਤਾਂ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ਲੰਮਾਈ=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    ਜਦੋਂ(name_ਲੰਮਾਈ>0) {
        --name_ਲੰਮਾਈ;
        c=*name++;

        ਜੇ(c>=token_ਗਿਣਤੀ) {
            ਜੇ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
            } ਨਹੀਂਤਾਂ {
                /*ਮੁਕੰਮਲ*/
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            token=ਟੋਕਨ[c];
            ਜੇ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ਟੋਕਨ[c<<8|*name++];
                --name_ਲੰਮਾਈ;
            }
            ਜੇ (token == static_cast<uint16_t>(-1)) {
                ਜੇ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
                } ਨਹੀਂਤਾਂ {
                    /*ਮੁਕੰਮਲ*/
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* write token word */
                uint8_t *tokenString=token_ਲੜੀਆਂ+token;
                ਜਦੋਂ((c=*tokenString++)!=0) {
                    WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    ਜੇ(ਬਫਰ_ਲੰਮਾਈ>0) {
        *ਬਫਰ=0;
    }

    ਵਾਪਸ ਬਫਰ_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ਸਥਿਰ UBool
compareName(UChar_ਨਾਂ *ਨਾਂ,
            ਅਚਲ uint8_t *name, uint16_t name_ਲੰਮਾਈ, UCharNameChoice nameChoice,
            ਅਚਲ CharacterNameQuery& otherName) {
    uint16_t* ਟੋਕਨ = reinterpret_cast<uint16_t*>(ਨਾਂ) + 8;
    uint16_t token, token_ਗਿਣਤੀ=*ਟੋਕਨ++;
    uint8_t* token_ਲੜੀਆਂ = reinterpret_cast<uint8_t*>(ਨਾਂ) + ਨਾਂ->token_String_ਵਿੱਥ;
    uint8_t c;
    ਸਵੈਚਾਲਿਤ matcher = otherName.matcher();

    ਜੇ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ਜੇ (static_cast<uint8_t>(';') >= token_ਗਿਣਤੀ || ਟੋਕਨ[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ਪੂਰਨ ਖੇਤਰ_ਸੂਚਕਾਂਕ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ਕਰੋ {
                ਜਦੋਂ(name_ਲੰਮਾਈ>0) {
                    --name_ਲੰਮਾਈ;
                    ਜੇ(*name++==';') {
                        ਰੋਕੋ;
                    }
                }
            } ਜਦੋਂ(--ਖੇਤਰ_ਸੂਚਕਾਂਕ>0);
        } ਨਹੀਂਤਾਂ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ਲੰਮਾਈ=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    ਜਦੋਂ(name_ਲੰਮਾਈ>0) {
        --name_ਲੰਮਾਈ;
        c=*name++;

        ਜੇ(c>=token_ਗਿਣਤੀ) {
            ਜੇ(c!=';') {
                /* implicit letter */
                ਜੇ (!matcher.consistentWith(c)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            } ਨਹੀਂਤਾਂ {
                /*ਮੁਕੰਮਲ*/
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            token=ਟੋਕਨ[c];
            ਜੇ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ਟੋਕਨ[c<<8|*name++];
                --name_ਲੰਮਾਈ;
            }
            ਜੇ (token == static_cast<uint16_t>(-1)) {
                ਜੇ(c!=';') {
                    /* explicit letter */
                    ਜੇ (!matcher.consistentWith(c)) {
                        ਵਾਪਸ ਝੂਠ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /*ਮੁਕੰਮਲ*/
                    ਰੋਕੋ;
                }
            } ਨਹੀਂਤਾਂ {
                /* write token word */
                ਜੇ (!matcher.consistentWith(reinterpret_cast<ਅਚਲ ਅੱਖਰ *>(token_ਲੜੀਆਂ + token))) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
        }
    }

    /* complete match? */
    ਵਾਪਸ matcher.matches();
}

ਸਥਿਰ uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    ਜੇ (U_IS_UNICODE_NONCHAR(cp)) {
        ਵਾਪਸ U_NONCHARACTER_ਕੋਡ_ਬਿੰਦੂ;
    }

    ਜੇ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    ਵਾਪਸ cat;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    ਜੇ (cat >= UPRV_LENGTHOF(char_Cat_ਨਾਂ)) {
        ਵਾਪਸ "unknown";
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ char_Cat_ਨਾਂ[cat];
    }
}

ਸਥਿਰ uint16_t getExtName(uint32_t ਕੋਡ, ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    ਅਚਲ ਅੱਖਰ *catname = getCharCatName(ਕੋਡ);
    uint16_t length = 0;

    UChar32 cp;
    ਪੂਰਨ ndigits, i;
    
    WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, length, '<');
    ਜਦੋਂ (catname[length - 1]) {
        WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, length, catname[length - 1]);
    }
    WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, length, '-');
    ਲਈ (cp = ਕੋਡ, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    ਜੇ (ndigits < 4)
        ndigits = 4;
    ਲਈ (cp = ਕੋਡ, i = ndigits; (cp || i > 0) && ਬਫਰ_ਲੰਮਾਈ; cp >>= 4, ਬਫਰ_ਲੰਮਾਈ--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        ਬਫਰ[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    ਬਫਰ += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, length, '>');

    ਵਾਪਸ length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ਸਥਿਰ ਅਚਲ uint16_t *
get_ਸਮੂਹ(UChar_ਨਾਂ *ਨਾਂ, uint32_t ਕੋਡ) {
    ਅਚਲ uint16_t *groups=GET_GROUPS(ਨਾਂ);
    uint16_t ਸਮੂਹ_MSB_2 = static_cast<uint16_t>(ਕੋਡ >> ਸਮੂਹ_ਖਿਸਕਾਓ),
             ਸ਼ੁਰੂ=0,
             ਹੱਦਬੰਦੀ=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    ਜਦੋਂ(ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ-1) {
        number = static_cast<uint16_t>((ਸ਼ੁਰੂ + ਹੱਦਬੰਦੀ) / 2);
        ਜੇ(ਸਮੂਹ_MSB_2<groups[number*ਸਮੂਹ_ਲੰਮਾਈ+ਸਮੂਹ_MSB]) {
            ਹੱਦਬੰਦੀ=number;
        } ਨਹੀਂਤਾਂ {
            ਸ਼ੁਰੂ=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    ਵਾਪਸ groups+ਸ਼ੁਰੂ*ਸਮੂਹ_ਲੰਮਾਈ;
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
ਸਥਿਰ ਅਚਲ uint8_t *
expand_ਸਮੂਹ_Lengths(ਅਚਲ uint8_t *s,
                   uint16_t offsets[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+1], uint16_t lengths[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, ਵਿੱਥ=0, length=0;
    uint8_t ਲੰਮਾਈ_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    ਜਦੋਂ(i<ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ) {
        ਲੰਮਾਈ_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        ਜੇ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | ਲੰਮਾਈ_Byte >> 4) + 12);
            ਲੰਮਾਈ_Byte&=0xf;
        } ਨਹੀਂਤਾਂ ਜੇ((ਲੰਮਾਈ_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((ਲੰਮਾਈ_Byte & 0x3f) + 12);
        } ਨਹੀਂਤਾਂ {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(ਲੰਮਾਈ_Byte >> 4);
            ਲੰਮਾਈ_Byte&=0xf;
        }

        *offsets++=ਵਿੱਥ;
        *lengths++=length;

        ਵਿੱਥ+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        ਜੇ((ਲੰਮਾਈ_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=ਲੰਮਾਈ_Byte;
            ਜੇ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=ਵਿੱਥ;
                *lengths++=length;

                ਵਿੱਥ+=length;
                ++i;
            }
        } ਨਹੀਂਤਾਂ {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    ਵਾਪਸ s;
}

ਸਥਿਰ uint16_t
expand_ਸਮੂਹ_Name(UChar_ਨਾਂ *ਨਾਂ, ਅਚਲ uint16_t *ਸਮੂਹ,
                uint16_t ਸਤਰ_ਸੰਖਿਆ, UCharNameChoice nameChoice,
                ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    uint16_t offsets[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2], lengths[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2];
    ਅਚਲ uint8_t* s = reinterpret_cast<uint8_t*>(ਨਾਂ) + ਨਾਂ->ਸਮੂਹ_String_ਵਿੱਥ + GET_ਸਮੂਹ_ਵਿੱਥ(ਸਮੂਹ);
    s=expand_ਸਮੂਹ_Lengths(s, offsets, lengths);
    ਵਾਪਸ expandName(ਨਾਂ, s+offsets[ਸਤਰ_ਸੰਖਿਆ], lengths[ਸਤਰ_ਸੰਖਿਆ], nameChoice,
                      ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ);
}

ਸਥਿਰ uint16_t
getName(UChar_ਨਾਂ *ਨਾਂ, uint32_t ਕੋਡ, UCharNameChoice nameChoice,
        ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    ਅਚਲ uint16_t *ਸਮੂਹ=get_ਸਮੂਹ(ਨਾਂ, ਕੋਡ);
    ਜੇ (static_cast<uint16_t>(ਕੋਡ >> ਸਮੂਹ_ਖਿਸਕਾਓ) == ਸਮੂਹ[ਸਮੂਹ_MSB]) {
        ਵਾਪਸ expand_ਸਮੂਹ_Name(ਨਾਂ, ਸਮੂਹ, static_cast<uint16_t>(ਕੋਡ & ਸਮੂਹ_ਨਕਾਬ), nameChoice,
                               ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ);
    } ਨਹੀਂਤਾਂ {
        /* group not found */
        /* zero-terminate */
        ਜੇ(ਬਫਰ_ਲੰਮਾਈ>0) {
            *ਬਫਰ=0;
        }
        ਵਾਪਸ 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ਸਥਿਰ UBool
enum_ਸਮੂਹ_ਨਾਂ(UChar_ਨਾਂ *ਨਾਂ, ਅਚਲ uint16_t *ਸਮੂਹ,
               UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
               UEnum_Char_ਨਾਂ_Fn *fn, ਖਾਲੀ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2], lengths[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2];
    ਅਚਲ uint8_t* s = reinterpret_cast<uint8_t*>(ਨਾਂ) + ਨਾਂ->ਸਮੂਹ_String_ਵਿੱਥ + GET_ਸਮੂਹ_ਵਿੱਥ(ਸਮੂਹ);

    s=expand_ਸਮੂਹ_Lengths(s, offsets, lengths);
    ਜੇ(fn!=DO_ਲੱਭੋ_NAME) {
        ਅੱਖਰ ਬਫਰ[200];
        uint16_t length;

        ਜਦੋਂ(ਸ਼ੁਰੂ<=ਸਿਰਾ) {
            length=expandName(ਨਾਂ, s+offsets[ਸ਼ੁਰੂ&ਸਮੂਹ_ਨਕਾਬ], lengths[ਸ਼ੁਰੂ&ਸਮੂਹ_ਨਕਾਬ], nameChoice, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
            ਜੇ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                ਬਫਰ[length = getExtName(ਸ਼ੁਰੂ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            ਜੇ(length>0) {
                ਜੇ(!fn(context, ਸ਼ੁਰੂ, nameChoice, ਬਫਰ, length)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ++ਸ਼ੁਰੂ;
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ CharacterNameQuery& otherName = *static_cast<ਲੱਭੋ_Name*>(context)->otherName;
        ਜਦੋਂ(ਸ਼ੁਰੂ<=ਸਿਰਾ) {
            ਜੇ(compareName(ਨਾਂ, s+offsets[ਸ਼ੁਰੂ&ਸਮੂਹ_ਨਕਾਬ], lengths[ਸ਼ੁਰੂ&ਸਮੂਹ_ਨਕਾਬ], nameChoice, otherName)) {
                static_cast<ਲੱਭੋ_Name*>(context)->ਕੋਡ = ਸ਼ੁਰੂ;
                ਵਾਪਸ ਝੂਠ;
            }
            ++ਸ਼ੁਰੂ;
        }
    }
    ਵਾਪਸ ਸੱਚ;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ਸਥਿਰ UBool
enum_Ext_ਨਾਂ(UChar32 ਸ਼ੁਰੂ, UChar32 ਸਿਰਾ,
             UEnum_Char_ਨਾਂ_Fn *fn, ਖਾਲੀ *context)
{
    ਜੇ(fn!=DO_ਲੱਭੋ_NAME) {
        ਅੱਖਰ ਬਫਰ[200];
        uint16_t length;
        
        ਜਦੋਂ(ਸ਼ੁਰੂ<=ਸਿਰਾ) {
            ਬਫਰ[length = getExtName(ਸ਼ੁਰੂ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ))] = 0;
            /* here, we assume that the buffer is large enough */
            ਜੇ(length>0) {
                ਜੇ(!fn(context, ਸ਼ੁਰੂ, U_EXTENDED_CHAR_NAME, ਬਫਰ, length)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ++ਸ਼ੁਰੂ;
        }
    }

    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ UBool
enum_ਨਾਂ(UChar_ਨਾਂ *ਨਾਂ,
          UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
          UEnum_Char_ਨਾਂ_Fn *fn, ਖਾਲੀ *context,
          UCharNameChoice nameChoice) {
    uint16_t ਸ਼ੁਰੂ_ਸਮੂਹ_MSB, ਸਿਰਾ_ਸਮੂਹ_MSB, ਸਮੂਹ_ਗਿਣਤੀ;
    ਅਚਲ uint16_t *ਸਮੂਹ, *ਸਮੂਹ_ਹੱਦਬੰਦੀ;

    ਸ਼ੁਰੂ_ਸਮੂਹ_MSB = static_cast<uint16_t>(ਸ਼ੁਰੂ >> ਸਮੂਹ_ਖਿਸਕਾਓ);
    ਸਿਰਾ_ਸਮੂਹ_MSB = static_cast<uint16_t>((ਹੱਦਬੰਦੀ - 1) >> ਸਮੂਹ_ਖਿਸਕਾਓ);

    /* find the group that contains start, or the highest before it */
    ਸਮੂਹ=get_ਸਮੂਹ(ਨਾਂ, ਸ਼ੁਰੂ);

    ਜੇ(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB<ਸਮੂਹ[ਸਮੂਹ_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_ਹੱਦਬੰਦੀ = static_cast<UChar32>(ਸਮੂਹ[ਸਮੂਹ_MSB]) << ਸਮੂਹ_ਖਿਸਕਾਓ;
        ਜੇ(ext_ਹੱਦਬੰਦੀ>ਹੱਦਬੰਦੀ) {
            ext_ਹੱਦਬੰਦੀ=ਹੱਦਬੰਦੀ;
        }
        ਜੇ(!enum_Ext_ਨਾਂ(ਸ਼ੁਰੂ, ext_ਹੱਦਬੰਦੀ-1, fn, context)) {
            ਵਾਪਸ ਝੂਠ;
        }
        ਸ਼ੁਰੂ=ext_ਹੱਦਬੰਦੀ;
    }

    ਜੇ(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB==ਸਿਰਾ_ਸਮੂਹ_MSB) {
        ਜੇ(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB==ਸਮੂਹ[ਸਮੂਹ_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            ਵਾਪਸ enum_ਸਮੂਹ_ਨਾਂ(ਨਾਂ, ਸਮੂਹ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ-1, fn, context, nameChoice);
        }
    } ਨਹੀਂਤਾਂ {
        ਅਚਲ uint16_t *groups=GET_GROUPS(ਨਾਂ);
        ਸਮੂਹ_ਗਿਣਤੀ=*groups++;
        ਸਮੂਹ_ਹੱਦਬੰਦੀ=groups+ਸਮੂਹ_ਗਿਣਤੀ*ਸਮੂਹ_ਲੰਮਾਈ;

        ਜੇ(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB==ਸਮੂਹ[ਸਮੂਹ_MSB]) {
            /* enumerate characters in the partial start group */
            ਜੇ((ਸ਼ੁਰੂ&ਸਮੂਹ_ਨਕਾਬ)!=0) {
                ਜੇ(!enum_ਸਮੂਹ_ਨਾਂ(ਨਾਂ, ਸਮੂਹ,
                                   ਸ਼ੁਰੂ, (static_cast<UChar32>(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB) << ਸਮੂਹ_ਖਿਸਕਾਓ) + ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ - 1,
                                   fn, context, nameChoice)) {
                    ਵਾਪਸ ਝੂਠ;
                }
                ਸਮੂਹ=ਅਗਲਾ_ਸਮੂਹ(ਸਮੂਹ); /* continue with the next group */
            }
        } ਨਹੀਂਤਾਂ ਜੇ(ਸ਼ੁਰੂ_ਸਮੂਹ_MSB>ਸਮੂਹ[ਸਮੂਹ_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ਅਚਲ uint16_t *ਅਗਲਾ_ਸਮੂਹ_2=ਅਗਲਾ_ਸਮੂਹ(ਸਮੂਹ);
            ਜੇ (ਅਗਲਾ_ਸਮੂਹ_2 < ਸਮੂਹ_ਹੱਦਬੰਦੀ && ਅਗਲਾ_ਸਮੂਹ_2[ਸਮੂਹ_MSB] > ਸ਼ੁਰੂ_ਸਮੂਹ_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 ਸਿਰਾ = ਅਗਲਾ_ਸਮੂਹ_2[ਸਮੂਹ_MSB] << ਸਮੂਹ_ਖਿਸਕਾਓ;
                ਜੇ (ਸਿਰਾ > ਹੱਦਬੰਦੀ) {
                    ਸਿਰਾ = ਹੱਦਬੰਦੀ;
                }
                ਜੇ (!enum_Ext_ਨਾਂ(ਸ਼ੁਰੂ, ਸਿਰਾ - 1, fn, context)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ਸਮੂਹ=ਅਗਲਾ_ਸਮੂਹ_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        ਜਦੋਂ(ਸਮੂਹ<ਸਮੂਹ_ਹੱਦਬੰਦੀ && ਸਮੂਹ[ਸਮੂਹ_MSB]<ਸਿਰਾ_ਸਮੂਹ_MSB) {
            ਅਚਲ uint16_t *ਅਗਲਾ_ਸਮੂਹ_2;
            ਸ਼ੁਰੂ = static_cast<UChar32>(ਸਮੂਹ[ਸਮੂਹ_MSB]) << ਸਮੂਹ_ਖਿਸਕਾਓ;
            ਜੇ(!enum_ਸਮੂਹ_ਨਾਂ(ਨਾਂ, ਸਮੂਹ, ਸ਼ੁਰੂ, ਸ਼ੁਰੂ+ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ-1, fn, context, nameChoice)) {
                ਵਾਪਸ ਝੂਠ;
            }
            ਅਗਲਾ_ਸਮੂਹ_2=ਅਗਲਾ_ਸਮੂਹ(ਸਮੂਹ);
            ਜੇ (ਅਗਲਾ_ਸਮੂਹ_2 < ਸਮੂਹ_ਹੱਦਬੰਦੀ && ਅਗਲਾ_ਸਮੂਹ_2[ਸਮੂਹ_MSB] > ਸਮੂਹ[ਸਮੂਹ_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 ਸਿਰਾ = ਅਗਲਾ_ਸਮੂਹ_2[ਸਮੂਹ_MSB] << ਸਮੂਹ_ਖਿਸਕਾਓ;
                ਜੇ (ਸਿਰਾ > ਹੱਦਬੰਦੀ) {
                    ਸਿਰਾ = ਹੱਦਬੰਦੀ;
                }
                ਜੇ (!enum_Ext_ਨਾਂ((ਸਮੂਹ[ਸਮੂਹ_MSB] + 1) << ਸਮੂਹ_ਖਿਸਕਾਓ, ਸਿਰਾ - 1, fn, context)) {
                    ਵਾਪਸ ਝੂਠ;
                }
            }
            ਸਮੂਹ=ਅਗਲਾ_ਸਮੂਹ_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        ਜੇ(ਸਮੂਹ<ਸਮੂਹ_ਹੱਦਬੰਦੀ && ਸਮੂਹ[ਸਮੂਹ_MSB]==ਸਿਰਾ_ਸਮੂਹ_MSB) {
            ਵਾਪਸ enum_ਸਮੂਹ_ਨਾਂ(ਨਾਂ, ਸਮੂਹ, (ਹੱਦਬੰਦੀ-1)&~ਸਮੂਹ_ਨਕਾਬ, ਹੱਦਬੰਦੀ-1, fn, context, nameChoice);
        } ਨਹੀਂਤਾਂ ਜੇ (nameChoice == U_EXTENDED_CHAR_NAME && ਸਮੂਹ == ਸਮੂਹ_ਹੱਦਬੰਦੀ) {
            UChar32 next = (PREV_ਸਮੂਹ(ਸਮੂਹ)[ਸਮੂਹ_MSB] + 1) << ਸਮੂਹ_ਖਿਸਕਾਓ;
            ਜੇ (next > ਸ਼ੁਰੂ) {
                ਸ਼ੁਰੂ = next;
            }
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਸੱਚ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    ਜੇ (nameChoice == U_EXTENDED_CHAR_NAME) {
        ਜੇ (ਹੱਦਬੰਦੀ > UCHAR_MAX_ਮੁੱਲ + 1) {
            ਹੱਦਬੰਦੀ = UCHAR_MAX_ਮੁੱਲ + 1;
        }
        ਵਾਪਸ enum_Ext_ਨਾਂ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ - 1, fn, context);
    }
    
    ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ uint16_t
write_ਗੁਣਕ_Suffix(ਅਚਲ uint16_t *factors, uint16_t count,
                  ਅਚਲ ਅੱਖਰ *s, /* suffix elements */
                  uint32_t ਕੋਡ,
                  uint16_t indexes[8], /* output fields from here */
                  ਅਚਲ ਅੱਖਰ *elementBases[8], ਅਚਲ ਅੱਖਰ *elements[8],
                  ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    uint16_t i, ਗੁਣਕ, ਬਫਰ_Pos=0;
    ਅੱਖਰ c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    ਲਈ(i=count; i>0; --i) {
        ਗੁਣਕ=factors[i];
        indexes[i] = static_cast<uint16_t>(ਕੋਡ % ਗੁਣਕ);
        ਕੋਡ/=ਗੁਣਕ;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(ਕੋਡ);

    /* write each element */
    ਲਈ(;;) {
        ਜੇ(elementBases!=ਖਾਲੀ_ਸੂਚਕ) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ਗੁਣਕ=indexes[i];
        ਜਦੋਂ(ਗੁਣਕ>0) {
            ਜਦੋਂ(*s++!=0) {}
            --ਗੁਣਕ;
        }
        ਜੇ(elements!=ਖਾਲੀ_ਸੂਚਕ) {
            *elements++=s;
        }

        /* write element */
        ਜਦੋਂ((c=*s++)!=0) {
            WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        ਜੇ(i>=count) {
            ਰੋਕੋ;
        }

        /* skip the rest of the strings for this factors[i] */
        ਗੁਣਕ = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        ਜਦੋਂ(ਗੁਣਕ>0) {
            ਜਦੋਂ(*s++!=0) {}
            --ਗੁਣਕ;
        }

        ++i;
    }

    /* zero-terminate */
    ਜੇ(ਬਫਰ_ਲੰਮਾਈ>0) {
        *ਬਫਰ=0;
    }

    ਵਾਪਸ ਬਫਰ_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ਸਥਿਰ uint16_t
getAlgName(AlgorithmicRange *range, uint32_t ਕੋਡ, UCharNameChoice nameChoice,
        ਅੱਖਰ *ਬਫਰ, uint16_t ਬਫਰ_ਲੰਮਾਈ) {
    uint16_t ਬਫਰ_Pos=0;

    /* Only the normative character name can be algorithmic. */
    ਜੇ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        ਜੇ(ਬਫਰ_ਲੰਮਾਈ>0) {
            *ਬਫਰ=0;
        }
        ਵਾਪਸ 0;
    }

    ਚੋਣ(range->type) {
    ਹਾਲਤ 0: {
        /* name = prefix hex-digits */
        ਅਚਲ ਅੱਖਰ* s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(range + 1);
        ਅੱਖਰ c;

        uint16_t i, count;

        /* copy prefix */
        ਜਦੋਂ((c=*s++)!=0) {
            WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        ਜੇ(count<ਬਫਰ_ਲੰਮਾਈ) {
            ਬਫਰ[count]=0;
        }

        ਲਈ(i=count; i>0;) {
            ਜੇ(--i<ਬਫਰ_ਲੰਮਾਈ) {
                c = static_cast<ਅੱਖਰ>(ਕੋਡ & 0xf);
                ਜੇ(c<10) {
                    c+='0';
                } ਨਹੀਂਤਾਂ {
                    c+='A'-10;
                }
                ਬਫਰ[i]=c;
            }
            ਕੋਡ>>=4;
        }

        ਬਫਰ_Pos+=count;
        ਰੋਕੋ;
    }
    ਹਾਲਤ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ਅਚਲ uint16_t* factors = reinterpret_cast<ਅਚਲ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ਅਚਲ ਅੱਖਰ* s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(factors + count);
        ਅੱਖਰ c;

        /* copy prefix */
        ਜਦੋਂ((c=*s++)!=0) {
            WRITE_CHAR(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, ਬਫਰ_Pos, c);
        }

        ਬਫਰ_Pos+=write_ਗੁਣਕ_Suffix(factors, count,
                                     s, ਕੋਡ-range->ਸ਼ੁਰੂ, indexes, ਖਾਲੀ_ਸੂਚਕ, ਖਾਲੀ_ਸੂਚਕ, ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ);
        ਰੋਕੋ;
    }
    ਮੂਲ:
        /* undefined type */
        /* zero-terminate */
        ਜੇ(ਬਫਰ_ਲੰਮਾਈ>0) {
            *ਬਫਰ=0;
        }
        ਰੋਕੋ;
    }

    ਵਾਪਸ ਬਫਰ_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ਸਥਿਰ UBool
enum_Alg_ਨਾਂ(AlgorithmicRange *range,
             UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
             UEnum_Char_ਨਾਂ_Fn *fn, ਖਾਲੀ *context,
             UCharNameChoice nameChoice) {
    ਅੱਖਰ ਬਫਰ[200];
    uint16_t length;

    ਜੇ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ਵਾਪਸ ਸੱਚ;
    }

    ਚੋਣ(range->type) {
    ਹਾਲਤ 0: {
        ਅੱਖਰ *s, *ਸਿਰਾ;
        ਅੱਖਰ c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(ਸ਼ੁਰੂ), nameChoice, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
        ਜੇ(length<=0) {
            ਵਾਪਸ ਸੱਚ;
        }

        /* call the enumerator function with this first character */
        ਜੇ(!fn(context, ਸ਼ੁਰੂ, nameChoice, ਬਫਰ, length)) {
            ਵਾਪਸ ਝੂਠ;
        }

        /* go to the end of the name; all these names have the same length */
        ਸਿਰਾ=ਬਫਰ;
        ਜਦੋਂ(*ਸਿਰਾ!=0) {
            ++ਸਿਰਾ;
        }

        /* enumerate the rest of the names */
        ਜਦੋਂ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
            /* increment the hexadecimal number on a character-basis */
            s=ਸਿਰਾ;
            ਲਈ (;;) {
                c=*--s;
                ਜੇ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<ਅੱਖਰ>(c + 1);
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(c=='9') {
                    *s='A';
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ ਜੇ(c=='F') {
                    *s='0';
                }
            }

            ਜੇ(!fn(context, ਸ਼ੁਰੂ, nameChoice, ਬਫਰ, length)) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ 1: {
        uint16_t indexes[8];
        ਅਚਲ ਅੱਖਰ *elementBases[8], *elements[8];
        ਅਚਲ uint16_t* factors = reinterpret_cast<ਅਚਲ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ਅਚਲ ਅੱਖਰ* s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(factors + count);
        ਅੱਖਰ *suffix, *t;
        uint16_t ਅਗੇਤਰ_ਲੰਮਾਈ, i, idx;

        ਅੱਖਰ c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=ਬਫਰ;
        ਅਗੇਤਰ_ਲੰਮਾਈ=0;
        ਜਦੋਂ((c=*s++)!=0) {
            *suffix++=c;
            ++ਅਗੇਤਰ_ਲੰਮਾਈ;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(ਅਗੇਤਰ_ਲੰਮਾਈ + write_ਗੁਣਕ_Suffix(factors, count,
                                              s, static_cast<uint32_t>(ਸ਼ੁਰੂ) - range->ਸ਼ੁਰੂ,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(ਆਕਾਰ(ਬਫਰ) - ਅਗੇਤਰ_ਲੰਮਾਈ)));

        /* call the enumerator function with this first character */
        ਜੇ(!fn(context, ਸ਼ੁਰੂ, nameChoice, ਬਫਰ, length)) {
            ਵਾਪਸ ਝੂਠ;
        }

        /* enumerate the rest of the names */
        ਜਦੋਂ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ਲਈ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ਜੇ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ਜਦੋਂ(*s++!=0) {
                    }
                    elements[i]=s;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=ਅਗੇਤਰ_ਲੰਮਾਈ;
            ਲਈ(i=0; i<count; ++i) {
                s=elements[i];
                ਜਦੋਂ((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            ਜੇ(!fn(context, ਸ਼ੁਰੂ, nameChoice, ਬਫਰ, length)) {
                ਵਾਪਸ ਝੂਠ;
            }
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        /* undefined type */
        ਰੋਕੋ;
    }

    ਵਾਪਸ ਸੱਚ;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ਸਥਿਰ UChar32
ਲੱਭੋ_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 ਕੋਡ;
    ਸਵੈਚਾਲਿਤ matcher = query.matcher();

    ਜੇ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ਵਾਪਸ 0xffff;
    }

    ਚੋਣ(range->type) {
    ਹਾਲਤ 0: {
        /* name = prefix hex-digits */
        ਅਚਲ ਅੱਖਰ* s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(range + 1);

        /* compare prefix */
        ਜੇ (!matcher.consistentWith(s)) {
            ਵਾਪਸ 0xffff;
        }

        /* read hexadecimal code point value */
        ਕੋਡ=0;
        ਲਈ(ਅੱਖਰ c : matcher.ਬਚਿਆ_Significant_Characters()) {
            ਜੇ('0'<=c && c<='9') {
                ਕੋਡ=(ਕੋਡ<<4)|(c-'0');
            } ਨਹੀਂਤਾਂ ਜੇ('A'<=c && c<='F') {
                ਕੋਡ=(ਕੋਡ<<4)|(c-'A'+10);
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ 0xffff;
            }
        }

        /* does it fit into the range? */
        ਜੇ (range->ਸ਼ੁਰੂ <= static_cast<uint32_t>(ਕੋਡ) && static_cast<uint32_t>(ਕੋਡ) <= range->ਸਿਰਾ) {
            ਵਾਪਸ ਕੋਡ;
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ 1: {
        ਅੱਖਰ ਬਫਰ[64];
        uint16_t indexes[8];
        ਅਚਲ ਅੱਖਰ *elementBases[8], *elements[8];
        ਅਚਲ uint16_t* factors = reinterpret_cast<ਅਚਲ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ਅਚਲ ਅੱਖਰ *s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(factors + count);
        UChar32 ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view ਅਗੇਤਰ = s;
        ਜੇ (!matcher.consistentWith(ਅਗੇਤਰ)) {
            ਵਾਪਸ 0xffff;
        }

        s += ਅਗੇਤਰ.length() + 1;

        ਸ਼ੁਰੂ = static_cast<UChar32>(range->ਸ਼ੁਰੂ);
        ਹੱਦਬੰਦੀ = static_cast<UChar32>(range->ਸਿਰਾ + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.ਬਚਿਆ_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ਗੁਣਕ_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));

        /* compare the first suffix */
        ਜੇ(querySuffix == ਬਫਰ) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }

        /* enumerate and compare the rest of the suffixes */
        ਜਦੋਂ(++ਸ਼ੁਰੂ<ਹੱਦਬੰਦੀ) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ਲਈ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ਜੇ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ਜਦੋਂ(*s++!=0) {}
                    elements[i]=s;
                    ਰੋਕੋ;
                } ਨਹੀਂਤਾਂ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            ਸਵੈਚਾਲਿਤ it = querySuffix.begin();
            ਲਈ(i=0; i<count; ++i) {
                s=elements[i];
                ਅੱਖਰ c;
                ਜਦੋਂ((c=*s++)!=0) {
                    ਜੇ(it == querySuffix.ਸਿਰਾ() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            ਜੇ(i<99 && it == querySuffix.ਸਿਰਾ()) {
                ਵਾਪਸ ਸ਼ੁਰੂ;
            }
        }
        ਰੋਕੋ;
    }
    ਮੂਲ:
        /* undefined type */
        ਰੋਕੋ;
    }

    ਵਾਪਸ 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਜੋੜੋ_3(ਸੈੱਟ, c) ((ਸੈੱਟ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(ਸੈੱਟ, c) (((ਸੈੱਟ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ਸਥਿਰ int32_t
calc_String_ਸੈੱਟ_ਲੰਮਾਈ(uint32_t ਸੈੱਟ[8], ਅਚਲ ਅੱਖਰ *s) {
    int32_t length=0;
    ਅੱਖਰ c;

    ਜਦੋਂ((c=*s++)!=0) {
        ਸੈੱਟ_ਜੋੜੋ_3(ਸੈੱਟ, c);
        ++length;
    }
    ਵਾਪਸ length;
}

ਸਥਿਰ int32_t
calcAlgNameSetsLengths(int32_t max_Name_ਲੰਮਾਈ) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_ਗਿਣਤੀ;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_ਨਾਂ) + u_Char_ਨਾਂ->alg_ਨਾਂ_ਵਿੱਥ);
    range_ਗਿਣਤੀ=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    ਜਦੋਂ(range_ਗਿਣਤੀ>0) {
        ਚੋਣ(range->type) {
        ਹਾਲਤ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*ਅਗੇਤਰ*/
            length = calc_String_ਸੈੱਟ_ਲੰਮਾਈ(g_Name_ਸੈੱਟ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(range + 1)) + range->variant;
            ਜੇ(length>max_Name_ਲੰਮਾਈ) {
                max_Name_ਲੰਮਾਈ=length;
            }
            ਰੋਕੋ;
        ਹਾਲਤ 1: {
            /* name = prefix factorized-elements */
            ਅਚਲ uint16_t* factors = reinterpret_cast<ਅਚਲ uint16_t*>(range + 1);
            ਅਚਲ ਅੱਖਰ *s;
            int32_t i, count=range->variant, ਗੁਣਕ, ਗੁਣਕ_ਲੰਮਾਈ, max_ਗੁਣਕ_ਲੰਮਾਈ;

            /* prefix length */
            s = reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(factors + count);
            length=calc_String_ਸੈੱਟ_ਲੰਮਾਈ(g_Name_ਸੈੱਟ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            ਲਈ(i=0; i<count; ++i) {
                max_ਗੁਣਕ_ਲੰਮਾਈ=0;
                ਲਈ(ਗੁਣਕ=factors[i]; ਗੁਣਕ>0; --ਗੁਣਕ) {
                    ਗੁਣਕ_ਲੰਮਾਈ=calc_String_ਸੈੱਟ_ਲੰਮਾਈ(g_Name_ਸੈੱਟ, s);
                    s+=ਗੁਣਕ_ਲੰਮਾਈ+1;
                    ਜੇ(ਗੁਣਕ_ਲੰਮਾਈ>max_ਗੁਣਕ_ਲੰਮਾਈ) {
                        max_ਗੁਣਕ_ਲੰਮਾਈ=ਗੁਣਕ_ਲੰਮਾਈ;
                    }
                }
                length+=max_ਗੁਣਕ_ਲੰਮਾਈ;
            }

            ਜੇ(length>max_Name_ਲੰਮਾਈ) {
                max_Name_ਲੰਮਾਈ=length;
            }
            ਰੋਕੋ;
        }
        ਮੂਲ:
            /* unknown type */
            ਰੋਕੋ;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_ਗਿਣਤੀ;
    }
    ਵਾਪਸ max_Name_ਲੰਮਾਈ;
}

ਸਥਿਰ int32_t
calcExtNameSetsLengths(int32_t max_Name_ਲੰਮਾਈ) {
    int32_t i, length;

    ਲਈ(i=0; i<UPRV_LENGTHOF(char_Cat_ਨਾਂ); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_ਸੈੱਟ_ਲੰਮਾਈ(g_Name_ਸੈੱਟ, char_Cat_ਨਾਂ[i]);
        ਜੇ(length>max_Name_ਲੰਮਾਈ) {
            max_Name_ਲੰਮਾਈ=length;
        }
    }
    ਵਾਪਸ max_Name_ਲੰਮਾਈ;
}

ਸਥਿਰ int32_t
calc_Name_ਸੈੱਟ_ਲੰਮਾਈ(ਅਚਲ uint16_t *ਟੋਕਨ, uint16_t token_ਗਿਣਤੀ, ਅਚਲ uint8_t *token_ਲੜੀਆਂ, int8_t *tokenLengths,
                  uint32_t ਸੈੱਟ[8],
                  ਅਚਲ uint8_t **p_ਸਤਰ, ਅਚਲ uint8_t *ਸਤਰ_ਹੱਦਬੰਦੀ) {
    ਅਚਲ uint8_t *ਸਤਰ=*p_ਸਤਰ;
    int32_t length=0, token_ਲੰਮਾਈ;
    uint16_t c, token;

    ਜਦੋਂ (ਸਤਰ != ਸਤਰ_ਹੱਦਬੰਦੀ && (c = *ਸਤਰ++) != static_cast<uint8_t>(';')) {
        ਜੇ(c>=token_ਗਿਣਤੀ) {
            /* implicit letter */
            ਸੈੱਟ_ਜੋੜੋ_3(ਸੈੱਟ, c);
            ++length;
        } ਨਹੀਂਤਾਂ {
            token=ਟੋਕਨ[c];
            ਜੇ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*ਸਤਰ++;
                token=ਟੋਕਨ[c];
            }
            ਜੇ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                ਸੈੱਟ_ਜੋੜੋ_3(ਸੈੱਟ, c);
                ++length;
            } ਨਹੀਂਤਾਂ {
                /* count token word */
                ਜੇ(tokenLengths!=ਖਾਲੀ_ਸੂਚਕ) {
                    /* use cached token length */
                    token_ਲੰਮਾਈ=tokenLengths[c];
                    ਜੇ(token_ਲੰਮਾਈ==0) {
                        token_ਲੰਮਾਈ = calc_String_ਸੈੱਟ_ਲੰਮਾਈ(ਸੈੱਟ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(token_ਲੜੀਆਂ) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_ਲੰਮਾਈ);
                    }
                } ਨਹੀਂਤਾਂ {
                    token_ਲੰਮਾਈ = calc_String_ਸੈੱਟ_ਲੰਮਾਈ(ਸੈੱਟ, reinterpret_cast<ਅਚਲ ਅੱਖਰ*>(token_ਲੜੀਆਂ) + token);
                }
                length+=token_ਲੰਮਾਈ;
            }
        }
    }

    *p_ਸਤਰ=ਸਤਰ;
    ਵਾਪਸ length;
}

ਸਥਿਰ ਖਾਲੀ
calc_ਸਮੂਹ_Name_Sets_Lengths(int32_t max_Name_ਲੰਮਾਈ) {
    uint16_t offsets[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2], lengths[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+2];

    uint16_t* ਟੋਕਨ = reinterpret_cast<uint16_t*>(u_Char_ਨਾਂ) + 8;
    uint16_t token_ਗਿਣਤੀ=*ਟੋਕਨ++;
    uint8_t* token_ਲੜੀਆਂ = reinterpret_cast<uint8_t*>(u_Char_ਨਾਂ) + u_Char_ਨਾਂ->token_String_ਵਿੱਥ;

    int8_t *tokenLengths;

    ਅਚਲ uint16_t *ਸਮੂਹ;
    ਅਚਲ uint8_t *s, *ਸਤਰ, *ਸਤਰ_ਹੱਦਬੰਦੀ;

    int32_t ਸਮੂਹ_ਗਿਣਤੀ, ਸਤਰ_ਸੰਖਿਆ, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_ਗਿਣਤੀ));
    ਜੇ(tokenLengths!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_memset(tokenLengths, 0, token_ਗਿਣਤੀ);
    }

    ਸਮੂਹ=GET_GROUPS(u_Char_ਨਾਂ);
    ਸਮੂਹ_ਗਿਣਤੀ=*ਸਮੂਹ++;

    /* enumerate all groups */
    ਜਦੋਂ(ਸਮੂਹ_ਗਿਣਤੀ>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_ਨਾਂ) + u_Char_ਨਾਂ->ਸਮੂਹ_String_ਵਿੱਥ + GET_ਸਮੂਹ_ਵਿੱਥ(ਸਮੂਹ);
        s=expand_ਸਮੂਹ_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        ਲਈ(ਸਤਰ_ਸੰਖਿਆ=0; ਸਤਰ_ਸੰਖਿਆ<ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ; ++ਸਤਰ_ਸੰਖਿਆ) {
            ਸਤਰ=s+offsets[ਸਤਰ_ਸੰਖਿਆ];
            length=lengths[ਸਤਰ_ਸੰਖਿਆ];
            ਜੇ(length==0) {
                ਜਾਰੀ;
            }

            ਸਤਰ_ਹੱਦਬੰਦੀ=ਸਤਰ+length;

            /* read regular name */
            length=calc_Name_ਸੈੱਟ_ਲੰਮਾਈ(ਟੋਕਨ, token_ਗਿਣਤੀ, token_ਲੜੀਆਂ, tokenLengths, g_Name_ਸੈੱਟ, &ਸਤਰ, ਸਤਰ_ਹੱਦਬੰਦੀ);
            ਜੇ(length>max_Name_ਲੰਮਾਈ) {
                max_Name_ਲੰਮਾਈ=length;
            }
            ਜੇ(ਸਤਰ==ਸਤਰ_ਹੱਦਬੰਦੀ) {
                ਜਾਰੀ;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_ਸੈੱਟ_ਲੰਮਾਈ(ਟੋਕਨ, token_ਗਿਣਤੀ, token_ਲੜੀਆਂ, tokenLengths, g_Name_ਸੈੱਟ, &ਸਤਰ, ਸਤਰ_ਹੱਦਬੰਦੀ);
            ਜੇ(length>max_Name_ਲੰਮਾਈ) {
                max_Name_ਲੰਮਾਈ=length;
            }
            ਜੇ(ਸਤਰ==ਸਤਰ_ਹੱਦਬੰਦੀ) {
                ਜਾਰੀ;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        ਸਮੂਹ=ਅਗਲਾ_ਸਮੂਹ(ਸਮੂਹ);
        --ਸਮੂਹ_ਗਿਣਤੀ;
    }

    ਜੇ(tokenLengths!=ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_ਲੰਮਾਈ=max_Name_ਲੰਮਾਈ;
}

ਸਥਿਰ UBool
calcNameSetsLengths(UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_ਲੰਮਾਈ;

    ਜੇ(g_Max_Name_ਲੰਮਾਈ!=0) {
        ਵਾਪਸ ਸੱਚ;
    }

    ਜੇ(!is_ਅੰਕੜੇ_Loaded(p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਝੂਠ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    ਲਈ (i = 0; i < static_cast<int32_t>(ਆਕਾਰ(extChars)) - 1; ++i) {
        ਸੈੱਟ_ਜੋੜੋ_3(g_Name_ਸੈੱਟ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_ਲੰਮਾਈ=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_ਲੰਮਾਈ=calcExtNameSetsLengths(max_Name_ਲੰਮਾਈ);

    /* set sets and lengths from group names, set global maximum values */
    calc_ਸਮੂਹ_Name_Sets_Lengths(max_Name_ਲੰਮਾਈ);

    ਵਾਪਸ ਸੱਚ;
}

U_NAMESPACE_ਸਿਰਾ

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 ਕੋਡ, UCharNameChoice nameChoice,
           ਅੱਖਰ *ਬਫਰ, int32_t ਬਫਰ_ਲੰਮਾਈ,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ(nameChoice>=U_CHAR_NAME_CHOICE_ਗਿਣਤੀ ||
              ਬਫਰ_ਲੰਮਾਈ<0 || (ਬਫਰ_ਲੰਮਾਈ>0 && ਬਫਰ==ਖਾਲੀ_ਸੂਚਕ)
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ((uint32_t)ਕੋਡ>UCHAR_MAX_ਮੁੱਲ || !is_ਅੰਕੜੇ_Loaded(p_Error_ਕੋਡ)) {
        ਵਾਪਸ u_terminateChars(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, 0, p_Error_ਕੋਡ);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_ਨਾਂ+u_Char_ਨਾਂ->alg_ਨਾਂ_ਵਿੱਥ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ਜਦੋਂ(i>0) {
        ਜੇ(algRange->ਸ਼ੁਰੂ<=(uint32_t)ਕੋਡ && (uint32_t)ਕੋਡ<=algRange->ਸਿਰਾ) {
            length=getAlgName(algRange, (uint32_t)ਕੋਡ, nameChoice, ਬਫਰ, (uint16_t)ਬਫਰ_ਲੰਮਾਈ);
            ਰੋਕੋ;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    ਜੇ(i==0) {
        ਜੇ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_ਨਾਂ, (uint32_t )ਕੋਡ, U_EXTENDED_CHAR_NAME, ਬਫਰ, (uint16_t) ਬਫਰ_ਲੰਮਾਈ);
            ਜੇ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) ਕੋਡ, ਬਫਰ, (uint16_t) ਬਫਰ_ਲੰਮਾਈ);
            }
        } ਨਹੀਂਤਾਂ {
            /* normal character name */
            length=getName(u_Char_ਨਾਂ, (uint32_t)ਕੋਡ, nameChoice, ਬਫਰ, (uint16_t)ਬਫਰ_ਲੰਮਾਈ);
        }
    }

    ਵਾਪਸ u_terminateChars(ਬਫਰ, ਬਫਰ_ਲੰਮਾਈ, length, p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                ਅੱਖਰ *dest, int32_t dest_ਸਮਰੱਥਾ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* check the argument values */
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    } ਨਹੀਂਤਾਂ ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest_ਸਮਰੱਥਾ>0 && dest==ਖਾਲੀ_ਸੂਚਕ)) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਾਪਸ u_terminateChars(dest, dest_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ਅਚਲ ਅੱਖਰ *ਅਚਲ name,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅੱਖਰ ਹੇਠਲਾ[120] = {0};
    ਲੱਭੋ_Name ਲੱਭੋ_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    ਅੱਖਰ c0;
    ਸਥਿਰ constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ error;
    }

    ਜੇ(nameChoice>=U_CHAR_NAME_CHOICE_ਗਿਣਤੀ || name==ਖਾਲੀ_ਸੂਚਕ || *name==0) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ error;
    }

    ਜੇ(!is_ਅੰਕੜੇ_Loaded(p_Error_ਕੋਡ)) {
        ਵਾਪਸ error;
    }

    /* construct the lowercase of the name first */
    ਅਚਲ ਅੱਖਰ* nameChar = name;
    ਲਈ(i=0; i<ਆਕਾਰ(ਹੇਠਲਾ); ++i) {
        ਜੇ((c0=*nameChar++)!=0) {
            ਹੇਠਲਾ[i]=uprv_tolower(c0);
        } ਨਹੀਂਤਾਂ {
            ਰੋਕੋ;
        }
    }
    ਜੇ(i==ਆਕਾਰ(ਹੇਠਲਾ)) {
        /* name too long, there is no such character */
        *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    ਜੇ (ਹੇਠਲਾ[0] == '<') {
        ਜੇ (nameChoice == U_EXTENDED_CHAR_NAME && ਹੇਠਲਾ[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t ਹੱਦਬੰਦੀ = i;
            ਜਦੋਂ (i >= 3 && ਹੇਠਲਾ[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_ਲੰਮਾਈ = ਹੱਦਬੰਦੀ - (i + 1);
            ਜੇ (i >= 2 && ਹੇਠਲਾ[i] == '-' && 1 <= hex_ਲੰਮਾਈ && hex_ਲੰਮਾਈ <= 8) {
                uint32_t cIdx;

                ਹੇਠਲਾ[i] = 0;

                ਲਈ (++i; i < ਹੱਦਬੰਦੀ; ++i) {
                    ਜੇ (ਹੇਠਲਾ[i] >= '0' && ਹੇਠਲਾ[i] <= '9') {
                        cp = (cp << 4) + ਹੇਠਲਾ[i] - '0';
                    } ਨਹੀਂਤਾਂ ਜੇ (ਹੇਠਲਾ[i] >= 'a' && ਹੇਠਲਾ[i] <= 'f') {
                        cp = (cp << 4) + ਹੇਠਲਾ[i] - 'a' + 10;
                    } ਨਹੀਂਤਾਂ {
                        *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    ਜੇ (cp > UCHAR_MAX_ਮੁੱਲ) {
                        *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                        ਵਾਪਸ error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                ਲਈ (ਹੇਠਲਾ[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_ਨਾਂ); ++cIdx) {

                    ਜੇ (!uprv_strcmp(ਹੇਠਲਾ + 1, char_Cat_ਨਾਂ[cIdx])) {
                        ਜੇ (cat == cIdx) {
                            ਵਾਪਸ cp;
                        }
                        ਰੋਕੋ;
                    }
                }
            }
        }

        *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
        ਵਾਪਸ error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_ਨਾਂ+u_Char_ਨਾਂ->alg_ਨਾਂ_ਵਿੱਥ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ਜਦੋਂ(i>0) {
        ਜੇ((cp=ਲੱਭੋ_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            ਵਾਪਸ cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    ਲੱਭੋ_Name_2.otherName=&query;
    ਲੱਭੋ_Name_2.ਕੋਡ=error;
    enum_ਨਾਂ(u_Char_ਨਾਂ, 0, UCHAR_MAX_ਮੁੱਲ + 1, DO_ਲੱਭੋ_NAME, &ਲੱਭੋ_Name_2, nameChoice);
    ਜੇ (ਲੱਭੋ_Name_2.ਕੋਡ == error) {
         *p_Error_ਕੋਡ = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
    }
    ਵਾਪਸ ਲੱਭੋ_Name_2.ਕੋਡ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
u_enum_Char_ਨਾਂ(UChar32 ਸ਼ੁਰੂ, UChar32 ਹੱਦਬੰਦੀ,
                UEnum_Char_ਨਾਂ_Fn *fn,
                ਖਾਲੀ *context,
                UCharNameChoice nameChoice,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਜੇ(nameChoice>=U_CHAR_NAME_CHOICE_ਗਿਣਤੀ || fn==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    ਜੇ((uint32_t) ਹੱਦਬੰਦੀ > UCHAR_MAX_ਮੁੱਲ + 1) {
        ਹੱਦਬੰਦੀ = UCHAR_MAX_ਮੁੱਲ + 1;
    }
    ਜੇ((uint32_t)ਸ਼ੁਰੂ>=(uint32_t)ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ;
    }

    ਜੇ(!is_ਅੰਕੜੇ_Loaded(p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_ਨਾਂ+u_Char_ਨਾਂ->alg_ਨਾਂ_ਵਿੱਥ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ਜਦੋਂ(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        ਜੇ((uint32_t)ਸ਼ੁਰੂ<algRange->ਸ਼ੁਰੂ) {
            ਜੇ((uint32_t)ਹੱਦਬੰਦੀ<=algRange->ਸ਼ੁਰੂ) {
                enum_ਨਾਂ(u_Char_ਨਾਂ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, fn, context, nameChoice);
                ਵਾਪਸ;
            }
            ਜੇ(!enum_ਨਾਂ(u_Char_ਨਾਂ, ਸ਼ੁਰੂ, (UChar32)algRange->ਸ਼ੁਰੂ, fn, context, nameChoice)) {
                ਵਾਪਸ;
            }
            ਸ਼ੁਰੂ=(UChar32)algRange->ਸ਼ੁਰੂ;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        ਜੇ((uint32_t)ਸ਼ੁਰੂ<=algRange->ਸਿਰਾ) {
            ਜੇ((uint32_t)ਹੱਦਬੰਦੀ<=(algRange->ਸਿਰਾ+1)) {
                enum_Alg_ਨਾਂ(algRange, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, fn, context, nameChoice);
                ਵਾਪਸ;
            }
            ਜੇ(!enum_Alg_ਨਾਂ(algRange, ਸ਼ੁਰੂ, (UChar32)algRange->ਸਿਰਾ+1, fn, context, nameChoice)) {
                ਵਾਪਸ;
            }
            ਸ਼ੁਰੂ=(UChar32)algRange->ਸਿਰਾ+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_ਨਾਂ(u_Char_ਨਾਂ, ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_ਲੰਮਾਈ() {
    UError_ਕੋਡ error_ਕੋਡ=U_ZERO_ERROR;
    ਜੇ(calcNameSetsLengths(&error_ਕੋਡ)) {
        ਵਾਪਸ g_Max_Name_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ਸਥਿਰ ਖਾਲੀ
char_ਸੈੱਟ_ਮੰਜ਼ਿਲ_USet(uint32_t cset[8], ਅਚਲ USetAdder *sa) {
    char16_t us[256];
    ਅੱਖਰ cs[256];

    int32_t i, length;
    UError_ਕੋਡ error_ਕੋਡ;

    error_ਕੋਡ=U_ZERO_ERROR;

    ਜੇ(!calcNameSetsLengths(&error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    ਲਈ(i=0; i<256; ++i) {
        ਜੇ(ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(cset, i)) {
            cs[length++] = static_cast<ਅੱਖਰ>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_ਮੰਜ਼ਿਲ_UChars(cs, us, length);

    /* add each char16_t to the USet */
    ਲਈ(i=0; i<length; ++i) {
        ਜੇ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->ਜੋੜੋ(sa->ਸੈੱਟ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI ਖਾਲੀ U_EXPORT2
uprv_getCharNameCharacters(ਅਚਲ USetAdder *sa) {
    char_ਸੈੱਟ_ਮੰਜ਼ਿਲ_USet(g_Name_ਸੈੱਟ, sa);
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
ਸਥਿਰ ਖਾਲੀ
makeTokenMap(ਅਚਲ UDataSwapper *ds,
             int16_t ਟੋਕਨ[], uint16_t token_ਗਿਣਤੀ,
             uint8_t map[256],
             UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UBool ਵਰਤਿਆ_ਬਾਹਰਲਾ_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਜੇ(ds->ਵਿੱਚ_Charset==ds->ਬਾਹਰਲਾ_Charset) {
        /* Same charset family: identity permutation */
        ਲਈ(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } ਨਹੀਂਤਾਂ {
        uprv_memset(map, 0, 256);
        uprv_memset(ਵਰਤਿਆ_ਬਾਹਰਲਾ_Char, 0, 256);

        ਜੇ(token_ਗਿਣਤੀ>256) {
            token_ਗਿਣਤੀ=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        ਲਈ(i=1; i<token_ਗਿਣਤੀ; ++i) {
            ਜੇ(ਟੋਕਨ[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, &c1, 1, &c2, p_Error_ਕੋਡ);
                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->ਵਿੱਚ_Charset);
                    ਵਾਪਸ;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                ਵਰਤਿਆ_ਬਾਹਰਲਾ_Char[c2]=ਸੱਚ;
            }
        }

        /* set the mappings for the rest of the permutation */
        ਲਈ(i=j=1; i<token_ਗਿਣਤੀ; ++i) {
            /* set mappings that were not set for direct bytes */
            ਜੇ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                ਜਦੋਂ(ਵਰਤਿਆ_ਬਾਹਰਲਾ_Char[j]) {
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
uchar_ਅਦਲਾ_ਬਦਲੀ_ਨਾਂ(ਅਚਲ UDataSwapper *ds,
                ਅਚਲ ਖਾਲੀ *ਵਿੱਚ_ਅੰਕੜੇ, int32_t length, ਖਾਲੀ *ਬਾਹਰਲਾ_ਅੰਕੜੇ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ UDataInfo *pInfo;
    int32_t ਸਿਰਲੇਖ_ਮਾਪ;

    ਅਚਲ uint8_t *ਵਿੱਚ_ਬਾਈਟਾਂ;
    uint8_t *ਬਾਹਰਲਾ_ਬਾਈਟਾਂ;

    uint32_t token_String_ਵਿੱਥ, groups_ਵਿੱਥ, ਸਮੂਹ_String_ਵਿੱਥ, alg_ਨਾਂ_ਵਿੱਥ,
             ਵਿੱਥ, i, count, ਲੜੀਆਂ_ਗਿਣਤੀ;

    ਅਚਲ AlgorithmicRange *ਵਿੱਚ_Range;
    AlgorithmicRange *ਬਾਹਰਲਾ_Range;

    /* udata_swapDataHeader checks the arguments */
    ਸਿਰਲੇਖ_ਮਾਪ=udata_ਅਦਲਾ_ਬਦਲੀ_ਅੰਕੜੇ_ਸਿਰਲੇਖ(ds, ਵਿੱਚ_ਅੰਕੜੇ, length, ਬਾਹਰਲਾ_ਅੰਕੜੇ, p_Error_ਕੋਡ);
    ਜੇ(p_Error_ਕੋਡ==ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }

    /* check data format and format version */
    pInfo=(ਅਚਲ UDataInfo *)((ਅਚਲ ਅੱਖਰ *)ਵਿੱਚ_ਅੰਕੜੇ+4);
    ਜੇ(!(
        pInfo->ਅੰਕੜੇ_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ਅੰਕੜੇ_Format[1]==0x6e &&
        pInfo->ਅੰਕੜੇ_Format[2]==0x61 &&
        pInfo->ਅੰਕੜੇ_Format[3]==0x6d &&
        pInfo->format_ਸੰਸਕਰਣ[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->ਅੰਕੜੇ_Format[0], pInfo->ਅੰਕੜੇ_Format[1],
                         pInfo->ਅੰਕੜੇ_Format[2], pInfo->ਅੰਕੜੇ_Format[3],
                         pInfo->format_ਸੰਸਕਰਣ[0]);
        *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
        ਵਾਪਸ 0;
    }

    ਵਿੱਚ_ਬਾਈਟਾਂ=(ਅਚਲ uint8_t *)ਵਿੱਚ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ=(ਬਾਹਰਲਾ_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) ? ਖਾਲੀ_ਸੂਚਕ : (uint8_t *)ਬਾਹਰਲਾ_ਅੰਕੜੇ+ਸਿਰਲੇਖ_ਮਾਪ;
    ਜੇ(length<0) {
        alg_ਨਾਂ_ਵਿੱਥ=ds->readUInt32(((ਅਚਲ uint32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ)[3]);
    } ਨਹੀਂਤਾਂ {
        length-=ਸਿਰਲੇਖ_ਮਾਪ;
        ਜੇ( length<20 ||
            (uint32_t)length<(alg_ਨਾਂ_ਵਿੱਥ=ds->readUInt32(((ਅਚਲ uint32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
            ਵਾਪਸ 0;
        }
    }

    ਜੇ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        ਵਿੱਥ=alg_ਨਾਂ_ਵਿੱਥ;
        count=ds->readUInt32(*((ਅਚਲ uint32_t *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ)));
        ਵਿੱਥ+=4;

        ਲਈ(i=0; i<count; ++i) {
            ਵਿੱਚ_Range=(ਅਚਲ AlgorithmicRange *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ);
            ਵਿੱਥ+=ds->readUInt16(ਵਿੱਚ_Range->size);
        }
    } ਨਹੀਂਤਾਂ {
        /* swap data */
        ਅਚਲ uint16_t *p;
        uint16_t *q, *ਆਰਜ਼ੀ;

        int16_t ਟੋਕਨ[512];
        uint16_t token_ਗਿਣਤੀ;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        ਜੇ(ਵਿੱਚ_ਬਾਈਟਾਂ!=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ) {
            uprv_memcpy(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, ਵਿੱਚ_ਬਾਈਟਾਂ, length);
        }

        /* the initial 4 offsets first */
        token_String_ਵਿੱਥ=ds->readUInt32(((ਅਚਲ uint32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ)[0]);
        groups_ਵਿੱਥ=ds->readUInt32(((ਅਚਲ uint32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ)[1]);
        ਸਮੂਹ_String_ਵਿੱਥ=ds->readUInt32(((ਅਚਲ uint32_t *)ਵਿੱਚ_ਬਾਈਟਾਂ)[2]);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ, 16, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ, p_Error_ਕੋਡ);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ਅਚਲ uint16_t *)(ਵਿੱਚ_ਬਾਈਟਾਂ+16);
        q=(uint16_t *)(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+16);

        /* read and swap the tokenCount */
        token_ਗਿਣਤੀ=ds->readUInt16(*p);
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p, 2, q, p_Error_ਕੋਡ);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        ਜੇ(token_ਗਿਣਤੀ<=512) {
            count=token_ਗਿਣਤੀ;
        } ਨਹੀਂਤਾਂ {
            count=512;
        }
        ਲਈ(i=0; i<count; ++i) {
            ਟੋਕਨ[i]=udata_readInt16(ds, p[i]);
        }
        ਲਈ(; i<512; ++i) {
            ਟੋਕਨ[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, ਟੋਕਨ, token_ਗਿਣਤੀ, map, p_Error_ਕੋਡ);
        makeTokenMap(ds, ਟੋਕਨ+256, (uint16_t)(token_ਗਿਣਤੀ>256 ? token_ਗਿਣਤੀ-256 : 0), trailMap, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        ਆਰਜ਼ੀ=(uint16_t *)uprv_malloc(token_ਗਿਣਤੀ*2);
        ਜੇ(ਆਰਜ਼ੀ==ਖਾਲੀ_ਸੂਚਕ) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_ਗਿਣਤੀ);
            *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
            ਵਾਪਸ 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        ਲਈ(i=0; i<token_ਗਿਣਤੀ && i<256; ++i) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p+i, 2, ਆਰਜ਼ੀ+map[i], p_Error_ਕੋਡ);
        }

        /* swap and permutate trail-byte tokens */
        ਲਈ(; i<token_ਗਿਣਤੀ; ++i) {
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p+i, 2, ਆਰਜ਼ੀ+(i&0xffffff00)+trailMap[i&0xff], p_Error_ਕੋਡ);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, ਆਰਜ਼ੀ, token_ਗਿਣਤੀ*2);
        uprv_free(ਆਰਜ਼ੀ);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_ਅਦਲਾ_ਬਦਲੀ_Inv_String_ਖੰਡ(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+token_String_ਵਿੱਥ, (int32_t)(groups_ਵਿੱਥ-token_String_ਵਿੱਥ),
                                    ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+token_String_ਵਿੱਥ, p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            ਵਾਪਸ 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ਅਚਲ uint16_t *)(ਵਿੱਚ_ਬਾਈਟਾਂ+groups_ਵਿੱਥ)));
        ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+groups_ਵਿੱਥ, (int32_t)((1+count*3)*2),
                           ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+groups_ਵਿੱਥ, p_Error_ਕੋਡ);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        ਜੇ(ds->ਵਿੱਚ_Charset!=ds->ਬਾਹਰਲਾ_Charset) {
            uint16_t offsets[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+1], lengths[ਸਤਰਾਂ_ਪ੍ਰਤੀ_ਸਮੂਹ+1];

            ਅਚਲ uint8_t *ਵਿੱਚ_ਲੜੀਆਂ, *ਅਗਲਾ_ਵਿੱਚ_ਲੜੀਆਂ;
            uint8_t *ਬਾਹਰਲਾ_ਲੜੀਆਂ;

            uint8_t c;

            ਵਿੱਚ_ਲੜੀਆਂ=ਵਿੱਚ_ਬਾਈਟਾਂ+ਸਮੂਹ_String_ਵਿੱਥ;
            ਬਾਹਰਲਾ_ਲੜੀਆਂ=ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਸਮੂਹ_String_ਵਿੱਥ;

            ਲੜੀਆਂ_ਗਿਣਤੀ=alg_ਨਾਂ_ਵਿੱਥ-ਸਮੂਹ_String_ਵਿੱਥ;

            /* iterate through string groups until only a few padding bytes are left */
            ਜਦੋਂ(ਲੜੀਆਂ_ਗਿਣਤੀ>32) {
                ਅਗਲਾ_ਵਿੱਚ_ਲੜੀਆਂ=expand_ਸਮੂਹ_Lengths(ਵਿੱਚ_ਲੜੀਆਂ, offsets, lengths);

                /* move past the length bytes */
                ਲੜੀਆਂ_ਗਿਣਤੀ-=(uint32_t)(ਅਗਲਾ_ਵਿੱਚ_ਲੜੀਆਂ-ਵਿੱਚ_ਲੜੀਆਂ);
                ਬਾਹਰਲਾ_ਲੜੀਆਂ+=ਅਗਲਾ_ਵਿੱਚ_ਲੜੀਆਂ-ਵਿੱਚ_ਲੜੀਆਂ;
                ਵਿੱਚ_ਲੜੀਆਂ=ਅਗਲਾ_ਵਿੱਚ_ਲੜੀਆਂ;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                ਲੜੀਆਂ_ਗਿਣਤੀ-=count;

                /* swap the string bytes using map[] and trailMap[] */
                ਜਦੋਂ(count>0) {
                    c=*ਵਿੱਚ_ਲੜੀਆਂ++;
                    *ਬਾਹਰਲਾ_ਲੜੀਆਂ++=map[c];
                    ਜੇ(ਟੋਕਨ[c]!=-2) {
                        --count;
                    } ਨਹੀਂਤਾਂ {
                        /* token lead byte: swap the trail byte, too */
                        *ਬਾਹਰਲਾ_ਲੜੀਆਂ++=trailMap[*ਵਿੱਚ_ਲੜੀਆਂ++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        ਵਿੱਥ=alg_ਨਾਂ_ਵਿੱਥ;
        count=ds->readUInt32(*((ਅਚਲ uint32_t *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ)));
        ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ, 4, ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ, p_Error_ਕੋਡ);
        ਵਿੱਥ+=4;

        ਲਈ(i=0; i<count; ++i) {
            ਜੇ(ਵਿੱਥ>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_ਕੋਡ=U_ਸੂਚਕਾਂਕ_OUTOFBOUNDS_ERROR;
                ਵਾਪਸ 0;
            }

            ਵਿੱਚ_Range=(ਅਚਲ AlgorithmicRange *)(ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ);
            ਬਾਹਰਲਾ_Range=(AlgorithmicRange *)(ਬਾਹਰਲਾ_ਬਾਈਟਾਂ+ਵਿੱਥ);
            ਵਿੱਥ+=ds->readUInt16(ਵਿੱਚ_Range->size);

            ds->ਅਦਲਾ_ਬਦਲੀ_Array32(ds, ਵਿੱਚ_Range, 8, ਬਾਹਰਲਾ_Range, p_Error_ਕੋਡ);
            ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, &ਵਿੱਚ_Range->size, 2, &ਬਾਹਰਲਾ_Range->size, p_Error_ਕੋਡ);
            ਚੋਣ(ਵਿੱਚ_Range->type) {
            ਹਾਲਤ 0:
                /* swap prefix string */
                ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, ਵਿੱਚ_Range+1, (int32_t)uprv_strlen((ਅਚਲ ਅੱਖਰ *)(ਵਿੱਚ_Range+1)),
                                    ਬਾਹਰਲਾ_Range+1, p_Error_ਕੋਡ);
                ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    ਵਾਪਸ 0;
                }
                ਰੋਕੋ;
            ਹਾਲਤ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_ਗਿਣਤੀ;

                    factors_ਗਿਣਤੀ=ਵਿੱਚ_Range->variant;
                    p=(ਅਚਲ uint16_t *)(ਵਿੱਚ_Range+1);
                    q=(uint16_t *)(ਬਾਹਰਲਾ_Range+1);
                    ds->ਅਦਲਾ_ਬਦਲੀ_Array16(ds, p, (int32_t)(factors_ਗਿਣਤੀ*2), q, p_Error_ਕੋਡ);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_ਗਿਣਤੀ;
                    q+=factors_ਗਿਣਤੀ;
                    ਲੜੀਆਂ_ਗਿਣਤੀ=(uint32_t)((ਵਿੱਚ_ਬਾਈਟਾਂ+ਵਿੱਥ)-(ਅਚਲ uint8_t *)p);
                    ਜਦੋਂ(ਲੜੀਆਂ_ਗਿਣਤੀ>0 && ((ਅਚਲ uint8_t *)p)[ਲੜੀਆਂ_ਗਿਣਤੀ-1]!=0) {
                        --ਲੜੀਆਂ_ਗਿਣਤੀ;
                    }
                    ds->ਅਦਲਾ_ਬਦਲੀ_Inv_Chars(ds, p, (int32_t)ਲੜੀਆਂ_ਗਿਣਤੀ, q, p_Error_ਕੋਡ);
                }
                ਰੋਕੋ;
            ਮੂਲ:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 ਵਿੱਚ_Range->type, i);
                *p_Error_ਕੋਡ=U_UNSUPPORTED_ERROR;
                ਵਾਪਸ 0;
            }
        }
    }

    ਵਾਪਸ ਸਿਰਲੇਖ_ਮਾਪ+(int32_t)ਵਿੱਥ;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
