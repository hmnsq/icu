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

#وصف_ڪريو گروهه_سرڪايو 5
#وصف_ڪريو سٽون_في_گروهه (1L<<گروهه_سرڪايو)
#وصف_ڪريو گروهه_نقاب (سٽون_في_گروهه-1)

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
    گروهه_MSB,
    گروهه_وٿي_مٿاهون,
    گروهه_وٿي_هيٺيون,
    گروهه_ڊيگهائي
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#وصف_ڪريو GET_گروهه_وٿي(گروهه) ((int32_t)(گروهه)[گروهه_وٿي_مٿاهون]<<16|(گروهه)[گروهه_وٿي_هيٺيون])

#وصف_ڪريو ايندڙ_گروهه(گروهه) ((گروهه)+گروهه_ڊيگهائي)
#وصف_ڪريو PREV_گروهه(گروهه) ((گروهه)-گروهه_ڊيگهائي)

تعريفنوع بنية {
    uint32_t شروعات, پڇاڙي;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعريفنوع بنية {
    uint32_t token_String_وٿي, groups_وٿي, گروهه_String_وٿي, alg_نالا_وٿي;
} UChar_نالا;

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

            إذا (skeletonIterator == query.skeleton.پڇاڙي()) {
                أرجع خطأ;
            }
            إذا (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                إذا (آخري_Char == '-') {
                    إذا (*skeletonIterator++ != آخري_Char) {
                        أرجع خطأ;
                    }
                }
                آخري_Char = c;
                أرجع سچ;
            } وإلا إذا (c == '-') {
                إذا (آخري_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.پڇاڙي() - 2)) {
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
                آخري_Char = c;
                أرجع سچ;
            } وإلا {
                إذا (*skeletonIterator++ != c) {
                    أرجع خطأ;
                }
                آخري_Char = c;
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
            U_ASSERT(آخري_Char != '-');
            أرجع skeletonIterator == query.skeleton.پڇاڙي();
        }

        std::string_view بچيل_Significant_Characters() ثابت {
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
        حرف آخري_Char = ' ';
    };

    واضح CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeleton_حد_بندي = skeleton_معلومات;
        لكل (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_حد_بندي < skeleton_معلومات + حجم(skeleton_معلومات));
            إذا (skeleton_حد_بندي >= skeleton_معلومات + حجم(skeleton_معلومات)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_حد_بندي = skeleton_معلومات;
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
                    std::string_view(skeleton_معلومات, skeleton_حد_بندي - skeleton_معلومات) == "HANGULJUNGSEONGO") {
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
                    *skeleton_حد_بندي++ = query[i];
                }
            } وإلا {
                *skeleton_حد_بندي++ = uprv_toupper(query[i]);
            }
        }
        skeleton = std::string_view(skeleton_معلومات, skeleton_حد_بندي - skeleton_معلومات);
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
#وصف_ڪريو GET_GROUPS(نالا) (ثابت uint16_t *)((ثابت حرف *)نالا+نالا->groups_وٿي)

تعريفنوع بنية {
    ثابت CharacterNameQuery *otherName;
    UChar32 ڪوڊ;
} ڳوليو_Name;

#وصف_ڪريو DO_ڳوليو_NAME خالي_اشارو

ساكن UData_يادگيري *u_Char_نالا_معلومات=خالي_اشارو;
ساكن UChar_نالا *u_Char_نالا=خالي_اشارو;
ساكن icu::UInitOnce g_Char_نالا_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساكن int32_t g_Max_Name_ڊيگهائي=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساكن uint32_t g_Name_سيٽ[8]={ 0 };

#وصف_ڪريو U_NONCHARACTER_ڪوڊ_نقطو U_CHAR_CATEGORY_ڳڻپ
#وصف_ڪريو U_LEAD_SURROGATE U_CHAR_CATEGORY_ڳڻپ + 1
#وصف_ڪريو U_TRAIL_SURROGATE U_CHAR_CATEGORY_ڳڻپ + 2

#وصف_ڪريو U_CHAR_EXTENDED_CATEGORY_ڳڻپ (U_CHAR_CATEGORY_ڳڻپ + 3)

ساكن ثابت حرف * ثابت char_Cat_نالا[U_CHAR_EXTENDED_CATEGORY_ڳڻپ] = {
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
    إذا(u_Char_نالا_معلومات) {
        udata_close(u_Char_نالا_معلومات);
        u_Char_نالا_معلومات = خالي_اشارو;
    }
    إذا(u_Char_نالا) {
        u_Char_نالا = خالي_اشارو;
    }
    g_Char_نالا_Init_Once.reset();
    g_Max_Name_ڊيگهائي=0;
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
        pInfo->معلومات_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x61 &&
        pInfo->معلومات_Format[3]==0x6d &&
        pInfo->format_نسخو[0]==1;
}

ساكن فراغ U_CALLCONV
ڀريو_Char_نالا(UError_ڪوڊ &حالت) {
    U_ASSERT(u_Char_نالا_معلومات == خالي_اشارو);
    U_ASSERT(u_Char_نالا == خالي_اشارو);

    u_Char_نالا_معلومات = udata_openChoice(خالي_اشارو, معلومات_TYPE, معلومات_NAME, isAcceptable, خالي_اشارو, &حالت);
    إذا(U_FAILURE(حالت)) {
        u_Char_نالا_معلومات = خالي_اشارو;
    } وإلا {
        u_Char_نالا = (UChar_نالا *)udata_get_يادگيري(u_Char_نالا_معلومات);
    }
    ucln_common_رجسٽر_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساكن UBool
is_معلومات_Loaded(UError_ڪوڊ *p_Error_ڪوڊ) {
    umtx_initOnce(g_Char_نالا_Init_Once, &ڀريو_Char_نالا, *p_Error_ڪوڊ);
    أرجع U_SUCCESS(*p_Error_ڪوڊ);
}

#وصف_ڪريو WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c) UPRV_بلاڪ_MACRO_BEGIN { \
    إذا((بفر_ڊيگهائي)>0) { \
        *(بفر)++=c; \
        --(بفر_ڊيگهائي); \
    } \
    ++(بفر_Pos); \
} UPRV_بلاڪ_MACRO_پڇاڙي

