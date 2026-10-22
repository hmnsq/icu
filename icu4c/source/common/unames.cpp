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

ساكن ثابت حرف معلومات_NAME[] = "unames";
ساكن ثابت حرف معلومات_TYPE[] = "icu";

#تعریف_کړه ډله_ښويدنه 5
#تعریف_کړه کرښې_فی_ډله (1L<<ډله_ښويدنه)
#تعریف_کړه ډله_ماسک (کرښې_فی_ډله-1)

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
    ډله_MSB,
    ډله_دننه_واټن_لوړ,
    ډله_دننه_واټن_ټیټ,
    ډله_اوږدتیا
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#تعریف_کړه GET_ډله_دننه_واټن(ډله) ((int32_t)(ډله)[ډله_دننه_واټن_لوړ]<<16|(ډله)[ډله_دننه_واټن_ټیټ])

#تعریف_کړه بل_ډله(ډله) ((ډله)+ډله_اوږدتیا)
#تعریف_کړه PREV_ډله(ډله) ((ډله)-ډله_اوږدتیا)

تعريفنوع بنية {
    uint32_t پیل, پای_ټکی;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعريفنوع بنية {
    uint32_t token_String_دننه_واټن, groups_دننه_واټن, ډله_String_دننه_واټن, alg_نومونه_دننه_واټن;
} UChar_نومونه;

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

            إذا (skeletonIterator == query.skeleton.پای_ټکی()) {
                أرجع خطأ;
            }
            إذا (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                إذا (وروستی_Char == '-') {
                    إذا (*skeletonIterator++ != وروستی_Char) {
                        أرجع خطأ;
                    }
                }
                وروستی_Char = c;
                أرجع ریښتیا;
            } وإلا إذا (c == '-') {
                إذا (وروستی_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.پای_ټکی() - 2)) {
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
                وروستی_Char = c;
                أرجع ریښتیا;
            } وإلا {
                إذا (*skeletonIterator++ != c) {
                    أرجع خطأ;
                }
                وروستی_Char = c;
                أرجع ریښتیا;
            }
        }

        منطقي consistentWith(ثابت std::string_view substring) {
            لكل (ثابت حرف c : substring) {
                إذا (!consistentWith(c)) {
                    أرجع خطأ;
                }
            }
            أرجع ریښتیا;
        }

        منطقي matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(وروستی_Char != '-');
            أرجع skeletonIterator == query.skeleton.پای_ټکی();
        }

        std::string_view پاتې_شوی_Significant_Characters() ثابت {
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
        حرف وروستی_Char = ' ';
    };

    څرګند CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeleton_حد = skeleton_معلومات;
        لكل (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_حد < skeleton_معلومات + حجم(skeleton_معلومات));
            إذا (skeleton_حد >= skeleton_معلومات + حجم(skeleton_معلومات)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_حد = skeleton_معلومات;
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
                    std::string_view(skeleton_معلومات, skeleton_حد - skeleton_معلومات) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = ریښتیا;
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
                    *skeleton_حد++ = query[i];
                }
            } وإلا {
                *skeleton_حد++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_معلومات, skeleton_حد - skeleton_معلومات);
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

    حرف skeleton_معلومات[120];
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
#تعریف_کړه GET_GROUPS(نومونه) (ثابت uint16_t *)((ثابت حرف *)نومونه+نومونه->groups_دننه_واټن)

تعريفنوع بنية {
    ثابت CharacterNameQuery *otherName;
    UChar32 کوډ;
} پیدا_کړه_Name;

#تعریف_کړه DO_پیدا_کړه_NAME تش_اشاره

ساكن UData_حافظه *u_Char_نومونه_معلومات=تش_اشاره;
ساكن UChar_نومونه *u_Char_نومونه=تش_اشاره;
ساكن icu::UInitOnce g_Char_نومونه_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساكن int32_t g_Max_Name_اوږدتیا=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساكن uint32_t g_Name_سېټ[8]={ 0 };

#تعریف_کړه U_NONCHARACTER_کوډ_ټکی U_CHAR_CATEGORY_شمېر
#تعریف_کړه U_LEAD_SURROGATE U_CHAR_CATEGORY_شمېر + 1
#تعریف_کړه U_TRAIL_SURROGATE U_CHAR_CATEGORY_شمېر + 2

#تعریف_کړه U_CHAR_EXTENDED_CATEGORY_شمېر (U_CHAR_CATEGORY_شمېر + 3)

ساكن ثابت حرف * ثابت char_Cat_نومونه[U_CHAR_EXTENDED_CATEGORY_شمېر] = {
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
    إذا(u_Char_نومونه_معلومات) {
        udata_close(u_Char_نومونه_معلومات);
        u_Char_نومونه_معلومات = تش_اشاره;
    }
    إذا(u_Char_نومونه) {
        u_Char_نومونه = تش_اشاره;
    }
    g_Char_نومونه_Init_Once.reset();
    g_Max_Name_اوږدتیا=0;
    أرجع ریښتیا;
}

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->معلومات_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x61 &&
        pInfo->معلومات_Format[3]==0x6d &&
        pInfo->format_نسخه[0]==1;
}

ساكن فراغ U_CALLCONV
بارول_Char_نومونه(UError_کوډ &حالت) {
    U_ASSERT(u_Char_نومونه_معلومات == تش_اشاره);
    U_ASSERT(u_Char_نومونه == تش_اشاره);

    u_Char_نومونه_معلومات = udata_openChoice(تش_اشاره, معلومات_TYPE, معلومات_NAME, isAcceptable, تش_اشاره, &حالت);
    إذا(U_FAILURE(حالت)) {
        u_Char_نومونه_معلومات = تش_اشاره;
    } وإلا {
        u_Char_نومونه = (UChar_نومونه *)udata_get_حافظه(u_Char_نومونه_معلومات);
    }
    ucln_common_ثبتوونکی_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساكن UBool
is_معلومات_Loaded(UError_کوډ *p_Error_کوډ) {
    umtx_initOnce(g_Char_نومونه_Init_Once, &بارول_Char_نومونه, *p_Error_کوډ);
    أرجع U_SUCCESS(*p_Error_کوډ);
}

#تعریف_کړه WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c) UPRV_بلاک_MACRO_BEGIN { \
    إذا((بفر_اوږدتیا)>0) { \
        *(بفر)++=c; \
        --(بفر_اوږدتیا); \
    } \
    ++(بفر_Pos); \
} UPRV_بلاک_MACRO_پای_ټکی

