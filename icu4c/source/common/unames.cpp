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

#شامل "unicode/utypes.h"
#شامل "unicode/putil.h"
#شامل "unicode/uchar.h"
#شامل "unicode/udata.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "uassert.h"
#شامل "ustr_imp.h"
#شامل "umutex.h"
#شامل "cmemory.h"
#شامل "cstring.h"
#شامل "ucln_cmn.h"
#شامل "udataswp.h"
#شامل "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

ایستا ثابت نویسه DATA_NAME[] = "unames";
ایستا ثابت نویسه DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define سطرها_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (سطرها_PER_GROUP-1)

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
شمارش {
    GROUP_MSB,
    GROUP_OFFSET_بالا,
    GROUP_OFFSET_کم,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_بالا]<<16|(group)[GROUP_OFFSET_کم])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

تعریفنوع ساختار {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعریفنوع ساختار {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

فضاینام {
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
رده CharacterNameQuery {
  عمومی:
    رده Matcher {
      عمومی:
        // `query` must outlive the constructed object.
        Matcher(ثابت CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        منطقی consistentWith(نویسه c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            اگر (skeletonIterator == query.skeleton.end()) {
                بازگشت نادرست;
            }
            اگر (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                اگر (lastChar == '-') {
                    اگر (*skeletonIterator++ != lastChar) {
                        بازگشت نادرست;
                    }
                }
                lastChar = c;
                بازگشت درست;
            } وگرنه اگر (c == '-') {
                اگر (lastChar == ' ' ||
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
                    اگر (*skeletonIterator++ != c) {
                        بازگشت نادرست;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                بازگشت درست;
            } وگرنه {
                اگر (*skeletonIterator++ != c) {
                    بازگشت نادرست;
                }
                lastChar = c;
                بازگشت درست;
            }
        }

        منطقی consistentWith(ثابت std::string_view substring) {
            برای (ثابت نویسه c : substring) {
                اگر (!consistentWith(c)) {
                    بازگشت نادرست;
                }
            }
            بازگشت درست;
        }

        منطقی matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            بازگشت skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() ثابت {
            بازگشت query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      خصوصی:
        ثابت CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        نویسه lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        نویسه *skeletonLimit = skeletonData;
        برای (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + اندازه(skeletonData));
            اگر (skeletonLimit >= skeletonData + اندازه(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                بشکن;
            }
            اگر (query[i] == ' ' || query[i] == '_') {
                ادامه;
            }
            اگر (query[i] == '-') {
                منطقی isMedial;
                منطقی is1180MedialHyphen = نادرست;
                اگر (i == 0 || i == query.length() - 1) {
                    isMedial = نادرست;
                } وگرنه {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                اگر (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = درست;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    برای (std::size_t j = i + 2; j < query.length(); ++j) {
                        اگر (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = نادرست;
                        }
                    }
                }
                اگر (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } وگرنه {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ثابت {
        بازگشت Matcher(*این);
    }

خصوصی:
    ایستا منطقی isASCIILetterOrDigit(ثابت نویسه c) {
        بازگشت uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    نویسه skeletonData[120];
    std::string_view skeleton;
    منطقی is1180;
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
#define GET_GROUPS(names) (ثابت uint16_t *)((ثابت نویسه *)names+names->groupsOffset)

تعریفنوع ساختار {
    ثابت CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

ایستا UDataMemory *uCharNamesData=nullptr;
ایستا UCharNames *uCharNames=nullptr;
ایستا icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ایستا int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ایستا uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_شمار
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_شمار + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_شمار + 2

#define U_CHAR_EXTENDED_CATEGORY_شمار (U_CHAR_CATEGORY_شمار + 3)

ایستا ثابت نویسه * ثابت charCatNames[U_CHAR_EXTENDED_CATEGORY_شمار] = {
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

ایستا UBool U_CALLCONV unames_cleanup()
{
    اگر(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    اگر(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    بازگشت درست;
}

ایستا UBool U_CALLCONV
isAcceptable(تهی * /*context*/,
             ثابت نویسه * /*type*/, ثابت نویسه * /*name*/,
             ثابت UDataInfo *pInfo) {
    بازگشت
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

ایستا تهی U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    اگر(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } وگرنه {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ایستا UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    بازگشت U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    اگر((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_شمار

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
ایستا uint16_t
expandName(UCharNames *names,
           ثابت uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           نویسه *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_شمار=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        اگر (static_cast<uint8_t>(';') >= token_شمار || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحیح fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            انجامبده {
                وقتی(nameLength>0) {
                    --nameLength;
                    اگر(*name++==';') {
                        بشکن;
                    }
                }
            } وقتی(--fieldIndex>0);
        } وگرنه {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    وقتی(nameLength>0) {
        --nameLength;
        c=*name++;

        اگر(c>=token_شمار) {
            اگر(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } وگرنه {
                /* finished */
                بشکن;
            }
        } وگرنه {
            token=tokens[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                اگر(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } وگرنه {
                    /* finished */
                    بشکن;
                }
            } وگرنه {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                وقتی((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    اگر(bufferLength>0) {
        *buffer=0;
    }

    بازگشت bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ایستا UBool
compareName(UCharNames *names,
            ثابت uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_شمار=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        اگر (static_cast<uint8_t>(';') >= token_شمار || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحیح fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            انجامبده {
                وقتی(nameLength>0) {
                    --nameLength;
                    اگر(*name++==';') {
                        بشکن;
                    }
                }
            } وقتی(--fieldIndex>0);
        } وگرنه {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    وقتی(nameLength>0) {
        --nameLength;
        c=*name++;

        اگر(c>=token_شمار) {
            اگر(c!=';') {
                /* implicit letter */
                اگر (!matcher.consistentWith(c)) {
                    بازگشت نادرست;
                }
            } وگرنه {
                /* finished */
                بشکن;
            }
        } وگرنه {
            token=tokens[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                اگر(c!=';') {
                    /* explicit letter */
                    اگر (!matcher.consistentWith(c)) {
                        بازگشت نادرست;
                    }
                } وگرنه {
                    /* finished */
                    بشکن;
                }
            } وگرنه {
                /* write token word */
                اگر (!matcher.consistentWith(reinterpret_cast<ثابت نویسه *>(tokenStrings + token))) {
                    بازگشت نادرست;
                }
            }
        }
    }

    /* complete match? */
    بازگشت matcher.matches();
}

ایستا uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    اگر (U_IS_UNICODE_NONCHAR(cp)) {
        بازگشت U_NONCHARACTER_CODE_POINT;
    }

    اگر ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    بازگشت cat;
}

ایستا ثابت نویسه *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    اگر (cat >= UPRV_LENGTHOF(charCatNames)) {
        بازگشت "unknown";
    } وگرنه {
        بازگشت charCatNames[cat];
    }
}

ایستا uint16_t getExtName(uint32_t code, نویسه *buffer, uint16_t bufferLength) {
    ثابت نویسه *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    صحیح ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    وقتی (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    برای (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    اگر (ndigits < 4)
        ndigits = 4;
    برای (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    بازگشت length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ایستا ثابت uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    ثابت uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    وقتی(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        اگر(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } وگرنه {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    بازگشت groups+start*GROUP_LENGTH;
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
ایستا ثابت uint8_t *
expandGroupLengths(ثابت uint8_t *s,
                   uint16_t offsets[سطرها_PER_GROUP+1], uint16_t lengths[سطرها_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    وقتی(i<سطرها_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        اگر(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } وگرنه اگر((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } وگرنه {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        اگر((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            اگر(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } وگرنه {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    بازگشت s;
}

ایستا uint16_t
expandGroupName(UCharNames *names, ثابت uint16_t *group,
                uint16_t سطر_عدد, UCharNameChoice nameChoice,
                نویسه *buffer, uint16_t bufferLength) {
    uint16_t offsets[سطرها_PER_GROUP+2], lengths[سطرها_PER_GROUP+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    بازگشت expandName(names, s+offsets[سطر_عدد], lengths[سطر_عدد], nameChoice,
                      buffer, bufferLength);
}

ایستا uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        نویسه *buffer, uint16_t bufferLength) {
    ثابت uint16_t *group=getGroup(names, code);
    اگر (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        بازگشت expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } وگرنه {
        /* group not found */
        /* zero-terminate */
        اگر(bufferLength>0) {
            *buffer=0;
        }
        بازگشت 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ایستا UBool
enumGroupNames(UCharNames *names, ثابت uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, تهی *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[سطرها_PER_GROUP+2], lengths[سطرها_PER_GROUP+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    اگر(fn!=DO_FIND_NAME) {
        نویسه buffer[200];
        uint16_t length;

        وقتی(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, اندازه(buffer));
            اگر (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, اندازه(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            اگر(length>0) {
                اگر(!fn(context, start, nameChoice, buffer, length)) {
                    بازگشت نادرست;
                }
            }
            ++start;
        }
    } وگرنه {
        ثابت CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        وقتی(start<=end) {
            اگر(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                بازگشت نادرست;
            }
            ++start;
        }
    }
    بازگشت درست;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ایستا UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, تهی *context)
{
    اگر(fn!=DO_FIND_NAME) {
        نویسه buffer[200];
        uint16_t length;
        
        وقتی(start<=end) {
            buffer[length = getExtName(start, buffer, اندازه(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            اگر(length>0) {
                اگر(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    بازگشت نادرست;
                }
            }
            ++start;
        }
    }

    بازگشت درست;
}

ایستا UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, تهی *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_شمار;
    ثابت uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    اگر(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        اگر(extLimit>limit) {
            extLimit=limit;
        }
        اگر(!enumExtNames(start, extLimit-1, fn, context)) {
            بازگشت نادرست;
        }
        start=extLimit;
    }

    اگر(startGroupMSB==endGroupMSB) {
        اگر(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            بازگشت enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } وگرنه {
        ثابت uint16_t *groups=GET_GROUPS(names);
        group_شمار=*groups++;
        groupLimit=groups+group_شمار*GROUP_LENGTH;

        اگر(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            اگر((start&GROUP_MASK)!=0) {
                اگر(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + سطرها_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    بازگشت نادرست;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } وگرنه اگر(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *nextGroup=NEXT_GROUP(group);
            اگر (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                اگر (end > limit) {
                    end = limit;
                }
                اگر (!enumExtNames(start, end - 1, fn, context)) {
                    بازگشت نادرست;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        وقتی(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            ثابت uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            اگر(!enumGroupNames(names, group, start, start+سطرها_PER_GROUP-1, fn, context, nameChoice)) {
                بازگشت نادرست;
            }
            nextGroup=NEXT_GROUP(group);
            اگر (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                اگر (end > limit) {
                    end = limit;
                }
                اگر (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    بازگشت نادرست;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        اگر(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            بازگشت enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } وگرنه اگر (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            اگر (next > start) {
                start = next;
            }
        } وگرنه {
            بازگشت درست;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    اگر (nameChoice == U_EXTENDED_CHAR_NAME) {
        اگر (limit > UCHAR_MAX_مقدار + 1) {
            limit = UCHAR_MAX_مقدار + 1;
        }
        بازگشت enumExtNames(start, limit - 1, fn, context);
    }
    
    بازگشت درست;
}

ایستا uint16_t
writeFactorSuffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت نویسه *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت نویسه *elementBases[8], ثابت نویسه *elements[8],
                  نویسه *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    نویسه c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    برای(i=count; i>0; --i) {
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
    برای(;;) {
        اگر(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        وقتی(factor>0) {
            وقتی(*s++!=0) {}
            --factor;
        }
        اگر(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        وقتی((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        اگر(i>=count) {
            بشکن;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        وقتی(factor>0) {
            وقتی(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    اگر(bufferLength>0) {
        *buffer=0;
    }

    بازگشت bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ایستا uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        نویسه *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        اگر(bufferLength>0) {
            *buffer=0;
        }
        بازگشت 0;
    }

    انتخاب(range->type) {
    حالت 0: {
        /* name = prefix hex-digits */
        ثابت نویسه* s = reinterpret_cast<ثابت نویسه*>(range + 1);
        نویسه c;

        uint16_t i, count;

        /* copy prefix */
        وقتی((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        اگر(count<bufferLength) {
            buffer[count]=0;
        }

        برای(i=count; i>0;) {
            اگر(--i<bufferLength) {
                c = static_cast<نویسه>(code & 0xf);
                اگر(c<10) {
                    c+='0';
                } وگرنه {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        بشکن;
    }
    حالت 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت نویسه* s = reinterpret_cast<ثابت نویسه*>(factors + count);
        نویسه c;

        /* copy prefix */
        وقتی((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        بشکن;
    }
    پیشفرض:
        /* undefined type */
        /* zero-terminate */
        اگر(bufferLength>0) {
            *buffer=0;
        }
        بشکن;
    }

    بازگشت bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ایستا UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, تهی *context,
             UCharNameChoice nameChoice) {
    نویسه buffer[200];
    uint16_t length;

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        بازگشت درست;
    }

    انتخاب(range->type) {
    حالت 0: {
        نویسه *s, *end;
        نویسه c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, اندازه(buffer));
        اگر(length<=0) {
            بازگشت درست;
        }

        /* call the enumerator function with this first character */
        اگر(!fn(context, start, nameChoice, buffer, length)) {
            بازگشت نادرست;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        وقتی(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        وقتی(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            برای (;;) {
                c=*--s;
                اگر(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<نویسه>(c + 1);
                    بشکن;
                } وگرنه اگر(c=='9') {
                    *s='A';
                    بشکن;
                } وگرنه اگر(c=='F') {
                    *s='0';
                }
            }

            اگر(!fn(context, start, nameChoice, buffer, length)) {
                بازگشت نادرست;
            }
        }
        بشکن;
    }
    حالت 1: {
        uint16_t indexes[8];
        ثابت نویسه *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت نویسه* s = reinterpret_cast<ثابت نویسه*>(factors + count);
        نویسه *suffix, *t;
        uint16_t prefixLength, i, idx;

        نویسه c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        وقتی((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(اندازه(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        اگر(!fn(context, start, nameChoice, buffer, length)) {
            بازگشت نادرست;
        }

        /* enumerate the rest of the names */
        وقتی(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            برای (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                اگر(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    وقتی(*s++!=0) {
                    }
                    elements[i]=s;
                    بشکن;
                } وگرنه {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            برای(i=0; i<count; ++i) {
                s=elements[i];
                وقتی((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            اگر(!fn(context, start, nameChoice, buffer, length)) {
                بازگشت نادرست;
            }
        }
        بشکن;
    }
    پیشفرض:
        /* undefined type */
        بشکن;
    }

    بازگشت درست;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ایستا UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        بازگشت 0xffff;
    }

    انتخاب(range->type) {
    حالت 0: {
        /* name = prefix hex-digits */
        ثابت نویسه* s = reinterpret_cast<ثابت نویسه*>(range + 1);

        /* compare prefix */
        اگر (!matcher.consistentWith(s)) {
            بازگشت 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        برای(نویسه c : matcher.remainingSignificantCharacters()) {
            اگر('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } وگرنه اگر('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } وگرنه {
                بازگشت 0xffff;
            }
        }

        /* does it fit into the range? */
        اگر (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            بازگشت code;
        }
        بشکن;
    }
    حالت 1: {
        نویسه buffer[64];
        uint16_t indexes[8];
        ثابت نویسه *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت نویسه *s = reinterpret_cast<ثابت نویسه*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        اگر (!matcher.consistentWith(prefix)) {
            بازگشت 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, اندازه(buffer));

        /* compare the first suffix */
        اگر(querySuffix == buffer) {
            بازگشت start;
        }

        /* enumerate and compare the rest of the suffixes */
        وقتی(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            برای (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                اگر(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    وقتی(*s++!=0) {}
                    elements[i]=s;
                    بشکن;
                } وگرنه {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            برای(i=0; i<count; ++i) {
                s=elements[i];
                نویسه c;
                وقتی((c=*s++)!=0) {
                    اگر(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            اگر(i<99 && it == querySuffix.end()) {
                بازگشت start;
            }
        }
        بشکن;
    }
    پیشفرض:
        /* undefined type */
        بشکن;
    }

    بازگشت 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ایستا int32_t
calcStringSetLength(uint32_t set[8], ثابت نویسه *s) {
    int32_t length=0;
    نویسه c;

    وقتی((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    بازگشت length;
}

ایستا int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_شمار;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_شمار=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    وقتی(range_شمار>0) {
        انتخاب(range->type) {
        حالت 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<ثابت نویسه*>(range + 1)) + range->variant;
            اگر(length>maxNameLength) {
                maxNameLength=length;
            }
            بشکن;
        حالت 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت نویسه *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<ثابت نویسه*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            برای(i=0; i<count; ++i) {
                maxFactorLength=0;
                برای(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    اگر(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            اگر(length>maxNameLength) {
                maxNameLength=length;
            }
            بشکن;
        }
        پیشفرض:
            /* unknown type */
            بشکن;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_شمار;
    }
    بازگشت maxNameLength;
}

ایستا int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    برای(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        اگر(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    بازگشت maxNameLength;
}

ایستا int32_t
calcNameSetLength(ثابت uint16_t *tokens, uint16_t token_شمار, ثابت uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  ثابت uint8_t **p_سطر, ثابت uint8_t *سطر_Limit) {
    ثابت uint8_t *سطر=*p_سطر;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    وقتی (سطر != سطر_Limit && (c = *سطر++) != static_cast<uint8_t>(';')) {
        اگر(c>=token_شمار) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } وگرنه {
            token=tokens[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*سطر++;
                token=tokens[c];
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } وگرنه {
                /* count token word */
                اگر(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    اگر(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<ثابت نویسه*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } وگرنه {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<ثابت نویسه*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_سطر=سطر;
    بازگشت length;
}

ایستا تهی
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[سطرها_PER_GROUP+2], lengths[سطرها_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_شمار=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    ثابت uint16_t *group;
    ثابت uint8_t *s, *سطر, *سطر_Limit;

    int32_t group_شمار, سطر_عدد, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_شمار));
    اگر(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_شمار);
    }

    group=GET_GROUPS(uCharNames);
    group_شمار=*group++;

    /* enumerate all groups */
    وقتی(group_شمار>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        برای(سطر_عدد=0; سطر_عدد<سطرها_PER_GROUP; ++سطر_عدد) {
            سطر=s+offsets[سطر_عدد];
            length=lengths[سطر_عدد];
            اگر(length==0) {
                ادامه;
            }

            سطر_Limit=سطر+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_شمار, tokenStrings, tokenLengths, gNameSet, &سطر, سطر_Limit);
            اگر(length>maxNameLength) {
                maxNameLength=length;
            }
            اگر(سطر==سطر_Limit) {
                ادامه;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_شمار, tokenStrings, tokenLengths, gNameSet, &سطر, سطر_Limit);
            اگر(length>maxNameLength) {
                maxNameLength=length;
            }
            اگر(سطر==سطر_Limit) {
                ادامه;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_شمار;
    }

    اگر(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

ایستا UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    ایستا ثابت نویسه extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    اگر(gMaxNameLength!=0) {
        بازگشت درست;
    }

    اگر(!isDataLoaded(pErrorCode)) {
        بازگشت نادرست;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    برای (i = 0; i < static_cast<int32_t>(اندازه(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    بازگشت درست;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           نویسه *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    } وگرنه اگر(nameChoice>=U_CHAR_NAME_CHOICE_شمار ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    اگر((uint32_t)code>UCHAR_MAX_مقدار || !isDataLoaded(pErrorCode)) {
        بازگشت u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    وقتی(i>0) {
        اگر(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            بشکن;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    اگر(i==0) {
        اگر (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            اگر (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } وگرنه {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    بازگشت u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                نویسه *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    } وگرنه اگر(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت 0;
    }

    بازگشت u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت نویسه *ثابت name,
               UErrorCode *pErrorCode) {
    نویسه lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    نویسه c0;
    ایستا constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت error;
    }

    اگر(nameChoice>=U_CHAR_NAME_CHOICE_شمار || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت error;
    }

    اگر(!isDataLoaded(pErrorCode)) {
        بازگشت error;
    }

    /* construct the lowercase of the name first */
    ثابت نویسه* nameChar = name;
    برای(i=0; i<اندازه(lower); ++i) {
        اگر((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } وگرنه {
            بشکن;
        }
    }
    اگر(i==اندازه(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        بازگشت error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    اگر (lower[0] == '<') {
        اگر (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            وقتی (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            اگر (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                برای (++i; i < limit; ++i) {
                    اگر (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } وگرنه اگر (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } وگرنه {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        بازگشت error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    اگر (cp > UCHAR_MAX_مقدار) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        بازگشت error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                برای (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    اگر (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        اگر (cat == cIdx) {
                            بازگشت cp;
                        }
                        بشکن;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        بازگشت error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    وقتی(i>0) {
        اگر((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            بازگشت cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_مقدار + 1, DO_FIND_NAME, &findName, nameChoice);
    اگر (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    بازگشت findName.code;
}

U_CAPI تهی U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                تهی *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    اگر(nameChoice>=U_CHAR_NAME_CHOICE_شمار || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
    }

    اگر((uint32_t) limit > UCHAR_MAX_مقدار + 1) {
        limit = UCHAR_MAX_مقدار + 1;
    }
    اگر((uint32_t)start>=(uint32_t)limit) {
        بازگشت;
    }

    اگر(!isDataLoaded(pErrorCode)) {
        بازگشت;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    وقتی(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        اگر((uint32_t)start<algRange->start) {
            اگر((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                بازگشت;
            }
            اگر(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                بازگشت;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        اگر((uint32_t)start<=algRange->end) {
            اگر((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                بازگشت;
            }
            اگر(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                بازگشت;
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
    اگر(calcNameSetsLengths(&errorCode)) {
        بازگشت gMaxNameLength;
    } وگرنه {
        بازگشت 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ایستا تهی
charSetToUSet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    نویسه cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    اگر(!calcNameSetsLengths(&errorCode)) {
        بازگشت;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    برای(i=0; i<256; ++i) {
        اگر(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<نویسه>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    برای(i=0; i<length; ++i) {
        اگر(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI تهی U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
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
ایستا تهی
makeTokenMap(ثابت UDataSwapper *ds,
             int16_t tokens[], uint16_t token_شمار,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    اگر(U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    اگر(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        برای(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وگرنه {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        اگر(token_شمار>256) {
            token_شمار=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        برای(i=1; i<token_شمار; ++i) {
            اگر(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                اگر(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    بازگشت;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=درست;
            }
        }

        /* set the mappings for the rest of the permutation */
        برای(i=j=1; i<token_شمار; ++i) {
            /* set mappings that were not set for direct bytes */
            اگر(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                وقتی(usedOutChar[j]) {
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
uchar_swapNames(ثابت UDataSwapper *ds,
                ثابت تهی *inData, int32_t length, تهی *outData,
                UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;

    ثابت uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_شمار;

    ثابت AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    اگر(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت نویسه *)inData+4);
    اگر(!(
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
        بازگشت 0;
    }

    inBytes=(ثابت uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    اگر(length<0) {
        algNamesOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[3]);
    } وگرنه {
        length-=headerSize;
        اگر( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            بازگشت 0;
        }
    }

    اگر(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ثابت uint32_t *)(inBytes+offset)));
        offset+=4;

        برای(i=0; i<count; ++i) {
            inRange=(ثابت AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } وگرنه {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_شمار;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        اگر(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_شمار=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        اگر(token_شمار<=512) {
            count=token_شمار;
        } وگرنه {
            count=512;
        }
        برای(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        برای(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_شمار, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_شمار>256 ? token_شمار-256 : 0), trailMap, pErrorCode);
        اگر(U_FAILURE(*pErrorCode)) {
            بازگشت 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_شمار*2);
        اگر(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_شمار);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            بازگشت 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        برای(i=0; i<token_شمار && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        برای(; i<token_شمار; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_شمار*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        اگر(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            بازگشت 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        اگر(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[سطرها_PER_GROUP+1], lengths[سطرها_PER_GROUP+1];

            ثابت uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_شمار=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            وقتی(strings_شمار>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_شمار-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_شمار-=count;

                /* swap the string bytes using map[] and trailMap[] */
                وقتی(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    اگر(tokens[c]!=-2) {
                        --count;
                    } وگرنه {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ثابت uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        برای(i=0; i<count; ++i) {
            اگر(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                بازگشت 0;
            }

            inRange=(ثابت AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            انتخاب(inRange->type) {
            حالت 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((ثابت نویسه *)(inRange+1)),
                                    outRange+1, pErrorCode);
                اگر(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    بازگشت 0;
                }
                بشکن;
            حالت 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_شمار;

                    factors_شمار=inRange->variant;
                    p=(ثابت uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_شمار*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_شمار;
                    q+=factors_شمار;
                    strings_شمار=(uint32_t)((inBytes+offset)-(ثابت uint8_t *)p);
                    وقتی(strings_شمار>0 && ((ثابت uint8_t *)p)[strings_شمار-1]!=0) {
                        --strings_شمار;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_شمار, q, pErrorCode);
                }
                بشکن;
            پیشفرض:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                بازگشت 0;
            }
        }
    }

    بازگشت headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
