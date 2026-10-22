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

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "unicode/uchar.h"
#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "ucln_cmn.h"
#அந்தர்பாவ "udataswp.h"
#அந்தர்பாவ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

நிலையான மாறிலி எழுத்து DATA_NAME[] = "unames";
நிலையான மாறிலி எழுத்து DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define வரிகள்_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (வரிகள்_PER_GROUP-1)

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
எண்ணிக்கை {
    GROUP_MSB,
    GROUP_OFFSET_உயர்வு,
    GROUP_OFFSET_கீழ்,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_உயர்வு]<<16|(group)[GROUP_OFFSET_கீழ்])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

வகைவரை கட்டமைப்பு {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

வகைவரை கட்டமைப்பு {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

நாமாகாஶ {
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
வகுப்பு CharacterNameQuery {
  பொது:
    வகுப்பு Matcher {
      பொது:
        // `query` must outlive the constructed object.
        Matcher(மாறிலி CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        தருக்க consistentWith(எழுத்து c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            எனில் (skeletonIterator == query.skeleton.end()) {
                திரும்பு பொய்;
            }
            எனில் (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                எனில் (lastChar == '-') {
                    எனில் (*skeletonIterator++ != lastChar) {
                        திரும்பு பொய்;
                    }
                }
                lastChar = c;
                திரும்பு உண்மை;
            } இல்லையெனில் எனில் (c == '-') {
                எனில் (lastChar == ' ' ||
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
                    எனில் (*skeletonIterator++ != c) {
                        திரும்பு பொய்;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                திரும்பு உண்மை;
            } இல்லையெனில் {
                எனில் (*skeletonIterator++ != c) {
                    திரும்பு பொய்;
                }
                lastChar = c;
                திரும்பு உண்மை;
            }
        }

        தருக்க consistentWith(மாறிலி std::string_view substring) {
            க்கு (மாறிலி எழுத்து c : substring) {
                எனில் (!consistentWith(c)) {
                    திரும்பு பொய்;
                }
            }
            திரும்பு உண்மை;
        }

        தருக்க matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            திரும்பு skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() மாறிலி {
            திரும்பு query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      தனிப்பட்ட:
        மாறிலி CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        எழுத்து lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        எழுத்து *skeletonLimit = skeletonData;
        க்கு (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + அளவு(skeletonData));
            எனில் (skeletonLimit >= skeletonData + அளவு(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                நிறுத்து;
            }
            எனில் (query[i] == ' ' || query[i] == '_') {
                தொடர்;
            }
            எனில் (query[i] == '-') {
                தருக்க isMedial;
                தருக்க is1180MedialHyphen = பொய்;
                எனில் (i == 0 || i == query.length() - 1) {
                    isMedial = பொய்;
                } இல்லையெனில் {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                எனில் (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = உண்மை;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    க்கு (std::size_t j = i + 2; j < query.length(); ++j) {
                        எனில் (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = பொய்;
                        }
                    }
                }
                எனில் (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } இல்லையெனில் {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() மாறிலி {
        திரும்பு Matcher(*இது);
    }

தனிப்பட்ட:
    நிலையான தருக்க isASCIILetterOrDigit(மாறிலி எழுத்து c) {
        திரும்பு uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    எழுத்து skeletonData[120];
    std::string_view skeleton;
    தருக்க is1180;
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
#define GET_GROUPS(names) (மாறிலி uint16_t *)((மாறிலி எழுத்து *)names+names->groupsOffset)

வகைவரை கட்டமைப்பு {
    மாறிலி CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

நிலையான UDataMemory *uCharNamesData=nullptr;
நிலையான UCharNames *uCharNames=nullptr;
நிலையான icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
நிலையான int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
நிலையான uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_மொத்தம்
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_மொத்தம் + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_மொத்தம் + 2

#define U_CHAR_EXTENDED_CATEGORY_மொத்தம் (U_CHAR_CATEGORY_மொத்தம் + 3)

நிலையான மாறிலி எழுத்து * மாறிலி charCatNames[U_CHAR_EXTENDED_CATEGORY_மொத்தம்] = {
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

நிலையான UBool U_CALLCONV unames_cleanup()
{
    எனில்(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    எனில்(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    திரும்பு உண்மை;
}

நிலையான UBool U_CALLCONV
isAcceptable(வெறுமை * /*context*/,
             மாறிலி எழுத்து * /*type*/, மாறிலி எழுத்து * /*name*/,
             மாறிலி UDataInfo *pInfo) {
    திரும்பு
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

நிலையான வெறுமை U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    எனில்(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } இல்லையெனில் {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


நிலையான UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    திரும்பு U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    எனில்((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_மொத்தம்

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
நிலையான uint16_t
expandName(UCharNames *names,
           மாறிலி uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           எழுத்து *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_மொத்தம்=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    எனில்(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        எனில் (static_cast<uint8_t>(';') >= token_மொத்தம் || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            முழு fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            செய் {
                போது(nameLength>0) {
                    --nameLength;
                    எனில்(*name++==';') {
                        நிறுத்து;
                    }
                }
            } போது(--fieldIndex>0);
        } இல்லையெனில் {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    போது(nameLength>0) {
        --nameLength;
        c=*name++;

        எனில்(c>=token_மொத்தம்) {
            எனில்(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } இல்லையெனில் {
                /* finished */
                நிறுத்து;
            }
        } இல்லையெனில் {
            token=tokens[c];
            எனில் (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            எனில் (token == static_cast<uint16_t>(-1)) {
                எனில்(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } இல்லையெனில் {
                    /* finished */
                    நிறுத்து;
                }
            } இல்லையெனில் {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                போது((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    எனில்(bufferLength>0) {
        *buffer=0;
    }

    திரும்பு bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
நிலையான UBool
compareName(UCharNames *names,
            மாறிலி uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            மாறிலி CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_மொத்தம்=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    எனில்(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        எனில் (static_cast<uint8_t>(';') >= token_மொத்தம் || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            முழு fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            செய் {
                போது(nameLength>0) {
                    --nameLength;
                    எனில்(*name++==';') {
                        நிறுத்து;
                    }
                }
            } போது(--fieldIndex>0);
        } இல்லையெனில் {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    போது(nameLength>0) {
        --nameLength;
        c=*name++;

        எனில்(c>=token_மொத்தம்) {
            எனில்(c!=';') {
                /* implicit letter */
                எனில் (!matcher.consistentWith(c)) {
                    திரும்பு பொய்;
                }
            } இல்லையெனில் {
                /* finished */
                நிறுத்து;
            }
        } இல்லையெனில் {
            token=tokens[c];
            எனில் (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            எனில் (token == static_cast<uint16_t>(-1)) {
                எனில்(c!=';') {
                    /* explicit letter */
                    எனில் (!matcher.consistentWith(c)) {
                        திரும்பு பொய்;
                    }
                } இல்லையெனில் {
                    /* finished */
                    நிறுத்து;
                }
            } இல்லையெனில் {
                /* write token word */
                எனில் (!matcher.consistentWith(reinterpret_cast<மாறிலி எழுத்து *>(tokenStrings + token))) {
                    திரும்பு பொய்;
                }
            }
        }
    }

    /* complete match? */
    திரும்பு matcher.matches();
}

நிலையான uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    எனில் (U_IS_UNICODE_NONCHAR(cp)) {
        திரும்பு U_NONCHARACTER_CODE_POINT;
    }

    எனில் ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    திரும்பு cat;
}

நிலையான மாறிலி எழுத்து *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    எனில் (cat >= UPRV_LENGTHOF(charCatNames)) {
        திரும்பு "unknown";
    } இல்லையெனில் {
        திரும்பு charCatNames[cat];
    }
}

நிலையான uint16_t getExtName(uint32_t code, எழுத்து *buffer, uint16_t bufferLength) {
    மாறிலி எழுத்து *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    முழு ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    போது (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    க்கு (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    எனில் (ndigits < 4)
        ndigits = 4;
    க்கு (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    திரும்பு length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
நிலையான மாறிலி uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    மாறிலி uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    போது(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        எனில்(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } இல்லையெனில் {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    திரும்பு groups+start*GROUP_LENGTH;
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
நிலையான மாறிலி uint8_t *
expandGroupLengths(மாறிலி uint8_t *s,
                   uint16_t offsets[வரிகள்_PER_GROUP+1], uint16_t lengths[வரிகள்_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    போது(i<வரிகள்_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        எனில்(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } இல்லையெனில் எனில்((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } இல்லையெனில் {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        எனில்((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            எனில்(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } இல்லையெனில் {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    திரும்பு s;
}

நிலையான uint16_t
expandGroupName(UCharNames *names, மாறிலி uint16_t *group,
                uint16_t வரி_எண், UCharNameChoice nameChoice,
                எழுத்து *buffer, uint16_t bufferLength) {
    uint16_t offsets[வரிகள்_PER_GROUP+2], lengths[வரிகள்_PER_GROUP+2];
    மாறிலி uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    திரும்பு expandName(names, s+offsets[வரி_எண்], lengths[வரி_எண்], nameChoice,
                      buffer, bufferLength);
}

நிலையான uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        எழுத்து *buffer, uint16_t bufferLength) {
    மாறிலி uint16_t *group=getGroup(names, code);
    எனில் (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        திரும்பு expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } இல்லையெனில் {
        /* group not found */
        /* zero-terminate */
        எனில்(bufferLength>0) {
            *buffer=0;
        }
        திரும்பு 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
நிலையான UBool
enumGroupNames(UCharNames *names, மாறிலி uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, வெறுமை *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[வரிகள்_PER_GROUP+2], lengths[வரிகள்_PER_GROUP+2];
    மாறிலி uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    எனில்(fn!=DO_FIND_NAME) {
        எழுத்து buffer[200];
        uint16_t length;

        போது(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, அளவு(buffer));
            எனில் (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, அளவு(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            எனில்(length>0) {
                எனில்(!fn(context, start, nameChoice, buffer, length)) {
                    திரும்பு பொய்;
                }
            }
            ++start;
        }
    } இல்லையெனில் {
        மாறிலி CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        போது(start<=end) {
            எனில்(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                திரும்பு பொய்;
            }
            ++start;
        }
    }
    திரும்பு உண்மை;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
நிலையான UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, வெறுமை *context)
{
    எனில்(fn!=DO_FIND_NAME) {
        எழுத்து buffer[200];
        uint16_t length;
        
        போது(start<=end) {
            buffer[length = getExtName(start, buffer, அளவு(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            எனில்(length>0) {
                எனில்(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    திரும்பு பொய்;
                }
            }
            ++start;
        }
    }

    திரும்பு உண்மை;
}

நிலையான UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, வெறுமை *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_மொத்தம்;
    மாறிலி uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    எனில்(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        எனில்(extLimit>limit) {
            extLimit=limit;
        }
        எனில்(!enumExtNames(start, extLimit-1, fn, context)) {
            திரும்பு பொய்;
        }
        start=extLimit;
    }

    எனில்(startGroupMSB==endGroupMSB) {
        எனில்(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            திரும்பு enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } இல்லையெனில் {
        மாறிலி uint16_t *groups=GET_GROUPS(names);
        group_மொத்தம்=*groups++;
        groupLimit=groups+group_மொத்தம்*GROUP_LENGTH;

        எனில்(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            எனில்((start&GROUP_MASK)!=0) {
                எனில்(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + வரிகள்_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    திரும்பு பொய்;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } இல்லையெனில் எனில்(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            மாறிலி uint16_t *nextGroup=NEXT_GROUP(group);
            எனில் (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                எனில் (end > limit) {
                    end = limit;
                }
                எனில் (!enumExtNames(start, end - 1, fn, context)) {
                    திரும்பு பொய்;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        போது(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            மாறிலி uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            எனில்(!enumGroupNames(names, group, start, start+வரிகள்_PER_GROUP-1, fn, context, nameChoice)) {
                திரும்பு பொய்;
            }
            nextGroup=NEXT_GROUP(group);
            எனில் (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                எனில் (end > limit) {
                    end = limit;
                }
                எனில் (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    திரும்பு பொய்;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        எனில்(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            திரும்பு enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } இல்லையெனில் எனில் (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            எனில் (next > start) {
                start = next;
            }
        } இல்லையெனில் {
            திரும்பு உண்மை;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    எனில் (nameChoice == U_EXTENDED_CHAR_NAME) {
        எனில் (limit > UCHAR_MAX_மதிப்பு + 1) {
            limit = UCHAR_MAX_மதிப்பு + 1;
        }
        திரும்பு enumExtNames(start, limit - 1, fn, context);
    }
    
    திரும்பு உண்மை;
}

நிலையான uint16_t
writeFactorSuffix(மாறிலி uint16_t *factors, uint16_t count,
                  மாறிலி எழுத்து *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  மாறிலி எழுத்து *elementBases[8], மாறிலி எழுத்து *elements[8],
                  எழுத்து *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    எழுத்து c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    க்கு(i=count; i>0; --i) {
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
    க்கு(;;) {
        எனில்(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        போது(factor>0) {
            போது(*s++!=0) {}
            --factor;
        }
        எனில்(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        போது((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        எனில்(i>=count) {
            நிறுத்து;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        போது(factor>0) {
            போது(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    எனில்(bufferLength>0) {
        *buffer=0;
    }

    திரும்பு bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
நிலையான uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        எழுத்து *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    எனில்(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        எனில்(bufferLength>0) {
            *buffer=0;
        }
        திரும்பு 0;
    }

    தேர்வு(range->type) {
    நிலை 0: {
        /* name = prefix hex-digits */
        மாறிலி எழுத்து* s = reinterpret_cast<மாறிலி எழுத்து*>(range + 1);
        எழுத்து c;

        uint16_t i, count;

        /* copy prefix */
        போது((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        எனில்(count<bufferLength) {
            buffer[count]=0;
        }

        க்கு(i=count; i>0;) {
            எனில்(--i<bufferLength) {
                c = static_cast<எழுத்து>(code & 0xf);
                எனில்(c<10) {
                    c+='0';
                } இல்லையெனில் {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        நிறுத்து;
    }
    நிலை 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        மாறிலி uint16_t* factors = reinterpret_cast<மாறிலி uint16_t*>(range + 1);
        uint16_t count=range->variant;
        மாறிலி எழுத்து* s = reinterpret_cast<மாறிலி எழுத்து*>(factors + count);
        எழுத்து c;

        /* copy prefix */
        போது((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        நிறுத்து;
    }
    இயல்பு:
        /* undefined type */
        /* zero-terminate */
        எனில்(bufferLength>0) {
            *buffer=0;
        }
        நிறுத்து;
    }

    திரும்பு bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
நிலையான UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, வெறுமை *context,
             UCharNameChoice nameChoice) {
    எழுத்து buffer[200];
    uint16_t length;

    எனில்(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        திரும்பு உண்மை;
    }

    தேர்வு(range->type) {
    நிலை 0: {
        எழுத்து *s, *end;
        எழுத்து c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, அளவு(buffer));
        எனில்(length<=0) {
            திரும்பு உண்மை;
        }

        /* call the enumerator function with this first character */
        எனில்(!fn(context, start, nameChoice, buffer, length)) {
            திரும்பு பொய்;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        போது(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        போது(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            க்கு (;;) {
                c=*--s;
                எனில்(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<எழுத்து>(c + 1);
                    நிறுத்து;
                } இல்லையெனில் எனில்(c=='9') {
                    *s='A';
                    நிறுத்து;
                } இல்லையெனில் எனில்(c=='F') {
                    *s='0';
                }
            }

            எனில்(!fn(context, start, nameChoice, buffer, length)) {
                திரும்பு பொய்;
            }
        }
        நிறுத்து;
    }
    நிலை 1: {
        uint16_t indexes[8];
        மாறிலி எழுத்து *elementBases[8], *elements[8];
        மாறிலி uint16_t* factors = reinterpret_cast<மாறிலி uint16_t*>(range + 1);
        uint16_t count=range->variant;
        மாறிலி எழுத்து* s = reinterpret_cast<மாறிலி எழுத்து*>(factors + count);
        எழுத்து *suffix, *t;
        uint16_t prefixLength, i, idx;

        எழுத்து c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        போது((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(அளவு(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        எனில்(!fn(context, start, nameChoice, buffer, length)) {
            திரும்பு பொய்;
        }

        /* enumerate the rest of the names */
        போது(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            க்கு (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                எனில்(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    போது(*s++!=0) {
                    }
                    elements[i]=s;
                    நிறுத்து;
                } இல்லையெனில் {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            க்கு(i=0; i<count; ++i) {
                s=elements[i];
                போது((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            எனில்(!fn(context, start, nameChoice, buffer, length)) {
                திரும்பு பொய்;
            }
        }
        நிறுத்து;
    }
    இயல்பு:
        /* undefined type */
        நிறுத்து;
    }

    திரும்பு உண்மை;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
நிலையான UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    எனில்(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        திரும்பு 0xffff;
    }

    தேர்வு(range->type) {
    நிலை 0: {
        /* name = prefix hex-digits */
        மாறிலி எழுத்து* s = reinterpret_cast<மாறிலி எழுத்து*>(range + 1);

        /* compare prefix */
        எனில் (!matcher.consistentWith(s)) {
            திரும்பு 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        க்கு(எழுத்து c : matcher.remainingSignificantCharacters()) {
            எனில்('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } இல்லையெனில் எனில்('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } இல்லையெனில் {
                திரும்பு 0xffff;
            }
        }

        /* does it fit into the range? */
        எனில் (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            திரும்பு code;
        }
        நிறுத்து;
    }
    நிலை 1: {
        எழுத்து buffer[64];
        uint16_t indexes[8];
        மாறிலி எழுத்து *elementBases[8], *elements[8];
        மாறிலி uint16_t* factors = reinterpret_cast<மாறிலி uint16_t*>(range + 1);
        uint16_t count=range->variant;
        மாறிலி எழுத்து *s = reinterpret_cast<மாறிலி எழுத்து*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        எனில் (!matcher.consistentWith(prefix)) {
            திரும்பு 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, அளவு(buffer));

        /* compare the first suffix */
        எனில்(querySuffix == buffer) {
            திரும்பு start;
        }

        /* enumerate and compare the rest of the suffixes */
        போது(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            க்கு (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                எனில்(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    போது(*s++!=0) {}
                    elements[i]=s;
                    நிறுத்து;
                } இல்லையெனில் {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            க்கு(i=0; i<count; ++i) {
                s=elements[i];
                எழுத்து c;
                போது((c=*s++)!=0) {
                    எனில்(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            எனில்(i<99 && it == querySuffix.end()) {
                திரும்பு start;
            }
        }
        நிறுத்து;
    }
    இயல்பு:
        /* undefined type */
        நிறுத்து;
    }

    திரும்பு 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

நிலையான int32_t
calcStringSetLength(uint32_t set[8], மாறிலி எழுத்து *s) {
    int32_t length=0;
    எழுத்து c;

    போது((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    திரும்பு length;
}

நிலையான int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_மொத்தம்;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_மொத்தம்=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    போது(range_மொத்தம்>0) {
        தேர்வு(range->type) {
        நிலை 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<மாறிலி எழுத்து*>(range + 1)) + range->variant;
            எனில்(length>maxNameLength) {
                maxNameLength=length;
            }
            நிறுத்து;
        நிலை 1: {
            /* name = prefix factorized-elements */
            மாறிலி uint16_t* factors = reinterpret_cast<மாறிலி uint16_t*>(range + 1);
            மாறிலி எழுத்து *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<மாறிலி எழுத்து*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            க்கு(i=0; i<count; ++i) {
                maxFactorLength=0;
                க்கு(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    எனில்(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            எனில்(length>maxNameLength) {
                maxNameLength=length;
            }
            நிறுத்து;
        }
        இயல்பு:
            /* unknown type */
            நிறுத்து;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_மொத்தம்;
    }
    திரும்பு maxNameLength;
}

நிலையான int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    க்கு(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        எனில்(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    திரும்பு maxNameLength;
}

நிலையான int32_t
calcNameSetLength(மாறிலி uint16_t *tokens, uint16_t token_மொத்தம், மாறிலி uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  மாறிலி uint8_t **p_வரி, மாறிலி uint8_t *வரி_Limit) {
    மாறிலி uint8_t *வரி=*p_வரி;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    போது (வரி != வரி_Limit && (c = *வரி++) != static_cast<uint8_t>(';')) {
        எனில்(c>=token_மொத்தம்) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } இல்லையெனில் {
            token=tokens[c];
            எனில் (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*வரி++;
                token=tokens[c];
            }
            எனில் (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } இல்லையெனில் {
                /* count token word */
                எனில்(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    எனில்(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<மாறிலி எழுத்து*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } இல்லையெனில் {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<மாறிலி எழுத்து*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_வரி=வரி;
    திரும்பு length;
}

நிலையான வெறுமை
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[வரிகள்_PER_GROUP+2], lengths[வரிகள்_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_மொத்தம்=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    மாறிலி uint16_t *group;
    மாறிலி uint8_t *s, *வரி, *வரி_Limit;

    int32_t group_மொத்தம், வரி_எண், length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_மொத்தம்));
    எனில்(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_மொத்தம்);
    }

    group=GET_GROUPS(uCharNames);
    group_மொத்தம்=*group++;

    /* enumerate all groups */
    போது(group_மொத்தம்>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        க்கு(வரி_எண்=0; வரி_எண்<வரிகள்_PER_GROUP; ++வரி_எண்) {
            வரி=s+offsets[வரி_எண்];
            length=lengths[வரி_எண்];
            எனில்(length==0) {
                தொடர்;
            }

            வரி_Limit=வரி+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_மொத்தம், tokenStrings, tokenLengths, gNameSet, &வரி, வரி_Limit);
            எனில்(length>maxNameLength) {
                maxNameLength=length;
            }
            எனில்(வரி==வரி_Limit) {
                தொடர்;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_மொத்தம், tokenStrings, tokenLengths, gNameSet, &வரி, வரி_Limit);
            எனில்(length>maxNameLength) {
                maxNameLength=length;
            }
            எனில்(வரி==வரி_Limit) {
                தொடர்;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_மொத்தம்;
    }

    எனில்(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

நிலையான UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    நிலையான மாறிலி எழுத்து extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    எனில்(gMaxNameLength!=0) {
        திரும்பு உண்மை;
    }

    எனில்(!isDataLoaded(pErrorCode)) {
        திரும்பு பொய்;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    க்கு (i = 0; i < static_cast<int32_t>(அளவு(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    திரும்பு உண்மை;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           எழுத்து *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    } இல்லையெனில் எனில்(nameChoice>=U_CHAR_NAME_CHOICE_மொத்தம் ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்((uint32_t)code>UCHAR_MAX_மதிப்பு || !isDataLoaded(pErrorCode)) {
        திரும்பு u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    போது(i>0) {
        எனில்(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            நிறுத்து;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    எனில்(i==0) {
        எனில் (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            எனில் (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } இல்லையெனில் {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    திரும்பு u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                எழுத்து *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    } இல்லையெனில் எனில்(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    திரும்பு u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               மாறிலி எழுத்து *மாறிலி name,
               UErrorCode *pErrorCode) {
    எழுத்து lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    எழுத்து c0;
    நிலையான constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு error;
    }

    எனில்(nameChoice>=U_CHAR_NAME_CHOICE_மொத்தம் || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு error;
    }

    எனில்(!isDataLoaded(pErrorCode)) {
        திரும்பு error;
    }

    /* construct the lowercase of the name first */
    மாறிலி எழுத்து* nameChar = name;
    க்கு(i=0; i<அளவு(lower); ++i) {
        எனில்((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } இல்லையெனில் {
            நிறுத்து;
        }
    }
    எனில்(i==அளவு(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        திரும்பு error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    எனில் (lower[0] == '<') {
        எனில் (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            போது (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            எனில் (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                க்கு (++i; i < limit; ++i) {
                    எனில் (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } இல்லையெனில் எனில் (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } இல்லையெனில் {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        திரும்பு error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    எனில் (cp > UCHAR_MAX_மதிப்பு) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        திரும்பு error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                க்கு (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    எனில் (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        எனில் (cat == cIdx) {
                            திரும்பு cp;
                        }
                        நிறுத்து;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        திரும்பு error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    போது(i>0) {
        எனில்((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            திரும்பு cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_மதிப்பு + 1, DO_FIND_NAME, &findName, nameChoice);
    எனில் (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    திரும்பு findName.code;
}

U_CAPI வெறுமை U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                வெறுமை *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    எனில்(nameChoice>=U_CHAR_NAME_CHOICE_மொத்தம் || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
    }

    எனில்((uint32_t) limit > UCHAR_MAX_மதிப்பு + 1) {
        limit = UCHAR_MAX_மதிப்பு + 1;
    }
    எனில்((uint32_t)start>=(uint32_t)limit) {
        திரும்பு;
    }

    எனில்(!isDataLoaded(pErrorCode)) {
        திரும்பு;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    போது(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        எனில்((uint32_t)start<algRange->start) {
            எனில்((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                திரும்பு;
            }
            எனில்(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                திரும்பு;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        எனில்((uint32_t)start<=algRange->end) {
            எனில்((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                திரும்பு;
            }
            எனில்(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                திரும்பு;
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
    எனில்(calcNameSetsLengths(&errorCode)) {
        திரும்பு gMaxNameLength;
    } இல்லையெனில் {
        திரும்பு 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
நிலையான வெறுமை
charSetToUSet(uint32_t cset[8], மாறிலி USetAdder *sa) {
    char16_t us[256];
    எழுத்து cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    எனில்(!calcNameSetsLengths(&errorCode)) {
        திரும்பு;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    க்கு(i=0; i<256; ++i) {
        எனில்(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<எழுத்து>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    க்கு(i=0; i<length; ++i) {
        எனில்(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI வெறுமை U_EXPORT2
uprv_getCharNameCharacters(மாறிலி USetAdder *sa) {
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
நிலையான வெறுமை
makeTokenMap(மாறிலி UDataSwapper *ds,
             int16_t tokens[], uint16_t token_மொத்தம்,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    எனில்(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        க்கு(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } இல்லையெனில் {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        எனில்(token_மொத்தம்>256) {
            token_மொத்தம்=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        க்கு(i=1; i<token_மொத்தம்; ++i) {
            எனில்(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                எனில்(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    திரும்பு;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=உண்மை;
            }
        }

        /* set the mappings for the rest of the permutation */
        க்கு(i=j=1; i<token_மொத்தம்; ++i) {
            /* set mappings that were not set for direct bytes */
            எனில்(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                போது(usedOutChar[j]) {
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
uchar_swapNames(மாறிலி UDataSwapper *ds,
                மாறிலி வெறுமை *inData, int32_t length, வெறுமை *outData,
                UErrorCode *pErrorCode) {
    மாறிலி UDataInfo *pInfo;
    int32_t headerSize;

    மாறிலி uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_மொத்தம்;

    மாறிலி AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    எனில்(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }

    /* check data format and format version */
    pInfo=(மாறிலி UDataInfo *)((மாறிலி எழுத்து *)inData+4);
    எனில்(!(
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
        திரும்பு 0;
    }

    inBytes=(மாறிலி uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    எனில்(length<0) {
        algNamesOffset=ds->readUInt32(((மாறிலி uint32_t *)inBytes)[3]);
    } இல்லையெனில் {
        length-=headerSize;
        எனில்( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((மாறிலி uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            திரும்பு 0;
        }
    }

    எனில்(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((மாறிலி uint32_t *)(inBytes+offset)));
        offset+=4;

        க்கு(i=0; i<count; ++i) {
            inRange=(மாறிலி AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } இல்லையெனில் {
        /* swap data */
        மாறிலி uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_மொத்தம்;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        எனில்(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((மாறிலி uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((மாறிலி uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((மாறிலி uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(மாறிலி uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_மொத்தம்=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        எனில்(token_மொத்தம்<=512) {
            count=token_மொத்தம்;
        } இல்லையெனில் {
            count=512;
        }
        க்கு(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        க்கு(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_மொத்தம், map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_மொத்தம்>256 ? token_மொத்தம்-256 : 0), trailMap, pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            திரும்பு 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_மொத்தம்*2);
        எனில்(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_மொத்தம்);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            திரும்பு 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        க்கு(i=0; i<token_மொத்தம் && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        க்கு(; i<token_மொத்தம்; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_மொத்தம்*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            திரும்பு 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((மாறிலி uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        எனில்(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[வரிகள்_PER_GROUP+1], lengths[வரிகள்_PER_GROUP+1];

            மாறிலி uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_மொத்தம்=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            போது(strings_மொத்தம்>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_மொத்தம்-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_மொத்தம்-=count;

                /* swap the string bytes using map[] and trailMap[] */
                போது(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    எனில்(tokens[c]!=-2) {
                        --count;
                    } இல்லையெனில் {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((மாறிலி uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        க்கு(i=0; i<count; ++i) {
            எனில்(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                திரும்பு 0;
            }

            inRange=(மாறிலி AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            தேர்வு(inRange->type) {
            நிலை 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((மாறிலி எழுத்து *)(inRange+1)),
                                    outRange+1, pErrorCode);
                எனில்(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    திரும்பு 0;
                }
                நிறுத்து;
            நிலை 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_மொத்தம்;

                    factors_மொத்தம்=inRange->variant;
                    p=(மாறிலி uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_மொத்தம்*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_மொத்தம்;
                    q+=factors_மொத்தம்;
                    strings_மொத்தம்=(uint32_t)((inBytes+offset)-(மாறிலி uint8_t *)p);
                    போது(strings_மொத்தம்>0 && ((மாறிலி uint8_t *)p)[strings_மொத்தம்-1]!=0) {
                        --strings_மொத்தம்;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_மொத்தம், q, pErrorCode);
                }
                நிறுத்து;
            இயல்பு:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                திரும்பு 0;
            }
        }
    }

    திரும்பு headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