#تعریف_کړه U_ISO_COMMENT U_CHAR_NAME_CHOICE_شمېر

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
expandName(UChar_نومونه *نومونه,
           ثابت uint8_t *name, uint16_t name_اوږدتیا, UCharNameChoice nameChoice,
           حرف *بفر, uint16_t بفر_اوږدتیا) {
    uint16_t* نښې = reinterpret_cast<uint16_t*>(نومونه) + 8;
    uint16_t token, token_شمېر=*نښې++, بفر_Pos=0;
    uint8_t* token_مزي_ګانې = reinterpret_cast<uint8_t*>(نومونه) + نومونه->token_String_دننه_واټن;
    uint8_t c;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_شمېر || نښې[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح ډګر_شاخص= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_اوږدتیا>0) {
                    --name_اوږدتیا;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--ډګر_شاخص>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_اوږدتیا=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    طالما(name_اوږدتیا>0) {
        --name_اوږدتیا;
        c=*name++;

        إذا(c>=token_شمېر) {
            إذا(c!=';') {
                /* implicit letter */
                WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
            } وإلا {
                /*پای_ته_رسېدلی*/
                اكسر;
            }
        } وإلا {
            token=نښې[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=نښې[c<<8|*name++];
                --name_اوږدتیا;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
                } وإلا {
                    /*پای_ته_رسېدلی*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                uint8_t *tokenString=token_مزي_ګانې+token;
                طالما((c=*tokenString++)!=0) {
                    WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    إذا(بفر_اوږدتیا>0) {
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
compareName(UChar_نومونه *نومونه,
            ثابت uint8_t *name, uint16_t name_اوږدتیا, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* نښې = reinterpret_cast<uint16_t*>(نومونه) + 8;
    uint16_t token, token_شمېر=*نښې++;
    uint8_t* token_مزي_ګانې = reinterpret_cast<uint8_t*>(نومونه) + نومونه->token_String_دننه_واټن;
    uint8_t c;
    خپلکاری matcher = otherName.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_شمېر || نښې[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح ډګر_شاخص= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_اوږدتیا>0) {
                    --name_اوږدتیا;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--ډګر_شاخص>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_اوږدتیا=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    طالما(name_اوږدتیا>0) {
        --name_اوږدتیا;
        c=*name++;

        إذا(c>=token_شمېر) {
            إذا(c!=';') {
                /* implicit letter */
                إذا (!matcher.consistentWith(c)) {
                    أرجع خطأ;
                }
            } وإلا {
                /*پای_ته_رسېدلی*/
                اكسر;
            }
        } وإلا {
            token=نښې[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=نښې[c<<8|*name++];
                --name_اوږدتیا;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    إذا (!matcher.consistentWith(c)) {
                        أرجع خطأ;
                    }
                } وإلا {
                    /*پای_ته_رسېدلی*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                إذا (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(token_مزي_ګانې + token))) {
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
        أرجع U_NONCHARACTER_کوډ_ټکی;
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

    إذا (cat >= UPRV_LENGTHOF(char_Cat_نومونه)) {
        أرجع "unknown";
    } وإلا {
        أرجع char_Cat_نومونه[cat];
    }
}

ساكن uint16_t getExtName(uint32_t کوډ, حرف *بفر, uint16_t بفر_اوږدتیا) {
    ثابت حرف *catname = getCharCatName(کوډ);
    uint16_t length = 0;

    UChar32 cp;
    صحيح ndigits, i;
    
    WRITE_CHAR(بفر, بفر_اوږدتیا, length, '<');
    طالما (catname[length - 1]) {
        WRITE_CHAR(بفر, بفر_اوږدتیا, length, catname[length - 1]);
    }
    WRITE_CHAR(بفر, بفر_اوږدتیا, length, '-');
    لكل (cp = کوډ, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    إذا (ndigits < 4)
        ndigits = 4;
    لكل (cp = کوډ, i = ndigits; (cp || i > 0) && بفر_اوږدتیا; cp >>= 4, بفر_اوږدتیا--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        بفر[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    بفر += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(بفر, بفر_اوږدتیا, length, '>');

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
get_ډله(UChar_نومونه *نومونه, uint32_t کوډ) {
    ثابت uint16_t *groups=GET_GROUPS(نومونه);
    uint16_t ډله_MSB_2 = static_cast<uint16_t>(کوډ >> ډله_ښويدنه),
             پیل=0,
             حد=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    طالما(پیل<حد-1) {
        number = static_cast<uint16_t>((پیل + حد) / 2);
        إذا(ډله_MSB_2<groups[number*ډله_اوږدتیا+ډله_MSB]) {
            حد=number;
        } وإلا {
            پیل=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    أرجع groups+پیل*ډله_اوږدتیا;
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
expand_ډله_Lengths(ثابت uint8_t *s,
                   uint16_t offsets[کرښې_فی_ډله+1], uint16_t lengths[کرښې_فی_ډله+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, دننه_واټن=0, length=0;
    uint8_t اوږدتیا_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    طالما(i<کرښې_فی_ډله) {
        اوږدتیا_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        إذا(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | اوږدتیا_Byte >> 4) + 12);
            اوږدتیا_Byte&=0xf;
        } وإلا إذا((اوږدتیا_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((اوږدتیا_Byte & 0x3f) + 12);
        } وإلا {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(اوږدتیا_Byte >> 4);
            اوږدتیا_Byte&=0xf;
        }

        *offsets++=دننه_واټن;
        *lengths++=length;

        دننه_واټن+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        إذا((اوږدتیا_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=اوږدتیا_Byte;
            إذا(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=دننه_واټن;
                *lengths++=length;

                دننه_واټن+=length;
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
expand_ډله_Name(UChar_نومونه *نومونه, ثابت uint16_t *ډله,
                uint16_t کرښه_شمېره, UCharNameChoice nameChoice,
                حرف *بفر, uint16_t بفر_اوږدتیا) {
    uint16_t offsets[کرښې_فی_ډله+2], lengths[کرښې_فی_ډله+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نومونه) + نومونه->ډله_String_دننه_واټن + GET_ډله_دننه_واټن(ډله);
    s=expand_ډله_Lengths(s, offsets, lengths);
    أرجع expandName(نومونه, s+offsets[کرښه_شمېره], lengths[کرښه_شمېره], nameChoice,
                      بفر, بفر_اوږدتیا);
}

ساكن uint16_t
getName(UChar_نومونه *نومونه, uint32_t کوډ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_اوږدتیا) {
    ثابت uint16_t *ډله=get_ډله(نومونه, کوډ);
    إذا (static_cast<uint16_t>(کوډ >> ډله_ښويدنه) == ډله[ډله_MSB]) {
        أرجع expand_ډله_Name(نومونه, ډله, static_cast<uint16_t>(کوډ & ډله_ماسک), nameChoice,
                               بفر, بفر_اوږدتیا);
    } وإلا {
        /* group not found */
        /* zero-terminate */
        إذا(بفر_اوږدتیا>0) {
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
enum_ډله_نومونه(UChar_نومونه *نومونه, ثابت uint16_t *ډله,
               UChar32 پیل, UChar32 پای_ټکی,
               UEnum_Char_نومونه_Fn *fn, فراغ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[کرښې_فی_ډله+2], lengths[کرښې_فی_ډله+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نومونه) + نومونه->ډله_String_دننه_واټن + GET_ډله_دننه_واټن(ډله);

    s=expand_ډله_Lengths(s, offsets, lengths);
    إذا(fn!=DO_پیدا_کړه_NAME) {
        حرف بفر[200];
        uint16_t length;

        طالما(پیل<=پای_ټکی) {
            length=expandName(نومونه, s+offsets[پیل&ډله_ماسک], lengths[پیل&ډله_ماسک], nameChoice, بفر, حجم(بفر));
            إذا (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                بفر[length = getExtName(پیل, بفر, حجم(بفر))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, پیل, nameChoice, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++پیل;
        }
    } وإلا {
        ثابت CharacterNameQuery& otherName = *static_cast<پیدا_کړه_Name*>(context)->otherName;
        طالما(پیل<=پای_ټکی) {
            إذا(compareName(نومونه, s+offsets[پیل&ډله_ماسک], lengths[پیل&ډله_ماسک], nameChoice, otherName)) {
                static_cast<پیدا_کړه_Name*>(context)->کوډ = پیل;
                أرجع خطأ;
            }
            ++پیل;
        }
    }
    أرجع ریښتیا;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ساكن UBool
enum_Ext_نومونه(UChar32 پیل, UChar32 پای_ټکی,
             UEnum_Char_نومونه_Fn *fn, فراغ *context)
{
    إذا(fn!=DO_پیدا_کړه_NAME) {
        حرف بفر[200];
        uint16_t length;
        
        طالما(پیل<=پای_ټکی) {
            بفر[length = getExtName(پیل, بفر, حجم(بفر))] = 0;
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, پیل, U_EXTENDED_CHAR_NAME, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++پیل;
        }
    }

    أرجع ریښتیا;
}

ساكن UBool
enum_نومونه(UChar_نومونه *نومونه,
          UChar32 پیل, UChar32 حد,
          UEnum_Char_نومونه_Fn *fn, فراغ *context,
          UCharNameChoice nameChoice) {
    uint16_t پیل_ډله_MSB, پای_ټکی_ډله_MSB, ډله_شمېر;
    ثابت uint16_t *ډله, *ډله_حد;

    پیل_ډله_MSB = static_cast<uint16_t>(پیل >> ډله_ښويدنه);
    پای_ټکی_ډله_MSB = static_cast<uint16_t>((حد - 1) >> ډله_ښويدنه);

    /* find the group that contains start, or the highest before it */
    ډله=get_ډله(نومونه, پیل);

    إذا(پیل_ډله_MSB<ډله[ډله_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_حد = static_cast<UChar32>(ډله[ډله_MSB]) << ډله_ښويدنه;
        إذا(ext_حد>حد) {
            ext_حد=حد;
        }
        إذا(!enum_Ext_نومونه(پیل, ext_حد-1, fn, context)) {
            أرجع خطأ;
        }
        پیل=ext_حد;
    }

    إذا(پیل_ډله_MSB==پای_ټکی_ډله_MSB) {
        إذا(پیل_ډله_MSB==ډله[ډله_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            أرجع enum_ډله_نومونه(نومونه, ډله, پیل, حد-1, fn, context, nameChoice);
        }
    } وإلا {
        ثابت uint16_t *groups=GET_GROUPS(نومونه);
        ډله_شمېر=*groups++;
        ډله_حد=groups+ډله_شمېر*ډله_اوږدتیا;

        إذا(پیل_ډله_MSB==ډله[ډله_MSB]) {
            /* enumerate characters in the partial start group */
            إذا((پیل&ډله_ماسک)!=0) {
                إذا(!enum_ډله_نومونه(نومونه, ډله,
                                   پیل, (static_cast<UChar32>(پیل_ډله_MSB) << ډله_ښويدنه) + کرښې_فی_ډله - 1,
                                   fn, context, nameChoice)) {
                    أرجع خطأ;
                }
                ډله=بل_ډله(ډله); /* continue with the next group */
            }
        } وإلا إذا(پیل_ډله_MSB>ډله[ډله_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *بل_ډله_2=بل_ډله(ډله);
            إذا (بل_ډله_2 < ډله_حد && بل_ډله_2[ډله_MSB] > پیل_ډله_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 پای_ټکی = بل_ډله_2[ډله_MSB] << ډله_ښويدنه;
                إذا (پای_ټکی > حد) {
                    پای_ټکی = حد;
                }
                إذا (!enum_Ext_نومونه(پیل, پای_ټکی - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            ډله=بل_ډله_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        طالما(ډله<ډله_حد && ډله[ډله_MSB]<پای_ټکی_ډله_MSB) {
            ثابت uint16_t *بل_ډله_2;
            پیل = static_cast<UChar32>(ډله[ډله_MSB]) << ډله_ښويدنه;
            إذا(!enum_ډله_نومونه(نومونه, ډله, پیل, پیل+کرښې_فی_ډله-1, fn, context, nameChoice)) {
                أرجع خطأ;
            }
            بل_ډله_2=بل_ډله(ډله);
            إذا (بل_ډله_2 < ډله_حد && بل_ډله_2[ډله_MSB] > ډله[ډله_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 پای_ټکی = بل_ډله_2[ډله_MSB] << ډله_ښويدنه;
                إذا (پای_ټکی > حد) {
                    پای_ټکی = حد;
                }
                إذا (!enum_Ext_نومونه((ډله[ډله_MSB] + 1) << ډله_ښويدنه, پای_ټکی - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            ډله=بل_ډله_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        إذا(ډله<ډله_حد && ډله[ډله_MSB]==پای_ټکی_ډله_MSB) {
            أرجع enum_ډله_نومونه(نومونه, ډله, (حد-1)&~ډله_ماسک, حد-1, fn, context, nameChoice);
        } وإلا إذا (nameChoice == U_EXTENDED_CHAR_NAME && ډله == ډله_حد) {
            UChar32 next = (PREV_ډله(ډله)[ډله_MSB] + 1) << ډله_ښويدنه;
            إذا (next > پیل) {
                پیل = next;
            }
        } وإلا {
            أرجع ریښتیا;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
        إذا (حد > UCHAR_MAX_ارزښت + 1) {
            حد = UCHAR_MAX_ارزښت + 1;
        }
        أرجع enum_Ext_نومونه(پیل, حد - 1, fn, context);
    }
    
    أرجع ریښتیا;
}

ساكن uint16_t
write_ضریب_Suffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t کوډ,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *بفر, uint16_t بفر_اوږدتیا) {
    uint16_t i, ضریب, بفر_Pos=0;
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
        ضریب=factors[i];
        indexes[i] = static_cast<uint16_t>(کوډ % ضریب);
        کوډ/=ضریب;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(کوډ);

    /* write each element */
    لكل(;;) {
        إذا(elementBases!=تش_اشاره) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ضریب=indexes[i];
        طالما(ضریب>0) {
            طالما(*s++!=0) {}
            --ضریب;
        }
        إذا(elements!=تش_اشاره) {
            *elements++=s;
        }

        /* write element */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        إذا(i>=count) {
            اكسر;
        }

        /* skip the rest of the strings for this factors[i] */
        ضریب = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        طالما(ضریب>0) {
            طالما(*s++!=0) {}
            --ضریب;
        }

        ++i;
    }

    /* zero-terminate */
    إذا(بفر_اوږدتیا>0) {
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
getAlgName(AlgorithmicRange *range, uint32_t کوډ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_اوږدتیا) {
    uint16_t بفر_Pos=0;

    /* Only the normative character name can be algorithmic. */
    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        إذا(بفر_اوږدتیا>0) {
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
            WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        إذا(count<بفر_اوږدتیا) {
            بفر[count]=0;
        }

        لكل(i=count; i>0;) {
            إذا(--i<بفر_اوږدتیا) {
                c = static_cast<حرف>(کوډ & 0xf);
                إذا(c<10) {
                    c+='0';
                } وإلا {
                    c+='A'-10;
                }
                بفر[i]=c;
            }
            کوډ>>=4;
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
            WRITE_CHAR(بفر, بفر_اوږدتیا, بفر_Pos, c);
        }

        بفر_Pos+=write_ضریب_Suffix(factors, count,
                                     s, کوډ-range->پیل, indexes, تش_اشاره, تش_اشاره, بفر, بفر_اوږدتیا);
        اكسر;
    }
    افتراضي:
        /* undefined type */
        /* zero-terminate */
        إذا(بفر_اوږدتیا>0) {
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
enum_Alg_نومونه(AlgorithmicRange *range,
             UChar32 پیل, UChar32 حد,
             UEnum_Char_نومونه_Fn *fn, فراغ *context,
             UCharNameChoice nameChoice) {
    حرف بفر[200];
    uint16_t length;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع ریښتیا;
    }

    بدل(range->type) {
    حالة 0: {
        حرف *s, *پای_ټکی;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(پیل), nameChoice, بفر, حجم(بفر));
        إذا(length<=0) {
            أرجع ریښتیا;
        }

        /* call the enumerator function with this first character */
        إذا(!fn(context, پیل, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* go to the end of the name; all these names have the same length */
        پای_ټکی=بفر;
        طالما(*پای_ټکی!=0) {
            ++پای_ټکی;
        }

        /* enumerate the rest of the names */
        طالما(++پیل<حد) {
            /* increment the hexadecimal number on a character-basis */
            s=پای_ټکی;
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

            إذا(!fn(context, پیل, nameChoice, بفر, length)) {
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
        uint16_t مختاړی_اوږدتیا, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=بفر;
        مختاړی_اوږدتیا=0;
        طالما((c=*s++)!=0) {
            *suffix++=c;
            ++مختاړی_اوږدتیا;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(مختاړی_اوږدتیا + write_ضریب_Suffix(factors, count,
                                              s, static_cast<uint32_t>(پیل) - range->پیل,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(بفر) - مختاړی_اوږدتیا)));

        /* call the enumerator function with this first character */
        إذا(!fn(context, پیل, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* enumerate the rest of the names */
        طالما(++پیل<حد) {
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
            length=مختاړی_اوږدتیا;
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                طالما((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            إذا(!fn(context, پیل, nameChoice, بفر, length)) {
                أرجع خطأ;
            }
        }
        اكسر;
    }
    افتراضي:
        /* undefined type */
        اكسر;
    }

    أرجع ریښتیا;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ساكن UChar32
پیدا_کړه_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 کوډ;
    خپلکاری matcher = query.matcher();

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
        کوډ=0;
        لكل(حرف c : matcher.پاتې_شوی_Significant_Characters()) {
            إذا('0'<=c && c<='9') {
                کوډ=(کوډ<<4)|(c-'0');
            } وإلا إذا('A'<=c && c<='F') {
                کوډ=(کوډ<<4)|(c-'A'+10);
            } وإلا {
                أرجع 0xffff;
            }
        }

        /* does it fit into the range? */
        إذا (range->پیل <= static_cast<uint32_t>(کوډ) && static_cast<uint32_t>(کوډ) <= range->پای_ټکی) {
            أرجع کوډ;
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
        UChar32 پیل, حد;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view مختاړی = s;
        إذا (!matcher.consistentWith(مختاړی)) {
            أرجع 0xffff;
        }

        s += مختاړی.length() + 1;

        پیل = static_cast<UChar32>(range->پیل);
        حد = static_cast<UChar32>(range->پای_ټکی + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.پاتې_شوی_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ضریب_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, بفر, حجم(بفر));

        /* compare the first suffix */
        إذا(querySuffix == بفر) {
            أرجع پیل;
        }

        /* enumerate and compare the rest of the suffixes */
        طالما(++پیل<حد) {
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
            خپلکاری it = querySuffix.begin();
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                حرف c;
                طالما((c=*s++)!=0) {
                    إذا(it == querySuffix.پای_ټکی() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            إذا(i<99 && it == querySuffix.پای_ټکی()) {
                أرجع پیل;
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

#تعریف_کړه سېټ_جمع_کړه_3(سېټ, c) ((سېټ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#تعریف_کړه سېټ_لري(سېټ, c) (((سېټ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساكن int32_t
calc_String_سېټ_اوږدتیا(uint32_t سېټ[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    طالما((c=*s++)!=0) {
        سېټ_جمع_کړه_3(سېټ, c);
        ++length;
    }
    أرجع length;
}

ساكن int32_t
calcAlgNameSetsLengths(int32_t max_Name_اوږدتیا) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_شمېر;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_نومونه) + u_Char_نومونه->alg_نومونه_دننه_واټن);
    range_شمېر=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    طالما(range_شمېر>0) {
        بدل(range->type) {
        حالة 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*مختاړی*/
            length = calc_String_سېټ_اوږدتیا(g_Name_سېټ, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            إذا(length>max_Name_اوږدتیا) {
                max_Name_اوږدتیا=length;
            }
            اكسر;
        حالة 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, ضریب, ضریب_اوږدتیا, max_ضریب_اوږدتیا;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calc_String_سېټ_اوږدتیا(g_Name_سېټ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            لكل(i=0; i<count; ++i) {
                max_ضریب_اوږدتیا=0;
                لكل(ضریب=factors[i]; ضریب>0; --ضریب) {
                    ضریب_اوږدتیا=calc_String_سېټ_اوږدتیا(g_Name_سېټ, s);
                    s+=ضریب_اوږدتیا+1;
                    إذا(ضریب_اوږدتیا>max_ضریب_اوږدتیا) {
                        max_ضریب_اوږدتیا=ضریب_اوږدتیا;
                    }
                }
                length+=max_ضریب_اوږدتیا;
            }

            إذا(length>max_Name_اوږدتیا) {
                max_Name_اوږدتیا=length;
            }
            اكسر;
        }
        افتراضي:
            /* unknown type */
            اكسر;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_شمېر;
    }
    أرجع max_Name_اوږدتیا;
}

ساكن int32_t
calcExtNameSetsLengths(int32_t max_Name_اوږدتیا) {
    int32_t i, length;

    لكل(i=0; i<UPRV_LENGTHOF(char_Cat_نومونه); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_سېټ_اوږدتیا(g_Name_سېټ, char_Cat_نومونه[i]);
        إذا(length>max_Name_اوږدتیا) {
            max_Name_اوږدتیا=length;
        }
    }
    أرجع max_Name_اوږدتیا;
}

ساكن int32_t
calc_Name_سېټ_اوږدتیا(ثابت uint16_t *نښې, uint16_t token_شمېر, ثابت uint8_t *token_مزي_ګانې, int8_t *tokenLengths,
                  uint32_t سېټ[8],
                  ثابت uint8_t **p_کرښه, ثابت uint8_t *کرښه_حد) {
    ثابت uint8_t *کرښه=*p_کرښه;
    int32_t length=0, token_اوږدتیا;
    uint16_t c, token;

    طالما (کرښه != کرښه_حد && (c = *کرښه++) != static_cast<uint8_t>(';')) {
        إذا(c>=token_شمېر) {
            /* implicit letter */
            سېټ_جمع_کړه_3(سېټ, c);
            ++length;
        } وإلا {
            token=نښې[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*کرښه++;
                token=نښې[c];
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                سېټ_جمع_کړه_3(سېټ, c);
                ++length;
            } وإلا {
                /* count token word */
                إذا(tokenLengths!=تش_اشاره) {
                    /* use cached token length */
                    token_اوږدتیا=tokenLengths[c];
                    إذا(token_اوږدتیا==0) {
                        token_اوږدتیا = calc_String_سېټ_اوږدتیا(سېټ, reinterpret_cast<ثابت حرف*>(token_مزي_ګانې) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_اوږدتیا);
                    }
                } وإلا {
                    token_اوږدتیا = calc_String_سېټ_اوږدتیا(سېټ, reinterpret_cast<ثابت حرف*>(token_مزي_ګانې) + token);
                }
                length+=token_اوږدتیا;
            }
        }
    }

    *p_کرښه=کرښه;
    أرجع length;
}

ساكن فراغ
calc_ډله_Name_Sets_Lengths(int32_t max_Name_اوږدتیا) {
    uint16_t offsets[کرښې_فی_ډله+2], lengths[کرښې_فی_ډله+2];

    uint16_t* نښې = reinterpret_cast<uint16_t*>(u_Char_نومونه) + 8;
    uint16_t token_شمېر=*نښې++;
    uint8_t* token_مزي_ګانې = reinterpret_cast<uint8_t*>(u_Char_نومونه) + u_Char_نومونه->token_String_دننه_واټن;

    int8_t *tokenLengths;

    ثابت uint16_t *ډله;
    ثابت uint8_t *s, *کرښه, *کرښه_حد;

    int32_t ډله_شمېر, کرښه_شمېره, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_شمېر));
    إذا(tokenLengths!=تش_اشاره) {
        uprv_memset(tokenLengths, 0, token_شمېر);
    }

    ډله=GET_GROUPS(u_Char_نومونه);
    ډله_شمېر=*ډله++;

    /* enumerate all groups */
    طالما(ډله_شمېر>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_نومونه) + u_Char_نومونه->ډله_String_دننه_واټن + GET_ډله_دننه_واټن(ډله);
        s=expand_ډله_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        لكل(کرښه_شمېره=0; کرښه_شمېره<کرښې_فی_ډله; ++کرښه_شمېره) {
            کرښه=s+offsets[کرښه_شمېره];
            length=lengths[کرښه_شمېره];
            إذا(length==0) {
                استمر;
            }

            کرښه_حد=کرښه+length;

            /* read regular name */
            length=calc_Name_سېټ_اوږدتیا(نښې, token_شمېر, token_مزي_ګانې, tokenLengths, g_Name_سېټ, &کرښه, کرښه_حد);
            إذا(length>max_Name_اوږدتیا) {
                max_Name_اوږدتیا=length;
            }
            إذا(کرښه==کرښه_حد) {
                استمر;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_سېټ_اوږدتیا(نښې, token_شمېر, token_مزي_ګانې, tokenLengths, g_Name_سېټ, &کرښه, کرښه_حد);
            إذا(length>max_Name_اوږدتیا) {
                max_Name_اوږدتیا=length;
            }
            إذا(کرښه==کرښه_حد) {
                استمر;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        ډله=بل_ډله(ډله);
        --ډله_شمېر;
    }

    إذا(tokenLengths!=تش_اشاره) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_اوږدتیا=max_Name_اوږدتیا;
}

ساكن UBool
calcNameSetsLengths(UError_کوډ *p_Error_کوډ) {
    ساكن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_اوږدتیا;

    إذا(g_Max_Name_اوږدتیا!=0) {
        أرجع ریښتیا;
    }

    إذا(!is_معلومات_Loaded(p_Error_کوډ)) {
        أرجع خطأ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    لكل (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        سېټ_جمع_کړه_3(g_Name_سېټ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_اوږدتیا=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_اوږدتیا=calcExtNameSetsLengths(max_Name_اوږدتیا);

    /* set sets and lengths from group names, set global maximum values */
    calc_ډله_Name_Sets_Lengths(max_Name_اوږدتیا);

    أرجع ریښتیا;
}

U_NAMESPACE_پای_ټکی

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 کوډ, UCharNameChoice nameChoice,
           حرف *بفر, int32_t بفر_اوږدتیا,
           UError_کوډ *p_Error_کوډ) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    } وإلا إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمېر ||
              بفر_اوږدتیا<0 || (بفر_اوږدتیا>0 && بفر==تش_اشاره)
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا((uint32_t)کوډ>UCHAR_MAX_ارزښت || !is_معلومات_Loaded(p_Error_کوډ)) {
        أرجع u_terminateChars(بفر, بفر_اوږدتیا, 0, p_Error_کوډ);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_نومونه+u_Char_نومونه->alg_نومونه_دننه_واټن);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا(algRange->پیل<=(uint32_t)کوډ && (uint32_t)کوډ<=algRange->پای_ټکی) {
            length=getAlgName(algRange, (uint32_t)کوډ, nameChoice, بفر, (uint16_t)بفر_اوږدتیا);
            اكسر;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    إذا(i==0) {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_نومونه, (uint32_t )کوډ, U_EXTENDED_CHAR_NAME, بفر, (uint16_t) بفر_اوږدتیا);
            إذا (!length) {
                /* extended character name */
                length = getExtName((uint32_t) کوډ, بفر, (uint16_t) بفر_اوږدتیا);
            }
        } وإلا {
            /* normal character name */
            length=getName(u_Char_نومونه, (uint32_t)کوډ, nameChoice, بفر, (uint16_t)بفر_اوږدتیا);
        }
    }

    أرجع u_terminateChars(بفر, بفر_اوږدتیا, length, p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t dest_ظرفیت,
                UError_کوډ *p_Error_کوډ) {
    /* check the argument values */
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    } وإلا إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==تش_اشاره)) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    أرجع u_terminateChars(dest, dest_ظرفیت, 0, p_Error_کوډ);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UError_کوډ *p_Error_کوډ) {
    حرف ښکتنی[120] = {0};
    پیدا_کړه_Name پیدا_کړه_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساكن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع error;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمېر || name==تش_اشاره || *name==0) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع error;
    }

    إذا(!is_معلومات_Loaded(p_Error_کوډ)) {
        أرجع error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    لكل(i=0; i<حجم(ښکتنی); ++i) {
        إذا((c0=*nameChar++)!=0) {
            ښکتنی[i]=uprv_tolower(c0);
        } وإلا {
            اكسر;
        }
    }
    إذا(i==حجم(ښکتنی)) {
        /* name too long, there is no such character */
        *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
        أرجع error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    إذا (ښکتنی[0] == '<') {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME && ښکتنی[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t حد = i;
            طالما (i >= 3 && ښکتنی[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_اوږدتیا = حد - (i + 1);
            إذا (i >= 2 && ښکتنی[i] == '-' && 1 <= hex_اوږدتیا && hex_اوږدتیا <= 8) {
                uint32_t cIdx;

                ښکتنی[i] = 0;

                لكل (++i; i < حد; ++i) {
                    إذا (ښکتنی[i] >= '0' && ښکتنی[i] <= '9') {
                        cp = (cp << 4) + ښکتنی[i] - '0';
                    } وإلا إذا (ښکتنی[i] >= 'a' && ښکتنی[i] <= 'f') {
                        cp = (cp << 4) + ښکتنی[i] - 'a' + 10;
                    } وإلا {
                        *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
                        أرجع error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    إذا (cp > UCHAR_MAX_ارزښت) {
                        *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
                        أرجع error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                لكل (ښکتنی[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_نومونه); ++cIdx) {

                    إذا (!uprv_strcmp(ښکتنی + 1, char_Cat_نومونه[cIdx])) {
                        إذا (cat == cIdx) {
                            أرجع cp;
                        }
                        اكسر;
                    }
                }
            }
        }

        *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
        أرجع error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_نومونه+u_Char_نومونه->alg_نومونه_دننه_واټن);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا((cp=پیدا_کړه_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            أرجع cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    پیدا_کړه_Name_2.otherName=&query;
    پیدا_کړه_Name_2.کوډ=error;
    enum_نومونه(u_Char_نومونه, 0, UCHAR_MAX_ارزښت + 1, DO_پیدا_کړه_NAME, &پیدا_کړه_Name_2, nameChoice);
    إذا (پیدا_کړه_Name_2.کوډ == error) {
         *p_Error_کوډ = U_ILLEGAL_CHAR_وموندل;
    }
    أرجع پیدا_کړه_Name_2.کوډ;
}

U_CAPI فراغ U_EXPORT2
u_enum_Char_نومونه(UChar32 پیل, UChar32 حد,
                UEnum_Char_نومونه_Fn *fn,
                فراغ *context,
                UCharNameChoice nameChoice,
                UError_کوډ *p_Error_کوډ) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمېر || fn==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    إذا((uint32_t) حد > UCHAR_MAX_ارزښت + 1) {
        حد = UCHAR_MAX_ارزښت + 1;
    }
    إذا((uint32_t)پیل>=(uint32_t)حد) {
        أرجع;
    }

    إذا(!is_معلومات_Loaded(p_Error_کوډ)) {
        أرجع;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_نومونه+u_Char_نومونه->alg_نومونه_دننه_واټن);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        إذا((uint32_t)پیل<algRange->پیل) {
            إذا((uint32_t)حد<=algRange->پیل) {
                enum_نومونه(u_Char_نومونه, پیل, حد, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_نومونه(u_Char_نومونه, پیل, (UChar32)algRange->پیل, fn, context, nameChoice)) {
                أرجع;
            }
            پیل=(UChar32)algRange->پیل;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        إذا((uint32_t)پیل<=algRange->پای_ټکی) {
            إذا((uint32_t)حد<=(algRange->پای_ټکی+1)) {
                enum_Alg_نومونه(algRange, پیل, حد, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_Alg_نومونه(algRange, پیل, (UChar32)algRange->پای_ټکی+1, fn, context, nameChoice)) {
                أرجع;
            }
            پیل=(UChar32)algRange->پای_ټکی+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_نومونه(u_Char_نومونه, پیل, حد, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_اوږدتیا() {
    UError_کوډ error_کوډ=U_ZERO_ERROR;
    إذا(calcNameSetsLengths(&error_کوډ)) {
        أرجع g_Max_Name_اوږدتیا;
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
char_سېټ_منزل_USet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UError_کوډ error_کوډ;

    error_کوډ=U_ZERO_ERROR;

    إذا(!calcNameSetsLengths(&error_کوډ)) {
        أرجع;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    لكل(i=0; i<256; ++i) {
        إذا(سېټ_لري(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_منزل_UChars(cs, us, length);

    /* add each char16_t to the USet */
    لكل(i=0; i<length; ++i) {
        إذا(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->جمع_کړه(sa->سېټ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI فراغ U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
    char_سېټ_منزل_USet(g_Name_سېټ, sa);
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
             int16_t نښې[], uint16_t token_شمېر,
             uint8_t map[256],
             UError_کوډ *p_Error_کوډ) {
    UBool کارول_شوی_بهر_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    إذا(ds->دننه_Charset==ds->بهر_Charset) {
        /* Same charset family: identity permutation */
        لكل(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وإلا {
        uprv_memset(map, 0, 256);
        uprv_memset(کارول_شوی_بهر_Char, 0, 256);

        إذا(token_شمېر>256) {
            token_شمېر=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        لكل(i=1; i<token_شمېر; ++i) {
            إذا(نښې[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->بدلون_Inv_Chars(ds, &c1, 1, &c2, p_Error_کوډ);
                إذا(U_FAILURE(*p_Error_کوډ)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->دننه_Charset);
                    أرجع;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                کارول_شوی_بهر_Char[c2]=ریښتیا;
            }
        }

        /* set the mappings for the rest of the permutation */
        لكل(i=j=1; i<token_شمېر; ++i) {
            /* set mappings that were not set for direct bytes */
            إذا(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                طالما(کارول_شوی_بهر_Char[j]) {
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
uchar_بدلون_نومونه(ثابت UDataSwapper *ds,
                ثابت فراغ *دننه_معلومات, int32_t length, فراغ *بهر_معلومات,
                UError_کوډ *p_Error_کوډ) {
    ثابت UDataInfo *pInfo;
    int32_t سرلیک_اندازه;

    ثابت uint8_t *دننه_بایټونه;
    uint8_t *بهر_بایټونه;

    uint32_t token_String_دننه_واټن, groups_دننه_واټن, ډله_String_دننه_واټن, alg_نومونه_دننه_واټن,
             دننه_واټن, i, count, مزي_ګانې_شمېر;

    ثابت AlgorithmicRange *دننه_Range;
    AlgorithmicRange *بهر_Range;

    /* udata_swapDataHeader checks the arguments */
    سرلیک_اندازه=udata_بدلون_معلومات_سرلیک(ds, دننه_معلومات, length, بهر_معلومات, p_Error_کوډ);
    إذا(p_Error_کوډ==تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)دننه_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x61 &&
        pInfo->معلومات_Format[3]==0x6d &&
        pInfo->format_نسخه[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کوډ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    دننه_بایټونه=(ثابت uint8_t *)دننه_معلومات+سرلیک_اندازه;
    بهر_بایټونه=(بهر_معلومات == تش_اشاره) ? تش_اشاره : (uint8_t *)بهر_معلومات+سرلیک_اندازه;
    إذا(length<0) {
        alg_نومونه_دننه_واټن=ds->readUInt32(((ثابت uint32_t *)دننه_بایټونه)[3]);
    } وإلا {
        length-=سرلیک_اندازه;
        إذا( length<20 ||
            (uint32_t)length<(alg_نومونه_دننه_واټن=ds->readUInt32(((ثابت uint32_t *)دننه_بایټونه)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        دننه_واټن=alg_نومونه_دننه_واټن;
        count=ds->readUInt32(*((ثابت uint32_t *)(دننه_بایټونه+دننه_واټن)));
        دننه_واټن+=4;

        لكل(i=0; i<count; ++i) {
            دننه_Range=(ثابت AlgorithmicRange *)(دننه_بایټونه+دننه_واټن);
            دننه_واټن+=ds->readUInt16(دننه_Range->size);
        }
    } وإلا {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *لنډمهاله;

        int16_t نښې[512];
        uint16_t token_شمېر;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        إذا(دننه_بایټونه!=بهر_بایټونه) {
            uprv_memcpy(بهر_بایټونه, دننه_بایټونه, length);
        }

        /* the initial 4 offsets first */
        token_String_دننه_واټن=ds->readUInt32(((ثابت uint32_t *)دننه_بایټونه)[0]);
        groups_دننه_واټن=ds->readUInt32(((ثابت uint32_t *)دننه_بایټونه)[1]);
        ډله_String_دننه_واټن=ds->readUInt32(((ثابت uint32_t *)دننه_بایټونه)[2]);
        ds->بدلون_Array32(ds, دننه_بایټونه, 16, بهر_بایټونه, p_Error_کوډ);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(دننه_بایټونه+16);
        q=(uint16_t *)(بهر_بایټونه+16);

        /* read and swap the tokenCount */
        token_شمېر=ds->readUInt16(*p);
        ds->بدلون_Array16(ds, p, 2, q, p_Error_کوډ);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        إذا(token_شمېر<=512) {
            count=token_شمېر;
        } وإلا {
            count=512;
        }
        لكل(i=0; i<count; ++i) {
            نښې[i]=udata_readInt16(ds, p[i]);
        }
        لكل(; i<512; ++i) {
            نښې[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, نښې, token_شمېر, map, p_Error_کوډ);
        makeTokenMap(ds, نښې+256, (uint16_t)(token_شمېر>256 ? token_شمېر-256 : 0), trailMap, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        لنډمهاله=(uint16_t *)uprv_malloc(token_شمېر*2);
        إذا(لنډمهاله==تش_اشاره) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_شمېر);
            *p_Error_کوډ=U_حافظه_ویش_ERROR;
            أرجع 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        لكل(i=0; i<token_شمېر && i<256; ++i) {
            ds->بدلون_Array16(ds, p+i, 2, لنډمهاله+map[i], p_Error_کوډ);
        }

        /* swap and permutate trail-byte tokens */
        لكل(; i<token_شمېر; ++i) {
            ds->بدلون_Array16(ds, p+i, 2, لنډمهاله+(i&0xffffff00)+trailMap[i&0xff], p_Error_کوډ);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, لنډمهاله, token_شمېر*2);
        uprv_free(لنډمهاله);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_بدلون_Inv_String_بلاک(ds, دننه_بایټونه+token_String_دننه_واټن, (int32_t)(groups_دننه_واټن-token_String_دننه_واټن),
                                    بهر_بایټونه+token_String_دننه_واټن, p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            أرجع 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(دننه_بایټونه+groups_دننه_واټن)));
        ds->بدلون_Array16(ds, دننه_بایټونه+groups_دننه_واټن, (int32_t)((1+count*3)*2),
                           بهر_بایټونه+groups_دننه_واټن, p_Error_کوډ);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        إذا(ds->دننه_Charset!=ds->بهر_Charset) {
            uint16_t offsets[کرښې_فی_ډله+1], lengths[کرښې_فی_ډله+1];

            ثابت uint8_t *دننه_مزي_ګانې, *بل_دننه_مزي_ګانې;
            uint8_t *بهر_مزي_ګانې;

            uint8_t c;

            دننه_مزي_ګانې=دننه_بایټونه+ډله_String_دننه_واټن;
            بهر_مزي_ګانې=بهر_بایټونه+ډله_String_دننه_واټن;

            مزي_ګانې_شمېر=alg_نومونه_دننه_واټن-ډله_String_دننه_واټن;

            /* iterate through string groups until only a few padding bytes are left */
            طالما(مزي_ګانې_شمېر>32) {
                بل_دننه_مزي_ګانې=expand_ډله_Lengths(دننه_مزي_ګانې, offsets, lengths);

                /* move past the length bytes */
                مزي_ګانې_شمېر-=(uint32_t)(بل_دننه_مزي_ګانې-دننه_مزي_ګانې);
                بهر_مزي_ګانې+=بل_دننه_مزي_ګانې-دننه_مزي_ګانې;
                دننه_مزي_ګانې=بل_دننه_مزي_ګانې;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                مزي_ګانې_شمېر-=count;

                /* swap the string bytes using map[] and trailMap[] */
                طالما(count>0) {
                    c=*دننه_مزي_ګانې++;
                    *بهر_مزي_ګانې++=map[c];
                    إذا(نښې[c]!=-2) {
                        --count;
                    } وإلا {
                        /* token lead byte: swap the trail byte, too */
                        *بهر_مزي_ګانې++=trailMap[*دننه_مزي_ګانې++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        دننه_واټن=alg_نومونه_دننه_واټن;
        count=ds->readUInt32(*((ثابت uint32_t *)(دننه_بایټونه+دننه_واټن)));
        ds->بدلون_Array32(ds, دننه_بایټونه+دننه_واټن, 4, بهر_بایټونه+دننه_واټن, p_Error_کوډ);
        دننه_واټن+=4;

        لكل(i=0; i<count; ++i) {
            إذا(دننه_واټن>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_کوډ=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            دننه_Range=(ثابت AlgorithmicRange *)(دننه_بایټونه+دننه_واټن);
            بهر_Range=(AlgorithmicRange *)(بهر_بایټونه+دننه_واټن);
            دننه_واټن+=ds->readUInt16(دننه_Range->size);

            ds->بدلون_Array32(ds, دننه_Range, 8, بهر_Range, p_Error_کوډ);
            ds->بدلون_Array16(ds, &دننه_Range->size, 2, &بهر_Range->size, p_Error_کوډ);
            بدل(دننه_Range->type) {
            حالة 0:
                /* swap prefix string */
                ds->بدلون_Inv_Chars(ds, دننه_Range+1, (int32_t)uprv_strlen((ثابت حرف *)(دننه_Range+1)),
                                    بهر_Range+1, p_Error_کوډ);
                إذا(U_FAILURE(*p_Error_کوډ)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    أرجع 0;
                }
                اكسر;
            حالة 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_شمېر;

                    factors_شمېر=دننه_Range->variant;
                    p=(ثابت uint16_t *)(دننه_Range+1);
                    q=(uint16_t *)(بهر_Range+1);
                    ds->بدلون_Array16(ds, p, (int32_t)(factors_شمېر*2), q, p_Error_کوډ);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_شمېر;
                    q+=factors_شمېر;
                    مزي_ګانې_شمېر=(uint32_t)((دننه_بایټونه+دننه_واټن)-(ثابت uint8_t *)p);
                    طالما(مزي_ګانې_شمېر>0 && ((ثابت uint8_t *)p)[مزي_ګانې_شمېر-1]!=0) {
                        --مزي_ګانې_شمېر;
                    }
                    ds->بدلون_Inv_Chars(ds, p, (int32_t)مزي_ګانې_شمېر, q, p_Error_کوډ);
                }
                اكسر;
            افتراضي:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 دننه_Range->type, i);
                *p_Error_کوډ=U_UNSUPPORTED_ERROR;
                أرجع 0;
            }
        }
    }

    أرجع سرلیک_اندازه+(int32_t)دننه_واټن;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
