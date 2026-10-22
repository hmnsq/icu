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

#समावेश "unicode/utypes.h"
#समावेश "unicode/putil.h"
#समावेश "unicode/uchar.h"
#समावेश "unicode/udata.h"
#समावेश "unicode/utf.h"
#समावेश "unicode/utf16.h"
#समावेश "uassert.h"
#समावेश "ustr_imp.h"
#समावेश "umutex.h"
#समावेश "cmemory.h"
#समावेश "cstring.h"
#समावेश "ucln_cmn.h"
#समावेश "udataswp.h"
#समावेश "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

अटल स्थिर अक्षर तथ्य_NAME[] = "unames";
अटल स्थिर अक्षर तथ्य_TYPE[] = "icu";

#घोषणा GROUP_SHIFT 5
#घोषणा पंक्तियाँ_PER_GROUP (1L<<GROUP_SHIFT)
#घोषणा GROUP_MASK (पंक्तियाँ_PER_GROUP-1)

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
क्रमागत {
    GROUP_MSB,
    GROUP_OFFSET_ऊपरी,
    GROUP_OFFSET_निचला,
    GROUP_लंबाई
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#घोषणा GET_GROUP_OFFSET(group) ((int32_t)(group)[GROUP_OFFSET_ऊपरी]<<16|(group)[GROUP_OFFSET_निचला])

#घोषणा अग्र_GROUP(group) ((group)+GROUP_लंबाई)
#घोषणा पूर्व_GROUP(group) ((group)-GROUP_लंबाई)

प्रकार काष्ठा {
    uint32_t start, end;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

प्रकार काष्ठा {
    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset;
} UCharNames;

namespace {
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
श्रेणी CharacterNameQuery {
  खुला:
    श्रेणी Matcher {
      खुला:
        // `query` must outlive the constructed object.
        Matcher(स्थिर CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        bool consistentWith(अक्षर c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            अगर (skeletonIterator == query.skeleton.end()) {
                वापस false;
            }
            अगर (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                अगर (lastChar == '-') {
                    अगर (*skeletonIterator++ != lastChar) {
                        वापस false;
                    }
                }
                lastChar = c;
                वापस true;
            } अन्यथा अगर (c == '-') {
                अगर (lastChar == ' ' ||
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
                    अगर (*skeletonIterator++ != c) {
                        वापस false;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                lastChar = c;
                वापस true;
            } अन्यथा {
                अगर (*skeletonIterator++ != c) {
                    वापस false;
                }
                lastChar = c;
                वापस true;
            }
        }

        bool consistentWith(स्थिर std::string_view substring) {
            क्रम (स्थिर अक्षर c : substring) {
                अगर (!consistentWith(c)) {
                    वापस false;
                }
            }
            वापस true;
        }

        bool matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(lastChar != '-');
            वापस skeletonIterator == query.skeleton.end();
        }

        std::string_view remainingSignificantCharacters() स्थिर {
            वापस query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      निजी:
        स्थिर CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        अक्षर lastChar = ' ';
    };

    explicit CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        अक्षर *skeletonLimit = skeleton_तथ्य;
        क्रम (std::माप_प्रकार i = 0; i < query.length(); ++i) {
            U_ASSERT(skeletonLimit < skeleton_तथ्य + माप(skeleton_तथ्य));
            अगर (skeletonLimit >= skeleton_तथ्य + माप(skeleton_तथ्य)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeletonLimit = skeleton_तथ्य;
                अवरोध;
            }
            अगर (query[i] == ' ' || query[i] == '_') {
                जारी;
            }
            अगर (query[i] == '-') {
                bool isMedial;
                bool is1180MedialHyphen = false;
                अगर (i == 0 || i == query.length() - 1) {
                    isMedial = false;
                } अन्यथा {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                अगर (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_तथ्य, skeletonLimit - skeleton_तथ्य) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = true;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    क्रम (std::माप_प्रकार j = i + 2; j < query.length(); ++j) {
                        अगर (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = false;
                        }
                    }
                }
                अगर (!isMedial || is1180MedialHyphen) {
                    *skeletonLimit++ = query[i];
                }
            } अन्यथा {
                *skeletonLimit++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_तथ्य, skeletonLimit - skeleton_तथ्य);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() स्थिर {
        वापस Matcher(*यह);
    }

निजी:
    अटल bool isASCIILetterOrDigit(स्थिर अक्षर c) {
        वापस uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    अक्षर skeleton_तथ्य[120];
    std::string_view skeleton;
    bool is1180;
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
#घोषणा GET_GROUPS(names) (स्थिर uint16_t *)((स्थिर अक्षर *)names+names->groupsOffset)

प्रकार काष्ठा {
    स्थिर CharacterNameQuery *otherName;
    UChar32 code;
} FindName;

#घोषणा DO_FIND_NAME nullptr

अटल UDataMemory *u_Char_Names_तथ्य=nullptr;
अटल UCharNames *uCharNames=nullptr;
अटल icu::UInitOnce gCharNamesInitOnce {};

/*
 * Maximum length of character names (regular & 1.0).
 */
अटल int32_t g_Max_Name_लंबाई=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
अटल uint32_t gNameSet[8]={ 0 };

#घोषणा U_NONCHARACTER_CODE_POINT U_CHAR_CATEGORY_गिनती
#घोषणा U_LEAD_SURROGATE U_CHAR_CATEGORY_गिनती + 1
#घोषणा U_TRAIL_SURROGATE U_CHAR_CATEGORY_गिनती + 2

#घोषणा U_CHAR_EXTENDED_CATEGORY_गिनती (U_CHAR_CATEGORY_गिनती + 3)

अटल स्थिर अक्षर * स्थिर charCatNames[U_CHAR_EXTENDED_CATEGORY_गिनती] = {
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

अटल UBool U_CALLCONV unames_cleanup()
{
    अगर(u_Char_Names_तथ्य) {
        udata_close(u_Char_Names_तथ्य);
        u_Char_Names_तथ्य = nullptr;
    }
    अगर(uCharNames) {
        uCharNames = nullptr;
    }
    gCharNamesInitOnce.reset();
    g_Max_Name_लंबाई=0;
    वापस true;
}

अटल UBool U_CALLCONV
isAcceptable(व्योम * /*context*/,
             स्थिर अक्षर * /*type*/, स्थिर अक्षर * /*name*/,
             स्थिर UDataInfo *pInfo) {
    वापस
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->तथ्य_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->तथ्य_Format[1]==0x6e &&
        pInfo->तथ्य_Format[2]==0x61 &&
        pInfo->तथ्य_Format[3]==0x6d &&
        pInfo->formatVersion[0]==1;
}

अटल व्योम U_CALLCONV
loadCharNames(UErrorCode &status) {
    U_ASSERT(u_Char_Names_तथ्य == nullptr);
    U_ASSERT(uCharNames == nullptr);

    u_Char_Names_तथ्य = udata_openChoice(nullptr, तथ्य_TYPE, तथ्य_NAME, isAcceptable, nullptr, &status);
    अगर(U_FAILURE(status)) {
        u_Char_Names_तथ्य = nullptr;
    } अन्यथा {
        uCharNames = (UCharNames *)udata_getMemory(u_Char_Names_तथ्य);
    }
    ucln_common_registerCleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


अटल UBool
is_तथ्य_Loaded(UErrorCode *pErrorCode) {
    umtx_initOnce(gCharNamesInitOnce, &loadCharNames, *pErrorCode);
    वापस U_SUCCESS(*pErrorCode);
}

#घोषणा WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c) UPRV_BLOCK_MACRO_BEGIN { \
    अगर((बफर_लंबाई)>0) { \
        *(बफर)++=c; \
        --(बफर_लंबाई); \
    } \
    ++(बफर_Pos); \
} UPRV_BLOCK_MACRO_END

#घोषणा U_ISO_COMMENT U_CHAR_NAME_CHOICE_गिनती

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
अटल uint16_t
expandName(UCharNames *names,
           स्थिर uint8_t *name, uint16_t name_लंबाई, UCharNameChoice nameChoice,
           अक्षर *बफर, uint16_t बफर_लंबाई) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_गिनती=*tokens++, बफर_Pos=0;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;

    अगर(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        अगर (static_cast<uint8_t>(';') >= token_गिनती || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णांक field_सूचकांक= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            करो {
                जबतक(name_लंबाई>0) {
                    --name_लंबाई;
                    अगर(*name++==';') {
                        अवरोध;
                    }
                }
            } जबतक(--field_सूचकांक>0);
        } अन्यथा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_लंबाई=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    जबतक(name_लंबाई>0) {
        --name_लंबाई;
        c=*name++;

        अगर(c>=token_गिनती) {
            अगर(c!=';') {
                /* implicit letter */
                WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
            } अन्यथा {
                /* finished */
                अवरोध;
            }
        } अन्यथा {
            token=tokens[c];
            अगर (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --name_लंबाई;
            }
            अगर (token == static_cast<uint16_t>(-1)) {
                अगर(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
                } अन्यथा {
                    /* finished */
                    अवरोध;
                }
            } अन्यथा {
                /* write token word */
                uint8_t *tokenString=tokenStrings+token;
                जबतक((c=*tokenString++)!=0) {
                    WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    अगर(बफर_लंबाई>0) {
        *बफर=0;
    }

    वापस बफर_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
अटल UBool
compareName(UCharNames *names,
            स्थिर uint8_t *name, uint16_t name_लंबाई, UCharNameChoice nameChoice,
            स्थिर CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(names) + 8;
    uint16_t token, token_गिनती=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(names) + names->tokenStringOffset;
    uint8_t c;
    स्वतः matcher = otherName.matcher();

    अगर(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        अगर (static_cast<uint8_t>(';') >= token_गिनती || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णांक field_सूचकांक= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            करो {
                जबतक(name_लंबाई>0) {
                    --name_लंबाई;
                    अगर(*name++==';') {
                        अवरोध;
                    }
                }
            } जबतक(--field_सूचकांक>0);
        } अन्यथा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_लंबाई=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    जबतक(name_लंबाई>0) {
        --name_लंबाई;
        c=*name++;

        अगर(c>=token_गिनती) {
            अगर(c!=';') {
                /* implicit letter */
                अगर (!matcher.consistentWith(c)) {
                    वापस false;
                }
            } अन्यथा {
                /* finished */
                अवरोध;
            }
        } अन्यथा {
            token=tokens[c];
            अगर (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --name_लंबाई;
            }
            अगर (token == static_cast<uint16_t>(-1)) {
                अगर(c!=';') {
                    /* explicit letter */
                    अगर (!matcher.consistentWith(c)) {
                        वापस false;
                    }
                } अन्यथा {
                    /* finished */
                    अवरोध;
                }
            } अन्यथा {
                /* write token word */
                अगर (!matcher.consistentWith(reinterpret_cast<स्थिर अक्षर *>(tokenStrings + token))) {
                    वापस false;
                }
            }
        }
    }

    /* complete match? */
    वापस matcher.matches();
}

अटल uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    अगर (U_IS_UNICODE_NONCHAR(cp)) {
        वापस U_NONCHARACTER_CODE_POINT;
    }

    अगर ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    वापस cat;
}

अटल स्थिर अक्षर *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    अगर (cat >= UPRV_LENGTHOF(charCatNames)) {
        वापस "unknown";
    } अन्यथा {
        वापस charCatNames[cat];
    }
}

अटल uint16_t getExtName(uint32_t code, अक्षर *बफर, uint16_t बफर_लंबाई) {
    स्थिर अक्षर *catname = getCharCatName(code);
    uint16_t length = 0;

    UChar32 cp;
    पूर्णांक ndigits, i;
    
    WRITE_CHAR(बफर, बफर_लंबाई, length, '<');
    जबतक (catname[length - 1]) {
        WRITE_CHAR(बफर, बफर_लंबाई, length, catname[length - 1]);
    }
    WRITE_CHAR(बफर, बफर_लंबाई, length, '-');
    क्रम (cp = code, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    अगर (ndigits < 4)
        ndigits = 4;
    क्रम (cp = code, i = ndigits; (cp || i > 0) && बफर_लंबाई; cp >>= 4, बफर_लंबाई--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        बफर[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    बफर += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(बफर, बफर_लंबाई, length, '>');

    वापस length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
अटल स्थिर uint16_t *
getGroup(UCharNames *names, uint32_t code) {
    स्थिर uint16_t *groups=GET_GROUPS(names);
    uint16_t groupMSB = static_cast<uint16_t>(code >> GROUP_SHIFT),
             start=0,
             limit=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    जबतक(start<limit-1) {
        number = static_cast<uint16_t>((start + limit) / 2);
        अगर(groupMSB<groups[number*GROUP_लंबाई+GROUP_MSB]) {
            limit=number;
        } अन्यथा {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    वापस groups+start*GROUP_लंबाई;
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
अटल स्थिर uint8_t *
expandGroupLengths(स्थिर uint8_t *s,
                   uint16_t offsets[पंक्तियाँ_PER_GROUP+1], uint16_t lengths[पंक्तियाँ_PER_GROUP+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, offset=0, length=0;
    uint8_t लंबाई_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    जबतक(i<पंक्तियाँ_PER_GROUP) {
        लंबाई_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        अगर(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | लंबाई_Byte >> 4) + 12);
            लंबाई_Byte&=0xf;
        } अन्यथा अगर((लंबाई_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((लंबाई_Byte & 0x3f) + 12);
        } अन्यथा {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(लंबाई_Byte >> 4);
            लंबाई_Byte&=0xf;
        }

        *offsets++=offset;
        *lengths++=length;

        offset+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        अगर((लंबाई_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=लंबाई_Byte;
            अगर(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=offset;
                *lengths++=length;

                offset+=length;
                ++i;
            }
        } अन्यथा {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    वापस s;
}

अटल uint16_t
expandGroupName(UCharNames *names, स्थिर uint16_t *group,
                uint16_t पंक्ति_संख्या, UCharNameChoice nameChoice,
                अक्षर *बफर, uint16_t बफर_लंबाई) {
    uint16_t offsets[पंक्तियाँ_PER_GROUP+2], lengths[पंक्तियाँ_PER_GROUP+2];
    स्थिर uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);
    s=expandGroupLengths(s, offsets, lengths);
    वापस expandName(names, s+offsets[पंक्ति_संख्या], lengths[पंक्ति_संख्या], nameChoice,
                      बफर, बफर_लंबाई);
}

अटल uint16_t
getName(UCharNames *names, uint32_t code, UCharNameChoice nameChoice,
        अक्षर *बफर, uint16_t बफर_लंबाई) {
    स्थिर uint16_t *group=getGroup(names, code);
    अगर (static_cast<uint16_t>(code >> GROUP_SHIFT) == group[GROUP_MSB]) {
        वापस expandGroupName(names, group, static_cast<uint16_t>(code & GROUP_MASK), nameChoice,
                               बफर, बफर_लंबाई);
    } अन्यथा {
        /* group not found */
        /* zero-terminate */
        अगर(बफर_लंबाई>0) {
            *बफर=0;
        }
        वापस 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
अटल UBool
enumGroupNames(UCharNames *names, स्थिर uint16_t *group,
               UChar32 start, UChar32 end,
               UEnumCharNamesFn *fn, व्योम *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[पंक्तियाँ_PER_GROUP+2], lengths[पंक्तियाँ_PER_GROUP+2];
    स्थिर uint8_t* s = reinterpret_cast<uint8_t*>(names) + names->groupStringOffset + GET_GROUP_OFFSET(group);

    s=expandGroupLengths(s, offsets, lengths);
    अगर(fn!=DO_FIND_NAME) {
        अक्षर बफर[200];
        uint16_t length;

        जबतक(start<=end) {
            length=expandName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, बफर, माप(बफर));
            अगर (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                बफर[length = getExtName(start, बफर, माप(बफर))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            अगर(length>0) {
                अगर(!fn(context, start, nameChoice, बफर, length)) {
                    वापस false;
                }
            }
            ++start;
        }
    } अन्यथा {
        स्थिर CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        जबतक(start<=end) {
            अगर(compareName(names, s+offsets[start&GROUP_MASK], lengths[start&GROUP_MASK], nameChoice, otherName)) {
                static_cast<FindName*>(context)->code = start;
                वापस false;
            }
            ++start;
        }
    }
    वापस true;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
अटल UBool
enumExtNames(UChar32 start, UChar32 end,
             UEnumCharNamesFn *fn, व्योम *context)
{
    अगर(fn!=DO_FIND_NAME) {
        अक्षर बफर[200];
        uint16_t length;
        
        जबतक(start<=end) {
            बफर[length = getExtName(start, बफर, माप(बफर))] = 0;
            /* here, we assume that the buffer is large enough */
            अगर(length>0) {
                अगर(!fn(context, start, U_EXTENDED_CHAR_NAME, बफर, length)) {
                    वापस false;
                }
            }
            ++start;
        }
    }

    वापस true;
}

अटल UBool
enumNames(UCharNames *names,
          UChar32 start, UChar32 limit,
          UEnumCharNamesFn *fn, व्योम *context,
          UCharNameChoice nameChoice) {
    uint16_t startGroupMSB, endGroupMSB, group_गिनती;
    स्थिर uint16_t *group, *groupLimit;

    startGroupMSB = static_cast<uint16_t>(start >> GROUP_SHIFT);
    endGroupMSB = static_cast<uint16_t>((limit - 1) >> GROUP_SHIFT);

    /* find the group that contains start, or the highest before it */
    group=getGroup(names, start);

    अगर(startGroupMSB<group[GROUP_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 extLimit = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
        अगर(extLimit>limit) {
            extLimit=limit;
        }
        अगर(!enumExtNames(start, extLimit-1, fn, context)) {
            वापस false;
        }
        start=extLimit;
    }

    अगर(startGroupMSB==endGroupMSB) {
        अगर(startGroupMSB==group[GROUP_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            वापस enumGroupNames(names, group, start, limit-1, fn, context, nameChoice);
        }
    } अन्यथा {
        स्थिर uint16_t *groups=GET_GROUPS(names);
        group_गिनती=*groups++;
        groupLimit=groups+group_गिनती*GROUP_लंबाई;

        अगर(startGroupMSB==group[GROUP_MSB]) {
            /* enumerate characters in the partial start group */
            अगर((start&GROUP_MASK)!=0) {
                अगर(!enumGroupNames(names, group,
                                   start, (static_cast<UChar32>(startGroupMSB) << GROUP_SHIFT) + पंक्तियाँ_PER_GROUP - 1,
                                   fn, context, nameChoice)) {
                    वापस false;
                }
                group=अग्र_GROUP(group); /* continue with the next group */
            }
        } अन्यथा अगर(startGroupMSB>group[GROUP_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            स्थिर uint16_t *अग्र_Group=अग्र_GROUP(group);
            अगर (अग्र_Group < groupLimit && अग्र_Group[GROUP_MSB] > startGroupMSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = अग्र_Group[GROUP_MSB] << GROUP_SHIFT;
                अगर (end > limit) {
                    end = limit;
                }
                अगर (!enumExtNames(start, end - 1, fn, context)) {
                    वापस false;
                }
            }
            group=अग्र_Group;
        }

        /* enumerate entire groups between the start- and end-groups */
        जबतक(group<groupLimit && group[GROUP_MSB]<endGroupMSB) {
            स्थिर uint16_t *अग्र_Group;
            start = static_cast<UChar32>(group[GROUP_MSB]) << GROUP_SHIFT;
            अगर(!enumGroupNames(names, group, start, start+पंक्तियाँ_PER_GROUP-1, fn, context, nameChoice)) {
                वापस false;
            }
            अग्र_Group=अग्र_GROUP(group);
            अगर (अग्र_Group < groupLimit && अग्र_Group[GROUP_MSB] > group[GROUP_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 end = अग्र_Group[GROUP_MSB] << GROUP_SHIFT;
                अगर (end > limit) {
                    end = limit;
                }
                अगर (!enumExtNames((group[GROUP_MSB] + 1) << GROUP_SHIFT, end - 1, fn, context)) {
                    वापस false;
                }
            }
            group=अग्र_Group;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        अगर(group<groupLimit && group[GROUP_MSB]==endGroupMSB) {
            वापस enumGroupNames(names, group, (limit-1)&~GROUP_MASK, limit-1, fn, context, nameChoice);
        } अन्यथा अगर (nameChoice == U_EXTENDED_CHAR_NAME && group == groupLimit) {
            UChar32 next = (पूर्व_GROUP(group)[GROUP_MSB] + 1) << GROUP_SHIFT;
            अगर (next > start) {
                start = next;
            }
        } अन्यथा {
            वापस true;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    अगर (nameChoice == U_EXTENDED_CHAR_NAME) {
        अगर (limit > UCHAR_MAX_मूल्य + 1) {
            limit = UCHAR_MAX_मूल्य + 1;
        }
        वापस enumExtNames(start, limit - 1, fn, context);
    }
    
    वापस true;
}

अटल uint16_t
writeFactorSuffix(स्थिर uint16_t *factors, uint16_t count,
                  स्थिर अक्षर *s, /* suffix elements */
                  uint32_t code,
                  uint16_t indexes[8], /* output fields from here */
                  स्थिर अक्षर *elementBases[8], स्थिर अक्षर *elements[8],
                  अक्षर *बफर, uint16_t बफर_लंबाई) {
    uint16_t i, factor, बफर_Pos=0;
    अक्षर c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    क्रम(i=count; i>0; --i) {
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
    क्रम(;;) {
        अगर(elementBases!=nullptr) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        factor=indexes[i];
        जबतक(factor>0) {
            जबतक(*s++!=0) {}
            --factor;
        }
        अगर(elements!=nullptr) {
            *elements++=s;
        }

        /* write element */
        जबतक((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        अगर(i>=count) {
            अवरोध;
        }

        /* skip the rest of the strings for this factors[i] */
        factor = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        जबतक(factor>0) {
            जबतक(*s++!=0) {}
            --factor;
        }

        ++i;
    }

    /* zero-terminate */
    अगर(बफर_लंबाई>0) {
        *बफर=0;
    }

    वापस बफर_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
अटल uint16_t
getAlgName(AlgorithmicRange *range, uint32_t code, UCharNameChoice nameChoice,
        अक्षर *बफर, uint16_t बफर_लंबाई) {
    uint16_t बफर_Pos=0;

    /* Only the normative character name can be algorithmic. */
    अगर(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        अगर(बफर_लंबाई>0) {
            *बफर=0;
        }
        वापस 0;
    }

    चयन(range->type) {
    हाल 0: {
        /* name = prefix hex-digits */
        स्थिर अक्षर* s = reinterpret_cast<स्थिर अक्षर*>(range + 1);
        अक्षर c;

        uint16_t i, count;

        /* copy prefix */
        जबतक((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        अगर(count<बफर_लंबाई) {
            बफर[count]=0;
        }

        क्रम(i=count; i>0;) {
            अगर(--i<बफर_लंबाई) {
                c = static_cast<अक्षर>(code & 0xf);
                अगर(c<10) {
                    c+='0';
                } अन्यथा {
                    c+='A'-10;
                }
                बफर[i]=c;
            }
            code>>=4;
        }

        बफर_Pos+=count;
        अवरोध;
    }
    हाल 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        स्थिर uint16_t* factors = reinterpret_cast<स्थिर uint16_t*>(range + 1);
        uint16_t count=range->variant;
        स्थिर अक्षर* s = reinterpret_cast<स्थिर अक्षर*>(factors + count);
        अक्षर c;

        /* copy prefix */
        जबतक((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_लंबाई, बफर_Pos, c);
        }

        बफर_Pos+=writeFactorSuffix(factors, count,
                                     s, code-range->start, indexes, nullptr, nullptr, बफर, बफर_लंबाई);
        अवरोध;
    }
    शेष:
        /* undefined type */
        /* zero-terminate */
        अगर(बफर_लंबाई>0) {
            *बफर=0;
        }
        अवरोध;
    }

    वापस बफर_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
अटल UBool
enumAlgNames(AlgorithmicRange *range,
             UChar32 start, UChar32 limit,
             UEnumCharNamesFn *fn, व्योम *context,
             UCharNameChoice nameChoice) {
    अक्षर बफर[200];
    uint16_t length;

    अगर(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        वापस true;
    }

    चयन(range->type) {
    हाल 0: {
        अक्षर *s, *end;
        अक्षर c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, बफर, माप(बफर));
        अगर(length<=0) {
            वापस true;
        }

        /* call the enumerator function with this first character */
        अगर(!fn(context, start, nameChoice, बफर, length)) {
            वापस false;
        }

        /* go to the end of the name; all these names have the same length */
        end=बफर;
        जबतक(*end!=0) {
            ++end;
        }

        /* enumerate the rest of the names */
        जबतक(++start<limit) {
            /* increment the hexadecimal number on a character-basis */
            s=end;
            क्रम (;;) {
                c=*--s;
                अगर(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<अक्षर>(c + 1);
                    अवरोध;
                } अन्यथा अगर(c=='9') {
                    *s='A';
                    अवरोध;
                } अन्यथा अगर(c=='F') {
                    *s='0';
                }
            }

            अगर(!fn(context, start, nameChoice, बफर, length)) {
                वापस false;
            }
        }
        अवरोध;
    }
    हाल 1: {
        uint16_t indexes[8];
        स्थिर अक्षर *elementBases[8], *elements[8];
        स्थिर uint16_t* factors = reinterpret_cast<स्थिर uint16_t*>(range + 1);
        uint16_t count=range->variant;
        स्थिर अक्षर* s = reinterpret_cast<स्थिर अक्षर*>(factors + count);
        अक्षर *suffix, *t;
        uint16_t prefix_लंबाई, i, idx;

        अक्षर c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=बफर;
        prefix_लंबाई=0;
        जबतक((c=*s++)!=0) {
            *suffix++=c;
            ++prefix_लंबाई;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(prefix_लंबाई + writeFactorSuffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(माप(बफर) - prefix_लंबाई)));

        /* call the enumerator function with this first character */
        अगर(!fn(context, start, nameChoice, बफर, length)) {
            वापस false;
        }

        /* enumerate the rest of the names */
        जबतक(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            क्रम (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                अगर(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    जबतक(*s++!=0) {
                    }
                    elements[i]=s;
                    अवरोध;
                } अन्यथा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=prefix_लंबाई;
            क्रम(i=0; i<count; ++i) {
                s=elements[i];
                जबतक((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            अगर(!fn(context, start, nameChoice, बफर, length)) {
                वापस false;
            }
        }
        अवरोध;
    }
    शेष:
        /* undefined type */
        अवरोध;
    }

    वापस true;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
अटल UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 code;
    स्वतः matcher = query.matcher();

    अगर(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        वापस 0xffff;
    }

    चयन(range->type) {
    हाल 0: {
        /* name = prefix hex-digits */
        स्थिर अक्षर* s = reinterpret_cast<स्थिर अक्षर*>(range + 1);

        /* compare prefix */
        अगर (!matcher.consistentWith(s)) {
            वापस 0xffff;
        }

        /* read hexadecimal code point value */
        code=0;
        क्रम(अक्षर c : matcher.remainingSignificantCharacters()) {
            अगर('0'<=c && c<='9') {
                code=(code<<4)|(c-'0');
            } अन्यथा अगर('A'<=c && c<='F') {
                code=(code<<4)|(c-'A'+10);
            } अन्यथा {
                वापस 0xffff;
            }
        }

        /* does it fit into the range? */
        अगर (range->start <= static_cast<uint32_t>(code) && static_cast<uint32_t>(code) <= range->end) {
            वापस code;
        }
        अवरोध;
    }
    हाल 1: {
        अक्षर बफर[64];
        uint16_t indexes[8];
        स्थिर अक्षर *elementBases[8], *elements[8];
        स्थिर uint16_t* factors = reinterpret_cast<स्थिर uint16_t*>(range + 1);
        uint16_t count=range->variant;
        स्थिर अक्षर *s = reinterpret_cast<स्थिर अक्षर*>(factors + count);
        UChar32 start, limit;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view prefix = s;
        अगर (!matcher.consistentWith(prefix)) {
            वापस 0xffff;
        }

        s += prefix.length() + 1;

        start = static_cast<UChar32>(range->start);
        limit = static_cast<UChar32>(range->end + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        writeFactorSuffix(factors, count, s, 0,
                          indexes, elementBases, elements, बफर, माप(बफर));

        /* compare the first suffix */
        अगर(querySuffix == बफर) {
            वापस start;
        }

        /* enumerate and compare the rest of the suffixes */
        जबतक(++start<limit) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            क्रम (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                अगर(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    जबतक(*s++!=0) {}
                    elements[i]=s;
                    अवरोध;
                } अन्यथा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            स्वतः it = querySuffix.begin();
            क्रम(i=0; i<count; ++i) {
                s=elements[i];
                अक्षर c;
                जबतक((c=*s++)!=0) {
                    अगर(it == querySuffix.end() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            अगर(i<99 && it == querySuffix.end()) {
                वापस start;
            }
        }
        अवरोध;
    }
    शेष:
        /* undefined type */
        अवरोध;
    }

    वापस 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#घोषणा SET_ADD(set, c) ((set)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#घोषणा SET_CONTAINS(set, c) (((set)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

अटल int32_t
calc_String_Set_लंबाई(uint32_t set[8], स्थिर अक्षर *s) {
    int32_t length=0;
    अक्षर c;

    जबतक((c=*s++)!=0) {
        SET_ADD(set, c);
        ++length;
    }
    वापस length;
}

अटल int32_t
calcAlgNameSetsLengths(int32_t max_Name_लंबाई) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_गिनती;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->algNamesOffset);
    range_गिनती=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    जबतक(range_गिनती>0) {
        चयन(range->type) {
        हाल 0:
            /* name = prefix + (range->variant times) hex-digits */
            /* prefix */
            length = calc_String_Set_लंबाई(gNameSet, reinterpret_cast<स्थिर अक्षर*>(range + 1)) + range->variant;
            अगर(length>max_Name_लंबाई) {
                max_Name_लंबाई=length;
            }
            अवरोध;
        हाल 1: {
            /* name = prefix factorized-elements */
            स्थिर uint16_t* factors = reinterpret_cast<स्थिर uint16_t*>(range + 1);
            स्थिर अक्षर *s;
            int32_t i, count=range->variant, factor, factor_लंबाई, max_Factor_लंबाई;

            /* prefix length */
            s = reinterpret_cast<स्थिर अक्षर*>(factors + count);
            length=calc_String_Set_लंबाई(gNameSet, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            क्रम(i=0; i<count; ++i) {
                max_Factor_लंबाई=0;
                क्रम(factor=factors[i]; factor>0; --factor) {
                    factor_लंबाई=calc_String_Set_लंबाई(gNameSet, s);
                    s+=factor_लंबाई+1;
                    अगर(factor_लंबाई>max_Factor_लंबाई) {
                        max_Factor_लंबाई=factor_लंबाई;
                    }
                }
                length+=max_Factor_लंबाई;
            }

            अगर(length>max_Name_लंबाई) {
                max_Name_लंबाई=length;
            }
            अवरोध;
        }
        शेष:
            /* unknown type */
            अवरोध;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_गिनती;
    }
    वापस max_Name_लंबाई;
}

अटल int32_t
calcExtNameSetsLengths(int32_t max_Name_लंबाई) {
    int32_t i, length;

    क्रम(i=0; i<UPRV_LENGTHOF(charCatNames); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_Set_लंबाई(gNameSet, charCatNames[i]);
        अगर(length>max_Name_लंबाई) {
            max_Name_लंबाई=length;
        }
    }
    वापस max_Name_लंबाई;
}

अटल int32_t
calc_Name_Set_लंबाई(स्थिर uint16_t *tokens, uint16_t token_गिनती, स्थिर uint8_t *tokenStrings, int8_t *tokenLengths,
                  uint32_t set[8],
                  स्थिर uint8_t **p_पंक्ति, स्थिर uint8_t *पंक्ति_Limit) {
    स्थिर uint8_t *पंक्ति=*p_पंक्ति;
    int32_t length=0, token_लंबाई;
    uint16_t c, token;

    जबतक (पंक्ति != पंक्ति_Limit && (c = *पंक्ति++) != static_cast<uint8_t>(';')) {
        अगर(c>=token_गिनती) {
            /* implicit letter */
            SET_ADD(set, c);
            ++length;
        } अन्यथा {
            token=tokens[c];
            अगर (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*पंक्ति++;
                token=tokens[c];
            }
            अगर (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                SET_ADD(set, c);
                ++length;
            } अन्यथा {
                /* count token word */
                अगर(tokenLengths!=nullptr) {
                    /* use cached token length */
                    token_लंबाई=tokenLengths[c];
                    अगर(token_लंबाई==0) {
                        token_लंबाई = calc_String_Set_लंबाई(set, reinterpret_cast<स्थिर अक्षर*>(tokenStrings) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_लंबाई);
                    }
                } अन्यथा {
                    token_लंबाई = calc_String_Set_लंबाई(set, reinterpret_cast<स्थिर अक्षर*>(tokenStrings) + token);
                }
                length+=token_लंबाई;
            }
        }
    }

    *p_पंक्ति=पंक्ति;
    वापस length;
}

अटल व्योम
calcGroupNameSetsLengths(int32_t max_Name_लंबाई) {
    uint16_t offsets[पंक्तियाँ_PER_GROUP+2], lengths[पंक्तियाँ_PER_GROUP+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(uCharNames) + 8;
    uint16_t token_गिनती=*tokens++;
    uint8_t* tokenStrings = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->tokenStringOffset;

    int8_t *tokenLengths;

    स्थिर uint16_t *group;
    स्थिर uint8_t *s, *पंक्ति, *पंक्ति_Limit;

    int32_t group_गिनती, पंक्ति_संख्या, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_गिनती));
    अगर(tokenLengths!=nullptr) {
        uprv_memset(tokenLengths, 0, token_गिनती);
    }

    group=GET_GROUPS(uCharNames);
    group_गिनती=*group++;

    /* enumerate all groups */
    जबतक(group_गिनती>0) {
        s = reinterpret_cast<uint8_t*>(uCharNames) + uCharNames->groupStringOffset + GET_GROUP_OFFSET(group);
        s=expandGroupLengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        क्रम(पंक्ति_संख्या=0; पंक्ति_संख्या<पंक्तियाँ_PER_GROUP; ++पंक्ति_संख्या) {
            पंक्ति=s+offsets[पंक्ति_संख्या];
            length=lengths[पंक्ति_संख्या];
            अगर(length==0) {
                जारी;
            }

            पंक्ति_Limit=पंक्ति+length;

            /* read regular name */
            length=calc_Name_Set_लंबाई(tokens, token_गिनती, tokenStrings, tokenLengths, gNameSet, &पंक्ति, पंक्ति_Limit);
            अगर(length>max_Name_लंबाई) {
                max_Name_लंबाई=length;
            }
            अगर(पंक्ति==पंक्ति_Limit) {
                जारी;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_Set_लंबाई(tokens, token_गिनती, tokenStrings, tokenLengths, gNameSet, &पंक्ति, पंक्ति_Limit);
            अगर(length>max_Name_लंबाई) {
                max_Name_लंबाई=length;
            }
            अगर(पंक्ति==पंक्ति_Limit) {
                जारी;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        group=अग्र_GROUP(group);
        --group_गिनती;
    }

    अगर(tokenLengths!=nullptr) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_लंबाई=max_Name_लंबाई;
}

अटल UBool
calcNameSetsLengths(UErrorCode *pErrorCode) {
    अटल स्थिर अक्षर extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_लंबाई;

    अगर(g_Max_Name_लंबाई!=0) {
        वापस true;
    }

    अगर(!is_तथ्य_Loaded(pErrorCode)) {
        वापस false;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    क्रम (i = 0; i < static_cast<int32_t>(माप(extChars)) - 1; ++i) {
        SET_ADD(gNameSet, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_लंबाई=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_लंबाई=calcExtNameSetsLengths(max_Name_लंबाई);

    /* set sets and lengths from group names, set global maximum values */
    calcGroupNameSetsLengths(max_Name_लंबाई);

    वापस true;
}

U_NAMESPACE_END

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 code, UCharNameChoice nameChoice,
           अक्षर *बफर, int32_t बफर_लंबाई,
           UErrorCode *pErrorCode) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    } अन्यथा अगर(nameChoice>=U_CHAR_NAME_CHOICE_गिनती ||
              बफर_लंबाई<0 || (बफर_लंबाई>0 && बफर==nullptr)
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर((uint32_t)code>UCHAR_MAX_मूल्य || !is_तथ्य_Loaded(pErrorCode)) {
        वापस u_terminateChars(बफर, बफर_लंबाई, 0, pErrorCode);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जबतक(i>0) {
        अगर(algRange->start<=(uint32_t)code && (uint32_t)code<=algRange->end) {
            length=getAlgName(algRange, (uint32_t)code, nameChoice, बफर, (uint16_t)बफर_लंबाई);
            अवरोध;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    अगर(i==0) {
        अगर (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(uCharNames, (uint32_t )code, U_EXTENDED_CHAR_NAME, बफर, (uint16_t) बफर_लंबाई);
            अगर (!length) {
                /* extended character name */
                length = getExtName((uint32_t) code, बफर, (uint16_t) बफर_लंबाई);
            }
        } अन्यथा {
            /* normal character name */
            length=getName(uCharNames, (uint32_t)code, nameChoice, बफर, (uint16_t)बफर_लंबाई);
        }
    }

    वापस u_terminateChars(बफर, बफर_लंबाई, length, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                अक्षर *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    /* check the argument values */
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    } अन्यथा अगर(destCapacity<0 || (destCapacity>0 && dest==nullptr)) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    वापस u_terminateChars(dest, destCapacity, 0, pErrorCode);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               स्थिर अक्षर *स्थिर name,
               UErrorCode *pErrorCode) {
    अक्षर lower[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    अक्षर c0;
    अटल constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस error;
    }

    अगर(nameChoice>=U_CHAR_NAME_CHOICE_गिनती || name==nullptr || *name==0) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस error;
    }

    अगर(!is_तथ्य_Loaded(pErrorCode)) {
        वापस error;
    }

    /* construct the lowercase of the name first */
    स्थिर अक्षर* nameChar = name;
    क्रम(i=0; i<माप(lower); ++i) {
        अगर((c0=*nameChar++)!=0) {
            lower[i]=uprv_tolower(c0);
        } अन्यथा {
            अवरोध;
        }
    }
    अगर(i==माप(lower)) {
        /* name too long, there is no such character */
        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        वापस error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    अगर (lower[0] == '<') {
        अगर (nameChoice == U_EXTENDED_CHAR_NAME && lower[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t limit = i;
            जबतक (i >= 3 && lower[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_लंबाई = limit - (i + 1);
            अगर (i >= 2 && lower[i] == '-' && 1 <= hex_लंबाई && hex_लंबाई <= 8) {
                uint32_t cIdx;

                lower[i] = 0;

                क्रम (++i; i < limit; ++i) {
                    अगर (lower[i] >= '0' && lower[i] <= '9') {
                        cp = (cp << 4) + lower[i] - '0';
                    } अन्यथा अगर (lower[i] >= 'a' && lower[i] <= 'f') {
                        cp = (cp << 4) + lower[i] - 'a' + 10;
                    } अन्यथा {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        वापस error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    अगर (cp > UCHAR_MAX_मूल्य) {
                        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
                        वापस error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                क्रम (lower[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(charCatNames); ++cIdx) {

                    अगर (!uprv_strcmp(lower + 1, charCatNames[cIdx])) {
                        अगर (cat == cIdx) {
                            वापस cp;
                        }
                        अवरोध;
                    }
                }
            }
        }

        *pErrorCode = U_ILLEGAL_CHAR_FOUND;
        वापस error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जबतक(i>0) {
        अगर((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            वापस cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.code=error;
    enumNames(uCharNames, 0, UCHAR_MAX_मूल्य + 1, DO_FIND_NAME, &findName, nameChoice);
    अगर (findName.code == error) {
         *pErrorCode = U_ILLEGAL_CHAR_FOUND;
    }
    वापस findName.code;
}

U_CAPI व्योम U_EXPORT2
u_enumCharNames(UChar32 start, UChar32 limit,
                UEnumCharNamesFn *fn,
                व्योम *context,
                UCharNameChoice nameChoice,
                UErrorCode *pErrorCode) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }

    अगर(nameChoice>=U_CHAR_NAME_CHOICE_गिनती || fn==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    अगर((uint32_t) limit > UCHAR_MAX_मूल्य + 1) {
        limit = UCHAR_MAX_मूल्य + 1;
    }
    अगर((uint32_t)start>=(uint32_t)limit) {
        वापस;
    }

    अगर(!is_तथ्य_Loaded(pErrorCode)) {
        वापस;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)uCharNames+uCharNames->algNamesOffset);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    जबतक(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        अगर((uint32_t)start<algRange->start) {
            अगर((uint32_t)limit<=algRange->start) {
                enumNames(uCharNames, start, limit, fn, context, nameChoice);
                वापस;
            }
            अगर(!enumNames(uCharNames, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                वापस;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        अगर((uint32_t)start<=algRange->end) {
            अगर((uint32_t)limit<=(algRange->end+1)) {
                enumAlgNames(algRange, start, limit, fn, context, nameChoice);
                वापस;
            }
            अगर(!enumAlgNames(algRange, start, (UChar32)algRange->end+1, fn, context, nameChoice)) {
                वापस;
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
uprv_get_Max_Char_Name_लंबाई() {
    UErrorCode errorCode=U_ZERO_ERROR;
    अगर(calcNameSetsLengths(&errorCode)) {
        वापस g_Max_Name_लंबाई;
    } अन्यथा {
        वापस 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
अटल व्योम
charSetToUSet(uint32_t cset[8], स्थिर USetAdder *sa) {
    char16_t us[256];
    अक्षर cs[256];

    int32_t i, length;
    UErrorCode errorCode;

    errorCode=U_ZERO_ERROR;

    अगर(!calcNameSetsLengths(&errorCode)) {
        वापस;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    क्रम(i=0; i<256; ++i) {
        अगर(SET_CONTAINS(cset, i)) {
            cs[length++] = static_cast<अक्षर>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_charsToUChars(cs, us, length);

    /* add each char16_t to the USet */
    क्रम(i=0; i<length; ++i) {
        अगर(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->add(sa->set, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI व्योम U_EXPORT2
uprv_getCharNameCharacters(स्थिर USetAdder *sa) {
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
अटल व्योम
makeTokenMap(स्थिर UDataSwapper *ds,
             int16_t tokens[], uint16_t token_गिनती,
             uint8_t map[256],
             UErrorCode *pErrorCode) {
    UBool usedOutChar[256];
    uint16_t i, j;
    uint8_t c1, c2;

    अगर(U_FAILURE(*pErrorCode)) {
        वापस;
    }

    अगर(ds->inCharset==ds->outCharset) {
        /* Same charset family: identity permutation */
        क्रम(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } अन्यथा {
        uprv_memset(map, 0, 256);
        uprv_memset(usedOutChar, 0, 256);

        अगर(token_गिनती>256) {
            token_गिनती=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        क्रम(i=1; i<token_गिनती; ++i) {
            अगर(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, pErrorCode);
                अगर(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->inCharset);
                    वापस;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                usedOutChar[c2]=true;
            }
        }

        /* set the mappings for the rest of the permutation */
        क्रम(i=j=1; i<token_गिनती; ++i) {
            /* set mappings that were not set for direct bytes */
            अगर(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                जबतक(usedOutChar[j]) {
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
uchar_swapNames(स्थिर UDataSwapper *ds,
                स्थिर व्योम *in_तथ्य, int32_t length, व्योम *out_तथ्य,
                UErrorCode *pErrorCode) {
    स्थिर UDataInfo *pInfo;
    int32_t header_आकार;

    स्थिर uint8_t *inBytes;
    uint8_t *outBytes;

    uint32_t tokenStringOffset, groupsOffset, groupStringOffset, algNamesOffset,
             offset, i, count, strings_गिनती;

    स्थिर AlgorithmicRange *inRange;
    AlgorithmicRange *outRange;

    /* udata_swapDataHeader checks the arguments */
    header_आकार=udata_swap_तथ्य_Header(ds, in_तथ्य, length, out_तथ्य, pErrorCode);
    अगर(pErrorCode==nullptr || U_FAILURE(*pErrorCode)) {
        वापस 0;
    }

    /* check data format and format version */
    pInfo=(स्थिर UDataInfo *)((स्थिर अक्षर *)in_तथ्य+4);
    अगर(!(
        pInfo->तथ्य_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->तथ्य_Format[1]==0x6e &&
        pInfo->तथ्य_Format[2]==0x61 &&
        pInfo->तथ्य_Format[3]==0x6d &&
        pInfo->formatVersion[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->तथ्य_Format[0], pInfo->तथ्य_Format[1],
                         pInfo->तथ्य_Format[2], pInfo->तथ्य_Format[3],
                         pInfo->formatVersion[0]);
        *pErrorCode=U_UNSUPPORTED_ERROR;
        वापस 0;
    }

    inBytes=(स्थिर uint8_t *)in_तथ्य+header_आकार;
    outBytes=(out_तथ्य == nullptr) ? nullptr : (uint8_t *)out_तथ्य+header_आकार;
    अगर(length<0) {
        algNamesOffset=ds->readUInt32(((स्थिर uint32_t *)inBytes)[3]);
    } अन्यथा {
        length-=header_आकार;
        अगर( length<20 ||
            (uint32_t)length<(algNamesOffset=ds->readUInt32(((स्थिर uint32_t *)inBytes)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
            वापस 0;
        }
    }

    अगर(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((स्थिर uint32_t *)(inBytes+offset)));
        offset+=4;

        क्रम(i=0; i<count; ++i) {
            inRange=(स्थिर AlgorithmicRange *)(inBytes+offset);
            offset+=ds->readUInt16(inRange->size);
        }
    } अन्यथा {
        /* swap data */
        स्थिर uint16_t *p;
        uint16_t *q, *अस्थायी;

        int16_t tokens[512];
        uint16_t token_गिनती;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        अगर(inBytes!=outBytes) {
            uprv_memcpy(outBytes, inBytes, length);
        }

        /* the initial 4 offsets first */
        tokenStringOffset=ds->readUInt32(((स्थिर uint32_t *)inBytes)[0]);
        groupsOffset=ds->readUInt32(((स्थिर uint32_t *)inBytes)[1]);
        groupStringOffset=ds->readUInt32(((स्थिर uint32_t *)inBytes)[2]);
        ds->swapArray32(ds, inBytes, 16, outBytes, pErrorCode);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(स्थिर uint16_t *)(inBytes+16);
        q=(uint16_t *)(outBytes+16);

        /* read and swap the tokenCount */
        token_गिनती=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, pErrorCode);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        अगर(token_गिनती<=512) {
            count=token_गिनती;
        } अन्यथा {
            count=512;
        }
        क्रम(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        क्रम(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_गिनती, map, pErrorCode);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_गिनती>256 ? token_गिनती-256 : 0), trailMap, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        अस्थायी=(uint16_t *)uprv_malloc(token_गिनती*2);
        अगर(अस्थायी==nullptr) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_गिनती);
            *pErrorCode=U_MEMORY_ALLOCATION_ERROR;
            वापस 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        क्रम(i=0; i<token_गिनती && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, अस्थायी+map[i], pErrorCode);
        }

        /* swap and permutate trail-byte tokens */
        क्रम(; i<token_गिनती; ++i) {
            ds->swapArray16(ds, p+i, 2, अस्थायी+(i&0xffffff00)+trailMap[i&0xff], pErrorCode);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, अस्थायी, token_गिनती*2);
        uprv_free(अस्थायी);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swapInvStringBlock(ds, inBytes+tokenStringOffset, (int32_t)(groupsOffset-tokenStringOffset),
                                    outBytes+tokenStringOffset, pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            वापस 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((स्थिर uint16_t *)(inBytes+groupsOffset)));
        ds->swapArray16(ds, inBytes+groupsOffset, (int32_t)((1+count*3)*2),
                           outBytes+groupsOffset, pErrorCode);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        अगर(ds->inCharset!=ds->outCharset) {
            uint16_t offsets[पंक्तियाँ_PER_GROUP+1], lengths[पंक्तियाँ_PER_GROUP+1];

            स्थिर uint8_t *inStrings, *अग्र_In_Strings;
            uint8_t *outStrings;

            uint8_t c;

            inStrings=inBytes+groupStringOffset;
            outStrings=outBytes+groupStringOffset;

            strings_गिनती=algNamesOffset-groupStringOffset;

            /* iterate through string groups until only a few padding bytes are left */
            जबतक(strings_गिनती>32) {
                अग्र_In_Strings=expandGroupLengths(inStrings, offsets, lengths);

                /* move past the length bytes */
                strings_गिनती-=(uint32_t)(अग्र_In_Strings-inStrings);
                outStrings+=अग्र_In_Strings-inStrings;
                inStrings=अग्र_In_Strings;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                strings_गिनती-=count;

                /* swap the string bytes using map[] and trailMap[] */
                जबतक(count>0) {
                    c=*inStrings++;
                    *outStrings++=map[c];
                    अगर(tokens[c]!=-2) {
                        --count;
                    } अन्यथा {
                        /* token lead byte: swap the trail byte, too */
                        *outStrings++=trailMap[*inStrings++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        offset=algNamesOffset;
        count=ds->readUInt32(*((स्थिर uint32_t *)(inBytes+offset)));
        ds->swapArray32(ds, inBytes+offset, 4, outBytes+offset, pErrorCode);
        offset+=4;

        क्रम(i=0; i<count; ++i) {
            अगर(offset>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *pErrorCode=U_सूचकांक_OUTOFBOUNDS_ERROR;
                वापस 0;
            }

            inRange=(स्थिर AlgorithmicRange *)(inBytes+offset);
            outRange=(AlgorithmicRange *)(outBytes+offset);
            offset+=ds->readUInt16(inRange->size);

            ds->swapArray32(ds, inRange, 8, outRange, pErrorCode);
            ds->swapArray16(ds, &inRange->size, 2, &outRange->size, pErrorCode);
            चयन(inRange->type) {
            हाल 0:
                /* swap prefix string */
                ds->swapInvChars(ds, inRange+1, (int32_t)uprv_strlen((स्थिर अक्षर *)(inRange+1)),
                                    outRange+1, pErrorCode);
                अगर(U_FAILURE(*pErrorCode)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    वापस 0;
                }
                अवरोध;
            हाल 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_गिनती;

                    factors_गिनती=inRange->variant;
                    p=(स्थिर uint16_t *)(inRange+1);
                    q=(uint16_t *)(outRange+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_गिनती*2), q, pErrorCode);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_गिनती;
                    q+=factors_गिनती;
                    strings_गिनती=(uint32_t)((inBytes+offset)-(स्थिर uint8_t *)p);
                    जबतक(strings_गिनती>0 && ((स्थिर uint8_t *)p)[strings_गिनती-1]!=0) {
                        --strings_गिनती;
                    }
                    ds->swapInvChars(ds, p, (int32_t)strings_गिनती, q, pErrorCode);
                }
                अवरोध;
            शेष:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 inRange->type, i);
                *pErrorCode=U_UNSUPPORTED_ERROR;
                वापस 0;
            }
        }
    }

    वापस header_आकार+(int32_t)offset;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
