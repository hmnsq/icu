// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2001-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  unormcmp.cpp
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2004sep13
*   created by: Markus W. Scherer
*
*   unorm_compare() function moved here from unorm.cpp for better modularization.
*   Depends on both normalization and case folding.
*   Allows unorm.cpp to not depend on any character properties code.
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/unorm.h"
#شامل "unicode/ustring.h"
#شامل "cmemory.h"
#شامل "normalizer2impl.h"
#شامل "ucase.h"
#شامل "uprops.h"
#شامل "ustr_imp.h"

U_NAMESPACE_USE

/* compare canonically equivalent ------------------------------------------- */

/*
 * Compare two strings for canonical equivalence.
 * Further options include case-insensitive comparison and
 * code point order (as opposed to code unit order).
 *
 * In this function, canonical equivalence is optional as well.
 * If canonical equivalence is tested, then both strings must fulfill
 * the FCD check.
 *
 * Semantically, this is equivalent to
 *   strcmp[CodePointOrder](NFD(foldCase(s1)), NFD(foldCase(s2)))
 * where code point order, NFD and foldCase are all optional.
 *
 * String comparisons almost always yield results before processing both strings
 * completely.
 * They are generally more efficient working incrementally instead of
 * performing the sub-processing (strlen, normalization, case-folding)
 * on the entire strings first.
 *
 * It is also unnecessary to not normalize identical characters.
 *
 * This function works in principle as follows:
 *
 * loop {
 *   get one code unit c1 from s1 (-1 if end of source)
 *   get one code unit c2 from s2 (-1 if end of source)
 *
 *   if(either string finished) {
 *     return result;
 *   }
 *   if(c1==c2) {
 *     continue;
 *   }
 *
 *   // c1!=c2
 *   try to decompose/case-fold c1/c2, and continue if one does;
 *
 *   // still c1!=c2 and neither decomposes/case-folds, return result
 *   return c1-c2;
 * }
 *
 * When a character decomposes, then the pointer for that source changes to
 * the decomposition, pushing the previous pointer onto a stack.
 * When the end of the decomposition is reached, then the code unit reader
 * pops the previous source from the stack.
 * (Same for case-folding.)
 *
 * This is complicated further by operating on variable-width UTF-16.
 * The top part of the loop works on code units, while lookups for decomposition
 * and case-folding need code points.
 * Code points are assembled after the equality/end-of-source part.
 * The source pointer is only advanced beyond all code units when the code point
 * actually decomposes/case-folds.
 *
 * If we were on a trail surrogate unit when assembling a code point,
 * and the code point decomposes/case-folds, then the decomposition/folding
 * result must be compared with the part of the other string that corresponds to
 * this string's lead surrogate.
 * Since we only assemble a code point when hitting a trail unit when the
 * preceding lead units were identical, we back up the other string by one unit
 * in such a case.
 *
 * The optional code point order comparison at the end works with
 * the same fix-up as the other code point order comparison functions.
 * See ustring.c and the comment near the end of this function.
 *
 * Assumption: A decomposition or case-folding result string never contains
 * a single surrogate. This is a safe assumption in the Unicode Standard.
 * Therefore, we do not need to check for surrogate pairs across
 * decomposition/case-folding boundaries.
 *
 * Further assumptions (see verifications tstnorm.cpp):
 * The API function checks for FCD first, while the core function
 * first case-folds and then decomposes. This requires that case-folding does not
 * un-FCD any strings.
 *
 * The API function may also NFD the input and turn off decomposition.
 * This requires that case-folding does not un-NFD strings either.
 *
 * TODO If any of the above two assumptions is violated,
 * then this entire code must be re-thought.
 * If this happens, then a simple solution is to case-fold both strings up front
 * and to turn off UNORM_INPUT_IS_FCD.
 * We already do this when not both strings are in FCD because makeFCD
 * would be a partial NFD before the case folding, which does not work.
 * Note that all of this is only a problem when case-folding _and_
 * canonical equivalence come together.
 * (Comments in unorm_compare() are more up to date than this TODO.)
 */

/* stack element for previous-level source/decomposition pointers */
ساخت CmpEquivLevel {
    ثابت char16_t *آغاز, *s, *حد_بندی;
};
قسمتعریف ساخت CmpEquivLevel CmpEquivLevel;

