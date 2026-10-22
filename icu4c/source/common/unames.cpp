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

#અન્તર્ભાવ "unicode/utypes.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "unicode/uchar.h"
#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/utf.h"
#અન્તર્ભાવ "unicode/utf16.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ustr_imp.h"
#અન્તર્ભાવ "umutex.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "ucln_cmn.h"
#અન્તર્ભાવ "udataswp.h"
#અન્તર્ભાવ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

સ્થિર અચલ અક્ષર DATA_NAME[] = "unames";
સ્થિર અચલ અક્ષર DATA_TYPE[] = "icu";

#define GROUP_SHIFT 5
#define લીટીઓ_PER_GROUP (1L<<GROUP_SHIFT)
#define GROUP_MASK (લીટીઓ_PER_GROUP-1)

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
ગણના {
    GROUP_MSB,
    GROUP_OFFSET_ઉપલું,
    GROUP_OFFSET_નીચલું,
    GROUP_LENGTH
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#define GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_ઉપલું]<<16|(group)[GROUP_OFFSET_નીચલું])

#define NEXT_GROUP(group) ((group)+GROUP_LENGTH)
#define PREV_GROUP(group) ((group)-GROUP_LENGTH)

પ્રકારવ્યાખ્યા સંરચના {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

પ્રકારવ્યાખ્યા સંરચના {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

નામાકાશ {
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
વર્ગ CharacterNameQuery {
  જાહેર:
    વર્ગ Matcher {
      જાહેર:
        // `query` must outlive the constructed object.
        Matcher(અચલ CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        તાર્કિક consistentWith(અક્ષર c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            જો (skeletonIterator == query.skeleton.end()) {
                પાછા ખોટું;
            }
            જો (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                જો (lastChar == '-') {
                    જો (*skeletonIterator++ != lastChar) {
                        પાછા ખોટું;
                    }
                }
                lastChar = c;
                પાછા સાચું;
            } નહીંતર જો (c == '-') {
                જો (lastChar == ' ' ||
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
                    જો (*skeletonIterator++ != c) {
                        પાછા ખોટું;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                પાછા સાચું;
            } નહીંતર {
                જો (*skeletonIterator++ != c) {
                    પાછા ખોટું;
                }
                lastChar = c;
                પાછા સાચું;
            }
        }

        તાર્કિક consistentWith(અચલ std::string_view substring) {
            માટે (અચલ અક્ષર c : substring) {
                જો (!consistentWith(c)) {
                    પાછા ખોટું;
                }
            }
            પાછા સાચું;
        }

        તાર્કિક matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            પાછા skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() અચલ {
            પાછા query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      ખાનગી:
        અચલ CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        અક્ષર lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        અક્ષર *skeletonLimit = skeletonData;
        માટે (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeletonData + કદ(skeletonData));
            જો (skeletonLimit >= skeletonData + કદ(skeletonData)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeletonData;
                તોડો;
            }
            જો (query[i] == ' ' || query[i] == '_') {
                ચાલુ;
            }
            જો (query[i] == '-') {
                તાર્કિક isMedial;
                તાર્કિક is1180MedialHyphen = ખોટું;
                જો (i == 0 || i == query.length() - 1) {
                    isMedial = ખોટું;
                } નહીંતર {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                જો (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeletonData, skeletonLimit - skeletonData) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = સાચું;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    માટે (std::size_t j = i + 2; j < query.length(); ++j) {
                        જો (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = ખોટું;
                        }
                    }
                }
                જો (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } નહીંતર {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeletonData, skeletonLimit - skeletonData);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() અચલ {
        પાછા Matcher(*આ);
    }

ખાનગી:
    સ્થિર તાર્કિક isASCIILetterOrDigit(અચલ અક્ષર c) {
        પાછા uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    અક્ષર skeletonData[120];
    std::string_view skeleton;
    તાર્કિક is1180;
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
#define GET_GROUPS(names) (અચલ uint16_t *)((અચલ અક્ષર *)names+names->groupsOffset)

પ્રકારવ્યાખ્યા સંરચના {
    અચલ CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#define DO_FIND_NAME nullptr

સ્થિર UDataMemory *uCharNamesData=nullptr;
સ્થિર UCharNames *uCharNames=nullptr;
સ્થિર icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
સ્થિર int32_t gMaxNameLength=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
સ્થિર uint32_t gNameSet[8]={ 0 };

#define U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_ગણતરી
#define U_LEAD_SURROGATE U_CHAR_CATEGORY_ગણતરી + 1
#define U_TRAIL_SURROGATE U_CHAR_CATEGORY_ગણતરી + 2

#define U_CHAR_EXTENDED_CATEGORY_ગણતરી (U_CHAR_CATEGORY_ગણતરી + 3)

સ્થિર અચલ અક્ષર * અચલ charCatNames[U_CHAR_EXTENDED_CATEGORY_ગણતરી] = {
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

સ્થિર UBool U_CALLCONV unames_cleanup()
{
    જો(uCharNamesData) {
        udata_close(uCharNamesData);
        uCharNamesData = nullptr;
    }
    જો(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    gMaxNameLength=0;
    પાછા સાચું;
}

સ્થિર UBool U_CALLCONV
isAcceptable(શૂન્ય * /*context*/,
             અચલ અક્ષર * /*type*/, અચલ અક્ષર * /*name*/,
             અચલ UDataInfo *pInfo) {
    પાછા
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->dataFormat[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->dataFormat[1]==0x6e &&
        pInfo->dataFormat[2]==0x61 &&
        pInfo->dataFormat[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

સ્થિર શૂન્ય U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(uCharNamesData == nullptr);
    U_ASSERT(uCharNames == nullptr);

    uCharNamesData = udata_openChoice(nullptr, DATA_TYPE, DATA_NAME, isAcceptable, nullptr, &status);
    જો(U_FAILURE(status)) {
        uCharNamesData = nullptr;
    } નહીંતર {
        uCharNames = (UCharNames *)udata_getMemory(uCharNamesData);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


સ્થિર UBool
isDataLoaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    પાછા U_SUCCESS(*pErrorCode);
}

#define WRITE_CHAR(buffer, bufferLength, bufferPos, c) UPRV_BLOCK_MACRO_BEGIN { \
    જો((bufferLength)>0) { \
        *(buffer)++=c; \
        --(bufferLength); \
    } \
    ++(bufferPos); \
} UPRV_BLOCK_MACRO_END

#define U_ISO_COMMENT U_CHAR_NAME_CHOICE_ગણતરી

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
સ્થિર uint16_t
expandName(UCharNames *names,
           અચલ uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
           અક્ષર *buffer, uint16_t bufferLength) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_ગણતરી=*tokens++, bufferPos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    જો(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        જો (static_cast<uint8_t>(';') >= token_ગણતરી || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            પૂર્ણાંક fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            કરો {
                જ્યારે(nameLength>0) {
                    --nameLength;
                    જો(*name++==';') {
                        તોડો;
                    }
                }
            } જ્યારે(--fieldIndex>0);
        } નહીંતર {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    જ્યારે(nameLength>0) {
        --nameLength;
        c=*name++;

        જો(c>=token_ગણતરી) {
            જો(c!=';') {
                /* implicit letter */
                WRITE_CHAR(buffer, bufferLength, bufferPos, c);
            } નહીંતર {
                /* finished */
                તોડો;
            }
        } નહીંતર {
            token=tokens[c];
            જો (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            જો (token == static_cast<uint16_t>(-1)) {
                જો(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                } નહીંતર {
                    /* finished */
                    તોડો;
                }
            } નહીંતર {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                જ્યારે((c=*tokenString++)!=0) {
                    WRITE_CHAR(buffer, bufferLength, bufferPos, c);
                }
            }
        }
    }

    /* zero-terminate */
    જો(bufferLength>0) {
        *buffer=0;
    }

    પાછા bufferPos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
સ્થિર UBool
compareName(UCharNames *names,
            અચલ uint8_t *name, uint16_t nameLength, UCharNameChoice nameChoice,
            અચલ CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_ગણતરી=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    auto matcher = otherName.matcher();

    જો(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        જો (static_cast<uint8_t>(';') >= token_ગણતરી || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            પૂર્ણાંક fieldIndex= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            કરો {
                જ્યારે(nameLength>0) {
                    --nameLength;
                    જો(*name++==';') {
                        તોડો;
                    }
                }
            } જ્યારે(--fieldIndex>0);
        } નહીંતર {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            nameLength=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    જ્યારે(nameLength>0) {
        --nameLength;
        c=*name++;

        જો(c>=token_ગણતરી) {
            જો(c!=';') {
                /* implicit letter */
                જો (!matcher.consistentWith(c)) {
                    પાછા ખોટું;
                }
            } નહીંતર {
                /* finished */
                તોડો;
            }
        } નહીંતર {
            token=tokens[c];
            જો (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --nameLength;
            }
            જો (token == static_cast<uint16_t>(-1)) {
                જો(c!=';') {
                    /* explicit letter */
                    જો (!matcher.consistentWith(c)) {
                        પાછા ખોટું;
                    }
                } નહીંતર {
                    /* finished */
                    તોડો;
                }
            } નહીંતર {
                /* write token word */
                જો (!matcher.consistentWith(reinterpret_cast<અચલ અક્ષર *>(tokenStrings + token))) {
                    પાછા ખોટું;
                }
            }
        }
    }

    /* complete match? */
    પાછા matcher.matches();
}

સ્થિર uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    જો (U_IS_UNICODE_NONCHAR(cp)) {
        પાછા U_NONCHARACTER_CODE_POINT;
    }

    જો ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    પાછા cat;
}

સ્થિર અચલ અક્ષર *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    જો (cat >= UPRV_LENGTHOF(charCatNames)) {
        પાછા "unknown";
    } નહીંતર {
        પાછા charCatNames[cat];
    }
}

સ્થિર uint16_t getExtName(uint32_t code, અક્ષર *buffer, uint16_t bufferLength) {
    અચલ અક્ષર *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    પૂર્ણાંક ndigits, i;
    
    WRITE_CHAR(buffer, bufferLength, length, '<');
    જ્યારે (catname[length - 1]) {
        WRITE_CHAR(buffer, bufferLength, length, catname[length - 1]);
    }
    WRITE_CHAR(buffer, bufferLength, length, '-');
    માટે (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    જો (ndigits < 4)
        ndigits = 4;
    માટે (cp = code, i = ndigits; (cp || i > 0) && bufferLength; cp >>= 4, bufferLength--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        buffer[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    buffer += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(buffer, bufferLength, length, '>');

    પાછા length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
સ્થિર અચલ uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    અચલ uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    જ્યારે(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        જો(groupMSB<groups[number*GROUP_LENGTH+GROUP_MSB]) {
            limit=number;
        } નહીંતર {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    પાછા groups+start*GROUP_LENGTH;
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
સ્થિર અચલ uint8_t *
expandGroupLengths(અચલ uint8_t *s,
                   uint16_t offsets[લીટીઓ_PER_GROUP+1], uint16_t lengths[લીટીઓ_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t lengthByte;

    /* all 32 lengths must be read to get the offset of the first group string */
    જ્યારે(i<લીટીઓ_PER_GROUP) {
        lengthByte=*s++;

        /* read even nibble - MSBs of lengthByte */
        જો(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | lengthByte >> 4) + 12);
            lengthByte&=0xf;
        } નહીંતર જો((lengthByte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((lengthByte & 0x3f) + 12);
        } નહીંતર {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(lengthByte >> 4);
            lengthByte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        જો((lengthByte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=lengthByte;
            જો(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } નહીંતર {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    પાછા s;
}

સ્થિર uint16_t
expandGroupName(UCharNames *names, અચલ uint16_t *group,
                uint16_t લીટી_સંખ્યા, UCharNameChoice nameChoice,
                અક્ષર *buffer, uint16_t bufferLength) {
    uint16_t offsets[લીટીઓ_PER_GROUP+2], lengths[લીટીઓ_PER_GROUP+2];
    અચલ uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    પાછા expandName(names, s+offsets[લીટી_સંખ્યા], lengths[લીટી_સંખ્યા], nameChoice,
                      buffer, bufferLength);
}

સ્થિર uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        અક્ષર *buffer, uint16_t bufferLength) {
    અચલ uint16_t *group=getGroup(names, code);
    જો (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        પાછા expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               buffer, bufferLength);
    } નહીંતર {
        /* group not found */
        /* zero-terminate */
        જો(bufferLength>0) {
            *buffer=0;
        }
        પાછા 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
સ્થિર UBool
enumGroupNames(UCharNames *names, અચલ uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, શૂન્ય *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[લીટીઓ_PER_GROUP+2], lengths[લીટીઓ_PER_GROUP+2];
    અચલ uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    જો(fn!=DO_FIND_NAME) {
        અક્ષર buffer[200];
        uint16_t length;

        જ્યારે(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, buffer, કદ(buffer));
            જો (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                buffer[length = getExtName(start, buffer, કદ(buffer))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            જો(length>0) {
                જો(!fn(context, start, nameChoice, buffer, length)) {
                    પાછા ખોટું;
                }
            }
            ++start;
        }
    } નહીંતર {
        અચલ CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        જ્યારે(start<=end) {
            જો(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                પાછા ખોટું;
            }
            ++start;
        }
    }
    પાછા સાચું;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
સ્થિર UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, શૂન્ય *context)
{
    જો(fn!=DO_FIND_NAME) {
        અક્ષર buffer[200];
        uint16_t length;
        
        જ્યારે(start<=end) {
            buffer[length = getExtName(start, buffer, કદ(buffer))] = 0;
            /* here, we assume that the buffer is large enough */
            જો(length>0) {
                જો(!fn(context, start, U_EXTENDED_CHAR_NAME, buffer, length)) {
                    પાછા ખોટું;
                }
            }
            ++start;
        }
    }

    પાછા સાચું;
}

સ્થિર UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, શૂન્ય *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_ગણતરી;
    અચલ uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    જો(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        જો(extLimit>limit) {
            extLimit=limit;
        }
        જો(!enumExtNames(start, extLimit-1, fn, context)) {
            પાછા ખોટું;
        }
        start=extLimit;
    }

    જો(startGroupMSB==endGroupMSB) {
        જો(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            પાછા enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } નહીંતર {
        અચલ uint16_t *groups=GET_GROUPS(names);
        group_ગણતરી=*groups++;
        groupLimit=groups+group_ગણતરી*GROUP_LENGTH;

        જો(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            જો((start&GROUP_MASK)!=0) {
                જો(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + લીટીઓ_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    પાછા ખોટું;
                }
                group=NEXT_GROUP(group); /* continue with the next group */
            }
        } નહીંતર જો(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            અચલ uint16_t *nextGroup=NEXT_GROUP(group);
            જો (nextGroup < groupLimit && nextGroup[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                જો (end > limit) {
                    end = limit;
                }
                જો (!enumExtNames(start, end - 1, fn, context)) {
                    પાછા ખોટું;
                }
            }
            group=nextGroup;
        }

        /* enumerate entire groups between the start- and end-groups */
        જ્યારે(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            અચલ uint16_t *nextGroup;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            જો(!enumGroupNames(names, group, start, start+લીટીઓ_PER_GROUP-1, fn, context, nameChoice)) {
                પાછા ખોટું;
            }
            nextGroup=NEXT_GROUP(group);
            જો (nextGroup < groupLimit && nextGroup[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = nextGroup[GROUP_MSB] << GROUP_SHIFT;
                જો (end > limit) {
                    end = limit;
                }
                જો (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    પાછા ખોટું;
                }
            }
            group=nextGroup;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        જો(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            પાછા enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } નહીંતર જો (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (PREV_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            જો (next > start) {
                start = next;
            }
        } નહીંતર {
            પાછા સાચું;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    જો (nameChoice == U_EXTENDED_CHAR_NAME) {
        જો (limit > UCHAR_MAX_કિંમત + 1) {
            limit = UCHAR_MAX_કિંમત + 1;
        }
        પાછા enumExtNames(start, limit - 1, fn, context);
    }
    
    પાછા સાચું;
}

સ્થિર uint16_t
writeFactorSuffix(અચલ uint16_t *factors, uint16_t count,
                  અચલ અક્ષર *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  અચલ અક્ષર *elementBases[8], અચલ અક્ષર *elements[8],
                  અક્ષર *buffer, uint16_t bufferLength) {
    uint16_t i, factor, bufferPos=0;
    અક્ષર c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    માટે(i=count; i>0; --i) {
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
    માટે(;;) {
        જો(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        જ્યારે(factor>0) {
            જ્યારે(*s++!=0) {}
            --factor;
        }
        જો(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        જ્યારે((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        જો(i>=count) {
            તોડો;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        જ્યારે(factor>0) {
            જ્યારે(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    જો(bufferLength>0) {
        *buffer=0;
    }

    પાછા bufferPos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
સ્થિર uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        અક્ષર *buffer, uint16_t bufferLength) {
    uint16_t bufferPos=0;

    /* Only the normative character name can be algorithmic. */
    જો(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        જો(bufferLength>0) {
            *buffer=0;
        }
        પાછા 0;
    }

    પસંદગી(range->type) {
    સ્થિતિ 0: {
        /* name = prefix hex-digits */
        અચલ અક્ષર* s = reinterpret_cast<અચલ અક્ષર*>(range + 1);
        અક્ષર c;

        uint16_t i, count;

        /* copy prefix */
        જ્યારે((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        જો(count<bufferLength) {
            buffer[count]=0;
        }

        માટે(i=count; i>0;) {
            જો(--i<bufferLength) {
                c = static_cast<અક્ષર>(code & 0xf);
                જો(c<10) {
                    c+='0';
                } નહીંતર {
                    c+='A'-10;
                }
                buffer[i]=c;
            }
            code>>=4;
        }

        bufferPos+=count;
        તોડો;
    }
    સ્થિતિ 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        અચલ uint16_t* factors = reinterpret_cast<અચલ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        અચલ અક્ષર* s = reinterpret_cast<અચલ અક્ષર*>(factors + count);
        અક્ષર c;

        /* copy prefix */
        જ્યારે((c=*s++)!=0) {
            WRITE_CHAR(buffer, bufferLength, bufferPos, c);
        }

        bufferPos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, buffer, bufferLength);
        તોડો;
    }
    મૂળભૂત:
        /* undefined type */
        /* zero-terminate */
        જો(bufferLength>0) {
            *buffer=0;
        }
        તોડો;
    }

    પાછા bufferPos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
સ્થિર UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, શૂન્ય *context,
             UCharNameChoice nameChoice) {
    અક્ષર buffer[200];
    uint16_t length;

    જો(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        પાછા સાચું;
    }

    પસંદગી(range->type) {
    સ્થિતિ 0: {
        અક્ષર *s, *end;
        અક્ષર c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, buffer, કદ(buffer));
        જો(length<=0) {
            પાછા સાચું;
        }

        /* call the enumerator function with this first character */
        જો(!fn(context, start, nameChoice, buffer, length)) {
            પાછા ખોટું;
        }

        /* go to the end of the name; all these names have the same length */
        end=buffer;
        જ્યારે(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        જ્યારે(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            માટે (;;) {
                c=*--s;
                જો(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<અક્ષર>(c + 1);
                    તોડો;
                } નહીંતર જો(c=='9') {
                    *s='A';
                    તોડો;
                } નહીંતર જો(c=='F') {
                    *s='0';
                }
            }

            જો(!fn(context, start, nameChoice, buffer, length)) {
                પાછા ખોટું;
            }
        }
        તોડો;
    }
    સ્થિતિ 1: {
        uint16_t indexes[8];
        અચલ અક્ષર *elementBases[8], *elements[8];
        અચલ uint16_t* factors = reinterpret_cast<અચલ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        અચલ અક્ષર* s = reinterpret_cast<અચલ અક્ષર*>(factors + count);
        અક્ષર *suffix, *t;
        uint16_t prefixLength, i, idx;

        અક્ષર c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=buffer;
        prefixLength=0;
        જ્યારે((c=*s++)!=0) {
            *suffix++=c;
            ++prefixLength;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefixLength + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(કદ(buffer) - prefixLength)));

        /* call the enumerator function with this first character */
        જો(!fn(context, start, nameChoice, buffer, length)) {
            પાછા ખોટું;
        }

        /* enumerate the rest of the names */
        જ્યારે(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            માટે (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                જો(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    જ્યારે(*s++!=0) {
                    }
                    elements[i]=s;
                    તોડો;
                } નહીંતર {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefixLength;
            માટે(i=0; i<count; ++i) {
                s=elements[i];
                જ્યારે((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            જો(!fn(context, start, nameChoice, buffer, length)) {
                પાછા ખોટું;
            }
        }
        તોડો;
    }
    મૂળભૂત:
        /* undefined type */
        તોડો;
    }

    પાછા સાચું;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
સ્થિર UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    auto matcher = query.matcher();

    જો(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        પાછા 0xffff;
    }

    પસંદગી(range->type) {
    સ્થિતિ 0: {
        /* name = prefix hex-digits */
        અચલ અક્ષર* s = reinterpret_cast<અચલ અક્ષર*>(range + 1);

        /* compare prefix */
        જો (!matcher.consistentWith(s)) {
            પાછા 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        માટે(અક્ષર c : matcher.remainingSignificantCharacters()) {
            જો('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } નહીંતર જો('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } નહીંતર {
                પાછા 0xffff;
            }
        }

        /* does it fit into the range? */
        જો (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            પાછા code;
        }
        તોડો;
    }
    સ્થિતિ 1: {
        અક્ષર buffer[64];
        uint16_t indexes[8];
        અચલ અક્ષર *elementBases[8], *elements[8];
        અચલ uint16_t* factors = reinterpret_cast<અચલ uint16_t*>(range + 1);
        uint16_t count=range->variant;
        અચલ અક્ષર *s = reinterpret_cast<અચલ અક્ષર*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        જો (!matcher.consistentWith(prefix)) {
            પાછા 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, buffer, કદ(buffer));

        /* compare the first suffix */
        જો(querySuffix == buffer) {
            પાછા start;
        }

        /* enumerate and compare the rest of the suffixes */
        જ્યારે(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            માટે (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                જો(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    જ્યારે(*s++!=0) {}
                    elements[i]=s;
                    તોડો;
                } નહીંતર {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            auto it = querySuffix.begin();
            માટે(i=0; i<count; ++i) {
                s=elements[i];
                અક્ષર c;
                જ્યારે((c=*s++)!=0) {
                    જો(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            જો(i<99 && it == querySuffix.end()) {
                પાછા start;
            }
        }
        તોડો;
    }
    મૂળભૂત:
        /* undefined type */
        તોડો;
    }

    પાછા 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#define SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#define SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

સ્થિર int32_t
calcStringSetLength(uint32_t set[8], અચલ અક્ષર *s) {
    int32_t length=0;
    અક્ષર c;

    જ્યારે((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    પાછા length;
}

સ્થિર int32_t
calcAlgNameSetsLengths(int32_t maxNameLength) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_ગણતરી;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_ગણતરી=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    જ્યારે(range_ગણતરી>0) {
        પસંદગી(range->type) {
        સ્થિતિ 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calcStringSetLength(gNameSet, reinterpret_cast<અચલ અક્ષર*>(range + 1)) + range->variant;
            જો(length>maxNameLength) {
                maxNameLength=length;
            }
            તોડો;
        સ્થિતિ 1: {
            /* name = prefix factorized-elements */
            અચલ uint16_t* factors = reinterpret_cast<અચલ uint16_t*>(range + 1);
            અચલ અક્ષર *s;
            int32_t i, count=range->variant, factor, factorLength, maxFactorLength;

            /* prefix length */
            s = reinterpret_cast<અચલ અક્ષર*>(factors + count);
            length=calcStringSetLength(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            માટે(i=0; i<count; ++i) {
                maxFactorLength=0;
                માટે(factor=factors[i]; factor>0; --factor) {
                    factorLength=calcStringSetLength(gNameSet, s);
                    s+=factorLength+1;
                    જો(factorLength>maxFactorLength) {
                        maxFactorLength=factorLength;
                    }
                }
                length+=maxFactorLength;
            }

            જો(length>maxNameLength) {
                maxNameLength=length;
            }
            તોડો;
        }
        મૂળભૂત:
            /* unknown type */
            તોડો;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_ગણતરી;
    }
    પાછા maxNameLength;
}

સ્થિર int32_t
calcExtNameSetsLengths(int32_t maxNameLength) {
    int32_t i, length;

    માટે(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calcStringSetLength(gNameSet, charCatNames[i]);
        જો(length>maxNameLength) {
            maxNameLength=length;
        }
    }
    પાછા maxNameLength;
}

સ્થિર int32_t
calcNameSetLength(અચલ uint16_t *tokens, uint16_t token_ગણતરી, અચલ uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  અચલ uint8_t **p_લીટી, અચલ uint8_t *લીટી_Limit) {
    અચલ uint8_t *લીટી=*p_લીટી;
    int32_t length=0, tokenLength;
    uint16_t c, token;

    જ્યારે (લીટી != લીટી_Limit && (c = *લીટી++) != static_cast<uint8_t>(';')) {
        જો(c>=token_ગણતરી) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } નહીંતર {
            token=tokens[c];
            જો (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*લીટી++;
                token=tokens[c];
            }
            જો (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } નહીંતર {
                /* count token word */
                જો(tokenLengths!=nullptr) {
                    /* use cached token length */
                    tokenLength=tokenLengths[c];
                    જો(tokenLength==0) {
                        tokenLength = calcStringSetLength(set, reinterpret_cast<અચલ અક્ષર*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(tokenLength);
                    }
                } નહીંતર {
                    tokenLength = calcStringSetLength(set, reinterpret_cast<અચલ અક્ષર*>(tokenStrings) + token);
                }
                length+=tokenLength;
            }
        }
    }

    *p_લીટી=લીટી;
    પાછા length;
}

સ્થિર શૂન્ય
calcGroupNameSetsLengths(int32_t maxNameLength) {
    uint16_t offsets[લીટીઓ_PER_GROUP+2], lengths[લીટીઓ_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_ગણતરી=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    અચલ uint16_t *group;
    અચલ uint8_t *s, *લીટી, *લીટી_Limit;

    int32_t group_ગણતરી, લીટી_સંખ્યા, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_ગણતરી));
    જો(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_ગણતરી);
    }

    group=GET_GROUPS(uCharNames);
    group_ગણતરી=*group++;

    /* enumerate all groups */
    જ્યારે(group_ગણતરી>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        માટે(લીટી_સંખ્યા=0; લીટી_સંખ્યા<લીટીઓ_PER_GROUP; ++લીટી_સંખ્યા) {
            લીટી=s+offsets[લીટી_સંખ્યા];
            length=lengths[લીટી_સંખ્યા];
            જો(length==0) {
                ચાલુ;
            }

            લીટી_Limit=લીટી+length;

            /* read regular name */
            length=calcNameSetLength(tokens, token_ગણતરી, tokenStrings, tokenLengths, gNameSet, &લીટી, લીટી_Limit);
            જો(length>maxNameLength) {
                maxNameLength=length;
            }
            જો(લીટી==લીટી_Limit) {
                ચાલુ;
            }

            /* read Unicode 1.0 name */
            length=calcNameSetLength(tokens, token_ગણતરી, tokenStrings, tokenLengths, gNameSet, &લીટી, લીટી_Limit);
            જો(length>maxNameLength) {
                maxNameLength=length;
            }
            જો(લીટી==લીટી_Limit) {
                ચાલુ;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=NEXT_GROUP(group);
        --group_ગણતરી;
    }

    જો(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    gMaxNameLength=maxNameLength;
}

સ્થિર UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    સ્થિર અચલ અક્ષર extChars[]="0123456789ABCDEF<>-";
    int32_t i, maxNameLength;

    જો(gMaxNameLength!=0) {
        પાછા સાચું;
    }

    જો(!isDataLoaded(pErrorCode)) {
        પાછા ખોટું;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    માટે (i = 0; i < static_cast<int32_t>(કદ(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    maxNameLength=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    maxNameLength=calcExtNameSetsLengths(maxNameLength);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(maxNameLength);

    પાછા સાચું;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           અક્ષર *buffer, int32_t bufferLength,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    } નહીંતર જો(nameChoice>=U_CHAR_NAME_CHOICE_ગણતરી ||
              bufferLength<0 || (bufferLength>0 && buffer==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો((uint32_t)code>UCHAR_MAX_કિંમત || !isDataLoaded(pErrorCode)) {
        પાછા u_terminateChars(buffer, bufferLength, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    જ્યારે(i>0) {
        જો(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
            તોડો;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    જો(i==0) {
        જો (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, buffer, (uint16_t) bufferLength);
            જો (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, buffer, (uint16_t) bufferLength);
            }
        } નહીંતર {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, buffer, (uint16_t)bufferLength);
        }
    }

    પાછા u_terminateChars(buffer, bufferLength, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                અક્ષર *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    } નહીંતર જો(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    પાછા u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               અચલ અક્ષર *અચલ name,
               UErrorCode *pErrorCode) {
    અક્ષર lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    અક્ષર c0;
    સ્થિર constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા error;
    }

    જો(nameChoice>=U_CHAR_NAME_CHOICE_ગણતરી || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા error;
    }

    જો(!isDataLoaded(pErrorCode)) {
        પાછા error;
    }

    /* construct the lowercase of the name first */
    અચલ અક્ષર* nameChar = name;
    માટે(i=0; i<કદ(lower); ++i) {
        જો((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } નહીંતર {
            તોડો;
        }
    }
    જો(i==કદ(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        પાછા error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    જો (lower[0] == '<') {
        જો (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            જ્યારે (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hexLength = limit - (i + 1);
            જો (i >= 2 && lower[i] == '-' && 1 <= hexLength && hexLength <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                માટે (++i; i < limit; ++i) {
                    જો (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } નહીંતર જો (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } નહીંતર {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        પાછા error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    જો (cp > UCHAR_MAX_કિંમત) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        પાછા error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                માટે (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    જો (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        જો (cat == cIdx) {
                            પાછા cp;
                        }
                        તોડો;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        પાછા error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    જ્યારે(i>0) {
        જો((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            પાછા cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_કિંમત + 1, DO_FIND_NAME, &findName, nameChoice);
    જો (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    પાછા findName.code;
}

U_CAPI શૂન્ય U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                શૂન્ય *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા;
    }

    જો(nameChoice>=U_CHAR_NAME_CHOICE_ગણતરી || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
    }

    જો((uint32_t) limit > UCHAR_MAX_કિંમત + 1) {
        limit = UCHAR_MAX_કિંમત + 1;
    }
    જો((uint32_t)start>=(uint32_t)limit) {
        પાછા;
    }

    જો(!isDataLoaded(pErrorCode)) {
        પાછા;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    જ્યારે(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        જો((uint32_t)start<algRange->start) {
            જો((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                પાછા;
            }
            જો(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                પાછા;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        જો((uint32_t)start<=algRange->end) {
            જો((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                પાછા;
            }
            જો(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                પાછા;
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
    જો(calcNameSetsLengths(&errorCode)) {
        પાછા gMaxNameLength;
    } નહીંતર {
        પાછા 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
સ્થિર શૂન્ય
charSetToUSet(uint32_t cset[8], અચલ USetAdder *sa) {
    char16_t us[256];
    અક્ષર cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    જો(!calcNameSetsLengths(&errorCode)) {
        પાછા;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    માટે(i=0; i<256; ++i) {
        જો(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<અક્ષર>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    માટે(i=0; i<length; ++i) {
        જો(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI શૂન્ય U_EXPORT2
uprv_getCharNameCharacters(અચલ USetAdder *sa) {
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
સ્થિર શૂન્ય
makeTokenMap(અચલ UDataSwapper *ds,
             int16_t tokens[], uint16_t token_ગણતરી,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    જો(U_FAILURE(*pErrorCode)) {
        પાછા;
    }

    જો(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        માટે(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } નહીંતર {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        જો(token_ગણતરી>256) {
            token_ગણતરી=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        માટે(i=1; i<token_ગણતરી; ++i) {
            જો(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                જો(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    પાછા;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=સાચું;
            }
        }

        /* set the mappings for the rest of the permutation */
        માટે(i=j=1; i<token_ગણતરી; ++i) {
            /* set mappings that were not set for direct bytes */
            જો(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                જ્યારે(usedOutChar[j]) {
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
uchar_swapNames(અચલ UDataSwapper *ds,
                અચલ શૂન્ય *inData, int32_t length, શૂન્ય *outData,
                UErrorCode *pErrorCode) {
    અચલ UDataInfo *pInfo;
    int32_t headerSize;

    અચલ uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_ગણતરી;

    અચલ AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    headerSize=udata_swapDataHeader(ds, inData, length, outData, pErrorCode);
    જો(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }

    /* check data format and format version */
    pInfo=(અચલ UDataInfo *)((અચલ અક્ષર *)inData+4);
    જો(!(
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
        પાછા 0;
    }

    inBytes=(અચલ uint8_t *)inData+headerSize;
    outBytes=(outData == nullptr) ? nullptr : (uint8_t *)outData+headerSize;
    જો(length<0) {
        algNamesOffset=ds->readUInt32(((અચલ uint32_t *)inBytes)[3]);
    } નહીંતર {
        length-=headerSize;
        જો( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((અચલ uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
            પાછા 0;
        }
    }

    જો(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((અચલ uint32_t *)(inBytes+offset)));
        offset+=4;

        માટે(i=0; i<count; ++i) {
            inRange=(અચલ AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } નહીંતર {
        /* swap data */
        અચલ uint16_t *p;
        uint16_t *q, *temp;

        int16_t tokens[512];
        uint16_t token_ગણતરી;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        જો(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((અચલ uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((અચલ uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((અચલ uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(અચલ uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_ગણતરી=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        જો(token_ગણતરી<=512) {
            count=token_ગણતરી;
        } નહીંતર {
            count=512;
        }
        માટે(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        માટે(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_ગણતરી, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_ગણતરી>256 ? token_ગણતરી-256 : 0), trailMap, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            પાછા 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        temp=(uint16_t *)uprv_malloc(token_ગણતરી*2);
        જો(temp==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_ગણતરી);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            પાછા 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        માટે(i=0; i<token_ગણતરી && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        માટે(; i<token_ગણતરી; ++i) {
            ds->swapArray16(ds, p+i, 2, temp+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, temp, token_ગણતરી*2);
        uprv_free(temp);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            પાછા 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((અચલ uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        જો(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[લીટીઓ_PER_GROUP+1], lengths[લીટીઓ_PER_GROUP+1];

            અચલ uint8_t *inStrings, *nextInStrings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_ગણતરી=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            જ્યારે(strings_ગણતરી>32) {
                nextInStrings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_ગણતરી-=(uint32_t)(nextInStrings-inStrings);
                outStrings+=nextInStrings-inStrings;
                inStrings=nextInStrings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_ગણતરી-=count;

                /* swap the string bytes using map[] and trailMap[] */
                જ્યારે(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    જો(tokens[c]!=-2) {
                        --count;
                    } નહીંતર {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((અચલ uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        માટે(i=0; i<count; ++i) {
            જો(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_INDEX_OUTOFBOUNDS_ERROR;
                પાછા 0;
            }

            inRange=(અચલ AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            પસંદગી(inRange->type) {
            સ્થિતિ 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((અચલ અક્ષર *)(inRange+1)),
                                    outRange+1, pErrorCode);
                જો(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    પાછા 0;
                }
                તોડો;
            સ્થિતિ 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_ગણતરી;

                    factors_ગણતરી=inRange->variant;
                    p=(અચલ uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_ગણતરી*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_ગણતરી;
                    q+=factors_ગણતરી;
                    strings_ગણતરી=(uint32_t)((inBytes+offset)-(અચલ uint8_t *)p);
                    જ્યારે(strings_ગણતરી>0 && ((અચલ uint8_t *)p)[strings_ગણતરી-1]!=0) {
                        --strings_ગણતરી;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_ગણતરી, q, pErrorCode);
                }
                તોડો;
            મૂળભૂત:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                પાછા 0;
            }
        }
    }

    પાછા headerSize+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
