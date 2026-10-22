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

ساكن ثابت حرف انکڑے_NAME[] = "unames";
ساكن ثابت حرف انکڑے_TYPE[] = "icu";

#تعریف_کرو گروہ_کھسکاؤ 5
#تعریف_کرو سطریں_فی_گروہ (1L<<گروہ_کھسکاؤ)
#تعریف_کرو گروہ_نقاب (سطریں_فی_گروہ-1)

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
    گروہ_MSB,
    گروہ_وتھ_بلند,
    گروہ_وتھ_کم,
    گروہ_لمائی
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#تعریف_کرو GET_گروہ_وتھ(گروہ) ((int32_t)(گروہ)[گروہ_وتھ_بلند]<<16|(گروہ)[گروہ_وتھ_کم])

#تعریف_کرو اگلا_گروہ(گروہ) ((گروہ)+گروہ_لمائی)
#تعریف_کرو PREV_گروہ(گروہ) ((گروہ)-گروہ_لمائی)

تعريفنوع بنية {
    uint32_t شروع, سرا;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعريفنوع بنية {
    uint32_t token_String_وتھ, groups_وتھ, گروہ_String_وتھ, alg_ناں_وتھ;
} UChar_ناں;

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

            إذا (skeletonIterator == query.skeleton.سرا()) {
                أرجع خطأ;
            }
            إذا (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                إذا (آخری_Char == '-') {
                    إذا (*skeletonIterator++ != آخری_Char) {
                        أرجع خطأ;
                    }
                }
                آخری_Char = c;
                أرجع سچ;
            } وإلا إذا (c == '-') {
                إذا (آخری_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.سرا() - 2)) {
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
                آخری_Char = c;
                أرجع سچ;
            } وإلا {
                إذا (*skeletonIterator++ != c) {
                    أرجع خطأ;
                }
                آخری_Char = c;
                أرجع سچ;
            }
        }

        منطقي consistentWith(ثابت std::string_view substring) {
            لكل (ثابت حرف c : substring) {
                إذا (!consistentWith(c)) {
                    أرجع خطأ;
                }
            }
            أرجع سچ;
        }

        منطقي matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(آخری_Char != '-');
            أرجع skeletonIterator == query.skeleton.سرا();
        }

        std::string_view بچیا_Significant_Characters() ثابت {
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
        حرف آخری_Char = ' ';
    };

    واضح CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeleton_حد_بندی = skeleton_انکڑے;
        لكل (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_حد_بندی < skeleton_انکڑے + حجم(skeleton_انکڑے));
            إذا (skeleton_حد_بندی >= skeleton_انکڑے + حجم(skeleton_انکڑے)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_حد_بندی = skeleton_انکڑے;
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
                    std::string_view(skeleton_انکڑے, skeleton_حد_بندی - skeleton_انکڑے) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = سچ;
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
                    *skeleton_حد_بندی++ = query[i];
                }
            } وإلا {
                *skeleton_حد_بندی++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_انکڑے, skeleton_حد_بندی - skeleton_انکڑے);
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

    حرف skeleton_انکڑے[120];
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
#تعریف_کرو GET_GROUPS(ناں) (ثابت uint16_t *)((ثابت حرف *)ناں+ناں->groups_وتھ)

تعريفنوع بنية {
    ثابت CharacterNameQuery *otherName;
    UChar32 کوڈ;
} لبھو_Name;

#تعریف_کرو DO_لبھو_NAME خالی_سوچک

ساكن UData_حافظہ *u_Char_ناں_انکڑے=خالی_سوچک;
ساكن UChar_ناں *u_Char_ناں=خالی_سوچک;
ساكن icu::UInitOnce g_Char_ناں_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساكن int32_t g_Max_Name_لمائی=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساكن uint32_t g_Name_سیٹ[8]={ 0 };

#تعریف_کرو U_NONCHARACTER_کوڈ_نقطہ U_CHAR_CATEGORY_گنتی
#تعریف_کرو U_LEAD_SURROGATE U_CHAR_CATEGORY_گنتی + 1
#تعریف_کرو U_TRAIL_SURROGATE U_CHAR_CATEGORY_گنتی + 2

#تعریف_کرو U_CHAR_EXTENDED_CATEGORY_گنتی (U_CHAR_CATEGORY_گنتی + 3)

ساكن ثابت حرف * ثابت char_Cat_ناں[U_CHAR_EXTENDED_CATEGORY_گنتی] = {
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
    إذا(u_Char_ناں_انکڑے) {
        udata_close(u_Char_ناں_انکڑے);
        u_Char_ناں_انکڑے = خالی_سوچک;
    }
    إذا(u_Char_ناں) {
        u_Char_ناں = خالی_سوچک;
    }
    g_Char_ناں_Init_Once.reset();
    g_Max_Name_لمائی=0;
    أرجع سچ;
}

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->انکڑے_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->انکڑے_Format[1]==0x6e &&
        pInfo->انکڑے_Format[2]==0x61 &&
        pInfo->انکڑے_Format[3]==0x6d &&
        pInfo->format_نسخہ[0]==1;
}