#وصف_ڪريو U_ISO_COMMENT U_CHAR_NAME_CHOICE_ڳڻپ

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
expandName(UChar_نالا *نالا,
           ثابت uint8_t *name, uint16_t name_ڊيگهائي, UCharNameChoice nameChoice,
           حرف *بفر, uint16_t بفر_ڊيگهائي) {
    uint16_t* ٽوڪن = reinterpret_cast<uint16_t*>(نالا) + 8;
    uint16_t token, token_ڳڻپ=*ٽوڪن++, بفر_Pos=0;
    uint8_t* token_لڙيون = reinterpret_cast<uint8_t*>(نالا) + نالا->token_String_وٿي;
    uint8_t c;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_ڳڻپ || ٽوڪن[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح ميدان_اشاريو= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_ڊيگهائي>0) {
                    --name_ڊيگهائي;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--ميدان_اشاريو>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ڊيگهائي=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    طالما(name_ڊيگهائي>0) {
        --name_ڊيگهائي;
        c=*name++;

        إذا(c>=token_ڳڻپ) {
            إذا(c!=';') {
                /* implicit letter */
                WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
            } وإلا {
                /*ختم*/
                اكسر;
            }
        } وإلا {
            token=ٽوڪن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ٽوڪن[c<<8|*name++];
                --name_ڊيگهائي;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
                } وإلا {
                    /*ختم*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                uint8_t *tokenString=token_لڙيون+token;
                طالما((c=*tokenString++)!=0) {
                    WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    إذا(بفر_ڊيگهائي>0) {
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
compareName(UChar_نالا *نالا,
            ثابت uint8_t *name, uint16_t name_ڊيگهائي, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* ٽوڪن = reinterpret_cast<uint16_t*>(نالا) + 8;
    uint16_t token, token_ڳڻپ=*ٽوڪن++;
    uint8_t* token_لڙيون = reinterpret_cast<uint8_t*>(نالا) + نالا->token_String_وٿي;
    uint8_t c;
    خودڪار matcher = otherName.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_ڳڻپ || ٽوڪن[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح ميدان_اشاريو= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_ڊيگهائي>0) {
                    --name_ڊيگهائي;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--ميدان_اشاريو>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_ڊيگهائي=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    طالما(name_ڊيگهائي>0) {
        --name_ڊيگهائي;
        c=*name++;

        إذا(c>=token_ڳڻپ) {
            إذا(c!=';') {
                /* implicit letter */
                إذا (!matcher.consistentWith(c)) {
                    أرجع خطأ;
                }
            } وإلا {
                /*ختم*/
                اكسر;
            }
        } وإلا {
            token=ٽوڪن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=ٽوڪن[c<<8|*name++];
                --name_ڊيگهائي;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    إذا (!matcher.consistentWith(c)) {
                        أرجع خطأ;
                    }
                } وإلا {
                    /*ختم*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                إذا (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(token_لڙيون + token))) {
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
        أرجع U_NONCHARACTER_ڪوڊ_نقطو;
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

    إذا (cat >= UPRV_LENGTHOF(char_Cat_نالا)) {
        أرجع "unknown";
    } وإلا {
        أرجع char_Cat_نالا[cat];
    }
}

ساكن uint16_t getExtName(uint32_t ڪوڊ, حرف *بفر, uint16_t بفر_ڊيگهائي) {
    ثابت حرف *catname = getCharCatName(ڪوڊ);
    uint16_t length = 0;

    UChar32 cp;
    صحيح ndigits, i;
    
    WRITE_CHAR(بفر, بفر_ڊيگهائي, length, '<');
    طالما (catname[length - 1]) {
        WRITE_CHAR(بفر, بفر_ڊيگهائي, length, catname[length - 1]);
    }
    WRITE_CHAR(بفر, بفر_ڊيگهائي, length, '-');
    لكل (cp = ڪوڊ, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    إذا (ndigits < 4)
        ndigits = 4;
    لكل (cp = ڪوڊ, i = ndigits; (cp || i > 0) && بفر_ڊيگهائي; cp >>= 4, بفر_ڊيگهائي--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        بفر[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    بفر += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(بفر, بفر_ڊيگهائي, length, '>');

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
get_گروهه(UChar_نالا *نالا, uint32_t ڪوڊ) {
    ثابت uint16_t *groups=GET_GROUPS(نالا);
    uint16_t گروهه_MSB_2 = static_cast<uint16_t>(ڪوڊ >> گروهه_سرڪايو),
             شروعات=0,
             حد_بندي=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    طالما(شروعات<حد_بندي-1) {
        number = static_cast<uint16_t>((شروعات + حد_بندي) / 2);
        إذا(گروهه_MSB_2<groups[number*گروهه_ڊيگهائي+گروهه_MSB]) {
            حد_بندي=number;
        } وإلا {
            شروعات=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    أرجع groups+شروعات*گروهه_ڊيگهائي;
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
expand_گروهه_Lengths(ثابت uint8_t *s,
                   uint16_t offsets[سٽون_في_گروهه+1], uint16_t lengths[سٽون_في_گروهه+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, وٿي=0, length=0;
    uint8_t ڊيگهائي_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    طالما(i<سٽون_في_گروهه) {
        ڊيگهائي_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        إذا(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | ڊيگهائي_Byte >> 4) + 12);
            ڊيگهائي_Byte&=0xf;
        } وإلا إذا((ڊيگهائي_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((ڊيگهائي_Byte & 0x3f) + 12);
        } وإلا {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(ڊيگهائي_Byte >> 4);
            ڊيگهائي_Byte&=0xf;
        }

        *offsets++=وٿي;
        *lengths++=length;

        وٿي+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        إذا((ڊيگهائي_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=ڊيگهائي_Byte;
            إذا(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=وٿي;
                *lengths++=length;

                وٿي+=length;
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
expand_گروهه_Name(UChar_نالا *نالا, ثابت uint16_t *گروهه,
                uint16_t سٽ_عدد, UCharNameChoice nameChoice,
                حرف *بفر, uint16_t بفر_ڊيگهائي) {
    uint16_t offsets[سٽون_في_گروهه+2], lengths[سٽون_في_گروهه+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نالا) + نالا->گروهه_String_وٿي + GET_گروهه_وٿي(گروهه);
    s=expand_گروهه_Lengths(s, offsets, lengths);
    أرجع expandName(نالا, s+offsets[سٽ_عدد], lengths[سٽ_عدد], nameChoice,
                      بفر, بفر_ڊيگهائي);
}

ساكن uint16_t
getName(UChar_نالا *نالا, uint32_t ڪوڊ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_ڊيگهائي) {
    ثابت uint16_t *گروهه=get_گروهه(نالا, ڪوڊ);
    إذا (static_cast<uint16_t>(ڪوڊ >> گروهه_سرڪايو) == گروهه[گروهه_MSB]) {
        أرجع expand_گروهه_Name(نالا, گروهه, static_cast<uint16_t>(ڪوڊ & گروهه_نقاب), nameChoice,
                               بفر, بفر_ڊيگهائي);
    } وإلا {
        /* group not found */
        /* zero-terminate */
        إذا(بفر_ڊيگهائي>0) {
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
enum_گروهه_نالا(UChar_نالا *نالا, ثابت uint16_t *گروهه,
               UChar32 شروعات, UChar32 پڇاڙي,
               UEnum_Char_نالا_Fn *fn, فراغ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[سٽون_في_گروهه+2], lengths[سٽون_في_گروهه+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نالا) + نالا->گروهه_String_وٿي + GET_گروهه_وٿي(گروهه);

    s=expand_گروهه_Lengths(s, offsets, lengths);
    إذا(fn!=DO_ڳوليو_NAME) {
        حرف بفر[200];
        uint16_t length;

        طالما(شروعات<=پڇاڙي) {
            length=expandName(نالا, s+offsets[شروعات&گروهه_نقاب], lengths[شروعات&گروهه_نقاب], nameChoice, بفر, حجم(بفر));
            إذا (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                بفر[length = getExtName(شروعات, بفر, حجم(بفر))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروعات, nameChoice, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروعات;
        }
    } وإلا {
        ثابت CharacterNameQuery& otherName = *static_cast<ڳوليو_Name*>(context)->otherName;
        طالما(شروعات<=پڇاڙي) {
            إذا(compareName(نالا, s+offsets[شروعات&گروهه_نقاب], lengths[شروعات&گروهه_نقاب], nameChoice, otherName)) {
                static_cast<ڳوليو_Name*>(context)->ڪوڊ = شروعات;
                أرجع خطأ;
            }
            ++شروعات;
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
enum_Ext_نالا(UChar32 شروعات, UChar32 پڇاڙي,
             UEnum_Char_نالا_Fn *fn, فراغ *context)
{
    إذا(fn!=DO_ڳوليو_NAME) {
        حرف بفر[200];
        uint16_t length;
        
        طالما(شروعات<=پڇاڙي) {
            بفر[length = getExtName(شروعات, بفر, حجم(بفر))] = 0;
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروعات, U_EXTENDED_CHAR_NAME, بفر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروعات;
        }
    }

    أرجع سچ;
}

ساكن UBool
enum_نالا(UChar_نالا *نالا,
          UChar32 شروعات, UChar32 حد_بندي,
          UEnum_Char_نالا_Fn *fn, فراغ *context,
          UCharNameChoice nameChoice) {
    uint16_t شروعات_گروهه_MSB, پڇاڙي_گروهه_MSB, گروهه_ڳڻپ;
    ثابت uint16_t *گروهه, *گروهه_حد_بندي;

    شروعات_گروهه_MSB = static_cast<uint16_t>(شروعات >> گروهه_سرڪايو);
    پڇاڙي_گروهه_MSB = static_cast<uint16_t>((حد_بندي - 1) >> گروهه_سرڪايو);

    /* find the group that contains start, or the highest before it */
    گروهه=get_گروهه(نالا, شروعات);

    إذا(شروعات_گروهه_MSB<گروهه[گروهه_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_حد_بندي = static_cast<UChar32>(گروهه[گروهه_MSB]) << گروهه_سرڪايو;
        إذا(ext_حد_بندي>حد_بندي) {
            ext_حد_بندي=حد_بندي;
        }
        إذا(!enum_Ext_نالا(شروعات, ext_حد_بندي-1, fn, context)) {
            أرجع خطأ;
        }
        شروعات=ext_حد_بندي;
    }

    إذا(شروعات_گروهه_MSB==پڇاڙي_گروهه_MSB) {
        إذا(شروعات_گروهه_MSB==گروهه[گروهه_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            أرجع enum_گروهه_نالا(نالا, گروهه, شروعات, حد_بندي-1, fn, context, nameChoice);
        }
    } وإلا {
        ثابت uint16_t *groups=GET_GROUPS(نالا);
        گروهه_ڳڻپ=*groups++;
        گروهه_حد_بندي=groups+گروهه_ڳڻپ*گروهه_ڊيگهائي;

        إذا(شروعات_گروهه_MSB==گروهه[گروهه_MSB]) {
            /* enumerate characters in the partial start group */
            إذا((شروعات&گروهه_نقاب)!=0) {
                إذا(!enum_گروهه_نالا(نالا, گروهه,
                                   شروعات, (static_cast<UChar32>(شروعات_گروهه_MSB) << گروهه_سرڪايو) + سٽون_في_گروهه - 1,
                                   fn, context, nameChoice)) {
                    أرجع خطأ;
                }
                گروهه=ايندڙ_گروهه(گروهه); /* continue with the next group */
            }
        } وإلا إذا(شروعات_گروهه_MSB>گروهه[گروهه_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *ايندڙ_گروهه_2=ايندڙ_گروهه(گروهه);
            إذا (ايندڙ_گروهه_2 < گروهه_حد_بندي && ايندڙ_گروهه_2[گروهه_MSB] > شروعات_گروهه_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 پڇاڙي = ايندڙ_گروهه_2[گروهه_MSB] << گروهه_سرڪايو;
                إذا (پڇاڙي > حد_بندي) {
                    پڇاڙي = حد_بندي;
                }
                إذا (!enum_Ext_نالا(شروعات, پڇاڙي - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروهه=ايندڙ_گروهه_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        طالما(گروهه<گروهه_حد_بندي && گروهه[گروهه_MSB]<پڇاڙي_گروهه_MSB) {
            ثابت uint16_t *ايندڙ_گروهه_2;
            شروعات = static_cast<UChar32>(گروهه[گروهه_MSB]) << گروهه_سرڪايو;
            إذا(!enum_گروهه_نالا(نالا, گروهه, شروعات, شروعات+سٽون_في_گروهه-1, fn, context, nameChoice)) {
                أرجع خطأ;
            }
            ايندڙ_گروهه_2=ايندڙ_گروهه(گروهه);
            إذا (ايندڙ_گروهه_2 < گروهه_حد_بندي && ايندڙ_گروهه_2[گروهه_MSB] > گروهه[گروهه_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 پڇاڙي = ايندڙ_گروهه_2[گروهه_MSB] << گروهه_سرڪايو;
                إذا (پڇاڙي > حد_بندي) {
                    پڇاڙي = حد_بندي;
                }
                إذا (!enum_Ext_نالا((گروهه[گروهه_MSB] + 1) << گروهه_سرڪايو, پڇاڙي - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروهه=ايندڙ_گروهه_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        إذا(گروهه<گروهه_حد_بندي && گروهه[گروهه_MSB]==پڇاڙي_گروهه_MSB) {
            أرجع enum_گروهه_نالا(نالا, گروهه, (حد_بندي-1)&~گروهه_نقاب, حد_بندي-1, fn, context, nameChoice);
        } وإلا إذا (nameChoice == U_EXTENDED_CHAR_NAME && گروهه == گروهه_حد_بندي) {
            UChar32 next = (PREV_گروهه(گروهه)[گروهه_MSB] + 1) << گروهه_سرڪايو;
            إذا (next > شروعات) {
                شروعات = next;
            }
        } وإلا {
            أرجع سچ;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
        إذا (حد_بندي > UCHAR_MAX_قدر + 1) {
            حد_بندي = UCHAR_MAX_قدر + 1;
        }
        أرجع enum_Ext_نالا(شروعات, حد_بندي - 1, fn, context);
    }
    
    أرجع سچ;
}

ساكن uint16_t
write_ضرب_ڪندڙ_Suffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t ڪوڊ,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *بفر, uint16_t بفر_ڊيگهائي) {
    uint16_t i, ضرب_ڪندڙ, بفر_Pos=0;
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
        ضرب_ڪندڙ=factors[i];
        indexes[i] = static_cast<uint16_t>(ڪوڊ % ضرب_ڪندڙ);
        ڪوڊ/=ضرب_ڪندڙ;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(ڪوڊ);

    /* write each element */
    لكل(;;) {
        إذا(elementBases!=خالي_اشارو) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ضرب_ڪندڙ=indexes[i];
        طالما(ضرب_ڪندڙ>0) {
            طالما(*s++!=0) {}
            --ضرب_ڪندڙ;
        }
        إذا(elements!=خالي_اشارو) {
            *elements++=s;
        }

        /* write element */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
        }

        /* we do not need to perform the rest of this loop for i==count - break here */
        إذا(i>=count) {
            اكسر;
        }

        /* skip the rest of the strings for this factors[i] */
        ضرب_ڪندڙ = static_cast<uint16_t>(factors[i] - indexes[i] - 1);
        طالما(ضرب_ڪندڙ>0) {
            طالما(*s++!=0) {}
            --ضرب_ڪندڙ;
        }

        ++i;
    }

    /* zero-terminate */
    إذا(بفر_ڊيگهائي>0) {
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
getAlgName(AlgorithmicRange *range, uint32_t ڪوڊ, UCharNameChoice nameChoice,
        حرف *بفر, uint16_t بفر_ڊيگهائي) {
    uint16_t بفر_Pos=0;

    /* Only the normative character name can be algorithmic. */
    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        إذا(بفر_ڊيگهائي>0) {
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
            WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        إذا(count<بفر_ڊيگهائي) {
            بفر[count]=0;
        }

        لكل(i=count; i>0;) {
            إذا(--i<بفر_ڊيگهائي) {
                c = static_cast<حرف>(ڪوڊ & 0xf);
                إذا(c<10) {
                    c+='0';
                } وإلا {
                    c+='A'-10;
                }
                بفر[i]=c;
            }
            ڪوڊ>>=4;
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
            WRITE_CHAR(بفر, بفر_ڊيگهائي, بفر_Pos, c);
        }

        بفر_Pos+=write_ضرب_ڪندڙ_Suffix(factors, count,
                                     s, ڪوڊ-range->شروعات, indexes, خالي_اشارو, خالي_اشارو, بفر, بفر_ڊيگهائي);
        اكسر;
    }
    افتراضي:
        /* undefined type */
        /* zero-terminate */
        إذا(بفر_ڊيگهائي>0) {
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
enum_Alg_نالا(AlgorithmicRange *range,
             UChar32 شروعات, UChar32 حد_بندي,
             UEnum_Char_نالا_Fn *fn, فراغ *context,
             UCharNameChoice nameChoice) {
    حرف بفر[200];
    uint16_t length;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع سچ;
    }

    بدل(range->type) {
    حالة 0: {
        حرف *s, *پڇاڙي;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(شروعات), nameChoice, بفر, حجم(بفر));
        إذا(length<=0) {
            أرجع سچ;
        }

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروعات, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* go to the end of the name; all these names have the same length */
        پڇاڙي=بفر;
        طالما(*پڇاڙي!=0) {
            ++پڇاڙي;
        }

        /* enumerate the rest of the names */
        طالما(++شروعات<حد_بندي) {
            /* increment the hexadecimal number on a character-basis */
            s=پڇاڙي;
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

            إذا(!fn(context, شروعات, nameChoice, بفر, length)) {
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
        uint16_t اڳياڙي_ڊيگهائي, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=بفر;
        اڳياڙي_ڊيگهائي=0;
        طالما((c=*s++)!=0) {
            *suffix++=c;
            ++اڳياڙي_ڊيگهائي;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(اڳياڙي_ڊيگهائي + write_ضرب_ڪندڙ_Suffix(factors, count,
                                              s, static_cast<uint32_t>(شروعات) - range->شروعات,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(بفر) - اڳياڙي_ڊيگهائي)));

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروعات, nameChoice, بفر, length)) {
            أرجع خطأ;
        }

        /* enumerate the rest of the names */
        طالما(++شروعات<حد_بندي) {
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
            length=اڳياڙي_ڊيگهائي;
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                طالما((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            إذا(!fn(context, شروعات, nameChoice, بفر, length)) {
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
ڳوليو_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 ڪوڊ;
    خودڪار matcher = query.matcher();

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
        ڪوڊ=0;
        لكل(حرف c : matcher.بچيل_Significant_Characters()) {
            إذا('0'<=c && c<='9') {
                ڪوڊ=(ڪوڊ<<4)|(c-'0');
            } وإلا إذا('A'<=c && c<='F') {
                ڪوڊ=(ڪوڊ<<4)|(c-'A'+10);
            } وإلا {
                أرجع 0xffff;
            }
        }

        /* does it fit into the range? */
        إذا (range->شروعات <= static_cast<uint32_t>(ڪوڊ) && static_cast<uint32_t>(ڪوڊ) <= range->پڇاڙي) {
            أرجع ڪوڊ;
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
        UChar32 شروعات, حد_بندي;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view اڳياڙي = s;
        إذا (!matcher.consistentWith(اڳياڙي)) {
            أرجع 0xffff;
        }

        s += اڳياڙي.length() + 1;

        شروعات = static_cast<UChar32>(range->شروعات);
        حد_بندي = static_cast<UChar32>(range->پڇاڙي + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.بچيل_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ضرب_ڪندڙ_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, بفر, حجم(بفر));

        /* compare the first suffix */
        إذا(querySuffix == بفر) {
            أرجع شروعات;
        }

        /* enumerate and compare the rest of the suffixes */
        طالما(++شروعات<حد_بندي) {
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
            خودڪار it = querySuffix.begin();
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                حرف c;
                طالما((c=*s++)!=0) {
                    إذا(it == querySuffix.پڇاڙي() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            إذا(i<99 && it == querySuffix.پڇاڙي()) {
                أرجع شروعات;
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

#وصف_ڪريو سيٽ_جوڙيو_3(سيٽ, c) ((سيٽ)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#وصف_ڪريو سيٽ_شامل_آهي(سيٽ, c) (((سيٽ)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساكن int32_t
calc_String_سيٽ_ڊيگهائي(uint32_t سيٽ[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    طالما((c=*s++)!=0) {
        سيٽ_جوڙيو_3(سيٽ, c);
        ++length;
    }
    أرجع length;
}

ساكن int32_t
calcAlgNameSetsLengths(int32_t max_Name_ڊيگهائي) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_ڳڻپ;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_نالا) + u_Char_نالا->alg_نالا_وٿي);
    range_ڳڻپ=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    طالما(range_ڳڻپ>0) {
        بدل(range->type) {
        حالة 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*اڳياڙي*/
            length = calc_String_سيٽ_ڊيگهائي(g_Name_سيٽ, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            إذا(length>max_Name_ڊيگهائي) {
                max_Name_ڊيگهائي=length;
            }
            اكسر;
        حالة 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, ضرب_ڪندڙ, ضرب_ڪندڙ_ڊيگهائي, max_ضرب_ڪندڙ_ڊيگهائي;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calc_String_سيٽ_ڊيگهائي(g_Name_سيٽ, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            لكل(i=0; i<count; ++i) {
                max_ضرب_ڪندڙ_ڊيگهائي=0;
                لكل(ضرب_ڪندڙ=factors[i]; ضرب_ڪندڙ>0; --ضرب_ڪندڙ) {
                    ضرب_ڪندڙ_ڊيگهائي=calc_String_سيٽ_ڊيگهائي(g_Name_سيٽ, s);
                    s+=ضرب_ڪندڙ_ڊيگهائي+1;
                    إذا(ضرب_ڪندڙ_ڊيگهائي>max_ضرب_ڪندڙ_ڊيگهائي) {
                        max_ضرب_ڪندڙ_ڊيگهائي=ضرب_ڪندڙ_ڊيگهائي;
                    }
                }
                length+=max_ضرب_ڪندڙ_ڊيگهائي;
            }

            إذا(length>max_Name_ڊيگهائي) {
                max_Name_ڊيگهائي=length;
            }
            اكسر;
        }
        افتراضي:
            /* unknown type */
            اكسر;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_ڳڻپ;
    }
    أرجع max_Name_ڊيگهائي;
}

ساكن int32_t
calcExtNameSetsLengths(int32_t max_Name_ڊيگهائي) {
    int32_t i, length;

    لكل(i=0; i<UPRV_LENGTHOF(char_Cat_نالا); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_سيٽ_ڊيگهائي(g_Name_سيٽ, char_Cat_نالا[i]);
        إذا(length>max_Name_ڊيگهائي) {
            max_Name_ڊيگهائي=length;
        }
    }
    أرجع max_Name_ڊيگهائي;
}

ساكن int32_t
calc_Name_سيٽ_ڊيگهائي(ثابت uint16_t *ٽوڪن, uint16_t token_ڳڻپ, ثابت uint8_t *token_لڙيون, int8_t *tokenLengths,
                  uint32_t سيٽ[8],
                  ثابت uint8_t **p_سٽ, ثابت uint8_t *سٽ_حد_بندي) {
    ثابت uint8_t *سٽ=*p_سٽ;
    int32_t length=0, token_ڊيگهائي;
    uint16_t c, token;

    طالما (سٽ != سٽ_حد_بندي && (c = *سٽ++) != static_cast<uint8_t>(';')) {
        إذا(c>=token_ڳڻپ) {
            /* implicit letter */
            سيٽ_جوڙيو_3(سيٽ, c);
            ++length;
        } وإلا {
            token=ٽوڪن[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*سٽ++;
                token=ٽوڪن[c];
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                سيٽ_جوڙيو_3(سيٽ, c);
                ++length;
            } وإلا {
                /* count token word */
                إذا(tokenLengths!=خالي_اشارو) {
                    /* use cached token length */
                    token_ڊيگهائي=tokenLengths[c];
                    إذا(token_ڊيگهائي==0) {
                        token_ڊيگهائي = calc_String_سيٽ_ڊيگهائي(سيٽ, reinterpret_cast<ثابت حرف*>(token_لڙيون) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_ڊيگهائي);
                    }
                } وإلا {
                    token_ڊيگهائي = calc_String_سيٽ_ڊيگهائي(سيٽ, reinterpret_cast<ثابت حرف*>(token_لڙيون) + token);
                }
                length+=token_ڊيگهائي;
            }
        }
    }

    *p_سٽ=سٽ;
    أرجع length;
}

ساكن فراغ
calc_گروهه_Name_Sets_Lengths(int32_t max_Name_ڊيگهائي) {
    uint16_t offsets[سٽون_في_گروهه+2], lengths[سٽون_في_گروهه+2];

    uint16_t* ٽوڪن = reinterpret_cast<uint16_t*>(u_Char_نالا) + 8;
    uint16_t token_ڳڻپ=*ٽوڪن++;
    uint8_t* token_لڙيون = reinterpret_cast<uint8_t*>(u_Char_نالا) + u_Char_نالا->token_String_وٿي;

    int8_t *tokenLengths;

    ثابت uint16_t *گروهه;
    ثابت uint8_t *s, *سٽ, *سٽ_حد_بندي;

    int32_t گروهه_ڳڻپ, سٽ_عدد, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_ڳڻپ));
    إذا(tokenLengths!=خالي_اشارو) {
        uprv_memset(tokenLengths, 0, token_ڳڻپ);
    }

    گروهه=GET_GROUPS(u_Char_نالا);
    گروهه_ڳڻپ=*گروهه++;

    /* enumerate all groups */
    طالما(گروهه_ڳڻپ>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_نالا) + u_Char_نالا->گروهه_String_وٿي + GET_گروهه_وٿي(گروهه);
        s=expand_گروهه_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        لكل(سٽ_عدد=0; سٽ_عدد<سٽون_في_گروهه; ++سٽ_عدد) {
            سٽ=s+offsets[سٽ_عدد];
            length=lengths[سٽ_عدد];
            إذا(length==0) {
                استمر;
            }

            سٽ_حد_بندي=سٽ+length;

            /* read regular name */
            length=calc_Name_سيٽ_ڊيگهائي(ٽوڪن, token_ڳڻپ, token_لڙيون, tokenLengths, g_Name_سيٽ, &سٽ, سٽ_حد_بندي);
            إذا(length>max_Name_ڊيگهائي) {
                max_Name_ڊيگهائي=length;
            }
            إذا(سٽ==سٽ_حد_بندي) {
                استمر;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_سيٽ_ڊيگهائي(ٽوڪن, token_ڳڻپ, token_لڙيون, tokenLengths, g_Name_سيٽ, &سٽ, سٽ_حد_بندي);
            إذا(length>max_Name_ڊيگهائي) {
                max_Name_ڊيگهائي=length;
            }
            إذا(سٽ==سٽ_حد_بندي) {
                استمر;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        گروهه=ايندڙ_گروهه(گروهه);
        --گروهه_ڳڻپ;
    }

    إذا(tokenLengths!=خالي_اشارو) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_ڊيگهائي=max_Name_ڊيگهائي;
}

ساكن UBool
calcNameSetsLengths(UError_ڪوڊ *p_Error_ڪوڊ) {
    ساكن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_ڊيگهائي;

    إذا(g_Max_Name_ڊيگهائي!=0) {
        أرجع سچ;
    }

    إذا(!is_معلومات_Loaded(p_Error_ڪوڊ)) {
        أرجع خطأ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    لكل (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        سيٽ_جوڙيو_3(g_Name_سيٽ, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_ڊيگهائي=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_ڊيگهائي=calcExtNameSetsLengths(max_Name_ڊيگهائي);

    /* set sets and lengths from group names, set global maximum values */
    calc_گروهه_Name_Sets_Lengths(max_Name_ڊيگهائي);

    أرجع سچ;
}

U_NAMESPACE_پڇاڙي

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 ڪوڊ, UCharNameChoice nameChoice,
           حرف *بفر, int32_t بفر_ڊيگهائي,
           UError_ڪوڊ *p_Error_ڪوڊ) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    } وإلا إذا(nameChoice>=U_CHAR_NAME_CHOICE_ڳڻپ ||
              بفر_ڊيگهائي<0 || (بفر_ڊيگهائي>0 && بفر==خالي_اشارو)
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا((uint32_t)ڪوڊ>UCHAR_MAX_قدر || !is_معلومات_Loaded(p_Error_ڪوڊ)) {
        أرجع u_terminateChars(بفر, بفر_ڊيگهائي, 0, p_Error_ڪوڊ);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_نالا+u_Char_نالا->alg_نالا_وٿي);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا(algRange->شروعات<=(uint32_t)ڪوڊ && (uint32_t)ڪوڊ<=algRange->پڇاڙي) {
            length=getAlgName(algRange, (uint32_t)ڪوڊ, nameChoice, بفر, (uint16_t)بفر_ڊيگهائي);
            اكسر;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    إذا(i==0) {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_نالا, (uint32_t )ڪوڊ, U_EXTENDED_CHAR_NAME, بفر, (uint16_t) بفر_ڊيگهائي);
            إذا (!length) {
                /* extended character name */
                length = getExtName((uint32_t) ڪوڊ, بفر, (uint16_t) بفر_ڊيگهائي);
            }
        } وإلا {
            /* normal character name */
            length=getName(u_Char_نالا, (uint32_t)ڪوڊ, nameChoice, بفر, (uint16_t)بفر_ڊيگهائي);
        }
    }

    أرجع u_terminateChars(بفر, بفر_ڊيگهائي, length, p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t dest_صلاحيت,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    /* check the argument values */
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    } وإلا إذا(dest_صلاحيت<0 || (dest_صلاحيت>0 && dest==خالي_اشارو)) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    أرجع u_terminateChars(dest, dest_صلاحيت, 0, p_Error_ڪوڊ);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UError_ڪوڊ *p_Error_ڪوڊ) {
    حرف هيٺاهون[120] = {0};
    ڳوليو_Name ڳوليو_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساكن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع error;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_ڳڻپ || name==خالي_اشارو || *name==0) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع error;
    }

    إذا(!is_معلومات_Loaded(p_Error_ڪوڊ)) {
        أرجع error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    لكل(i=0; i<حجم(هيٺاهون); ++i) {
        إذا((c0=*nameChar++)!=0) {
            هيٺاهون[i]=uprv_tolower(c0);
        } وإلا {
            اكسر;
        }
    }
    إذا(i==حجم(هيٺاهون)) {
        /* name too long, there is no such character */
        *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
        أرجع error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    إذا (هيٺاهون[0] == '<') {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME && هيٺاهون[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t حد_بندي = i;
            طالما (i >= 3 && هيٺاهون[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_ڊيگهائي = حد_بندي - (i + 1);
            إذا (i >= 2 && هيٺاهون[i] == '-' && 1 <= hex_ڊيگهائي && hex_ڊيگهائي <= 8) {
                uint32_t cIdx;

                هيٺاهون[i] = 0;

                لكل (++i; i < حد_بندي; ++i) {
                    إذا (هيٺاهون[i] >= '0' && هيٺاهون[i] <= '9') {
                        cp = (cp << 4) + هيٺاهون[i] - '0';
                    } وإلا إذا (هيٺاهون[i] >= 'a' && هيٺاهون[i] <= 'f') {
                        cp = (cp << 4) + هيٺاهون[i] - 'a' + 10;
                    } وإلا {
                        *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
                        أرجع error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    إذا (cp > UCHAR_MAX_قدر) {
                        *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
                        أرجع error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                لكل (هيٺاهون[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_نالا); ++cIdx) {

                    إذا (!uprv_strcmp(هيٺاهون + 1, char_Cat_نالا[cIdx])) {
                        إذا (cat == cIdx) {
                            أرجع cp;
                        }
                        اكسر;
                    }
                }
            }
        }

        *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
        أرجع error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_نالا+u_Char_نالا->alg_نالا_وٿي);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا((cp=ڳوليو_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            أرجع cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    ڳوليو_Name_2.otherName=&query;
    ڳوليو_Name_2.ڪوڊ=error;
    enum_نالا(u_Char_نالا, 0, UCHAR_MAX_قدر + 1, DO_ڳوليو_NAME, &ڳوليو_Name_2, nameChoice);
    إذا (ڳوليو_Name_2.ڪوڊ == error) {
         *p_Error_ڪوڊ = U_ILLEGAL_CHAR_مليو;
    }
    أرجع ڳوليو_Name_2.ڪوڊ;
}

U_CAPI فراغ U_EXPORT2
u_enum_Char_نالا(UChar32 شروعات, UChar32 حد_بندي,
                UEnum_Char_نالا_Fn *fn,
                فراغ *context,
                UCharNameChoice nameChoice,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_ڳڻپ || fn==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    إذا((uint32_t) حد_بندي > UCHAR_MAX_قدر + 1) {
        حد_بندي = UCHAR_MAX_قدر + 1;
    }
    إذا((uint32_t)شروعات>=(uint32_t)حد_بندي) {
        أرجع;
    }

    إذا(!is_معلومات_Loaded(p_Error_ڪوڊ)) {
        أرجع;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_نالا+u_Char_نالا->alg_نالا_وٿي);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        إذا((uint32_t)شروعات<algRange->شروعات) {
            إذا((uint32_t)حد_بندي<=algRange->شروعات) {
                enum_نالا(u_Char_نالا, شروعات, حد_بندي, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_نالا(u_Char_نالا, شروعات, (UChar32)algRange->شروعات, fn, context, nameChoice)) {
                أرجع;
            }
            شروعات=(UChar32)algRange->شروعات;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        إذا((uint32_t)شروعات<=algRange->پڇاڙي) {
            إذا((uint32_t)حد_بندي<=(algRange->پڇاڙي+1)) {
                enum_Alg_نالا(algRange, شروعات, حد_بندي, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_Alg_نالا(algRange, شروعات, (UChar32)algRange->پڇاڙي+1, fn, context, nameChoice)) {
                أرجع;
            }
            شروعات=(UChar32)algRange->پڇاڙي+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_نالا(u_Char_نالا, شروعات, حد_بندي, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_ڊيگهائي() {
    UError_ڪوڊ error_ڪوڊ=U_ZERO_ERROR;
    إذا(calcNameSetsLengths(&error_ڪوڊ)) {
        أرجع g_Max_Name_ڊيگهائي;
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
char_سيٽ_منزل_USet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UError_ڪوڊ error_ڪوڊ;

    error_ڪوڊ=U_ZERO_ERROR;

    إذا(!calcNameSetsLengths(&error_ڪوڊ)) {
        أرجع;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    لكل(i=0; i<256; ++i) {
        إذا(سيٽ_شامل_آهي(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_منزل_UChars(cs, us, length);

    /* add each char16_t to the USet */
    لكل(i=0; i<length; ++i) {
        إذا(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->جوڙيو(sa->سيٽ, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI فراغ U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
    char_سيٽ_منزل_USet(g_Name_سيٽ, sa);
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
             int16_t ٽوڪن[], uint16_t token_ڳڻپ,
             uint8_t map[256],
             UError_ڪوڊ *p_Error_ڪوڊ) {
    UBool استعمال_ٿيل_ٻاهريون_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    إذا(ds->اندريون_Charset==ds->ٻاهريون_Charset) {
        /* Same charset family: identity permutation */
        لكل(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وإلا {
        uprv_memset(map, 0, 256);
        uprv_memset(استعمال_ٿيل_ٻاهريون_Char, 0, 256);

        إذا(token_ڳڻپ>256) {
            token_ڳڻپ=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        لكل(i=1; i<token_ڳڻپ; ++i) {
            إذا(ٽوڪن[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->مٽا_سٽا_Inv_Chars(ds, &c1, 1, &c2, p_Error_ڪوڊ);
                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->اندريون_Charset);
                    أرجع;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                استعمال_ٿيل_ٻاهريون_Char[c2]=سچ;
            }
        }

        /* set the mappings for the rest of the permutation */
        لكل(i=j=1; i<token_ڳڻپ; ++i) {
            /* set mappings that were not set for direct bytes */
            إذا(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                طالما(استعمال_ٿيل_ٻاهريون_Char[j]) {
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
uchar_مٽا_سٽا_نالا(ثابت UDataSwapper *ds,
                ثابت فراغ *اندريون_معلومات, int32_t length, فراغ *ٻاهريون_معلومات,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت UDataInfo *pInfo;
    int32_t سرو_سائيز;

    ثابت uint8_t *اندريون_بائيٽون;
    uint8_t *ٻاهريون_بائيٽون;

    uint32_t token_String_وٿي, groups_وٿي, گروهه_String_وٿي, alg_نالا_وٿي,
             وٿي, i, count, لڙيون_ڳڻپ;

    ثابت AlgorithmicRange *اندريون_Range;
    AlgorithmicRange *ٻاهريون_Range;

    /* udata_swapDataHeader checks the arguments */
    سرو_سائيز=udata_مٽا_سٽا_معلومات_سرو(ds, اندريون_معلومات, length, ٻاهريون_معلومات, p_Error_ڪوڊ);
    إذا(p_Error_ڪوڊ==خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)اندريون_معلومات+4);
    إذا(!(
        pInfo->معلومات_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->معلومات_Format[1]==0x6e &&
        pInfo->معلومات_Format[2]==0x61 &&
        pInfo->معلومات_Format[3]==0x6d &&
        pInfo->format_نسخو[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->معلومات_Format[0], pInfo->معلومات_Format[1],
                         pInfo->معلومات_Format[2], pInfo->معلومات_Format[3],
                         pInfo->format_نسخو[0]);
        *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    اندريون_بائيٽون=(ثابت uint8_t *)اندريون_معلومات+سرو_سائيز;
    ٻاهريون_بائيٽون=(ٻاهريون_معلومات == خالي_اشارو) ? خالي_اشارو : (uint8_t *)ٻاهريون_معلومات+سرو_سائيز;
    إذا(length<0) {
        alg_نالا_وٿي=ds->readUInt32(((ثابت uint32_t *)اندريون_بائيٽون)[3]);
    } وإلا {
        length-=سرو_سائيز;
        إذا( length<20 ||
            (uint32_t)length<(alg_نالا_وٿي=ds->readUInt32(((ثابت uint32_t *)اندريون_بائيٽون)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        وٿي=alg_نالا_وٿي;
        count=ds->readUInt32(*((ثابت uint32_t *)(اندريون_بائيٽون+وٿي)));
        وٿي+=4;

        لكل(i=0; i<count; ++i) {
            اندريون_Range=(ثابت AlgorithmicRange *)(اندريون_بائيٽون+وٿي);
            وٿي+=ds->readUInt16(اندريون_Range->size);
        }
    } وإلا {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *عارضي;

        int16_t ٽوڪن[512];
        uint16_t token_ڳڻپ;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        إذا(اندريون_بائيٽون!=ٻاهريون_بائيٽون) {
            uprv_memcpy(ٻاهريون_بائيٽون, اندريون_بائيٽون, length);
        }

        /* the initial 4 offsets first */
        token_String_وٿي=ds->readUInt32(((ثابت uint32_t *)اندريون_بائيٽون)[0]);
        groups_وٿي=ds->readUInt32(((ثابت uint32_t *)اندريون_بائيٽون)[1]);
        گروهه_String_وٿي=ds->readUInt32(((ثابت uint32_t *)اندريون_بائيٽون)[2]);
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون, 16, ٻاهريون_بائيٽون, p_Error_ڪوڊ);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(اندريون_بائيٽون+16);
        q=(uint16_t *)(ٻاهريون_بائيٽون+16);

        /* read and swap the tokenCount */
        token_ڳڻپ=ds->readUInt16(*p);
        ds->مٽا_سٽا_Array16(ds, p, 2, q, p_Error_ڪوڊ);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        إذا(token_ڳڻپ<=512) {
            count=token_ڳڻپ;
        } وإلا {
            count=512;
        }
        لكل(i=0; i<count; ++i) {
            ٽوڪن[i]=udata_readInt16(ds, p[i]);
        }
        لكل(; i<512; ++i) {
            ٽوڪن[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, ٽوڪن, token_ڳڻپ, map, p_Error_ڪوڊ);
        makeTokenMap(ds, ٽوڪن+256, (uint16_t)(token_ڳڻپ>256 ? token_ڳڻپ-256 : 0), trailMap, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        عارضي=(uint16_t *)uprv_malloc(token_ڳڻپ*2);
        إذا(عارضي==خالي_اشارو) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_ڳڻپ);
            *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
            أرجع 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        لكل(i=0; i<token_ڳڻپ && i<256; ++i) {
            ds->مٽا_سٽا_Array16(ds, p+i, 2, عارضي+map[i], p_Error_ڪوڊ);
        }

        /* swap and permutate trail-byte tokens */
        لكل(; i<token_ڳڻپ; ++i) {
            ds->مٽا_سٽا_Array16(ds, p+i, 2, عارضي+(i&0xffffff00)+trailMap[i&0xff], p_Error_ڪوڊ);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, عارضي, token_ڳڻپ*2);
        uprv_free(عارضي);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_مٽا_سٽا_Inv_String_بلاڪ(ds, اندريون_بائيٽون+token_String_وٿي, (int32_t)(groups_وٿي-token_String_وٿي),
                                    ٻاهريون_بائيٽون+token_String_وٿي, p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            أرجع 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(اندريون_بائيٽون+groups_وٿي)));
        ds->مٽا_سٽا_Array16(ds, اندريون_بائيٽون+groups_وٿي, (int32_t)((1+count*3)*2),
                           ٻاهريون_بائيٽون+groups_وٿي, p_Error_ڪوڊ);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        إذا(ds->اندريون_Charset!=ds->ٻاهريون_Charset) {
            uint16_t offsets[سٽون_في_گروهه+1], lengths[سٽون_في_گروهه+1];

            ثابت uint8_t *اندريون_لڙيون, *ايندڙ_اندريون_لڙيون;
            uint8_t *ٻاهريون_لڙيون;

            uint8_t c;

            اندريون_لڙيون=اندريون_بائيٽون+گروهه_String_وٿي;
            ٻاهريون_لڙيون=ٻاهريون_بائيٽون+گروهه_String_وٿي;

            لڙيون_ڳڻپ=alg_نالا_وٿي-گروهه_String_وٿي;

            /* iterate through string groups until only a few padding bytes are left */
            طالما(لڙيون_ڳڻپ>32) {
                ايندڙ_اندريون_لڙيون=expand_گروهه_Lengths(اندريون_لڙيون, offsets, lengths);

                /* move past the length bytes */
                لڙيون_ڳڻپ-=(uint32_t)(ايندڙ_اندريون_لڙيون-اندريون_لڙيون);
                ٻاهريون_لڙيون+=ايندڙ_اندريون_لڙيون-اندريون_لڙيون;
                اندريون_لڙيون=ايندڙ_اندريون_لڙيون;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                لڙيون_ڳڻپ-=count;

                /* swap the string bytes using map[] and trailMap[] */
                طالما(count>0) {
                    c=*اندريون_لڙيون++;
                    *ٻاهريون_لڙيون++=map[c];
                    إذا(ٽوڪن[c]!=-2) {
                        --count;
                    } وإلا {
                        /* token lead byte: swap the trail byte, too */
                        *ٻاهريون_لڙيون++=trailMap[*اندريون_لڙيون++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        وٿي=alg_نالا_وٿي;
        count=ds->readUInt32(*((ثابت uint32_t *)(اندريون_بائيٽون+وٿي)));
        ds->مٽا_سٽا_Array32(ds, اندريون_بائيٽون+وٿي, 4, ٻاهريون_بائيٽون+وٿي, p_Error_ڪوڊ);
        وٿي+=4;

        لكل(i=0; i<count; ++i) {
            إذا(وٿي>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_ڪوڊ=U_اشاريو_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            اندريون_Range=(ثابت AlgorithmicRange *)(اندريون_بائيٽون+وٿي);
            ٻاهريون_Range=(AlgorithmicRange *)(ٻاهريون_بائيٽون+وٿي);
            وٿي+=ds->readUInt16(اندريون_Range->size);

            ds->مٽا_سٽا_Array32(ds, اندريون_Range, 8, ٻاهريون_Range, p_Error_ڪوڊ);
            ds->مٽا_سٽا_Array16(ds, &اندريون_Range->size, 2, &ٻاهريون_Range->size, p_Error_ڪوڊ);
            بدل(اندريون_Range->type) {
            حالة 0:
                /* swap prefix string */
                ds->مٽا_سٽا_Inv_Chars(ds, اندريون_Range+1, (int32_t)uprv_strlen((ثابت حرف *)(اندريون_Range+1)),
                                    ٻاهريون_Range+1, p_Error_ڪوڊ);
                إذا(U_FAILURE(*p_Error_ڪوڊ)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    أرجع 0;
                }
                اكسر;
            حالة 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_ڳڻپ;

                    factors_ڳڻپ=اندريون_Range->variant;
                    p=(ثابت uint16_t *)(اندريون_Range+1);
                    q=(uint16_t *)(ٻاهريون_Range+1);
                    ds->مٽا_سٽا_Array16(ds, p, (int32_t)(factors_ڳڻپ*2), q, p_Error_ڪوڊ);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_ڳڻپ;
                    q+=factors_ڳڻپ;
                    لڙيون_ڳڻپ=(uint32_t)((اندريون_بائيٽون+وٿي)-(ثابت uint8_t *)p);
                    طالما(لڙيون_ڳڻپ>0 && ((ثابت uint8_t *)p)[لڙيون_ڳڻپ-1]!=0) {
                        --لڙيون_ڳڻپ;
                    }
                    ds->مٽا_سٽا_Inv_Chars(ds, p, (int32_t)لڙيون_ڳڻپ, q, p_Error_ڪوڊ);
                }
                اكسر;
            افتراضي:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 اندريون_Range->type, i);
                *p_Error_ڪوڊ=U_UNSUPPORTED_ERROR;
                أرجع 0;
            }
        }
    }

    أرجع سرو_سائيز+(int32_t)وٿي;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
