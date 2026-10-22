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

ساکن ثابت حرف ڈیٹا_NAME[] = "unames";
ساکن ثابت حرف ڈیٹا_TYPE[] = "icu";

#تعریف_کرو گروہ_سرکاؤ 5
#تعریف_کرو سطریں_فی_گروہ (1L<<گروہ_سرکاؤ)
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
شمار {
    گروہ_MSB,
    گروہ_انحراف_بلند,
    گروہ_انحراف_کم,
    گروہ_لمبائی
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#تعریف_کرو GET_گروہ_انحراف(گروہ) ((int32_t)(گروہ)[گروہ_انحراف_بلند]<<16|(گروہ)[گروہ_انحراف_کم])

#تعریف_کرو اگلا_گروہ(گروہ) ((گروہ)+گروہ_لمبائی)
#تعریف_کرو پچھلا_گروہ(گروہ) ((گروہ)-گروہ_لمبائی)

قسمتعریف ساخت {
    uint32_t آغاز, انتہا;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

قسمتعریف ساخت {
    uint32_t token_String_انحراف, groups_انحراف, گروہ_String_انحراف, alg_نام_انحراف;
} UChar_نام;

نامخلا {
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
جماعت CharacterNameQuery {
  عوامی:
    جماعت Matcher {
      عوامی:
        // `query` must outlive the constructed object.
        Matcher(ثابت CharacterNameQuery& query)
            : query(query), skeletonIterator(query.skeleton.begin()) {}

        منطقی consistentWith(حرف c) {
            // Instead of constructing a skeleton from the name as in the constructor of
            // CharacterNameQuery, we check character-by-character if the skeleton we would
            // construct from the name would be consistent with `query->skeleton`.
            // We require that this be called with characters from an actual character name, so we
            // need not worry about case or underscores here.

            اگر (skeletonIterator == query.skeleton.انتہا()) {
                واپس جھوٹ;
            }
            اگر (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                اگر (آخری_Char == '-') {
                    اگر (*skeletonIterator++ != آخری_Char) {
                        واپس جھوٹ;
                    }
                }
                آخری_Char = c;
                واپس سچ;
            } ورنہ اگر (c == '-') {
                اگر (آخری_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.انتہا() - 2)) {
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
                        واپس جھوٹ;
                    }
                }
                // If lastChar is not ' ', we do not know whether this hyphen is word-final, so we
                // cannot check against the skeleton.
                آخری_Char = c;
                واپس سچ;
            } ورنہ {
                اگر (*skeletonIterator++ != c) {
                    واپس جھوٹ;
                }
                آخری_Char = c;
                واپس سچ;
            }
        }

        منطقی consistentWith(ثابت std::string_view substring) {
            برائے (ثابت حرف c : substring) {
                اگر (!consistentWith(c)) {
                    واپس جھوٹ;
                }
            }
            واپس سچ;
        }

        منطقی matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(آخری_Char != '-');
            واپس skeletonIterator == query.skeleton.انتہا();
        }

        std::string_view بقایا_Significant_Characters() ثابت {
            واپس query.skeleton.substr(skeletonIterator - query.skeleton.begin());
        }

      نجی:
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

    صریح CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeleton_حد_بندی = skeleton_ڈیٹا;
        برائے (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_حد_بندی < skeleton_ڈیٹا + حجم(skeleton_ڈیٹا));
            اگر (skeleton_حد_بندی >= skeleton_ڈیٹا + حجم(skeleton_ڈیٹا)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_حد_بندی = skeleton_ڈیٹا;
                توڑو;
            }
            اگر (query[i] == ' ' || query[i] == '_') {
                جاری;
            }
            اگر (query[i] == '-') {
                منطقی isMedial;
                منطقی is1180MedialHyphen = جھوٹ;
                اگر (i == 0 || i == query.length() - 1) {
                    isMedial = جھوٹ;
                } ورنہ {
                    isMedial = isASCIILetterOrDigit(query[i - 1]) && isASCIILetterOrDigit(query[i + 1]);
                }
                // A medial hyphen is the hyphen in the name of U+1180 HANGUL JUNGSEONG O-E if what
                // comes before skeletonizes to HANGULJUNGSEONGO and what comes after skeletonizes
                // to E.
                اگر (isMedial && uprv_toupper(query[i + 1]) == 'E' &&
                    std::string_view(skeleton_ڈیٹا, skeleton_حد_بندی - skeleton_ڈیٹا) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = سچ;
                    // If there is anything significant after the E, the part of the name after the
                    // hyphen does not skeletonize to E, and thus this is not U+1180.
                    // There can be no hyphens there: if there is one, it is either non-medial, or
                    // there is another letter beyond the E.  The insignificant characters are thus
                    // only spaces and underscores.
                    برائے (std::size_t j = i + 2; j < query.length(); ++j) {
                        اگر (query[j] != ' ' && query[j] != '_') {
                            is1180MedialHyphen = جھوٹ;
                        }
                    }
                }
                اگر (!isMedial || is1180MedialHyphen) {
                    *skeleton_حد_بندی++ = query[i];
                }
            } ورنہ {
                *skeleton_حد_بندی++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_ڈیٹا, skeleton_حد_بندی - skeleton_ڈیٹا);
        // This can be true even if we never went through the is1180MedialHyphen path, e.g., if the
        // query was "HANGUL JUNGSEONG O -E".
        is1180 = skeleton == "HANGULJUNGSEONGO-E";
    }

    Matcher matcher() ثابت {
        واپس Matcher(*یہ);
    }

نجی:
    ساکن منطقی isASCIILetterOrDigit(ثابت حرف c) {
        واپس uprv_isASCIILetter(c) || (c >= '0' && c <= '9');
    }

    حرف skeleton_ڈیٹا[120];
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
#تعریف_کرو GET_GROUPS(نام) (ثابت uint16_t *)((ثابت حرف *)نام+نام->groups_انحراف)

قسمتعریف ساخت {
    ثابت CharacterNameQuery *otherName;
    UChar32 رمز;
} ڈھونڈو_Name;

#تعریف_کرو DO_ڈھونڈو_NAME خالی_اشاریہ

ساکن UData_حافظہ *u_Char_نام_ڈیٹا=خالی_اشاریہ;
ساکن UChar_نام *u_Char_نام=خالی_اشاریہ;
ساکن icu::UInitOnce g_Char_نام_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساکن int32_t g_Max_Name_لمبائی=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساکن uint32_t g_Name_سیٹ[8]={ 0 };

#تعریف_کرو U_NONCHARACTER_رمز_نقطہ U_CHAR_CATEGORY_گنتی
#تعریف_کرو U_LEAD_SURROGATE U_CHAR_CATEGORY_گنتی + 1
#تعریف_کرو U_TRAIL_SURROGATE U_CHAR_CATEGORY_گنتی + 2

#تعریف_کرو U_CHAR_EXTENDED_CATEGORY_گنتی (U_CHAR_CATEGORY_گنتی + 3)

ساکن ثابت حرف * ثابت char_Cat_نام[U_CHAR_EXTENDED_CATEGORY_گنتی] = {
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

ساکن UBool U_CALLCONV unames_cleanup()
{
    اگر(u_Char_نام_ڈیٹا) {
        udata_close(u_Char_نام_ڈیٹا);
        u_Char_نام_ڈیٹا = خالی_اشاریہ;
    }
    اگر(u_Char_نام) {
        u_Char_نام = خالی_اشاریہ;
    }
    g_Char_نام_Init_Once.reset();
    g_Max_Name_لمبائی=0;
    واپس سچ;
}

ساکن UBool U_CALLCONV
isAcceptable(خالی * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    واپس
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->ڈیٹا_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ڈیٹا_Format[1]==0x6e &&
        pInfo->ڈیٹا_Format[2]==0x61 &&
        pInfo->ڈیٹا_Format[3]==0x6d &&
        pInfo->format_نسخہ[0]==1;
}

ساکن خالی U_CALLCONV
بھرو_Char_نام(UError_رمز &حالت_2) {
    U_ASSERT(u_Char_نام_ڈیٹا == خالی_اشاریہ);
    U_ASSERT(u_Char_نام == خالی_اشاریہ);

    u_Char_نام_ڈیٹا = udata_openChoice(خالی_اشاریہ, ڈیٹا_TYPE, ڈیٹا_NAME, isAcceptable, خالی_اشاریہ, &حالت_2);
    اگر(U_FAILURE(حالت_2)) {
        u_Char_نام_ڈیٹا = خالی_اشاریہ;
    } ورنہ {
        u_Char_نام = (UChar_نام *)udata_get_حافظہ(u_Char_نام_ڈیٹا);
    }
    ucln_common_رجسٹر_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساکن UBool
is_ڈیٹا_Loaded(UError_رمز *p_Error_رمز) {
    umtx_initOnce(g_Char_نام_Init_Once, &بھرو_Char_نام, *p_Error_رمز);
    واپس U_SUCCESS(*p_Error_رمز);
}

#تعریف_کرو WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c) UPRV_بلاک_MACRO_BEGIN { \
    اگر((بفر_لمبائی)>0) { \
        *(بفر)++=c; \
        --(بفر_لمبائی); \
    } \
    ++(بفر_Pos); \
} UPRV_بلاک_MACRO_انتہا

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
ساکن uint16_t
expandName(UChar_نام *نام,
           ثابت uint8_t *name, uint16_t name_لمبائی, UCharNameChoice nameChoice,
           حرف *بفر, uint16_t بفر_لمبائی) {
    uint16_t* علامات = reinterpret_cast<uint16_t*>(نام) + 8;
    uint16_t token, token_گنتی=*علامات++, بفر_Pos=0;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(نام) + نام->token_String_انحراف;
    uint8_t c;

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        اگر (static_cast<uint8_t>(';') >= token_گنتی || علامات[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحیح میدان_اشاریہ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            کرو {
                جب(name_لمبائی>0) {
                    --name_لمبائی;
                    اگر(*name++==';') {
                        توڑو;
                    }
                }
            } جب(--میدان_اشاریہ>0);
        } ورنہ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_لمبائی=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    جب(name_لمبائی>0) {
        --name_لمبائی;
        c=*name++;

        اگر(c>=token_گنتی) {
            اگر(c!=';') {
                /* implicit letter */
                WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
            } ورنہ {
                /*ختم_شدہ*/
                توڑو;
            }
        } ورنہ {
            token=علامات[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=علامات[c<<8|*name++];
                --name_لمبائی;
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                اگر(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
                } ورنہ {
                    /*ختم_شدہ*/
                    توڑو;
                }
            } ورنہ {
                /* write token word */
                uint8_t *tokenString=token_لڑیاں+token;
                جب((c=*tokenString++)!=0) {
                    WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    اگر(بفر_لمبائی>0) {
        *بفر=0;
    }

    واپس بفر_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ساکن UBool
compareName(UChar_نام *نام,
            ثابت uint8_t *name, uint16_t name_لمبائی, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* علامات = reinterpret_cast<uint16_t*>(نام) + 8;
    uint16_t token, token_گنتی=*علامات++;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(نام) + نام->token_String_انحراف;
    uint8_t c;
    خودکار matcher = otherName.matcher();

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        اگر (static_cast<uint8_t>(';') >= token_گنتی || علامات[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحیح میدان_اشاریہ= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            کرو {
                جب(name_لمبائی>0) {
                    --name_لمبائی;
                    اگر(*name++==';') {
                        توڑو;
                    }
                }
            } جب(--میدان_اشاریہ>0);
        } ورنہ {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_لمبائی=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    جب(name_لمبائی>0) {
        --name_لمبائی;
        c=*name++;

        اگر(c>=token_گنتی) {
            اگر(c!=';') {
                /* implicit letter */
                اگر (!matcher.consistentWith(c)) {
                    واپس جھوٹ;
                }
            } ورنہ {
                /*ختم_شدہ*/
                توڑو;
            }
        } ورنہ {
            token=علامات[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=علامات[c<<8|*name++];
                --name_لمبائی;
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                اگر(c!=';') {
                    /* explicit letter */
                    اگر (!matcher.consistentWith(c)) {
                        واپس جھوٹ;
                    }
                } ورنہ {
                    /*ختم_شدہ*/
                    توڑو;
                }
            } ورنہ {
                /* write token word */
                اگر (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(token_لڑیاں + token))) {
                    واپس جھوٹ;
                }
            }
        }
    }

    /* complete match? */
    واپس matcher.matches();
}

ساکن uint8_t getCharCat(UChar32 cp) {
    uint8_t cat;

    اگر (U_IS_UNICODE_NONCHAR(cp)) {
        واپس U_NONCHARACTER_رمز_نقطہ;
    }

    اگر ((cat = u_charType(cp)) == U_SURROGATE) {
        cat = U_IS_LEAD(cp) ? U_LEAD_SURROGATE : U_TRAIL_SURROGATE;
    }

    واپس cat;
}

ساکن ثابت حرف *getCharCatName(UChar32 cp) {
    uint8_t cat = getCharCat(cp);

    /* Return unknown if the table of names above is not up to
       date. */

    اگر (cat >= UPRV_LENGTHOF(char_Cat_نام)) {
        واپس "unknown";
    } ورنہ {
        واپس char_Cat_نام[cat];
    }
}

ساکن uint16_t getExtName(uint32_t رمز, حرف *بفر, uint16_t بفر_لمبائی) {
    ثابت حرف *catname = getCharCatName(رمز);
    uint16_t length = 0;

    UChar32 cp;
    صحیح ndigits, i;
    
    WRITE_CHAR(بفر, بفر_لمبائی, length, '<');
    جب (catname[length - 1]) {
        WRITE_CHAR(بفر, بفر_لمبائی, length, catname[length - 1]);
    }
    WRITE_CHAR(بفر, بفر_لمبائی, length, '-');
    برائے (cp = رمز, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    اگر (ndigits < 4)
        ndigits = 4;
    برائے (cp = رمز, i = ndigits; (cp || i > 0) && بفر_لمبائی; cp >>= 4, بفر_لمبائی--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        بفر[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    بفر += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(بفر, بفر_لمبائی, length, '>');

    واپس length;
}

/*
 * getGroup() does a binary search for the group that contains the
 * Unicode code point "code".
 * The return value is always a valid Group* that may contain "code"
 * or else is the highest group before "code".
 * If the lowest group is after "code", then that one is returned.
 */
ساکن ثابت uint16_t *
get_گروہ(UChar_نام *نام, uint32_t رمز) {
    ثابت uint16_t *groups=GET_GROUPS(نام);
    uint16_t گروہ_MSB_2 = static_cast<uint16_t>(رمز >> گروہ_سرکاؤ),
             آغاز=0,
             حد_بندی=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    جب(آغاز<حد_بندی-1) {
        number = static_cast<uint16_t>((آغاز + حد_بندی) / 2);
        اگر(گروہ_MSB_2<groups[number*گروہ_لمبائی+گروہ_MSB]) {
            حد_بندی=number;
        } ورنہ {
            آغاز=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    واپس groups+آغاز*گروہ_لمبائی;
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
ساکن ثابت uint8_t *
expand_گروہ_Lengths(ثابت uint8_t *s,
                   uint16_t offsets[سطریں_فی_گروہ+1], uint16_t lengths[سطریں_فی_گروہ+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, انحراف=0, length=0;
    uint8_t لمبائی_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    جب(i<سطریں_فی_گروہ) {
        لمبائی_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        اگر(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | لمبائی_Byte >> 4) + 12);
            لمبائی_Byte&=0xf;
        } ورنہ اگر((لمبائی_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((لمبائی_Byte & 0x3f) + 12);
        } ورنہ {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(لمبائی_Byte >> 4);
            لمبائی_Byte&=0xf;
        }

        *offsets++=انحراف;
        *lengths++=length;

        انحراف+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        اگر((لمبائی_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=لمبائی_Byte;
            اگر(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=انحراف;
                *lengths++=length;

                انحراف+=length;
                ++i;
            }
        } ورنہ {
            length=0;   /* prevent double-nibble detection in the next iteration */
        }
    }

    /* now, s is at the first group string */
    واپس s;
}

ساکن uint16_t
expand_گروہ_Name(UChar_نام *نام, ثابت uint16_t *گروہ,
                uint16_t سطر_نمبر, UCharNameChoice nameChoice,
                حرف *بفر, uint16_t بفر_لمبائی) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نام) + نام->گروہ_String_انحراف + GET_گروہ_انحراف(گروہ);
    s=expand_گروہ_Lengths(s, offsets, lengths);
    واپس expandName(نام, s+offsets[سطر_نمبر], lengths[سطر_نمبر], nameChoice,
                      بفر, بفر_لمبائی);
}

ساکن uint16_t
getName(UChar_نام *نام, uint32_t رمز, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_لمبائی) {
    ثابت uint16_t *گروہ=get_گروہ(نام, رمز);
    اگر (static_cast<uint16_t>(رمز >> گروہ_سرکاؤ) == گروہ[گروہ_MSB]) {
        واپس expand_گروہ_Name(نام, گروہ, static_cast<uint16_t>(رمز & گروہ_نقاب), nameChoice,
                               بفر, بفر_لمبائی);
    } ورنہ {
        /* group not found */
        /* zero-terminate */
        اگر(بفر_لمبائی>0) {
            *بفر=0;
        }
        واپس 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ساکن UBool
enum_گروہ_نام(UChar_نام *نام, ثابت uint16_t *گروہ,
               UChar32 آغاز, UChar32 انتہا,
               UEnum_Char_نام_Fn *fn, خالی *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نام) + نام->گروہ_String_انحراف + GET_گروہ_انحراف(گروہ);

    s=expand_گروہ_Lengths(s, offsets, lengths);
    اگر(fn!=DO_ڈھونڈو_NAME) {
        حرف بفر[200];
        uint16_t length;

        جب(آغاز<=انتہا) {
            length=expandName(نام, s+offsets[آغاز&گروہ_نقاب], lengths[آغاز&گروہ_نقاب], nameChoice, بفر, حجم(بفر));
            اگر (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                بفر[length = getExtName(آغاز, بفر, حجم(بفر))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            اگر(length>0) {
                اگر(!fn(context, آغاز, nameChoice, بفر, length)) {
                    واپس جھوٹ;
                }
            }
            ++آغاز;
        }
    } ورنہ {
        ثابت CharacterNameQuery& otherName = *static_cast<ڈھونڈو_Name*>(context)->otherName;
        جب(آغاز<=انتہا) {
            اگر(compareName(نام, s+offsets[آغاز&گروہ_نقاب], lengths[آغاز&گروہ_نقاب], nameChoice, otherName)) {
                static_cast<ڈھونڈو_Name*>(context)->رمز = آغاز;
                واپس جھوٹ;
            }
            ++آغاز;
        }
    }
    واپس سچ;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ساکن UBool
enum_Ext_نام(UChar32 آغاز, UChar32 انتہا,
             UEnum_Char_نام_Fn *fn, خالی *context)
{
    اگر(fn!=DO_ڈھونڈو_NAME) {
        حرف بفر[200];
        uint16_t length;
        
        جب(آغاز<=انتہا) {
            بفر[length = getExtName(آغاز, بفر, حجم(بفر))] = 0;
            /* here, we assume that the buffer is large enough */
            اگر(length>0) {
                اگر(!fn(context, آغاز, U_EXTENDED_CHAR_NAME, بفر, length)) {
                    واپس جھوٹ;
                }
            }
            ++آغاز;
        }
    }

    واپس سچ;
}

ساکن UBool
enum_نام(UChar_نام *نام,
          UChar32 آغاز, UChar32 حد_بندی,
          UEnum_Char_نام_Fn *fn, خالی *context,
          UCharNameChoice nameChoice) {
    uint16_t آغاز_گروہ_MSB, انتہا_گروہ_MSB, گروہ_گنتی;
    ثابت uint16_t *گروہ, *گروہ_حد_بندی;

    آغاز_گروہ_MSB = static_cast<uint16_t>(آغاز >> گروہ_سرکاؤ);
    انتہا_گروہ_MSB = static_cast<uint16_t>((حد_بندی - 1) >> گروہ_سرکاؤ);

    /* find the group that contains start, or the highest before it */
    گروہ=get_گروہ(نام, آغاز);

    اگر(آغاز_گروہ_MSB<گروہ[گروہ_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_حد_بندی = static_cast<UChar32>(گروہ[گروہ_MSB]) << گروہ_سرکاؤ;
        اگر(ext_حد_بندی>حد_بندی) {
            ext_حد_بندی=حد_بندی;
        }
        اگر(!enum_Ext_نام(آغاز, ext_حد_بندی-1, fn, context)) {
            واپس جھوٹ;
        }
        آغاز=ext_حد_بندی;
    }

    اگر(آغاز_گروہ_MSB==انتہا_گروہ_MSB) {
        اگر(آغاز_گروہ_MSB==گروہ[گروہ_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            واپس enum_گروہ_نام(نام, گروہ, آغاز, حد_بندی-1, fn, context, nameChoice);
        }
    } ورنہ {
        ثابت uint16_t *groups=GET_GROUPS(نام);
        گروہ_گنتی=*groups++;
        گروہ_حد_بندی=groups+گروہ_گنتی*گروہ_لمبائی;

        اگر(آغاز_گروہ_MSB==گروہ[گروہ_MSB]) {
            /* enumerate characters in the partial start group */
            اگر((آغاز&گروہ_نقاب)!=0) {
                اگر(!enum_گروہ_نام(نام, گروہ,
                                   آغاز, (static_cast<UChar32>(آغاز_گروہ_MSB) << گروہ_سرکاؤ) + سطریں_فی_گروہ - 1,
                                   fn, context, nameChoice)) {
                    واپس جھوٹ;
                }
                گروہ=اگلا_گروہ(گروہ); /* continue with the next group */
            }
        } ورنہ اگر(آغاز_گروہ_MSB>گروہ[گروہ_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *اگلا_گروہ_2=اگلا_گروہ(گروہ);
            اگر (اگلا_گروہ_2 < گروہ_حد_بندی && اگلا_گروہ_2[گروہ_MSB] > آغاز_گروہ_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 انتہا = اگلا_گروہ_2[گروہ_MSB] << گروہ_سرکاؤ;
                اگر (انتہا > حد_بندی) {
                    انتہا = حد_بندی;
                }
                اگر (!enum_Ext_نام(آغاز, انتہا - 1, fn, context)) {
                    واپس جھوٹ;
                }
            }
            گروہ=اگلا_گروہ_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        جب(گروہ<گروہ_حد_بندی && گروہ[گروہ_MSB]<انتہا_گروہ_MSB) {
            ثابت uint16_t *اگلا_گروہ_2;
            آغاز = static_cast<UChar32>(گروہ[گروہ_MSB]) << گروہ_سرکاؤ;
            اگر(!enum_گروہ_نام(نام, گروہ, آغاز, آغاز+سطریں_فی_گروہ-1, fn, context, nameChoice)) {
                واپس جھوٹ;
            }
            اگلا_گروہ_2=اگلا_گروہ(گروہ);
            اگر (اگلا_گروہ_2 < گروہ_حد_بندی && اگلا_گروہ_2[گروہ_MSB] > گروہ[گروہ_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 انتہا = اگلا_گروہ_2[گروہ_MSB] << گروہ_سرکاؤ;
                اگر (انتہا > حد_بندی) {
                    انتہا = حد_بندی;
                }
                اگر (!enum_Ext_نام((گروہ[گروہ_MSB] + 1) << گروہ_سرکاؤ, انتہا - 1, fn, context)) {
                    واپس جھوٹ;
                }
            }
            گروہ=اگلا_گروہ_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        اگر(گروہ<گروہ_حد_بندی && گروہ[گروہ_MSB]==انتہا_گروہ_MSB) {
            واپس enum_گروہ_نام(نام, گروہ, (حد_بندی-1)&~گروہ_نقاب, حد_بندی-1, fn, context, nameChoice);
        } ورنہ اگر (nameChoice == U_EXTENDED_CHAR_NAME && گروہ == گروہ_حد_بندی) {
            UChar32 next = (پچھلا_گروہ(گروہ)[گروہ_MSB] + 1) << گروہ_سرکاؤ;
            اگر (next > آغاز) {
                آغاز = next;
            }
        } ورنہ {
            واپس سچ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    اگر (nameChoice == U_EXTENDED_CHAR_NAME) {
        اگر (حد_بندی > UCHAR_MAX_قدر + 1) {
            حد_بندی = UCHAR_MAX_قدر + 1;
        }
        واپس enum_Ext_نام(آغاز, حد_بندی - 1, fn, context);
    }
    
    واپس سچ;
}

ساکن uint16_t
write_ضربی_عدد_Suffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t رمز,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *بفر, uint16_t بفر_لمبائی) {
    uint16_t i, ضربی_عدد, بفر_Pos=0;
    حرف c;

    /* write elements according to the factors */

    /*
     * the factorized elements are determined by modulo arithmetic
     * with the factors of this algorithm
     *
     * note that for fewer operations, count is decremented here
     */
    --count;
    برائے(i=count; i>0; --i) {
        ضربی_عدد=factors[i];
        indexes[i] = static_cast<uint16_t>(رمز % ضربی_عدد);
        رمز/=ضربی_عدد;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(رمز);

    /* write each element */
    برائے(;;) {
        اگر(elementBases!=خالی_اشاریہ) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ضربی_عدد=indexes[i];
        جب(ضربی_عدد>0) {
            جب(*s++!=0) {}
            --ضربی_عدد;
        }
        اگر(elements!=خالی_اشاریہ) {
            *elements++=s;
        }

        /* write element */
        جب((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        اگر(i>=count) {
            توڑو;
        }

        /* skip the rest of the strings for this factors[i] */
        ضربی_عدد = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        جب(ضربی_عدد>0) {
            جب(*s++!=0) {}
            --ضربی_عدد;
        }

        ++i;
    }

    /* zero-terminate */
    اگر(بفر_لمبائی>0) {
        *بفر=0;
    }

    واپس بفر_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ساکن uint16_t
getAlgName(AlgorithmicRange *range, uint32_t رمز, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_لمبائی) {
    uint16_t بفر_Pos=0;

    /* Only the normative character name can be algorithmic. */
    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        اگر(بفر_لمبائی>0) {
            *بفر=0;
        }
        واپس 0;
    }

    انتخاب(range->type) {
    حالت 0: {
        /* name = prefix hex-digits */
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(range + 1);
        حرف c;

        uint16_t i, count;

        /* copy prefix */
        جب((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        اگر(count<بفر_لمبائی) {
            بفر[count]=0;
        }

        برائے(i=count; i>0;) {
            اگر(--i<بفر_لمبائی) {
                c = static_cast<حرف>(رمز & 0xf);
                اگر(c<10) {
                    c+='0';
                } ورنہ {
                    c+='A'-10;
                }
                بفر[i]=c;
            }
            رمز>>=4;
        }

        بفر_Pos+=count;
        توڑو;
    }
    حالت 1: {
        /* name = prefix factorized-elements */
        uint16_t indexes[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(factors + count);
        حرف c;

        /* copy prefix */
        جب((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_لمبائی, بفر_Pos, c);
        }

        بفر_Pos+=write_ضربی_عدد_Suffix(factors, count,
                                     s, رمز-range->آغاز, indexes, خالی_اشاریہ, خالی_اشاریہ, بفر, بفر_لمبائی);
        توڑو;
    }
    طےشدہ:
        /* undefined type */
        /* zero-terminate */
        اگر(بفر_لمبائی>0) {
            *بفر=0;
        }
        توڑو;
    }

    واپس بفر_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ساکن UBool
enum_Alg_نام(AlgorithmicRange *range,
             UChar32 آغاز, UChar32 حد_بندی,
             UEnum_Char_نام_Fn *fn, خالی *context,
             UCharNameChoice nameChoice) {
    حرف بفر[200];
    uint16_t length;

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        واپس سچ;
    }

    انتخاب(range->type) {
    حالت 0: {
        حرف *s, *انتہا;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(آغاز), nameChoice, بفر, حجم(بفر));
        اگر(length<=0) {
            واپس سچ;
        }

        /* call the enumerator function with this first character */
        اگر(!fn(context, آغاز, nameChoice, بفر, length)) {
            واپس جھوٹ;
        }

        /* go to the end of the name; all these names have the same length */
        انتہا=بفر;
        جب(*انتہا!=0) {
            ++انتہا;
        }

        /* enumerate the rest of the names */
        جب(++آغاز<حد_بندی) {
            /* increment the hexadecimal number on a character-basis */
            s=انتہا;
            برائے (;;) {
                c=*--s;
                اگر(('0'<=c && c<'9') || ('A'<=c && c<'F')) {
                    *s = static_cast<حرف>(c + 1);
                    توڑو;
                } ورنہ اگر(c=='9') {
                    *s='A';
                    توڑو;
                } ورنہ اگر(c=='F') {
                    *s='0';
                }
            }

            اگر(!fn(context, آغاز, nameChoice, بفر, length)) {
                واپس جھوٹ;
            }
        }
        توڑو;
    }
    حالت 1: {
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(factors + count);
        حرف *suffix, *t;
        uint16_t سابقہ_لمبائی, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=بفر;
        سابقہ_لمبائی=0;
        جب((c=*s++)!=0) {
            *suffix++=c;
            ++سابقہ_لمبائی;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(سابقہ_لمبائی + write_ضربی_عدد_Suffix(factors, count,
                                              s, static_cast<uint32_t>(آغاز) - range->آغاز,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(بفر) - سابقہ_لمبائی)));

        /* call the enumerator function with this first character */
        اگر(!fn(context, آغاز, nameChoice, بفر, length)) {
            واپس جھوٹ;
        }

        /* enumerate the rest of the names */
        جب(++آغاز<حد_بندی) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            برائے (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                اگر(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    جب(*s++!=0) {
                    }
                    elements[i]=s;
                    توڑو;
                } ورنہ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just append all elements to the suffix */
            t=suffix;
            length=سابقہ_لمبائی;
            برائے(i=0; i<count; ++i) {
                s=elements[i];
                جب((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            اگر(!fn(context, آغاز, nameChoice, بفر, length)) {
                واپس جھوٹ;
            }
        }
        توڑو;
    }
    طےشدہ:
        /* undefined type */
        توڑو;
    }

    واپس سچ;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ساکن UChar32
ڈھونڈو_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 رمز;
    خودکار matcher = query.matcher();

    اگر(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        واپس 0xffff;
    }

    انتخاب(range->type) {
    حالت 0: {
        /* name = prefix hex-digits */
        ثابت حرف* s = reinterpret_cast<ثابت حرف*>(range + 1);

        /* compare prefix */
        اگر (!matcher.consistentWith(s)) {
            واپس 0xffff;
        }

        /* read hexadecimal code point value */
        رمز=0;
        برائے(حرف c : matcher.بقایا_Significant_Characters()) {
            اگر('0'<=c && c<='9') {
                رمز=(رمز<<4)|(c-'0');
            } ورنہ اگر('A'<=c && c<='F') {
                رمز=(رمز<<4)|(c-'A'+10);
            } ورنہ {
                واپس 0xffff;
            }
        }

        /* does it fit into the range? */
        اگر (range->آغاز <= static_cast<uint32_t>(رمز) && static_cast<uint32_t>(رمز) <= range->انتہا) {
            واپس رمز;
        }
        توڑو;
    }
    حالت 1: {
        حرف بفر[64];
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف *s = reinterpret_cast<ثابت حرف*>(factors + count);
        UChar32 آغاز, حد_بندی;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view سابقہ = s;
        اگر (!matcher.consistentWith(سابقہ)) {
            واپس 0xffff;
        }

        s += سابقہ.length() + 1;

        آغاز = static_cast<UChar32>(range->آغاز);
        حد_بندی = static_cast<UChar32>(range->انتہا + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.بقایا_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ضربی_عدد_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, بفر, حجم(بفر));

        /* compare the first suffix */
        اگر(querySuffix == بفر) {
            واپس آغاز;
        }

        /* enumerate and compare the rest of the suffixes */
        جب(++آغاز<حد_بندی) {
            /* increment the indexes in lexical order bound by the factors */
            i=count;
            برائے (;;) {
                idx = static_cast<uint16_t>(indexes[--i] + 1);
                اگر(idx<factors[i]) {
                    /* skip one index and its element string */
                    indexes[i]=idx;
                    s=elements[i];
                    جب(*s++!=0) {}
                    elements[i]=s;
                    توڑو;
                } ورنہ {
                    /* reset this index to 0 and its element string to the first one */
                    indexes[i]=0;
                    elements[i]=elementBases[i];
                }
            }

            /* to make matters a little easier, just compare all elements of the suffix */
            خودکار it = querySuffix.begin();
            برائے(i=0; i<count; ++i) {
                s=elements[i];
                حرف c;
                جب((c=*s++)!=0) {
                    اگر(it == querySuffix.انتہا() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            اگر(i<99 && it == querySuffix.انتہا()) {
                واپس آغاز;
            }
        }
        توڑو;
    }
    طےشدہ:
        /* undefined type */
        توڑو;
    }

    واپس 0xffff;
}

/* sets of name characters, maximum name lengths ---------------------------- */

#تعریف_کرو سیٹ_جوڑو_3(سیٹ, c) ((سیٹ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#تعریف_کرو سیٹ_شامل_ہے(سیٹ, c) (((سیٹ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساکن int32_t
calc_String_سیٹ_لمبائی(uint32_t سیٹ[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    جب((c=*s++)!=0) {
        سیٹ_جوڑو_3(سیٹ, c);
        ++length;
    }
    واپس length;
}

ساکن int32_t
calcAlgNameSetsLengths(int32_t max_Name_لمبائی) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_گنتی;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_نام) + u_Char_نام->alg_نام_انحراف);
    range_گنتی=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    جب(range_گنتی>0) {
        انتخاب(range->type) {
        حالت 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*سابقہ*/
            length = calc_String_سیٹ_لمبائی(g_Name_سیٹ, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            اگر(length>max_Name_لمبائی) {
                max_Name_لمبائی=length;
            }
            توڑو;
        حالت 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, ضربی_عدد, ضربی_عدد_لمبائی, max_ضربی_عدد_لمبائی;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calc_String_سیٹ_لمبائی(g_Name_سیٹ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            برائے(i=0; i<count; ++i) {
                max_ضربی_عدد_لمبائی=0;
                برائے(ضربی_عدد=factors[i]; ضربی_عدد>0; --ضربی_عدد) {
                    ضربی_عدد_لمبائی=calc_String_سیٹ_لمبائی(g_Name_سیٹ, s);
                    s+=ضربی_عدد_لمبائی+1;
                    اگر(ضربی_عدد_لمبائی>max_ضربی_عدد_لمبائی) {
                        max_ضربی_عدد_لمبائی=ضربی_عدد_لمبائی;
                    }
                }
                length+=max_ضربی_عدد_لمبائی;
            }

            اگر(length>max_Name_لمبائی) {
                max_Name_لمبائی=length;
            }
            توڑو;
        }
        طےشدہ:
            /* unknown type */
            توڑو;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_گنتی;
    }
    واپس max_Name_لمبائی;
}

ساکن int32_t
calcExtNameSetsLengths(int32_t max_Name_لمبائی) {
    int32_t i, length;

    برائے(i=0; i<UPRV_LENGTHOF(char_Cat_نام); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_سیٹ_لمبائی(g_Name_سیٹ, char_Cat_نام[i]);
        اگر(length>max_Name_لمبائی) {
            max_Name_لمبائی=length;
        }
    }
    واپس max_Name_لمبائی;
}

ساکن int32_t
calc_Name_سیٹ_لمبائی(ثابت uint16_t *علامات, uint16_t token_گنتی, ثابت uint8_t *token_لڑیاں, int8_t *tokenLengths,
                  uint32_t سیٹ[8],
                  ثابت uint8_t **p_سطر, ثابت uint8_t *سطر_حد_بندی) {
    ثابت uint8_t *سطر=*p_سطر;
    int32_t length=0, token_لمبائی;
    uint16_t c, token;

    جب (سطر != سطر_حد_بندی && (c = *سطر++) != static_cast<uint8_t>(';')) {
        اگر(c>=token_گنتی) {
            /* implicit letter */
            سیٹ_جوڑو_3(سیٹ, c);
            ++length;
        } ورنہ {
            token=علامات[c];
            اگر (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*سطر++;
                token=علامات[c];
            }
            اگر (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                سیٹ_جوڑو_3(سیٹ, c);
                ++length;
            } ورنہ {
                /* count token word */
                اگر(tokenLengths!=خالی_اشاریہ) {
                    /* use cached token length */
                    token_لمبائی=tokenLengths[c];
                    اگر(token_لمبائی==0) {
                        token_لمبائی = calc_String_سیٹ_لمبائی(سیٹ, reinterpret_cast<ثابت حرف*>(token_لڑیاں) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_لمبائی);
                    }
                } ورنہ {
                    token_لمبائی = calc_String_سیٹ_لمبائی(سیٹ, reinterpret_cast<ثابت حرف*>(token_لڑیاں) + token);
                }
                length+=token_لمبائی;
            }
        }
    }

    *p_سطر=سطر;
    واپس length;
}

ساکن خالی
calc_گروہ_Name_Sets_Lengths(int32_t max_Name_لمبائی) {
    uint16_t offsets[سطریں_فی_گروہ+2], lengths[سطریں_فی_گروہ+2];

    uint16_t* علامات = reinterpret_cast<uint16_t*>(u_Char_نام) + 8;
    uint16_t token_گنتی=*علامات++;
    uint8_t* token_لڑیاں = reinterpret_cast<uint8_t*>(u_Char_نام) + u_Char_نام->token_String_انحراف;

    int8_t *tokenLengths;

    ثابت uint16_t *گروہ;
    ثابت uint8_t *s, *سطر, *سطر_حد_بندی;

    int32_t گروہ_گنتی, سطر_نمبر, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_گنتی));
    اگر(tokenLengths!=خالی_اشاریہ) {
        uprv_memset(tokenLengths, 0, token_گنتی);
    }

    گروہ=GET_GROUPS(u_Char_نام);
    گروہ_گنتی=*گروہ++;

    /* enumerate all groups */
    جب(گروہ_گنتی>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_نام) + u_Char_نام->گروہ_String_انحراف + GET_گروہ_انحراف(گروہ);
        s=expand_گروہ_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        برائے(سطر_نمبر=0; سطر_نمبر<سطریں_فی_گروہ; ++سطر_نمبر) {
            سطر=s+offsets[سطر_نمبر];
            length=lengths[سطر_نمبر];
            اگر(length==0) {
                جاری;
            }

            سطر_حد_بندی=سطر+length;

            /* read regular name */
            length=calc_Name_سیٹ_لمبائی(علامات, token_گنتی, token_لڑیاں, tokenLengths, g_Name_سیٹ, &سطر, سطر_حد_بندی);
            اگر(length>max_Name_لمبائی) {
                max_Name_لمبائی=length;
            }
            اگر(سطر==سطر_حد_بندی) {
                جاری;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_سیٹ_لمبائی(علامات, token_گنتی, token_لڑیاں, tokenLengths, g_Name_سیٹ, &سطر, سطر_حد_بندی);
            اگر(length>max_Name_لمبائی) {
                max_Name_لمبائی=length;
            }
            اگر(سطر==سطر_حد_بندی) {
                جاری;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        گروہ=اگلا_گروہ(گروہ);
        --گروہ_گنتی;
    }

    اگر(tokenLengths!=خالی_اشاریہ) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_لمبائی=max_Name_لمبائی;
}

ساکن UBool
calcNameSetsLengths(UError_رمز *p_Error_رمز) {
    ساکن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_لمبائی;

    اگر(g_Max_Name_لمبائی!=0) {
        واپس سچ;
    }

    اگر(!is_ڈیٹا_Loaded(p_Error_رمز)) {
        واپس جھوٹ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    برائے (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        سیٹ_جوڑو_3(g_Name_سیٹ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_لمبائی=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_لمبائی=calcExtNameSetsLengths(max_Name_لمبائی);

    /* set sets and lengths from group names, set global maximum values */
    calc_گروہ_Name_Sets_Lengths(max_Name_لمبائی);

    واپس سچ;
}

U_NAMESPACE_انتہا

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 رمز, UCharNameChoice nameChoice,
           حرف *بفر, int32_t بفر_لمبائی,
           UError_رمز *p_Error_رمز) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    } ورنہ اگر(nameChoice>=U_CHAR_NAME_CHOICE_گنتی ||
              بفر_لمبائی<0 || (بفر_لمبائی>0 && بفر==خالی_اشاریہ)
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر((uint32_t)رمز>UCHAR_MAX_قدر || !is_ڈیٹا_Loaded(p_Error_رمز)) {
        واپس u_terminateChars(بفر, بفر_لمبائی, 0, p_Error_رمز);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_نام+u_Char_نام->alg_نام_انحراف);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    جب(i>0) {
        اگر(algRange->آغاز<=(uint32_t)رمز && (uint32_t)رمز<=algRange->انتہا) {
            length=getAlgName(algRange, (uint32_t)رمز, nameChoice, بفر, (uint16_t)بفر_لمبائی);
            توڑو;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    اگر(i==0) {
        اگر (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_نام, (uint32_t )رمز, U_EXTENDED_CHAR_NAME, بفر, (uint16_t) بفر_لمبائی);
            اگر (!length) {
                /* extended character name */
                length = getExtName((uint32_t) رمز, بفر, (uint16_t) بفر_لمبائی);
            }
        } ورنہ {
            /* normal character name */
            length=getName(u_Char_نام, (uint32_t)رمز, nameChoice, بفر, (uint16_t)بفر_لمبائی);
        }
    }

    واپس u_terminateChars(بفر, بفر_لمبائی, length, p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t dest_استعداد,
                UError_رمز *p_Error_رمز) {
    /* check the argument values */
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    } ورنہ اگر(dest_استعداد<0 || (dest_استعداد>0 && dest==خالی_اشاریہ)) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    واپس u_terminateChars(dest, dest_استعداد, 0, p_Error_رمز);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UError_رمز *p_Error_رمز) {
    حرف زیریں[120] = {0};
    ڈھونڈو_Name ڈھونڈو_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساکن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس error;
    }

    اگر(nameChoice>=U_CHAR_NAME_CHOICE_گنتی || name==خالی_اشاریہ || *name==0) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس error;
    }

    اگر(!is_ڈیٹا_Loaded(p_Error_رمز)) {
        واپس error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    برائے(i=0; i<حجم(زیریں); ++i) {
        اگر((c0=*nameChar++)!=0) {
            زیریں[i]=uprv_tolower(c0);
        } ورنہ {
            توڑو;
        }
    }
    اگر(i==حجم(زیریں)) {
        /* name too long, there is no such character */
        *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
        واپس error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    اگر (زیریں[0] == '<') {
        اگر (nameChoice == U_EXTENDED_CHAR_NAME && زیریں[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t حد_بندی = i;
            جب (i >= 3 && زیریں[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_لمبائی = حد_بندی - (i + 1);
            اگر (i >= 2 && زیریں[i] == '-' && 1 <= hex_لمبائی && hex_لمبائی <= 8) {
                uint32_t cIdx;

                زیریں[i] = 0;

                برائے (++i; i < حد_بندی; ++i) {
                    اگر (زیریں[i] >= '0' && زیریں[i] <= '9') {
                        cp = (cp << 4) + زیریں[i] - '0';
                    } ورنہ اگر (زیریں[i] >= 'a' && زیریں[i] <= 'f') {
                        cp = (cp << 4) + زیریں[i] - 'a' + 10;
                    } ورنہ {
                        *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
                        واپس error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    اگر (cp > UCHAR_MAX_قدر) {
                        *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
                        واپس error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                برائے (زیریں[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_نام); ++cIdx) {

                    اگر (!uprv_strcmp(زیریں + 1, char_Cat_نام[cIdx])) {
                        اگر (cat == cIdx) {
                            واپس cp;
                        }
                        توڑو;
                    }
                }
            }
        }

        *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
        واپس error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_نام+u_Char_نام->alg_نام_انحراف);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    جب(i>0) {
        اگر((cp=ڈھونڈو_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            واپس cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    ڈھونڈو_Name_2.otherName=&query;
    ڈھونڈو_Name_2.رمز=error;
    enum_نام(u_Char_نام, 0, UCHAR_MAX_قدر + 1, DO_ڈھونڈو_NAME, &ڈھونڈو_Name_2, nameChoice);
    اگر (ڈھونڈو_Name_2.رمز == error) {
         *p_Error_رمز = U_ILLEGAL_CHAR_ملا;
    }
    واپس ڈھونڈو_Name_2.رمز;
}

U_CAPI خالی U_EXPORT2
u_enum_Char_نام(UChar32 آغاز, UChar32 حد_بندی,
                UEnum_Char_نام_Fn *fn,
                خالی *context,
                UCharNameChoice nameChoice,
                UError_رمز *p_Error_رمز) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    اگر(nameChoice>=U_CHAR_NAME_CHOICE_گنتی || fn==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    اگر((uint32_t) حد_بندی > UCHAR_MAX_قدر + 1) {
        حد_بندی = UCHAR_MAX_قدر + 1;
    }
    اگر((uint32_t)آغاز>=(uint32_t)حد_بندی) {
        واپس;
    }

    اگر(!is_ڈیٹا_Loaded(p_Error_رمز)) {
        واپس;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_نام+u_Char_نام->alg_نام_انحراف);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    جب(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        اگر((uint32_t)آغاز<algRange->آغاز) {
            اگر((uint32_t)حد_بندی<=algRange->آغاز) {
                enum_نام(u_Char_نام, آغاز, حد_بندی, fn, context, nameChoice);
                واپس;
            }
            اگر(!enum_نام(u_Char_نام, آغاز, (UChar32)algRange->آغاز, fn, context, nameChoice)) {
                واپس;
            }
            آغاز=(UChar32)algRange->آغاز;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        اگر((uint32_t)آغاز<=algRange->انتہا) {
            اگر((uint32_t)حد_بندی<=(algRange->انتہا+1)) {
                enum_Alg_نام(algRange, آغاز, حد_بندی, fn, context, nameChoice);
                واپس;
            }
            اگر(!enum_Alg_نام(algRange, آغاز, (UChar32)algRange->انتہا+1, fn, context, nameChoice)) {
                واپس;
            }
            آغاز=(UChar32)algRange->انتہا+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_نام(u_Char_نام, آغاز, حد_بندی, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_لمبائی() {
    UError_رمز error_رمز=U_ZERO_ERROR;
    اگر(calcNameSetsLengths(&error_رمز)) {
        واپس g_Max_Name_لمبائی;
    } ورنہ {
        واپس 0;
    }
}

/**
 * Converts the char set cset into a Unicode set uset.
 * @param cset Set of 256 bit flags corresponding to a set of chars.
 * @param uset USet to receive characters. Existing contents are deleted.
 */
ساکن خالی
char_سیٹ_مقصد_USet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UError_رمز error_رمز;

    error_رمز=U_ZERO_ERROR;

    اگر(!calcNameSetsLengths(&error_رمز)) {
        واپس;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    برائے(i=0; i<256; ++i) {
        اگر(سیٹ_شامل_ہے(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_مقصد_UChars(cs, us, length);

    /* add each char16_t to the USet */
    برائے(i=0; i<length; ++i) {
        اگر(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->جوڑو(sa->سیٹ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI خالی U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
    char_سیٹ_مقصد_USet(g_Name_سیٹ, sa);
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
ساکن خالی
makeTokenMap(ثابت UDataSwapper *ds,
             int16_t علامات[], uint16_t token_گنتی,
             uint8_t map[256],
             UError_رمز *p_Error_رمز) {
    UBool مستعمل_برآمد_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    اگر(ds->اندر_Charset==ds->برآمد_Charset) {
        /* Same charset family: identity permutation */
        برائے(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } ورنہ {
        uprv_memset(map, 0, 256);
        uprv_memset(مستعمل_برآمد_Char, 0, 256);

        اگر(token_گنتی>256) {
            token_گنتی=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        برائے(i=1; i<token_گنتی; ++i) {
            اگر(علامات[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->تبادلہ_Inv_Chars(ds, &c1, 1, &c2, p_Error_رمز);
                اگر(U_FAILURE(*p_Error_رمز)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->اندر_Charset);
                    واپس;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                مستعمل_برآمد_Char[c2]=سچ;
            }
        }

        /* set the mappings for the rest of the permutation */
        برائے(i=j=1; i<token_گنتی; ++i) {
            /* set mappings that were not set for direct bytes */
            اگر(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                جب(مستعمل_برآمد_Char[j]) {
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
uchar_تبادلہ_نام(ثابت UDataSwapper *ds,
                ثابت خالی *اندر_ڈیٹا, int32_t length, خالی *برآمد_ڈیٹا,
                UError_رمز *p_Error_رمز) {
    ثابت UDataInfo *pInfo;
    int32_t سرخی_سائز;

    ثابت uint8_t *اندر_بائٹیں;
    uint8_t *برآمد_بائٹیں;

    uint32_t token_String_انحراف, groups_انحراف, گروہ_String_انحراف, alg_نام_انحراف,
             انحراف, i, count, لڑیاں_گنتی;

    ثابت AlgorithmicRange *اندر_Range;
    AlgorithmicRange *برآمد_Range;

    /* udata_swapDataHeader checks the arguments */
    سرخی_سائز=udata_تبادلہ_ڈیٹا_سرخی(ds, اندر_ڈیٹا, length, برآمد_ڈیٹا, p_Error_رمز);
    اگر(p_Error_رمز==خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندر_ڈیٹا+4);
    اگر(!(
        pInfo->ڈیٹا_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->ڈیٹا_Format[1]==0x6e &&
        pInfo->ڈیٹا_Format[2]==0x61 &&
        pInfo->ڈیٹا_Format[3]==0x6d &&
        pInfo->format_نسخہ[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->ڈیٹا_Format[0], pInfo->ڈیٹا_Format[1],
                         pInfo->ڈیٹا_Format[2], pInfo->ڈیٹا_Format[3],
                         pInfo->format_نسخہ[0]);
        *p_Error_رمز=U_UNSUPPORTED_ERROR;
        واپس 0;
    }

    اندر_بائٹیں=(ثابت uint8_t *)اندر_ڈیٹا+سرخی_سائز;
    برآمد_بائٹیں=(برآمد_ڈیٹا == خالی_اشاریہ) ? خالی_اشاریہ : (uint8_t *)برآمد_ڈیٹا+سرخی_سائز;
    اگر(length<0) {
        alg_نام_انحراف=ds->readUInt32(((ثابت uint32_t *)اندر_بائٹیں)[3]);
    } ورنہ {
        length-=سرخی_سائز;
        اگر( length<20 ||
            (uint32_t)length<(alg_نام_انحراف=ds->readUInt32(((ثابت uint32_t *)اندر_بائٹیں)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
            واپس 0;
        }
    }

    اگر(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        انحراف=alg_نام_انحراف;
        count=ds->readUInt32(*((ثابت uint32_t *)(اندر_بائٹیں+انحراف)));
        انحراف+=4;

        برائے(i=0; i<count; ++i) {
            اندر_Range=(ثابت AlgorithmicRange *)(اندر_بائٹیں+انحراف);
            انحراف+=ds->readUInt16(اندر_Range->size);
        }
    } ورنہ {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *عارضی;

        int16_t علامات[512];
        uint16_t token_گنتی;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        اگر(اندر_بائٹیں!=برآمد_بائٹیں) {
            uprv_memcpy(برآمد_بائٹیں, اندر_بائٹیں, length);
        }

        /* the initial 4 offsets first */
        token_String_انحراف=ds->readUInt32(((ثابت uint32_t *)اندر_بائٹیں)[0]);
        groups_انحراف=ds->readUInt32(((ثابت uint32_t *)اندر_بائٹیں)[1]);
        گروہ_String_انحراف=ds->readUInt32(((ثابت uint32_t *)اندر_بائٹیں)[2]);
        ds->تبادلہ_Array32(ds, اندر_بائٹیں, 16, برآمد_بائٹیں, p_Error_رمز);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(اندر_بائٹیں+16);
        q=(uint16_t *)(برآمد_بائٹیں+16);

        /* read and swap the tokenCount */
        token_گنتی=ds->readUInt16(*p);
        ds->تبادلہ_Array16(ds, p, 2, q, p_Error_رمز);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        اگر(token_گنتی<=512) {
            count=token_گنتی;
        } ورنہ {
            count=512;
        }
        برائے(i=0; i<count; ++i) {
            علامات[i]=udata_readInt16(ds, p[i]);
        }
        برائے(; i<512; ++i) {
            علامات[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, علامات, token_گنتی, map, p_Error_رمز);
        makeTokenMap(ds, علامات+256, (uint16_t)(token_گنتی>256 ? token_گنتی-256 : 0), trailMap, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        عارضی=(uint16_t *)uprv_malloc(token_گنتی*2);
        اگر(عارضی==خالی_اشاریہ) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_گنتی);
            *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
            واپس 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        برائے(i=0; i<token_گنتی && i<256; ++i) {
            ds->تبادلہ_Array16(ds, p+i, 2, عارضی+map[i], p_Error_رمز);
        }

        /* swap and permutate trail-byte tokens */
        برائے(; i<token_گنتی; ++i) {
            ds->تبادلہ_Array16(ds, p+i, 2, عارضی+(i&0xffffff00)+trailMap[i&0xff], p_Error_رمز);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, عارضی, token_گنتی*2);
        uprv_free(عارضی);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_تبادلہ_Inv_String_بلاک(ds, اندر_بائٹیں+token_String_انحراف, (int32_t)(groups_انحراف-token_String_انحراف),
                                    برآمد_بائٹیں+token_String_انحراف, p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            واپس 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(اندر_بائٹیں+groups_انحراف)));
        ds->تبادلہ_Array16(ds, اندر_بائٹیں+groups_انحراف, (int32_t)((1+count*3)*2),
                           برآمد_بائٹیں+groups_انحراف, p_Error_رمز);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        اگر(ds->اندر_Charset!=ds->برآمد_Charset) {
            uint16_t offsets[سطریں_فی_گروہ+1], lengths[سطریں_فی_گروہ+1];

            ثابت uint8_t *اندر_لڑیاں, *اگلا_اندر_لڑیاں;
            uint8_t *برآمد_لڑیاں;

            uint8_t c;

            اندر_لڑیاں=اندر_بائٹیں+گروہ_String_انحراف;
            برآمد_لڑیاں=برآمد_بائٹیں+گروہ_String_انحراف;

            لڑیاں_گنتی=alg_نام_انحراف-گروہ_String_انحراف;

            /* iterate through string groups until only a few padding bytes are left */
            جب(لڑیاں_گنتی>32) {
                اگلا_اندر_لڑیاں=expand_گروہ_Lengths(اندر_لڑیاں, offsets, lengths);

                /* move past the length bytes */
                لڑیاں_گنتی-=(uint32_t)(اگلا_اندر_لڑیاں-اندر_لڑیاں);
                برآمد_لڑیاں+=اگلا_اندر_لڑیاں-اندر_لڑیاں;
                اندر_لڑیاں=اگلا_اندر_لڑیاں;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                لڑیاں_گنتی-=count;

                /* swap the string bytes using map[] and trailMap[] */
                جب(count>0) {
                    c=*اندر_لڑیاں++;
                    *برآمد_لڑیاں++=map[c];
                    اگر(علامات[c]!=-2) {
                        --count;
                    } ورنہ {
                        /* token lead byte: swap the trail byte, too */
                        *برآمد_لڑیاں++=trailMap[*اندر_لڑیاں++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        انحراف=alg_نام_انحراف;
        count=ds->readUInt32(*((ثابت uint32_t *)(اندر_بائٹیں+انحراف)));
        ds->تبادلہ_Array32(ds, اندر_بائٹیں+انحراف, 4, برآمد_بائٹیں+انحراف, p_Error_رمز);
        انحراف+=4;

        برائے(i=0; i<count; ++i) {
            اگر(انحراف>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_رمز=U_اشاریہ_OUTOFBOUNDS_ERROR;
                واپس 0;
            }

            اندر_Range=(ثابت AlgorithmicRange *)(اندر_بائٹیں+انحراف);
            برآمد_Range=(AlgorithmicRange *)(برآمد_بائٹیں+انحراف);
            انحراف+=ds->readUInt16(اندر_Range->size);

            ds->تبادلہ_Array32(ds, اندر_Range, 8, برآمد_Range, p_Error_رمز);
            ds->تبادلہ_Array16(ds, &اندر_Range->size, 2, &برآمد_Range->size, p_Error_رمز);
            انتخاب(اندر_Range->type) {
            حالت 0:
                /* swap prefix string */
                ds->تبادلہ_Inv_Chars(ds, اندر_Range+1, (int32_t)uprv_strlen((ثابت حرف *)(اندر_Range+1)),
                                    برآمد_Range+1, p_Error_رمز);
                اگر(U_FAILURE(*p_Error_رمز)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    واپس 0;
                }
                توڑو;
            حالت 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_گنتی;

                    factors_گنتی=اندر_Range->variant;
                    p=(ثابت uint16_t *)(اندر_Range+1);
                    q=(uint16_t *)(برآمد_Range+1);
                    ds->تبادلہ_Array16(ds, p, (int32_t)(factors_گنتی*2), q, p_Error_رمز);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_گنتی;
                    q+=factors_گنتی;
                    لڑیاں_گنتی=(uint32_t)((اندر_بائٹیں+انحراف)-(ثابت uint8_t *)p);
                    جب(لڑیاں_گنتی>0 && ((ثابت uint8_t *)p)[لڑیاں_گنتی-1]!=0) {
                        --لڑیاں_گنتی;
                    }
                    ds->تبادلہ_Inv_Chars(ds, p, (int32_t)لڑیاں_گنتی, q, p_Error_رمز);
                }
                توڑو;
            طےشدہ:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 اندر_Range->type, i);
                *p_Error_رمز=U_UNSUPPORTED_ERROR;
                واپس 0;
            }
        }
    }

    واپس سرخی_سائز+(int32_t)انحراف;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
