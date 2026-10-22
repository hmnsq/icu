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

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "unicode/uchar.h"
#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "ucln_cmn.h"
#अन्तर्भाव "udataswp.h"
#अन्तर्भाव "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

स्थिर ध्रुव वर्ण DATA_NAME[] = "unames";
स्थिर ध्रुव वर्ण DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define पङ्क्तयः_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (पङ्क्तयः_PER_GROUP-1)

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
गणना {
    GROUP_MSB,
    GROUP_OFFSET_उच्चः,
    GROUP_OFFSET_नीचः,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_उच्चः]<<16|(group)[GROUP_OFFSET_नीचः])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

प्रकारनिर्देश संरचना {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

प्रकारनिर्देश संरचना {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

नामाकाश {
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
वर्ग CharacterNameQuery {
  सार्वजनिक:
    वर्ग Matcher {
      सार्वजनिक:
        // `query` must outlive the constructed object.
        Matcher(ध्रुव CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        तार्किक consistentWith(वर्ण c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            जइ (skeletonIterator == query.skeleton.end()) {
                प्रत्यागम मिच्छ;
            }
            जइ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                जइ (lastChar == '-') {
                    जइ (*skeletonIterator++ != lastChar) {
                        प्रत्यागम मिच्छ;
                    }
                }
                lastChar = c;
                प्रत्यागम सच्च;
            } अण्णहा जइ (c == '-') {
                जइ (lastChar == ' ' ||
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
                    जइ (*skeletonIterator++ != c) {
                        प्रत्यागम मिच्छ;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                प्रत्यागम सच्च;
            } अण्णहा {
                जइ (*skeletonIterator++ != c) {
                    प्रत्यागम मिच्छ;
                }
                lastChar = c;
                प्रत्यागम सच्च;
            }
        }

        तार्किक consistentWith(ध्रुव std::string_view substring) {
            प्रति (ध्रुव वर्ण c : substring) {
                जइ (!consistentWith(c)) {
                    प्रत्यागम मिच्छ;
                }
            }
            प्रत्यागम सच्च;
        }

        तार्किक matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            प्रत्यागम skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() ध्रुव {
            प्रत्यागम query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      गुप्त:
        ध्रुव CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        वर्ण lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        वर्ण *skeletonLimit = skeletonData;
        प्रति (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + परिमाण(skeletonData));
            जइ (skeletonLimit >= skeletonData + परिमाण(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                भङ्ग;
            }
            जइ (query[i] == ' ' || query[i] == '_') {
                अनुवर्त;
            }
            जइ (query[i] == '-') {
                तार्किक isMedial;
                तार्किक is1180MedialHyphen = मिच्छ;
                जइ (i == 0 || i == query.length() - 1) {
                    isMedial = मिच्छ;
                } अण्णहा {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                जइ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = सच्च;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    प्रति (std::size_t j = i + 2; j < query.length(); ++j) {
                        जइ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = मिच्छ;
                        }
                    }
                }
                जइ (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } अण्णहा {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ध्रुव {
        प्रत्यागम Matcher(*एतत्);
    }

गुप्त:
    स्थिर तार्किक isASCIILetterOrDigit(ध्रुव वर्ण c) {
        प्रत्यागम uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    वर्ण skeletonData[120];
    std::string_view skeleton;
    तार्किक is1180;
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
#define GET_GROUPS(names) (ध्रुव uint16_t *)((ध्रुव वर्ण *)names+names->groupsOffset)

प्रकारनिर्देश संरचना {
    ध्रुव CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

स्थिर UDataMemory *uCharNamesData=nullptr;
स्थिर UCharNames *uCharNames=nullptr;
स्थिर icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
स्थिर int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
स्थिर uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_गणनम्
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_गणनम् + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_गणनम् + 2

#define U_CHAR_EXTENDED_CATEGORY_गणनम् (U_CHAR_CATEGORY_गणनम् + 3)

स्थिर ध्रुव वर्ण * ध्रुव charCatNames[U_CHAR_EXTENDED_CATEGORY_गणनम्] = {
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

स्थिर UBool U_CALLCONV unames_cleanup()
{
    जइ(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    जइ(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    प्रत्यागम सच्च;
}

स्थिर UBool U_CALLCONV
isAcceptable(शून्य * /*context*/,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    प्रत्यागम
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

स्थिर शून्य U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    जइ(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } अण्णहा {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


स्थिर UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    प्रत्यागम U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    जइ((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_गणनम्

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
स्थिर uint16_t
expandName(UCharNames *names,
           ध्रुव uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           वर्ण *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_गणनम्=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    जइ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        जइ (static_cast<uint8_t>(';') >= token_गणनम् || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णाङ्क fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            करेहि {
                जाव(nameLength>0) {
                    --nameLength;
                    जइ(*name++==';') {
                        भङ्ग;
                    }
                }
            } जाव(--fieldIndex>0);
        } अण्णहा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    जाव(nameLength>0) {
        --nameLength;
        c=*name++;

        जइ(c>=token_गणनम्) {
            जइ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } अण्णहा {
                /* finished */
                भङ्ग;
            }
        } अण्णहा {
            token=tokens[c];
            जइ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            जइ (token == static_cast<uint16_t>(-1)) {
                जइ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } अण्णहा {
                    /* finished */
                    भङ्ग;
                }
            } अण्णहा {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                जाव((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    जइ(bufferLength>0) {
        *buffer=0;
    }

    प्रत्यागम bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
स्थिर UBool
compareName(UCharNames *names,
            ध्रुव uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            ध्रुव CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_गणनम्=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    जइ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        जइ (static_cast<uint8_t>(';') >= token_गणनम् || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णाङ्क fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            करेहि {
                जाव(nameLength>0) {
                    --nameLength;
                    जइ(*name++==';') {
                        भङ्ग;
                    }
                }
            } जाव(--fieldIndex>0);
        } अण्णहा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    जाव(nameLength>0) {
        --nameLength;
        c=*name++;

        जइ(c>=token_गणनम्) {
            जइ(c!=';') {
                /* implicit letter */
                जइ (!matcher.consistentWith(c)) {
                    प्रत्यागम मिच्छ;
                }
            } अण्णहा {
                /* finished */
                भङ्ग;
            }
        } अण्णहा {
            token=tokens[c];
            जइ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            जइ (token == static_cast<uint16_t>(-1)) {
                जइ(c!=';') {
                    /* explicit letter */
                    जइ (!matcher.consistentWith(c)) {
                        प्रत्यागम मिच्छ;
                    }
                } अण्णहा {
                    /* finished */
                    भङ्ग;
                }
            } अण्णहा {
                /* write token word */
                जइ (!matcher.consistentWith(reinterpret_cast<ध्रुव वर्ण *>(tokenStrings + token))) {
                    प्रत्यागम मिच्छ;
                }
            }
        }
    }

    /* complete match? */
    प्रत्यागम matcher.matches();
}

स्थिर uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    जइ (U_IS_UNICODE_NONCHAR(cp)) {
        प्रत्यागम U_NONCHARACTER_CODE_POINT;
    }

    जइ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    प्रत्यागम cat;
}

स्थिर ध्रुव वर्ण *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    जइ (cat >= UPRV_LENGTHOF(charCatNames)) {
        प्रत्यागम "unknown";
    } अण्णहा {
        प्रत्यागम charCatNames[cat];
    }
}

स्थिर uint16_t getExtName(uint32_t code, वर्ण *buffer, uint16_t bufferLength) {
    ध्रुव वर्ण *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    पूर्णाङ्क ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    जाव (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    प्रति (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    जइ (ndigits < 4)
        ndigits = 4;
    प्रति (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    प्रत्यागम length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
स्थिर ध्रुव uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    ध्रुव uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    जाव(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        जइ(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } अण्णहा {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    प्रत्यागम groups+start*GROUP_LENGTH;
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
स्थिर ध्रुव uint8_t *
expandGroupLengths(ध्रुव uint8_t *s,
                   uint16_t offsets[पङ्क्तयः_PER_GROUP+1], uint16_t lengths[पङ्क्तयः_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    जाव(i<पङ्क्तयः_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        जइ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } अण्णहा जइ((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } अण्णहा {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        जइ((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            जइ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } अण्णहा {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    प्रत्यागम s;
}

स्थिर uint16_t
expandGroupName(UCharNames *names, ध्रुव uint16_t *group,
                uint16_t पङ्क्तिः_सङ्ख्या, UCharNameChoice nameChoice,
                वर्ण *buffer, uint16_t bufferLength) {
    uint16_t offsets[पङ्क्तयः_PER_GROUP+2], lengths[पङ्क्तयः_PER_GROUP+2];
    ध्रुव uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    प्रत्यागम expandName(names, s+offsets[पङ्क्तिः_सङ्ख्या], lengths[पङ्क्तिः_सङ्ख्या], nameChoice,
                      buffer, bufferLength);
}

स्थिर uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        वर्ण *buffer, uint16_t bufferLength) {
    ध्रुव uint16_t *group=getGroup(names, code);
    जइ (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        प्रत्यागम expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } अण्णहा {
        /* group not found */
        /* zero-terminate */
        जइ(bufferLength>0) {
            *buffer=0;
        }
        प्रत्यागम 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
स्थिर UBool
enumGroupNames(UCharNames *names, ध्रुव uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, शून्य *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[पङ्क्तयः_PER_GROUP+2], lengths[पङ्क्तयः_PER_GROUP+2];
    ध्रुव uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    जइ(fn!=DO_FIND_NAME) {
        वर्ण buffer[200];
        uint16_t length;

        जाव(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, परिमाण(buffer));
            जइ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, परिमाण(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            जइ(length>0) {
                जइ(!fn(context, start, nameChoice, buffer, length)) {
                    प्रत्यागम मिच्छ;
                }
            }
            ++start;
        }
    } अण्णहा {
        ध्रुव CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        जाव(start<=end) {
            जइ(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                प्रत्यागम मिच्छ;
            }
            ++start;
        }
    }
    प्रत्यागम सच्च;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
स्थिर UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, शून्य *context)
{
    जइ(fn!=DO_FIND_NAME) {
        वर्ण buffer[200];
        uint16_t length;
        
        जाव(start<=end) {
            buffer[length = getExtName(start, buffer, परिमाण(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            जइ(length>0) {
                जइ(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    प्रत्यागम मिच्छ;
                }
            }
            ++start;
        }
    }

    प्रत्यागम सच्च;
}

स्थिर UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, शून्य *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_गणनम्;
    ध्रुव uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    जइ(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        जइ(extLimit>limit) {
            extLimit=limit;
        }
        जइ(!enumExtNames(start, extLimit-1, fn, context)) {
            प्रत्यागम मिच्छ;
        }
        start=extLimit;
    }

    जइ(startGroupMSB==endGroupMSB) {
        जइ(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            प्रत्यागम enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } अण्णहा {
        ध्रुव uint16_t *groups=GET_GROUPS(names);
        group_गणनम्=*groups++;
        groupLimit=groups+group_गणनम्*GROUP_LENGTH;

        जइ(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            जइ((start&GROUP_MASK)!=0) {
                जइ(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + पङ्क्तयः_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    प्रत्यागम मिच्छ;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } अण्णहा जइ(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ध्रुव uint16_t *nextGroup=NEXT_GROUP(group);
            जइ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                जइ (end > limit) {
                    end = limit;
                }
                जइ (!enumExtNames(start, end - 1, fn, context)) {
                    प्रत्यागम मिच्छ;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        जाव(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            ध्रुव uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            जइ(!enumGroupNames(names, group, start, start+पङ्क्तयः_PER_GROUP-1, fn, context, nameChoice)) {
                प्रत्यागम मिच्छ;
            }
            nextGroup=NEXT_GROUP(group);
            जइ (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                जइ (end > limit) {
                    end = limit;
                }
                जइ (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    प्रत्यागम मिच्छ;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        जइ(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            प्रत्यागम enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } अण्णहा जइ (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            जइ (next > start) {
                start = next;
            }
        } अण्णहा {
            प्रत्यागम सच्च;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    जइ (nameChoice == U_EXTENDED_CHAR_NAME) {
        जइ (limit > UCHAR_MAX_मानम् + 1) {
            limit = UCHAR_MAX_मानम् + 1;
        }
        प्रत्यागम enumExtNames(start, limit - 1, fn, context);
    }
    
    प्रत्यागम सच्च;
}

स्थिर uint16_t
writeFactorSuffix(ध्रुव uint16_t *factors, uint16_t count,
                  ध्रुव वर्ण *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  ध्रुव वर्ण *elementBases[8], ध्रुव वर्ण *elements[8],
                  वर्ण *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    वर्ण c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    प्रति(i=count; i>0; --i) {
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
    प्रति(;;) {
        जइ(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        जाव(factor>0) {
            जाव(*s++!=0) {}
            --factor;
        }
        जइ(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        जाव((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        जइ(i>=count) {
            भङ्ग;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        जाव(factor>0) {
            जाव(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    जइ(bufferLength>0) {
        *buffer=0;
    }

    प्रत्यागम bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
स्थिर uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        वर्ण *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    जइ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        जइ(bufferLength>0) {
            *buffer=0;
        }
        प्रत्यागम 0;
    }

    विकल्प(range->type) {
    पक्ष 0: {
        /* name = prefix hex-digits */
        ध्रुव वर्ण* s = reinterpret_cast<ध्रुव वर्ण*>(range + 1);
        वर्ण c;

        uint16_t i, count;

        /* copy prefix */
        जाव((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        जइ(count<bufferLength) {
            buffer[count]=0;
        }

        प्रति(i=count; i>0;) {
            जइ(--i<bufferLength) {
                c = static_cast<वर्ण>(code & 0xf);
                जइ(c<10) {
                    c+='0';
                } अण्णहा {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        भङ्ग;
    }
    पक्ष 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ध्रुव वर्ण* s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
        वर्ण c;

        /* copy prefix */
        जाव((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        भङ्ग;
    }
    स्वभाव:
        /* undefined type */
        /* zero-terminate */
        जइ(bufferLength>0) {
            *buffer=0;
        }
        भङ्ग;
    }

    प्रत्यागम bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
स्थिर UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, शून्य *context,
             UCharNameChoice nameChoice) {
    वर्ण buffer[200];
    uint16_t length;

    जइ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        प्रत्यागम सच्च;
    }

    विकल्प(range->type) {
    पक्ष 0: {
        वर्ण *s, *end;
        वर्ण c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, परिमाण(buffer));
        जइ(length<=0) {
            प्रत्यागम सच्च;
        }

        /* call the enumerator function with this first character */
        जइ(!fn(context, start, nameChoice, buffer, length)) {
            प्रत्यागम मिच्छ;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        जाव(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        जाव(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            प्रति (;;) {
                c=*--s;
                जइ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<वर्ण>(c + 1);
                    भङ्ग;
                } अण्णहा जइ(c=='9') {
                    *s='A';
                    भङ्ग;
                } अण्णहा जइ(c=='F') {
                    *s='0';
                }
            }

            जइ(!fn(context, start, nameChoice, buffer, length)) {
                प्रत्यागम मिच्छ;
            }
        }
        भङ्ग;
    }
    पक्ष 1: {
        uint16_t indexes[8];
        ध्रुव वर्ण *elementBases[8], *elements[8];
        ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ध्रुव वर्ण* s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
        वर्ण *suffix, *t;
        uint16_t prefixLength, i, idx;

        वर्ण c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        जाव((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(परिमाण(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        जइ(!fn(context, start, nameChoice, buffer, length)) {
            प्रत्यागम मिच्छ;
        }

        /* enumerate the rest of the names */
        जाव(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            प्रति (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                जइ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    जाव(*s++!=0) {
                    }
                    elements[i]=s;
                    भङ्ग;
                } अण्णहा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            प्रति(i=0; i<count; ++i) {
                s=elements[i];
                जाव((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            जइ(!fn(context, start, nameChoice, buffer, length)) {
                प्रत्यागम मिच्छ;
            }
        }
        भङ्ग;
    }
    स्वभाव:
        /* undefined type */
        भङ्ग;
    }

    प्रत्यागम सच्च;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
स्थिर UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    जइ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        प्रत्यागम 0xffff;
    }

    विकल्प(range->type) {
    पक्ष 0: {
        /* name = prefix hex-digits */
        ध्रुव वर्ण* s = reinterpret_cast<ध्रुव वर्ण*>(range + 1);

        /* compare prefix */
        जइ (!matcher.consistentWith(s)) {
            प्रत्यागम 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        प्रति(वर्ण c : matcher.remainingSignificantCharacters()) {
            जइ('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } अण्णहा जइ('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } अण्णहा {
                प्रत्यागम 0xffff;
            }
        }

        /* does it fit into the range? */
        जइ (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            प्रत्यागम code;
        }
        भङ्ग;
    }
    पक्ष 1: {
        वर्ण buffer[64];
        uint16_t indexes[8];
        ध्रुव वर्ण *elementBases[8], *elements[8];
        ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ध्रुव वर्ण *s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        जइ (!matcher.consistentWith(prefix)) {
            प्रत्यागम 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, परिमाण(buffer));

        /* compare the first suffix */
        जइ(querySuffix == buffer) {
            प्रत्यागम start;
        }

        /* enumerate and compare the rest of the suffixes */
        जाव(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            प्रति (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                जइ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    जाव(*s++!=0) {}
                    elements[i]=s;
                    भङ्ग;
                } अण्णहा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            प्रति(i=0; i<count; ++i) {
                s=elements[i];
                वर्ण c;
                जाव((c=*s++)!=0) {
                    जइ(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            जइ(i<99 && it == querySuffix.end()) {
                प्रत्यागम start;
            }
        }
        भङ्ग;
    }
    स्वभाव:
        /* undefined type */
        भङ्ग;
    }

    प्रत्यागम 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

स्थिर int32_t
calcStringSetLength(uint32_t set[8], ध्रुव वर्ण *s) {
    int32_t length=0;
    वर्ण c;

    जाव((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    प्रत्यागम length;
}

स्थिर int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_गणनम्;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_गणनम्=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    जाव(range_गणनम्>0) {
        विकल्प(range->type) {
        पक्ष 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<ध्रुव वर्ण*>(range + 1)) + range->variant;
            जइ(length>maxNameLength) {
                maxNameLength=length;
            }
            भङ्ग;
        पक्ष 1: {
            /* name = prefix factorized-elements */
            ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
            ध्रुव वर्ण *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            प्रति(i=0; i<count; ++i) {
                maxFactorLength=0;
                प्रति(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    जइ(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            जइ(length>maxNameLength) {
                maxNameLength=length;
            }
            भङ्ग;
        }
        स्वभाव:
            /* unknown type */
            भङ्ग;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_गणनम्;
    }
    प्रत्यागम maxNameLength;
}

स्थिर int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    प्रति(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        जइ(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    प्रत्यागम maxNameLength;
}

स्थिर int32_t
calcNameSetLength(ध्रुव uint16_t *tokens, uint16_t token_गणनम्, ध्रुव uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  ध्रुव uint8_t **p_पङ्क्तिः, ध्रुव uint8_t *पङ्क्तिः_Limit) {
    ध्रुव uint8_t *पङ्क्तिः=*p_पङ्क्तिः;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    जाव (पङ्क्तिः != पङ्क्तिः_Limit && (c = *पङ्क्तिः++) != static_cast<uint8_t>(';')) {
        जइ(c>=token_गणनम्) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } अण्णहा {
            token=tokens[c];
            जइ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*पङ्क्तिः++;
                token=tokens[c];
            }
            जइ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } अण्णहा {
                /* count token word */
                जइ(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    जइ(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<ध्रुव वर्ण*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } अण्णहा {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<ध्रुव वर्ण*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_पङ्क्तिः=पङ्क्तिः;
    प्रत्यागम length;
}

स्थिर शून्य
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[पङ्क्तयः_PER_GROUP+2], lengths[पङ्क्तयः_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_गणनम्=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    ध्रुव uint16_t *group;
    ध्रुव uint8_t *s, *पङ्क्तिः, *पङ्क्तिः_Limit;

    int32_t group_गणनम्, पङ्क्तिः_सङ्ख्या, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_गणनम्));
    जइ(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_गणनम्);
    }

    group=GET_GROUPS(uCharNames);
    group_गणनम्=*group++;

    /* enumerate all groups */
    जाव(group_गणनम्>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        प्रति(पङ्क्तिः_सङ्ख्या=0; पङ्क्तिः_सङ्ख्या<पङ्क्तयः_PER_GROUP; ++पङ्क्तिः_सङ्ख्या) {
            पङ्क्तिः=s+offsets[पङ्क्तिः_सङ्ख्या];
            length=lengths[पङ्क्तिः_सङ्ख्या];
            जइ(length==0) {
                अनुवर्त;
            }

            पङ्क्तिः_Limit=पङ्क्तिः+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_गणनम्, tokenStrings, tokenLengths, gNameSet, &पङ्क्तिः, पङ्क्तिः_Limit);
            जइ(length>maxNameLength) {
                maxNameLength=length;
            }
            जइ(पङ्क्तिः==पङ्क्तिः_Limit) {
                अनुवर्त;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_गणनम्, tokenStrings, tokenLengths, gNameSet, &पङ्क्तिः, पङ्क्तिः_Limit);
            जइ(length>maxNameLength) {
                maxNameLength=length;
            }
            जइ(पङ्क्तिः==पङ्क्तिः_Limit) {
                अनुवर्त;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_गणनम्;
    }

    जइ(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

स्थिर UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    स्थिर ध्रुव वर्ण extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    जइ(gMaxNameLength!=0) {
        प्रत्यागम सच्च;
    }

    जइ(!isDataLoaded(pErrorCode)) {
        प्रत्यागम मिच्छ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    प्रति (i = 0; i < static_cast<int32_t>(परिमाण(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    प्रत्यागम सच्च;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           वर्ण *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    } अण्णहा जइ(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ((uint32_t)code>UCHAR_MAX_मानम् || !isDataLoaded(pErrorCode)) {
        प्रत्यागम u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जाव(i>0) {
        जइ(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            भङ्ग;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    जइ(i==0) {
        जइ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            जइ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } अण्णहा {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    प्रत्यागम u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                वर्ण *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    } अण्णहा जइ(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    प्रत्यागम u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ध्रुव वर्ण *ध्रुव name,
               UErrorCode *pErrorCode) {
    वर्ण lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    वर्ण c0;
    स्थिर constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम error;
    }

    जइ(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम error;
    }

    जइ(!isDataLoaded(pErrorCode)) {
        प्रत्यागम error;
    }

    /* construct the lowercase of the name first */
    ध्रुव वर्ण* nameChar = name;
    प्रति(i=0; i<परिमाण(lower); ++i) {
        जइ((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } अण्णहा {
            भङ्ग;
        }
    }
    जइ(i==परिमाण(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        प्रत्यागम error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    जइ (lower[0] == '<') {
        जइ (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            जाव (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            जइ (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                प्रति (++i; i < limit; ++i) {
                    जइ (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } अण्णहा जइ (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } अण्णहा {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        प्रत्यागम error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    जइ (cp > UCHAR_MAX_मानम्) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        प्रत्यागम error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                प्रति (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    जइ (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        जइ (cat == cIdx) {
                            प्रत्यागम cp;
                        }
                        भङ्ग;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        प्रत्यागम error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जाव(i>0) {
        जइ((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            प्रत्यागम cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_मानम् + 1, DO_FIND_NAME, &findName, nameChoice);
    जइ (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    प्रत्यागम findName.code;
}

U_CAPI शून्य U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                शून्य *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }

    जइ(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम;
    }

    जइ((uint32_t) limit > UCHAR_MAX_मानम् + 1) {
        limit = UCHAR_MAX_मानम् + 1;
    }
    जइ((uint32_t)start>=(uint32_t)limit) {
        प्रत्यागम;
    }

    जइ(!isDataLoaded(pErrorCode)) {
        प्रत्यागम;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जाव(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        जइ((uint32_t)start<algRange->start) {
            जइ((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                प्रत्यागम;
            }
            जइ(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                प्रत्यागम;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        जइ((uint32_t)start<=algRange->end) {
            जइ((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                प्रत्यागम;
            }
            जइ(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                प्रत्यागम;
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
    जइ(calcNameSetsLengths(&errorCode)) {
        प्रत्यागम gMaxNameLength;
    } अण्णहा {
        प्रत्यागम 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
स्थिर शून्य
charSetToUSet(uint32_t cset[8], ध्रुव USetAdder *sa) {
    char16_t us[256];
    वर्ण cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    जइ(!calcNameSetsLengths(&errorCode)) {
        प्रत्यागम;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    प्रति(i=0; i<256; ++i) {
        जइ(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<वर्ण>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    प्रति(i=0; i<length; ++i) {
        जइ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI शून्य U_EXPORT2
uprv_getCharNameCharacters(ध्रुव USetAdder *sa) {
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
स्थिर शून्य
makeTokenMap(ध्रुव UDataSwapper *ds,
             int16_t tokens[], uint16_t token_गणनम्,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम;
    }

    जइ(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        प्रति(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } अण्णहा {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        जइ(token_गणनम्>256) {
            token_गणनम्=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        प्रति(i=1; i<token_गणनम्; ++i) {
            जइ(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                जइ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    प्रत्यागम;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=सच्च;
            }
        }

        /* set the mappings for the rest of the permutation */
        प्रति(i=j=1; i<token_गणनम्; ++i) {
            /* set mappings that were not set for direct bytes */
            जइ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                जाव(usedOutChar[j]) {
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
uchar_swapNames(ध्रुव UDataSwapper *ds,
                ध्रुव शून्य *inData, int32_t length, शून्य *outData,
                UErrorCode *pErrorCode) {
    ध्रुव UDataInfo *pInfo;
    int32_t headerSize;

    ध्रुव uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_गणनम्;

    ध्रुव AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    जइ(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)inData+4);
    जइ(!(
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
        प्रत्यागम 0;
    }

    inBytes=(ध्रुव uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    जइ(length<0) {
        algNamesOffset=ds->readUInt32(((ध्रुव uint32_t *)inBytes)[3]);
    } अण्णहा {
        length-=headerSize;
        जइ( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((ध्रुव uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    जइ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ध्रुव uint32_t *)(inBytes+offset)));
        offset+=4;

        प्रति(i=0; i<count; ++i) {
            inRange=(ध्रुव AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } अण्णहा {
        /* swap data */
        ध्रुव uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_गणनम्;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        जइ(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((ध्रुव uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((ध्रुव uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((ध्रुव uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ध्रुव uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_गणनम्=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        जइ(token_गणनम्<=512) {
            count=token_गणनम्;
        } अण्णहा {
            count=512;
        }
        प्रति(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        प्रति(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_गणनम्, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_गणनम्>256 ? token_गणनम्-256 : 0), trailMap, pErrorCode);
        जइ(U_FAILURE(*pErrorCode)) {
            प्रत्यागम 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_गणनम्*2);
        जइ(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_गणनम्);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            प्रत्यागम 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        प्रति(i=0; i<token_गणनम् && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        प्रति(; i<token_गणनम्; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_गणनम्*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        जइ(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            प्रत्यागम 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ध्रुव uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        जइ(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[पङ्क्तयः_PER_GROUP+1], lengths[पङ्क्तयः_PER_GROUP+1];

            ध्रुव uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_गणनम्=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            जाव(strings_गणनम्>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_गणनम्-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_गणनम्-=count;

                /* swap the string bytes using map[] and trailMap[] */
                जाव(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    जइ(tokens[c]!=-2) {
                        --count;
                    } अण्णहा {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ध्रुव uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        प्रति(i=0; i<count; ++i) {
            जइ(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }

            inRange=(ध्रुव AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            विकल्प(inRange->type) {
            पक्ष 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((ध्रुव वर्ण *)(inRange+1)),
                                    outRange+1, pErrorCode);
                जइ(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    प्रत्यागम 0;
                }
                भङ्ग;
            पक्ष 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_गणनम्;

                    factors_गणनम्=inRange->variant;
                    p=(ध्रुव uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_गणनम्*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_गणनम्;
                    q+=factors_गणनम्;
                    strings_गणनम्=(uint32_t)((inBytes+offset)-(ध्रुव uint8_t *)p);
                    जाव(strings_गणनम्>0 && ((ध्रुव uint8_t *)p)[strings_गणनम्-1]!=0) {
                        --strings_गणनम्;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_गणनम्, q, pErrorCode);
                }
                भङ्ग;
            स्वभाव:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                प्रत्यागम 0;
            }
        }
    }

    प्रत्यागम headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
