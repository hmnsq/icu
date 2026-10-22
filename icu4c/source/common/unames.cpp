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

#ضمن "unicode/utypes.h"
#ضمن "unicode/putil.h"
#ضمن "unicode/uchar.h"
#ضمن "unicode/udata.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf16.h"
#ضمن "uassert.h"
#ضمن "ustr_imp.h"
#ضمن "umutex.h"
#ضمن "cmemory.h"
#ضمن "cstring.h"
#ضمن "ucln_cmn.h"
#ضمن "udataswp.h"
#ضمن "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

ساكن ثابت حرف DATA_NAME[] = "unames";
ساكن ثابت حرف DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define ليوك_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (ليوك_PER_GROUP-1)

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
تعداد {
    GROUP_MSB,
    GROUP_OFFSET_سم,
    GROUP_OFFSET_كس,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_سم]<<16|(group)[GROUP_OFFSET_كس])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

تعريفنوع بنية {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعريفنوع بنية {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

نطاق {
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
صنف CharacterNameQuery {
  عام:
    صنف Matcher {
      عام:
        // `query` must outlive the constructed object.
        Matcher(ثابت CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        منطقي consistentWith(حرف c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            إذا (skeletonIterator == query.skeleton.end()) {
                أرجع خطأ;
            }
            إذا (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                إذا (lastChar == '-') {
                    إذا (*skeletonIterator++ != lastChar) {
                        أرجع خطأ;
                    }
                }
                lastChar = c;
                أرجع true;
            } وإلا إذا (c == '-') {
                إذا (lastChar == ' ' ||
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
                    إذا (*skeletonIterator++ != c) {
                        أرجع خطأ;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                أرجع true;
            } وإلا {
                إذا (*skeletonIterator++ != c) {
                    أرجع خطأ;
                }
                lastChar = c;
                أرجع true;
            }
        }

        منطقي consistentWith(ثابت std::string_view substring) {
            لكل (ثابت حرف c : substring) {
                إذا (!consistentWith(c)) {
                    أرجع خطأ;
                }
            }
            أرجع true;
        }

        منطقي matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            أرجع skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() ثابت {
            أرجع query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      خاص:
        ثابت CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        حرف lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeletonLimit = skeletonData;
        لكل (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + حجم(skeletonData));
            إذا (skeletonLimit >= skeletonData + حجم(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                اكسر;
            }
            إذا (query[i] == ' ' || query[i] == '_') {
                استمر;
            }
            إذا (query[i] == '-') {
                منطقي isMedial;
                منطقي is1180MedialHyphen = خطأ;
                إذا (i == 0 || i == query.length() - 1) {
                    isMedial = خطأ;
                } وإلا {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                إذا (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = true;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    لكل (std::size_t j = i + 2; j < query.length(); ++j) {
                        إذا (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = خطأ;
                        }
                    }
                }
                إذا (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } وإلا {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ثابت {
        أرجع Matcher(*هذا);
    }

خاص:
    ساكن منطقي isASCIILetterOrDigit(ثابت حرف c) {
        أرجع uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    حرف skeletonData[120];
    std::string_view skeleton;
    منطقي is1180;
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
#define GET_GROUPS(names) (ثابت uint16_t *)((ثابت حرف *)names+names->groupsOffset)

تعريفنوع بنية {
    ثابت CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

ساكن UDataMemory *uCharNamesData=nullptr;
ساكن UCharNames *uCharNames=nullptr;
ساكن icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساكن int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساكن uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_كدي
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_كدي + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_كدي + 2

#define U_CHAR_EXTENDED_CATEGORY_كدي (U_CHAR_CATEGORY_كدي + 3)

ساكن ثابت حرف * ثابت charCatNames[U_CHAR_EXTENDED_CATEGORY_كدي] = {
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

ساكن UBool U_CALLCONV unames_cleanup()
{
    إذا(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    إذا(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    أرجع true;
}

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

ساكن فراغ U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    إذا(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } وإلا {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساكن UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    أرجع U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    إذا((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_كدي

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
ساكن uint16_t
expandName(UCharNames *names,
           ثابت uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           حرف *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_كدي=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_كدي || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(nameLength>0) {
                    --nameLength;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--fieldIndex>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    طالما(nameLength>0) {
        --nameLength;
        c=*name++;

        إذا(c>=token_كدي) {
            إذا(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } وإلا {
                /* finished */
                اكسر;
            }
        } وإلا {
            token=tokens[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } وإلا {
                    /* finished */
                    اكسر;
                }
            } وإلا {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                طالما((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    إذا(bufferLength>0) {
        *buffer=0;
    }

    أرجع bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ساكن UBool
compareName(UCharNames *names,
            ثابت uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_كدي=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_كدي || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(nameLength>0) {
                    --nameLength;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--fieldIndex>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    طالما(nameLength>0) {
        --nameLength;
        c=*name++;

        إذا(c>=token_كدي) {
            إذا(c!=';') {
                /* implicit letter */
                إذا (!matcher.consistentWith(c)) {
                    أرجع خطأ;
                }
            } وإلا {
                /* finished */
                اكسر;
            }
        } وإلا {
            token=tokens[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    إذا (!matcher.consistentWith(c)) {
                        أرجع خطأ;
                    }
                } وإلا {
                    /* finished */
                    اكسر;
                }
            } وإلا {
                /* write token word */
                إذا (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(tokenStrings + token))) {
                    أرجع خطأ;
                }
            }
        }
    }

    /* complete match? */
    أرجع matcher.matches();
}

ساكن uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    إذا (U_IS_UNICODE_NONCHAR(cp)) {
        أرجع U_NONCHARACTER_CODE_POINT;
    }

    إذا ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    أرجع cat;
}

ساكن ثابت حرف *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    إذا (cat >= UPRV_LENGTHOF(charCatNames)) {
        أرجع "unknown";
    } وإلا {
        أرجع charCatNames[cat];
    }
}

ساكن uint16_t getExtName(uint32_t code, حرف *buffer, uint16_t bufferLength) {
    ثابت حرف *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    صحيح ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    طالما (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    لكل (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    إذا (ndigits < 4)
        ndigits = 4;
    لكل (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    أرجع length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ساكن ثابت uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    ثابت uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    طالما(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        إذا(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } وإلا {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    أرجع groups+start*GROUP_LENGTH;
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
ساكن ثابت uint8_t *
expandGroupLengths(ثابت uint8_t *s,
                   uint16_t offsets[ليوك_PER_GROUP+1], uint16_t lengths[ليوك_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    طالما(i<ليوك_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        إذا(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } وإلا إذا((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } وإلا {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        إذا((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            إذا(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } وإلا {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    أرجع s;
}

ساكن uint16_t
expandGroupName(UCharNames *names, ثابت uint16_t *group,
                uint16_t لي_لمب, UCharNameChoice nameChoice,
                حرف *buffer, uint16_t bufferLength) {
    uint16_t offsets[ليوك_PER_GROUP+2], lengths[ليوك_PER_GROUP+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    أرجع expandName(names, s+offsets[لي_لمب], lengths[لي_لمب], nameChoice,
                      buffer, bufferLength);
}

ساكن uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        حرف *buffer, uint16_t bufferLength) {
    ثابت uint16_t *group=getGroup(names, code);
    إذا (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        أرجع expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } وإلا {
        /* group not found */
        /* zero-terminate */
        إذا(bufferLength>0) {
            *buffer=0;
        }
        أرجع 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ساكن UBool
enumGroupNames(UCharNames *names, ثابت uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, فراغ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[ليوك_PER_GROUP+2], lengths[ليوك_PER_GROUP+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    إذا(fn!=DO_FIND_NAME) {
        حرف buffer[200];
        uint16_t length;

        طالما(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, حجم(buffer));
            إذا (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, حجم(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, start, nameChoice, buffer, length)) {
                    أرجع خطأ;
                }
            }
            ++start;
        }
    } وإلا {
        ثابت CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        طالما(start<=end) {
            إذا(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                أرجع خطأ;
            }
            ++start;
        }
    }
    أرجع true;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ساكن UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, فراغ *context)
{
    إذا(fn!=DO_FIND_NAME) {
        حرف buffer[200];
        uint16_t length;
        
        طالما(start<=end) {
            buffer[length = getExtName(start, buffer, حجم(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    أرجع خطأ;
                }
            }
            ++start;
        }
    }

    أرجع true;
}

ساكن UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, فراغ *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_كدي;
    ثابت uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    إذا(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        إذا(extLimit>limit) {
            extLimit=limit;
        }
        إذا(!enumExtNames(start, extLimit-1, fn, context)) {
            أرجع خطأ;
        }
        start=extLimit;
    }

    إذا(startGroupMSB==endGroupMSB) {
        إذا(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            أرجع enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } وإلا {
        ثابت uint16_t *groups=GET_GROUPS(names);
        group_كدي=*groups++;
        groupLimit=groups+group_كدي*GROUP_LENGTH;

        إذا(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            إذا((start&GROUP_MASK)!=0) {
                إذا(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + ليوك_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    أرجع خطأ;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } وإلا إذا(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *nextGroup=NEXT_GROUP(group);
            إذا (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                إذا (end > limit) {
                    end = limit;
                }
                إذا (!enumExtNames(start, end - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        طالما(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            ثابت uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            إذا(!enumGroupNames(names, group, start, start+ليوك_PER_GROUP-1, fn, context, nameChoice)) {
                أرجع خطأ;
            }
            nextGroup=NEXT_GROUP(group);
            إذا (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                إذا (end > limit) {
                    end = limit;
                }
                إذا (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        إذا(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            أرجع enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } وإلا إذا (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            إذا (next > start) {
                start = next;
            }
        } وإلا {
            أرجع true;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
        إذا (limit > UCHAR_MAX_درج + 1) {
            limit = UCHAR_MAX_درج + 1;
        }
        أرجع enumExtNames(start, limit - 1, fn, context);
    }
    
    أرجع true;
}

ساكن uint16_t
writeFactorSuffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    حرف c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    لكل(i=count; i>0; --i) {
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
    لكل(;;) {
        إذا(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        طالما(factor>0) {
            طالما(*s++!=0) {}
            --factor;
        }
        إذا(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        إذا(i>=count) {
            اكسر;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        طالما(factor>0) {
            طالما(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    إذا(bufferLength>0) {
        *buffer=0;
    }

    أرجع bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ساكن uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        حرف *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        إذا(bufferLength>0) {
            *buffer=0;
        }
        أرجع 0;
    }

    بدل(range->type) {
    حالة 0: {
        /* name = prefix hex-digits */
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(range + 1);
        حرف c;

        uint16_t i, count;

        /* copy prefix */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        إذا(count<bufferLength) {
            buffer[count]=0;
        }

        لكل(i=count; i>0;) {
            إذا(--i<bufferLength) {
                c = static_cast<حرف>(code & 0xf);
                إذا(c<10) {
                    c+='0';
                } وإلا {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        اكسر;
    }
    حالة 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(factors + count);
        حرف c;

        /* copy prefix */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        اكسر;
    }
    افتراضي:
        /* undefined type */
        /* zero-terminate */
        إذا(bufferLength>0) {
            *buffer=0;
        }
        اكسر;
    }

    أرجع bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ساكن UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, فراغ *context,
             UCharNameChoice nameChoice) {
    حرف buffer[200];
    uint16_t length;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع true;
    }

    بدل(range->type) {
    حالة 0: {
        حرف *s, *end;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, حجم(buffer));
        إذا(length<=0) {
            أرجع true;
        }

        /* call the enumerator function with this first character */
        إذا(!fn(context, start, nameChoice, buffer, length)) {
            أرجع خطأ;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        طالما(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        طالما(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            لكل (;;) {
                c=*--s;
                إذا(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<حرف>(c + 1);
                    اكسر;
                } وإلا إذا(c=='9') {
                    *s='A';
                    اكسر;
                } وإلا إذا(c=='F') {
                    *s='0';
                }
            }

            إذا(!fn(context, start, nameChoice, buffer, length)) {
                أرجع خطأ;
            }
        }
        اكسر;
    }
    حالة 1: {
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(factors + count);
        حرف *suffix, *t;
        uint16_t prefixLength, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        طالما((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        إذا(!fn(context, start, nameChoice, buffer, length)) {
            أرجع خطأ;
        }

        /* enumerate the rest of the names */
        طالما(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            لكل (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                إذا(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    طالما(*s++!=0) {
                    }
                    elements[i]=s;
                    اكسر;
                } وإلا {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                طالما((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            إذا(!fn(context, start, nameChoice, buffer, length)) {
                أرجع خطأ;
            }
        }
        اكسر;
    }
    افتراضي:
        /* undefined type */
        اكسر;
    }

    أرجع true;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ساكن UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع 0xffff;
    }

    بدل(range->type) {
    حالة 0: {
        /* name = prefix hex-digits */
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(range + 1);

        /* compare prefix */
        إذا (!matcher.consistentWith(s)) {
            أرجع 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        لكل(حرف c : matcher.remainingSignificantCharacters()) {
            إذا('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } وإلا إذا('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } وإلا {
                أرجع 0xffff;
            }
        }

        /* does it fit into the range? */
        إذا (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            أرجع code;
        }
        اكسر;
    }
    حالة 1: {
        حرف buffer[64];
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف *s = reinterpret_cast<ثابت حرف*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        إذا (!matcher.consistentWith(prefix)) {
            أرجع 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, حجم(buffer));

        /* compare the first suffix */
        إذا(querySuffix == buffer) {
            أرجع start;
        }

        /* enumerate and compare the rest of the suffixes */
        طالما(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            لكل (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                إذا(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    طالما(*s++!=0) {}
                    elements[i]=s;
                    اكسر;
                } وإلا {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                حرف c;
                طالما((c=*s++)!=0) {
                    إذا(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            إذا(i<99 && it == querySuffix.end()) {
                أرجع start;
            }
        }
        اكسر;
    }
    افتراضي:
        /* undefined type */
        اكسر;
    }

    أرجع 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساكن int32_t
calcStringSetLength(uint32_t set[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    طالما((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    أرجع length;
}

ساكن int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_كدي;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_كدي=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    طالما(range_كدي>0) {
        بدل(range->type) {
        حالة 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            إذا(length>maxNameLength) {
                maxNameLength=length;
            }
            اكسر;
        حالة 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            لكل(i=0; i<count; ++i) {
                maxFactorLength=0;
                لكل(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    إذا(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            إذا(length>maxNameLength) {
                maxNameLength=length;
            }
            اكسر;
        }
        افتراضي:
            /* unknown type */
            اكسر;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_كدي;
    }
    أرجع maxNameLength;
}

ساكن int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    لكل(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        إذا(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    أرجع maxNameLength;
}

ساكن int32_t
calcNameSetLength(ثابت uint16_t *tokens, uint16_t token_كدي, ثابت uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  ثابت uint8_t **p_لي, ثابت uint8_t *لي_Limit) {
    ثابت uint8_t *لي=*p_لي;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    طالما (لي != لي_Limit && (c = *لي++) != static_cast<uint8_t>(';')) {
        إذا(c>=token_كدي) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } وإلا {
            token=tokens[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*لي++;
                token=tokens[c];
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } وإلا {
                /* count token word */
                إذا(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    إذا(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<ثابت حرف*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } وإلا {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<ثابت حرف*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_لي=لي;
    أرجع length;
}

ساكن فراغ
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[ليوك_PER_GROUP+2], lengths[ليوك_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_كدي=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    ثابت uint16_t *group;
    ثابت uint8_t *s, *لي, *لي_Limit;

    int32_t group_كدي, لي_لمب, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_كدي));
    إذا(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_كدي);
    }

    group=GET_GROUPS(uCharNames);
    group_كدي=*group++;

    /* enumerate all groups */
    طالما(group_كدي>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        لكل(لي_لمب=0; لي_لمب<ليوك_PER_GROUP; ++لي_لمب) {
            لي=s+offsets[لي_لمب];
            length=lengths[لي_لمب];
            إذا(length==0) {
                استمر;
            }

            لي_Limit=لي+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_كدي, tokenStrings, tokenLengths, gNameSet, &لي, لي_Limit);
            إذا(length>maxNameLength) {
                maxNameLength=length;
            }
            إذا(لي==لي_Limit) {
                استمر;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_كدي, tokenStrings, tokenLengths, gNameSet, &لي, لي_Limit);
            إذا(length>maxNameLength) {
                maxNameLength=length;
            }
            إذا(لي==لي_Limit) {
                استمر;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_كدي;
    }

    إذا(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

ساكن UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    ساكن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    إذا(gMaxNameLength!=0) {
        أرجع true;
    }

    إذا(!isDataLoaded(pErrorCode)) {
        أرجع خطأ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    لكل (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    أرجع true;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           حرف *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    } وإلا إذا(nameChoice>=U_CHAR_NAME_CHOICE_كدي ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    إذا((uint32_t)code>UCHAR_MAX_درج || !isDataLoaded(pErrorCode)) {
        أرجع u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            اكسر;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    إذا(i==0) {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            إذا (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } وإلا {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    أرجع u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    } وإلا إذا(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    أرجع u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UErrorCode *pErrorCode) {
    حرف lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساكن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع error;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_كدي || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع error;
    }

    إذا(!isDataLoaded(pErrorCode)) {
        أرجع error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    لكل(i=0; i<حجم(lower); ++i) {
        إذا((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } وإلا {
            اكسر;
        }
    }
    إذا(i==حجم(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        أرجع error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    إذا (lower[0] == '<') {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            طالما (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            إذا (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                لكل (++i; i < limit; ++i) {
                    إذا (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } وإلا إذا (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } وإلا {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        أرجع error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    إذا (cp > UCHAR_MAX_درج) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        أرجع error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                لكل (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    إذا (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        إذا (cat == cIdx) {
                            أرجع cp;
                        }
                        اكسر;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        أرجع error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            أرجع cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_درج + 1, DO_FIND_NAME, &findName, nameChoice);
    إذا (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    أرجع findName.code;
}

U_CAPI فراغ U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                فراغ *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_كدي || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
    }

    إذا((uint32_t) limit > UCHAR_MAX_درج + 1) {
        limit = UCHAR_MAX_درج + 1;
    }
    إذا((uint32_t)start>=(uint32_t)limit) {
        أرجع;
    }

    إذا(!isDataLoaded(pErrorCode)) {
        أرجع;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        إذا((uint32_t)start<algRange->start) {
            إذا((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                أرجع;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        إذا((uint32_t)start<=algRange->end) {
            إذا((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                أرجع;
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
    إذا(calcNameSetsLengths(&errorCode)) {
        أرجع gMaxNameLength;
    } وإلا {
        أرجع 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ساكن فراغ
charSetToUSet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    إذا(!calcNameSetsLengths(&errorCode)) {
        أرجع;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    لكل(i=0; i<256; ++i) {
        إذا(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    لكل(i=0; i<length; ++i) {
        إذا(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI فراغ U_EXPORT2
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
ساكن فراغ
makeTokenMap(ثابت UDataSwapper *ds,
             int16_t tokens[], uint16_t token_كدي,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    إذا(U_FAILURE(*pErrorCode)) {
        أرجع;
    }

    إذا(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        لكل(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وإلا {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        إذا(token_كدي>256) {
            token_كدي=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        لكل(i=1; i<token_كدي; ++i) {
            إذا(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                إذا(U_FAILURE(*pErrorCode)) {
                    udata_بغ_Error(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    أرجع;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=true;
            }
        }

        /* set the mappings for the rest of the permutation */
        لكل(i=j=1; i<token_كدي; ++i) {
            /* set mappings that were not set for direct bytes */
            إذا(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                طالما(usedOutChar[j]) {
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
                ثابت فراغ *inData, int32_t length, فراغ *outData,
                UErrorCode *pErrorCode) {
    ثابت UDataInfo *pInfo;
    int32_t headerSize;

    ثابت uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_كدي;

    ثابت AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    إذا(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)inData+4);
    إذا(!(
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1
    )) {
        udata_بغ_Error(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->dataFormat[0], pInfo->dataFormat[1],
                         pInfo->dataFormat[2], pInfo->dataFormat[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    inBytes=(ثابت uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    إذا(length<0) {
        algNamesOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[3]);
    } وإلا {
        length-=headerSize;
        إذا( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((ثابت uint32_t *)inBytes)[3]))
        ) {
            udata_بغ_Error(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ثابت uint32_t *)(inBytes+offset)));
        offset+=4;

        لكل(i=0; i<count; ++i) {
            inRange=(ثابت AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } وإلا {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_كدي;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        إذا(inBytes!=outBytes) {
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
        token_كدي=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        إذا(token_كدي<=512) {
            count=token_كدي;
        } وإلا {
            count=512;
        }
        لكل(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        لكل(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_كدي, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_كدي>256 ? token_كدي-256 : 0), trailMap, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            أرجع 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_كدي*2);
        إذا(temp==nullptr) {
            udata_بغ_Error(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_كدي);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            أرجع 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        لكل(i=0; i<token_كدي && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        لكل(; i<token_كدي; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_كدي*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        إذا(U_FAILURE(*pErrorCode)) {
            udata_بغ_Error(ds, "uchar_swapNames(token strings) failed\n");
            أرجع 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        إذا(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[ليوك_PER_GROUP+1], lengths[ليوك_PER_GROUP+1];

            ثابت uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_كدي=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            طالما(strings_كدي>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_كدي-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_كدي-=count;

                /* swap the string bytes using map[] and trailMap[] */
                طالما(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    إذا(tokens[c]!=-2) {
                        --count;
                    } وإلا {
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

        لكل(i=0; i<count; ++i) {
            إذا(offset>(uint32_t)length) {
                udata_بغ_Error(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            inRange=(ثابت AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            بدل(inRange->type) {
            حالة 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((ثابت حرف *)(inRange+1)),
                                    outRange+1, pErrorCode);
                إذا(U_FAILURE(*pErrorCode)) {
                    udata_بغ_Error(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    أرجع 0;
                }
                اكسر;
            حالة 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_كدي;

                    factors_كدي=inRange->variant;
                    p=(ثابت uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_كدي*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_كدي;
                    q+=factors_كدي;
                    strings_كدي=(uint32_t)((inBytes+offset)-(ثابت uint8_t *)p);
                    طالما(strings_كدي>0 && ((ثابت uint8_t *)p)[strings_كدي-1]!=0) {
                        --strings_كدي;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_كدي, q, pErrorCode);
                }
                اكسر;
            افتراضي:
                udata_بغ_Error(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                أرجع 0;
            }
        }
    }

    أرجع headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
