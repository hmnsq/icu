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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DATA_NAME[] = "unames";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define ಸಾಲುಗಳು_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (ಸಾಲುಗಳು_PER_GROUP-1)

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
ಗಣನಾ {
    GROUP_MSB,
    GROUP_OFFSET_ಮೇಲಿನ,
    GROUP_OFFSET_ಕೆಳಗಿನ,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_ಮೇಲಿನ]<<16|(group)[GROUP_OFFSET_ಕೆಳಗಿನ])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

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
        Matcher(ಧ್ರುವ CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        ತಾರ್ಕಿಕ consistentWith(ವರ್ಣ c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            ಯದಿ (skeletonIterator == query.skeleton.end()) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ಯದಿ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                ಯದಿ (lastChar == '-') {
                    ಯದಿ (*skeletonIterator++ != lastChar) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                }
                lastChar = c;
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            } ಅನ್ಯಥಾ ಯದಿ (c == '-') {
                ಯದಿ (lastChar == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.end() - 2)) {
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
                    ಯದಿ (*skeletonIterator++ != c) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            } ಅನ್ಯಥಾ {
                ಯದಿ (*skeletonIterator++ != c) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
                lastChar = c;
                ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
            }
        }

        ತಾರ್ಕಿಕ consistentWith(ಧ್ರುವ std::string_view substring) {
            ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ c : substring) {
                ಯದಿ (!consistentWith(c)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }

        ತಾರ್ಕಿಕ matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            ಪ್ರತ್ಯಾಗಮ skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() ಧ್ರುವ {
            ಪ್ರತ್ಯಾಗಮ query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      ಗುಪ್ತ:
        ಧ್ರುವ CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        ವರ್ಣ lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        ವರ್ಣ *skeletonLimit = skeletonData;
        ಪ್ರತಿ (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + ಪರಿಮಾಣ(skeletonData));
            ಯದಿ (skeletonLimit >= skeletonData + ಪರಿಮಾಣ(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                ಭಙ್ಗ;
            }
            ಯದಿ (query[i] == ' ' || query[i] == '_') {
                ಅನುವರ್ತ;
            }
            ಯದಿ (query[i] == '-') {
                ತಾರ್ಕಿಕ isMedial;
                ತಾರ್ಕಿಕ is1180MedialHyphen = ಅಸತ್ಯ;
                ಯದಿ (i == 0 || i == query.length() - 1) {
                    isMedial = ಅಸತ್ಯ;
                } ಅನ್ಯಥಾ {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                ಯದಿ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = ಸತ್ಯ;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    ಪ್ರತಿ (std::size_t j = i + 2; j < query.length(); ++j) {
                        ಯದಿ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = ಅಸತ್ಯ;
                        }
                    }
                }
                ಯದಿ (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } ಅನ್ಯಥಾ {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ಧ್ರುವ {
        ಪ್ರತ್ಯಾಗಮ Matcher(*ಏತತ್);
    }

ಗುಪ್ತ:
    ಸ್ಥಿರ ತಾರ್ಕಿಕ isASCIILetterOrDigit(ಧ್ರುವ ವರ್ಣ c) {
        ಪ್ರತ್ಯಾಗಮ uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    ವರ್ಣ skeletonData[120];
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
#define GET_GROUPS(names) (ಧ್ರುವ uint16_t *)((ಧ್ರುವ ವರ್ಣ *)names+names->groupsOffset)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

ಸ್ಥಿರ UDataMemory *uCharNamesData=nullptr;
ಸ್ಥಿರ UCharNames *uCharNames=nullptr;
ಸ್ಥಿರ icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ಸ್ಥಿರ int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ಸ್ಥಿರ uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_ಎಣಿಕೆ
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_ಎಣಿಕೆ + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_ಎಣಿಕೆ + 2

#define U_CHAR_EXTENDED_CATEGORY_ಎಣಿಕೆ (U_CHAR_CATEGORY_ಎಣಿಕೆ + 3)

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ charCatNames[U_CHAR_EXTENDED_CATEGORY_ಎಣಿಕೆ] = {
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
    ಯದಿ(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    ಯದಿ(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ UBool U_CALLCONV
isAcceptable(ಶೂನ್ಯ * /*context*/,
             ಧ್ರುವ ವರ್ಣ * /*type*/, ಧ್ರುವ ವರ್ಣ * /*name*/,
             ಧ್ರುವ UDataInfo *pInfo) {
    ಪ್ರತ್ಯಾಗಮ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    ಯದಿ(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } ಅನ್ಯಥಾ {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ಸ್ಥಿರ UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    ಯದಿ((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_ಎಣಿಕೆ

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
expandName(UCharNames *names,
           ಧ್ರುವ uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           ವರ್ಣ *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_ಎಣಿಕೆ=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    ಯದಿ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ಯದಿ (static_cast<uint8_t>(';') >= token_ಎಣಿಕೆ || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ಪೂರ್ಣಾಙ್ಕ fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ಕುರು {
                ಯಾವತ್(nameLength>0) {
                    --nameLength;
                    ಯದಿ(*name++==';') {
                        ಭಙ್ಗ;
                    }
                }
            } ಯಾವತ್(--fieldIndex>0);
        } ಅನ್ಯಥಾ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    ಯಾವತ್(nameLength>0) {
        --nameLength;
        c=*name++;

        ಯದಿ(c>=token_ಎಣಿಕೆ) {
            ಯದಿ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } ಅನ್ಯಥಾ {
                /* finished */
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ {
            token=tokens[c];
            ಯದಿ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            ಯದಿ (token == static_cast<uint16_t>(-1)) {
                ಯದಿ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } ಅನ್ಯಥಾ {
                    /* finished */
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                ಯಾವತ್((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    ಯದಿ(bufferLength>0) {
        *buffer=0;
    }

    ಪ್ರತ್ಯಾಗಮ bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ಸ್ಥಿರ UBool
compareName(UCharNames *names,
            ಧ್ರುವ uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            ಧ್ರುವ CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_ಎಣಿಕೆ=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    ಯದಿ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ಯದಿ (static_cast<uint8_t>(';') >= token_ಎಣಿಕೆ || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            ಪೂರ್ಣಾಙ್ಕ fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ಕುರು {
                ಯಾವತ್(nameLength>0) {
                    --nameLength;
                    ಯದಿ(*name++==';') {
                        ಭಙ್ಗ;
                    }
                }
            } ಯಾವತ್(--fieldIndex>0);
        } ಅನ್ಯಥಾ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    ಯಾವತ್(nameLength>0) {
        --nameLength;
        c=*name++;

        ಯದಿ(c>=token_ಎಣಿಕೆ) {
            ಯದಿ(c!=';') {
                /* implicit letter */
                ಯದಿ (!matcher.consistentWith(c)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            } ಅನ್ಯಥಾ {
                /* finished */
                ಭಙ್ಗ;
            }
        } ಅನ್ಯಥಾ {
            token=tokens[c];
            ಯದಿ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            ಯದಿ (token == static_cast<uint16_t>(-1)) {
                ಯದಿ(c!=';') {
                    /* explicit letter */
                    ಯದಿ (!matcher.consistentWith(c)) {
                        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                    }
                } ಅನ್ಯಥಾ {
                    /* finished */
                    ಭಙ್ಗ;
                }
            } ಅನ್ಯಥಾ {
                /* write token word */
                ಯದಿ (!matcher.consistentWith(reinterpret_cast<ಧ್ರುವ ವರ್ಣ *>(tokenStrings + token))) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
        }
    }

    /* complete match? */
    ಪ್ರತ್ಯಾಗಮ matcher.matches();
}

ಸ್ಥಿರ uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    ಯದಿ (U_IS_UNICODE_NONCHAR(cp)) {
        ಪ್ರತ್ಯಾಗಮ U_NONCHARACTER_CODE_POINT;
    }

    ಯದಿ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    ಪ್ರತ್ಯಾಗಮ cat;
}

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    ಯದಿ (cat >= UPRV_LENGTHOF(charCatNames)) {
        ಪ್ರತ್ಯಾಗಮ "unknown";
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ charCatNames[cat];
    }
}

ಸ್ಥಿರ uint16_t getExtName(uint32_t code, ವರ್ಣ *buffer, uint16_t bufferLength) {
    ಧ್ರುವ ವರ್ಣ *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    ಪೂರ್ಣಾಙ್ಕ ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    ಯಾವತ್ (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    ಪ್ರತಿ (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    ಯದಿ (ndigits < 4)
        ndigits = 4;
    ಪ್ರತಿ (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    ಪ್ರತ್ಯಾಗಮ length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ಸ್ಥಿರ ಧ್ರುವ uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    ಧ್ರುವ uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    ಯಾವತ್(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        ಯದಿ(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } ಅನ್ಯಥಾ {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    ಪ್ರತ್ಯಾಗಮ groups+start*GROUP_LENGTH;
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
ಸ್ಥಿರ ಧ್ರುವ uint8_t *
expandGroupLengths(ಧ್ರುವ uint8_t *s,
                   uint16_t offsets[ಸಾಲುಗಳು_PER_GROUP+1], uint16_t lengths[ಸಾಲುಗಳು_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    ಯಾವತ್(i<ಸಾಲುಗಳು_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        ಯದಿ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } ಅನ್ಯಥಾ ಯದಿ((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } ಅನ್ಯಥಾ {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        ಯದಿ((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            ಯದಿ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } ಅನ್ಯಥಾ {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    ಪ್ರತ್ಯಾಗಮ s;
}

ಸ್ಥಿರ uint16_t
expandGroupName(UCharNames *names, ಧ್ರುವ uint16_t *group,
                uint16_t ಸಾಲು_ಸಂಖ್ಯೆ, UCharNameChoice nameChoice,
                ವರ್ಣ *buffer, uint16_t bufferLength) {
    uint16_t offsets[ಸಾಲುಗಳು_PER_GROUP+2], lengths[ಸಾಲುಗಳು_PER_GROUP+2];
    ಧ್ರುವ uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    ಪ್ರತ್ಯಾಗಮ expandName(names, s+offsets[ಸಾಲು_ಸಂಖ್ಯೆ], lengths[ಸಾಲು_ಸಂಖ್ಯೆ], nameChoice,
                      buffer, bufferLength);
}

ಸ್ಥಿರ uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        ವರ್ಣ *buffer, uint16_t bufferLength) {
    ಧ್ರುವ uint16_t *group=getGroup(names, code);
    ಯದಿ (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        ಪ್ರತ್ಯಾಗಮ expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } ಅನ್ಯಥಾ {
        /* group not found */
        /* zero-terminate */
        ಯದಿ(bufferLength>0) {
            *buffer=0;
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ಸ್ಥಿರ UBool
enumGroupNames(UCharNames *names, ಧ್ರುವ uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, ಶೂನ್ಯ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[ಸಾಲುಗಳು_PER_GROUP+2], lengths[ಸಾಲುಗಳು_PER_GROUP+2];
    ಧ್ರುವ uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    ಯದಿ(fn!=DO_FIND_NAME) {
        ವರ್ಣ buffer[200];
        uint16_t length;

        ಯಾವತ್(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, ಪರಿಮಾಣ(buffer));
            ಯದಿ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, ಪರಿಮಾಣ(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            ಯದಿ(length>0) {
                ಯದಿ(!fn(context, start, nameChoice, buffer, length)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            ++start;
        }
    } ಅನ್ಯಥಾ {
        ಧ್ರುವ CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        ಯಾವತ್(start<=end) {
            ಯದಿ(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            ++start;
        }
    }
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ಸ್ಥಿರ UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, ಶೂನ್ಯ *context)
{
    ಯದಿ(fn!=DO_FIND_NAME) {
        ವರ್ಣ buffer[200];
        uint16_t length;
        
        ಯಾವತ್(start<=end) {
            buffer[length = getExtName(start, buffer, ಪರಿಮಾಣ(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            ಯದಿ(length>0) {
                ಯದಿ(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            ++start;
        }
    }

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, ಶೂನ್ಯ *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_ಎಣಿಕೆ;
    ಧ್ರುವ uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    ಯದಿ(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        ಯದಿ(extLimit>limit) {
            extLimit=limit;
        }
        ಯದಿ(!enumExtNames(start, extLimit-1, fn, context)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }
        start=extLimit;
    }

    ಯದಿ(startGroupMSB==endGroupMSB) {
        ಯದಿ(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            ಪ್ರತ್ಯಾಗಮ enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } ಅನ್ಯಥಾ {
        ಧ್ರುವ uint16_t *groups=GET_GROUPS(names);
        group_ಎಣಿಕೆ=*groups++;
        groupLimit=groups+group_ಎಣಿಕೆ*GROUP_LENGTH;

        ಯದಿ(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            ಯದಿ((start&GROUP_MASK)!=0) {
                ಯದಿ(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + ಸಾಲುಗಳು_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } ಅನ್ಯಥಾ ಯದಿ(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ಧ್ರುವ uint16_t *nextGroup=NEXT_GROUP(group);
            ಯದಿ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                ಯದಿ (end > limit) {
                    end = limit;
                }
                ಯದಿ (!enumExtNames(start, end - 1, fn, context)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        ಯಾವತ್(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            ಧ್ರುವ uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            ಯದಿ(!enumGroupNames(names, group, start, start+ಸಾಲುಗಳು_PER_GROUP-1, fn, context, nameChoice)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
            nextGroup=NEXT_GROUP(group);
            ಯದಿ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                ಯದಿ (end > limit) {
                    end = limit;
                }
                ಯದಿ (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        ಯದಿ(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            ಪ್ರತ್ಯಾಗಮ enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } ಅನ್ಯಥಾ ಯದಿ (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            ಯದಿ (next > start) {
                start = next;
            }
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    ಯದಿ (nameChoice == U_EXTENDED_CHAR_NAME) {
        ಯದಿ (limit > UCHAR_MAX_ಮೌಲ್ಯ + 1) {
            limit = UCHAR_MAX_ಮೌಲ್ಯ + 1;
        }
        ಪ್ರತ್ಯಾಗಮ enumExtNames(start, limit - 1, fn, context);
    }
    
    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ಸ್ಥಿರ uint16_t
writeFactorSuffix(ಧ್ರುವ uint16_t *factors, uint16_t count,
                  ಧ್ರುವ ವರ್ಣ *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  ಧ್ರುವ ವರ್ಣ *elementBases[8], ಧ್ರುವ ವರ್ಣ *elements[8],
                  ವರ್ಣ *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    ವರ್ಣ c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    ಪ್ರತಿ(i=count; i>0; --i) {
        factor=factors[i];
        indexes[i] = static_cast<uint16_t>(code % factor);
        code/=factor;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(code);

    /* write each element */
    ಪ್ರತಿ(;;) {
        ಯದಿ(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        ಯಾವತ್(factor>0) {
            ಯಾವತ್(*s++!=0) {}
            --factor;
        }
        ಯದಿ(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        ಯಾವತ್((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        ಯದಿ(i>=count) {
            ಭಙ್ಗ;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        ಯಾವತ್(factor>0) {
            ಯಾವತ್(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    ಯದಿ(bufferLength>0) {
        *buffer=0;
    }

    ಪ್ರತ್ಯಾಗಮ bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ಸ್ಥಿರ uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        ವರ್ಣ *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    ಯದಿ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        ಯದಿ(bufferLength>0) {
            *buffer=0;
        }
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ವಿಕಲ್ಪ(range->type) {
    ಪಕ್ಷ 0: {
        /* name = prefix hex-digits */
        ಧ್ರುವ ವರ್ಣ* s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(range + 1);
        ವರ್ಣ c;

        uint16_t i, count;

        /* copy prefix */
        ಯಾವತ್((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        ಯದಿ(count<bufferLength) {
            buffer[count]=0;
        }

        ಪ್ರತಿ(i=count; i>0;) {
            ಯದಿ(--i<bufferLength) {
                c = static_cast<ವರ್ಣ>(code & 0xf);
                ಯದಿ(c<10) {
                    c+='0';
                } ಅನ್ಯಥಾ {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ಧ್ರುವ uint16_t* factors = reinterpret_cast<ಧ್ರುವ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಧ್ರುವ ವರ್ಣ* s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(factors + count);
        ವರ್ಣ c;

        /* copy prefix */
        ಯಾವತ್((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        /* undefined type */
        /* zero-terminate */
        ಯದಿ(bufferLength>0) {
            *buffer=0;
        }
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ಸ್ಥಿರ UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, ಶೂನ್ಯ *context,
             UCharNameChoice nameChoice) {
    ವರ್ಣ buffer[200];
    uint16_t length;

    ಯದಿ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ವಿಕಲ್ಪ(range->type) {
    ಪಕ್ಷ 0: {
        ವರ್ಣ *s, *end;
        ವರ್ಣ c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, ಪರಿಮಾಣ(buffer));
        ಯದಿ(length<=0) {
            ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
        }

        /* call the enumerator function with this first character */
        ಯದಿ(!fn(context, start, nameChoice, buffer, length)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        ಯಾವತ್(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        ಯಾವತ್(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            ಪ್ರತಿ (;;) {
                c=*--s;
                ಯದಿ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<ವರ್ಣ>(c + 1);
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ ಯದಿ(c=='9') {
                    *s='A';
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ ಯದಿ(c=='F') {
                    *s='0';
                }
            }

            ಯದಿ(!fn(context, start, nameChoice, buffer, length)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ 1: {
        uint16_t indexes[8];
        ಧ್ರುವ ವರ್ಣ *elementBases[8], *elements[8];
        ಧ್ರುವ uint16_t* factors = reinterpret_cast<ಧ್ರುವ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಧ್ರುವ ವರ್ಣ* s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(factors + count);
        ವರ್ಣ *suffix, *t;
        uint16_t prefixLength, i, idx;

        ವರ್ಣ c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        ಯಾವತ್((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(ಪರಿಮಾಣ(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        ಯದಿ(!fn(context, start, nameChoice, buffer, length)) {
            ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
        }

        /* enumerate the rest of the names */
        ಯಾವತ್(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ಪ್ರತಿ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ಯದಿ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ಯಾವತ್(*s++!=0) {
                    }
                    elements[i]=s;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            ಪ್ರತಿ(i=0; i<count; ++i) {
                s=elements[i];
                ಯಾವತ್((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            ಯದಿ(!fn(context, start, nameChoice, buffer, length)) {
                ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
            }
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        /* undefined type */
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ಸ್ಥಿರ UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    ಯದಿ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        ಪ್ರತ್ಯಾಗಮ 0xffff;
    }

    ವಿಕಲ್ಪ(range->type) {
    ಪಕ್ಷ 0: {
        /* name = prefix hex-digits */
        ಧ್ರುವ ವರ್ಣ* s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(range + 1);

        /* compare prefix */
        ಯದಿ (!matcher.consistentWith(s)) {
            ಪ್ರತ್ಯಾಗಮ 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        ಪ್ರತಿ(ವರ್ಣ c : matcher.remainingSignificantCharacters()) {
            ಯದಿ('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } ಅನ್ಯಥಾ ಯದಿ('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ 0xffff;
            }
        }

        /* does it fit into the range? */
        ಯದಿ (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            ಪ್ರತ್ಯಾಗಮ code;
        }
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ 1: {
        ವರ್ಣ buffer[64];
        uint16_t indexes[8];
        ಧ್ರುವ ವರ್ಣ *elementBases[8], *elements[8];
        ಧ್ರುವ uint16_t* factors = reinterpret_cast<ಧ್ರುವ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ಧ್ರುವ ವರ್ಣ *s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        ಯದಿ (!matcher.consistentWith(prefix)) {
            ಪ್ರತ್ಯಾಗಮ 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, ಪರಿಮಾಣ(buffer));

        /* compare the first suffix */
        ಯದಿ(querySuffix == buffer) {
            ಪ್ರತ್ಯಾಗಮ start;
        }

        /* enumerate and compare the rest of the suffixes */
        ಯಾವತ್(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            ಪ್ರತಿ (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ಯದಿ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ಯಾವತ್(*s++!=0) {}
                    elements[i]=s;
                    ಭಙ್ಗ;
                } ಅನ್ಯಥಾ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            ಪ್ರತಿ(i=0; i<count; ++i) {
                s=elements[i];
                ವರ್ಣ c;
                ಯಾವತ್((c=*s++)!=0) {
                    ಯದಿ(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            ಯದಿ(i<99 && it == querySuffix.end()) {
                ಪ್ರತ್ಯಾಗಮ start;
            }
        }
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        /* undefined type */
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ಸ್ಥಿರ int32_t
calcStringSetLength(uint32_t set[8], ಧ್ರುವ ವರ್ಣ *s) {
    int32_t length=0;
    ವರ್ಣ c;

    ಯಾವತ್((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_ಎಣಿಕೆ;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_ಎಣಿಕೆ=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    ಯಾವತ್(range_ಎಣಿಕೆ>0) {
        ವಿಕಲ್ಪ(range->type) {
        ಪಕ್ಷ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(range + 1)) + range->variant;
            ಯದಿ(length>maxNameLength) {
                maxNameLength=length;
            }
            ಭಙ್ಗ;
        ಪಕ್ಷ 1: {
            /* name = prefix factorized-elements */
            ಧ್ರುವ uint16_t* factors = reinterpret_cast<ಧ್ರುವ uint16_t*>(range + 1);
            ಧ್ರುವ ವರ್ಣ *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            ಪ್ರತಿ(i=0; i<count; ++i) {
                maxFactorLength=0;
                ಪ್ರತಿ(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    ಯದಿ(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            ಯದಿ(length>maxNameLength) {
                maxNameLength=length;
            }
            ಭಙ್ಗ;
        }
        ಸ್ವಭಾವ:
            /* unknown type */
            ಭಙ್ಗ;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_ಎಣಿಕೆ;
    }
    ಪ್ರತ್ಯಾಗಮ maxNameLength;
}

ಸ್ಥಿರ int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    ಪ್ರತಿ(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        ಯದಿ(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    ಪ್ರತ್ಯಾಗಮ maxNameLength;
}

ಸ್ಥಿರ int32_t
calcNameSetLength(ಧ್ರುವ uint16_t *tokens, uint16_t token_ಎಣಿಕೆ, ಧ್ರುವ uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  ಧ್ರುವ uint8_t **p_ಸಾಲು, ಧ್ರುವ uint8_t *ಸಾಲು_Limit) {
    ಧ್ರುವ uint8_t *ಸಾಲು=*p_ಸಾಲು;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    ಯಾವತ್ (ಸಾಲು != ಸಾಲು_Limit && (c = *ಸಾಲು++) != static_cast<uint8_t>(';')) {
        ಯದಿ(c>=token_ಎಣಿಕೆ) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } ಅನ್ಯಥಾ {
            token=tokens[c];
            ಯದಿ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*ಸಾಲು++;
                token=tokens[c];
            }
            ಯದಿ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } ಅನ್ಯಥಾ {
                /* count token word */
                ಯದಿ(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    ಯದಿ(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } ಅನ್ಯಥಾ {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<ಧ್ರುವ ವರ್ಣ*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_ಸಾಲು=ಸಾಲು;
    ಪ್ರತ್ಯಾಗಮ length;
}

ಸ್ಥಿರ ಶೂನ್ಯ
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[ಸಾಲುಗಳು_PER_GROUP+2], lengths[ಸಾಲುಗಳು_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_ಎಣಿಕೆ=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    ಧ್ರುವ uint16_t *group;
    ಧ್ರುವ uint8_t *s, *ಸಾಲು, *ಸಾಲು_Limit;

    int32_t group_ಎಣಿಕೆ, ಸಾಲು_ಸಂಖ್ಯೆ, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_ಎಣಿಕೆ));
    ಯದಿ(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_ಎಣಿಕೆ);
    }

    group=GET_GROUPS(uCharNames);
    group_ಎಣಿಕೆ=*group++;

    /* enumerate all groups */
    ಯಾವತ್(group_ಎಣಿಕೆ>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        ಪ್ರತಿ(ಸಾಲು_ಸಂಖ್ಯೆ=0; ಸಾಲು_ಸಂಖ್ಯೆ<ಸಾಲುಗಳು_PER_GROUP; ++ಸಾಲು_ಸಂಖ್ಯೆ) {
            ಸಾಲು=s+offsets[ಸಾಲು_ಸಂಖ್ಯೆ];
            length=lengths[ಸಾಲು_ಸಂಖ್ಯೆ];
            ಯದಿ(length==0) {
                ಅನುವರ್ತ;
            }

            ಸಾಲು_Limit=ಸಾಲು+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_ಎಣಿಕೆ, tokenStrings, tokenLengths, gNameSet, &ಸಾಲು, ಸಾಲು_Limit);
            ಯದಿ(length>maxNameLength) {
                maxNameLength=length;
            }
            ಯದಿ(ಸಾಲು==ಸಾಲು_Limit) {
                ಅನುವರ್ತ;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_ಎಣಿಕೆ, tokenStrings, tokenLengths, gNameSet, &ಸಾಲು, ಸಾಲು_Limit);
            ಯದಿ(length>maxNameLength) {
                maxNameLength=length;
            }
            ಯದಿ(ಸಾಲು==ಸಾಲು_Limit) {
                ಅನುವರ್ತ;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_ಎಣಿಕೆ;
    }

    ಯದಿ(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

ಸ್ಥಿರ UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    ಯದಿ(gMaxNameLength!=0) {
        ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
    }

    ಯದಿ(!isDataLoaded(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    ಪ್ರತಿ (i = 0; i < static_cast<int32_t>(ಪರಿಮಾಣ(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           ವರ್ಣ *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ ಯದಿ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ((uint32_t)code>UCHAR_MAX_ಮೌಲ್ಯ || !isDataLoaded(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಯಾವತ್(i>0) {
        ಯದಿ(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            ಭಙ್ಗ;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    ಯದಿ(i==0) {
        ಯದಿ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            ಯದಿ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } ಅನ್ಯಥಾ {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    ಪ್ರತ್ಯಾಗಮ u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                ವರ್ಣ *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    } ಅನ್ಯಥಾ ಯದಿ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತ್ಯಾಗಮ u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ಧ್ರುವ ವರ್ಣ *ಧ್ರುವ name,
               UErrorCode *pErrorCode) {
    ವರ್ಣ lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    ವರ್ಣ c0;
    ಸ್ಥಿರ constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ error;
    }

    ಯದಿ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ error;
    }

    ಯದಿ(!isDataLoaded(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ error;
    }

    /* construct the lowercase of the name first */
    ಧ್ರುವ ವರ್ಣ* nameChar = name;
    ಪ್ರತಿ(i=0; i<ಪರಿಮಾಣ(lower); ++i) {
        ಯದಿ((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } ಅನ್ಯಥಾ {
            ಭಙ್ಗ;
        }
    }
    ಯದಿ(i==ಪರಿಮಾಣ(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    ಯದಿ (lower[0] == '<') {
        ಯದಿ (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            ಯಾವತ್ (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            ಯದಿ (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                ಪ್ರತಿ (++i; i < limit; ++i) {
                    ಯದಿ (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } ಅನ್ಯಥಾ ಯದಿ (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } ಅನ್ಯಥಾ {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        ಪ್ರತ್ಯಾಗಮ error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    ಯದಿ (cp > UCHAR_MAX_ಮೌಲ್ಯ) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        ಪ್ರತ್ಯಾಗಮ error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                ಪ್ರತಿ (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    ಯದಿ (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        ಯದಿ (cat == cIdx) {
                            ಪ್ರತ್ಯಾಗಮ cp;
                        }
                        ಭಙ್ಗ;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        ಪ್ರತ್ಯಾಗಮ error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಯಾವತ್(i>0) {
        ಯದಿ((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            ಪ್ರತ್ಯಾಗಮ cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_ಮೌಲ್ಯ + 1, DO_FIND_NAME, &findName, nameChoice);
    ಯದಿ (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    ಪ್ರತ್ಯಾಗಮ findName.code;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                ಶೂನ್ಯ *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(nameChoice>=U_CHAR_NAME_CHOICE_ಎಣಿಕೆ || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ((uint32_t) limit > UCHAR_MAX_ಮೌಲ್ಯ + 1) {
        limit = UCHAR_MAX_ಮೌಲ್ಯ + 1;
    }
    ಯದಿ((uint32_t)start>=(uint32_t)limit) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(!isDataLoaded(pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ಯಾವತ್(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        ಯದಿ((uint32_t)start<algRange->start) {
            ಯದಿ((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯದಿ(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        ಯದಿ((uint32_t)start<=algRange->end) {
            ಯದಿ((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                ಪ್ರತ್ಯಾಗಮ;
            }
            ಯದಿ(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                ಪ್ರತ್ಯಾಗಮ;
            }
            start=(UChar32)algRange->end+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enumNames(uCharNames, start, limit, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_getMaxCharNameLength() {
    UErrorCode errorCode=U_ZERO_ERROR;
    ಯದಿ(calcNameSetsLengths(&errorCode)) {
        ಪ್ರತ್ಯಾಗಮ gMaxNameLength;
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
charSetToUSet(uint32_t cset[8], ಧ್ರುವ USetAdder *sa) {
    char16_t us[256];
    ವರ್ಣ cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    ಯದಿ(!calcNameSetsLengths(&errorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    ಪ್ರತಿ(i=0; i<256; ++i) {
        ಯದಿ(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<ವರ್ಣ>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    ಪ್ರತಿ(i=0; i<length; ++i) {
        ಯದಿ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI ಶೂನ್ಯ U_EXPORT2
uprv_getCharNameCharacters(ಧ್ರುವ USetAdder *sa) {
    charSetToUSet(gNameSet, sa);
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
makeTokenMap(ಧ್ರುವ UDataSwapper *ds,
             int16_t tokens[], uint16_t token_ಎಣಿಕೆ,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        ಪ್ರತಿ(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } ಅನ್ಯಥಾ {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        ಯದಿ(token_ಎಣಿಕೆ>256) {
            token_ಎಣಿಕೆ=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        ಪ್ರತಿ(i=1; i<token_ಎಣಿಕೆ; ++i) {
            ಯದಿ(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                ಯದಿ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    ಪ್ರತ್ಯಾಗಮ;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=ಸತ್ಯ;
            }
        }

        /* set the mappings for the rest of the permutation */
        ಪ್ರತಿ(i=j=1; i<token_ಎಣಿಕೆ; ++i) {
            /* set mappings that were not set for direct bytes */
            ಯದಿ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                ಯಾವತ್(usedOutChar[j]) {
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
uchar_swapNames(ಧ್ರುವ UDataSwapper *ds,
                ಧ್ರುವ ಶೂನ್ಯ *inData, int32_t length, ಶೂನ್ಯ *outData,
                UErrorCode *pErrorCode) {
    ಧ್ರುವ UDataInfo *pInfo;
    int32_t headerSize;

    ಧ್ರುವ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_ಎಣಿಕೆ;

    ಧ್ರುವ AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    ಯದಿ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check data format and format version */
    pInfo=(ಧ್ರುವ UDataInfo *)((ಧ್ರುವ ವರ್ಣ *)inData+4);
    ಯದಿ(!(
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    inBytes=(ಧ್ರುವ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    ಯದಿ(length<0) {
        algNamesOffset=ds->readUInt32(((ಧ್ರುವ uint32_t *)inBytes)[3]);
    } ಅನ್ಯಥಾ {
        length-=headerSize;
        ಯದಿ( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((ಧ್ರುವ uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }
    }

    ಯದಿ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ಧ್ರುವ uint32_t *)(inBytes+offset)));
        offset+=4;

        ಪ್ರತಿ(i=0; i<count; ++i) {
            inRange=(ಧ್ರುವ AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } ಅನ್ಯಥಾ {
        /* swap data */
        ಧ್ರುವ uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_ಎಣಿಕೆ;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        ಯದಿ(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((ಧ್ರುವ uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((ಧ್ರುವ uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((ಧ್ರುವ uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ಧ್ರುವ uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_ಎಣಿಕೆ=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        ಯದಿ(token_ಎಣಿಕೆ<=512) {
            count=token_ಎಣಿಕೆ;
        } ಅನ್ಯಥಾ {
            count=512;
        }
        ಪ್ರತಿ(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        ಪ್ರತಿ(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_ಎಣಿಕೆ, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_ಎಣಿಕೆ>256 ? token_ಎಣಿಕೆ-256 : 0), trailMap, pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_ಎಣಿಕೆ*2);
        ಯದಿ(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_ಎಣಿಕೆ);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        ಪ್ರತಿ(i=0; i<token_ಎಣಿಕೆ && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        ಪ್ರತಿ(; i<token_ಎಣಿಕೆ; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_ಎಣಿಕೆ*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ಧ್ರುವ uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        ಯದಿ(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[ಸಾಲುಗಳು_PER_GROUP+1], lengths[ಸಾಲುಗಳು_PER_GROUP+1];

            ಧ್ರುವ uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_ಎಣಿಕೆ=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            ಯಾವತ್(strings_ಎಣಿಕೆ>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_ಎಣಿಕೆ-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_ಎಣಿಕೆ-=count;

                /* swap the string bytes using map[] and trailMap[] */
                ಯಾವತ್(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    ಯದಿ(tokens[c]!=-2) {
                        --count;
                    } ಅನ್ಯಥಾ {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ಧ್ರುವ uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        ಪ್ರತಿ(i=0; i<count; ++i) {
            ಯದಿ(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }

            inRange=(ಧ್ರುವ AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            ವಿಕಲ್ಪ(inRange->type) {
            ಪಕ್ಷ 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((ಧ್ರುವ ವರ್ಣ *)(inRange+1)),
                                    outRange+1, pErrorCode);
                ಯದಿ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಭಙ್ಗ;
            ಪಕ್ಷ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_ಎಣಿಕೆ;

                    factors_ಎಣಿಕೆ=inRange->variant;
                    p=(ಧ್ರುವ uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_ಎಣಿಕೆ*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_ಎಣಿಕೆ;
                    q+=factors_ಎಣಿಕೆ;
                    strings_ಎಣಿಕೆ=(uint32_t)((inBytes+offset)-(ಧ್ರುವ uint8_t *)p);
                    ಯಾವತ್(strings_ಎಣಿಕೆ>0 && ((ಧ್ರುವ uint8_t *)p)[strings_ಎಣಿಕೆ-1]!=0) {
                        --strings_ಎಣಿಕೆ;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_ಎಣಿಕೆ, q, pErrorCode);
                }
                ಭಙ್ಗ;
            ಸ್ವಭಾವ:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                ಪ್ರತ್ಯಾಗಮ 0;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
