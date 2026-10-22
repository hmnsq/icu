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

स्थिर ध्रुव वर्ण दत्त_NAME[] = "unames";
स्थिर ध्रुव वर्ण दत्त_TYPE[] = "icu";

#परिभाषय गणः_सरणम् 5
#परिभाषय पङ्क्तयः_प्रत्येकम्_गणः (1L<<गणः_सरणम्)
#परिभाषय गणः_आवरणम् (पङ्क्तयः_प्रत्येकम्_गणः-1)

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
    गणः_MSB,
    गणः_विस्थापनम्_उच्चः,
    गणः_विस्थापनम्_नीचः,
    गणः_दीर्घता
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#परिभाषय GET_गणः_विस्थापनम्(गणः) ((int32_t)(गणः)[गणः_विस्थापनम्_उच्चः]<<16|(गणः)[गणः_विस्थापनम्_नीचः])

#परिभाषय अग्र_गणः(गणः) ((गणः)+गणः_दीर्घता)
#परिभाषय पूर्व_गणः(गणः) ((गणः)-गणः_दीर्घता)

प्रकारनिर्देश संरचना {
    uint32_t start, अन्तः;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

प्रकारनिर्देश संरचना {
    uint32_t token_String_विस्थापनम्, groups_विस्थापनम्, गणः_String_विस्थापनम्, alg_नामानि_विस्थापनम्;
} UChar_नामानि;

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

            यदि (skeletonIterator == query.skeleton.अन्तः()) {
                प्रत्यागम असत्य;
            }
            यदि (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                यदि (चरमः_Char == '-') {
                    यदि (*skeletonIterator++ != चरमः_Char) {
                        प्रत्यागम असत्य;
                    }
                }
                चरमः_Char = c;
                प्रत्यागम सत्य;
            } अन्यथा यदि (c == '-') {
                यदि (चरमः_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.अन्तः() - 2)) {
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
                    यदि (*skeletonIterator++ != c) {
                        प्रत्यागम असत्य;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                चरमः_Char = c;
                प्रत्यागम सत्य;
            } अन्यथा {
                यदि (*skeletonIterator++ != c) {
                    प्रत्यागम असत्य;
                }
                चरमः_Char = c;
                प्रत्यागम सत्य;
            }
        }

        तार्किक consistentWith(ध्रुव std::string_view substring) {
            प्रति (ध्रुव वर्ण c : substring) {
                यदि (!consistentWith(c)) {
                    प्रत्यागम असत्य;
                }
            }
            प्रत्यागम सत्य;
        }

        तार्किक matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(चरमः_Char != '-');
            प्रत्यागम skeletonIterator == query.skeleton.अन्तः();
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
        वर्ण चरमः_Char = ' ';
    };

    स्पष्ट CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        वर्ण *skeleton_सीमा = skeleton_दत्त;
        प्रति (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_सीमा < skeleton_दत्त + परिमाण(skeleton_दत्त));
            यदि (skeleton_सीमा >= skeleton_दत्त + परिमाण(skeleton_दत्त)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_सीमा = skeleton_दत्त;
                भङ्ग;
            }
            यदि (query[i] == ' ' || query[i] == '_') {
                अनुवर्त;
            }
            यदि (query[i] == '-') {
                तार्किक isMedial;
                तार्किक is1180MedialHyphen = असत्य;
                यदि (i == 0 || i == query.length() - 1) {
                    isMedial = असत्य;
                } अन्यथा {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                यदि (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_दत्त, skeleton_सीमा - skeleton_दत्त) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = सत्य;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    प्रति (std::size_t j = i + 2; j < query.length(); ++j) {
                        यदि (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = असत्य;
                        }
                    }
                }
                यदि (!isMedial || is1180MedialHyphen) {
                    *skeleton_सीमा++ = query[i];
                }
            } अन्यथा {
                *skeleton_सीमा++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_दत्त, skeleton_सीमा - skeleton_दत्त);
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

    वर्ण skeleton_दत्त[120];
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
#परिभाषय GET_GROUPS(नामानि) (ध्रुव uint16_t *)((ध्रुव वर्ण *)नामानि+नामानि->groups_विस्थापनम्)

प्रकारनिर्देश संरचना {
    ध्रुव CharacterNameQuery *otherName;
    UChar32 संहिता;
} FindName;

#परिभाषय DO_FIND_NAME शून्यसूचकः

स्थिर UData_स्मृतिः *u_Char_नामानि_दत्त=शून्यसूचकः;
स्थिर UChar_नामानि *u_Char_नामानि=शून्यसूचकः;
स्थिर icu::UInitOnce g_Char_नामानि_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
स्थिर int32_t g_Max_Name_दीर्घता=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
स्थिर uint32_t g_Name_समुच्चयः[8]={ 0 };

#परिभाषय U_NONCHARACTER_संहिता_POINT U_CHAR_CATEGORY_गणनम्
#परिभाषय U_LEAD_SURROGATE U_CHAR_CATEGORY_गणनम् + 1
#परिभाषय U_TRAIL_SURROGATE U_CHAR_CATEGORY_गणनम् + 2

#परिभाषय U_CHAR_EXTENDED_CATEGORY_गणनम् (U_CHAR_CATEGORY_गणनम् + 3)

स्थिर ध्रुव वर्ण * ध्रुव char_Cat_नामानि[U_CHAR_EXTENDED_CATEGORY_गणनम्] = {
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
    यदि(u_Char_नामानि_दत्त) {
        udata_close(u_Char_नामानि_दत्त);
        u_Char_नामानि_दत्त = शून्यसूचकः;
    }
    यदि(u_Char_नामानि) {
        u_Char_नामानि = शून्यसूचकः;
    }
    g_Char_नामानि_Init_Once.reset();
    g_Max_Name_दीर्घता=0;
    प्रत्यागम सत्य;
}

स्थिर UBool U_CALLCONV
isAcceptable(शून्य * /*context*/,
             ध्रुव वर्ण * /*type*/, ध्रुव वर्ण * /*name*/,
             ध्रुव UDataInfo *pInfo) {
    प्रत्यागम
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->दत्त_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->दत्त_Format[1]==0x6e &&
        pInfo->दत्त_Format[2]==0x61 &&
        pInfo->दत्त_Format[3]==0x6d &&
        pInfo->format_संस्करणम्[0]==1;
}

स्थिर शून्य U_CALLCONV
आरोपय_Char_नामानि(UError_संहिता &स्थितिः) {
    U_ASSERT(u_Char_नामानि_दत्त == शून्यसूचकः);
    U_ASSERT(u_Char_नामानि == शून्यसूचकः);

    u_Char_नामानि_दत्त = udata_openChoice(शून्यसूचकः, दत्त_TYPE, दत्त_NAME, isAcceptable, शून्यसूचकः, &स्थितिः);
    यदि(U_FAILURE(स्थितिः)) {
        u_Char_नामानि_दत्त = शून्यसूचकः;
    } अन्यथा {
        u_Char_नामानि = (UChar_नामानि *)udata_get_स्मृतिः(u_Char_नामानि_दत्त);
    }
    ucln_common_पञ्जिकाकोशः_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


स्थिर UBool
is_दत्त_Loaded(UError_संहिता *p_Error_संहिता) {
    umtx_initOnce(g_Char_नामानि_Init_Once, &आरोपय_Char_नामानि, *p_Error_संहिता);
    प्रत्यागम U_SUCCESS(*p_Error_संहिता);
}

#परिभाषय WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c) UPRV_खण्डम्_MACRO_BEGIN { \
    यदि((बफर_दीर्घता)>0) { \
        *(बफर)++=c; \
        --(बफर_दीर्घता); \
    } \
    ++(बफर_Pos); \
} UPRV_खण्डम्_MACRO_अन्तः

