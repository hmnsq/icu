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

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "unicode/uchar.h"
#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "ucln_cmn.h"
#അന്തര്ഭാവ "udataswp.h"
#അന്തര്ഭാവ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

സ്ഥിതം സ്ഥിരം അക്ഷരം DATA_NAME[] = "unames";
സ്ഥിതം സ്ഥിരം അക്ഷരം DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define വരികൾ_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (വരികൾ_PER_GROUP-1)

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
എണ്ണൽ {
    GROUP_MSB,
    GROUP_OFFSET_മുകളിലെ,
    GROUP_OFFSET_താഴെയുള്ള,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_മുകളിലെ]<<16|(group)[GROUP_OFFSET_താഴെയുള്ള])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

തരനിർവചനം ഘടന {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

തരനിർവചനം ഘടന {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

നാമാകാശ {
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
വർഗ്ഗം CharacterNameQuery {
  പൊതു:
    വർഗ്ഗം Matcher {
      പൊതു:
        // `query` must outlive the constructed object.
        Matcher(സ്ഥിരം CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        തർക്കം consistentWith(അക്ഷരം c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            എങ്കിൽ (skeletonIterator == query.skeleton.end()) {
                തിരികെ അസത്യം;
            }
            എങ്കിൽ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                എങ്കിൽ (lastChar == '-') {
                    എങ്കിൽ (*skeletonIterator++ != lastChar) {
                        തിരികെ അസത്യം;
                    }
                }
                lastChar = c;
                തിരികെ സത്യം;
            } അല്ലെങ്കിൽ എങ്കിൽ (c == '-') {
                എങ്കിൽ (lastChar == ' ' ||
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
                    എങ്കിൽ (*skeletonIterator++ != c) {
                        തിരികെ അസത്യം;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                തിരികെ സത്യം;
            } അല്ലെങ്കിൽ {
                എങ്കിൽ (*skeletonIterator++ != c) {
                    തിരികെ അസത്യം;
                }
                lastChar = c;
                തിരികെ സത്യം;
            }
        }

        തർക്കം consistentWith(സ്ഥിരം std::string_view substring) {
            വേണ്ടി (സ്ഥിരം അക്ഷരം c : substring) {
                എങ്കിൽ (!consistentWith(c)) {
                    തിരികെ അസത്യം;
                }
            }
            തിരികെ സത്യം;
        }

        തർക്കം matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            തിരികെ skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() സ്ഥിരം {
            തിരികെ query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      സ്വകാര്യ:
        സ്ഥിരം CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        അക്ഷരം lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        അക്ഷരം *skeletonLimit = skeletonData;
        വേണ്ടി (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + വലുപ്പം(skeletonData));
            എങ്കിൽ (skeletonLimit >= skeletonData + വലുപ്പം(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                നിർത്തുക;
            }
            എങ്കിൽ (query[i] == ' ' || query[i] == '_') {
                തുടരുക;
            }
            എങ്കിൽ (query[i] == '-') {
                തർക്കം isMedial;
                തർക്കം is1180MedialHyphen = അസത്യം;
                എങ്കിൽ (i == 0 || i == query.length() - 1) {
                    isMedial = അസത്യം;
                } അല്ലെങ്കിൽ {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                എങ്കിൽ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = സത്യം;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    വേണ്ടി (std::size_t j = i + 2; j < query.length(); ++j) {
                        എങ്കിൽ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = അസത്യം;
                        }
                    }
                }
                എങ്കിൽ (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } അല്ലെങ്കിൽ {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() സ്ഥിരം {
        തിരികെ Matcher(*ഇത്);
    }

സ്വകാര്യ:
    സ്ഥിതം തർക്കം isASCIILetterOrDigit(സ്ഥിരം അക്ഷരം c) {
        തിരികെ uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    അക്ഷരം skeletonData[120];
    std::string_view skeleton;
    തർക്കം is1180;
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
#define GET_GROUPS(names) (സ്ഥിരം uint16_t *)((സ്ഥിരം അക്ഷരം *)names+names->groupsOffset)

തരനിർവചനം ഘടന {
    സ്ഥിരം CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

സ്ഥിതം UDataMemory *uCharNamesData=nullptr;
സ്ഥിതം UCharNames *uCharNames=nullptr;
സ്ഥിതം icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
സ്ഥിതം int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
സ്ഥിതം uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_എണ്ണം
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_എണ്ണം + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_എണ്ണം + 2

#define U_CHAR_EXTENDED_CATEGORY_എണ്ണം (U_CHAR_CATEGORY_എണ്ണം + 3)

സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം charCatNames[U_CHAR_EXTENDED_CATEGORY_എണ്ണം] = {
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

സ്ഥിതം UBool U_CALLCONV unames_cleanup()
{
    എങ്കിൽ(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    എങ്കിൽ(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    തിരികെ സത്യം;
}

സ്ഥിതം UBool U_CALLCONV
isAcceptable(ശൂന്യം * /*context*/,
             സ്ഥിരം അക്ഷരം * /*type*/, സ്ഥിരം അക്ഷരം * /*name*/,
             സ്ഥിരം UDataInfo *pInfo) {
    തിരികെ
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    എങ്കിൽ(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } അല്ലെങ്കിൽ {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


സ്ഥിതം UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    തിരികെ U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    എങ്കിൽ((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_എണ്ണം

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
സ്ഥിതം uint16_t
expandName(UCharNames *names,
           സ്ഥിരം uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           അക്ഷരം *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_എണ്ണം=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    എങ്കിൽ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        എങ്കിൽ (static_cast<uint8_t>(';') >= token_എണ്ണം || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            പൂർണ്ണസംഖ്യ fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ചെയ്യുക {
                ആകുമ്പോൾ(nameLength>0) {
                    --nameLength;
                    എങ്കിൽ(*name++==';') {
                        നിർത്തുക;
                    }
                }
            } ആകുമ്പോൾ(--fieldIndex>0);
        } അല്ലെങ്കിൽ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    ആകുമ്പോൾ(nameLength>0) {
        --nameLength;
        c=*name++;

        എങ്കിൽ(c>=token_എണ്ണം) {
            എങ്കിൽ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } അല്ലെങ്കിൽ {
                /* finished */
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ {
            token=tokens[c];
            എങ്കിൽ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            എങ്കിൽ (token == static_cast<uint16_t>(-1)) {
                എങ്കിൽ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } അല്ലെങ്കിൽ {
                    /* finished */
                    നിർത്തുക;
                }
            } അല്ലെങ്കിൽ {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                ആകുമ്പോൾ((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    എങ്കിൽ(bufferLength>0) {
        *buffer=0;
    }

    തിരികെ bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
സ്ഥിതം UBool
compareName(UCharNames *names,
            സ്ഥിരം uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            സ്ഥിരം CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_എണ്ണം=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    എങ്കിൽ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        എങ്കിൽ (static_cast<uint8_t>(';') >= token_എണ്ണം || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            പൂർണ്ണസംഖ്യ fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            ചെയ്യുക {
                ആകുമ്പോൾ(nameLength>0) {
                    --nameLength;
                    എങ്കിൽ(*name++==';') {
                        നിർത്തുക;
                    }
                }
            } ആകുമ്പോൾ(--fieldIndex>0);
        } അല്ലെങ്കിൽ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    ആകുമ്പോൾ(nameLength>0) {
        --nameLength;
        c=*name++;

        എങ്കിൽ(c>=token_എണ്ണം) {
            എങ്കിൽ(c!=';') {
                /* implicit letter */
                എങ്കിൽ (!matcher.consistentWith(c)) {
                    തിരികെ അസത്യം;
                }
            } അല്ലെങ്കിൽ {
                /* finished */
                നിർത്തുക;
            }
        } അല്ലെങ്കിൽ {
            token=tokens[c];
            എങ്കിൽ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            എങ്കിൽ (token == static_cast<uint16_t>(-1)) {
                എങ്കിൽ(c!=';') {
                    /* explicit letter */
                    എങ്കിൽ (!matcher.consistentWith(c)) {
                        തിരികെ അസത്യം;
                    }
                } അല്ലെങ്കിൽ {
                    /* finished */
                    നിർത്തുക;
                }
            } അല്ലെങ്കിൽ {
                /* write token word */
                എങ്കിൽ (!matcher.consistentWith(reinterpret_cast<സ്ഥിരം അക്ഷരം *>(tokenStrings + token))) {
                    തിരികെ അസത്യം;
                }
            }
        }
    }

    /* complete match? */
    തിരികെ matcher.matches();
}

സ്ഥിതം uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    എങ്കിൽ (U_IS_UNICODE_NONCHAR(cp)) {
        തിരികെ U_NONCHARACTER_CODE_POINT;
    }

    എങ്കിൽ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    തിരികെ cat;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    എങ്കിൽ (cat >= UPRV_LENGTHOF(charCatNames)) {
        തിരികെ "unknown";
    } അല്ലെങ്കിൽ {
        തിരികെ charCatNames[cat];
    }
}

സ്ഥിതം uint16_t getExtName(uint32_t code, അക്ഷരം *buffer, uint16_t bufferLength) {
    സ്ഥിരം അക്ഷരം *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    പൂർണ്ണസംഖ്യ ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    ആകുമ്പോൾ (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    വേണ്ടി (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    എങ്കിൽ (ndigits < 4)
        ndigits = 4;
    വേണ്ടി (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    തിരികെ length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
സ്ഥിതം സ്ഥിരം uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    സ്ഥിരം uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    ആകുമ്പോൾ(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        എങ്കിൽ(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } അല്ലെങ്കിൽ {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    തിരികെ groups+start*GROUP_LENGTH;
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
സ്ഥിതം സ്ഥിരം uint8_t *
expandGroupLengths(സ്ഥിരം uint8_t *s,
                   uint16_t offsets[വരികൾ_PER_GROUP+1], uint16_t lengths[വരികൾ_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    ആകുമ്പോൾ(i<വരികൾ_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        എങ്കിൽ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } അല്ലെങ്കിൽ എങ്കിൽ((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } അല്ലെങ്കിൽ {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        എങ്കിൽ((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            എങ്കിൽ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } അല്ലെങ്കിൽ {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    തിരികെ s;
}

സ്ഥിതം uint16_t
expandGroupName(UCharNames *names, സ്ഥിരം uint16_t *group,
                uint16_t വരി_സംഖ്യ, UCharNameChoice nameChoice,
                അക്ഷരം *buffer, uint16_t bufferLength) {
    uint16_t offsets[വരികൾ_PER_GROUP+2], lengths[വരികൾ_PER_GROUP+2];
    സ്ഥിരം uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    തിരികെ expandName(names, s+offsets[വരി_സംഖ്യ], lengths[വരി_സംഖ്യ], nameChoice,
                      buffer, bufferLength);
}

സ്ഥിതം uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        അക്ഷരം *buffer, uint16_t bufferLength) {
    സ്ഥിരം uint16_t *group=getGroup(names, code);
    എങ്കിൽ (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        തിരികെ expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } അല്ലെങ്കിൽ {
        /* group not found */
        /* zero-terminate */
        എങ്കിൽ(bufferLength>0) {
            *buffer=0;
        }
        തിരികെ 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
സ്ഥിതം UBool
enumGroupNames(UCharNames *names, സ്ഥിരം uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, ശൂന്യം *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[വരികൾ_PER_GROUP+2], lengths[വരികൾ_PER_GROUP+2];
    സ്ഥിരം uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    എങ്കിൽ(fn!=DO_FIND_NAME) {
        അക്ഷരം buffer[200];
        uint16_t length;

        ആകുമ്പോൾ(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, വലുപ്പം(buffer));
            എങ്കിൽ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, വലുപ്പം(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            എങ്കിൽ(length>0) {
                എങ്കിൽ(!fn(context, start, nameChoice, buffer, length)) {
                    തിരികെ അസത്യം;
                }
            }
            ++start;
        }
    } അല്ലെങ്കിൽ {
        സ്ഥിരം CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        ആകുമ്പോൾ(start<=end) {
            എങ്കിൽ(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                തിരികെ അസത്യം;
            }
            ++start;
        }
    }
    തിരികെ സത്യം;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
സ്ഥിതം UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, ശൂന്യം *context)
{
    എങ്കിൽ(fn!=DO_FIND_NAME) {
        അക്ഷരം buffer[200];
        uint16_t length;
        
        ആകുമ്പോൾ(start<=end) {
            buffer[length = getExtName(start, buffer, വലുപ്പം(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            എങ്കിൽ(length>0) {
                എങ്കിൽ(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    തിരികെ അസത്യം;
                }
            }
            ++start;
        }
    }

    തിരികെ സത്യം;
}

സ്ഥിതം UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, ശൂന്യം *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_എണ്ണം;
    സ്ഥിരം uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    എങ്കിൽ(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        എങ്കിൽ(extLimit>limit) {
            extLimit=limit;
        }
        എങ്കിൽ(!enumExtNames(start, extLimit-1, fn, context)) {
            തിരികെ അസത്യം;
        }
        start=extLimit;
    }

    എങ്കിൽ(startGroupMSB==endGroupMSB) {
        എങ്കിൽ(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            തിരികെ enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } അല്ലെങ്കിൽ {
        സ്ഥിരം uint16_t *groups=GET_GROUPS(names);
        group_എണ്ണം=*groups++;
        groupLimit=groups+group_എണ്ണം*GROUP_LENGTH;

        എങ്കിൽ(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            എങ്കിൽ((start&GROUP_MASK)!=0) {
                എങ്കിൽ(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + വരികൾ_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    തിരികെ അസത്യം;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } അല്ലെങ്കിൽ എങ്കിൽ(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            സ്ഥിരം uint16_t *nextGroup=NEXT_GROUP(group);
            എങ്കിൽ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                എങ്കിൽ (end > limit) {
                    end = limit;
                }
                എങ്കിൽ (!enumExtNames(start, end - 1, fn, context)) {
                    തിരികെ അസത്യം;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        ആകുമ്പോൾ(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            സ്ഥിരം uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            എങ്കിൽ(!enumGroupNames(names, group, start, start+വരികൾ_PER_GROUP-1, fn, context, nameChoice)) {
                തിരികെ അസത്യം;
            }
            nextGroup=NEXT_GROUP(group);
            എങ്കിൽ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                എങ്കിൽ (end > limit) {
                    end = limit;
                }
                എങ്കിൽ (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    തിരികെ അസത്യം;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        എങ്കിൽ(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            തിരികെ enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } അല്ലെങ്കിൽ എങ്കിൽ (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            എങ്കിൽ (next > start) {
                start = next;
            }
        } അല്ലെങ്കിൽ {
            തിരികെ സത്യം;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    എങ്കിൽ (nameChoice == U_EXTENDED_CHAR_NAME) {
        എങ്കിൽ (limit > UCHAR_MAX_മൂല്യം + 1) {
            limit = UCHAR_MAX_മൂല്യം + 1;
        }
        തിരികെ enumExtNames(start, limit - 1, fn, context);
    }
    
    തിരികെ സത്യം;
}

സ്ഥിതം uint16_t
writeFactorSuffix(സ്ഥിരം uint16_t *factors, uint16_t count,
                  സ്ഥിരം അക്ഷരം *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  സ്ഥിരം അക്ഷരം *elementBases[8], സ്ഥിരം അക്ഷരം *elements[8],
                  അക്ഷരം *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    അക്ഷരം c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    വേണ്ടി(i=count; i>0; --i) {
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
    വേണ്ടി(;;) {
        എങ്കിൽ(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        ആകുമ്പോൾ(factor>0) {
            ആകുമ്പോൾ(*s++!=0) {}
            --factor;
        }
        എങ്കിൽ(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        ആകുമ്പോൾ((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        എങ്കിൽ(i>=count) {
            നിർത്തുക;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        ആകുമ്പോൾ(factor>0) {
            ആകുമ്പോൾ(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    എങ്കിൽ(bufferLength>0) {
        *buffer=0;
    }

    തിരികെ bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
സ്ഥിതം uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        അക്ഷരം *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    എങ്കിൽ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        എങ്കിൽ(bufferLength>0) {
            *buffer=0;
        }
        തിരികെ 0;
    }

    തിരഞ്ഞെടുപ്പ്(range->type) {
    അവസ്ഥ 0: {
        /* name = prefix hex-digits */
        സ്ഥിരം അക്ഷരം* s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(range + 1);
        അക്ഷരം c;

        uint16_t i, count;

        /* copy prefix */
        ആകുമ്പോൾ((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        എങ്കിൽ(count<bufferLength) {
            buffer[count]=0;
        }

        വേണ്ടി(i=count; i>0;) {
            എങ്കിൽ(--i<bufferLength) {
                c = static_cast<അക്ഷരം>(code & 0xf);
                എങ്കിൽ(c<10) {
                    c+='0';
                } അല്ലെങ്കിൽ {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        നിർത്തുക;
    }
    അവസ്ഥ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        സ്ഥിരം uint16_t* factors = reinterpret_cast<സ്ഥിരം uint16_t*>(range + 1);
        uint16_t count=range->variant;
        സ്ഥിരം അക്ഷരം* s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(factors + count);
        അക്ഷരം c;

        /* copy prefix */
        ആകുമ്പോൾ((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        /* undefined type */
        /* zero-terminate */
        എങ്കിൽ(bufferLength>0) {
            *buffer=0;
        }
        നിർത്തുക;
    }

    തിരികെ bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
സ്ഥിതം UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, ശൂന്യം *context,
             UCharNameChoice nameChoice) {
    അക്ഷരം buffer[200];
    uint16_t length;

    എങ്കിൽ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        തിരികെ സത്യം;
    }

    തിരഞ്ഞെടുപ്പ്(range->type) {
    അവസ്ഥ 0: {
        അക്ഷരം *s, *end;
        അക്ഷരം c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, വലുപ്പം(buffer));
        എങ്കിൽ(length<=0) {
            തിരികെ സത്യം;
        }

        /* call the enumerator function with this first character */
        എങ്കിൽ(!fn(context, start, nameChoice, buffer, length)) {
            തിരികെ അസത്യം;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        ആകുമ്പോൾ(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        ആകുമ്പോൾ(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            വേണ്ടി (;;) {
                c=*--s;
                എങ്കിൽ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<അക്ഷരം>(c + 1);
                    നിർത്തുക;
                } അല്ലെങ്കിൽ എങ്കിൽ(c=='9') {
                    *s='A';
                    നിർത്തുക;
                } അല്ലെങ്കിൽ എങ്കിൽ(c=='F') {
                    *s='0';
                }
            }

            എങ്കിൽ(!fn(context, start, nameChoice, buffer, length)) {
                തിരികെ അസത്യം;
            }
        }
        നിർത്തുക;
    }
    അവസ്ഥ 1: {
        uint16_t indexes[8];
        സ്ഥിരം അക്ഷരം *elementBases[8], *elements[8];
        സ്ഥിരം uint16_t* factors = reinterpret_cast<സ്ഥിരം uint16_t*>(range + 1);
        uint16_t count=range->variant;
        സ്ഥിരം അക്ഷരം* s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(factors + count);
        അക്ഷരം *suffix, *t;
        uint16_t prefixLength, i, idx;

        അക്ഷരം c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        ആകുമ്പോൾ((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(വലുപ്പം(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        എങ്കിൽ(!fn(context, start, nameChoice, buffer, length)) {
            തിരികെ അസത്യം;
        }

        /* enumerate the rest of the names */
        ആകുമ്പോൾ(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            വേണ്ടി (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                എങ്കിൽ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ആകുമ്പോൾ(*s++!=0) {
                    }
                    elements[i]=s;
                    നിർത്തുക;
                } അല്ലെങ്കിൽ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            വേണ്ടി(i=0; i<count; ++i) {
                s=elements[i];
                ആകുമ്പോൾ((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            എങ്കിൽ(!fn(context, start, nameChoice, buffer, length)) {
                തിരികെ അസത്യം;
            }
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        /* undefined type */
        നിർത്തുക;
    }

    തിരികെ സത്യം;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
സ്ഥിതം UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    എങ്കിൽ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        തിരികെ 0xffff;
    }

    തിരഞ്ഞെടുപ്പ്(range->type) {
    അവസ്ഥ 0: {
        /* name = prefix hex-digits */
        സ്ഥിരം അക്ഷരം* s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(range + 1);

        /* compare prefix */
        എങ്കിൽ (!matcher.consistentWith(s)) {
            തിരികെ 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        വേണ്ടി(അക്ഷരം c : matcher.remainingSignificantCharacters()) {
            എങ്കിൽ('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } അല്ലെങ്കിൽ എങ്കിൽ('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } അല്ലെങ്കിൽ {
                തിരികെ 0xffff;
            }
        }

        /* does it fit into the range? */
        എങ്കിൽ (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            തിരികെ code;
        }
        നിർത്തുക;
    }
    അവസ്ഥ 1: {
        അക്ഷരം buffer[64];
        uint16_t indexes[8];
        സ്ഥിരം അക്ഷരം *elementBases[8], *elements[8];
        സ്ഥിരം uint16_t* factors = reinterpret_cast<സ്ഥിരം uint16_t*>(range + 1);
        uint16_t count=range->variant;
        സ്ഥിരം അക്ഷരം *s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        എങ്കിൽ (!matcher.consistentWith(prefix)) {
            തിരികെ 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, വലുപ്പം(buffer));

        /* compare the first suffix */
        എങ്കിൽ(querySuffix == buffer) {
            തിരികെ start;
        }

        /* enumerate and compare the rest of the suffixes */
        ആകുമ്പോൾ(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            വേണ്ടി (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                എങ്കിൽ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    ആകുമ്പോൾ(*s++!=0) {}
                    elements[i]=s;
                    നിർത്തുക;
                } അല്ലെങ്കിൽ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            വേണ്ടി(i=0; i<count; ++i) {
                s=elements[i];
                അക്ഷരം c;
                ആകുമ്പോൾ((c=*s++)!=0) {
                    എങ്കിൽ(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            എങ്കിൽ(i<99 && it == querySuffix.end()) {
                തിരികെ start;
            }
        }
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        /* undefined type */
        നിർത്തുക;
    }

    തിരികെ 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

സ്ഥിതം int32_t
calcStringSetLength(uint32_t set[8], സ്ഥിരം അക്ഷരം *s) {
    int32_t length=0;
    അക്ഷരം c;

    ആകുമ്പോൾ((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    തിരികെ length;
}

സ്ഥിതം int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_എണ്ണം;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_എണ്ണം=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    ആകുമ്പോൾ(range_എണ്ണം>0) {
        തിരഞ്ഞെടുപ്പ്(range->type) {
        അവസ്ഥ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<സ്ഥിരം അക്ഷരം*>(range + 1)) + range->variant;
            എങ്കിൽ(length>maxNameLength) {
                maxNameLength=length;
            }
            നിർത്തുക;
        അവസ്ഥ 1: {
            /* name = prefix factorized-elements */
            സ്ഥിരം uint16_t* factors = reinterpret_cast<സ്ഥിരം uint16_t*>(range + 1);
            സ്ഥിരം അക്ഷരം *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<സ്ഥിരം അക്ഷരം*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            വേണ്ടി(i=0; i<count; ++i) {
                maxFactorLength=0;
                വേണ്ടി(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    എങ്കിൽ(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            എങ്കിൽ(length>maxNameLength) {
                maxNameLength=length;
            }
            നിർത്തുക;
        }
        സ്ഥിരസ്ഥിതി:
            /* unknown type */
            നിർത്തുക;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_എണ്ണം;
    }
    തിരികെ maxNameLength;
}

സ്ഥിതം int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    വേണ്ടി(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        എങ്കിൽ(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    തിരികെ maxNameLength;
}

സ്ഥിതം int32_t
calcNameSetLength(സ്ഥിരം uint16_t *tokens, uint16_t token_എണ്ണം, സ്ഥിരം uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  സ്ഥിരം uint8_t **p_വരി, സ്ഥിരം uint8_t *വരി_Limit) {
    സ്ഥിരം uint8_t *വരി=*p_വരി;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    ആകുമ്പോൾ (വരി != വരി_Limit && (c = *വരി++) != static_cast<uint8_t>(';')) {
        എങ്കിൽ(c>=token_എണ്ണം) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } അല്ലെങ്കിൽ {
            token=tokens[c];
            എങ്കിൽ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*വരി++;
                token=tokens[c];
            }
            എങ്കിൽ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } അല്ലെങ്കിൽ {
                /* count token word */
                എങ്കിൽ(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    എങ്കിൽ(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<സ്ഥിരം അക്ഷരം*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } അല്ലെങ്കിൽ {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<സ്ഥിരം അക്ഷരം*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_വരി=വരി;
    തിരികെ length;
}

സ്ഥിതം ശൂന്യം
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[വരികൾ_PER_GROUP+2], lengths[വരികൾ_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_എണ്ണം=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    സ്ഥിരം uint16_t *group;
    സ്ഥിരം uint8_t *s, *വരി, *വരി_Limit;

    int32_t group_എണ്ണം, വരി_സംഖ്യ, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_എണ്ണം));
    എങ്കിൽ(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_എണ്ണം);
    }

    group=GET_GROUPS(uCharNames);
    group_എണ്ണം=*group++;

    /* enumerate all groups */
    ആകുമ്പോൾ(group_എണ്ണം>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        വേണ്ടി(വരി_സംഖ്യ=0; വരി_സംഖ്യ<വരികൾ_PER_GROUP; ++വരി_സംഖ്യ) {
            വരി=s+offsets[വരി_സംഖ്യ];
            length=lengths[വരി_സംഖ്യ];
            എങ്കിൽ(length==0) {
                തുടരുക;
            }

            വരി_Limit=വരി+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_എണ്ണം, tokenStrings, tokenLengths, gNameSet, &വരി, വരി_Limit);
            എങ്കിൽ(length>maxNameLength) {
                maxNameLength=length;
            }
            എങ്കിൽ(വരി==വരി_Limit) {
                തുടരുക;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_എണ്ണം, tokenStrings, tokenLengths, gNameSet, &വരി, വരി_Limit);
            എങ്കിൽ(length>maxNameLength) {
                maxNameLength=length;
            }
            എങ്കിൽ(വരി==വരി_Limit) {
                തുടരുക;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_എണ്ണം;
    }

    എങ്കിൽ(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

സ്ഥിതം UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    സ്ഥിതം സ്ഥിരം അക്ഷരം extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    എങ്കിൽ(gMaxNameLength!=0) {
        തിരികെ സത്യം;
    }

    എങ്കിൽ(!isDataLoaded(pErrorCode)) {
        തിരികെ അസത്യം;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    വേണ്ടി (i = 0; i < static_cast<int32_t>(വലുപ്പം(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    തിരികെ സത്യം;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           അക്ഷരം *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(nameChoice>=U_CHAR_NAME_CHOICE_എണ്ണം ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ((uint32_t)code>UCHAR_MAX_മൂല്യം || !isDataLoaded(pErrorCode)) {
        തിരികെ u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ആകുമ്പോൾ(i>0) {
        എങ്കിൽ(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            നിർത്തുക;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    എങ്കിൽ(i==0) {
        എങ്കിൽ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            എങ്കിൽ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } അല്ലെങ്കിൽ {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    തിരികെ u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                അക്ഷരം *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    തിരികെ u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               സ്ഥിരം അക്ഷരം *സ്ഥിരം name,
               UErrorCode *pErrorCode) {
    അക്ഷരം lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    അക്ഷരം c0;
    സ്ഥിതം constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ error;
    }

    എങ്കിൽ(nameChoice>=U_CHAR_NAME_CHOICE_എണ്ണം || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ error;
    }

    എങ്കിൽ(!isDataLoaded(pErrorCode)) {
        തിരികെ error;
    }

    /* construct the lowercase of the name first */
    സ്ഥിരം അക്ഷരം* nameChar = name;
    വേണ്ടി(i=0; i<വലുപ്പം(lower); ++i) {
        എങ്കിൽ((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } അല്ലെങ്കിൽ {
            നിർത്തുക;
        }
    }
    എങ്കിൽ(i==വലുപ്പം(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        തിരികെ error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    എങ്കിൽ (lower[0] == '<') {
        എങ്കിൽ (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            ആകുമ്പോൾ (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            എങ്കിൽ (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                വേണ്ടി (++i; i < limit; ++i) {
                    എങ്കിൽ (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } അല്ലെങ്കിൽ എങ്കിൽ (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } അല്ലെങ്കിൽ {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        തിരികെ error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    എങ്കിൽ (cp > UCHAR_MAX_മൂല്യം) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        തിരികെ error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                വേണ്ടി (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    എങ്കിൽ (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        എങ്കിൽ (cat == cIdx) {
                            തിരികെ cp;
                        }
                        നിർത്തുക;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        തിരികെ error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ആകുമ്പോൾ(i>0) {
        എങ്കിൽ((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            തിരികെ cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_മൂല്യം + 1, DO_FIND_NAME, &findName, nameChoice);
    എങ്കിൽ (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    തിരികെ findName.code;
}

U_CAPI ശൂന്യം U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                ശൂന്യം *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    എങ്കിൽ(nameChoice>=U_CHAR_NAME_CHOICE_എണ്ണം || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
    }

    എങ്കിൽ((uint32_t) limit > UCHAR_MAX_മൂല്യം + 1) {
        limit = UCHAR_MAX_മൂല്യം + 1;
    }
    എങ്കിൽ((uint32_t)start>=(uint32_t)limit) {
        തിരികെ;
    }

    എങ്കിൽ(!isDataLoaded(pErrorCode)) {
        തിരികെ;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    ആകുമ്പോൾ(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        എങ്കിൽ((uint32_t)start<algRange->start) {
            എങ്കിൽ((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                തിരികെ;
            }
            എങ്കിൽ(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                തിരികെ;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        എങ്കിൽ((uint32_t)start<=algRange->end) {
            എങ്കിൽ((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                തിരികെ;
            }
            എങ്കിൽ(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                തിരികെ;
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
    എങ്കിൽ(calcNameSetsLengths(&errorCode)) {
        തിരികെ gMaxNameLength;
    } അല്ലെങ്കിൽ {
        തിരികെ 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
സ്ഥിതം ശൂന്യം
charSetToUSet(uint32_t cset[8], സ്ഥിരം USetAdder *sa) {
    char16_t us[256];
    അക്ഷരം cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    എങ്കിൽ(!calcNameSetsLengths(&errorCode)) {
        തിരികെ;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    വേണ്ടി(i=0; i<256; ++i) {
        എങ്കിൽ(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<അക്ഷരം>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    വേണ്ടി(i=0; i<length; ++i) {
        എങ്കിൽ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI ശൂന്യം U_EXPORT2
uprv_getCharNameCharacters(സ്ഥിരം USetAdder *sa) {
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
സ്ഥിതം ശൂന്യം
makeTokenMap(സ്ഥിരം UDataSwapper *ds,
             int16_t tokens[], uint16_t token_എണ്ണം,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    എങ്കിൽ(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        വേണ്ടി(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } അല്ലെങ്കിൽ {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        എങ്കിൽ(token_എണ്ണം>256) {
            token_എണ്ണം=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        വേണ്ടി(i=1; i<token_എണ്ണം; ++i) {
            എങ്കിൽ(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    തിരികെ;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=സത്യം;
            }
        }

        /* set the mappings for the rest of the permutation */
        വേണ്ടി(i=j=1; i<token_എണ്ണം; ++i) {
            /* set mappings that were not set for direct bytes */
            എങ്കിൽ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                ആകുമ്പോൾ(usedOutChar[j]) {
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
uchar_swapNames(സ്ഥിരം UDataSwapper *ds,
                സ്ഥിരം ശൂന്യം *inData, int32_t length, ശൂന്യം *outData,
                UErrorCode *pErrorCode) {
    സ്ഥിരം UDataInfo *pInfo;
    int32_t headerSize;

    സ്ഥിരം uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_എണ്ണം;

    സ്ഥിരം AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    എങ്കിൽ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }

    /* check data format and format version */
    pInfo=(സ്ഥിരം UDataInfo *)((സ്ഥിരം അക്ഷരം *)inData+4);
    എങ്കിൽ(!(
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
        തിരികെ 0;
    }

    inBytes=(സ്ഥിരം uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    എങ്കിൽ(length<0) {
        algNamesOffset=ds->readUInt32(((സ്ഥിരം uint32_t *)inBytes)[3]);
    } അല്ലെങ്കിൽ {
        length-=headerSize;
        എങ്കിൽ( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((സ്ഥിരം uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            തിരികെ 0;
        }
    }

    എങ്കിൽ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((സ്ഥിരം uint32_t *)(inBytes+offset)));
        offset+=4;

        വേണ്ടി(i=0; i<count; ++i) {
            inRange=(സ്ഥിരം AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } അല്ലെങ്കിൽ {
        /* swap data */
        സ്ഥിരം uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_എണ്ണം;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        എങ്കിൽ(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((സ്ഥിരം uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((സ്ഥിരം uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((സ്ഥിരം uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(സ്ഥിരം uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_എണ്ണം=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        എങ്കിൽ(token_എണ്ണം<=512) {
            count=token_എണ്ണം;
        } അല്ലെങ്കിൽ {
            count=512;
        }
        വേണ്ടി(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        വേണ്ടി(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_എണ്ണം, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_എണ്ണം>256 ? token_എണ്ണം-256 : 0), trailMap, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            തിരികെ 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_എണ്ണം*2);
        എങ്കിൽ(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_എണ്ണം);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            തിരികെ 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        വേണ്ടി(i=0; i<token_എണ്ണം && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        വേണ്ടി(; i<token_എണ്ണം; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_എണ്ണം*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            തിരികെ 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((സ്ഥിരം uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        എങ്കിൽ(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[വരികൾ_PER_GROUP+1], lengths[വരികൾ_PER_GROUP+1];

            സ്ഥിരം uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_എണ്ണം=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            ആകുമ്പോൾ(strings_എണ്ണം>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_എണ്ണം-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_എണ്ണം-=count;

                /* swap the string bytes using map[] and trailMap[] */
                ആകുമ്പോൾ(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    എങ്കിൽ(tokens[c]!=-2) {
                        --count;
                    } അല്ലെങ്കിൽ {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((സ്ഥിരം uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        വേണ്ടി(i=0; i<count; ++i) {
            എങ്കിൽ(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                തിരികെ 0;
            }

            inRange=(സ്ഥിരം AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            തിരഞ്ഞെടുപ്പ്(inRange->type) {
            അവസ്ഥ 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((സ്ഥിരം അക്ഷരം *)(inRange+1)),
                                    outRange+1, pErrorCode);
                എങ്കിൽ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    തിരികെ 0;
                }
                നിർത്തുക;
            അവസ്ഥ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_എണ്ണം;

                    factors_എണ്ണം=inRange->variant;
                    p=(സ്ഥിരം uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_എണ്ണം*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_എണ്ണം;
                    q+=factors_എണ്ണം;
                    strings_എണ്ണം=(uint32_t)((inBytes+offset)-(സ്ഥിരം uint8_t *)p);
                    ആകുമ്പോൾ(strings_എണ്ണം>0 && ((സ്ഥിരം uint8_t *)p)[strings_എണ്ണം-1]!=0) {
                        --strings_എണ്ണം;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_എണ്ണം, q, pErrorCode);
                }
                നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                തിരികെ 0;
            }
        }
    }

    തിരികെ headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
