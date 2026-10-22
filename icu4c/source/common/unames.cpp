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

ساكن ثابت حرف داده_NAME[] = "unames";
ساكن ثابت حرف داده_TYPE[] = "icu";

#تعریف_کن گروه_لغزش 5
#تعریف_کن سطرها_در_هر_گروه (1L<<گروه_لغزش)
#تعریف_کن گروه_ماسک (سطرها_در_هر_گروه-1)

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
    گروه_MSB,
    گروه_جابه_جایی_بالا,
    گروه_جابه_جایی_کم,
    گروه_درازی
};

/*
 * Get the 32-bit group offset.
 * @param group (const uint16_t *) pointer to a Group triple of uint16_t
 * @return group offset (int32_t)
 */
#تعریف_کن GET_گروه_جابه_جایی(گروه) ((int32_t)(گروه)[گروه_جابه_جایی_بالا]<<16|(گروه)[گروه_جابه_جایی_کم])

#تعریف_کن بعدی_گروه(گروه) ((گروه)+گروه_درازی)
#تعریف_کن PREV_گروه(گروه) ((گروه)-گروه_درازی)

تعريفنوع بنية {
    uint32_t شروع, انتها;
    uint8_t type, variant;
    uint16_t size;
} AlgorithmicRange;

تعريفنوع بنية {
    uint32_t token_String_جابه_جایی, groups_جابه_جایی, گروه_String_جابه_جایی, alg_نامها_جابه_جایی;
} UChar_نامها;

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

            إذا (skeletonIterator == query.skeleton.انتها()) {
                أرجع خطأ;
            }
            إذا (c == ' ') {
                // The last hyphen was word-final; check that there is a corresponding significant
                // hyphen in the skeleton.  A hyphen in a character name cannot be both word-final
                // and word-initial by rule R3 in
                // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165, so we do
                // not have to worry about checking the skeleton for the same significant hyphen
                // twice.
                إذا (آخرین_Char == '-') {
                    إذا (*skeletonIterator++ != آخرین_Char) {
                        أرجع خطأ;
                    }
                }
                آخرین_Char = c;
                أرجع درست;
            } وإلا إذا (c == '-') {
                إذا (آخرین_Char == ' ' ||
                    (query.is1180 && skeletonIterator == query.skeleton.انتها() - 2)) {
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
                آخرین_Char = c;
                أرجع درست;
            } وإلا {
                إذا (*skeletonIterator++ != c) {
                    أرجع خطأ;
                }
                آخرین_Char = c;
                أرجع درست;
            }
        }

        منطقي consistentWith(ثابت std::string_view substring) {
            لكل (ثابت حرف c : substring) {
                إذا (!consistentWith(c)) {
                    أرجع خطأ;
                }
            }
            أرجع درست;
        }

        منطقي matches() {
            // If a character name could end with a HYPHEN-MINUS, that HYPHEN-MINUS would be
            // significant: we would need to check that if lastChar == '-',
            // *skeletonIterator == '-', and then advance skeletonIterator.
            // However, this is disallowed by rule R3 in
            // https://unicode.org/versions/Unicode17.0.0/core-spec/chapter-4/#G135165.
            U_ASSERT(آخرین_Char != '-');
            أرجع skeletonIterator == query.skeleton.انتها();
        }

        std::string_view مانده_Significant_Characters() ثابت {
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
        حرف آخرین_Char = ' ';
    };

    صریح CharacterNameQuery(std::string_view query) {
        // Construct a skeleton obtained by
        // 1. removing medial hyphens (except the one in the name of U+1180);
        // 2. removing spaces and underscores;
        // 3. uppercasing,
        // as described in https://www.unicode.org/reports/tr44/#UAX44-LM2.
        // We do all three in a single pass.
        حرف *skeleton_حد = skeleton_داده;
        لكل (std::size_t i = 0; i < query.length(); ++i) {
            U_ASSERT(skeleton_حد < skeleton_داده + حجم(skeleton_داده));
            إذا (skeleton_حد >= skeleton_داده + حجم(skeleton_داده)) {
                // The caller should limit the query length appropriately; if they do not, assert,
                // and if assertions are disabled, query for the empty string (which will quickly
                // find nothing).
                skeleton_حد = skeleton_داده;
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
                    std::string_view(skeleton_داده, skeleton_حد - skeleton_داده) == "HANGULJUNGSEONGO") {
                    is1180MedialHyphen = درست;
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
        skeleton = std::string_view(skeleton_داده, skeleton_حد - skeleton_داده);
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

    حرف skeleton_داده[120];
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
#تعریف_کن GET_GROUPS(نامها) (ثابت uint16_t *)((ثابت حرف *)نامها+نامها->groups_جابه_جایی)

تعريفنوع بنية {
    ثابت CharacterNameQuery *otherName;
    UChar32 کد;
} پیدا_کن_Name;

#تعریف_کن DO_پیدا_کن_NAME اشاره_تهی

ساكن UData_حافظه *u_Char_نامها_داده=اشاره_تهی;
ساكن UChar_نامها *u_Char_نامها=اشاره_تهی;
ساكن icu::UInitOnce g_Char_نامها_Init_Once {};

/*
 * Maximum length of character names (regular & 1.0).
 */
ساكن int32_t g_Max_Name_درازی=0;

/*
 * Set of chars used in character names (regular & 1.0).
 * Chars are platform-dependent (can be EBCDIC).
 */
ساكن uint32_t g_Name_مجموعه[8]={ 0 };

#تعریف_کن U_NONCHARACTER_کد_نقطه U_CHAR_CATEGORY_شمار
#تعریف_کن U_LEAD_SURROGATE U_CHAR_CATEGORY_شمار + 1
#تعریف_کن U_TRAIL_SURROGATE U_CHAR_CATEGORY_شمار + 2

#تعریف_کن U_CHAR_EXTENDED_CATEGORY_شمار (U_CHAR_CATEGORY_شمار + 3)

ساكن ثابت حرف * ثابت char_Cat_نامها[U_CHAR_EXTENDED_CATEGORY_شمار] = {
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
    إذا(u_Char_نامها_داده) {
        udata_close(u_Char_نامها_داده);
        u_Char_نامها_داده = اشاره_تهی;
    }
    إذا(u_Char_نامها) {
        u_Char_نامها = اشاره_تهی;
    }
    g_Char_نامها_Init_Once.reset();
    g_Max_Name_درازی=0;
    أرجع درست;
}

ساكن UBool U_CALLCONV
isAcceptable(فراغ * /*context*/,
             ثابت حرف * /*type*/, ثابت حرف * /*name*/,
             ثابت UDataInfo *pInfo) {
    أرجع
        pInfo->size>=20 &&
        pInfo->isBigEndian==U_IS_BIG_ENDIAN &&
        pInfo->charsetFamily==U_CHARSET_FAMILY &&
        pInfo->داده_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->داده_Format[1]==0x6e &&
        pInfo->داده_Format[2]==0x61 &&
        pInfo->داده_Format[3]==0x6d &&
        pInfo->format_نسخه[0]==1;
}

ساكن فراغ U_CALLCONV
بارگیری_Char_نامها(UError_کد &وضعیت) {
    U_ASSERT(u_Char_نامها_داده == اشاره_تهی);
    U_ASSERT(u_Char_نامها == اشاره_تهی);

    u_Char_نامها_داده = udata_openChoice(اشاره_تهی, داده_TYPE, داده_NAME, isAcceptable, اشاره_تهی, &وضعیت);
    إذا(U_FAILURE(وضعیت)) {
        u_Char_نامها_داده = اشاره_تهی;
    } وإلا {
        u_Char_نامها = (UChar_نامها *)udata_get_حافظه(u_Char_نامها_داده);
    }
    ucln_common_ثبات_Cleanup(UCLN_COMMON_UNAMES, unames_cleanup);
}


ساكن UBool
is_داده_Loaded(UError_کد *p_Error_کد) {
    umtx_initOnce(g_Char_نامها_Init_Once, &بارگیری_Char_نامها, *p_Error_کد);
    أرجع U_SUCCESS(*p_Error_کد);
}

#تعریف_کن WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c) UPRV_بلوک_MACRO_BEGIN { \
    إذا((میانگیر_درازی)>0) { \
        *(میانگیر)++=c; \
        --(میانگیر_درازی); \
    } \
    ++(میانگیر_Pos); \
} UPRV_بلوک_MACRO_انتها

#تعریف_کن U_ISO_COMMENT U_CHAR_NAME_CHOICE_شمار

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
expandName(UChar_نامها *نامها,
           ثابت uint8_t *name, uint16_t name_درازی, UCharNameChoice nameChoice,
           حرف *میانگیر, uint16_t میانگیر_درازی) {
    uint16_t* نشانهها = reinterpret_cast<uint16_t*>(نامها) + 8;
    uint16_t token, token_شمار=*نشانهها++, میانگیر_Pos=0;
    uint8_t* token_رشتهها = reinterpret_cast<uint8_t*>(نامها) + نامها->token_String_جابه_جایی;
    uint8_t c;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_شمار || نشانهها[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح فیلد_شاخص= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_درازی>0) {
                    --name_درازی;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--فیلد_شاخص>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_درازی=0;
        }
    }

    /* write each letter directly, and write a token word per token */
    طالما(name_درازی>0) {
        --name_درازی;
        c=*name++;

        إذا(c>=token_شمار) {
            إذا(c!=';') {
                /* implicit letter */
                WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
            } وإلا {
                /*پایانیافته*/
                اكسر;
            }
        } وإلا {
            token=نشانهها[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=نشانهها[c<<8|*name++];
                --name_درازی;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
                } وإلا {
                    /*پایانیافته*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                uint8_t *tokenString=token_رشتهها+token;
                طالما((c=*tokenString++)!=0) {
                    WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
                }
            }
        }
    }

    /* zero-terminate */
    إذا(میانگیر_درازی>0) {
        *میانگیر=0;
    }

    أرجع میانگیر_Pos;
}

/*
 * compareName() is almost the same as expandName() except that it compares
 * the currently expanded name to an input name.
 * It returns the match/no match result as soon as possible.
 */
ساكن UBool
compareName(UChar_نامها *نامها,
            ثابت uint8_t *name, uint16_t name_درازی, UCharNameChoice nameChoice,
            ثابت CharacterNameQuery& otherName) {
    uint16_t* نشانهها = reinterpret_cast<uint16_t*>(نامها) + 8;
    uint16_t token, token_شمار=*نشانهها++;
    uint8_t* token_رشتهها = reinterpret_cast<uint8_t*>(نامها) + نامها->token_String_جابه_جایی;
    uint8_t c;
    خودکار matcher = otherName.matcher();

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /*
         * skip the modern name if it is not requested _and_
         * if the semicolon byte value is a character, not a token number
         */
        إذا (static_cast<uint8_t>(';') >= token_شمار || نشانهها[static_cast<uint8_t>(';')] == static_cast<uint16_t>(-1)) {
            صحيح فیلد_شاخص= nameChoice==U_ISO_COMMENT ? 2 : nameChoice;
            افعل {
                طالما(name_درازی>0) {
                    --name_درازی;
                    إذا(*name++==';') {
                        اكسر;
                    }
                }
            } طالما(--فیلد_شاخص>0);
        } وإلا {
            /*
             * the semicolon byte value is a token number, therefore
             * only modern names are stored in unames.dat and there is no
             * such requested alternate name here
             */
            name_درازی=0;
        }
    }

    /* compare each letter directly, and compare a token word per token */
    طالما(name_درازی>0) {
        --name_درازی;
        c=*name++;

        إذا(c>=token_شمار) {
            إذا(c!=';') {
                /* implicit letter */
                إذا (!matcher.consistentWith(c)) {
                    أرجع خطأ;
                }
            } وإلا {
                /*پایانیافته*/
                اكسر;
            }
        } وإلا {
            token=نشانهها[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                token=نشانهها[c<<8|*name++];
                --name_درازی;
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                إذا(c!=';') {
                    /* explicit letter */
                    إذا (!matcher.consistentWith(c)) {
                        أرجع خطأ;
                    }
                } وإلا {
                    /*پایانیافته*/
                    اكسر;
                }
            } وإلا {
                /* write token word */
                إذا (!matcher.consistentWith(reinterpret_cast<ثابت حرف *>(token_رشتهها + token))) {
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
        أرجع U_NONCHARACTER_کد_نقطه;
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

    إذا (cat >= UPRV_LENGTHOF(char_Cat_نامها)) {
        أرجع "unknown";
    } وإلا {
        أرجع char_Cat_نامها[cat];
    }
}

ساكن uint16_t getExtName(uint32_t کد, حرف *میانگیر, uint16_t میانگیر_درازی) {
    ثابت حرف *catname = getCharCatName(کد);
    uint16_t length = 0;

    UChar32 cp;
    صحيح ndigits, i;
    
    WRITE_CHAR(میانگیر, میانگیر_درازی, length, '<');
    طالما (catname[length - 1]) {
        WRITE_CHAR(میانگیر, میانگیر_درازی, length, catname[length - 1]);
    }
    WRITE_CHAR(میانگیر, میانگیر_درازی, length, '-');
    لكل (cp = کد, ndigits = 0; cp; ++ndigits, cp >>= 4)
        ;
    إذا (ndigits < 4)
        ndigits = 4;
    لكل (cp = کد, i = ndigits; (cp || i > 0) && میانگیر_درازی; cp >>= 4, میانگیر_درازی--) {
        uint8_t v = static_cast<uint8_t>(cp & 0xf);
        میانگیر[--i] = (v < 10 ? '0' + v : 'A' + v - 10);
    }
    میانگیر += ndigits;
    length += static_cast<uint16_t>(ndigits);
    WRITE_CHAR(میانگیر, میانگیر_درازی, length, '>');

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
get_گروه(UChar_نامها *نامها, uint32_t کد) {
    ثابت uint16_t *groups=GET_GROUPS(نامها);
    uint16_t گروه_MSB_2 = static_cast<uint16_t>(کد >> گروه_لغزش),
             شروع=0,
             حد=*groups++,
             number;

    /* binary search for the group of names that contains the one for code */
    طالما(شروع<حد-1) {
        number = static_cast<uint16_t>((شروع + حد) / 2);
        إذا(گروه_MSB_2<groups[number*گروه_درازی+گروه_MSB]) {
            حد=number;
        } وإلا {
            شروع=number;
        }
    }

    /* return this regardless of whether it is an exact match */
    أرجع groups+شروع*گروه_درازی;
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
expand_گروه_Lengths(ثابت uint8_t *s,
                   uint16_t offsets[سطرها_در_هر_گروه+1], uint16_t lengths[سطرها_در_هر_گروه+1]) {
    /* read the lengths of the 32 strings in this group and get each string's offset */
    uint16_t i=0, جابه_جایی=0, length=0;
    uint8_t درازی_Byte;

    /* all 32 lengths must be read to get the offset of the first group string */
    طالما(i<سطرها_در_هر_گروه) {
        درازی_Byte=*s++;

        /* read even nibble - MSBs of lengthByte */
        إذا(length>=12) {
            /* double-nibble length spread across two bytes */
            length = static_cast<uint16_t>(((length & 0x3) << 4 | درازی_Byte >> 4) + 12);
            درازی_Byte&=0xf;
        } وإلا إذا((درازی_Byte /* &0xf0 */)>=0xc0) {
            /* double-nibble length spread across this one byte */
            length = static_cast<uint16_t>((درازی_Byte & 0x3f) + 12);
        } وإلا {
            /* single-nibble length in MSBs */
            length = static_cast<uint16_t>(درازی_Byte >> 4);
            درازی_Byte&=0xf;
        }

        *offsets++=جابه_جایی;
        *lengths++=length;

        جابه_جایی+=length;
        ++i;

        /* read odd nibble - LSBs of lengthByte */
        إذا((درازی_Byte&0xf0)==0) {
            /* this nibble was not consumed for a double-nibble length above */
            length=درازی_Byte;
            إذا(length<12) {
                /* single-nibble length in LSBs */
                *offsets++=جابه_جایی;
                *lengths++=length;

                جابه_جایی+=length;
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
expand_گروه_Name(UChar_نامها *نامها, ثابت uint16_t *گروه,
                uint16_t سطر_عدد, UCharNameChoice nameChoice,
                حرف *میانگیر, uint16_t میانگیر_درازی) {
    uint16_t offsets[سطرها_در_هر_گروه+2], lengths[سطرها_در_هر_گروه+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نامها) + نامها->گروه_String_جابه_جایی + GET_گروه_جابه_جایی(گروه);
    s=expand_گروه_Lengths(s, offsets, lengths);
    أرجع expandName(نامها, s+offsets[سطر_عدد], lengths[سطر_عدد], nameChoice,
                      میانگیر, میانگیر_درازی);
}

ساكن uint16_t
getName(UChar_نامها *نامها, uint32_t کد, UCharNameChoice nameChoice,
        حرف *میانگیر, uint16_t میانگیر_درازی) {
    ثابت uint16_t *گروه=get_گروه(نامها, کد);
    إذا (static_cast<uint16_t>(کد >> گروه_لغزش) == گروه[گروه_MSB]) {
        أرجع expand_گروه_Name(نامها, گروه, static_cast<uint16_t>(کد & گروه_ماسک), nameChoice,
                               میانگیر, میانگیر_درازی);
    } وإلا {
        /* group not found */
        /* zero-terminate */
        إذا(میانگیر_درازی>0) {
            *میانگیر=0;
        }
        أرجع 0;
    }
}

/*
 * enumGroupNames() enumerates all the names in a 32-group
 * and either calls the enumerator function or finds a given input name.
 */
ساكن UBool
enum_گروه_نامها(UChar_نامها *نامها, ثابت uint16_t *گروه,
               UChar32 شروع, UChar32 انتها,
               UEnum_Char_نامها_Fn *fn, فراغ *context,
               UCharNameChoice nameChoice) {
    uint16_t offsets[سطرها_در_هر_گروه+2], lengths[سطرها_در_هر_گروه+2];
    ثابت uint8_t* s = reinterpret_cast<uint8_t*>(نامها) + نامها->گروه_String_جابه_جایی + GET_گروه_جابه_جایی(گروه);

    s=expand_گروه_Lengths(s, offsets, lengths);
    إذا(fn!=DO_پیدا_کن_NAME) {
        حرف میانگیر[200];
        uint16_t length;

        طالما(شروع<=انتها) {
            length=expandName(نامها, s+offsets[شروع&گروه_ماسک], lengths[شروع&گروه_ماسک], nameChoice, میانگیر, حجم(میانگیر));
            إذا (!length && nameChoice == U_EXTENDED_CHAR_NAME) {
                میانگیر[length = getExtName(شروع, میانگیر, حجم(میانگیر))] = 0;
            }
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروع, nameChoice, میانگیر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروع;
        }
    } وإلا {
        ثابت CharacterNameQuery& otherName = *static_cast<پیدا_کن_Name*>(context)->otherName;
        طالما(شروع<=انتها) {
            إذا(compareName(نامها, s+offsets[شروع&گروه_ماسک], lengths[شروع&گروه_ماسک], nameChoice, otherName)) {
                static_cast<پیدا_کن_Name*>(context)->کد = شروع;
                أرجع خطأ;
            }
            ++شروع;
        }
    }
    أرجع درست;
}

/*
 * enumExtNames enumerate extended names.
 * It only needs to do it if it is called with a real function and not
 * with the dummy DO_FIND_NAME, because u_charFromName() does a check
 * for extended names by itself.
 */ 
ساكن UBool
enum_Ext_نامها(UChar32 شروع, UChar32 انتها,
             UEnum_Char_نامها_Fn *fn, فراغ *context)
{
    إذا(fn!=DO_پیدا_کن_NAME) {
        حرف میانگیر[200];
        uint16_t length;
        
        طالما(شروع<=انتها) {
            میانگیر[length = getExtName(شروع, میانگیر, حجم(میانگیر))] = 0;
            /* here, we assume that the buffer is large enough */
            إذا(length>0) {
                إذا(!fn(context, شروع, U_EXTENDED_CHAR_NAME, میانگیر, length)) {
                    أرجع خطأ;
                }
            }
            ++شروع;
        }
    }

    أرجع درست;
}

ساكن UBool
enum_نامها(UChar_نامها *نامها,
          UChar32 شروع, UChar32 حد,
          UEnum_Char_نامها_Fn *fn, فراغ *context,
          UCharNameChoice nameChoice) {
    uint16_t شروع_گروه_MSB, انتها_گروه_MSB, گروه_شمار;
    ثابت uint16_t *گروه, *گروه_حد;

    شروع_گروه_MSB = static_cast<uint16_t>(شروع >> گروه_لغزش);
    انتها_گروه_MSB = static_cast<uint16_t>((حد - 1) >> گروه_لغزش);

    /* find the group that contains start, or the highest before it */
    گروه=get_گروه(نامها, شروع);

    إذا(شروع_گروه_MSB<گروه[گروه_MSB] && nameChoice==U_EXTENDED_CHAR_NAME) {
        /* enumerate synthetic names between start and the group start */
        UChar32 ext_حد = static_cast<UChar32>(گروه[گروه_MSB]) << گروه_لغزش;
        إذا(ext_حد>حد) {
            ext_حد=حد;
        }
        إذا(!enum_Ext_نامها(شروع, ext_حد-1, fn, context)) {
            أرجع خطأ;
        }
        شروع=ext_حد;
    }

    إذا(شروع_گروه_MSB==انتها_گروه_MSB) {
        إذا(شروع_گروه_MSB==گروه[گروه_MSB]) {
            /* if start and limit-1 are in the same group, then enumerate only in that one */
            أرجع enum_گروه_نامها(نامها, گروه, شروع, حد-1, fn, context, nameChoice);
        }
    } وإلا {
        ثابت uint16_t *groups=GET_GROUPS(نامها);
        گروه_شمار=*groups++;
        گروه_حد=groups+گروه_شمار*گروه_درازی;

        إذا(شروع_گروه_MSB==گروه[گروه_MSB]) {
            /* enumerate characters in the partial start group */
            إذا((شروع&گروه_ماسک)!=0) {
                إذا(!enum_گروه_نامها(نامها, گروه,
                                   شروع, (static_cast<UChar32>(شروع_گروه_MSB) << گروه_لغزش) + سطرها_در_هر_گروه - 1,
                                   fn, context, nameChoice)) {
                    أرجع خطأ;
                }
                گروه=بعدی_گروه(گروه); /* continue with the next group */
            }
        } وإلا إذا(شروع_گروه_MSB>گروه[گروه_MSB]) {
            /* make sure that we start enumerating with the first group after start */
            ثابت uint16_t *بعدی_گروه_2=بعدی_گروه(گروه);
            إذا (بعدی_گروه_2 < گروه_حد && بعدی_گروه_2[گروه_MSB] > شروع_گروه_MSB && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 انتها = بعدی_گروه_2[گروه_MSB] << گروه_لغزش;
                إذا (انتها > حد) {
                    انتها = حد;
                }
                إذا (!enum_Ext_نامها(شروع, انتها - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروه=بعدی_گروه_2;
        }

        /* enumerate entire groups between the start- and end-groups */
        طالما(گروه<گروه_حد && گروه[گروه_MSB]<انتها_گروه_MSB) {
            ثابت uint16_t *بعدی_گروه_2;
            شروع = static_cast<UChar32>(گروه[گروه_MSB]) << گروه_لغزش;
            إذا(!enum_گروه_نامها(نامها, گروه, شروع, شروع+سطرها_در_هر_گروه-1, fn, context, nameChoice)) {
                أرجع خطأ;
            }
            بعدی_گروه_2=بعدی_گروه(گروه);
            إذا (بعدی_گروه_2 < گروه_حد && بعدی_گروه_2[گروه_MSB] > گروه[گروه_MSB] + 1 && nameChoice == U_EXTENDED_CHAR_NAME) {
                UChar32 انتها = بعدی_گروه_2[گروه_MSB] << گروه_لغزش;
                إذا (انتها > حد) {
                    انتها = حد;
                }
                إذا (!enum_Ext_نامها((گروه[گروه_MSB] + 1) << گروه_لغزش, انتها - 1, fn, context)) {
                    أرجع خطأ;
                }
            }
            گروه=بعدی_گروه_2;
        }

        /* enumerate within the end group (group[GROUP_MSB]==endGroupMSB) */
        إذا(گروه<گروه_حد && گروه[گروه_MSB]==انتها_گروه_MSB) {
            أرجع enum_گروه_نامها(نامها, گروه, (حد-1)&~گروه_ماسک, حد-1, fn, context, nameChoice);
        } وإلا إذا (nameChoice == U_EXTENDED_CHAR_NAME && گروه == گروه_حد) {
            UChar32 next = (PREV_گروه(گروه)[گروه_MSB] + 1) << گروه_لغزش;
            إذا (next > شروع) {
                شروع = next;
            }
        } وإلا {
            أرجع درست;
        }
    }

    /* we have not found a group, which means everything is made of
       extended names. */
    إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
        إذا (حد > UCHAR_MAX_مقدار + 1) {
            حد = UCHAR_MAX_مقدار + 1;
        }
        أرجع enum_Ext_نامها(شروع, حد - 1, fn, context);
    }
    
    أرجع درست;
}

ساكن uint16_t
write_ضریب_Suffix(ثابت uint16_t *factors, uint16_t count,
                  ثابت حرف *s, /* suffix elements */
                  uint32_t کد,
                  uint16_t indexes[8], /* output fields from here */
                  ثابت حرف *elementBases[8], ثابت حرف *elements[8],
                  حرف *میانگیر, uint16_t میانگیر_درازی) {
    uint16_t i, ضریب, میانگیر_Pos=0;
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
        indexes[i] = static_cast<uint16_t>(کد % ضریب);
        کد/=ضریب;
    }
    /*
     * we don't need to calculate the last modulus because start<=code<=end
     * guarantees here that code<=factors[0]
     */
    indexes[0] = static_cast<uint16_t>(کد);

    /* write each element */
    لكل(;;) {
        إذا(elementBases!=اشاره_تهی) {
            *elementBases++=s;
        }

        /* skip indexes[i] strings */
        ضریب=indexes[i];
        طالما(ضریب>0) {
            طالما(*s++!=0) {}
            --ضریب;
        }
        إذا(elements!=اشاره_تهی) {
            *elements++=s;
        }

        /* write element */
        طالما((c=*s++)!=0) {
            WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
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
    إذا(میانگیر_درازی>0) {
        *میانگیر=0;
    }

    أرجع میانگیر_Pos;
}

/*
 * Important:
 * Parts of findAlgName() are almost the same as some of getAlgName().
 * Fixes must be applied to both.
 */
ساكن uint16_t
getAlgName(AlgorithmicRange *range, uint32_t کد, UCharNameChoice nameChoice,
        حرف *میانگیر, uint16_t میانگیر_درازی) {
    uint16_t میانگیر_Pos=0;

    /* Only the normative character name can be algorithmic. */
    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        /* zero-terminate */
        إذا(میانگیر_درازی>0) {
            *میانگیر=0;
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
            WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
        }

        /* write hexadecimal code point value */
        count=range->variant;

        /* zero-terminate */
        إذا(count<میانگیر_درازی) {
            میانگیر[count]=0;
        }

        لكل(i=count; i>0;) {
            إذا(--i<میانگیر_درازی) {
                c = static_cast<حرف>(کد & 0xf);
                إذا(c<10) {
                    c+='0';
                } وإلا {
                    c+='A'-10;
                }
                میانگیر[i]=c;
            }
            کد>>=4;
        }

        میانگیر_Pos+=count;
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
            WRITE_CHAR(میانگیر, میانگیر_درازی, میانگیر_Pos, c);
        }

        میانگیر_Pos+=write_ضریب_Suffix(factors, count,
                                     s, کد-range->شروع, indexes, اشاره_تهی, اشاره_تهی, میانگیر, میانگیر_درازی);
        اكسر;
    }
    افتراضي:
        /* undefined type */
        /* zero-terminate */
        إذا(میانگیر_درازی>0) {
            *میانگیر=0;
        }
        اكسر;
    }

    أرجع میانگیر_Pos;
}

/*
 * Important: enumAlgNames() and findAlgName() are almost the same.
 * Any fix must be applied to both.
 */
ساكن UBool
enum_Alg_نامها(AlgorithmicRange *range,
             UChar32 شروع, UChar32 حد,
             UEnum_Char_نامها_Fn *fn, فراغ *context,
             UCharNameChoice nameChoice) {
    حرف میانگیر[200];
    uint16_t length;

    إذا(nameChoice!=U_UNICODE_CHAR_NAME && nameChoice!=U_EXTENDED_CHAR_NAME) {
        أرجع درست;
    }

    بدل(range->type) {
    حالة 0: {
        حرف *s, *انتها;
        حرف c;

        /* get the full name of the start character */
        length = getAlgName(range, static_cast<uint32_t>(شروع), nameChoice, میانگیر, حجم(میانگیر));
        إذا(length<=0) {
            أرجع درست;
        }

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروع, nameChoice, میانگیر, length)) {
            أرجع خطأ;
        }

        /* go to the end of the name; all these names have the same length */
        انتها=میانگیر;
        طالما(*انتها!=0) {
            ++انتها;
        }

        /* enumerate the rest of the names */
        طالما(++شروع<حد) {
            /* increment the hexadecimal number on a character-basis */
            s=انتها;
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

            إذا(!fn(context, شروع, nameChoice, میانگیر, length)) {
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
        uint16_t پیشوند_درازی, i, idx;

        حرف c;

        /* name = prefix factorized-elements */

        /* copy prefix */
        suffix=میانگیر;
        پیشوند_درازی=0;
        طالما((c=*s++)!=0) {
            *suffix++=c;
            ++پیشوند_درازی;
        }

        /* append the suffix of the start character */
        length = static_cast<uint16_t>(پیشوند_درازی + write_ضریب_Suffix(factors, count,
                                              s, static_cast<uint32_t>(شروع) - range->شروع,
                                              indexes, elementBases, elements,
                                              suffix, static_cast<uint16_t>(حجم(میانگیر) - پیشوند_درازی)));

        /* call the enumerator function with this first character */
        إذا(!fn(context, شروع, nameChoice, میانگیر, length)) {
            أرجع خطأ;
        }

        /* enumerate the rest of the names */
        طالما(++شروع<حد) {
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
            length=پیشوند_درازی;
            لكل(i=0; i<count; ++i) {
                s=elements[i];
                طالما((c=*s++)!=0) {
                    *t++=c;
                    ++length;
                }
            }
            /* zero-terminate */
            *t=0;

            إذا(!fn(context, شروع, nameChoice, میانگیر, length)) {
                أرجع خطأ;
            }
        }
        اكسر;
    }
    افتراضي:
        /* undefined type */
        اكسر;
    }

    أرجع درست;
}

/*
 * findAlgName() is almost the same as enumAlgNames() except that it
 * returns the code point for a name if it fits into the range.
 * It returns 0xffff otherwise.
 */
ساكن UChar32
پیدا_کن_Alg_Name(AlgorithmicRange *range, UCharNameChoice nameChoice, CharacterNameQuery& query) {
    UChar32 کد;
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
        کد=0;
        لكل(حرف c : matcher.مانده_Significant_Characters()) {
            إذا('0'<=c && c<='9') {
                کد=(کد<<4)|(c-'0');
            } وإلا إذا('A'<=c && c<='F') {
                کد=(کد<<4)|(c-'A'+10);
            } وإلا {
                أرجع 0xffff;
            }
        }

        /* does it fit into the range? */
        إذا (range->شروع <= static_cast<uint32_t>(کد) && static_cast<uint32_t>(کد) <= range->انتها) {
            أرجع کد;
        }
        اكسر;
    }
    حالة 1: {
        حرف میانگیر[64];
        uint16_t indexes[8];
        ثابت حرف *elementBases[8], *elements[8];
        ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
        uint16_t count=range->variant;
        ثابت حرف *s = reinterpret_cast<ثابت حرف*>(factors + count);
        UChar32 شروع, حد;
        uint16_t i, idx;

        /* name = prefix factorized-elements */

        /* compare prefix */
        std::string_view پیشوند = s;
        إذا (!matcher.consistentWith(پیشوند)) {
            أرجع 0xffff;
        }

        s += پیشوند.length() + 1;

        شروع = static_cast<UChar32>(range->شروع);
        حد = static_cast<UChar32>(range->انتها + 1);
        
        // This implementation assumes that the suffix does not contain SPACEs nor HYPHEN-MINUSes,
        // which is the case in practice for the names of Hangul syllables.
        std::string_view querySuffix = matcher.مانده_Significant_Characters();
        /* initialize the suffix elements for enumeration; indexes should all be set to 0 */
        write_ضریب_Suffix(factors, count, s, 0,
                          indexes, elementBases, elements, میانگیر, حجم(میانگیر));

        /* compare the first suffix */
        إذا(querySuffix == میانگیر) {
            أرجع شروع;
        }

        /* enumerate and compare the rest of the suffixes */
        طالما(++شروع<حد) {
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
                    إذا(it == querySuffix.انتها() || c!=*it++) {
                        s=""; /* does not match */
                        i=99;
                    }
                }
            }
            إذا(i<99 && it == querySuffix.انتها()) {
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

#تعریف_کن مجموعه_جمع_کن_3(مجموعه, c) ((مجموعه)[(uint8_t)c>>5]|=((uint32_t)1<<((uint8_t)c&0x1f)))
#تعریف_کن مجموعه_شامل_است(مجموعه, c) (((مجموعه)[(uint8_t)c>>5]&((uint32_t)1<<((uint8_t)c&0x1f)))!=0)

ساكن int32_t
calc_String_مجموعه_درازی(uint32_t مجموعه[8], ثابت حرف *s) {
    int32_t length=0;
    حرف c;

    طالما((c=*s++)!=0) {
        مجموعه_جمع_کن_3(مجموعه, c);
        ++length;
    }
    أرجع length;
}

ساكن int32_t
calcAlgNameSetsLengths(int32_t max_Name_درازی) {
    AlgorithmicRange *range;
    uint32_t *p;
    uint32_t range_شمار;
    int32_t length;

    /* enumerate algorithmic ranges */
    p = reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(u_Char_نامها) + u_Char_نامها->alg_نامها_جابه_جایی);
    range_شمار=*p;
    range = reinterpret_cast<AlgorithmicRange*>(p + 1);
    طالما(range_شمار>0) {
        بدل(range->type) {
        حالة 0:
            /* name = prefix + (range->variant times) hex-digits */
            /*پیشوند*/
            length = calc_String_مجموعه_درازی(g_Name_مجموعه, reinterpret_cast<ثابت حرف*>(range + 1)) + range->variant;
            إذا(length>max_Name_درازی) {
                max_Name_درازی=length;
            }
            اكسر;
        حالة 1: {
            /* name = prefix factorized-elements */
            ثابت uint16_t* factors = reinterpret_cast<ثابت uint16_t*>(range + 1);
            ثابت حرف *s;
            int32_t i, count=range->variant, ضریب, ضریب_درازی, max_ضریب_درازی;

            /* prefix length */
            s = reinterpret_cast<ثابت حرف*>(factors + count);
            length=calc_String_مجموعه_درازی(g_Name_مجموعه, s);
            s+=length+1; /* start of factor suffixes */

            /* get the set and maximum factor suffix length for each factor */
            لكل(i=0; i<count; ++i) {
                max_ضریب_درازی=0;
                لكل(ضریب=factors[i]; ضریب>0; --ضریب) {
                    ضریب_درازی=calc_String_مجموعه_درازی(g_Name_مجموعه, s);
                    s+=ضریب_درازی+1;
                    إذا(ضریب_درازی>max_ضریب_درازی) {
                        max_ضریب_درازی=ضریب_درازی;
                    }
                }
                length+=max_ضریب_درازی;
            }

            إذا(length>max_Name_درازی) {
                max_Name_درازی=length;
            }
            اكسر;
        }
        افتراضي:
            /* unknown type */
            اكسر;
        }

        range = reinterpret_cast<AlgorithmicRange*>(reinterpret_cast<uint8_t*>(range) + range->size);
        --range_شمار;
    }
    أرجع max_Name_درازی;
}

ساكن int32_t
calcExtNameSetsLengths(int32_t max_Name_درازی) {
    int32_t i, length;

    لكل(i=0; i<UPRV_LENGTHOF(char_Cat_نامها); ++i) {
        /*
         * for each category, count the length of the category name
         * plus 9=
         * 2 for <>
         * 1 for -
         * 6 for most hex digits per code point
         */
        length=9+calc_String_مجموعه_درازی(g_Name_مجموعه, char_Cat_نامها[i]);
        إذا(length>max_Name_درازی) {
            max_Name_درازی=length;
        }
    }
    أرجع max_Name_درازی;
}

ساكن int32_t
calc_Name_مجموعه_درازی(ثابت uint16_t *نشانهها, uint16_t token_شمار, ثابت uint8_t *token_رشتهها, int8_t *tokenLengths,
                  uint32_t مجموعه[8],
                  ثابت uint8_t **p_سطر, ثابت uint8_t *سطر_حد) {
    ثابت uint8_t *سطر=*p_سطر;
    int32_t length=0, token_درازی;
    uint16_t c, token;

    طالما (سطر != سطر_حد && (c = *سطر++) != static_cast<uint8_t>(';')) {
        إذا(c>=token_شمار) {
            /* implicit letter */
            مجموعه_جمع_کن_3(مجموعه, c);
            ++length;
        } وإلا {
            token=نشانهها[c];
            إذا (token == static_cast<uint16_t>(-2)) {
                /* this is a lead byte for a double-byte token */
                c=c<<8|*سطر++;
                token=نشانهها[c];
            }
            إذا (token == static_cast<uint16_t>(-1)) {
                /* explicit letter */
                مجموعه_جمع_کن_3(مجموعه, c);
                ++length;
            } وإلا {
                /* count token word */
                إذا(tokenLengths!=اشاره_تهی) {
                    /* use cached token length */
                    token_درازی=tokenLengths[c];
                    إذا(token_درازی==0) {
                        token_درازی = calc_String_مجموعه_درازی(مجموعه, reinterpret_cast<ثابت حرف*>(token_رشتهها) + token);
                        tokenLengths[c] = static_cast<int8_t>(token_درازی);
                    }
                } وإلا {
                    token_درازی = calc_String_مجموعه_درازی(مجموعه, reinterpret_cast<ثابت حرف*>(token_رشتهها) + token);
                }
                length+=token_درازی;
            }
        }
    }

    *p_سطر=سطر;
    أرجع length;
}

ساكن فراغ
calc_گروه_Name_Sets_Lengths(int32_t max_Name_درازی) {
    uint16_t offsets[سطرها_در_هر_گروه+2], lengths[سطرها_در_هر_گروه+2];

    uint16_t* نشانهها = reinterpret_cast<uint16_t*>(u_Char_نامها) + 8;
    uint16_t token_شمار=*نشانهها++;
    uint8_t* token_رشتهها = reinterpret_cast<uint8_t*>(u_Char_نامها) + u_Char_نامها->token_String_جابه_جایی;

    int8_t *tokenLengths;

    ثابت uint16_t *گروه;
    ثابت uint8_t *s, *سطر, *سطر_حد;

    int32_t گروه_شمار, سطر_عدد, length;

    tokenLengths = static_cast<int8_t*>(uprv_malloc(token_شمار));
    إذا(tokenLengths!=اشاره_تهی) {
        uprv_memset(tokenLengths, 0, token_شمار);
    }

    گروه=GET_GROUPS(u_Char_نامها);
    گروه_شمار=*گروه++;

    /* enumerate all groups */
    طالما(گروه_شمار>0) {
        s = reinterpret_cast<uint8_t*>(u_Char_نامها) + u_Char_نامها->گروه_String_جابه_جایی + GET_گروه_جابه_جایی(گروه);
        s=expand_گروه_Lengths(s, offsets, lengths);

        /* enumerate all lines in each group */
        لكل(سطر_عدد=0; سطر_عدد<سطرها_در_هر_گروه; ++سطر_عدد) {
            سطر=s+offsets[سطر_عدد];
            length=lengths[سطر_عدد];
            إذا(length==0) {
                استمر;
            }

            سطر_حد=سطر+length;

            /* read regular name */
            length=calc_Name_مجموعه_درازی(نشانهها, token_شمار, token_رشتهها, tokenLengths, g_Name_مجموعه, &سطر, سطر_حد);
            إذا(length>max_Name_درازی) {
                max_Name_درازی=length;
            }
            إذا(سطر==سطر_حد) {
                استمر;
            }

            /* read Unicode 1.0 name */
            length=calc_Name_مجموعه_درازی(نشانهها, token_شمار, token_رشتهها, tokenLengths, g_Name_مجموعه, &سطر, سطر_حد);
            إذا(length>max_Name_درازی) {
                max_Name_درازی=length;
            }
            إذا(سطر==سطر_حد) {
                استمر;
            }

            /* read ISO comment */
            /*length=calcNameSetLength(tokens, tokenCount, tokenStrings, tokenLengths, gISOCommentSet, &line, lineLimit);*/
        }

        گروه=بعدی_گروه(گروه);
        --گروه_شمار;
    }

    إذا(tokenLengths!=اشاره_تهی) {
        uprv_free(tokenLengths);
    }

    /* set gMax... - name length last for threading */
    g_Max_Name_درازی=max_Name_درازی;
}

ساكن UBool
calcNameSetsLengths(UError_کد *p_Error_کد) {
    ساكن ثابت حرف extChars[]="0123456789ABCDEF<>-";
    int32_t i, max_Name_درازی;

    إذا(g_Max_Name_درازی!=0) {
        أرجع درست;
    }

    إذا(!is_داده_Loaded(p_Error_کد)) {
        أرجع خطأ;
    }

    /* set hex digits, used in various names, and <>-, used in extended names */
    لكل (i = 0; i < static_cast<int32_t>(حجم(extChars)) - 1; ++i) {
        مجموعه_جمع_کن_3(g_Name_مجموعه, extChars[i]);
    }

    /* set sets and lengths from algorithmic names */
    max_Name_درازی=calcAlgNameSetsLengths(0);

    /* set sets and lengths from extended names */
    max_Name_درازی=calcExtNameSetsLengths(max_Name_درازی);

    /* set sets and lengths from group names, set global maximum values */
    calc_گروه_Name_Sets_Lengths(max_Name_درازی);

    أرجع درست;
}

U_NAMESPACE_انتها

/* public API --------------------------------------------------------------- */

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
u_charName(UChar32 کد, UCharNameChoice nameChoice,
           حرف *میانگیر, int32_t میانگیر_درازی,
           UError_کد *p_Error_کد) {
     AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    int32_t length;

    /* check the argument values */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    } وإلا إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمار ||
              میانگیر_درازی<0 || (میانگیر_درازی>0 && میانگیر==اشاره_تهی)
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا((uint32_t)کد>UCHAR_MAX_مقدار || !is_داده_Loaded(p_Error_کد)) {
        أرجع u_terminateChars(میانگیر, میانگیر_درازی, 0, p_Error_کد);
    }

    length=0;

    /* try algorithmic names first */
    p=(uint32_t *)((uint8_t *)u_Char_نامها+u_Char_نامها->alg_نامها_جابه_جایی);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا(algRange->شروع<=(uint32_t)کد && (uint32_t)کد<=algRange->انتها) {
            length=getAlgName(algRange, (uint32_t)کد, nameChoice, میانگیر, (uint16_t)میانگیر_درازی);
            اكسر;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    إذا(i==0) {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME) {
            length = getName(u_Char_نامها, (uint32_t )کد, U_EXTENDED_CHAR_NAME, میانگیر, (uint16_t) میانگیر_درازی);
            إذا (!length) {
                /* extended character name */
                length = getExtName((uint32_t) کد, میانگیر, (uint16_t) میانگیر_درازی);
            }
        } وإلا {
            /* normal character name */
            length=getName(u_Char_نامها, (uint32_t)کد, nameChoice, میانگیر, (uint16_t)میانگیر_درازی);
        }
    }

    أرجع u_terminateChars(میانگیر, میانگیر_درازی, length, p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
u_getISOComment(UChar32 /*c*/,
                حرف *dest, int32_t dest_ظرفیت,
                UError_کد *p_Error_کد) {
    /* check the argument values */
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    } وإلا إذا(dest_ظرفیت<0 || (dest_ظرفیت>0 && dest==اشاره_تهی)) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    أرجع u_terminateChars(dest, dest_ظرفیت, 0, p_Error_کد);
}

U_CAPI UChar32 U_EXPORT2
u_charFromName(UCharNameChoice nameChoice,
               ثابت حرف *ثابت name,
               UError_کد *p_Error_کد) {
    حرف پایینی[120] = {0};
    پیدا_کن_Name پیدا_کن_Name_2;
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;
    UChar32 cp = 0;
    حرف c0;
    ساكن constexpr UChar32 error = 0xffff;     /* Undefined, but use this for backwards compatibility. */

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع error;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمار || name==اشاره_تهی || *name==0) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع error;
    }

    إذا(!is_داده_Loaded(p_Error_کد)) {
        أرجع error;
    }

    /* construct the lowercase of the name first */
    ثابت حرف* nameChar = name;
    لكل(i=0; i<حجم(پایینی); ++i) {
        إذا((c0=*nameChar++)!=0) {
            پایینی[i]=uprv_tolower(c0);
        } وإلا {
            اكسر;
        }
    }
    إذا(i==حجم(پایینی)) {
        /* name too long, there is no such character */
        *p_Error_کد = U_ILLEGAL_CHAR_یافت;
        أرجع error;
    }
    // i==strlen(name)==strlen(lower)

    /* try extended names first */
    // The ICU extended names are not consistent with the UAX44 code point labels, e.g.,
    // "unassigned" vs. UAX44 "reserved", "private use area" vs. UAX44 "private-use", so we do not
    // apply UAX44-LM2 matching to them.
    إذا (پایینی[0] == '<') {
        إذا (nameChoice == U_EXTENDED_CHAR_NAME && پایینی[--i] == '>') {
            // Parse a string like "<category-HHHH>" where HHHH is a hex code point.
            uint32_t حد = i;
            طالما (i >= 3 && پایینی[--i] != '-') {}

            // There should be 1 to 8 hex digits.
            int32_t hex_درازی = حد - (i + 1);
            إذا (i >= 2 && پایینی[i] == '-' && 1 <= hex_درازی && hex_درازی <= 8) {
                uint32_t cIdx;

                پایینی[i] = 0;

                لكل (++i; i < حد; ++i) {
                    إذا (پایینی[i] >= '0' && پایینی[i] <= '9') {
                        cp = (cp << 4) + پایینی[i] - '0';
                    } وإلا إذا (پایینی[i] >= 'a' && پایینی[i] <= 'f') {
                        cp = (cp << 4) + پایینی[i] - 'a' + 10;
                    } وإلا {
                        *p_Error_کد = U_ILLEGAL_CHAR_یافت;
                        أرجع error;
                    }
                    // Prevent signed-integer overflow and out-of-range code points.
                    إذا (cp > UCHAR_MAX_مقدار) {
                        *p_Error_کد = U_ILLEGAL_CHAR_یافت;
                        أرجع error;
                    }
                }

                /* Now validate the category name.
                   We could use a binary search, or a trie, if
                   we really wanted to. */
                uint8_t cat = getCharCat(cp);
                لكل (پایینی[i] = 0, cIdx = 0; cIdx < UPRV_LENGTHOF(char_Cat_نامها); ++cIdx) {

                    إذا (!uprv_strcmp(پایینی + 1, char_Cat_نامها[cIdx])) {
                        إذا (cat == cIdx) {
                            أرجع cp;
                        }
                        اكسر;
                    }
                }
            }
        }

        *p_Error_کد = U_ILLEGAL_CHAR_یافت;
        أرجع error;
    }

    // From now on we do UAX44 matching.
    CharacterNameQuery query(name);

    /* try algorithmic names now */
    p=(uint32_t *)((uint8_t *)u_Char_نامها+u_Char_نامها->alg_نامها_جابه_جایی);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        إذا((cp=پیدا_کن_Alg_Name(algRange, nameChoice, query))!=0xffff) {
            أرجع cp;
        }
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }

    /* normal character name */
    پیدا_کن_Name_2.otherName=&query;
    پیدا_کن_Name_2.کد=error;
    enum_نامها(u_Char_نامها, 0, UCHAR_MAX_مقدار + 1, DO_پیدا_کن_NAME, &پیدا_کن_Name_2, nameChoice);
    إذا (پیدا_کن_Name_2.کد == error) {
         *p_Error_کد = U_ILLEGAL_CHAR_یافت;
    }
    أرجع پیدا_کن_Name_2.کد;
}

U_CAPI فراغ U_EXPORT2
u_enum_Char_نامها(UChar32 شروع, UChar32 حد,
                UEnum_Char_نامها_Fn *fn,
                فراغ *context,
                UCharNameChoice nameChoice,
                UError_کد *p_Error_کد) {
    AlgorithmicRange *algRange;
    uint32_t *p;
    uint32_t i;

    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    إذا(nameChoice>=U_CHAR_NAME_CHOICE_شمار || fn==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    إذا((uint32_t) حد > UCHAR_MAX_مقدار + 1) {
        حد = UCHAR_MAX_مقدار + 1;
    }
    إذا((uint32_t)شروع>=(uint32_t)حد) {
        أرجع;
    }

    إذا(!is_داده_Loaded(p_Error_کد)) {
        أرجع;
    }

    /* interleave the data-driven ones with the algorithmic ones */
    /* iterate over all algorithmic ranges; assume that they are in ascending order */
    p=(uint32_t *)((uint8_t *)u_Char_نامها+u_Char_نامها->alg_نامها_جابه_جایی);
    i=*p;
    algRange=(AlgorithmicRange *)(p+1);
    طالما(i>0) {
        /* enumerate the character names before the current algorithmic range */
        /* here: start<limit */
        إذا((uint32_t)شروع<algRange->شروع) {
            إذا((uint32_t)حد<=algRange->شروع) {
                enum_نامها(u_Char_نامها, شروع, حد, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_نامها(u_Char_نامها, شروع, (UChar32)algRange->شروع, fn, context, nameChoice)) {
                أرجع;
            }
            شروع=(UChar32)algRange->شروع;
        }
        /* enumerate the character names in the current algorithmic range */
        /* here: algRange->start<=start<limit */
        إذا((uint32_t)شروع<=algRange->انتها) {
            إذا((uint32_t)حد<=(algRange->انتها+1)) {
                enum_Alg_نامها(algRange, شروع, حد, fn, context, nameChoice);
                أرجع;
            }
            إذا(!enum_Alg_نامها(algRange, شروع, (UChar32)algRange->انتها+1, fn, context, nameChoice)) {
                أرجع;
            }
            شروع=(UChar32)algRange->انتها+1;
        }
        /* continue to the next algorithmic range (here: start<limit) */
        algRange=(AlgorithmicRange *)((uint8_t *)algRange+algRange->size);
        --i;
    }
    /* enumerate the character names after the last algorithmic range */
    enum_نامها(u_Char_نامها, شروع, حد, fn, context, nameChoice);
}

U_CAPI int32_t U_EXPORT2
uprv_get_Max_Char_Name_درازی() {
    UError_کد error_کد=U_ZERO_ERROR;
    إذا(calcNameSetsLengths(&error_کد)) {
        أرجع g_Max_Name_درازی;
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
char_مجموعه_مقصد_USet(uint32_t cset[8], ثابت USetAdder *sa) {
    char16_t us[256];
    حرف cs[256];

    int32_t i, length;
    UError_کد error_کد;

    error_کد=U_ZERO_ERROR;

    إذا(!calcNameSetsLengths(&error_کد)) {
        أرجع;
    }

    /* build a char string with all chars that are used in character names */
    length=0;
    لكل(i=0; i<256; ++i) {
        إذا(مجموعه_شامل_است(cset, i)) {
            cs[length++] = static_cast<حرف>(i);
        }
    }

    /* convert the char string to a char16_t string */
    u_chars_مقصد_UChars(cs, us, length);

    /* add each char16_t to the USet */
    لكل(i=0; i<length; ++i) {
        إذا(us[i]!=0 || cs[i]==0) { /* non-invariant chars become (char16_t)0 */
            sa->جمع_کن(sa->مجموعه, us[i]);
        }
    }
}

/**
 * Fills set with characters that are used in Unicode character names.
 * @param set USet to receive characters.
 */
U_CAPI فراغ U_EXPORT2
uprv_getCharNameCharacters(ثابت USetAdder *sa) {
    char_مجموعه_مقصد_USet(g_Name_مجموعه, sa);
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
             int16_t نشانهها[], uint16_t token_شمار,
             uint8_t map[256],
             UError_کد *p_Error_کد) {
    UBool استفاده_شده_بیرون_Char[256];
    uint16_t i, j;
    uint8_t c1, c2;

    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    إذا(ds->درون_Charset==ds->بیرون_Charset) {
        /* Same charset family: identity permutation */
        لكل(i=0; i<256; ++i) {
            map[i] = static_cast<uint8_t>(i);
        }
    } وإلا {
        uprv_memset(map, 0, 256);
        uprv_memset(استفاده_شده_بیرون_Char, 0, 256);

        إذا(token_شمار>256) {
            token_شمار=256;
        }

        /* set the direct bytes (byte 0 always maps to itself) */
        لكل(i=1; i<token_شمار; ++i) {
            إذا(نشانهها[i]==-1) {
                /* convert the direct byte character */
                c1 = static_cast<uint8_t>(i);
                ds->جابجا_Inv_Chars(ds, &c1, 1, &c2, p_Error_کد);
                إذا(U_FAILURE(*p_Error_کد)) {
                    udata_printError(ds, "unames/makeTokenMap() finds variant character 0x%02x used (input charset family %d)\n",
                                     i, ds->درون_Charset);
                    أرجع;
                }

                /* enter the converted character into the map and mark it used */
                map[c1]=c2;
                استفاده_شده_بیرون_Char[c2]=درست;
            }
        }

        /* set the mappings for the rest of the permutation */
        لكل(i=j=1; i<token_شمار; ++i) {
            /* set mappings that were not set for direct bytes */
            إذا(map[i]==0) {
                /* set an output byte value that was not used as an output byte above */
                طالما(استفاده_شده_بیرون_Char[j]) {
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
uchar_جابجا_نامها(ثابت UDataSwapper *ds,
                ثابت فراغ *درون_داده, int32_t length, فراغ *بیرون_داده,
                UError_کد *p_Error_کد) {
    ثابت UDataInfo *pInfo;
    int32_t سرآیند_اندازه;

    ثابت uint8_t *درون_بایتها;
    uint8_t *بیرون_بایتها;

    uint32_t token_String_جابه_جایی, groups_جابه_جایی, گروه_String_جابه_جایی, alg_نامها_جابه_جایی,
             جابه_جایی, i, count, رشتهها_شمار;

    ثابت AlgorithmicRange *درون_Range;
    AlgorithmicRange *بیرون_Range;

    /* udata_swapDataHeader checks the arguments */
    سرآیند_اندازه=udata_جابجا_داده_سرآیند(ds, درون_داده, length, بیرون_داده, p_Error_کد);
    إذا(p_Error_کد==اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }

    /* check data format and format version */
    pInfo=(ثابت UDataInfo *)((ثابت حرف *)درون_داده+4);
    إذا(!(
        pInfo->داده_Format[0]==0x75 &&   /* dataFormat="unam" */
        pInfo->داده_Format[1]==0x6e &&
        pInfo->داده_Format[2]==0x61 &&
        pInfo->داده_Format[3]==0x6d &&
        pInfo->format_نسخه[0]==1
    )) {
        udata_printError(ds, "uchar_swapNames(): data format %02x.%02x.%02x.%02x (format version %02x) is not recognized as unames.icu\n",
                         pInfo->داده_Format[0], pInfo->داده_Format[1],
                         pInfo->داده_Format[2], pInfo->داده_Format[3],
                         pInfo->format_نسخه[0]);
        *p_Error_کد=U_UNSUPPORTED_ERROR;
        أرجع 0;
    }

    درون_بایتها=(ثابت uint8_t *)درون_داده+سرآیند_اندازه;
    بیرون_بایتها=(بیرون_داده == اشاره_تهی) ? اشاره_تهی : (uint8_t *)بیرون_داده+سرآیند_اندازه;
    إذا(length<0) {
        alg_نامها_جابه_جایی=ds->readUInt32(((ثابت uint32_t *)درون_بایتها)[3]);
    } وإلا {
        length-=سرآیند_اندازه;
        إذا( length<20 ||
            (uint32_t)length<(alg_نامها_جابه_جایی=ds->readUInt32(((ثابت uint32_t *)درون_بایتها)[3]))
        ) {
            udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu\n",
                             length);
            *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
            أرجع 0;
        }
    }

    إذا(length<0) {
        /* preflighting: iterate through algorithmic ranges */
        جابه_جایی=alg_نامها_جابه_جایی;
        count=ds->readUInt32(*((ثابت uint32_t *)(درون_بایتها+جابه_جایی)));
        جابه_جایی+=4;

        لكل(i=0; i<count; ++i) {
            درون_Range=(ثابت AlgorithmicRange *)(درون_بایتها+جابه_جایی);
            جابه_جایی+=ds->readUInt16(درون_Range->size);
        }
    } وإلا {
        /* swap data */
        ثابت uint16_t *p;
        uint16_t *q, *موقت;

        int16_t نشانهها[512];
        uint16_t token_شمار;

        uint8_t map[256], trailMap[256];

        /* copy the data for inaccessible bytes */
        إذا(درون_بایتها!=بیرون_بایتها) {
            uprv_memcpy(بیرون_بایتها, درون_بایتها, length);
        }

        /* the initial 4 offsets first */
        token_String_جابه_جایی=ds->readUInt32(((ثابت uint32_t *)درون_بایتها)[0]);
        groups_جابه_جایی=ds->readUInt32(((ثابت uint32_t *)درون_بایتها)[1]);
        گروه_String_جابه_جایی=ds->readUInt32(((ثابت uint32_t *)درون_بایتها)[2]);
        ds->جابجا_Array32(ds, درون_بایتها, 16, بیرون_بایتها, p_Error_کد);

        /*
         * now the tokens table
         * it needs to be permutated along with the compressed name strings
         */
        p=(ثابت uint16_t *)(درون_بایتها+16);
        q=(uint16_t *)(بیرون_بایتها+16);

        /* read and swap the tokenCount */
        token_شمار=ds->readUInt16(*p);
        ds->جابجا_Array16(ds, p, 2, q, p_Error_کد);
        ++p;
        ++q;

        /* read the first 512 tokens and make the token maps */
        إذا(token_شمار<=512) {
            count=token_شمار;
        } وإلا {
            count=512;
        }
        لكل(i=0; i<count; ++i) {
            نشانهها[i]=udata_readInt16(ds, p[i]);
        }
        لكل(; i<512; ++i) {
            نشانهها[i]=0; /* fill the rest of the tokens array if tokenCount<512 */
        }
        makeTokenMap(ds, نشانهها, token_شمار, map, p_Error_کد);
        makeTokenMap(ds, نشانهها+256, (uint16_t)(token_شمار>256 ? token_شمار-256 : 0), trailMap, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع 0;
        }

        /*
         * swap and permutate the tokens
         * go through a temporary array to support in-place swapping
         */
        موقت=(uint16_t *)uprv_malloc(token_شمار*2);
        إذا(موقت==اشاره_تهی) {
            udata_printError(ds, "out of memory swapping %u unames.icu tokens\n",
                             token_شمار);
            *p_Error_کد=U_حافظه_تخصیص_ERROR;
            أرجع 0;
        }

        /* swap and permutate single-/lead-byte tokens */
        لكل(i=0; i<token_شمار && i<256; ++i) {
            ds->جابجا_Array16(ds, p+i, 2, موقت+map[i], p_Error_کد);
        }

        /* swap and permutate trail-byte tokens */
        لكل(; i<token_شمار; ++i) {
            ds->جابجا_Array16(ds, p+i, 2, موقت+(i&0xffffff00)+trailMap[i&0xff], p_Error_کد);
        }

        /* copy the result into the output and free the temporary array */
        uprv_memcpy(q, موقت, token_شمار*2);
        uprv_free(موقت);

        /*
         * swap the token strings but not a possible padding byte after
         * the terminating NUL of the last string
         */
        udata_جابجا_Inv_String_بلوک(ds, درون_بایتها+token_String_جابه_جایی, (int32_t)(groups_جابه_جایی-token_String_جابه_جایی),
                                    بیرون_بایتها+token_String_جابه_جایی, p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            udata_printError(ds, "uchar_swapNames(token strings) failed\n");
            أرجع 0;
        }

        /* swap the group table */
        count=ds->readUInt16(*((ثابت uint16_t *)(درون_بایتها+groups_جابه_جایی)));
        ds->جابجا_Array16(ds, درون_بایتها+groups_جابه_جایی, (int32_t)((1+count*3)*2),
                           بیرون_بایتها+groups_جابه_جایی, p_Error_کد);

        /*
         * swap the group strings
         * swap the string bytes but not the nibble-encoded string lengths
         */
        إذا(ds->درون_Charset!=ds->بیرون_Charset) {
            uint16_t offsets[سطرها_در_هر_گروه+1], lengths[سطرها_در_هر_گروه+1];

            ثابت uint8_t *درون_رشتهها, *بعدی_درون_رشتهها;
            uint8_t *بیرون_رشتهها;

            uint8_t c;

            درون_رشتهها=درون_بایتها+گروه_String_جابه_جایی;
            بیرون_رشتهها=بیرون_بایتها+گروه_String_جابه_جایی;

            رشتهها_شمار=alg_نامها_جابه_جایی-گروه_String_جابه_جایی;

            /* iterate through string groups until only a few padding bytes are left */
            طالما(رشتهها_شمار>32) {
                بعدی_درون_رشتهها=expand_گروه_Lengths(درون_رشتهها, offsets, lengths);

                /* move past the length bytes */
                رشتهها_شمار-=(uint32_t)(بعدی_درون_رشتهها-درون_رشتهها);
                بیرون_رشتهها+=بعدی_درون_رشتهها-درون_رشتهها;
                درون_رشتهها=بعدی_درون_رشتهها;

                count=offsets[31]+lengths[31]; /* total number of string bytes in this group */
                رشتهها_شمار-=count;

                /* swap the string bytes using map[] and trailMap[] */
                طالما(count>0) {
                    c=*درون_رشتهها++;
                    *بیرون_رشتهها++=map[c];
                    إذا(نشانهها[c]!=-2) {
                        --count;
                    } وإلا {
                        /* token lead byte: swap the trail byte, too */
                        *بیرون_رشتهها++=trailMap[*درون_رشتهها++];
                        count-=2;
                    }
                }
            }
        }

        /* swap the algorithmic ranges */
        جابه_جایی=alg_نامها_جابه_جایی;
        count=ds->readUInt32(*((ثابت uint32_t *)(درون_بایتها+جابه_جایی)));
        ds->جابجا_Array32(ds, درون_بایتها+جابه_جایی, 4, بیرون_بایتها+جابه_جایی, p_Error_کد);
        جابه_جایی+=4;

        لكل(i=0; i<count; ++i) {
            إذا(جابه_جایی>(uint32_t)length) {
                udata_printError(ds, "uchar_swapNames(): too few bytes (%d after header) for unames.icu algorithmic range %u\n",
                                 length, i);
                *p_Error_کد=U_شاخص_OUTOFBOUNDS_ERROR;
                أرجع 0;
            }

            درون_Range=(ثابت AlgorithmicRange *)(درون_بایتها+جابه_جایی);
            بیرون_Range=(AlgorithmicRange *)(بیرون_بایتها+جابه_جایی);
            جابه_جایی+=ds->readUInt16(درون_Range->size);

            ds->جابجا_Array32(ds, درون_Range, 8, بیرون_Range, p_Error_کد);
            ds->جابجا_Array16(ds, &درون_Range->size, 2, &بیرون_Range->size, p_Error_کد);
            بدل(درون_Range->type) {
            حالة 0:
                /* swap prefix string */
                ds->جابجا_Inv_Chars(ds, درون_Range+1, (int32_t)uprv_strlen((ثابت حرف *)(درون_Range+1)),
                                    بیرون_Range+1, p_Error_کد);
                إذا(U_FAILURE(*p_Error_کد)) {
                    udata_printError(ds, "uchar_swapNames(prefix string of algorithmic range %u) failed\n",
                                     i);
                    أرجع 0;
                }
                اكسر;
            حالة 1:
                {
                    /* swap factors and the prefix and factor strings */
                    uint32_t factors_شمار;

                    factors_شمار=درون_Range->variant;
                    p=(ثابت uint16_t *)(درون_Range+1);
                    q=(uint16_t *)(بیرون_Range+1);
                    ds->جابجا_Array16(ds, p, (int32_t)(factors_شمار*2), q, p_Error_کد);

                    /* swap the strings, up to the last terminating NUL */
                    p+=factors_شمار;
                    q+=factors_شمار;
                    رشتهها_شمار=(uint32_t)((درون_بایتها+جابه_جایی)-(ثابت uint8_t *)p);
                    طالما(رشتهها_شمار>0 && ((ثابت uint8_t *)p)[رشتهها_شمار-1]!=0) {
                        --رشتهها_شمار;
                    }
                    ds->جابجا_Inv_Chars(ds, p, (int32_t)رشتهها_شمار, q, p_Error_کد);
                }
                اكسر;
            افتراضي:
                udata_printError(ds, "uchar_swapNames(): unknown type %u of algorithmic range %u\n",
                                 درون_Range->type, i);
                *p_Error_کد=U_UNSUPPORTED_ERROR;
                أرجع 0;
            }
        }
    }

    أرجع سرآیند_اندازه+(int32_t)جابه_جایی;
}

/*
 * Hey, Emacs, please set the following:
 *
 * Local Variables:
 * indent-tabs-mode: nil
 * End:
 *
 */
