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

#అన్తర్భావ "unicode/utypes.h"
#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "unicode/uchar.h"
#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/utf.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ustr_imp.h"
#అన్తర్భావ "umutex.h"
#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "ucln_cmn.h"
#అన్తర్భావ "udataswp.h"
#అన్తర్భావ "uprops.h"

U_NAMESPACE_BEGIN

/* prototypes ------------------------------------------------------------- */

స్థిర స్థిరరాశి అక్షరం దత్తాంశం_NAME[] = "unames";
స్థిర స్థిరరాశి అక్షరం దత్తాంశం_TYPE[] = "icu";

#నిర్వచించు సమూహం_జరుపు 5
#నిర్వచించు పంక్తులు_చొప్పున_సమూహం (1L<<సమూహం_జరుపు)
#నిర్వచించు సమూహం_ముసుగు (పంక్తులు_చొప్పున_సమూహం-1)

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
గణన {
    సమూహం_MSB,
    సమూహం_స్థానభ్రంశం_ఎగువ,
    సమూహం_స్థానభ్రంశం_దిగువ,
    సమూహం_నిడివి
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#నిర్వచించు GET_సమూహం_స్థానభ్రంశం(సమూహం) ((int32_t)(సమూహం)[సమూహం_స్థానభ్రంశం_ఎగువ]<<16|(సమూహం)[సమూహం_స్థానభ్రంశం_దిగువ])

#నిర్వచించు తదుపరి_సమూహం(సమూహం) ((సమూహం)+సమూహం_నిడివి)
#నిర్వచించు PREV_సమూహం(సమూహం) ((సమూహం)-సమూహం_నిడివి)

రకనిర్వచనం నిర్మాణం {
    uint32_t ప్రారంభం, చివర;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

రకనిర్వచనం నిర్మాణం {
    uint32_t token_String_స్థానభ్రంశం, groups_స్థానభ్రంశం, సమూహం_String_స్థానభ్రంశం, alg_పేర్లు_స్థానభ్రంశం;
} UChar_పేర్లు;

నామాకాశ {
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
తరగతి CharacterNameQuery {
  బహిరంగ:
    తరగతి Matcher {
      బహిరంగ:
        // `query` must outlive the constructed object.
        Matcher(స్థిరరాశి CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        తార్కిక consistentWith(అక్షరం c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            ఒకవేళ (skeletonIterator == query.skeleton.చివర()) {
                తిరిగి అబద్ధం;
            }
            ఒకవేళ (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                ఒకవేళ (చివరి_Char == '-') {
                    ఒకవేళ (*skeletonIterator++ != చివరి_Char) {
                        తిరిగి అబద్ధం;
                    }
                }
                చివరి_Char = c;
                తిరిగి నిజం;
            } లేకపోతే ఒకవేళ (c == '-') {
                ఒకవేళ (చివరి_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.చివర() - 2)) {
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
                    ఒకవేళ (*skeletonIterator++ != c) {
                        తిరిగి అబద్ధం;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                చివరి_Char = c;
                తిరిగి నిజం;
            } లేకపోతే {
                ఒకవేళ (*skeletonIterator++ != c) {
                    తిరిగి అబద్ధం;
                }
                చివరి_Char = c;
                తిరిగి నిజం;
            }
        }

        తార్కిక consistentWith(స్థిరరాశి std::string_view substring) {
            కొరకు (స్థిరరాశి అక్షరం c : substring) {
                ఒకవేళ (!consistentWith(c)) {
                    తిరిగి అబద్ధం;
                }
            }
            తిరిగి నిజం;
        }

        తార్కిక matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(చివరి_Char != '-');
            తిరిగి skeletonIterator == query.skeleton.చివర();
        }

        std::string_view మిగిలిన_Significant_Characters() స్థిరరాశి {
            తిరిగి query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      అంతరంగ:
        స్థిరరాశి CharacterNameQuery& query;
        std::string_view::const_iterator skeletonIterator;
        // Initialized to ' ' so that a leading hyphen is treated like a hyphen that follows a
        // space (non-medial and thus not ignorable).  There are in fact no leading hyphens in
        // character names by rule R3 in
        // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, and technically
        // we do not read the value of this variable before writing to it because the first call to
        // consistentWith never has c=' ' either by R4, but it seems cleanest to initialize lastChar
        // nonetheless.
        అక్షరం చివరి_Char = ' ';
    };

    స్పష్టమైన CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        అక్షరం *skeleton_హద్దు = skeleton_దత్తాంశం;
        కొరకు (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_హద్దు < skeleton_దత్తాంశం + పరిమాణం(skeleton_దత్తాంశం));
            ఒకవేళ (skeleton_హద్దు >= skeleton_దత్తాంశం + పరిమాణం(skeleton_దత్తాంశం)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_హద్దు = skeleton_దత్తాంశం;
                ఆపు;
            }
            ఒకవేళ (query[i] == ' ' || query[i] == '_') {
                కొనసాగు;
            }
            ఒకవేళ (query[i] == '-') {
                తార్కిక isMedial;
                తార్కిక is1180MedialHyphen = అబద్ధం;
                ఒకవేళ (i == 0 || i == query.length() - 1) {
                    isMedial = అబద్ధం;
                } లేకపోతే {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                ఒకవేళ (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_దత్తాంశం, skeleton_హద్దు - skeleton_దత్తాంశం) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = నిజం;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    కొరకు (std::size_t j = i + 2; j < query.length(); ++j) {
                        ఒకవేళ (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = అబద్ధం;
                        }
                    }
                }
                ఒకవేళ (!isMedial || is1180MedialHyphen) {
                    *skeleton_హద్దు++ = query[i];
                }
            } లేకపోతే {
                *skeleton_హద్దు++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_దత్తాంశం, skeleton_హద్దు - skeleton_దత్తాంశం);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() స్థిరరాశి {
        తిరిగి Matcher(*ఇది);
    }

అంతరంగ:
    స్థిర తార్కిక isASCIILetterOrDigit(స్థిరరాశి అక్షరం c) {
        తిరిగి uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    అక్షరం skeleton_దత్తాంశం[120];
    std::string_view skeleton;
    తార్కిక is1180;
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
#నిర్వచించు GET_GROUPS(పేర్లు) (స్థిరరాశి uint16_t *)((స్థిరరాశి అక్షరం *)పేర్లు+పేర్లు->groups_స్థానభ్రంశం)

రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి CharacterNameQuery *otherName;
    UChar32 సంకేతం_2;
} కనుగొను_Name;

#నిర్వచించు DO_కనుగొను_NAME శూన్యసూచిక

స్థిర UData_జ్ఞాపకం *u_Char_పేర్లు_దత్తాంశం=శూన్యసూచిక;
స్థిర UChar_పేర్లు *u_Char_పేర్లు=శూన్యసూచిక;
స్థిర icu::UInitOnce g_Char_పేర్లు_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
స్థిర int32_t g_Max_Name_నిడివి=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
స్థిర uint32_t g_Name_సమితి[8]={ 0 };

#నిర్వచించు U_NONCHARACTER_సంకేతం_బిందువు U_CHAR_CATEGORY_లెక్క
#నిర్వచించు U_LEAD_SURROGATE U_CHAR_CATEGORY_లెక్క + 1
#నిర్వచించు U_TRAIL_SURROGATE U_CHAR_CATEGORY_లెక్క + 2

#నిర్వచించు U_CHAR_EXTENDED_CATEGORY_లెక్క (U_CHAR_CATEGORY_లెక్క + 3)

స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి char_Cat_పేర్లు[U_CHAR_EXTENDED_CATEGORY_లెక్క] = {
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

స్థిర UBool U_CALLCONV unames_cleanup()
{
    ఒకవేళ(u_Char_పేర్లు_దత్తాంశం) {
        udata_close(u_Char_పేర్లు_దత్తాంశం);
        u_Char_పేర్లు_దత్తాంశం = శూన్యసూచిక;
    }
    ఒకవేళ(u_Char_పేర్లు) {
        u_Char_పేర్లు = శూన్యసూచిక;
    }
    g_Char_పేర్లు_Init_Once.reset();
    g_Max_Name_నిడివి=0;
    తిరిగి నిజం;
}

స్థిర UBool U_CALLCONV
isAcceptable(శూన్య * /*context*/,
             స్థిరరాశి అక్షరం * /*type*/, స్థిరరాశి అక్షరం * /*name*/,
             స్థిరరాశి UDataInfo *pInfo) {
    తిరిగి
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->దత్తాంశం_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->దత్తాంశం_Format[1]==0x6e &&
        pInfo->దత్తాంశం_Format[2]==0x61 &&
        pInfo->దత్తాంశం_Format[3]==0x6d &&
        pInfo->format_సంస్కరణ[0]==1;
}

స్థిర శూన్య U_CALLCONV
ఎక్కించు_Char_పేర్లు(UError_సంకేతం &స్థితి_2) {
    U_ASSERT(u_Char_పేర్లు_దత్తాంశం == శూన్యసూచిక);
    U_ASSERT(u_Char_పేర్లు == శూన్యసూచిక);

    u_Char_పేర్లు_దత్తాంశం = udata_openChoice(శూన్యసూచిక, దత్తాంశం_TYPE, దత్తాంశం_NAME, isAcceptable, శూన్యసూచిక, &స్థితి_2);
    ఒకవేళ(U_FAILURE(స్థితి_2)) {
        u_Char_పేర్లు_దత్తాంశం = శూన్యసూచిక;
    } లేకపోతే {
        u_Char_పేర్లు = (UChar_పేర్లు *)udata_get_జ్ఞాపకం(u_Char_పేర్లు_దత్తాంశం);
    }
    ucln_common_రిజిస్టరు_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


స్థిర UBool
is_దత్తాంశం_Loaded(UError_సంకేతం *p_Error_సంకేతం) {
    umtx_initOnce(g_Char_పేర్లు_Init_Once, &ఎక్కించు_Char_పేర్లు, *p_Error_సంకేతం);
    తిరిగి U_SUCCESS(*p_Error_సంకేతం);
}

#నిర్వచించు WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c) UPRV_ఖండం_MACRO_BEGIN { \
    ఒకవేళ((బఫరు_నిడివి)>0) { \
        *(బఫరు)++=c; \
        --(బఫరు_నిడివి); \
    } \
    ++(బఫరు_Pos); \
} UPRV_ఖండం_MACRO_చివర

#నిర్వచించు U_ISO_COMMENT U_CHAR_NAME_CHOICE_లెక్క

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
స్థిర uint16_t
expandName(UChar_పేర్లు *పేర్లు,
           స్థిరరాశి uint8_t *name, uint16_t name_నిడివి, UCharNameChoice nameChoice,
           అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    uint16_t* టోకెన్లు = reinterpret_cast<uint16_t*>(పేర్లు) + 8;
    uint16_t token, token_లెక్క=*టోకెన్లు++, బఫరు_Pos=0;
    uint8_t* token_అక్షరశ్రేణులు = reinterpret_cast<uint8_t*>(పేర్లు) + పేర్లు->token_String_స్థానభ్రంశం;
    uint8_t c;

    ఒకవేళ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ఒకవేళ (static_cast<uint8_t>(';') >= token_లెక్క || టోకెన్లు[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            పూర్ణాంకం క్షేత్రం_సూచీ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            చేయి {
                అయితే(name_నిడివి>0) {
                    --name_నిడివి;
                    ఒకవేళ(*name++==';') {
                        ఆపు;
                    }
                }
            } అయితే(--క్షేత్రం_సూచీ>0);
        } లేకపోతే {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_నిడివి=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    అయితే(name_నిడివి>0) {
        --name_నిడివి;
        c=*name++;

        ఒకవేళ(c>=token_లెక్క) {
            ఒకవేళ(c!=';') {
                /* implicit letter */
                WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
            } లేకపోతే {
                /*ముగిసిన*/
                ఆపు;
            }
        } లేకపోతే {
            token=టోకెన్లు[c];
            ఒకవేళ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=టోకెన్లు[c<<8|*name++];
                --name_నిడివి;
            }
            ఒకవేళ (token == static_cast<uint16_t>(-1)) {
                ఒకవేళ(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
                } లేకపోతే {
                    /*ముగిసిన*/
                    ఆపు;
                }
            } లేకపోతే {
                /* write token word */
                uint8_t *tokenString=token_అక్షరశ్రేణులు+token;
                అయితే((c=*tokenString++)!=0) {
                    WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    ఒకవేళ(బఫరు_నిడివి>0) {
        *బఫరు=0;
    }

    తిరిగి బఫరు_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
స్థిర UBool
compareName(UChar_పేర్లు *పేర్లు,
            స్థిరరాశి uint8_t *name, uint16_t name_నిడివి, UCharNameChoice nameChoice,
            స్థిరరాశి CharacterNameQuery& otherName) {
    uint16_t* టోకెన్లు = reinterpret_cast<uint16_t*>(పేర్లు) + 8;
    uint16_t token, token_లెక్క=*టోకెన్లు++;
    uint8_t* token_అక్షరశ్రేణులు = reinterpret_cast<uint8_t*>(పేర్లు) + పేర్లు->token_String_స్థానభ్రంశం;
    uint8_t c;
    స్వయంచాలక matcher = otherName.matcher();

    ఒకవేళ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        ఒకవేళ (static_cast<uint8_t>(';') >= token_లెక్క || టోకెన్లు[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            పూర్ణాంకం క్షేత్రం_సూచీ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            చేయి {
                అయితే(name_నిడివి>0) {
                    --name_నిడివి;
                    ఒకవేళ(*name++==';') {
                        ఆపు;
                    }
                }
            } అయితే(--క్షేత్రం_సూచీ>0);
        } లేకపోతే {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_నిడివి=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    అయితే(name_నిడివి>0) {
        --name_నిడివి;
        c=*name++;

        ఒకవేళ(c>=token_లెక్క) {
            ఒకవేళ(c!=';') {
                /* implicit letter */
                ఒకవేళ (!matcher.consistentWith(c)) {
                    తిరిగి అబద్ధం;
                }
            } లేకపోతే {
                /*ముగిసిన*/
                ఆపు;
            }
        } లేకపోతే {
            token=టోకెన్లు[c];
            ఒకవేళ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=టోకెన్లు[c<<8|*name++];
                --name_నిడివి;
            }
            ఒకవేళ (token == static_cast<uint16_t>(-1)) {
                ఒకవేళ(c!=';') {
                    /* explicit letter */
                    ఒకవేళ (!matcher.consistentWith(c)) {
                        తిరిగి అబద్ధం;
                    }
                } లేకపోతే {
                    /*ముగిసిన*/
                    ఆపు;
                }
            } లేకపోతే {
                /* write token word */
                ఒకవేళ (!matcher.consistentWith(reinterpret_cast<స్థిరరాశి అక్షరం *>(token_అక్షరశ్రేణులు + token))) {
                    తిరిగి అబద్ధం;
                }
            }
        }
    }

    /* complete match? */
    తిరిగి matcher.matches();
}

స్థిర uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    ఒకవేళ (U_IS_UNICODE_NONCHAR(cp)) {
        తిరిగి U_NONCHARACTER_సంకేతం_బిందువు;
    }

    ఒకవేళ ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    తిరిగి cat;
}

స్థిర స్థిరరాశి అక్షరం *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    ఒకవేళ (cat >= UPRV_LENGTHOF(char_Cat_పేర్లు)) {
        తిరిగి "unknown";
    } లేకపోతే {
        తిరిగి char_Cat_పేర్లు[cat];
    }
}

స్థిర uint16_t getExtName(uint32_t సంకేతం_2, అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    స్థిరరాశి అక్షరం *catname = getCharCatName(సంకేతం_2);
    uint16_t length = 0;

    UChar32 cp;
    పూర్ణాంకం ndigits, i;
    
    WRITE_CHAR(బఫరు, బఫరు_నిడివి, length, '<');
    అయితే (catname[length - 1]) {
        WRITE_CHAR(బఫరు, బఫరు_నిడివి, length, catname[length - 1]);
    }
    WRITE_CHAR(బఫరు, బఫరు_నిడివి, length, '-');
    కొరకు (cp = సంకేతం_2, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    ఒకవేళ (ndigits < 4)
        ndigits = 4;
    కొరకు (cp = సంకేతం_2, i = ndigits; (cp || i > 0) && బఫరు_నిడివి; cp >>= 4, బఫరు_నిడివి--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        బఫరు[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    బఫరు += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(బఫరు, బఫరు_నిడివి, length, '>');

    తిరిగి length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
స్థిర స్థిరరాశి uint16_t *
get_సమూహం(UChar_పేర్లు *పేర్లు, uint32_t సంకేతం_2) {
    స్థిరరాశి uint16_t *groups=GET_GROUPS(పేర్లు);
    uint16_t సమూహం_MSB_2 = static_cast<uint16_t>(సంకేతం_2 >> సమూహం_జరుపు),
             ప్రారంభం=0,
             హద్దు=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    అయితే(ప్రారంభం<హద్దు-1) {
        number = static_cast<uint16_t>((ప్రారంభం + హద్దు) / 2);
        ఒకవేళ(సమూహం_MSB_2<groups[number*సమూహం_నిడివి+సమూహం_MSB]) {
            హద్దు=number;
        } లేకపోతే {
            ప్రారంభం=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    తిరిగి groups+ప్రారంభం*సమూహం_నిడివి;
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
స్థిర స్థిరరాశి uint8_t *
expand_సమూహం_Lengths(స్థిరరాశి uint8_t *s,
                   uint16_t offsets[పంక్తులు_చొప్పున_సమూహం+1], uint16_t lengths[పంక్తులు_చొప్పున_సమూహం+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, స్థానభ్రంశం=0, length=0;
    uint8_t నిడివి_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    అయితే(i<పంక్తులు_చొప్పున_సమూహం) {
        నిడివి_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        ఒకవేళ(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | నిడివి_Byte >> 4) + 12);
            నిడివి_Byte&=0xf;
        } లేకపోతే ఒకవేళ((నిడివి_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((నిడివి_Byte & 0x3f) + 12);
        } లేకపోతే {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(నిడివి_Byte >> 4);
            నిడివి_Byte&=0xf;
        }

        *offsets++=స్థానభ్రంశం;
        *lengths++=length;

        స్థానభ్రంశం+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        ఒకవేళ((నిడివి_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=నిడివి_Byte;
            ఒకవేళ(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=స్థానభ్రంశం;
                *lengths++=length;

                స్థానభ్రంశం+=length;
                ++i;
            }
        } లేకపోతే {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    తిరిగి s;
}

స్థిర uint16_t
expand_సమూహం_Name(UChar_పేర్లు *పేర్లు, స్థిరరాశి uint16_t *సమూహం,
                uint16_t పంక్తి_సంఖ్య, UCharNameChoice nameChoice,
                అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    uint16_t offsets[పంక్తులు_చొప్పున_సమూహం+2], lengths[పంక్తులు_చొప్పున_సమూహం+2];
    స్థిరరాశి uint8_t* s = reinterpret_cast<uint8_t*>(పేర్లు) + పేర్లు->సమూహం_String_స్థానభ్రంశం + GET_సమూహం_స్థానభ్రంశం(సమూహం);
    s=expand_సమూహం_Lengths(s, offsets, lengths);
    తిరిగి expandName(పేర్లు, s+offsets[పంక్తి_సంఖ్య], lengths[పంక్తి_సంఖ్య], nameChoice,
                      బఫరు, బఫరు_నిడివి);
}

స్థిర uint16_t
getName(UChar_పేర్లు *పేర్లు, uint32_t సంకేతం_2, UCharNameChoice nameChoice,
        అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    స్థిరరాశి uint16_t *సమూహం=get_సమూహం(పేర్లు, సంకేతం_2);
    ఒకవేళ (static_cast<uint16_t>(సంకేతం_2 >> సమూహం_జరుపు) == సమూహం[సమూహం_MSB]) {
        తిరిగి expand_సమూహం_Name(పేర్లు, సమూహం, static_cast<uint16_t>(సంకేతం_2 & సమూహం_ముసుగు), nameChoice,
                               బఫరు, బఫరు_నిడివి);
    } లేకపోతే {
        /* group not found */
        /* zero-terminate */
        ఒకవేళ(బఫరు_నిడివి>0) {
            *బఫరు=0;
        }
        తిరిగి 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
స్థిర UBool
enum_సమూహం_పేర్లు(UChar_పేర్లు *పేర్లు, స్థిరరాశి uint16_t *సమూహం,
               UChar32 ప్రారంభం, UChar32 చివర,
               UEnum_Char_పేర్లు_Fn *fn, శూన్య *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[పంక్తులు_చొప్పున_సమూహం+2], lengths[పంక్తులు_చొప్పున_సమూహం+2];
    స్థిరరాశి uint8_t* s = reinterpret_cast<uint8_t*>(పేర్లు) + పేర్లు->సమూహం_String_స్థానభ్రంశం + GET_సమూహం_స్థానభ్రంశం(సమూహం);

    s=expand_సమూహం_Lengths(s, offsets, lengths);
    ఒకవేళ(fn!=DO_కనుగొను_NAME) {
        అక్షరం బఫరు[200];
        uint16_t length;

        అయితే(ప్రారంభం<=చివర) {
            length=expandName(పేర్లు, s+offsets[ప్రారంభం&సమూహం_ముసుగు], lengths[ప్రారంభం&సమూహం_ముసుగు], nameChoice, బఫరు, పరిమాణం(బఫరు));
            ఒకవేళ (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                బఫరు[length = getExtName(ప్రారంభం, బఫరు, పరిమాణం(బఫరు))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            ఒకవేళ(length>0) {
                ఒకవేళ(!fn(context, ప్రారంభం, nameChoice, బఫరు, length)) {
                    తిరిగి అబద్ధం;
                }
            }
            ++ప్రారంభం;
        }
    } లేకపోతే {
        స్థిరరాశి CharacterNameQuery& otherName = *static_cast<కనుగొను_Name*>(context)->otherName;
        అయితే(ప్రారంభం<=చివర) {
            ఒకవేళ(compareName(పేర్లు, s+offsets[ప్రారంభం&సమూహం_ముసుగు], lengths[ప్రారంభం&సమూహం_ముసుగు], nameChoice, otherName)) {
                static_cast<కనుగొను_Name*>(context)->సంకేతం_2 = ప్రారంభం;
                తిరిగి అబద్ధం;
            }
            ++ప్రారంభం;
        }
    }
    తిరిగి నిజం;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
స్థిర UBool
enum_Ext_పేర్లు(UChar32 ప్రారంభం, UChar32 చివర,
             UEnum_Char_పేర్లు_Fn *fn, శూన్య *context)
{
    ఒకవేళ(fn!=DO_కనుగొను_NAME) {
        అక్షరం బఫరు[200];
        uint16_t length;
        
        అయితే(ప్రారంభం<=చివర) {
            బఫరు[length = getExtName(ప్రారంభం, బఫరు, పరిమాణం(బఫరు))] = 0;
            /* here, we assume that the buffer is large enough */
            ఒకవేళ(length>0) {
                ఒకవేళ(!fn(context, ప్రారంభం, U_EXTENDED_CHAR_NAME, బఫరు, length)) {
                    తిరిగి అబద్ధం;
                }
            }
            ++ప్రారంభం;
        }
    }

    తిరిగి నిజం;
}

స్థిర UBool
enum_పేర్లు(UChar_పేర్లు *పేర్లు,
          UChar32 ప్రారంభం, UChar32 హద్దు,
          UEnum_Char_పేర్లు_Fn *fn, శూన్య *context,
          UCharNameChoice nameChoice) {
    uint16_t ప్రారంభం_సమూహం_MSB, చివర_సమూహం_MSB, సమూహం_లెక్క;
    స్థిరరాశి uint16_t *సమూహం, *సమూహం_హద్దు;

    ప్రారంభం_సమూహం_MSB = static_cast<uint16_t>(ప్రారంభం >> సమూహం_జరుపు);
    చివర_సమూహం_MSB = static_cast<uint16_t>((హద్దు - 1) >> సమూహం_జరుపు);

    /* find the group that contains start, or the highest before it */
    సమూహం=get_సమూహం(పేర్లు, ప్రారంభం);

    ఒకవేళ(ప్రారంభం_సమూహం_MSB<సమూహం[సమూహం_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_హద్దు = static_cast<UChar32>(సమూహం[సమూహం_MSB]) << సమూహం_జరుపు;
        ఒకవేళ(ext_హద్దు>హద్దు) {
            ext_హద్దు=హద్దు;
        }
        ఒకవేళ(!enum_Ext_పేర్లు(ప్రారంభం, ext_హద్దు-1, fn, context)) {
            తిరిగి అబద్ధం;
        }
        ప్రారంభం=ext_హద్దు;
    }

    ఒకవేళ(ప్రారంభం_సమూహం_MSB==చివర_సమూహం_MSB) {
        ఒకవేళ(ప్రారంభం_సమూహం_MSB==సమూహం[సమూహం_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            తిరిగి enum_సమూహం_పేర్లు(పేర్లు, సమూహం, ప్రారంభం, హద్దు-1, fn, context, nameChoice);
        }
    } లేకపోతే {
        స్థిరరాశి uint16_t *groups=GET_GROUPS(పేర్లు);
        సమూహం_లెక్క=*groups++;
        సమూహం_హద్దు=groups+సమూహం_లెక్క*సమూహం_నిడివి;

        ఒకవేళ(ప్రారంభం_సమూహం_MSB==సమూహం[సమూహం_MSB]) {
            /* enumerate characters in the partial start group */
            ఒకవేళ((ప్రారంభం&సమూహం_ముసుగు)!=0) {
                ఒకవేళ(!enum_సమూహం_పేర్లు(పేర్లు, సమూహం,
                                   ప్రారంభం, (static_cast<UChar32>(ప్రారంభం_సమూహం_MSB) << సమూహం_జరుపు) + పంక్తులు_చొప్పున_సమూహం - 1,
                                   fn, context, nameChoice)) {
                    తిరిగి అబద్ధం;
                }
                సమూహం=తదుపరి_సమూహం(సమూహం); /* continue with the next group */
            }
        } లేకపోతే ఒకవేళ(ప్రారంభం_సమూహం_MSB>సమూహం[సమూహం_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            స్థిరరాశి uint16_t *తదుపరి_సమూహం_2=తదుపరి_సమూహం(సమూహం);
            ఒకవేళ (తదుపరి_సమూహం_2 < సమూహం_హద్దు && తదుపరి_సమూహం_2[సమూహం_MSB] > ప్రారంభం_సమూహం_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 చివర = తదుపరి_సమూహం_2[సమూహం_MSB] << సమూహం_జరుపు;
                ఒకవేళ (చివర > హద్దు) {
                    చివర = హద్దు;
                }
                ఒకవేళ (!enum_Ext_పేర్లు(ప్రారంభం, చివర - 1, fn, context)) {
                    తిరిగి అబద్ధం;
                }
            }
            సమూహం=తదుపరి_సమూహం_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        అయితే(సమూహం<సమూహం_హద్దు && సమూహం[సమూహం_MSB]<చివర_సమూహం_MSB) {
            స్థిరరాశి uint16_t *తదుపరి_సమూహం_2;
            ప్రారంభం = static_cast<UChar32>(సమూహం[సమూహం_MSB]) << సమూహం_జరుపు;
            ఒకవేళ(!enum_సమూహం_పేర్లు(పేర్లు, సమూహం, ప్రారంభం, ప్రారంభం+పంక్తులు_చొప్పున_సమూహం-1, fn, context, nameChoice)) {
                తిరిగి అబద్ధం;
            }
            తదుపరి_సమూహం_2=తదుపరి_సమూహం(సమూహం);
            ఒకవేళ (తదుపరి_సమూహం_2 < సమూహం_హద్దు && తదుపరి_సమూహం_2[సమూహం_MSB] > సమూహం[సమూహం_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 చివర = తదుపరి_సమూహం_2[సమూహం_MSB] << సమూహం_జరుపు;
                ఒకవేళ (చివర > హద్దు) {
                    చివర = హద్దు;
                }
                ఒకవేళ (!enum_Ext_పేర్లు((సమూహం[సమూహం_MSB] + 1) << సమూహం_జరుపు, చివర - 1, fn, context)) {
                    తిరిగి అబద్ధం;
                }
            }
            సమూహం=తదుపరి_సమూహం_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        ఒకవేళ(సమూహం<సమూహం_హద్దు && సమూహం[సమూహం_MSB]==చివర_సమూహం_MSB) {
            తిరిగి enum_సమూహం_పేర్లు(పేర్లు, సమూహం, (హద్దు-1)&~సమూహం_ముసుగు, హద్దు-1, fn, context, nameChoice);
        } లేకపోతే ఒకవేళ (nameChoice == U_EXTENDED_CHAR_NAME && సమూహం == సమూహం_హద్దు) {
            UChar32 next = (PREV_సమూహం(సమూహం)[సమూహం_MSB] + 1) << సమూహం_జరుపు;
            ఒకవేళ (next > ప్రారంభం) {
                ప్రారంభం = next;
            }
        } లేకపోతే {
            తిరిగి నిజం;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    ఒకవేళ (nameChoice == U_EXTENDED_CHAR_NAME) {
        ఒకవేళ (హద్దు > UCHAR_MAX_విలువ + 1) {
            హద్దు = UCHAR_MAX_విలువ + 1;
        }
        తిరిగి enum_Ext_పేర్లు(ప్రారంభం, హద్దు - 1, fn, context);
    }
    
    తిరిగి నిజం;
}

స్థిర uint16_t
write_గుణకం_Suffix(స్థిరరాశి uint16_t *factors, uint16_t count,
                  స్థిరరాశి అక్షరం *s, /* suffix elements */
                  uint32_t సంకేతం_2,
                  uint16_t indexes[8], /* output fields from here */
                  స్థిరరాశి అక్షరం *elementBases[8], స్థిరరాశి అక్షరం *elements[8],
                  అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    uint16_t i, గుణకం, బఫరు_Pos=0;
    అక్షరం c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    కొరకు(i=count; i>0; --i) {
        గుణకం=factors[i];
        indexes[i] = static_cast<uint16_t>(సంకేతం_2 % గుణకం);
        సంకేతం_2/=గుణకం;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(సంకేతం_2);

    /* write each element */
    కొరకు(;;) {
        ఒకవేళ(elementBases!=శూన్యసూచిక) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        గుణకం=indexes[i];
        అయితే(గుణకం>0) {
            అయితే(*s++!=0) {}
            --గుణకం;
        }
        ఒకవేళ(elements!=శూన్యసూచిక) {
            *elements++=s;
        }

        /* write element */
        అయితే((c=*s++)!=0) {
            WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        ఒకవేళ(i>=count) {
            ఆపు;
        }

        /* skip the rest of the strings for this factors[i] */
        గుణకం = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        అయితే(గుణకం>0) {
            అయితే(*s++!=0) {}
            --గుణకం;
        }

        ++i;
    }

    /* zero-terminate */
    ఒకవేళ(బఫరు_నిడివి>0) {
        *బఫరు=0;
    }

    తిరిగి బఫరు_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
స్థిర uint16_t
getAlgName(AlgorithmicRange *range, uint32_t సంకేతం_2, UCharNameChoice nameChoice,
        అక్షరం *బఫరు, uint16_t బఫరు_నిడివి) {
    uint16_t బఫరు_Pos=0;

    /* Only the normative character name can be algorithmic. */
    ఒకవేళ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        ఒకవేళ(బఫరు_నిడివి>0) {
            *బఫరు=0;
        }
        తిరిగి 0;
    }

    ఎంపిక(range->type) {
    స్థితి 0: {
        /* name = prefix hex-digits */
        స్థిరరాశి అక్షరం* s = reinterpret_cast<స్థిరరాశి అక్షరం*>(range + 1);
        అక్షరం c;

        uint16_t i, count;

        /* copy prefix */
        అయితే((c=*s++)!=0) {
            WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        ఒకవేళ(count<బఫరు_నిడివి) {
            బఫరు[count]=0;
        }

        కొరకు(i=count; i>0;) {
            ఒకవేళ(--i<బఫరు_నిడివి) {
                c = static_cast<అక్షరం>(సంకేతం_2 & 0xf);
                ఒకవేళ(c<10) {
                    c+='0';
                } లేకపోతే {
                    c+='A'-10;
                }
                బఫరు[i]=c;
            }
            సంకేతం_2>>=4;
        }

        బఫరు_Pos+=count;
        ఆపు;
    }
    స్థితి 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        స్థిరరాశి uint16_t* factors = reinterpret_cast<స్థిరరాశి uint16_t*>(range + 1);
        uint16_t count=range->variant;
        స్థిరరాశి అక్షరం* s = reinterpret_cast<స్థిరరాశి అక్షరం*>(factors + count);
        అక్షరం c;

        /* copy prefix */
        అయితే((c=*s++)!=0) {
            WRITE_CHAR(బఫరు, బఫరు_నిడివి, బఫరు_Pos, c);
        }

        బఫరు_Pos+=write_గుణకం_Suffix(factors, count,
                                     s, సంకేతం_2-range->ప్రారంభం, indexes, శూన్యసూచిక, శూన్యసూచిక, బఫరు, బఫరు_నిడివి);
        ఆపు;
    }
    అప్రమేయ:
        /* undefined type */
        /* zero-terminate */
        ఒకవేళ(బఫరు_నిడివి>0) {
            *బఫరు=0;
        }
        ఆపు;
    }

    తిరిగి బఫరు_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
స్థిర UBool
enum_Alg_పేర్లు(AlgorithmicRange *range,
             UChar32 ప్రారంభం, UChar32 హద్దు,
             UEnum_Char_పేర్లు_Fn *fn, శూన్య *context,
             UCharNameChoice nameChoice) {
    అక్షరం బఫరు[200];
    uint16_t length;

    ఒకవేళ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        తిరిగి నిజం;
    }

    ఎంపిక(range->type) {
    స్థితి 0: {
        అక్షరం *s, *చివర;
        అక్షరం c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(ప్రారంభం), nameChoice, బఫరు, పరిమాణం(బఫరు));
        ఒకవేళ(length<=0) {
            తిరిగి నిజం;
        }

        /* call the enumerator function with this first character */
        ఒకవేళ(!fn(context, ప్రారంభం, nameChoice, బఫరు, length)) {
            తిరిగి అబద్ధం;
        }

        /* go to the end of the name; all these names have the same length */
        చివర=బఫరు;
        అయితే(*చివర!=0) {
            ++చివర;
        }

        /* enumerate the rest of the names */
        అయితే(++ప్రారంభం<హద్దు) {
            /* increment the hexadecimal number on a character-basis */
            s=చివర;
            కొరకు (;;) {
                c=*--s;
                ఒకవేళ(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<అక్షరం>(c + 1);
                    ఆపు;
                } లేకపోతే ఒకవేళ(c=='9') {
                    *s='A';
                    ఆపు;
                } లేకపోతే ఒకవేళ(c=='F') {
                    *s='0';
                }
            }

            ఒకవేళ(!fn(context, ప్రారంభం, nameChoice, బఫరు, length)) {
                తిరిగి అబద్ధం;
            }
        }
        ఆపు;
    }
    స్థితి 1: {
        uint16_t indexes[8];
        స్థిరరాశి అక్షరం *elementBases[8], *elements[8];
        స్థిరరాశి uint16_t* factors = reinterpret_cast<స్థిరరాశి uint16_t*>(range + 1);
        uint16_t count=range->variant;
        స్థిరరాశి అక్షరం* s = reinterpret_cast<స్థిరరాశి అక్షరం*>(factors + count);
        అక్షరం *suffix, *t;
        uint16_t ఉపసర్గ_నిడివి, i, idx;

        అక్షరం c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=బఫరు;
        ఉపసర్గ_నిడివి=0;
        అయితే((c=*s++)!=0) {
            *suffix++=c;
            ++ఉపసర్గ_నిడివి;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(ఉపసర్గ_నిడివి + write_గుణకం_Suffix(factors, count,
                                              s, static_cast<uint32_t>(ప్రారంభం) - range->ప్రారంభం,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(పరిమాణం(బఫరు) - ఉపసర్గ_నిడివి)));

        /* call the enumerator function with this first character */
        ఒకవేళ(!fn(context, ప్రారంభం, nameChoice, బఫరు, length)) {
            తిరిగి అబద్ధం;
        }

        /* enumerate the rest of the names */
        అయితే(++ప్రారంభం<హద్దు) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            కొరకు (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ఒకవేళ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    అయితే(*s++!=0) {
                    }
                    elements[i]=s;
                    ఆపు;
                } లేకపోతే {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=ఉపసర్గ_నిడివి;
            కొరకు(i=0; i<count; ++i) {
                s=elements[i];
                అయితే((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            ఒకవేళ(!fn(context, ప్రారంభం, nameChoice, బఫరు, length)) {
                తిరిగి అబద్ధం;
            }
        }
        ఆపు;
    }
    అప్రమేయ:
        /* undefined type */
        ఆపు;
    }

    తిరిగి నిజం;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
స్థిర UChar32
కనుగొను_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 సంకేతం_2;
    స్వయంచాలక matcher = query.matcher();

    ఒకవేళ(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        తిరిగి 0xffff;
    }

    ఎంపిక(range->type) {
    స్థితి 0: {
        /* name = prefix hex-digits */
        స్థిరరాశి అక్షరం* s = reinterpret_cast<స్థిరరాశి అక్షరం*>(range + 1);

        /* compare prefix */
        ఒకవేళ (!matcher.consistentWith(s)) {
            తిరిగి 0xffff;
        }

        /* read hexadecimal code point value */
        సంకేతం_2=0;
        కొరకు(అక్షరం c : matcher.మిగిలిన_Significant_Characters()) {
            ఒకవేళ('0'<=c && c<='9') {
                సంకేతం_2=(సంకేతం_2<<4)|(c-'0');
            } లేకపోతే ఒకవేళ('A'<=c && c<='F') {
                సంకేతం_2=(సంకేతం_2<<4)|(c-'A'+10);
            } లేకపోతే {
                తిరిగి 0xffff;
            }
        }

        /* does it fit into the range? */
        ఒకవేళ (range->ప్రారంభం <= static_cast<uint32_t>(సంకేతం_2) && static_cast<uint32_t>(సంకేతం_2) <= range->చివర) {
            తిరిగి సంకేతం_2;
        }
        ఆపు;
    }
    స్థితి 1: {
        అక్షరం బఫరు[64];
        uint16_t indexes[8];
        స్థిరరాశి అక్షరం *elementBases[8], *elements[8];
        స్థిరరాశి uint16_t* factors = reinterpret_cast<స్థిరరాశి uint16_t*>(range + 1);
        uint16_t count=range->variant;
        స్థిరరాశి అక్షరం *s = reinterpret_cast<స్థిరరాశి అక్షరం*>(factors + count);
        UChar32 ప్రారంభం, హద్దు;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view ఉపసర్గ = s;
        ఒకవేళ (!matcher.consistentWith(ఉపసర్గ)) {
            తిరిగి 0xffff;
        }

        s += ఉపసర్గ.length() + 1;

        ప్రారంభం = static_cast<UChar32>(range->ప్రారంభం);
        హద్దు = static_cast<UChar32>(range->చివర + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.మిగిలిన_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_గుణకం_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, బఫరు, పరిమాణం(బఫరు));

        /* compare the first suffix */
        ఒకవేళ(querySuffix == బఫరు) {
            తిరిగి ప్రారంభం;
        }

        /* enumerate and compare the rest of the suffixes */
        అయితే(++ప్రారంభం<హద్దు) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            కొరకు (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                ఒకవేళ(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    అయితే(*s++!=0) {}
                    elements[i]=s;
                    ఆపు;
                } లేకపోతే {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            స్వయంచాలక it = querySuffix.begin();
            కొరకు(i=0; i<count; ++i) {
                s=elements[i];
                అక్షరం c;
                అయితే((c=*s++)!=0) {
                    ఒకవేళ(it == querySuffix.చివర() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            ఒకవేళ(i<99 && it == querySuffix.చివర()) {
                తిరిగి ప్రారంభం;
            }
        }
        ఆపు;
    }
    అప్రమేయ:
        /* undefined type */
        ఆపు;
    }

    తిరిగి 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#నిర్వచించు సమితి_కూడు_3(సమితి, c) ((సమితి)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#నిర్వచించు సమితి_కలిగియుంది(సమితి, c) (((సమితి)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

స్థిర int32_t
calc_String_సమితి_నిడివి(uint32_t సమితి[8], స్థిరరాశి అక్షరం *s) {
    int32_t length=0;
    అక్షరం c;

    అయితే((c=*s++)!=0) {
        సమితి_కూడు_3(సమితి, c);
        ++length;
    }
    తిరిగి length;
}

స్థిర int32_t
calcAlgNameSetsLengths(int32_t max_Name_నిడివి) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_లెక్క;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_పేర్లు) + u_Char_పేర్లు->alg_పేర్లు_స్థానభ్రంశం);
    range_లెక్క=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    అయితే(range_లెక్క>0) {
        ఎంపిక(range->type) {
        స్థితి 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*ఉపసర్గ*/
            length = calc_String_సమితి_నిడివి(g_Name_సమితి, reinterpret_cast<స్థిరరాశి అక్షరం*>(range + 1)) + range->variant;
            ఒకవేళ(length>max_Name_నిడివి) {
                max_Name_నిడివి=length;
            }
            ఆపు;
        స్థితి 1: {
            /* name = prefix factorized-elements */
            స్థిరరాశి uint16_t* factors = reinterpret_cast<స్థిరరాశి uint16_t*>(range + 1);
            స్థిరరాశి అక్షరం *s;
            int32_t i, count=range->variant, గుణకం, గుణకం_నిడివి, max_గుణకం_నిడివి;

            /* prefix length */
            s = reinterpret_cast<స్థిరరాశి అక్షరం*>(factors + count);
            length=calc_String_సమితి_నిడివి(g_Name_సమితి, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            కొరకు(i=0; i<count; ++i) {
                max_గుణకం_నిడివి=0;
                కొరకు(గుణకం=factors[i]; గుణకం>0; --గుణకం) {
                    గుణకం_నిడివి=calc_String_సమితి_నిడివి(g_Name_సమితి, s);
                    s+=గుణకం_నిడివి+1;
                    ఒకవేళ(గుణకం_నిడివి>max_గుణకం_నిడివి) {
                        max_గుణకం_నిడివి=గుణకం_నిడివి;
                    }
                }
                length+=max_గుణకం_నిడివి;
            }

            ఒకవేళ(length>max_Name_నిడివి) {
                max_Name_నిడివి=length;
            }
            ఆపు;
        }
        అప్రమేయ:
            /* unknown type */
            ఆపు;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_లెక్క;
    }
    తిరిగి max_Name_నిడివి;
}

స్థిర int32_t
calcExtNameSetsLengths(int32_t max_Name_నిడివి) {
    int32_t i, length;

    కొరకు(i=0; i<UPRV_LENGTHOF(char_Cat_పేర్లు); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_సమితి_నిడివి(g_Name_సమితి, char_Cat_పేర్లు[i]);
        ఒకవేళ(length>max_Name_నిడివి) {
            max_Name_నిడివి=length;
        }
    }
    తిరిగి max_Name_నిడివి;
}

స్థిర int32_t
calc_Name_సమితి_నిడివి(స్థిరరాశి uint16_t *టోకెన్లు, uint16_t token_లెక్క, స్థిరరాశి uint8_t *token_అక్షరశ్రేణులు, int8_t *tokenLengths,
                  uint32_t సమితి[8],
                  స్థిరరాశి uint8_t **p_పంక్తి, స్థిరరాశి uint8_t *పంక్తి_హద్దు) {
    స్థిరరాశి uint8_t *పంక్తి=*p_పంక్తి;
    int32_t length=0, token_నిడివి;
    uint16_t c, token;

    అయితే (పంక్తి != పంక్తి_హద్దు && (c = *పంక్తి++) != static_cast<uint8_t>(';')) {
        ఒకవేళ(c>=token_లెక్క) {
            /* implicit letter */
            సమితి_కూడు_3(సమితి, c);
            ++length;
        } లేకపోతే {
            token=టోకెన్లు[c];
            ఒకవేళ (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*పంక్తి++;
                token=టోకెన్లు[c];
            }
            ఒకవేళ (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                సమితి_కూడు_3(సమితి, c);
                ++length;
            } లేకపోతే {
                /* count token word */
                ఒకవేళ(tokenLengths!=శూన్యసూచిక) {
                    /* use cached token length */
                    token_నిడివి=tokenLengths[c];
                    ఒకవేళ(token_నిడివి==0) {
                        token_నిడివి = calc_String_సమితి_నిడివి(సమితి, reinterpret_cast<స్థిరరాశి అక్షరం*>(token_అక్షరశ్రేణులు) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_నిడివి);
                    }
                } లేకపోతే {
                    token_నిడివి = calc_String_సమితి_నిడివి(సమితి, reinterpret_cast<స్థిరరాశి అక్షరం*>(token_అక్షరశ్రేణులు) + token);
                }
                length+=token_నిడివి;
            }
        }
    }

    *p_పంక్తి=పంక్తి;
    తిరిగి length;
}

స్థిర శూన్య
calc_సమూహం_Name_Sets_Lengths(int32_t max_Name_నిడివి) {
    uint16_t offsets[పంక్తులు_చొప్పున_సమూహం+2], lengths[పంక్తులు_చొప్పున_సమూహం+2];

    uint16_t* టోకెన్లు = reinterpret_cast<uint16_t*>(u_Char_పేర్లు) + 8;
    uint16_t token_లెక్క=*టోకెన్లు++;
    uint8_t* token_అక్షరశ్రేణులు = reinterpret_cast<uint8_t*>(u_Char_పేర్లు) + u_Char_పేర్లు->token_String_స్థానభ్రంశం;

    int8_t *tokenLengths;

    స్థిరరాశి uint16_t *సమూహం;
    స్థిరరాశి uint8_t *s, *పంక్తి, *పంక్తి_హద్దు;

    int32_t సమూహం_లెక్క, పంక్తి_సంఖ్య, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_లెక్క));
    ఒకవేళ(tokenLengths!=శూన్యసూచిక) {
        uprv_memset(tokenLengths, 0, token_లెక్క);
    }

    సమూహం=GET_GROUPS(u_Char_పేర్లు);
    సమూహం_లెక్క=*సమూహం++;

    /* enumerate all groups */
    అయితే(సమూహం_లెక్క>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_పేర్లు) + u_Char_పేర్లు->సమూహం_String_స్థానభ్రంశం + GET_సమూహం_స్థానభ్రంశం(సమూహం);
        s=expand_సమూహం_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        కొరకు(పంక్తి_సంఖ్య=0; పంక్తి_సంఖ్య<పంక్తులు_చొప్పున_సమూహం; ++పంక్తి_సంఖ్య) {
            పంక్తి=s+offsets[పంక్తి_సంఖ్య];
            length=lengths[పంక్తి_సంఖ్య];
            ఒకవేళ(length==0) {
                కొనసాగు;
            }

            పంక్తి_హద్దు=పంక్తి+length;

            /* read regular name */
            length=calc_Name_సమితి_నిడివి(టోకెన్లు, token_లెక్క, token_అక్షరశ్రేణులు, tokenLengths, g_Name_సమితి, &పంక్తి, పంక్తి_హద్దు);
            ఒకవేళ(length>max_Name_నిడివి) {
                max_Name_నిడివి=length;
            }
            ఒకవేళ(పంక్తి==పంక్తి_హద్దు) {
                కొనసాగు;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_సమితి_నిడివి(టోకెన్లు, token_లెక్క, token_అక్షరశ్రేణులు, tokenLengths, g_Name_సమితి, &పంక్తి, పంక్తి_హద్దు);
            ఒకవేళ(length>max_Name_నిడివి) {
                max_Name_నిడివి=length;
            }
            ఒకవేళ(పంక్తి==పంక్తి_హద్దు) {
                కొనసాగు;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        సమూహం=తదుపరి_సమూహం(సమూహం);
        --సమూహం_లెక్క;
    }

    ఒకవేళ(tokenLengths!=శూన్యసూచిక) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_నిడివి=max_Name_నిడివి;
}

స్థిర UBool
calcNameSetsLengths(UError_సంకేతం *p_Error_సంకేతం) {
    స్థిర స్థిరరాశి అక్షరం extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_నిడివి;

    ఒకవేళ(g_Max_Name_నిడివి!=0) {
        తిరిగి నిజం;
    }

    ఒకవేళ(!is_దత్తాంశం_Loaded(p_Error_సంకేతం)) {
        తిరిగి అబద్ధం;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    కొరకు (i = 0; i < static_cast<int32_t>(పరిమాణం(extChars)) - 1; ++i) {
        సమితి_కూడు_3(g_Name_సమితి, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_నిడివి=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_నిడివి=calcExtNameSetsLengths(max_Name_నిడివి);

    /* set sets and lengths from group names, set global maximum values */
    calc_సమూహం_Name_Sets_Lengths(max_Name_నిడివి);

    తిరిగి నిజం;
}

U_NAMESPACE_చివర

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 సంకేతం_2, UCharNameChoice nameChoice,
           అక్షరం *బఫరు, int32_t బఫరు_నిడివి,
           UError_సంకేతం *p_Error_సంకేతం) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ(nameChoice>=U_CHAR_NAME_CHOICE_లెక్క ||
              బఫరు_నిడివి<0 || (బఫరు_నిడివి>0 && బఫరు==శూన్యసూచిక)
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ((uint32_t)సంకేతం_2>UCHAR_MAX_విలువ || !is_దత్తాంశం_Loaded(p_Error_సంకేతం)) {
        తిరిగి u_terminateChars(బఫరు, బఫరు_నిడివి, 0, p_Error_సంకేతం);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_పేర్లు+u_Char_పేర్లు->alg_పేర్లు_స్థానభ్రంశం);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    అయితే(i>0) {
        ఒకవేళ(algRange->ప్రారంభం<=(uint32_t)సంకేతం_2 && (uint32_t)సంకేతం_2<=algRange->చివర) {
            length=getAlgName(algRange, (uint32_t)సంకేతం_2, nameChoice, బఫరు, (uint16_t)బఫరు_నిడివి);
            ఆపు;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    ఒకవేళ(i==0) {
        ఒకవేళ (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_పేర్లు, (uint32_t )సంకేతం_2, U_EXTENDED_CHAR_NAME, బఫరు, (uint16_t) బఫరు_నిడివి);
            ఒకవేళ (!length) {
                /* extended character name */
                length = getExtName((uint32_t) సంకేతం_2, బఫరు, (uint16_t) బఫరు_నిడివి);
            }
        } లేకపోతే {
            /* normal character name */
            length=getName(u_Char_పేర్లు, (uint32_t)సంకేతం_2, nameChoice, బఫరు, (uint16_t)బఫరు_నిడివి);
        }
    }

    తిరిగి u_terminateChars(బఫరు, బఫరు_నిడివి, length, p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                అక్షరం *dest, int32_t dest_సామర్థ్యం,
                UError_సంకేతం *p_Error_సంకేతం) {
    /* check the argument values */
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    } లేకపోతే ఒకవేళ(dest_సామర్థ్యం<0 || (dest_సామర్థ్యం>0 && dest==శూన్యసూచిక)) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    తిరిగి u_terminateChars(dest, dest_సామర్థ్యం, 0, p_Error_సంకేతం);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               స్థిరరాశి అక్షరం *స్థిరరాశి name,
               UError_సంకేతం *p_Error_సంకేతం) {
    అక్షరం క్రింది[120] = {0};
    కనుగొను_Name కనుగొను_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    అక్షరం c0;
    స్థిర constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి error;
    }

    ఒకవేళ(nameChoice>=U_CHAR_NAME_CHOICE_లెక్క || name==శూన్యసూచిక || *name==0) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి error;
    }

    ఒకవేళ(!is_దత్తాంశం_Loaded(p_Error_సంకేతం)) {
        తిరిగి error;
    }

    /* construct the lowercase of the name first */
    స్థిరరాశి అక్షరం* nameChar = name;
    కొరకు(i=0; i<పరిమాణం(క్రింది); ++i) {
        ఒకవేళ((c0=*nameChar++)!=0) {
            క్రింది[i]=uprv_tolower(c0);
        } లేకపోతే {
            ఆపు;
        }
    }
    ఒకవేళ(i==పరిమాణం(క్రింది)) {
        /* name too long, there is no such character */
        *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
        తిరిగి error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    ఒకవేళ (క్రింది[0] == '<') {
        ఒకవేళ (nameChoice == U_EXTENDED_CHAR_NAME && క్రింది[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t హద్దు = i;
            అయితే (i >= 3 && క్రింది[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_నిడివి = హద్దు - (i + 1);
            ఒకవేళ (i >= 2 && క్రింది[i] == '-' && 1 <= hex_నిడివి && hex_నిడివి <= 8) {
                uint32_t cIdx;

                క్రింది[i] = 0;

                కొరకు (++i; i < హద్దు; ++i) {
                    ఒకవేళ (క్రింది[i] >= '0' && క్రింది[i] <= '9') {
                        cp = (cp << 4) + క్రింది[i] - '0';
                    } లేకపోతే ఒకవేళ (క్రింది[i] >= 'a' && క్రింది[i] <= 'f') {
                        cp = (cp << 4) + క్రింది[i] - 'a' + 10;
                    } లేకపోతే {
                        *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
                        తిరిగి error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    ఒకవేళ (cp > UCHAR_MAX_విలువ) {
                        *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
                        తిరిగి error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                కొరకు (క్రింది[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_పేర్లు); ++cIdx) {

                    ఒకవేళ (!uprv_strcmp(క్రింది + 1, char_Cat_పేర్లు[cIdx])) {
                        ఒకవేళ (cat == cIdx) {
                            తిరిగి cp;
                        }
                        ఆపు;
                    }
                }
            }
        }

        *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
        తిరిగి error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_పేర్లు+u_Char_పేర్లు->alg_పేర్లు_స్థానభ్రంశం);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    అయితే(i>0) {
        ఒకవేళ((cp=కనుగొను_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            తిరిగి cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    కనుగొను_Name_2.otherName=&query;
    కనుగొను_Name_2.సంకేతం_2=error;
    enum_పేర్లు(u_Char_పేర్లు, 0, UCHAR_MAX_విలువ + 1, DO_కనుగొను_NAME, &కనుగొను_Name_2, nameChoice);
    ఒకవేళ (కనుగొను_Name_2.సంకేతం_2 == error) {
         *p_Error_సంకేతం = U_ILLEGAL_CHAR_దొరికింది;
    }
    తిరిగి కనుగొను_Name_2.సంకేతం_2;
}

U_CAPI శూన్య U_EXPORT2
u_enum_Char_పేర్లు(UChar32 ప్రారంభం, UChar32 హద్దు,
                UEnum_Char_పేర్లు_Fn *fn,
                శూన్య *context,
                UCharNameChoice nameChoice,
                UError_సంకేతం *p_Error_సంకేతం) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    ఒకవేళ(nameChoice>=U_CHAR_NAME_CHOICE_లెక్క || fn==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    ఒకవేళ((uint32_t) హద్దు > UCHAR_MAX_విలువ + 1) {
        హద్దు = UCHAR_MAX_విలువ + 1;
    }
    ఒకవేళ((uint32_t)ప్రారంభం>=(uint32_t)హద్దు) {
        తిరిగి;
    }

    ఒకవేళ(!is_దత్తాంశం_Loaded(p_Error_సంకేతం)) {
        తిరిగి;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_పేర్లు+u_Char_పేర్లు->alg_పేర్లు_స్థానభ్రంశం);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    అయితే(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        ఒకవేళ((uint32_t)ప్రారంభం<algRange->ప్రారంభం) {
            ఒకవేళ((uint32_t)హద్దు<=algRange->ప్రారంభం) {
                enum_పేర్లు(u_Char_పేర్లు, ప్రారంభం, హద్దు, fn, context, nameChoice);
                తిరిగి;
            }
            ఒకవేళ(!enum_పేర్లు(u_Char_పేర్లు, ప్రారంభం, (UChar32)algRange->ప్రారంభం, fn, context, nameChoice)) {
                తిరిగి;
            }
            ప్రారంభం=(UChar32)algRange->ప్రారంభం;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        ఒకవేళ((uint32_t)ప్రారంభం<=algRange->చివర) {
            ఒకవేళ((uint32_t)హద్దు<=(algRange->చివర+1)) {
                enum_Alg_పేర్లు(algRange, ప్రారంభం, హద్దు, fn, context, nameChoice);
                తిరిగి;
            }
            ఒకవేళ(!enum_Alg_పేర్లు(algRange, ప్రారంభం, (UChar32)algRange->చివర+1, fn, context, nameChoice)) {
                తిరిగి;
            }
            ప్రారంభం=(UChar32)algRange->చివర+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_పేర్లు(u_Char_పేర్లు, ప్రారంభం, హద్దు, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_నిడివి() {
    UError_సంకేతం error_సంకేతం=U_ZERO_ERROR;
    ఒకవేళ(calcNameSetsLengths(&error_సంకేతం)) {
        తిరిగి g_Max_Name_నిడివి;
    } లేకపోతే {
        తిరిగి 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
స్థిర శూన్య
char_సమితి_గమ్యం_USet(uint32_t cset[8], స్థిరరాశి USetAdder *sa) {
    char16_t us[256];
    అక్షరం cs[256];

    int32_t i, length;
    UError_సంకేతం error_సంకేతం;

    error_సంకేతం=U_ZERO_ERROR;

    ఒకవేళ(!calcNameSetsLengths(&error_సంకేతం)) {
        తిరిగి;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    కొరకు(i=0; i<256; ++i) {
        ఒకవేళ(సమితి_కలిగియుంది(cset, i)) {
            cs[length++] = static_cast<అక్షరం>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_గమ్యం_UChars(cs, us, length);

    /* add each char16_t to the USet */
    కొరకు(i=0; i<length; ++i) {
        ఒకవేళ(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->కూడు(sa->సమితి, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI శూన్య U_EXPORT2
uprv_getCharNameCharacters(స్థిరరాశి USetAdder *sa) {
    char_సమితి_గమ్యం_USet(g_Name_సమితి, sa);
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
స్థిర శూన్య
makeTokenMap(స్థిరరాశి UDataSwapper *ds,
             int16_t టోకెన్లు[], uint16_t token_లెక్క,
             uint8_t map[256],
             UError_సంకేతం *p_Error_సంకేతం) {
    UBool వాడిన_బహిర్గతం_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    ఒకవేళ(ds->లోపల_Charset==ds->బహిర్గతం_Charset) {
        /* Same charset family: identity permutation */
        కొరకు(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } లేకపోతే {
        uprv_memset(map, 0, 256);
        uprv_memset(వాడిన_బహిర్గతం_Char, 0, 256);

        ఒకవేళ(token_లెక్క>256) {
            token_లెక్క=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        కొరకు(i=1; i<token_లెక్క; ++i) {
            ఒకవేళ(టోకెన్లు[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->మార్పిడి_Inv_Chars(ds, &c1, 1, &c2, p_Error_సంకేతం);
                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->లోపల_Charset);
                    తిరిగి;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                వాడిన_బహిర్గతం_Char[c2]=నిజం;
            }
        }

        /* set the mappings for the rest of the permutation */
        కొరకు(i=j=1; i<token_లెక్క; ++i) {
            /* set mappings that were not set for direct bytes */
            ఒకవేళ(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                అయితే(వాడిన_బహిర్గతం_Char[j]) {
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
uchar_మార్పిడి_పేర్లు(స్థిరరాశి UDataSwapper *ds,
                స్థిరరాశి శూన్య *లోపల_దత్తాంశం, int32_t length, శూన్య *బహిర్గతం_దత్తాంశం,
                UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి UDataInfo *pInfo;
    int32_t శీర్షిక_కొలత;

    స్థిరరాశి uint8_t *లోపల_బైట్లు;
    uint8_t *బహిర్గతం_బైట్లు;

    uint32_t token_String_స్థానభ్రంశం, groups_స్థానభ్రంశం, సమూహం_String_స్థానభ్రంశం, alg_పేర్లు_స్థానభ్రంశం,
             స్థానభ్రంశం, i, count, అక్షరశ్రేణులు_లెక్క;

    స్థిరరాశి AlgorithmicRange *లోపల_Range;
    AlgorithmicRange *బహిర్గతం_Range;

    /* udata_swapDataHeader checks the arguments */
    శీర్షిక_కొలత=udata_మార్పిడి_దత్తాంశం_శీర్షిక(ds, లోపల_దత్తాంశం, length, బహిర్గతం_దత్తాంశం, p_Error_సంకేతం);
    ఒకవేళ(p_Error_సంకేతం==శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }

    /* check data format and format version */
    pInfo=(స్థిరరాశి UDataInfo *)((స్థిరరాశి అక్షరం *)లోపల_దత్తాంశం+4);
    ఒకవేళ(!(
        pInfo->దత్తాంశం_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->దత్తాంశం_Format[1]==0x6e &&
        pInfo->దత్తాంశం_Format[2]==0x61 &&
        pInfo->దత్తాంశం_Format[3]==0x6d &&
        pInfo->format_సంస్కరణ[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->దత్తాంశం_Format[0], pInfo->దత్తాంశం_Format[1],
                         pInfo->దత్తాంశం_Format[2], pInfo->దత్తాంశం_Format[3],
                         pInfo->format_సంస్కరణ[0]);
        *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
        తిరిగి 0;
    }

    లోపల_బైట్లు=(స్థిరరాశి uint8_t *)లోపల_దత్తాంశం+శీర్షిక_కొలత;
    బహిర్గతం_బైట్లు=(బహిర్గతం_దత్తాంశం == శూన్యసూచిక) ? శూన్యసూచిక : (uint8_t *)బహిర్గతం_దత్తాంశం+శీర్షిక_కొలత;
    ఒకవేళ(length<0) {
        alg_పేర్లు_స్థానభ్రంశం=ds->readUInt32(((స్థిరరాశి uint32_t *)లోపల_బైట్లు)[3]);
    } లేకపోతే {
        length-=శీర్షిక_కొలత;
        ఒకవేళ( length<20 ||
            (uint32_t)length<(alg_పేర్లు_స్థానభ్రంశం=ds->readUInt32(((స్థిరరాశి uint32_t *)లోపల_బైట్లు)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
            తిరిగి 0;
        }
    }

    ఒకవేళ(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        స్థానభ్రంశం=alg_పేర్లు_స్థానభ్రంశం;
        count=ds->readUInt32(*((స్థిరరాశి uint32_t *)(లోపల_బైట్లు+స్థానభ్రంశం)));
        స్థానభ్రంశం+=4;

        కొరకు(i=0; i<count; ++i) {
            లోపల_Range=(స్థిరరాశి AlgorithmicRange *)(లోపల_బైట్లు+స్థానభ్రంశం);
            స్థానభ్రంశం+=ds->readUInt16(లోపల_Range->size);
        }
    } లేకపోతే {
        /* swap data */
        స్థిరరాశి uint16_t *p;
        uint16_t *q, *తాత్కాలిక;

        int16_t టోకెన్లు[512];
        uint16_t token_లెక్క;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        ఒకవేళ(లోపల_బైట్లు!=బహిర్గతం_బైట్లు) {
            uprv_memcpy(బహిర్గతం_బైట్లు, లోపల_బైట్లు, length);
        }

        /* the initial 4 offsets first */
        token_String_స్థానభ్రంశం=ds->readUInt32(((స్థిరరాశి uint32_t *)లోపల_బైట్లు)[0]);
        groups_స్థానభ్రంశం=ds->readUInt32(((స్థిరరాశి uint32_t *)లోపల_బైట్లు)[1]);
        సమూహం_String_స్థానభ్రంశం=ds->readUInt32(((స్థిరరాశి uint32_t *)లోపల_బైట్లు)[2]);
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు, 16, బహిర్గతం_బైట్లు, p_Error_సంకేతం);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(స్థిరరాశి uint16_t *)(లోపల_బైట్లు+16);
        q=(uint16_t *)(బహిర్గతం_బైట్లు+16);

        /* read and swap the tokenCount */
        token_లెక్క=ds->readUInt16(*p);
        ds->మార్పిడి_Array16(ds, p, 2, q, p_Error_సంకేతం);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        ఒకవేళ(token_లెక్క<=512) {
            count=token_లెక్క;
        } లేకపోతే {
            count=512;
        }
        కొరకు(i=0; i<count; ++i) {
            టోకెన్లు[i]=udata_readInt16(ds, p[i]);
        }
        కొరకు(; i<512; ++i) {
            టోకెన్లు[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, టోకెన్లు, token_లెక్క, map, p_Error_సంకేతం);
        makeTokenMap(ds, టోకెన్లు+256, (uint16_t)(token_లెక్క>256 ? token_లెక్క-256 : 0), trailMap, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        తాత్కాలిక=(uint16_t *)uprv_malloc(token_లెక్క*2);
        ఒకవేళ(తాత్కాలిక==శూన్యసూచిక) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_లెక్క);
            *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        కొరకు(i=0; i<token_లెక్క && i<256; ++i) {
            ds->మార్పిడి_Array16(ds, p+i, 2, తాత్కాలిక+map[i], p_Error_సంకేతం);
        }

        /* swap and permutate trail-byte tokens */
        కొరకు(; i<token_లెక్క; ++i) {
            ds->మార్పిడి_Array16(ds, p+i, 2, తాత్కాలిక+(i&0xffffff00)+trailMap[i&0xff], p_Error_సంకేతం);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, తాత్కాలిక, token_లెక్క*2);
        uprv_free(తాత్కాలిక);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_మార్పిడి_Inv_String_ఖండం(ds, లోపల_బైట్లు+token_String_స్థానభ్రంశం, (int32_t)(groups_స్థానభ్రంశం-token_String_స్థానభ్రంశం),
                                    బహిర్గతం_బైట్లు+token_String_స్థానభ్రంశం, p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            తిరిగి 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((స్థిరరాశి uint16_t *)(లోపల_బైట్లు+groups_స్థానభ్రంశం)));
        ds->మార్పిడి_Array16(ds, లోపల_బైట్లు+groups_స్థానభ్రంశం, (int32_t)((1+count*3)*2),
                           బహిర్గతం_బైట్లు+groups_స్థానభ్రంశం, p_Error_సంకేతం);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        ఒకవేళ(ds->లోపల_Charset!=ds->బహిర్గతం_Charset) {
            uint16_t offsets[పంక్తులు_చొప్పున_సమూహం+1], lengths[పంక్తులు_చొప్పున_సమూహం+1];

            స్థిరరాశి uint8_t *లోపల_అక్షరశ్రేణులు, *తదుపరి_లోపల_అక్షరశ్రేణులు;
            uint8_t *బహిర్గతం_అక్షరశ్రేణులు;

            uint8_t c;

            లోపల_అక్షరశ్రేణులు=లోపల_బైట్లు+సమూహం_String_స్థానభ్రంశం;
            బహిర్గతం_అక్షరశ్రేణులు=బహిర్గతం_బైట్లు+సమూహం_String_స్థానభ్రంశం;

            అక్షరశ్రేణులు_లెక్క=alg_పేర్లు_స్థానభ్రంశం-సమూహం_String_స్థానభ్రంశం;

            /* iterate through string groups until only a few padding bytes are left */
            అయితే(అక్షరశ్రేణులు_లెక్క>32) {
                తదుపరి_లోపల_అక్షరశ్రేణులు=expand_సమూహం_Lengths(లోపల_అక్షరశ్రేణులు, offsets, lengths);

                /* move past the length bytes */
                అక్షరశ్రేణులు_లెక్క-=(uint32_t)(తదుపరి_లోపల_అక్షరశ్రేణులు-లోపల_అక్షరశ్రేణులు);
                బహిర్గతం_అక్షరశ్రేణులు+=తదుపరి_లోపల_అక్షరశ్రేణులు-లోపల_అక్షరశ్రేణులు;
                లోపల_అక్షరశ్రేణులు=తదుపరి_లోపల_అక్షరశ్రేణులు;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                అక్షరశ్రేణులు_లెక్క-=count;

                /* swap the string bytes using map[] and trailMap[] */
                అయితే(count>0) {
                    c=*లోపల_అక్షరశ్రేణులు++;
                    *బహిర్గతం_అక్షరశ్రేణులు++=map[c];
                    ఒకవేళ(టోకెన్లు[c]!=-2) {
                        --count;
                    } లేకపోతే {
                        /* token lead byte: swap the trail byte, too */
                        *బహిర్గతం_అక్షరశ్రేణులు++=trailMap[*లోపల_అక్షరశ్రేణులు++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        స్థానభ్రంశం=alg_పేర్లు_స్థానభ్రంశం;
        count=ds->readUInt32(*((స్థిరరాశి uint32_t *)(లోపల_బైట్లు+స్థానభ్రంశం)));
        ds->మార్పిడి_Array32(ds, లోపల_బైట్లు+స్థానభ్రంశం, 4, బహిర్గతం_బైట్లు+స్థానభ్రంశం, p_Error_సంకేతం);
        స్థానభ్రంశం+=4;

        కొరకు(i=0; i<count; ++i) {
            ఒకవేళ(స్థానభ్రంశం>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_సంకేతం=U_సూచీ_OUTOFBOUNDS_ERROR;
                తిరిగి 0;
            }

            లోపల_Range=(స్థిరరాశి AlgorithmicRange *)(లోపల_బైట్లు+స్థానభ్రంశం);
            బహిర్గతం_Range=(AlgorithmicRange *)(బహిర్గతం_బైట్లు+స్థానభ్రంశం);
            స్థానభ్రంశం+=ds->readUInt16(లోపల_Range->size);

            ds->మార్పిడి_Array32(ds, లోపల_Range, 8, బహిర్గతం_Range, p_Error_సంకేతం);
            ds->మార్పిడి_Array16(ds, &లోపల_Range->size, 2, &బహిర్గతం_Range->size, p_Error_సంకేతం);
            ఎంపిక(లోపల_Range->type) {
            స్థితి 0:
                /* swap prefix string */
                ds->మార్పిడి_Inv_Chars(ds, లోపల_Range+1, (int32_t)uprv_strlen((స్థిరరాశి అక్షరం *)(లోపల_Range+1)),
                                    బహిర్గతం_Range+1, p_Error_సంకేతం);
                ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    తిరిగి 0;
                }
                ఆపు;
            స్థితి 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_లెక్క;

                    factors_లెక్క=లోపల_Range->variant;
                    p=(స్థిరరాశి uint16_t *)(లోపల_Range+1);
                    q=(uint16_t *)(బహిర్గతం_Range+1);
                    ds->మార్పిడి_Array16(ds, p, (int32_t)(factors_లెక్క*2), q, p_Error_సంకేతం);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_లెక్క;
                    q+=factors_లెక్క;
                    అక్షరశ్రేణులు_లెక్క=(uint32_t)((లోపల_బైట్లు+స్థానభ్రంశం)-(స్థిరరాశి uint8_t *)p);
                    అయితే(అక్షరశ్రేణులు_లెక్క>0 && ((స్థిరరాశి uint8_t *)p)[అక్షరశ్రేణులు_లెక్క-1]!=0) {
                        --అక్షరశ్రేణులు_లెక్క;
                    }
                    ds->మార్పిడి_Inv_Chars(ds, p, (int32_t)అక్షరశ్రేణులు_లెక్క, q, p_Error_సంకేతం);
                }
                ఆపు;
            అప్రమేయ:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 లోపల_Range->type, i);
                *p_Error_సంకేతం=U_UNSUPPORTED_ERROR;
                తిరిగి 0;
            }
        }
    }

    తిరిగి శీర్షిక_కొలత+(int32_t)స్థానభ్రంశం;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