ساكن فراغ U_CALLCONV
لدو_Char_ناں(UError_کوڈ &حالت) {
    U_ASSERT(u_Char_ناں_انکڑے == خالی_سوچک);
    U_ASSERT(u_Char_ناں == خالی_سوچک);

    u_Char_ناں_انکڑے = udata_openChoice(خالی_سوچک, انکڑے_TYPE, انکڑے_NAME, isAcceptable, خالی_سوچک, &حالت);
    إذا(U_FAILURE(حالت)) {
        u_Char_ناں_انکڑے = خالی_سوچک;
    } وإلا {
        u_Char_ناں = (UChar_ناں *)udata_get_حافظہ(u_Char_ناں_انکڑے);
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساكن UBool
is_انکڑے_Loaded(UError_کوڈ *p_Error_کوڈ) {
    umtx_initOnce(g_Char_ناں_Init_Once, &لدو_Char_ناں, *p_Error_کوڈ);
    أرجع U_SUCCESS(*p_Error_کوڈ);
}

#تعریف_کرو WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c) UPRV_بلاک_MACRO_BEGIN { \
    إذا((بفر_لمائی)>0) { \
        *(بفر)++=c; \
        --(بفر_لمائی); \
    } \
    ++(بفر_Pos); \
} UPRV_بلاک_MACRO_سرا

#تعریف_کرو U_ISO_COMMENT U_CHAR_NAME_CHOICE_گنتی

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
expandName(UChar_ناں *ناں,
           ثابت uint8_t *name, uint16_t name_لمائی, UCharNameChoice nameChoice,
           حرف *بفر, uint16_t بفر_لمائی) {
    uint16_t* ٹوکن = reinterpret_cast<uint16_t*>(ناں) + 8;
    uint16_t token, token_گنتی=*ٹوکن++, بفر_Pos=0;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(ناں) + ناں->token_String_وتھ;
    uint8_t c;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_گنتی || ٹوکن[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح میدان_اشاریہ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_لمائی>0) {
                    --name_لمائی;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--میدان_اشاریہ>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_لمائی=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    طالما(name_لمائی>0) {
        --name_لمائی;
        c=*name++;

        إذا(c>=token_گنتی) {
            إذا(c!=';') {
                /* implicit letter */
                WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
            } وإلا {
                /*مکمل*/
                اكسر;
            }
        } وإلا {
            token=ٹوکن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ٹوکن[c<<8|*name++];
                --name_لمائی;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
                } وإلا {
                    /*مکمل*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                uint8_t *tokenString=token_لڑیاں+token;
                طالما((c=*tokenString++)!=0) {
                    WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    إذا(بفر_لمائی>0) {
        *بفر=0;
    }

    أرجع بفر_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ساكن UBool
compareName(UChar_ناں *ناں,
            ثابت uint8_t *name, uint16_t name_لمائی, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* ٹوکن = reinterpret_cast<uint16_t*>(ناں) + 8;
    uint16_t token, token_گنتی=*ٹوکن++;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(ناں) + ناں->token_String_وتھ;
    uint8_t c;
    خودکار matcher = otherName.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_گنتی || ٹوکن[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح میدان_اشاریہ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_لمائی>0) {
                    --name_لمائی;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--میدان_اشاریہ>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_لمائی=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    طالما(name_لمائی>0) {
        --name_لمائی;
        c=*name++;

        إذا(c>=token_گنتی) {
            إذا(c!=';') {
                /* implicit letter */
                إذا (!matcher.consistentWith(c)) {
                    أرجع خطأ;
                }
            } وإلا {
                /*مکمل*/
                اكسر;
            }
        } وإلا {
            token=ٹوکن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ٹوکن[c<<8|*name++];
                --name_لمائی;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    إذا (!matcher.consistentWith(c)) {
                        أرجع خطأ;
                    }
                } وإلا {
                    /*مکمل*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                إذا (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(token_لڑیاں + token))) {
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
        أرجع U_NONCHARACTER_کوڈ_نقطہ;
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

    إذا (cat >= UPRV_LENGTHOF(char_Cat_ناں)) {
        أرجع "unknown";
    } وإلا {
        أرجع char_Cat_ناں[cat];
    }
}

ساكن uint16_t getExtName(uint32_t کوڈ, حرف *بفر, uint16_t بفر_لمائی) {
    ثابت حرف *catname = getCharCatName(کوڈ);
    uint16_t length = 0;

    UChar32 cp;
    صحيح ndigits, i;
    
    WRITE_CHAR(بفر, بفر_لمائی, length, '<');
    طالما (catname[length - 1]) {
        WRITE_CHAR(بفر, بفر_لمائی, length, catname[length - 1]);
    }
    WRITE_CHAR(بفر, بفر_لمائی, length, '-');
    لكل (cp = کوڈ, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    إذا (ndigits < 4)
        ndigits = 4;
    لكل (cp = کوڈ, i = ndigits; (cp || i > 0) && بفر_لمائی; cp >>= 4, بفر_لمائی--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        بفر[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    بفر += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(بفر, بفر_لمائی, length, '>');

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
get_گروہ(UChar_ناں *ناں, uint32_t کوڈ) {
    ثابت uint16_t *groups=GET_GROUPS(ناں);
    uint16_t گروہ_MSB_2 = static_cast<uint16_t>(کوڈ >> گروہ_کھسکاؤ),
             شروع=0,
             حد_بندی=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    طالما(شروع<حد_بندی-1) {
        number = static_cast<uint16_t>((شروع + حد_بندی) / 2);
        إذا(گروہ_MSB_2<groups[number*گروہ_لمائی+گروہ_MSB]) {
            حد_بندی=number;
        } وإلا {
            شروع=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    أرجع groups+شروع*گروہ_لمائی;
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
expand_گروہ_Lengths(ثابت uint8_t *s,
                   uint16_t offsets[سطریں_فی_گروہ+1], uint16_t lengths[سطریں_فی_گروہ+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, وتھ=0, length=0;
    uint8_t لمائی_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    طالما(i<سطریں_فی_گروہ) {
        لمائی_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        إذا(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | لمائی_Byte >> 4) + 12);
            لمائی_Byte&=0xf;
        } وإلا إذا((لمائی_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((لمائی_Byte & 0x3f) + 12);
        } وإلا {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(لمائی_Byte >> 4);
            لمائی_Byte&=0xf;
        }

        *offsets++=وتھ;
        *lengths++=length;

        وتھ+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        إذا((لمائی_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=لمائی_Byte;
            إذا(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=وتھ;
                *lengths++=length;

                وتھ+=length;
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
expand_گروہ_Name(UChar_ناں *ناں, ثابت uint16_t *گروہ,
                uint16_t سطر_نمبر, UCharNameChoice nameChoice,
                حرف *بفر, uint16_t بفر_لمائی) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(ناں) + ناں->گروہ_String_وتھ + GET_گروہ_وتھ(گروہ);
    s=expand_گروہ_Lengths(s, offsets, lengths);
    أرجع expandName(ناں, s+offsets[سطر_نمبر], lengths[سطر_نمبر], nameChoice,
                      بفر, بفر_لمائی);
}

ساكن uint16_t
getName(UChar_ناں *ناں, uint32_t کوڈ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_لمائی) {
    ثابت uint16_t *گروہ=get_گروہ(ناں, کوڈ);
    إذا (static_cast<uint16_t>(کوڈ >> گروہ_کھسکاؤ) == گروہ[گروہ_MSB]) {
        أرجع expand_گروہ_Name(ناں, گروہ, static_cast<uint16_t>(کوڈ & گروہ_نقاب), nameChoice,
                               بفر, بفر_لمائی);
    } وإلا {
        /* group not found */
        /* zero-terminate */
        إذا(بفر_لمائی>0) {
            *بفر=0;
        }
        أرجع 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ساكن UBool
enum_گروہ_ناں(UChar_ناں *ناں, ثابت uint16_t *گروہ,
               UChar32 شروع, UChar32 سرا,
               UEnum_Char_ناں_Fn *fn, فراغ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(ناں) + ناں->گروہ_String_وتھ + GET_گروہ_وتھ(گروہ);

    s=expand_گروہ_Lengths(s, offsets, lengths);
    إذا(fn!=DO_لبھو_NAME) {
        حرف بفر[200];
        uint16_t length;

        طالما(شروع<=سرا) {
            length=expandName(ناں, s+offsets[شروع&گروہ_نقاب], lengths[شروع&گروہ_نقاب], nameChoice, بفر, حجم(بفر));
            إذا (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                بفر[length = getExtName(شروع, بفر, حجم(بفر))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروع, nameChoice, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروع;
        }
    } وإلا {
        ثابت CharacterNameQuery& otherName = *static_cast<لبھو_Name*>(context)->otherName;
        طالما(شروع<=سرا) {
            إذا(compareName(ناں, s+offsets[شروع&گروہ_نقاب], lengths[شروع&گروہ_نقاب], nameChoice, otherName)) {
                static_cast<لبھو_Name*>(context)->کوڈ = شروع;
                أرجع خطأ;
            }
            ++شروع;
        }
    }
    أرجع سچ;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ساكن UBool
enum_Ext_ناں(UChar32 شروع, UChar32 سرا,
             UEnum_Char_ناں_Fn *fn, فراغ *context)
{
    إذا(fn!=DO_لبھو_NAME) {
        حرف بفر[200];
        uint16_t length;
        
        طالما(شروع<=سرا) {
            بفر[length = getExtName(شروع, بفر, حجم(بفر))] = 0;
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروع, U_EXTENDED_CHAR_NAME, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروع;
        }
    }

    أرجع سچ;
}

ساكن UBool
enum_ناں(UChar_ناں *ناں,
          UChar32 شروع, UChar32 حد_بندی,
          UEnum_Char_ناں_Fn *fn, فراغ *context,
          UCharNameChoice nameChoice) {
    uint16_t شروع_گروہ_MSB, سرا_گروہ_MSB, گروہ_گنتی;
    ثابت uint16_t *گروہ, *گروہ_حد_بندی;

    شروع_گروہ_MSB = static_cast<uint16_t>(شروع >> گروہ_کھسکاؤ);
    سرا_گروہ_MSB = static_cast<uint16_t>((حد_بندی - 1) >> گروہ_کھسکاؤ);

    /* find the group that contains start, or the highest before it */
    گروہ=get_گروہ(ناں, شروع);

    إذا(شروع_گروہ_MSB<گروہ[گروہ_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_حد_بندی = static_cast<UChar32>(گروہ[گروہ_MSB]) << گروہ_کھسکاؤ;
        إذا(ext_حد_بندی>حد_بندی) {
            ext_حد_بندی=حد_بندی;
        }
        إذا(!enum_Ext_ناں(شروع, ext_حد_بندی-1, fn, context)) {
            أرجع خطأ;
        }
        شروع=ext_حد_بندی;
    }

    إذا(شروع_گروہ_MSB==سرا_گروہ_MSB) {
        إذا(شروع_گروہ_MSB==گروہ[گروہ_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            أرجع enum_گروہ_ناں(ناں, گروہ, شروع, حد_بندی-1, fn, context, nameChoice);
        }
    } وإلا {
        ثابت uint16_t *groups=GET_GROUPS(ناں);
        گروہ_گنتی=*groups++;
        گروہ_حد_بندی=groups+گروہ_گنتی*گروہ_لمائی;

        إذا(شروع_گروہ_MSB==گروہ[گروہ_MSB]) {
            /* enumerate characters in the partial start group */
            إذا((شروع&گروہ_نقاب)!=0) {
                إذا(!enum_گروہ_ناں(ناں, گروہ,
                                   شروع, (static_cast<UChar32>(شروع_گروہ_MSB) << گروہ_کھسکاؤ) + سطریں_فی_گروہ - 1,
                                   fn, context, nameChoice)) {
                    أرجع خطأ;
                }
                گروہ=اگلا_گروہ(گروہ); /* continue with the next group */
            }
        } وإلا إذا(شروع_گروہ_MSB>گروہ[گروہ_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *اگلا_گروہ_2=اگلا_گروہ(گروہ);
            إذا (اگلا_گروہ_2 < گروہ_حد_بندی && اگلا_گروہ_2[گروہ_MSB] > شروع_گروہ_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 سرا = اگلا_گروہ_2[گروہ_MSB] << گروہ_کھسکاؤ;
                إذا (سرا > حد_بندی) {
                    سرا = حد_بندی;
                }
                إذا (!enum_Ext_ناں(شروع, سرا - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروہ=اگلا_گروہ_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        طالما(گروہ<گروہ_حد_بندی && گروہ[گروہ_MSB]<سرا_گروہ_MSB) {
            ثابت uint16_t *اگلا_گروہ_2;
            شروع = static_cast<UChar32>(گروہ[گروہ_MSB]) << گروہ_کھسکاؤ;
            إذا(!enum_گروہ_ناں(ناں, گروہ, شروع, شروع+سطریں_فی_گروہ-1, fn, context, nameChoice)) {
                أرجع خطأ;
            }
            اگلا_گروہ_2=اگلا_گروہ(گروہ);
            إذا (اگلا_گروہ_2 < گروہ_حد_بندی && اگلا_گروہ_2[گروہ_MSB] > گروہ[گروہ_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 سرا = اگلا_گروہ_2[گروہ_MSB] << گروہ_کھسکاؤ;
                إذا (سرا > حد_بندی) {
                    سرا = حد_بندی;
                }
                إذا (!enum_Ext_ناں((گروہ[گروہ_MSB] + 1) << گروہ_کھسکاؤ, سرا - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروہ=اگلا_گروہ_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        إذا(گروہ<گروہ_حد_بندی && گروہ[گروہ_MSB]==سرا_گروہ_MSB) {
            أرجع enum_گروہ_ناں(ناں, گروہ, (حد_بندی-1)&~گروہ_نقاب, حد_بندی-1, fn, context, nameChoice);
        } وإلا إذا (nameChoice == U_EXTENDED_CHAR_NAME && گروہ == گروہ_حد_بندی) {
            UChar32 next = (PREV_گروہ(گروہ)[گروہ_MSB] + 1) << گروہ_کھسکاؤ;
            إذا (next > شروع) {
                شروع = next;
            }
        } وإلا {
            أرجع سچ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
        إذا (حد_بندی > UCHAR_MAX_قدر + 1) {
            حد_بندی = UCHAR_MAX_قدر + 1;
        }
        أرجع enum_Ext_ناں(شروع, حد_بندی - 1, fn, context);
    }
    
    أرجع سچ;
}

ساكن uint16_t
write_ضارب_Suffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t کوڈ,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *بفر, uint16_t بفر_لمائی) {
    uint16_t i, ضارب, بفر_Pos=0;
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
        ضارب=factors[i];
        indexes[i] = static_cast<uint16_t>(کوڈ % ضارب);
        کوڈ/=ضارب;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(کوڈ);

    /* write each element */
    لكل(;;) {
        إذا(elementBases!=خالی_سوچک) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ضارب=indexes[i];
        طالما(ضارب>0) {
            طالما(*s++!=0) {}
            --ضارب;
        }
        إذا(elements!=خالی_سوچک) {
            *elements++=s;
        }

        /* write element */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        إذا(i>=count) {
            اكسر;
        }

        /* skip the rest of the strings for this factors[i] */
        ضارب = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        طالما(ضارب>0) {
            طالما(*s++!=0) {}
            --ضارب;
        }

        ++i;
    }

    /* zero-terminate */
    إذا(بفر_لمائی>0) {
        *بفر=0;
    }

    أرجع بفر_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ساكن uint16_t
getAlgName(AlgorithmicRange *range, uint32_t کوڈ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_لمائی) {
    uint16_t بفر_Pos=0;

    /* Only the normative character name can be algorithmic. */
    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        إذا(بفر_لمائی>0) {
            *بفر=0;
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
            WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        إذا(count<بفر_لمائی) {
            بفر[count]=0;
        }

        لكل(i=count; i>0;) {
            إذا(--i<بفر_لمائی) {
                c = static_cast<حرف>(کوڈ & 0xf);
                إذا(c<10) {
                    c+='0';
                } وإلا {
                    c+='A'-10;
                }
                بفر[i]=c;
            }
            کوڈ>>=4;
        }

        بفر_Pos+=count;
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
            WRITE_CHAR(بفر, بفر_لمائی, بفر_Pos, c);
        }

        بفر_Pos+=write_ضارب_Suffix(factors, count,
                                     s, کوڈ-range->شروع, indexes, خالی_سوچک, خالی_سوچک, بفر, بفر_لمائی);
        اكسر;
    }
    افتراضي:
        /* undefined type */
        /* zero-terminate */
        إذا(بفر_لمائی>0) {
            *بفر=0;
        }
        اكسر;
    }

    أرجع بفر_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ساكن UBool
enum_Alg_ناں(AlgorithmicRange *range,
             UChar32 شروع, UChar32 حد_بندی,
             UEnum_Char_ناں_Fn *fn, فراغ *context,
             UCharNameChoice nameChoice) {
    حرف بفر[200];
    uint16_t length;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع سچ;
    }

    بدل(range->type) {
    حالة 0: {
        حرف *s, *سرا;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(شروع), nameChoice, بفر, حجم(بفر));
        إذا(length<=0) {
            أرجع سچ;
        }

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروع, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* go to the end of the name; all these names have the same length */
        سرا=بفر;
        طالما(*سرا!=0) {
            ++سرا;
        }

        /* enumerate the rest of the names */
        طالما(++شروع<حد_بندی) {
            /* increment the hexadecimal number on a character-basis */
            s=سرا;
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

            إذا(!fn(context, شروع, nameChoice, بفر, length)) {
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
        uint16_t اگیتر_لمائی, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=بفر;
        اگیتر_لمائی=0;
        طالما((c=*s++)!=0) {
            *suffix++=c;
            ++اگیتر_لمائی;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(اگیتر_لمائی + write_ضارب_Suffix(factors, count,
                                              s, static_cast<uint32_t>(شروع) - range->شروع,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(بفر) - اگیتر_لمائی)));

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروع, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* enumerate the rest of the names */
        طالما(++شروع<حد_بندی) {
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
            length=اگیتر_لمائی;
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                طالما((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            إذا(!fn(context, شروع, nameChoice, بفر, length)) {
                أرجع خطأ;
            }
        }
        اكسر;
    }
    افتراضي:
        /* undefined type */
        اكسر;
    }

    أرجع سچ;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ساكن UChar32
لبھو_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 کوڈ;
    خودکار matcher = query.matcher();

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
        کوڈ=0;
        لكل(حرف c : matcher.بچیا_Significant_Characters()) {
            إذا('0'<=c && c<='9') {
                کوڈ=(کوڈ<<4)|(c-'0');
            } وإلا إذا('A'<=c && c<='F') {
                کوڈ=(کوڈ<<4)|(c-'A'+10);
            } وإلا {
                أرجع 0xffff;
            }
        }

        /* does it fit into the range? */
        إذا (range->شروع <= static_cast<uint32_t>(کوڈ) && static_cast<uint32_t>(کوڈ) <= range->سرا) {
            أرجع کوڈ;
        }
        اكسر;
    }
    حالة 1: {
        حرف بفر[64];
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف *s = reinterpret_cast<ثابت حرف*>(factors + count);
        UChar32 شروع, حد_بندی;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view اگیتر = s;
        إذا (!matcher.consistentWith(اگیتر)) {
            أرجع 0xffff;
        }

        s += اگیتر.length() + 1;

        شروع = static_cast<UChar32>(range->شروع);
        حد_بندی = static_cast<UChar32>(range->سرا + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.بچیا_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ضارب_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, بفر, حجم(بفر));

        /* compare the first suffix */
        إذا(querySuffix == بفر) {
            أرجع شروع;
        }

        /* enumerate and compare the rest of the suffixes */
        طالما(++شروع<حد_بندی) {
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
            خودکار it = querySuffix.begin();
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                حرف c;
                طالما((c=*s++)!=0) {
                    إذا(it == querySuffix.سرا() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            إذا(i<99 && it == querySuffix.سرا()) {
                أرجع شروع;
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

#تعریف_کرو سیٹ_جوڑو_3(سیٹ, c) ((سیٹ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#تعریف_کرو سیٹ_شامل_اے(سیٹ, c) (((سیٹ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساكن int32_t
calc_String_سیٹ_لمائی(uint32_t سیٹ[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    طالما((c=*s++)!=0) {
        سیٹ_جوڑو_3(سیٹ, c);
        ++length;
    }
    أرجع length;
}

ساكن int32_t
calcAlgNameSetsLengths(int32_t max_Name_لمائی) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_گنتی;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_ناں) + u_Char_ناں->alg_ناں_وتھ);
    range_گنتی=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    طالما(range_گنتی>0) {
        بدل(range->type) {
        حالة 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*اگیتر*/
            length = calc_String_سیٹ_لمائی(g_Name_سیٹ, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            إذا(length>max_Name_لمائی) {
                max_Name_لمائی=length;
            }
            اكسر;
        حالة 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, ضارب, ضارب_لمائی, max_ضارب_لمائی;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calc_String_سیٹ_لمائی(g_Name_سیٹ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            لكل(i=0; i<count; ++i) {
                max_ضارب_لمائی=0;
                لكل(ضارب=factors[i]; ضارب>0; --ضارب) {
                    ضارب_لمائی=calc_String_سیٹ_لمائی(g_Name_سیٹ, s);
                    s+=ضارب_لمائی+1;
                    إذا(ضارب_لمائی>max_ضارب_لمائی) {
                        max_ضارب_لمائی=ضارب_لمائی;
                    }
                }
                length+=max_ضارب_لمائی;
            }

            إذا(length>max_Name_لمائی) {
                max_Name_لمائی=length;
            }
            اكسر;
        }
        افتراضي:
            /* unknown type */
            اكسر;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_گنتی;
    }
    أرجع max_Name_لمائی;
}

ساكن int32_t
calcExtNameSetsLengths(int32_t max_Name_لمائی) {
    int32_t i, length;

    لكل(i=0; i<UPRV_LENGTHOF(char_Cat_ناں); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_سیٹ_لمائی(g_Name_سیٹ, char_Cat_ناں[i]);
        إذا(length>max_Name_لمائی) {
            max_Name_لمائی=length;
        }
    }
    أرجع max_Name_لمائی;
}

ساكن int32_t
calc_Name_سیٹ_لمائی(ثابت uint16_t *ٹوکن, uint16_t token_گنتی, ثابت uint8_t *token_لڑیاں, int8_t *tokenLengths,
                  uint32_t سیٹ[8],
                  ثابت uint8_t **p_سطر, ثابت uint8_t *سطر_حد_بندی) {
    ثابت uint8_t *سطر=*p_سطر;
    int32_t length=0, token_لمائی;
    uint16_t c, token;

    طالما (سطر != سطر_حد_بندی && (c = *سطر++) != static_cast<uint8_t>(';')) {
        إذا(c>=token_گنتی) {
            /* implicit letter */
            سیٹ_جوڑو_3(سیٹ, c);
            ++length;
        } وإلا {
            token=ٹوکن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*سطر++;
                token=ٹوکن[c];
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                سیٹ_جوڑو_3(سیٹ, c);
                ++length;
            } وإلا {
                /* count token word */
                إذا(tokenLengths!=خالی_سوچک) {
                    /* use cached token length */
                    token_لمائی=tokenLengths[c];
                    إذا(token_لمائی==0) {
                        token_لمائی = calc_String_سیٹ_لمائی(سیٹ, reinterpret_cast<ثابت حرف*>(token_لڑیاں) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_لمائی);
                    }
                } وإلا {
                    token_لمائی = calc_String_سیٹ_لمائی(سیٹ, reinterpret_cast<ثابت حرف*>(token_لڑیاں) + token);
                }
                length+=token_لمائی;
            }
        }
    }

    *p_سطر=سطر;
    أرجع length;
}

ساكن فراغ
calc_گروہ_Name_Sets_Lengths(int32_t max_Name_لمائی) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];

    uint16_t* ٹوکن = reinterpret_cast<uint16_t*>(u_Char_ناں) + 8;
    uint16_t token_گنتی=*ٹوکن++;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(u_Char_ناں) + u_Char_ناں->token_String_وتھ;

    int8_t *tokenLengths;

    ثابت uint16_t *گروہ;
    ثابت uint8_t *s, *سطر, *سطر_حد_بندی;

    int32_t گروہ_گنتی, سطر_نمبر, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_گنتی));
    إذا(tokenLengths!=خالی_سوچک) {
        uprv_memset(tokenLengths, 0, token_گنتی);
    }

    گروہ=GET_GROUPS(u_Char_ناں);
    گروہ_گنتی=*گروہ++;

    /* enumerate all groups */
    طالما(گروہ_گنتی>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_ناں) + u_Char_ناں->گروہ_String_وتھ + GET_گروہ_وتھ(گروہ);
        s=expand_گروہ_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        لكل(سطر_نمبر=0; سطر_نمبر<سطریں_فی_گروہ; ++سطر_نمبر) {
            سطر=s+offsets[سطر_نمبر];
            length=lengths[سطر_نمبر];
            إذا(length==0) {
                استمر;
            }

            سطر_حد_بندی=سطر+length;

            /* read regular name */
            length=calc_Name_سیٹ_لمائی(ٹوکن, token_گنتی, token_لڑیاں, tokenLengths, g_Name_سیٹ, &سطر, سطر_حد_بندی);
            إذا(length>max_Name_لمائی) {
                max_Name_لمائی=length;
            }
            إذا(سطر==سطر_حد_بندی) {
                استمر;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_سیٹ_لمائی(ٹوکن, token_گنتی, token_لڑیاں, tokenLengths, g_Name_سیٹ, &سطر, سطر_حد_بندی);
            إذا(length>max_Name_لمائی) {
                max_Name_لمائی=length;
            }
            إذا(سطر==سطر_حد_بندی) {
                استمر;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        گروہ=اگلا_گروہ(گروہ);
        --گروہ_گنتی;
    }

    إذا(tokenLengths!=خالی_سوچک) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_لمائی=max_Name_لمائی;
}

ساكن UBool
calcNameSetsLengths(UError_کوڈ *p_Error_کوڈ) {
    ساكن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_لمائی;

    إذا(g_Max_Name_لمائی!=0) {
        أرجع سچ;
    }

    إذا(!is_انکڑے_Loaded(p_Error_کوڈ)) {
        أرجع خطأ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    لكل (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        سیٹ_جوڑو_3(g_Name_سیٹ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_لمائی=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_لمائی=calcExtNameSetsLengths(max_Name_لمائی);

    /* set sets and lengths from group names, set global maximum values */
    calc_گروہ_Name_Sets_Lengths(max_Name_لمائی);

    أرجع سچ;
}

U_NAMESPACE_سرا

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 کوڈ, UCharNameChoice nameChoice,
           حرف *بفر, int32_t بفر_لمائی,
           UError_کوڈ *p_Error_کوڈ) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    } وإلا إذا(nameChoice>=U_CHAR_NAME_CHOICE_گنتی ||
              بفر_لمائی<0 || (بفر_لمائی>0 && بفر==خالی_سوچک)
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا((uint32_t)کوڈ>UCHAR_MAX_قدر || !is_انکڑے_Loaded(p_Error_کوڈ)) {
        أرجع u_terminateChars(بفر, بفر_لمائی, 0, p_Error_کوڈ);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_ناں+u_Char_ناں->alg_ناں_وتھ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا(algRange->شروع<=(uint32_t)کوڈ && (uint32_t)کوڈ<=algRange->سرا) {
            length=getAlgName(algRange, (uint32_t)کوڈ, nameChoice, بفر, (uint16_t)بفر_لمائی);
            اكسر;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    إذا(i==0) {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_ناں, (uint32_t )کوڈ, U_EXTENDED_CHAR_NAME, بفر, (uint16_t) بفر_لمائی);
            إذا (!length) {
                /* extended character name */
                length = getExtName((uint32_t) کوڈ, بفر, (uint16_t) بفر_لمائی);
            }
        } وإلا {
            /* normal character name */
            length=getName(u_Char_ناں, (uint32_t)کوڈ, nameChoice, بفر, (uint16_t)بفر_لمائی);
        }
    }

    أرجع u_terminateChars(بفر, بفر_لمائی, length, p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t dest_سمرتھا,
                UError_کوڈ *p_Error_کوڈ) {
    /* check the argument values */
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    } وإلا إذا(dest_سمرتھا<0 || (dest_سمرتھا>0 && dest==خالی_سوچک)) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    أرجع u_terminateChars(dest, dest_سمرتھا, 0, p_Error_کوڈ);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UError_کوڈ *p_Error_کوڈ) {
    حرف ہیٹھلا[120] = {0};
    لبھو_Name لبھو_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساكن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع error;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_گنتی || name==خالی_سوچک || *name==0) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع error;
    }

    إذا(!is_انکڑے_Loaded(p_Error_کوڈ)) {
        أرجع error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    لكل(i=0; i<حجم(ہیٹھلا); ++i) {
        إذا((c0=*nameChar++)!=0) {
            ہیٹھلا[i]=uprv_tolower(c0);
        } وإلا {
            اكسر;
        }
    }
    إذا(i==حجم(ہیٹھلا)) {
        /* name too long, there is no such character */
        *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
        أرجع error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    إذا (ہیٹھلا[0] == '<') {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME && ہیٹھلا[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t حد_بندی = i;
            طالما (i >= 3 && ہیٹھلا[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_لمائی = حد_بندی - (i + 1);
            إذا (i >= 2 && ہیٹھلا[i] == '-' && 1 <= hex_لمائی && hex_لمائی <= 8) {
                uint32_t cIdx;

                ہیٹھلا[i] = 0;

                لكل (++i; i < حد_بندی; ++i) {
                    إذا (ہیٹھلا[i] >= '0' && ہیٹھلا[i] <= '9') {
                        cp = (cp << 4) + ہیٹھلا[i] - '0';
                    } وإلا إذا (ہیٹھلا[i] >= 'a' && ہیٹھلا[i] <= 'f') {
                        cp = (cp << 4) + ہیٹھلا[i] - 'a' + 10;
                    } وإلا {
                        *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
                        أرجع error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    إذا (cp > UCHAR_MAX_قدر) {
                        *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
                        أرجع error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                لكل (ہیٹھلا[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_ناں); ++cIdx) {

                    إذا (!uprv_strcmp(ہیٹھلا + 1, char_Cat_ناں[cIdx])) {
                        إذا (cat == cIdx) {
                            أرجع cp;
                        }
                        اكسر;
                    }
                }
            }
        }

        *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
        أرجع error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_ناں+u_Char_ناں->alg_ناں_وتھ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا((cp=لبھو_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            أرجع cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    لبھو_Name_2.otherName=&query;
    لبھو_Name_2.کوڈ=error;
    enum_ناں(u_Char_ناں, 0, UCHAR_MAX_قدر + 1, DO_لبھو_NAME, &لبھو_Name_2, nameChoice);
    إذا (لبھو_Name_2.کوڈ == error) {
         *p_Error_کوڈ = U_ILLEGAL_CHAR_لبھیا;
    }
    أرجع لبھو_Name_2.کوڈ;
}

U_CAPI فراغ U_EXPORT2
u_enum_Char_ناں(UChar32 شروع, UChar32 حد_بندی,
                UEnum_Char_ناں_Fn *fn,
                فراغ *context,
                UCharNameChoice nameChoice,
                UError_کوڈ *p_Error_کوڈ) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_گنتی || fn==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا((uint32_t) حد_بندی > UCHAR_MAX_قدر + 1) {
        حد_بندی = UCHAR_MAX_قدر + 1;
    }
    إذا((uint32_t)شروع>=(uint32_t)حد_بندی) {
        أرجع;
    }

    إذا(!is_انکڑے_Loaded(p_Error_کوڈ)) {
        أرجع;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_ناں+u_Char_ناں->alg_ناں_وتھ);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        إذا((uint32_t)شروع<algRange->شروع) {
            إذا((uint32_t)حد_بندی<=algRange->شروع) {
                enum_ناں(u_Char_ناں, شروع, حد_بندی, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_ناں(u_Char_ناں, شروع, (UChar32)algRange->شروع, fn, context, nameChoice)) {
                أرجع;
            }
            شروع=(UChar32)algRange->شروع;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        إذا((uint32_t)شروع<=algRange->سرا) {
            إذا((uint32_t)حد_بندی<=(algRange->سرا+1)) {
                enum_Alg_ناں(algRange, شروع, حد_بندی, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_Alg_ناں(algRange, شروع, (UChar32)algRange->سرا+1, fn, context, nameChoice)) {
                أرجع;
            }
            شروع=(UChar32)algRange->سرا+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_ناں(u_Char_ناں, شروع, حد_بندی, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_لمائی() {
    UError_کوڈ error_کوڈ=U_ZERO_ERROR;
    إذا(calcNameSetsLengths(&error_کوڈ)) {
        أرجع g_Max_Name_لمائی;
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
char_سیٹ_منزل_USet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UError_کوڈ error_کوڈ;

    error_کوڈ=U_ZERO_ERROR;

    إذا(!calcNameSetsLengths(&error_کوڈ)) {
        أرجع;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    لكل(i=0; i<256; ++i) {
        إذا(سیٹ_شامل_اے(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_منزل_UChars(cs, us, length);

    /* add each char16_t to the USet */
    لكل(i=0; i<length; ++i) {
        إذا(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->جوڑو(sa->سیٹ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI فراغ U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
    char_سیٹ_منزل_USet(g_Name_سیٹ, sa);
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
             int16_t ٹوکن[], uint16_t token_گنتی,
             uint8_t map[256],
             UError_کوڈ *p_Error_کوڈ) {
    UBool ورتیا_باہرلا_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    إذا(ds->وچ_Charset==ds->باہرلا_Charset) {
        /* Same charset family: identity permutation */
        لكل(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وإلا {
        uprv_memset(map, 0, 256);
        uprv_memset(ورتیا_باہرلا_Char, 0, 256);

        إذا(token_گنتی>256) {
            token_گنتی=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        لكل(i=1; i<token_گنتی; ++i) {
            إذا(ٹوکن[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->وٹاندرا_Inv_Chars(ds, &c1, 1, &c2, p_Error_کوڈ);
                إذا(U_FAILURE(*p_Error_کوڈ)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->وچ_Charset);
                    أرجع;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                ورتیا_باہرلا_Char[c2]=سچ;
            }
        }

        /* set the mappings for the rest of the permutation */
        لكل(i=j=1; i<token_گنتی; ++i) {
            /* set mappings that were not set for direct bytes */
            إذا(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                طالما(ورتیا_باہرلا_Char[j]) {
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
uchar_وٹاندرا_ناں(ثابت UDataSwapper *ds,
                ثابت فراغ *وچ_انکڑے, int32_t length, فراغ *باہرلا_انکڑے,
                UError_کوڈ *p_Error_کوڈ) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_ناپ;

    ثابت uint8_t *وچ_بائٹاں;
    uint8_t *باہرلا_بائٹاں;

    uint32_t token_String_وتھ, groups_وتھ, گروہ_String_وتھ, alg_ناں_وتھ,
             وتھ, i, count, لڑیاں_گنتی;

    ثابت AlgorithmicRange *وچ_Range;
    AlgorithmicRange *باہرلا_Range;

    /* udata_swapDataHeader checks the arguments */
    سرخی_ناپ=udata_وٹاندرا_انکڑے_سرخی(ds, وچ_انکڑے, length, باہرلا_انکڑے, p_Error_کوڈ);
    إذا(p_Error_کوڈ==خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)وچ_انکڑے+4);
    إذا(!(
        pInfo->انکڑے_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->انکڑے_Format[1]==0x6e &&
        pInfo->انکڑے_Format[2]==0x61 &&
        pInfo->انکڑے_Format[3]==0x6d &&
        pInfo->format_نسخہ[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->انکڑے_Format[0], pInfo->انکڑے_Format[1],
                         pInfo->انکڑے_Format[2], pInfo->انکڑے_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    وچ_بائٹاں=(ثابت uint8_t *)وچ_انکڑے+سرخی_ناپ;
    باہرلا_بائٹاں=(باہرلا_انکڑے == خالی_سوچک) ? خالی_سوچک : (uint8_t *)باہرلا_انکڑے+سرخی_ناپ;
    إذا(length<0) {
        alg_ناں_وتھ=ds->readUInt32(((ثابت uint32_t *)وچ_بائٹاں)[3]);
    } وإلا {
        length-=سرخی_ناپ;
        إذا( length<20 ||
            (uint32_t)length<(alg_ناں_وتھ=ds->readUInt32(((ثابت uint32_t *)وچ_بائٹاں)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        وتھ=alg_ناں_وتھ;
        count=ds->readUInt32(*((ثابت uint32_t *)(وچ_بائٹاں+وتھ)));
        وتھ+=4;

        لكل(i=0; i<count; ++i) {
            وچ_Range=(ثابت AlgorithmicRange *)(وچ_بائٹاں+وتھ);
            وتھ+=ds->readUInt16(وچ_Range->size);
        }
    } وإلا {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *عارضی;

        int16_t ٹوکن[512];
        uint16_t token_گنتی;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        إذا(وچ_بائٹاں!=باہرلا_بائٹاں) {
            uprv_memcpy(باہرلا_بائٹاں, وچ_بائٹاں, length);
        }

        /* the initial 4 offsets first */
        token_String_وتھ=ds->readUInt32(((ثابت uint32_t *)وچ_بائٹاں)[0]);
        groups_وتھ=ds->readUInt32(((ثابت uint32_t *)وچ_بائٹاں)[1]);
        گروہ_String_وتھ=ds->readUInt32(((ثابت uint32_t *)وچ_بائٹاں)[2]);
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں, 16, باہرلا_بائٹاں, p_Error_کوڈ);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(وچ_بائٹاں+16);
        q=(uint16_t *)(باہرلا_بائٹاں+16);

        /* read and swap the tokenCount */
        token_گنتی=ds->readUInt16(*p);
        ds->وٹاندرا_Array16(ds, p, 2, q, p_Error_کوڈ);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        إذا(token_گنتی<=512) {
            count=token_گنتی;
        } وإلا {
            count=512;
        }
        لكل(i=0; i<count; ++i) {
            ٹوکن[i]=udata_readInt16(ds, p[i]);
        }
        لكل(; i<512; ++i) {
            ٹوکن[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, ٹوکن, token_گنتی, map, p_Error_کوڈ);
        makeTokenMap(ds, ٹوکن+256, (uint16_t)(token_گنتی>256 ? token_گنتی-256 : 0), trailMap, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        عارضی=(uint16_t *)uprv_malloc(token_گنتی*2);
        إذا(عارضی==خالی_سوچک) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_گنتی);
            *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
            أرجع 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        لكل(i=0; i<token_گنتی && i<256; ++i) {
            ds->وٹاندرا_Array16(ds, p+i, 2, عارضی+map[i], p_Error_کوڈ);
        }

        /* swap and permutate trail-byte tokens */
        لكل(; i<token_گنتی; ++i) {
            ds->وٹاندرا_Array16(ds, p+i, 2, عارضی+(i&0xffffff00)+trailMap[i&0xff], p_Error_کوڈ);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, عارضی, token_گنتی*2);
        uprv_free(عارضی);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_وٹاندرا_Inv_String_بلاک(ds, وچ_بائٹاں+token_String_وتھ, (int32_t)(groups_وتھ-token_String_وتھ),
                                    باہرلا_بائٹاں+token_String_وتھ, p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            أرجع 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(وچ_بائٹاں+groups_وتھ)));
        ds->وٹاندرا_Array16(ds, وچ_بائٹاں+groups_وتھ, (int32_t)((1+count*3)*2),
                           باہرلا_بائٹاں+groups_وتھ, p_Error_کوڈ);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        إذا(ds->وچ_Charset!=ds->باہرلا_Charset) {
            uint16_t offsets[سطریں_فی_گروہ+1], lengths[سطریں_فی_گروہ+1];

            ثابت uint8_t *وچ_لڑیاں, *اگلا_وچ_لڑیاں;
            uint8_t *باہرلا_لڑیاں;

            uint8_t c;

            وچ_لڑیاں=وچ_بائٹاں+گروہ_String_وتھ;
            باہرلا_لڑیاں=باہرلا_بائٹاں+گروہ_String_وتھ;

            لڑیاں_گنتی=alg_ناں_وتھ-گروہ_String_وتھ;

            /* iterate through string groups until only a few padding bytes are left */
            طالما(لڑیاں_گنتی>32) {
                اگلا_وچ_لڑیاں=expand_گروہ_Lengths(وچ_لڑیاں, offsets, lengths);

                /* move past the length bytes */
                لڑیاں_گنتی-=(uint32_t)(اگلا_وچ_لڑیاں-وچ_لڑیاں);
                باہرلا_لڑیاں+=اگلا_وچ_لڑیاں-وچ_لڑیاں;
                وچ_لڑیاں=اگلا_وچ_لڑیاں;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                لڑیاں_گنتی-=count;

                /* swap the string bytes using map[] and trailMap[] */
                طالما(count>0) {
                    c=*وچ_لڑیاں++;
                    *باہرلا_لڑیاں++=map[c];
                    إذا(ٹوکن[c]!=-2) {
                        --count;
                    } وإلا {
                        /* token lead byte: swap the trail byte, too */
                        *باہرلا_لڑیاں++=trailMap[*وچ_لڑیاں++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        وتھ=alg_ناں_وتھ;
        count=ds->readUInt32(*((ثابت uint32_t *)(وچ_بائٹاں+وتھ)));
        ds->وٹاندرا_Array32(ds, وچ_بائٹاں+وتھ, 4, باہرلا_بائٹاں+وتھ, p_Error_کوڈ);
        وتھ+=4;

        لكل(i=0; i<count; ++i) {
            إذا(وتھ>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_کوڈ=U_اشاریہ_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            وچ_Range=(ثابت AlgorithmicRange *)(وچ_بائٹاں+وتھ);
            باہرلا_Range=(AlgorithmicRange *)(باہرلا_بائٹاں+وتھ);
            وتھ+=ds->readUInt16(وچ_Range->size);

            ds->وٹاندرا_Array32(ds, وچ_Range, 8, باہرلا_Range, p_Error_کوڈ);
            ds->وٹاندرا_Array16(ds, &وچ_Range->size, 2, &باہرلا_Range->size, p_Error_کوڈ);
            بدل(وچ_Range->type) {
            حالة 0:
                /* swap prefix string */
                ds->وٹاندرا_Inv_Chars(ds, وچ_Range+1, (int32_t)uprv_strlen((ثابت حرف *)(وچ_Range+1)),
                                    باہرلا_Range+1, p_Error_کوڈ);
                إذا(U_FAILURE(*p_Error_کوڈ)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    أرجع 0;
                }
                اكسر;
            حالة 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_گنتی;

                    factors_گنتی=وچ_Range->variant;
                    p=(ثابت uint16_t *)(وچ_Range+1);
                    q=(uint16_t *)(باہرلا_Range+1);
                    ds->وٹاندرا_Array16(ds, p, (int32_t)(factors_گنتی*2), q, p_Error_کوڈ);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_گنتی;
                    q+=factors_گنتی;
                    لڑیاں_گنتی=(uint32_t)((وچ_بائٹاں+وتھ)-(ثابت uint8_t *)p);
                    طالما(لڑیاں_گنتی>0 && ((ثابت uint8_t *)p)[لڑیاں_گنتی-1]!=0) {
                        --لڑیاں_گنتی;
                    }
                    ds->وٹاندرا_Inv_Chars(ds, p, (int32_t)لڑیاں_گنتی, q, p_Error_کوڈ);
                }
                اكسر;
            افتراضي:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 وچ_Range->type, i);
                *p_Error_کوڈ=U_UNSUPPORTED_ERROR;
                أرجع 0;
            }
        }
    }

    أرجع سرخی_ناپ+(int32_t)وتھ;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