/**
 * Internal option for unorm_cmpEquivFold() for decomposing.
 * If not set, just do strcasecmp().
 */
#تعریف_کرو _COMPARE_EQUIV 0x80000

/* internal function */
ساکن int32_t
unorm_cmpEquivFold(ثابت char16_t *s1, int32_t length1,
                   ثابت char16_t *s2, int32_t length2,
                   uint32_t options,
                   UError_رمز *p_Error_رمز) {
    ثابت Normalizer2Impl *nfcImpl;

    /* current-level start/limit - s1/s2 as current */
    ثابت char16_t *start1, *start2, *limit1, *limit2;

    /* decomposition and case folding variables */
    ثابت char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* buffers for algorithmic decompositions */
    char16_t decomp1[4], decomp2[4];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_لمبائی+1], fold2[UCASE_MAX_STRING_لمبائی+1];

    /* track which is the current level per string */
    int32_t level1, level2;

    /* current code units, and code points for lookups */
    UChar32 c1, c2, cp1, cp2;

    /* no argument error checking because this itself is not an API */

    /*
     * assume that at least one of the options _COMPARE_EQUIV and U_COMPARE_IGNORE_CASE is set
     * otherwise this function must behave exactly as uprv_strCompare()
     * not checking for that here makes testing this function easier
     */

    /* normalization/properties data loaded? */
    اگر((options&_COMPARE_EQUIV)!=0) {
        nfcImpl=Normalizer2Factory::getNFCImpl(*p_Error_رمز);
    } ورنہ {
        nfcImpl=خالی_اشاریہ;
    }
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }

    /* initialize */
    start1=s1;
    اگر(length1==-1) {
        limit1=خالی_اشاریہ;
    } ورنہ {
        limit1=s1+length1;
    }

    start2=s2;
    اگر(length2==-1) {
        limit2=خالی_اشاریہ;
    } ورنہ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    برائے(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        اگر(c1<0) {
            /* get next code unit from string 1, post-increment */
            برائے(;;) {
                اگر(s1==limit1 || ((c1=*s1)==0 && (limit1==خالی_اشاریہ || (options&_STRNCMP_STYLE)))) {
                    اگر(level1==0) {
                        c1=-1;
                        توڑو;
                    }
                } ورنہ {
                    ++s1;
                    توڑو;
                }

                /* reached end of level buffer, pop one level */
                کرو {
                    --level1;
                    start1=stack1[level1].آغاز;    /*Not uninitialized*/
                } جب(start1==خالی_اشاریہ);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].حد_بندی;        /*Not uninitialized*/
            }
        }

        اگر(c2<0) {
            /* get next code unit from string 2, post-increment */
            برائے(;;) {
                اگر(s2==limit2 || ((c2=*s2)==0 && (limit2==خالی_اشاریہ || (options&_STRNCMP_STYLE)))) {
                    اگر(level2==0) {
                        c2=-1;
                        توڑو;
                    }
                } ورنہ {
                    ++s2;
                    توڑو;
                }

                /* reached end of level buffer, pop one level */
                کرو {
                    --level2;
                    start2=stack2[level2].آغاز;    /*Not uninitialized*/
                } جب(start2==خالی_اشاریہ);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].حد_بندی;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        اگر(c1==c2) {
            اگر(c1<0) {
                واپس 0;   /* c1==c2==-1 indicating end of strings */
            }
            c1=c2=-1;       /* make us fetch new code units */
            جاری;
        } ورنہ اگر(c1<0) {
            واپس -1;      /* string 1 ends before string 2 */
        } ورنہ اگر(c2<0) {
            واپس 1;       /* string 2 ends before string 1 */
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        اگر(U_IS_SURROGATE(c1)) {
            char16_t c;

            اگر(U_IS_SURROGATE_LEAD(c1)) {
                اگر(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ورنہ /* isTrail(c1) */ {
                اگر(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        اگر(U_IS_SURROGATE(c2)) {
            char16_t c;

            اگر(U_IS_SURROGATE_LEAD(c2)) {
                اگر(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ورنہ /* isTrail(c2) */ {
                اگر(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        اگر( level1==0 && (options&U_COMPARE_IGNORE_CASE) &&
            (length = ucase_مقصد_بھرا_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            اگر(U_IS_SURROGATE(c1)) {
                اگر(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ورنہ /* isTrail(c1) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s2;
                    c2=*(s2-1);
                }
            }

            /* push current level pointers */
            stack1[0].آغاز=start1;
            stack1[0].s=s1;
            stack1[0].حد_بندی=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            اگر(length<=UCASE_MAX_STRING_لمبائی) {
                u_memcpy(fold1, p, length);
            } ورنہ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            جاری;
        }

        اگر( level2==0 && (options&U_COMPARE_IGNORE_CASE) &&
            (length = ucase_مقصد_بھرا_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            اگر(U_IS_SURROGATE(c2)) {
                اگر(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ورنہ /* isTrail(c2) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s1;
                    c1=*(s1-1);
                }
            }

            /* push current level pointers */
            stack2[0].آغاز=start2;
            stack2[0].s=s2;
            stack2[0].حد_بندی=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            اگر(length<=UCASE_MAX_STRING_لمبائی) {
                u_memcpy(fold2, p, length);
            } ورنہ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            جاری;
        }

        اگر( level1<2 && (options&_COMPARE_EQUIV) &&
            خالی_اشاریہ != (p = nfcImpl->getDecomposition(cp1, decomp1, length))
        ) {
            /* cp1 decomposes into p[length] */
            اگر(U_IS_SURROGATE(c1)) {
                اگر(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it decomposes */
                    ++s1;
                } ورنہ /* isTrail(c1) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s2;
                    c2=*(s2-1);
                }
            }

            /* push current level pointers */
            stack1[level1].آغاز=start1;
            stack1[level1].s=s1;
            stack1[level1].حد_بندی=limit1;
            ++level1;

            /* set empty intermediate level if skipped */
            اگر(level1<2) {
                stack1[level1++].آغاز=خالی_اشاریہ;
            }

            /* set next level pointers to decomposition */
            start1=s1=p;
            limit1=p+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            جاری;
        }

        اگر( level2<2 && (options&_COMPARE_EQUIV) &&
            خالی_اشاریہ != (p = nfcImpl->getDecomposition(cp2, decomp2, length))
        ) {
            /* cp2 decomposes into p[length] */
            اگر(U_IS_SURROGATE(c2)) {
                اگر(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it decomposes */
                    ++s2;
                } ورنہ /* isTrail(c2) */ {
                    /*
                     * we got a supplementary code point when hitting its trail surrogate,
                     * therefore the lead surrogate must have been the same as in the other string;
                     * compare this decomposition with the lead surrogate in the other string
                     * remember that this simulates bulk text replacement:
                     * the decomposition would replace the entire code point
                     */
                    --s1;
                    c1=*(s1-1);
                }
            }

            /* push current level pointers */
            stack2[level2].آغاز=start2;
            stack2[level2].s=s2;
            stack2[level2].حد_بندی=limit2;
            ++level2;

            /* set empty intermediate level if skipped */
            اگر(level2<2) {
                stack2[level2++].آغاز=خالی_اشاریہ;
            }

            /* set next level pointers to decomposition */
            start2=s2=p;
            limit2=p+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            جاری;
        }

        /*
         * no decomposition/case folding, max level for both sides:
         * return difference result
         *
         * code point order comparison must not just return cp1-cp2
         * because when single surrogates are present then the surrogate pairs
         * that formed cp1 and cp2 may be from different string indexes
         *
         * example: { d800 d800 dc01 } vs. { d800 dc00 }, compare at second code units
         * c1=d800 cp1=10001 c2=dc00 cp2=10000
         * cp1-cp2>0 but c1-c2<0 and in fact in UTF-32 it is { d800 10001 } < { 10000 }
         *
         * therefore, use same fix-up as in ustring.c/uprv_strCompare()
         * except: uprv_strCompare() fetches c=*s while this functions fetches c=*s++
         * so we have slightly different pointer/start/limit comparisons here
         */

        اگر(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_رمز_نقطہ_ترتیب)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            اگر(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ورنہ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            اگر(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ورنہ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        واپس c1-c2;
    }
}

ساکن
UBool _normalize(ثابت Normalizer2 *n2, ثابت char16_t *s, int32_t length,
                UnicodeString &normalized, UError_رمز *p_Error_رمز) {
    UnicodeString str(length<0, s, length);

    // check if s fulfill the conditions
    int32_t وسعت_QCYes=n2->وسعت_تیز_Check_Yes(str, *p_Error_رمز);
    اگر (U_FAILURE(*p_Error_رمز)) {
        واپس جھوٹ;
    }
    /*
     * ICU 2.4 had a further optimization:
     * If both strings were not in FCD, then they were both NFD'ed,
     * and the _COMPARE_EQUIV option was turned off.
     * It is not entirely clear that this is valid with the current
     * definition of the canonical caseless match.
     * Therefore, ICU 2.6 removes that optimization.
     */
    اگر(وسعت_QCYes<str.length()) {
        UnicodeString unnormalized=str.عارضی_Sub_String(وسعت_QCYes);
        normalized.سیٹ_مقصد(جھوٹ, str.get_بفر(), وسعت_QCYes);
        n2->normalize_دوسرا_And_Append(normalized, unnormalized, *p_Error_رمز);
        اگر (U_SUCCESS(*p_Error_رمز)) {
            واپس سچ;
        }
    }
    واپس جھوٹ;
}

U_CAPI int32_t U_EXPORT2
unorm_compare(ثابت char16_t *s1, int32_t length1,
              ثابت char16_t *s2, int32_t length2,
              uint32_t options,
              UError_رمز *p_Error_رمز) {
    /* argument checking */
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر (s1 == خالی_اشاریہ || length1 < -1 || s2 == خالی_اشاریہ || length2 < -1) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    UnicodeString fcd1, fcd2;
    int32_t normOptions=(int32_t)(options>>UNORM_COMPARE_NORM_OPTIONS_سرکاؤ);
    options|=_COMPARE_EQUIV;

    /*
     * UAX #21 Case Mappings, as fixed for Unicode version 4
     * (see Jitterbug 2021), defines a canonical caseless match as
     *
     * A string X is a canonical caseless match
     * for a string Y if and only if
     * NFD(toCasefold(NFD(X))) = NFD(toCasefold(NFD(Y)))
     *
     * For better performance, we check for FCD (or let the caller tell us that
     * both strings are in FCD) for the inner normalization.
     * BasicNormalizerTest::FindFoldFCDExceptions() makes sure that
     * case-folding preserves the FCD-ness of a string.
     * The outer normalization is then only performed by unorm_cmpEquivFold()
     * when there is a difference.
     *
     * Exception: When using the Turkic case-folding option, we do perform
     * full NFD first. This is because in the Turkic case precomposed characters
     * with 0049 capital I or 0069 small i fold differently whether they
     * are first decomposed or not, so an FCD check - a check only for
     * canonical order - is not sufficient.
     */
    اگر(!(options&UNORM_داخلہ_IS_FCD) || (options&U_FOLD_CASE_EXCLUDE_SPECIAL_I)) {
        ثابت Normalizer2 *n2;
        اگر(options&U_FOLD_CASE_EXCLUDE_SPECIAL_I) {
            n2=Normalizer2::getNFDInstance(*p_Error_رمز);
        } ورنہ {
            n2=Normalizer2Factory::getFCDInstance(*p_Error_رمز);
        }
        اگر (U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }

        اگر(normOptions&UNORM_UNICODE_3_2) {
            ثابت Unicode_سیٹ *uni32=uniset_getUnicode32Instance(*p_Error_رمز);
            FilteredNormalizer2 fn2(*n2, *uni32);
            اگر(_normalize(&fn2, s1, length1, fcd1, p_Error_رمز)) {
                s1=fcd1.get_بفر();
                length1=fcd1.length();
            }
            اگر(_normalize(&fn2, s2, length2, fcd2, p_Error_رمز)) {
                s2=fcd2.get_بفر();
                length2=fcd2.length();
            }
        } ورنہ {
            اگر(_normalize(n2, s1, length1, fcd1, p_Error_رمز)) {
                s1=fcd1.get_بفر();
                length1=fcd1.length();
            }
            اگر(_normalize(n2, s2, length2, fcd2, p_Error_رمز)) {
                s2=fcd2.get_بفر();
                length2=fcd2.length();
            }
        }
    }

    اگر(U_SUCCESS(*p_Error_رمز)) {
        واپس unorm_cmpEquivFold(s1, length1, s2, length2, options, p_Error_رمز);
    } ورنہ {
        واپس 0;
    }
}

#اگر_ختم /* #if !UCONFIG_NO_NORMALIZATION */