#परिभाषय U_ISO_COMMENT U_CHAR_NAME_CHOICE_गणनम्

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
expandName(UChar_नामानि *नामानि,
           ध्रुव uint8_t *name, uint16_t name_दीर्घता, UCharNameChoice nameChoice,
           वर्ण *बफर, uint16_t बफर_दीर्घता) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(नामानि) + 8;
    uint16_t token, token_गणनम्=*tokens++, बफर_Pos=0;
    uint8_t* token_वाक्यानि = reinterpret_cast<uint8_t*>(नामानि) + नामानि->token_String_विस्थापनम्;
    uint8_t c;

    यदि(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        यदि (static_cast<uint8_t>(';') >= token_गणनम् || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णाङ्क क्षेत्रम्_सूचकांक= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            कुरु {
                यावत्(name_दीर्घता>0) {
                    --name_दीर्घता;
                    यदि(*name++==';') {
                        भङ्ग;
                    }
                }
            } यावत्(--क्षेत्रम्_सूचकांक>0);
        } अन्यथा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_दीर्घता=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    यावत्(name_दीर्घता>0) {
        --name_दीर्घता;
        c=*name++;

        यदि(c>=token_गणनम्) {
            यदि(c!=';') {
                /* implicit letter */
                WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
            } अन्यथा {
                /*परिसमाप्तम्*/
                भङ्ग;
            }
        } अन्यथा {
            token=tokens[c];
            यदि (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --name_दीर्घता;
            }
            यदि (token == static_cast<uint16_t>(-1)) {
                यदि(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
                } अन्यथा {
                    /*परिसमाप्तम्*/
                    भङ्ग;
                }
            } अन्यथा {
                /* write token word */
                uint8_t *tokenString=token_वाक्यानि+token;
                यावत्((c=*tokenString++)!=0) {
                    WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    यदि(बफर_दीर्घता>0) {
        *बफर=0;
    }

    प्रत्यागम बफर_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
स्थिर UBool
compareName(UChar_नामानि *नामानि,
            ध्रुव uint8_t *name, uint16_t name_दीर्घता, UCharNameChoice nameChoice,
            ध्रुव CharacterNameQuery& otherName) {
    uint16_t* tokens = reinterpret_cast<uint16_t*>(नामानि) + 8;
    uint16_t token, token_गणनम्=*tokens++;
    uint8_t* token_वाक्यानि = reinterpret_cast<uint8_t*>(नामानि) + नामानि->token_String_विस्थापनम्;
    uint8_t c;
    स्वचालित matcher = otherName.matcher();

    यदि(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        यदि (static_cast<uint8_t>(';') >= token_गणनम् || tokens[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            पूर्णाङ्क क्षेत्रम्_सूचकांक= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            कुरु {
                यावत्(name_दीर्घता>0) {
                    --name_दीर्घता;
                    यदि(*name++==';') {
                        भङ्ग;
                    }
                }
            } यावत्(--क्षेत्रम्_सूचकांक>0);
        } अन्यथा {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_दीर्घता=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    यावत्(name_दीर्घता>0) {
        --name_दीर्घता;
        c=*name++;

        यदि(c>=token_गणनम्) {
            यदि(c!=';') {
                /* implicit letter */
                यदि (!matcher.consistentWith(c)) {
                    प्रत्यागम असत्य;
                }
            } अन्यथा {
                /*परिसमाप्तम्*/
                भङ्ग;
            }
        } अन्यथा {
            token=tokens[c];
            यदि (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=tokens[c<<8|*name++];
                --name_दीर्घता;
            }
            यदि (token == static_cast<uint16_t>(-1)) {
                यदि(c!=';') {
                    /* explicit letter */
                    यदि (!matcher.consistentWith(c)) {
                        प्रत्यागम असत्य;
                    }
                } अन्यथा {
                    /*परिसमाप्तम्*/
                    भङ्ग;
                }
            } अन्यथा {
                /* write token word */
                यदि (!matcher.consistentWith(reinterpret_cast<ध्रुव वर्ण *>(token_वाक्यानि + token))) {
                    प्रत्यागम असत्य;
                }
            }
        }
    }

    /* complete match? */
    प्रत्यागम matcher.matches();
}

स्थिर uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    यदि (U_IS_UNICODE_NONCHAR(cp)) {
        प्रत्यागम U_NONCHARACTER_संहिता_POINT;
    }

    यदि ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    प्रत्यागम cat;
}

स्थिर ध्रुव वर्ण *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    यदि (cat >= UPRV_LENGTHOF(char_Cat_नामानि)) {
        प्रत्यागम "unknown";
    } अन्यथा {
        प्रत्यागम char_Cat_नामानि[cat];
    }
}

स्थिर uint16_t getExtName(uint32_t संहिता, वर्ण *बफर, uint16_t बफर_दीर्घता) {
    ध्रुव वर्ण *catname = getCharCatName(संहिता);
    uint16_t length = 0;

    UChar32 cp;
    पूर्णाङ्क ndigits, i;
    
    WRITE_CHAR(बफर, बफर_दीर्घता, length, '<');
    यावत् (catname[length - 1]) {
        WRITE_CHAR(बफर, बफर_दीर्घता, length, catname[length - 1]);
    }
    WRITE_CHAR(बफर, बफर_दीर्घता, length, '-');
    प्रति (cp = संहिता, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    यदि (ndigits < 4)
        ndigits = 4;
    प्रति (cp = संहिता, i = ndigits; (cp || i > 0) && बफर_दीर्घता; cp >>= 4, बफर_दीर्घता--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        बफर[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    बफर += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(बफर, बफर_दीर्घता, length, '>');

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
get_गणः(UChar_नामानि *नामानि, uint32_t संहिता) {
    ध्रुव uint16_t *groups=GET_GROUPS(नामानि);
    uint16_t गणः_MSB_2 = static_cast<uint16_t>(संहिता >> गणः_सरणम्),
             start=0,
             सीमा=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    यावत्(start<सीमा-1) {
        number = static_cast<uint16_t>((start + सीमा) / 2);
        यदि(गणः_MSB_2<groups[number*गणः_दीर्घता+गणः_MSB]) {
            सीमा=number;
        } अन्यथा {
            start=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    प्रत्यागम groups+start*गणः_दीर्घता;
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
expand_गणः_Lengths(ध्रुव uint8_t *s,
                   uint16_t offsets[पङ्क्तयः_प्रत्येकम्_गणः+1], uint16_t lengths[पङ्क्तयः_प्रत्येकम्_गणः+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, विस्थापनम्=0, length=0;
    uint8_t दीर्घता_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    यावत्(i<पङ्क्तयः_प्रत्येकम्_गणः) {
        दीर्घता_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        यदि(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | दीर्घता_Byte >> 4) + 12);
            दीर्घता_Byte&=0xf;
        } अन्यथा यदि((दीर्घता_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((दीर्घता_Byte & 0x3f) + 12);
        } अन्यथा {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(दीर्घता_Byte >> 4);
            दीर्घता_Byte&=0xf;
        }

        *offsets++=विस्थापनम्;
        *lengths++=length;

        विस्थापनम्+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        यदि((दीर्घता_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=दीर्घता_Byte;
            यदि(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=विस्थापनम्;
                *lengths++=length;

                विस्थापनम्+=length;
                ++i;
            }
        } अन्यथा {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    प्रत्यागम s;
}

स्थिर uint16_t
expand_गणः_Name(UChar_नामानि *नामानि, ध्रुव uint16_t *गणः,
                uint16_t पङ्क्तिः_सङ्ख्या, UCharNameChoice nameChoice,
                वर्ण *बफर, uint16_t बफर_दीर्घता) {
    uint16_t offsets[पङ्क्तयः_प्रत्येकम्_गणः+2], lengths[पङ्क्तयः_प्रत्येकम्_गणः+2];
    ध्रुव uint8_t* s = reinterpret_cast<uint8_t*>(नामानि) + नामानि->गणः_String_विस्थापनम् + GET_गणः_विस्थापनम्(गणः);
    s=expand_गणः_Lengths(s, offsets, lengths);
    प्रत्यागम expandName(नामानि, s+offsets[पङ्क्तिः_सङ्ख्या], lengths[पङ्क्तिः_सङ्ख्या], nameChoice,
                      बफर, बफर_दीर्घता);
}

स्थिर uint16_t
getName(UChar_नामानि *नामानि, uint32_t संहिता, UCharNameChoice nameChoice,
        वर्ण *बफर, uint16_t बफर_दीर्घता) {
    ध्रुव uint16_t *गणः=get_गणः(नामानि, संहिता);
    यदि (static_cast<uint16_t>(संहिता >> गणः_सरणम्) == गणः[गणः_MSB]) {
        प्रत्यागम expand_गणः_Name(नामानि, गणः, static_cast<uint16_t>(संहिता & गणः_आवरणम्), nameChoice,
                               बफर, बफर_दीर्घता);
    } अन्यथा {
        /* group not found */
        /* zero-terminate */
        यदि(बफर_दीर्घता>0) {
            *बफर=0;
        }
        प्रत्यागम 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
स्थिर UBool
enum_गणः_नामानि(UChar_नामानि *नामानि, ध्रुव uint16_t *गणः,
               UChar32 start, UChar32 अन्तः,
               UEnum_Char_नामानि_Fn *fn, शून्य *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[पङ्क्तयः_प्रत्येकम्_गणः+2], lengths[पङ्क्तयः_प्रत्येकम्_गणः+2];
    ध्रुव uint8_t* s = reinterpret_cast<uint8_t*>(नामानि) + नामानि->गणः_String_विस्थापनम् + GET_गणः_विस्थापनम्(गणः);

    s=expand_गणः_Lengths(s, offsets, lengths);
    यदि(fn!=DO_FIND_NAME) {
        वर्ण बफर[200];
        uint16_t length;

        यावत्(start<=अन्तः) {
            length=expandName(नामानि, s+offsets[start&गणः_आवरणम्], lengths[start&गणः_आवरणम्], nameChoice, बफर, परिमाण(बफर));
            यदि (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                बफर[length = getExtName(start, बफर, परिमाण(बफर))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            यदि(length>0) {
                यदि(!fn(context, start, nameChoice, बफर, length)) {
                    प्रत्यागम असत्य;
                }
            }
            ++start;
        }
    } अन्यथा {
        ध्रुव CharacterNameQuery& otherName = *static_cast<FindName*>(context)->otherName;
        यावत्(start<=अन्तः) {
            यदि(compareName(नामानि, s+offsets[start&गणः_आवरणम्], lengths[start&गणः_आवरणम्], nameChoice, otherName)) {
                static_cast<FindName*>(context)->संहिता = start;
                प्रत्यागम असत्य;
            }
            ++start;
        }
    }
    प्रत्यागम सत्य;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
स्थिर UBool
enum_Ext_नामानि(UChar32 start, UChar32 अन्तः,
             UEnum_Char_नामानि_Fn *fn, शून्य *context)
{
    यदि(fn!=DO_FIND_NAME) {
        वर्ण बफर[200];
        uint16_t length;
        
        यावत्(start<=अन्तः) {
            बफर[length = getExtName(start, बफर, परिमाण(बफर))] = 0;
            /* here, we assume that the buffer is large enough */
            यदि(length>0) {
                यदि(!fn(context, start, U_EXTENDED_CHAR_NAME, बफर, length)) {
                    प्रत्यागम असत्य;
                }
            }
            ++start;
        }
    }

    प्रत्यागम सत्य;
}

स्थिर UBool
enum_नामानि(UChar_नामानि *नामानि,
          UChar32 start, UChar32 सीमा,
          UEnum_Char_नामानि_Fn *fn, शून्य *context,
          UCharNameChoice nameChoice) {
    uint16_t start_गणः_MSB, अन्तः_गणः_MSB, गणः_गणनम्;
    ध्रुव uint16_t *गणः, *गणः_सीमा;

    start_गणः_MSB = static_cast<uint16_t>(start >> गणः_सरणम्);
    अन्तः_गणः_MSB = static_cast<uint16_t>((सीमा - 1) >> गणः_सरणम्);

    /* find the group that contains start, or the highest before it */
    गणः=get_गणः(नामानि, start);

    यदि(start_गणः_MSB<गणः[गणः_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_सीमा = static_cast<UChar32>(गणः[गणः_MSB]) << गणः_सरणम्;
        यदि(ext_सीमा>सीमा) {
            ext_सीमा=सीमा;
        }
        यदि(!enum_Ext_नामानि(start, ext_सीमा-1, fn, context)) {
            प्रत्यागम असत्य;
        }
        start=ext_सीमा;
    }

    यदि(start_गणः_MSB==अन्तः_गणः_MSB) {
        यदि(start_गणः_MSB==गणः[गणः_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            प्रत्यागम enum_गणः_नामानि(नामानि, गणः, start, सीमा-1, fn, context, nameChoice);
        }
    } अन्यथा {
        ध्रुव uint16_t *groups=GET_GROUPS(नामानि);
        गणः_गणनम्=*groups++;
        गणः_सीमा=groups+गणः_गणनम्*गणः_दीर्घता;

        यदि(start_गणः_MSB==गणः[गणः_MSB]) {
            /* enumerate characters in the partial start group */
            यदि((start&गणः_आवरणम्)!=0) {
                यदि(!enum_गणः_नामानि(नामानि, गणः,
                                   start, (static_cast<UChar32>(start_गणः_MSB) << गणः_सरणम्) + पङ्क्तयः_प्रत्येकम्_गणः - 1,
                                   fn, context, nameChoice)) {
                    प्रत्यागम असत्य;
                }
                गणः=अग्र_गणः(गणः); /* continue with the next group */
            }
        } अन्यथा यदि(start_गणः_MSB>गणः[गणः_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ध्रुव uint16_t *अग्र_गणः_2=अग्र_गणः(गणः);
            यदि (अग्र_गणः_2 < गणः_सीमा && अग्र_गणः_2[गणः_MSB] > start_गणः_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 अन्तः = अग्र_गणः_2[गणः_MSB] << गणः_सरणम्;
                यदि (अन्तः > सीमा) {
                    अन्तः = सीमा;
                }
                यदि (!enum_Ext_नामानि(start, अन्तः - 1, fn, context)) {
                    प्रत्यागम असत्य;
                }
            }
            गणः=अग्र_गणः_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        यावत्(गणः<गणः_सीमा && गणः[गणः_MSB]<अन्तः_गणः_MSB) {
            ध्रुव uint16_t *अग्र_गणः_2;
            start = static_cast<UChar32>(गणः[गणः_MSB]) << गणः_सरणम्;
            यदि(!enum_गणः_नामानि(नामानि, गणः, start, start+पङ्क्तयः_प्रत्येकम्_गणः-1, fn, context, nameChoice)) {
                प्रत्यागम असत्य;
            }
            अग्र_गणः_2=अग्र_गणः(गणः);
            यदि (अग्र_गणः_2 < गणः_सीमा && अग्र_गणः_2[गणः_MSB] > गणः[गणः_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 अन्तः = अग्र_गणः_2[गणः_MSB] << गणः_सरणम्;
                यदि (अन्तः > सीमा) {
                    अन्तः = सीमा;
                }
                यदि (!enum_Ext_नामानि((गणः[गणः_MSB] + 1) << गणः_सरणम्, अन्तः - 1, fn, context)) {
                    प्रत्यागम असत्य;
                }
            }
            गणः=अग्र_गणः_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        यदि(गणः<गणः_सीमा && गणः[गणः_MSB]==अन्तः_गणः_MSB) {
            प्रत्यागम enum_गणः_नामानि(नामानि, गणः, (सीमा-1)&~गणः_आवरणम्, सीमा-1, fn, context, nameChoice);
        } अन्यथा यदि (nameChoice == U_EXTENDED_CHAR_NAME && गणः == गणः_सीमा) {
            UChar32 next = (पूर्व_गणः(गणः)[गणः_MSB] + 1) << गणः_सरणम्;
            यदि (next > start) {
                start = next;
            }
        } अन्यथा {
            प्रत्यागम सत्य;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    यदि (nameChoice == U_EXTENDED_CHAR_NAME) {
        यदि (सीमा > UCHAR_MAX_मानम् + 1) {
            सीमा = UCHAR_MAX_मानम् + 1;
        }
        प्रत्यागम enum_Ext_नामानि(start, सीमा - 1, fn, context);
    }
    
    प्रत्यागम सत्य;
}

स्थिर uint16_t
write_गुणकः_Suffix(ध्रुव uint16_t *factors, uint16_t count,
                  ध्रुव वर्ण *s, /* suffix elements */
                  uint32_t संहिता,
                  uint16_t indexes[8], /* output fields from here */
                  ध्रुव वर्ण *elementBases[8], ध्रुव वर्ण *elements[8],
                  वर्ण *बफर, uint16_t बफर_दीर्घता) {
    uint16_t i, गुणकः, बफर_Pos=0;
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
        गुणकः=factors[i];
        indexes[i] = static_cast<uint16_t>(संहिता % गुणकः);
        संहिता/=गुणकः;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(संहिता);

    /* write each element */
    प्रति(;;) {
        यदि(elementBases!=शून्यसूचकः) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        गुणकः=indexes[i];
        यावत्(गुणकः>0) {
            यावत्(*s++!=0) {}
            --गुणकः;
        }
        यदि(elements!=शून्यसूचकः) {
            *elements++=s;
        }

        /* write element */
        यावत्((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        यदि(i>=count) {
            भङ्ग;
        }

        /* skip the rest of the strings for this factors[i] */
        गुणकः = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        यावत्(गुणकः>0) {
            यावत्(*s++!=0) {}
            --गुणकः;
        }

        ++i;
    }

    /* zero-terminate */
    यदि(बफर_दीर्घता>0) {
        *बफर=0;
    }

    प्रत्यागम बफर_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
स्थिर uint16_t
getAlgName(AlgorithmicRange *range, uint32_t संहिता, UCharNameChoice nameChoice,
        वर्ण *बफर, uint16_t बफर_दीर्घता) {
    uint16_t बफर_Pos=0;

    /* Only the normative character name can be algorithmic. */
    यदि(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        यदि(बफर_दीर्घता>0) {
            *बफर=0;
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
        यावत्((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        यदि(count<बफर_दीर्घता) {
            बफर[count]=0;
        }

        प्रति(i=count; i>0;) {
            यदि(--i<बफर_दीर्घता) {
                c = static_cast<वर्ण>(संहिता & 0xf);
                यदि(c<10) {
                    c+='0';
                } अन्यथा {
                    c+='A'-10;
                }
                बफर[i]=c;
            }
            संहिता>>=4;
        }

        बफर_Pos+=count;
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
        यावत्((c=*s++)!=0) {
            WRITE_CHAR(बफर, बफर_दीर्घता, बफर_Pos, c);
        }

        बफर_Pos+=write_गुणकः_Suffix(factors, count,
                                     s, संहिता-range->start, indexes, शून्यसूचकः, शून्यसूचकः, बफर, बफर_दीर्घता);
        भङ्ग;
    }
    स्वभाव:
        /* undefined type */
        /* zero-terminate */
        यदि(बफर_दीर्घता>0) {
            *बफर=0;
        }
        भङ्ग;
    }

    प्रत्यागम बफर_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
स्थिर UBool
enum_Alg_नामानि(AlgorithmicRange *range,
             UChar32 start, UChar32 सीमा,
             UEnum_Char_नामानि_Fn *fn, शून्य *context,
             UCharNameChoice nameChoice) {
    वर्ण बफर[200];
    uint16_t length;

    यदि(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        प्रत्यागम सत्य;
    }

    विकल्प(range->type) {
    पक्ष 0: {
        वर्ण *s, *अन्तः;
        वर्ण c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(start), nameChoice, बफर, परिमाण(बफर));
        यदि(length<=0) {
            प्रत्यागम सत्य;
        }

        /* call the enumerator function with this first character */
        यदि(!fn(context, start, nameChoice, बफर, length)) {
            प्रत्यागम असत्य;
        }

        /* go to the end of the name; all these names have the same length */
        अन्तः=बफर;
        यावत्(*अन्तः!=0) {
            ++अन्तः;
        }

        /* enumerate the rest of the names */
        यावत्(++start<सीमा) {
            /* increment the hexadecimal number on a character-basis */
            s=अन्तः;
            प्रति (;;) {
                c=*--s;
                यदि(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<वर्ण>(c + 1);
                    भङ्ग;
                } अन्यथा यदि(c=='9') {
                    *s='A';
                    भङ्ग;
                } अन्यथा यदि(c=='F') {
                    *s='0';
                }
            }

            यदि(!fn(context, start, nameChoice, बफर, length)) {
                प्रत्यागम असत्य;
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
        uint16_t उपसर्गः_दीर्घता, i, idx;

        वर्ण c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=बफर;
        उपसर्गः_दीर्घता=0;
        यावत्((c=*s++)!=0) {
            *suffix++=c;
            ++उपसर्गः_दीर्घता;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(उपसर्गः_दीर्घता + write_गुणकः_Suffix(factors, count,
                                              s, static_cast<uint32_t>(start) - range->start,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(परिमाण(बफर) - उपसर्गः_दीर्घता)));

        /* call the enumerator function with this first character */
        यदि(!fn(context, start, nameChoice, बफर, length)) {
            प्रत्यागम असत्य;
        }

        /* enumerate the rest of the names */
        यावत्(++start<सीमा) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            प्रति (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                यदि(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    यावत्(*s++!=0) {
                    }
                    elements[i]=s;
                    भङ्ग;
                } अन्यथा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=उपसर्गः_दीर्घता;
            प्रति(i=0; i<count; ++i) {
                s=elements[i];
                यावत्((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            यदि(!fn(context, start, nameChoice, बफर, length)) {
                प्रत्यागम असत्य;
            }
        }
        भङ्ग;
    }
    स्वभाव:
        /* undefined type */
        भङ्ग;
    }

    प्रत्यागम सत्य;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
स्थिर UChar32
findAlgName(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 संहिता;
    स्वचालित matcher = query.matcher();

    यदि(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        प्रत्यागम 0xffff;
    }

    विकल्प(range->type) {
    पक्ष 0: {
        /* name = prefix hex-digits */
        ध्रुव वर्ण* s = reinterpret_cast<ध्रुव वर्ण*>(range + 1);

        /* compare prefix */
        यदि (!matcher.consistentWith(s)) {
            प्रत्यागम 0xffff;
        }

        /* read hexadecimal code point value */
        संहिता=0;
        प्रति(वर्ण c : matcher.remainingSignificantCharacters()) {
            यदि('0'<=c && c<='9') {
                संहिता=(संहिता<<4)|(c-'0');
            } अन्यथा यदि('A'<=c && c<='F') {
                संहिता=(संहिता<<4)|(c-'A'+10);
            } अन्यथा {
                प्रत्यागम 0xffff;
            }
        }

        /* does it fit into the range? */
        यदि (range->start <= static_cast<uint32_t>(संहिता) && static_cast<uint32_t>(संहिता) <= range->अन्तः) {
            प्रत्यागम संहिता;
        }
        भङ्ग;
    }
    पक्ष 1: {
        वर्ण बफर[64];
        uint16_t indexes[8];
        ध्रुव वर्ण *elementBases[8], *elements[8];
        ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ध्रुव वर्ण *s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
        UChar32 start, सीमा;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view उपसर्गः = s;
        यदि (!matcher.consistentWith(उपसर्गः)) {
            प्रत्यागम 0xffff;
        }

        s += उपसर्गः.length() + 1;

        start = static_cast<UChar32>(range->start);
        सीमा = static_cast<UChar32>(range->अन्तः + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.remainingSignificantCharacters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_गुणकः_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, बफर, परिमाण(बफर));

        /* compare the first suffix */
        यदि(querySuffix == बफर) {
            प्रत्यागम start;
        }

        /* enumerate and compare the rest of the suffixes */
        यावत्(++start<सीमा) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            प्रति (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                यदि(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    यावत्(*s++!=0) {}
                    elements[i]=s;
                    भङ्ग;
                } अन्यथा {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            स्वचालित it = querySuffix.begin();
            प्रति(i=0; i<count; ++i) {
                s=elements[i];
                वर्ण c;
                यावत्((c=*s++)!=0) {
                    यदि(it == querySuffix.अन्तः() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            यदि(i<99 && it == querySuffix.अन्तः()) {
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

#परिभाषय समुच्चयः_सङ्कलय_3(समुच्चयः, c) ((समुच्चयः)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#परिभाषय समुच्चयः_अन्तर्भवति(समुच्चयः, c) (((समुच्चयः)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

स्थिर int32_t
calc_String_समुच्चयः_दीर्घता(uint32_t समुच्चयः[8], ध्रुव वर्ण *s) {
    int32_t length=0;
    वर्ण c;

    यावत्((c=*s++)!=0) {
        समुच्चयः_सङ्कलय_3(समुच्चयः, c);
        ++length;
    }
    प्रत्यागम length;
}

स्थिर int32_t
calcAlgNameSetsLengths(int32_t max_Name_दीर्घता) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_गणनम्;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_नामानि) + u_Char_नामानि->alg_नामानि_विस्थापनम्);
    range_गणनम्=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    यावत्(range_गणनम्>0) {
        विकल्प(range->type) {
        पक्ष 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*उपसर्गः*/
            length = calc_String_समुच्चयः_दीर्घता(g_Name_समुच्चयः, reinterpret_cast<ध्रुव वर्ण*>(range + 1)) + range->variant;
            यदि(length>max_Name_दीर्घता) {
                max_Name_दीर्घता=length;
            }
            भङ्ग;
        पक्ष 1: {
            /* name = prefix factorized-elements */
            ध्रुव uint16_t* factors = reinterpret_cast<ध्रुव uint16_t*>(range + 1);
            ध्रुव वर्ण *s;
            int32_t i, count=range->variant, गुणकः, गुणकः_दीर्घता, max_गुणकः_दीर्घता;

            /* prefix length */
            s = reinterpret_cast<ध्रुव वर्ण*>(factors + count);
            length=calc_String_समुच्चयः_दीर्घता(g_Name_समुच्चयः, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            प्रति(i=0; i<count; ++i) {
                max_गुणकः_दीर्घता=0;
                प्रति(गुणकः=factors[i]; गुणकः>0; --गुणकः) {
                    गुणकः_दीर्घता=calc_String_समुच्चयः_दीर्घता(g_Name_समुच्चयः, s);
                    s+=गुणकः_दीर्घता+1;
                    यदि(गुणकः_दीर्घता>max_गुणकः_दीर्घता) {
                        max_गुणकः_दीर्घता=गुणकः_दीर्घता;
                    }
                }
                length+=max_गुणकः_दीर्घता;
            }

            यदि(length>max_Name_दीर्घता) {
                max_Name_दीर्घता=length;
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
    प्रत्यागम max_Name_दीर्घता;
}

स्थिर int32_t
calcExtNameSetsLengths(int32_t max_Name_दीर्घता) {
    int32_t i, length;

    प्रति(i=0; i<UPRV_LENGTHOF(char_Cat_नामानि); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_समुच्चयः_दीर्घता(g_Name_समुच्चयः, char_Cat_नामानि[i]);
        यदि(length>max_Name_दीर्घता) {
            max_Name_दीर्घता=length;
        }
    }
    प्रत्यागम max_Name_दीर्घता;
}

स्थिर int32_t
calc_Name_समुच्चयः_दीर्घता(ध्रुव uint16_t *tokens, uint16_t token_गणनम्, ध्रुव uint8_t *token_वाक्यानि, int8_t *tokenLengths,
                  uint32_t समुच्चयः[8],
                  ध्रुव uint8_t **p_पङ्क्तिः, ध्रुव uint8_t *पङ्क्तिः_सीमा) {
    ध्रुव uint8_t *पङ्क्तिः=*p_पङ्क्तिः;
    int32_t length=0, token_दीर्घता;
    uint16_t c, token;

    यावत् (पङ्क्तिः != पङ्क्तिः_सीमा && (c = *पङ्क्तिः++) != static_cast<uint8_t>(';')) {
        यदि(c>=token_गणनम्) {
            /* implicit letter */
            समुच्चयः_सङ्कलय_3(समुच्चयः, c);
            ++length;
        } अन्यथा {
            token=tokens[c];
            यदि (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*पङ्क्तिः++;
                token=tokens[c];
            }
            यदि (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                समुच्चयः_सङ्कलय_3(समुच्चयः, c);
                ++length;
            } अन्यथा {
                /* count token word */
                यदि(tokenLengths!=शून्यसूचकः) {
                    /* use cached token length */
                    token_दीर्घता=tokenLengths[c];
                    यदि(token_दीर्घता==0) {
                        token_दीर्घता = calc_String_समुच्चयः_दीर्घता(समुच्चयः, reinterpret_cast<ध्रुव वर्ण*>(token_वाक्यानि) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_दीर्घता);
                    }
                } अन्यथा {
                    token_दीर्घता = calc_String_समुच्चयः_दीर्घता(समुच्चयः, reinterpret_cast<ध्रुव वर्ण*>(token_वाक्यानि) + token);
                }
                length+=token_दीर्घता;
            }
        }
    }

    *p_पङ्क्तिः=पङ्क्तिः;
    प्रत्यागम length;
}

स्थिर शून्य
calc_गणः_Name_Sets_Lengths(int32_t max_Name_दीर्घता) {
    uint16_t offsets[पङ्क्तयः_प्रत्येकम्_गणः+2], lengths[पङ्क्तयः_प्रत्येकम्_गणः+2];

    uint16_t* tokens = reinterpret_cast<uint16_t*>(u_Char_नामानि) + 8;
    uint16_t token_गणनम्=*tokens++;
    uint8_t* token_वाक्यानि = reinterpret_cast<uint8_t*>(u_Char_नामानि) + u_Char_नामानि->token_String_विस्थापनम्;

    int8_t *tokenLengths;

    ध्रुव uint16_t *गणः;
    ध्रुव uint8_t *s, *पङ्क्तिः, *पङ्क्तिः_सीमा;

    int32_t गणः_गणनम्, पङ्क्तिः_सङ्ख्या, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_गणनम्));
    यदि(tokenLengths!=शून्यसूचकः) {
        uprv_memset(tokenLengths, 0, token_गणनम्);
    }

    गणः=GET_GROUPS(u_Char_नामानि);
    गणः_गणनम्=*गणः++;

    /* enumerate all groups */
    यावत्(गणः_गणनम्>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_नामानि) + u_Char_नामानि->गणः_String_विस्थापनम् + GET_गणः_विस्थापनम्(गणः);
        s=expand_गणः_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        प्रति(पङ्क्तिः_सङ्ख्या=0; पङ्क्तिः_सङ्ख्या<पङ्क्तयः_प्रत्येकम्_गणः; ++पङ्क्तिः_सङ्ख्या) {
            पङ्क्तिः=s+offsets[पङ्क्तिः_सङ्ख्या];
            length=lengths[पङ्क्तिः_सङ्ख्या];
            यदि(length==0) {
                अनुवर्त;
            }

            पङ्क्तिः_सीमा=पङ्क्तिः+length;

            /* read regular name */
            length=calc_Name_समुच्चयः_दीर्घता(tokens, token_गणनम्, token_वाक्यानि, tokenLengths, g_Name_समुच्चयः, &पङ्क्तिः, पङ्क्तिः_सीमा);
            यदि(length>max_Name_दीर्घता) {
                max_Name_दीर्घता=length;
            }
            यदि(पङ्क्तिः==पङ्क्तिः_सीमा) {
                अनुवर्त;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_समुच्चयः_दीर्घता(tokens, token_गणनम्, token_वाक्यानि, tokenLengths, g_Name_समुच्चयः, &पङ्क्तिः, पङ्क्तिः_सीमा);
            यदि(length>max_Name_दीर्घता) {
                max_Name_दीर्घता=length;
            }
            यदि(पङ्क्तिः==पङ्क्तिः_सीमा) {
                अनुवर्त;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        गणः=अग्र_गणः(गणः);
        --गणः_गणनम्;
    }

    यदि(tokenLengths!=शून्यसूचकः) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_दीर्घता=max_Name_दीर्घता;
}

स्थिर UBool
calcNameSetsLengths(UError_संहिता *p_Error_संहिता) {
    स्थिर ध्रुव वर्ण extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_दीर्घता;

    यदि(g_Max_Name_दीर्घता!=0) {
        प्रत्यागम सत्य;
    }

    यदि(!is_दत्त_Loaded(p_Error_संहिता)) {
        प्रत्यागम असत्य;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    प्रति (i = 0; i < static_cast<int32_t>(परिमाण(extChars)) - 1; ++i) {
        समुच्चयः_सङ्कलय_3(g_Name_समुच्चयः, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_दीर्घता=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_दीर्घता=calcExtNameSetsLengths(max_Name_दीर्घता);

    /* set sets and lengths from group names, set global maximum values */
    calc_गणः_Name_Sets_Lengths(max_Name_दीर्घता);

    प्रत्यागम सत्य;
}

U_NAMESPACE_अन्तः

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 संहिता, UCharNameChoice nameChoice,
           वर्ण *बफर, int32_t बफर_दीर्घता,
           UError_संहिता *p_Error_संहिता) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    } अन्यथा यदि(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् ||
              बफर_दीर्घता<0 || (बफर_दीर्घता>0 && बफर==शून्यसूचकः)
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि((uint32_t)संहिता>UCHAR_MAX_मानम् || !is_दत्त_Loaded(p_Error_संहिता)) {
        प्रत्यागम u_terminateChars(बफर, बफर_दीर्घता, 0, p_Error_संहिता);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_नामानि+u_Char_नामानि->alg_नामानि_विस्थापनम्);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    यावत्(i>0) {
        यदि(algRange->start<=(uint32_t)संहिता && (uint32_t)संहिता<=algRange->अन्तः) {
            length=getAlgName(algRange, (uint32_t)संहिता, nameChoice, बफर, (uint16_t)बफर_दीर्घता);
            भङ्ग;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    यदि(i==0) {
        यदि (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_नामानि, (uint32_t )संहिता, U_EXTENDED_CHAR_NAME, बफर, (uint16_t) बफर_दीर्घता);
            यदि (!length) {
                /* extended character name */
                length = getExtName((uint32_t) संहिता, बफर, (uint16_t) बफर_दीर्घता);
            }
        } अन्यथा {
            /* normal character name */
            length=getName(u_Char_नामानि, (uint32_t)संहिता, nameChoice, बफर, (uint16_t)बफर_दीर्घता);
        }
    }

    प्रत्यागम u_terminateChars(बफर, बफर_दीर्घता, length, p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                वर्ण *dest, int32_t dest_क्षमता,
                UError_संहिता *p_Error_संहिता) {
    /* check the argument values */
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    } अन्यथा यदि(dest_क्षमता<0 || (dest_क्षमता>0 && dest==शून्यसूचकः)) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    प्रत्यागम u_terminateChars(dest, dest_क्षमता, 0, p_Error_संहिता);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ध्रुव वर्ण *ध्रुव name,
               UError_संहिता *p_Error_संहिता) {
    वर्ण अधस्तनम्[120] = {0};
    FindName findName;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    वर्ण c0;
    स्थिर constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम error;
    }

    यदि(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् || name==शून्यसूचकः || *name==0) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम error;
    }

    यदि(!is_दत्त_Loaded(p_Error_संहिता)) {
        प्रत्यागम error;
    }

    /* construct the lowercase of the name first */
    ध्रुव वर्ण* nameChar = name;
    प्रति(i=0; i<परिमाण(अधस्तनम्); ++i) {
        यदि((c0=*nameChar++)!=0) {
            अधस्तनम्[i]=uprv_tolower(c0);
        } अन्यथा {
            भङ्ग;
        }
    }
    यदि(i==परिमाण(अधस्तनम्)) {
        /* name too long, there is no such character */
        *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
        प्रत्यागम error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    यदि (अधस्तनम्[0] == '<') {
        यदि (nameChoice == U_EXTENDED_CHAR_NAME && अधस्तनम्[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t सीमा = i;
            यावत् (i >= 3 && अधस्तनम्[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_दीर्घता = सीमा - (i + 1);
            यदि (i >= 2 && अधस्तनम्[i] == '-' && 1 <= hex_दीर्घता && hex_दीर्घता <= 8) {
                uint32_t cIdx;

                अधस्तनम्[i] = 0;

                प्रति (++i; i < सीमा; ++i) {
                    यदि (अधस्तनम्[i] >= '0' && अधस्तनम्[i] <= '9') {
                        cp = (cp << 4) + अधस्तनम्[i] - '0';
                    } अन्यथा यदि (अधस्तनम्[i] >= 'a' && अधस्तनम्[i] <= 'f') {
                        cp = (cp << 4) + अधस्तनम्[i] - 'a' + 10;
                    } अन्यथा {
                        *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
                        प्रत्यागम error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    यदि (cp > UCHAR_MAX_मानम्) {
                        *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
                        प्रत्यागम error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                प्रति (अधस्तनम्[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_नामानि); ++cIdx) {

                    यदि (!uprv_strcmp(अधस्तनम् + 1, char_Cat_नामानि[cIdx])) {
                        यदि (cat == cIdx) {
                            प्रत्यागम cp;
                        }
                        भङ्ग;
                    }
                }
            }
        }

        *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
        प्रत्यागम error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_नामानि+u_Char_नामानि->alg_नामानि_विस्थापनम्);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    यावत्(i>0) {
        यदि((cp=findAlgName(algRange, nameChoice, query))!=0xffff) {
            प्रत्यागम cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    findName.otherName=&query;
    findName.संहिता=error;
    enum_नामानि(u_Char_नामानि, 0, UCHAR_MAX_मानम् + 1, DO_FIND_NAME, &findName, nameChoice);
    यदि (findName.संहिता == error) {
         *p_Error_संहिता = U_ILLEGAL_CHAR_प्राप्तम्;
    }
    प्रत्यागम findName.संहिता;
}

U_CAPI शून्य U_EXPORT2
u_enum_Char_नामानि(UChar32 start, UChar32 सीमा,
                UEnum_Char_नामानि_Fn *fn,
                शून्य *context,
                UCharNameChoice nameChoice,
                UError_संहिता *p_Error_संहिता) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    यदि(nameChoice>=U_CHAR_NAME_CHOICE_गणनम् || fn==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    यदि((uint32_t) सीमा > UCHAR_MAX_मानम् + 1) {
        सीमा = UCHAR_MAX_मानम् + 1;
    }
    यदि((uint32_t)start>=(uint32_t)सीमा) {
        प्रत्यागम;
    }

    यदि(!is_दत्त_Loaded(p_Error_संहिता)) {
        प्रत्यागम;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_नामानि+u_Char_नामानि->alg_नामानि_विस्थापनम्);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    यावत्(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        यदि((uint32_t)start<algRange->start) {
            यदि((uint32_t)सीमा<=algRange->start) {
                enum_नामानि(u_Char_नामानि, start, सीमा, fn, context, nameChoice);
                प्रत्यागम;
            }
            यदि(!enum_नामानि(u_Char_नामानि, start, (UChar32)algRange->start, fn, context, nameChoice)) {
                प्रत्यागम;
            }
            start=(UChar32)algRange->start;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        यदि((uint32_t)start<=algRange->अन्तः) {
            यदि((uint32_t)सीमा<=(algRange->अन्तः+1)) {
                enum_Alg_नामानि(algRange, start, सीमा, fn, context, nameChoice);
                प्रत्यागम;
            }
            यदि(!enum_Alg_नामानि(algRange, start, (UChar32)algRange->अन्तः+1, fn, context, nameChoice)) {
                प्रत्यागम;
            }
            start=(UChar32)algRange->अन्तः+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_नामानि(u_Char_नामानि, start, सीमा, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_दीर्घता() {
    UError_संहिता error_संहिता=U_ZERO_ERROR;
    यदि(calcNameSetsLengths(&error_संहिता)) {
        प्रत्यागम g_Max_Name_दीर्घता;
    } अन्यथा {
        प्रत्यागम 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
स्थिर शून्य
char_समुच्चयः_गन्तव्यम्_USet(uint32_t cset[8], ध्रुव USetAdder *sa) {
    char16_t us[256];
    वर्ण cs[256];

    int32_t i, length;
    UError_संहिता error_संहिता;

    error_संहिता=U_ZERO_ERROR;

    यदि(!calcNameSetsLengths(&error_संहिता)) {
        प्रत्यागम;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    प्रति(i=0; i<256; ++i) {
        यदि(समुच्चयः_अन्तर्भवति(cset, i)) {
            cs[length++] = static_cast<वर्ण>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_गन्तव्यम्_UChars(cs, us, length);

    /* add each char16_t to the USet */
    प्रति(i=0; i<length; ++i) {
        यदि(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->सङ्कलय(sa->समुच्चयः, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI शून्य U_EXPORT2
uprv_getCharNameCharacters(ध्रुव USetAdder *sa) {
    char_समुच्चयः_गन्तव्यम्_USet(g_Name_समुच्चयः, sa);
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
             UError_संहिता *p_Error_संहिता) {
    UBool प्रयुक्तम्_बहिर्गमनम्_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    यदि(ds->अन्तर्_Charset==ds->बहिर्गमनम्_Charset) {
        /* Same charset family: identity permutation */
        प्रति(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } अन्यथा {
        uprv_memset(map, 0, 256);
        uprv_memset(प्रयुक्तम्_बहिर्गमनम्_Char, 0, 256);

        यदि(token_गणनम्>256) {
            token_गणनम्=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        प्रति(i=1; i<token_गणनम्; ++i) {
            यदि(tokens[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->swapInvChars(ds, &c1, 1, &c2, p_Error_संहिता);
                यदि(U_FAILURE(*p_Error_संहिता)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->अन्तर्_Charset);
                    प्रत्यागम;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                प्रयुक्तम्_बहिर्गमनम्_Char[c2]=सत्य;
            }
        }

        /* set the mappings for the rest of the permutation */
        प्रति(i=j=1; i<token_गणनम्; ++i) {
            /* set mappings that were not set for direct bytes */
            यदि(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                यावत्(प्रयुक्तम्_बहिर्गमनम्_Char[j]) {
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
uchar_swap_नामानि(ध्रुव UDataSwapper *ds,
                ध्रुव शून्य *अन्तर्_दत्त, int32_t length, शून्य *बहिर्गमनम्_दत्त,
                UError_संहिता *p_Error_संहिता) {
    ध्रुव UDataInfo *pInfo;
    int32_t शीर्षकम्_आकार;

    ध्रुव uint8_t *अन्तर्_अष्टकानि;
    uint8_t *बहिर्गमनम्_अष्टकानि;

    uint32_t token_String_विस्थापनम्, groups_विस्थापनम्, गणः_String_विस्थापनम्, alg_नामानि_विस्थापनम्,
             विस्थापनम्, i, count, वाक्यानि_गणनम्;

    ध्रुव AlgorithmicRange *अन्तर्_Range;
    AlgorithmicRange *बहिर्गमनम्_Range;

    /* udata_swapDataHeader checks the arguments */
    शीर्षकम्_आकार=udata_swap_दत्त_शीर्षकम्(ds, अन्तर्_दत्त, length, बहिर्गमनम्_दत्त, p_Error_संहिता);
    यदि(p_Error_संहिता==शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }

    /* check data format and format version */
    pInfo=(ध्रुव UDataInfo *)((ध्रुव वर्ण *)अन्तर्_दत्त+4);
    यदि(!(
        pInfo->दत्त_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->दत्त_Format[1]==0x6e &&
        pInfo->दत्त_Format[2]==0x61 &&
        pInfo->दत्त_Format[3]==0x6d &&
        pInfo->format_संस्करणम्[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->दत्त_Format[0], pInfo->दत्त_Format[1],
                         pInfo->दत्त_Format[2], pInfo->दत्त_Format[3],
                         pInfo->format_संस्करणम्[0]);
        *p_Error_संहिता=U_UNSUPPORTED_ERROR;
        प्रत्यागम 0;
    }

    अन्तर्_अष्टकानि=(ध्रुव uint8_t *)अन्तर्_दत्त+शीर्षकम्_आकार;
    बहिर्गमनम्_अष्टकानि=(बहिर्गमनम्_दत्त == शून्यसूचकः) ? शून्यसूचकः : (uint8_t *)बहिर्गमनम्_दत्त+शीर्षकम्_आकार;
    यदि(length<0) {
        alg_नामानि_विस्थापनम्=ds->readUInt32(((ध्रुव uint32_t *)अन्तर्_अष्टकानि)[3]);
    } अन्यथा {
        length-=शीर्षकम्_आकार;
        यदि( length<20 ||
            (uint32_t)length<(alg_नामानि_विस्थापनम्=ds->readUInt32(((ध्रुव uint32_t *)अन्तर्_अष्टकानि)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
            प्रत्यागम 0;
        }
    }

    यदि(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        विस्थापनम्=alg_नामानि_विस्थापनम्;
        count=ds->readUInt32(*((ध्रुव uint32_t *)(अन्तर्_अष्टकानि+विस्थापनम्)));
        विस्थापनम्+=4;

        प्रति(i=0; i<count; ++i) {
            अन्तर्_Range=(ध्रुव AlgorithmicRange *)(अन्तर्_अष्टकानि+विस्थापनम्);
            विस्थापनम्+=ds->readUInt16(अन्तर्_Range->size);
        }
    } अन्यथा {
        /* swap data */
        ध्रुव uint16_t *p;
        uint16_t *q, *अस्थायी;

        int16_t tokens[512];
        uint16_t token_गणनम्;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        यदि(अन्तर्_अष्टकानि!=बहिर्गमनम्_अष्टकानि) {
            uprv_memcpy(बहिर्गमनम्_अष्टकानि, अन्तर्_अष्टकानि, length);
        }

        /* the initial 4 offsets first */
        token_String_विस्थापनम्=ds->readUInt32(((ध्रुव uint32_t *)अन्तर्_अष्टकानि)[0]);
        groups_विस्थापनम्=ds->readUInt32(((ध्रुव uint32_t *)अन्तर्_अष्टकानि)[1]);
        गणः_String_विस्थापनम्=ds->readUInt32(((ध्रुव uint32_t *)अन्तर्_अष्टकानि)[2]);
        ds->swapArray32(ds, अन्तर्_अष्टकानि, 16, बहिर्गमनम्_अष्टकानि, p_Error_संहिता);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ध्रुव uint16_t *)(अन्तर्_अष्टकानि+16);
        q=(uint16_t *)(बहिर्गमनम्_अष्टकानि+16);

        /* read and swap the tokenCount */
        token_गणनम्=ds->readUInt16(*p);
        ds->swapArray16(ds, p, 2, q, p_Error_संहिता);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        यदि(token_गणनम्<=512) {
            count=token_गणनम्;
        } अन्यथा {
            count=512;
        }
        प्रति(i=0; i<count; ++i) {
            tokens[i]=udata_readInt16(ds, p[i]);
        }
        प्रति(; i<512; ++i) {
            tokens[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, tokens, token_गणनम्, map, p_Error_संहिता);
        makeTokenMap(ds, tokens+256, (uint16_t)(token_गणनम्>256 ? token_गणनम्-256 : 0), trailMap, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        अस्थायी=(uint16_t *)uprv_malloc(token_गणनम्*2);
        यदि(अस्थायी==शून्यसूचकः) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_गणनम्);
            *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        प्रति(i=0; i<token_गणनम् && i<256; ++i) {
            ds->swapArray16(ds, p+i, 2, अस्थायी+map[i], p_Error_संहिता);
        }

        /* swap and permutate trail-byte tokens */
        प्रति(; i<token_गणनम्; ++i) {
            ds->swapArray16(ds, p+i, 2, अस्थायी+(i&0xffffff00)+trailMap[i&0xff], p_Error_संहिता);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, अस्थायी, token_गणनम्*2);
        uprv_free(अस्थायी);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_swap_Inv_String_खण्डम्(ds, अन्तर्_अष्टकानि+token_String_विस्थापनम्, (int32_t)(groups_विस्थापनम्-token_String_विस्थापनम्),
                                    बहिर्गमनम्_अष्टकानि+token_String_विस्थापनम्, p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            प्रत्यागम 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ध्रुव uint16_t *)(अन्तर्_अष्टकानि+groups_विस्थापनम्)));
        ds->swapArray16(ds, अन्तर्_अष्टकानि+groups_विस्थापनम्, (int32_t)((1+count*3)*2),
                           बहिर्गमनम्_अष्टकानि+groups_विस्थापनम्, p_Error_संहिता);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        यदि(ds->अन्तर्_Charset!=ds->बहिर्गमनम्_Charset) {
            uint16_t offsets[पङ्क्तयः_प्रत्येकम्_गणः+1], lengths[पङ्क्तयः_प्रत्येकम्_गणः+1];

            ध्रुव uint8_t *अन्तर्_वाक्यानि, *अग्र_अन्तर्_वाक्यानि;
            uint8_t *बहिर्गमनम्_वाक्यानि;

            uint8_t c;

            अन्तर्_वाक्यानि=अन्तर्_अष्टकानि+गणः_String_विस्थापनम्;
            बहिर्गमनम्_वाक्यानि=बहिर्गमनम्_अष्टकानि+गणः_String_विस्थापनम्;

            वाक्यानि_गणनम्=alg_नामानि_विस्थापनम्-गणः_String_विस्थापनम्;

            /* iterate through string groups until only a few padding bytes are left */
            यावत्(वाक्यानि_गणनम्>32) {
                अग्र_अन्तर्_वाक्यानि=expand_गणः_Lengths(अन्तर्_वाक्यानि, offsets, lengths);

                /* move past the length bytes */
                वाक्यानि_गणनम्-=(uint32_t)(अग्र_अन्तर्_वाक्यानि-अन्तर्_वाक्यानि);
                बहिर्गमनम्_वाक्यानि+=अग्र_अन्तर्_वाक्यानि-अन्तर्_वाक्यानि;
                अन्तर्_वाक्यानि=अग्र_अन्तर्_वाक्यानि;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                वाक्यानि_गणनम्-=count;

                /* swap the string bytes using map[] and trailMap[] */
                यावत्(count>0) {
                    c=*अन्तर्_वाक्यानि++;
                    *बहिर्गमनम्_वाक्यानि++=map[c];
                    यदि(tokens[c]!=-2) {
                        --count;
                    } अन्यथा {
                        /* token lead byte: swap the trail byte, too */
                        *बहिर्गमनम्_वाक्यानि++=trailMap[*अन्तर्_वाक्यानि++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        विस्थापनम्=alg_नामानि_विस्थापनम्;
        count=ds->readUInt32(*((ध्रुव uint32_t *)(अन्तर्_अष्टकानि+विस्थापनम्)));
        ds->swapArray32(ds, अन्तर्_अष्टकानि+विस्थापनम्, 4, बहिर्गमनम्_अष्टकानि+विस्थापनम्, p_Error_संहिता);
        विस्थापनम्+=4;

        प्रति(i=0; i<count; ++i) {
            यदि(विस्थापनम्>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_संहिता=U_सूचकांक_OUTOFBOUNDS_ERROR;
                प्रत्यागम 0;
            }

            अन्तर्_Range=(ध्रुव AlgorithmicRange *)(अन्तर्_अष्टकानि+विस्थापनम्);
            बहिर्गमनम्_Range=(AlgorithmicRange *)(बहिर्गमनम्_अष्टकानि+विस्थापनम्);
            विस्थापनम्+=ds->readUInt16(अन्तर्_Range->size);

            ds->swapArray32(ds, अन्तर्_Range, 8, बहिर्गमनम्_Range, p_Error_संहिता);
            ds->swapArray16(ds, &अन्तर्_Range->size, 2, &बहिर्गमनम्_Range->size, p_Error_संहिता);
            विकल्प(अन्तर्_Range->type) {
            पक्ष 0:
                /* swap prefix string */
                ds->swapInvChars(ds, अन्तर्_Range+1, (int32_t)uprv_strlen((ध्रुव वर्ण *)(अन्तर्_Range+1)),
                                    बहिर्गमनम्_Range+1, p_Error_संहिता);
                यदि(U_FAILURE(*p_Error_संहिता)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    प्रत्यागम 0;
                }
                भङ्ग;
            पक्ष 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_गणनम्;

                    factors_गणनम्=अन्तर्_Range->variant;
                    p=(ध्रुव uint16_t *)(अन्तर्_Range+1);
                    q=(uint16_t *)(बहिर्गमनम्_Range+1);
                    ds->swapArray16(ds, p, (int32_t)(factors_गणनम्*2), q, p_Error_संहिता);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_गणनम्;
                    q+=factors_गणनम्;
                    वाक्यानि_गणनम्=(uint32_t)((अन्तर्_अष्टकानि+विस्थापनम्)-(ध्रुव uint8_t *)p);
                    यावत्(वाक्यानि_गणनम्>0 && ((ध्रुव uint8_t *)p)[वाक्यानि_गणनम्-1]!=0) {
                        --वाक्यानि_गणनम्;
                    }
                    ds->swapInvChars(ds, p, (int32_t)वाक्यानि_गणनम्, q, p_Error_संहिता);
                }
                भङ्ग;
            स्वभाव:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 अन्तर्_Range->type, i);
                *p_Error_संहिता=U_UNSUPPORTED_ERROR;
                प्रत्यागम 0;
            }
        }
    }

    प्रत्यागम शीर्षकम्_आकार+(int32_t)विस्थापनम्;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
