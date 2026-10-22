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

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "unicode/uchar.h"
#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "ucln_cmn.h"
#অন্তর্ভাব "udataswp.h"
#অন্তর্ভাব "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

স্থির ধ্রুব বর্ণ DATA_NAME[] = "unames";
স্থির ধ্রুব বর্ণ DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define সারিগুলি_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (সারিগুলি_PER_GROUP-1)

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
গণনা {
    GROUP_MSB,
    GROUP_OFFSET_উপরের,
    GROUP_OFFSET_নিচের,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_উপরের]<<16|(group)[GROUP_OFFSET_নিচের])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

প্রকারনির্দেশ সংরচনা {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

প্রকারনির্দেশ সংরচনা {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

নামাকাশ {
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
বর্গ CharacterNameQuery {
  সার্বজনিক:
    বর্গ Matcher {
      সার্বজনিক:
        // `query` must outlive the constructed object.
        Matcher(ধ্রুব CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        তার্কিক consistentWith(বর্ণ c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            যদি (skeletonIterator == query.skeleton.end()) {
                প্রত্যাগম অসত্য;
            }
            যদি (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                যদি (lastChar == '-') {
                    যদি (*skeletonIterator++ != lastChar) {
                        প্রত্যাগম অসত্য;
                    }
                }
                lastChar = c;
                প্রত্যাগম সত্য;
            } অন্যথা যদি (c == '-') {
                যদি (lastChar == ' ' ||
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
                    যদি (*skeletonIterator++ != c) {
                        প্রত্যাগম অসত্য;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                প্রত্যাগম সত্য;
            } অন্যথা {
                যদি (*skeletonIterator++ != c) {
                    প্রত্যাগম অসত্য;
                }
                lastChar = c;
                প্রত্যাগম সত্য;
            }
        }

        তার্কিক consistentWith(ধ্রুব std::string_view substring) {
            প্রতি (ধ্রুব বর্ণ c : substring) {
                যদি (!consistentWith(c)) {
                    প্রত্যাগম অসত্য;
                }
            }
            প্রত্যাগম সত্য;
        }

        তার্কিক matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            প্রত্যাগম skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() ধ্রুব {
            প্রত্যাগম query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      গুপ্ত:
        ধ্রুব CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        বর্ণ lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        বর্ণ *skeletonLimit = skeletonData;
        প্রতি (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + পরিমাণ(skeletonData));
            যদি (skeletonLimit >= skeletonData + পরিমাণ(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                ভঙ্গ;
            }
            যদি (query[i] == ' ' || query[i] == '_') {
                অনুবর্ত;
            }
            যদি (query[i] == '-') {
                তার্কিক isMedial;
                তার্কিক is1180MedialHyphen = অসত্য;
                যদি (i == 0 || i == query.length() - 1) {
                    isMedial = অসত্য;
                } অন্যথা {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                যদি (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = সত্য;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    প্রতি (std::size_t j = i + 2; j < query.length(); ++j) {
                        যদি (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = অসত্য;
                        }
                    }
                }
                যদি (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } অন্যথা {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ধ্রুব {
        প্রত্যাগম Matcher(*এতত্);
    }

গুপ্ত:
    স্থির তার্কিক isASCIILetterOrDigit(ধ্রুব বর্ণ c) {
        প্রত্যাগম uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    বর্ণ skeletonData[120];
    std::string_view skeleton;
    তার্কিক is1180;
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
#define GET_GROUPS(names) (ধ্রুব uint16_t *)((ধ্রুব বর্ণ *)names+names->groupsOffset)

প্রকারনির্দেশ সংরচনা {
    ধ্রুব CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

স্থির UDataMemory *uCharNamesData=nullptr;
স্থির UCharNames *uCharNames=nullptr;
স্থির icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
স্থির int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
স্থির uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_গুনতি
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_গুনতি + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_গুনতি + 2

#define U_CHAR_EXTENDED_CATEGORY_গুনতি (U_CHAR_CATEGORY_গুনতি + 3)

স্থির ধ্রুব বর্ণ * ধ্রুব charCatNames[U_CHAR_EXTENDED_CATEGORY_গুনতি] = {
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

স্থির UBool U_CALLCONV unames_cleanup()
{
    যদি(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    যদি(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    প্রত্যাগম সত্য;
}

স্থির UBool U_CALLCONV
isAcceptable(শূন্য * /*context*/,
             ধ্রুব বর্ণ * /*type*/, ধ্রুব বর্ণ * /*name*/,
             ধ্রুব UDataInfo *pInfo) {
    প্রত্যাগম
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

স্থির শূন্য U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    যদি(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } অন্যথা {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


স্থির UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    প্রত্যাগম U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    যদি((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_গুনতি

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
স্থির uint16_t
expandName(UCharNames *names,
           ধ্রুব uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           বর্ণ *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_গুনতি=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    যদি(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        যদি (static_cast<uint8_t>(';') >= token_গুনতি || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            পূর্ণাঙ্ক fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            কুরু {
                যাবত্(nameLength>0) {
                    --nameLength;
                    যদি(*name++==';') {
                        ভঙ্গ;
                    }
                }
            } যাবত্(--fieldIndex>0);
        } অন্যথা {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    যাবত্(nameLength>0) {
        --nameLength;
        c=*name++;

        যদি(c>=token_গুনতি) {
            যদি(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } অন্যথা {
                /* finished */
                ভঙ্গ;
            }
        } অন্যথা {
            token=tokens[c];
            যদি (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            যদি (token == static_cast<uint16_t>(-1)) {
                যদি(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } অন্যথা {
                    /* finished */
                    ভঙ্গ;
                }
            } অন্যথা {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                যাবত্((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    যদি(bufferLength>0) {
        *buffer=0;
    }

    প্রত্যাগম bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
স্থির UBool
compareName(UCharNames *names,
            ধ্রুব uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            ধ্রুব CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_গুনতি=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    যদি(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        যদি (static_cast<uint8_t>(';') >= token_গুনতি || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            পূর্ণাঙ্ক fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            কুরু {
                যাবত্(nameLength>0) {
                    --nameLength;
                    যদি(*name++==';') {
                        ভঙ্গ;
                    }
                }
            } যাবত্(--fieldIndex>0);
        } অন্যথা {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    যাবত্(nameLength>0) {
        --nameLength;
        c=*name++;

        যদি(c>=token_গুনতি) {
            যদি(c!=';') {
                /* implicit letter */
                যদি (!matcher.consistentWith(c)) {
                    প্রত্যাগম অসত্য;
                }
            } অন্যথা {
                /* finished */
                ভঙ্গ;
            }
        } অন্যথা {
            token=tokens[c];
            যদি (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            যদি (token == static_cast<uint16_t>(-1)) {
                যদি(c!=';') {
                    /* explicit letter */
                    যদি (!matcher.consistentWith(c)) {
                        প্রত্যাগম অসত্য;
                    }
                } অন্যথা {
                    /* finished */
                    ভঙ্গ;
                }
            } অন্যথা {
                /* write token word */
                যদি (!matcher.consistentWith(reinterpret_cast<ধ্রুব বর্ণ *>(tokenStrings + token))) {
                    প্রত্যাগম অসত্য;
                }
            }
        }
    }

    /* complete match? */
    প্রত্যাগম matcher.matches();
}

স্থির uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    যদি (U_IS_UNICODE_NONCHAR(cp)) {
        প্রত্যাগম U_NONCHARACTER_CODE_POINT;
    }

    যদি ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    প্রত্যাগম cat;
}

স্থির ধ্রুব বর্ণ *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    যদি (cat >= UPRV_LENGTHOF(charCatNames)) {
        প্রত্যাগম "unknown";
    } অন্যথা {
        প্রত্যাগম charCatNames[cat];
    }
}

স্থির uint16_t getExtName(uint32_t code, বর্ণ *buffer, uint16_t bufferLength) {
    ধ্রুব বর্ণ *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    পূর্ণাঙ্ক ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    যাবত্ (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    প্রতি (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    যদি (ndigits < 4)
        ndigits = 4;
    প্রতি (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    প্রত্যাগম length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
স্থির ধ্রুব uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    ধ্রুব uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    যাবত্(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        যদি(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } অন্যথা {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    প্রত্যাগম groups+start*GROUP_LENGTH;
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
স্থির ধ্রুব uint8_t *
expandGroupLengths(ধ্রুব uint8_t *s,
                   uint16_t offsets[সারিগুলি_PER_GROUP+1], uint16_t lengths[সারিগুলি_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    যাবত্(i<সারিগুলি_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        যদি(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } অন্যথা যদি((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } অন্যথা {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        যদি((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            যদি(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } অন্যথা {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    প্রত্যাগম s;
}

স্থির uint16_t
expandGroupName(UCharNames *names, ধ্রুব uint16_t *group,
                uint16_t সারি_সংখ্যা, UCharNameChoice nameChoice,
                বর্ণ *buffer, uint16_t bufferLength) {
    uint16_t offsets[সারিগুলি_PER_GROUP+2], lengths[সারিগুলি_PER_GROUP+2];
    ধ্রুব uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    প্রত্যাগম expandName(names, s+offsets[সারি_সংখ্যা], lengths[সারি_সংখ্যা], nameChoice,
                      buffer, bufferLength);
}

স্থির uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        বর্ণ *buffer, uint16_t bufferLength) {
    ধ্রুব uint16_t *group=getGroup(names, code);
    যদি (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        প্রত্যাগম expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } অন্যথা {
        /* group not found */
        /* zero-terminate */
        যদি(bufferLength>0) {
            *buffer=0;
        }
        প্রত্যাগম 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
স্থির UBool
enumGroupNames(UCharNames *names, ধ্রুব uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, শূন্য *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[সারিগুলি_PER_GROUP+2], lengths[সারিগুলি_PER_GROUP+2];
    ধ্রুব uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    যদি(fn!=DO_FIND_NAME) {
        বর্ণ buffer[200];
        uint16_t length;

        যাবত্(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, পরিমাণ(buffer));
            যদি (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, পরিমাণ(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            যদি(length>0) {
                যদি(!fn(context, start, nameChoice, buffer, length)) {
                    প্রত্যাগম অসত্য;
                }
            }
            ++start;
        }
    } অন্যথা {
        ধ্রুব CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        যাবত্(start<=end) {
            যদি(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                প্রত্যাগম অসত্য;
            }
            ++start;
        }
    }
    প্রত্যাগম সত্য;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
স্থির UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, শূন্য *context)
{
    যদি(fn!=DO_FIND_NAME) {
        বর্ণ buffer[200];
        uint16_t length;
        
        যাবত্(start<=end) {
            buffer[length = getExtName(start, buffer, পরিমাণ(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            যদি(length>0) {
                যদি(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    প্রত্যাগম অসত্য;
                }
            }
            ++start;
        }
    }

    প্রত্যাগম সত্য;
}

স্থির UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, শূন্য *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_গুনতি;
    ধ্রুব uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    যদি(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        যদি(extLimit>limit) {
            extLimit=limit;
        }
        যদি(!enumExtNames(start, extLimit-1, fn, context)) {
            প্রত্যাগম অসত্য;
        }
        start=extLimit;
    }

    যদি(startGroupMSB==endGroupMSB) {
        যদি(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            প্রত্যাগম enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } অন্যথা {
        ধ্রুব uint16_t *groups=GET_GROUPS(names);
        group_গুনতি=*groups++;
        groupLimit=groups+group_গুনতি*GROUP_LENGTH;

        যদি(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            যদি((start&GROUP_MASK)!=0) {
                যদি(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + সারিগুলি_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    প্রত্যাগম অসত্য;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } অন্যথা যদি(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ধ্রুব uint16_t *nextGroup=NEXT_GROUP(group);
            যদি (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                যদি (end > limit) {
                    end = limit;
                }
                যদি (!enumExtNames(start, end - 1, fn, context)) {
                    প্রত্যাগম অসত্য;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        যাবত্(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            ধ্রুব uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            যদি(!enumGroupNames(names, group, start, start+সারিগুলি_PER_GROUP-1, fn, context, nameChoice)) {
                প্রত্যাগম অসত্য;
            }
            nextGroup=NEXT_GROUP(group);
            যদি (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                যদি (end > limit) {
                    end = limit;
                }
                যদি (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    প্রত্যাগম অসত্য;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        যদি(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            প্রত্যাগম enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } অন্যথা যদি (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            যদি (next > start) {
                start = next;
            }
        } অন্যথা {
            প্রত্যাগম সত্য;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    যদি (nameChoice == U_EXTENDED_CHAR_NAME) {
        যদি (limit > UCHAR_MAX_মান + 1) {
            limit = UCHAR_MAX_মান + 1;
        }
        প্রত্যাগম enumExtNames(start, limit - 1, fn, context);
    }
    
    প্রত্যাগম সত্য;
}

স্থির uint16_t
writeFactorSuffix(ধ্রুব uint16_t *factors, uint16_t count,
                  ধ্রুব বর্ণ *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  ধ্রুব বর্ণ *elementBases[8], ধ্রুব বর্ণ *elements[8],
                  বর্ণ *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    বর্ণ c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    প্রতি(i=count; i>0; --i) {
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
    প্রতি(;;) {
        যদি(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        যাবত্(factor>0) {
            যাবত্(*s++!=0) {}
            --factor;
        }
        যদি(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        যাবত্((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        যদি(i>=count) {
            ভঙ্গ;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        যাবত্(factor>0) {
            যাবত্(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    যদি(bufferLength>0) {
        *buffer=0;
    }

    প্রত্যাগম bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
স্থির uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        বর্ণ *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    যদি(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        যদি(bufferLength>0) {
            *buffer=0;
        }
        প্রত্যাগম 0;
    }

    বিকল্প(range->type) {
    পক্ষ 0: {
        /* name = prefix hex-digits */
        ধ্রুব বর্ণ* s = reinterpret_cast<ধ্রুব বর্ণ*>(range + 1);
        বর্ণ c;

        uint16_t i, count;

        /* copy prefix */
        যাবত্((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        যদি(count<bufferLength) {
            buffer[count]=0;
        }

        প্রতি(i=count; i>0;) {
            যদি(--i<bufferLength) {
                c = static_cast<বর্ণ>(code & 0xf);
                যদি(c<10) {
                    c+='0';
                } অন্যথা {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        ভঙ্গ;
    }
    পক্ষ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ধ্রুব uint16_t* factors = reinterpret_cast<ধ্রুব uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ধ্রুব বর্ণ* s = reinterpret_cast<ধ্রুব বর্ণ*>(factors + count);
        বর্ণ c;

        /* copy prefix */
        যাবত্((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        ভঙ্গ;
    }
    স্বভাব:
        /* undefined type */
        /* zero-terminate */
        যদি(bufferLength>0) {
            *buffer=0;
        }
        ভঙ্গ;
    }

    প্রত্যাগম bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
স্থির UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, শূন্য *context,
             UCharNameChoice nameChoice) {
    বর্ণ buffer[200];
    uint16_t length;

    যদি(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        প্রত্যাগম সত্য;
    }

    বিকল্প(range->type) {
    পক্ষ 0: {
        বর্ণ *s, *end;
        বর্ণ c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, পরিমাণ(buffer));
        যদি(length<=0) {
            প্রত্যাগম সত্য;
        }

        /* call the enumerator function with this first character */
        যদি(!fn(context, start, nameChoice, buffer, length)) {
            প্রত্যাগম অসত্য;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        যাবত্(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        যাবত্(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            প্রতি (;;) {
                c=*--s;
                যদি(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<বর্ণ>(c + 1);
                    ভঙ্গ;
                } অন্যথা যদি(c=='9') {
                    *s='A';
                    ভঙ্গ;
                } অন্যথা যদি(c=='F') {
                    *s='0';
                }
            }

            যদি(!fn(context, start, nameChoice, buffer, length)) {
                প্রত্যাগম অসত্য;
            }
        }
        ভঙ্গ;
    }
    পক্ষ 1: {
        uint16_t indexes[8];
        ধ্রুব বর্ণ *elementBases[8], *elements[8];
        ধ্রুব uint16_t* factors = reinterpret_cast<ধ্রুব uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ধ্রুব বর্ণ* s = reinterpret_cast<ধ্রুব বর্ণ*>(factors + count);
        বর্ণ *suffix, *t;
        uint16_t prefixLength, i, idx;

        বর্ণ c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        যাবত্((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(পরিমাণ(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        যদি(!fn(context, start, nameChoice, buffer, length)) {
            প্রত্যাগম অসত্য;
        }

        /* enumerate the rest of the names */
        যাবত্(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            প্রতি (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                যদি(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    যাবত্(*s++!=0) {
                    }
                    elements[i]=s;
                    ভঙ্গ;
                } অন্যথা {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            প্রতি(i=0; i<count; ++i) {
                s=elements[i];
                যাবত্((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            যদি(!fn(context, start, nameChoice, buffer, length)) {
                প্রত্যাগম অসত্য;
            }
        }
        ভঙ্গ;
    }
    স্বভাব:
        /* undefined type */
        ভঙ্গ;
    }

    প্রত্যাগম সত্য;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
স্থির UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    যদি(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        প্রত্যাগম 0xffff;
    }

    বিকল্প(range->type) {
    পক্ষ 0: {
        /* name = prefix hex-digits */
        ধ্রুব বর্ণ* s = reinterpret_cast<ধ্রুব বর্ণ*>(range + 1);

        /* compare prefix */
        যদি (!matcher.consistentWith(s)) {
            প্রত্যাগম 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        প্রতি(বর্ণ c : matcher.remainingSignificantCharacters()) {
            যদি('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } অন্যথা যদি('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } অন্যথা {
                প্রত্যাগম 0xffff;
            }
        }

        /* does it fit into the range? */
        যদি (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            প্রত্যাগম code;
        }
        ভঙ্গ;
    }
    পক্ষ 1: {
        বর্ণ buffer[64];
        uint16_t indexes[8];
        ধ্রুব বর্ণ *elementBases[8], *elements[8];
        ধ্রুব uint16_t* factors = reinterpret_cast<ধ্রুব uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ধ্রুব বর্ণ *s = reinterpret_cast<ধ্রুব বর্ণ*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        যদি (!matcher.consistentWith(prefix)) {
            প্রত্যাগম 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, পরিমাণ(buffer));

        /* compare the first suffix */
        যদি(querySuffix == buffer) {
            প্রত্যাগম start;
        }

        /* enumerate and compare the rest of the suffixes */
        যাবত্(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            প্রতি (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                যদি(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    যাবত্(*s++!=0) {}
                    elements[i]=s;
                    ভঙ্গ;
                } অন্যথা {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            প্রতি(i=0; i<count; ++i) {
                s=elements[i];
                বর্ণ c;
                যাবত্((c=*s++)!=0) {
                    যদি(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            যদি(i<99 && it == querySuffix.end()) {
                প্রত্যাগম start;
            }
        }
        ভঙ্গ;
    }
    স্বভাব:
        /* undefined type */
        ভঙ্গ;
    }

    প্রত্যাগম 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

স্থির int32_t
calcStringSetLength(uint32_t set[8], ধ্রুব বর্ণ *s) {
    int32_t length=0;
    বর্ণ c;

    যাবত্((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    প্রত্যাগম length;
}

স্থির int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_গুনতি;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_গুনতি=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    যাবত্(range_গুনতি>0) {
        বিকল্প(range->type) {
        পক্ষ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<ধ্রুব বর্ণ*>(range + 1)) + range->variant;
            যদি(length>maxNameLength) {
                maxNameLength=length;
            }
            ভঙ্গ;
        পক্ষ 1: {
            /* name = prefix factorized-elements */
            ধ্রুব uint16_t* factors = reinterpret_cast<ধ্রুব uint16_t*>(range + 1);
            ধ্রুব বর্ণ *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<ধ্রুব বর্ণ*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            প্রতি(i=0; i<count; ++i) {
                maxFactorLength=0;
                প্রতি(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    যদি(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            যদি(length>maxNameLength) {
                maxNameLength=length;
            }
            ভঙ্গ;
        }
        স্বভাব:
            /* unknown type */
            ভঙ্গ;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_গুনতি;
    }
    প্রত্যাগম maxNameLength;
}

স্থির int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    প্রতি(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        যদি(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    প্রত্যাগম maxNameLength;
}

স্থির int32_t
calcNameSetLength(ধ্রুব uint16_t *tokens, uint16_t token_গুনতি, ধ্রুব uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  ধ্রুব uint8_t **p_সারি, ধ্রুব uint8_t *সারি_Limit) {
    ধ্রুব uint8_t *সারি=*p_সারি;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    যাবত্ (সারি != সারি_Limit && (c = *সারি++) != static_cast<uint8_t>(';')) {
        যদি(c>=token_গুনতি) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } অন্যথা {
            token=tokens[c];
            যদি (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*সারি++;
                token=tokens[c];
            }
            যদি (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } অন্যথা {
                /* count token word */
                যদি(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    যদি(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<ধ্রুব বর্ণ*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } অন্যথা {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<ধ্রুব বর্ণ*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_সারি=সারি;
    প্রত্যাগম length;
}

স্থির শূন্য
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[সারিগুলি_PER_GROUP+2], lengths[সারিগুলি_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_গুনতি=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    ধ্রুব uint16_t *group;
    ধ্রুব uint8_t *s, *সারি, *সারি_Limit;

    int32_t group_গুনতি, সারি_সংখ্যা, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_গুনতি));
    যদি(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_গুনতি);
    }

    group=GET_GROUPS(uCharNames);
    group_গুনতি=*group++;

    /* enumerate all groups */
    যাবত্(group_গুনতি>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        প্রতি(সারি_সংখ্যা=0; সারি_সংখ্যা<সারিগুলি_PER_GROUP; ++সারি_সংখ্যা) {
            সারি=s+offsets[সারি_সংখ্যা];
            length=lengths[সারি_সংখ্যা];
            যদি(length==0) {
                অনুবর্ত;
            }

            সারি_Limit=সারি+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_গুনতি, tokenStrings, tokenLengths, gNameSet, &সারি, সারি_Limit);
            যদি(length>maxNameLength) {
                maxNameLength=length;
            }
            যদি(সারি==সারি_Limit) {
                অনুবর্ত;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_গুনতি, tokenStrings, tokenLengths, gNameSet, &সারি, সারি_Limit);
            যদি(length>maxNameLength) {
                maxNameLength=length;
            }
            যদি(সারি==সারি_Limit) {
                অনুবর্ত;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_গুনতি;
    }

    যদি(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

স্থির UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    স্থির ধ্রুব বর্ণ extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    যদি(gMaxNameLength!=0) {
        প্রত্যাগম সত্য;
    }

    যদি(!isDataLoaded(pErrorCode)) {
        প্রত্যাগম অসত্য;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    প্রতি (i = 0; i < static_cast<int32_t>(পরিমাণ(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    প্রত্যাগম সত্য;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           বর্ণ *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    } অন্যথা যদি(nameChoice>=U_CHAR_NAME_CHOICE_গুনতি ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    যদি((uint32_t)code>UCHAR_MAX_মান || !isDataLoaded(pErrorCode)) {
        প্রত্যাগম u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    যাবত্(i>0) {
        যদি(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            ভঙ্গ;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    যদি(i==0) {
        যদি (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            যদি (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } অন্যথা {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    প্রত্যাগম u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                বর্ণ *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    } অন্যথা যদি(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম 0;
    }

    প্রত্যাগম u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ধ্রুব বর্ণ *ধ্রুব name,
               UErrorCode *pErrorCode) {
    বর্ণ lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    বর্ণ c0;
    স্থির constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম error;
    }

    যদি(nameChoice>=U_CHAR_NAME_CHOICE_গুনতি || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম error;
    }

    যদি(!isDataLoaded(pErrorCode)) {
        প্রত্যাগম error;
    }

    /* construct the lowercase of the name first */
    ধ্রুব বর্ণ* nameChar = name;
    প্রতি(i=0; i<পরিমাণ(lower); ++i) {
        যদি((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } অন্যথা {
            ভঙ্গ;
        }
    }
    যদি(i==পরিমাণ(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        প্রত্যাগম error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    যদি (lower[0] == '<') {
        যদি (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            যাবত্ (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            যদি (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                প্রতি (++i; i < limit; ++i) {
                    যদি (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } অন্যথা যদি (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } অন্যথা {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        প্রত্যাগম error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    যদি (cp > UCHAR_MAX_মান) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        প্রত্যাগম error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                প্রতি (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    যদি (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        যদি (cat == cIdx) {
                            প্রত্যাগম cp;
                        }
                        ভঙ্গ;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        প্রত্যাগম error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    যাবত্(i>0) {
        যদি((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            প্রত্যাগম cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_মান + 1, DO_FIND_NAME, &findName, nameChoice);
    যদি (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    প্রত্যাগম findName.code;
}

U_CAPI শূন্য U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                শূন্য *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম;
    }

    যদি(nameChoice>=U_CHAR_NAME_CHOICE_গুনতি || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        প্রত্যাগম;
    }

    যদি((uint32_t) limit > UCHAR_MAX_মান + 1) {
        limit = UCHAR_MAX_মান + 1;
    }
    যদি((uint32_t)start>=(uint32_t)limit) {
        প্রত্যাগম;
    }

    যদি(!isDataLoaded(pErrorCode)) {
        প্রত্যাগম;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    যাবত্(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        যদি((uint32_t)start<algRange->start) {
            যদি((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                প্রত্যাগম;
            }
            যদি(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                প্রত্যাগম;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        যদি((uint32_t)start<=algRange->end) {
            যদি((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                প্রত্যাগম;
            }
            যদি(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                প্রত্যাগম;
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
    যদি(calcNameSetsLengths(&errorCode)) {
        প্রত্যাগম gMaxNameLength;
    } অন্যথা {
        প্রত্যাগম 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
স্থির শূন্য
charSetToUSet(uint32_t cset[8], ধ্রুব USetAdder *sa) {
    char16_t us[256];
    বর্ণ cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    যদি(!calcNameSetsLengths(&errorCode)) {
        প্রত্যাগম;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    প্রতি(i=0; i<256; ++i) {
        যদি(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<বর্ণ>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    প্রতি(i=0; i<length; ++i) {
        যদি(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI শূন্য U_EXPORT2
uprv_getCharNameCharacters(ধ্রুব USetAdder *sa) {
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
স্থির শূন্য
makeTokenMap(ধ্রুব UDataSwapper *ds,
             int16_t tokens[], uint16_t token_গুনতি,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    যদি(U_FAILURE(*pErrorCode)) {
        প্রত্যাগম;
    }

    যদি(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        প্রতি(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } অন্যথা {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        যদি(token_গুনতি>256) {
            token_গুনতি=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        প্রতি(i=1; i<token_গুনতি; ++i) {
            যদি(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    প্রত্যাগম;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=সত্য;
            }
        }

        /* set the mappings for the rest of the permutation */
        প্রতি(i=j=1; i<token_গুনতি; ++i) {
            /* set mappings that were not set for direct bytes */
            যদি(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                যাবত্(usedOutChar[j]) {
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
uchar_swapNames(ধ্রুব UDataSwapper *ds,
                ধ্রুব শূন্য *inData, int32_t length, শূন্য *outData,
                UErrorCode *pErrorCode) {
    ধ্রুব UDataInfo *pInfo;
    int32_t headerSize;

    ধ্রুব uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_গুনতি;

    ধ্রুব AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    যদি(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        প্রত্যাগম 0;
    }

    /* check data format and format version */
    pInfo=(ধ্রুব UDataInfo *)((ধ্রুব বর্ণ *)inData+4);
    যদি(!(
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
        প্রত্যাগম 0;
    }

    inBytes=(ধ্রুব uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    যদি(length<0) {
        algNamesOffset=ds->readUInt32(((ধ্রুব uint32_t *)inBytes)[3]);
    } অন্যথা {
        length-=headerSize;
        যদি( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((ধ্রুব uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            প্রত্যাগম 0;
        }
    }

    যদি(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ধ্রুব uint32_t *)(inBytes+offset)));
        offset+=4;

        প্রতি(i=0; i<count; ++i) {
            inRange=(ধ্রুব AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } অন্যথা {
        /* swap data */
        ধ্রুব uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_গুনতি;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        যদি(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((ধ্রুব uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((ধ্রুব uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((ধ্রুব uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ধ্রুব uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_গুনতি=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        যদি(token_গুনতি<=512) {
            count=token_গুনতি;
        } অন্যথা {
            count=512;
        }
        প্রতি(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        প্রতি(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_গুনতি, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_গুনতি>256 ? token_গুনতি-256 : 0), trailMap, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            প্রত্যাগম 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_গুনতি*2);
        যদি(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_গুনতি);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            প্রত্যাগম 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        প্রতি(i=0; i<token_গুনতি && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        প্রতি(; i<token_গুনতি; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_গুনতি*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            প্রত্যাগম 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ধ্রুব uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        যদি(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[সারিগুলি_PER_GROUP+1], lengths[সারিগুলি_PER_GROUP+1];

            ধ্রুব uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_গুনতি=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            যাবত্(strings_গুনতি>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_গুনতি-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_গুনতি-=count;

                /* swap the string bytes using map[] and trailMap[] */
                যাবত্(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    যদি(tokens[c]!=-2) {
                        --count;
                    } অন্যথা {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((ধ্রুব uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        প্রতি(i=0; i<count; ++i) {
            যদি(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                প্রত্যাগম 0;
            }

            inRange=(ধ্রুব AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            বিকল্প(inRange->type) {
            পক্ষ 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((ধ্রুব বর্ণ *)(inRange+1)),
                                    outRange+1, pErrorCode);
                যদি(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    প্রত্যাগম 0;
                }
                ভঙ্গ;
            পক্ষ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_গুনতি;

                    factors_গুনতি=inRange->variant;
                    p=(ধ্রুব uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_গুনতি*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_গুনতি;
                    q+=factors_গুনতি;
                    strings_গুনতি=(uint32_t)((inBytes+offset)-(ধ্রুব uint8_t *)p);
                    যাবত্(strings_গুনতি>0 && ((ধ্রুব uint8_t *)p)[strings_গুনতি-1]!=0) {
                        --strings_গুনতি;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_গুনতি, q, pErrorCode);
                }
                ভঙ্গ;
            স্বভাব:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                প্রত্যাগম 0;
            }
        }
    }

    প্রত্যাগম headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
