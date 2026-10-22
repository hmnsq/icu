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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/unorm.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "ucase.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

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
ರಚನೆ CmpEquivLevel {
    ಸ್ಥಿರಾಂಕ char16_t *ಆರಂಭ, *s, *ಎಲ್ಲೆ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CmpEquivLevel CmpEquivLevel;

/**
 * Internal option for unorm_cmpEquivFold() for decomposing.
 * If not set, just do strcasecmp().
 */
#ವ್ಯಾಖ್ಯಾನಿಸು _COMPARE_EQUIV 0x80000

/* internal function */
ಸ್ಥಿರ int32_t
unorm_cmpEquivFold(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
                   ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
                   uint32_t options,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2Impl *nfcImpl;

    /* current-level start/limit - s1/s2 as current */
    ಸ್ಥಿರಾಂಕ char16_t *start1, *start2, *limit1, *limit2;

    /* decomposition and case folding variables */
    ಸ್ಥಿರಾಂಕ char16_t *p;
    int32_t length;

    /* stacks of previous-level start/current/limit */
    CmpEquivLevel stack1[2], stack2[2];

    /* buffers for algorithmic decompositions */
    char16_t decomp1[4], decomp2[4];

    /* case folding buffers, only use current-level start/limit */
    char16_t fold1[UCASE_MAX_STRING_ಉದ್ದಳತೆ+1], fold2[UCASE_MAX_STRING_ಉದ್ದಳತೆ+1];

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
    ಆದರೆ((options&_COMPARE_EQUIV)!=0) {
        nfcImpl=Normalizer2Factory::getNFCImpl(*p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        nfcImpl=ಶೂನ್ಯಸೂಚಕ;
    }
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }

    /* initialize */
    start1=s1;
    ಆದರೆ(length1==-1) {
        limit1=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        limit1=s1+length1;
    }

    start2=s2;
    ಆದರೆ(length2==-1) {
        limit2=ಶೂನ್ಯಸೂಚಕ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        limit2=s2+length2;
    }

    level1=level2=0;
    c1=c2=-1;

    /* comparison loop */
    ಗಾಗಿ(;;) {
        /*
         * here a code unit value of -1 means "get another code unit"
         * below it will mean "this source is finished"
         */

        ಆದರೆ(c1<0) {
            /* get next code unit from string 1, post-increment */
            ಗಾಗಿ(;;) {
                ಆದರೆ(s1==limit1 || ((c1=*s1)==0 && (limit1==ಶೂನ್ಯಸೂಚಕ || (options&_STRNCMP_STYLE)))) {
                    ಆದರೆ(level1==0) {
                        c1=-1;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++s1;
                    ನಿಲ್ಲಿಸು;
                }

                /* reached end of level buffer, pop one level */
                ಮಾಡು {
                    --level1;
                    start1=stack1[level1].ಆರಂಭ;    /*Not uninitialized*/
                } ಆಗ(start1==ಶೂನ್ಯಸೂಚಕ);
                s1=stack1[level1].s;                /*Not uninitialized*/
                limit1=stack1[level1].ಎಲ್ಲೆ;        /*Not uninitialized*/
            }
        }

        ಆದರೆ(c2<0) {
            /* get next code unit from string 2, post-increment */
            ಗಾಗಿ(;;) {
                ಆದರೆ(s2==limit2 || ((c2=*s2)==0 && (limit2==ಶೂನ್ಯಸೂಚಕ || (options&_STRNCMP_STYLE)))) {
                    ಆದರೆ(level2==0) {
                        c2=-1;
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ++s2;
                    ನಿಲ್ಲಿಸು;
                }

                /* reached end of level buffer, pop one level */
                ಮಾಡು {
                    --level2;
                    start2=stack2[level2].ಆರಂಭ;    /*Not uninitialized*/
                } ಆಗ(start2==ಶೂನ್ಯಸೂಚಕ);
                s2=stack2[level2].s;                /*Not uninitialized*/
                limit2=stack2[level2].ಎಲ್ಲೆ;        /*Not uninitialized*/
            }
        }

        /*
         * compare c1 and c2
         * either variable c1, c2 is -1 only if the corresponding string is finished
         */
        ಆದರೆ(c1==c2) {
            ಆದರೆ(c1<0) {
                ಹಿಂತಿರುಗಿ 0;   /* c1==c2==-1 indicating end of strings */
            }
            c1=c2=-1;       /* make us fetch new code units */
            ಮುಂದುವರಿ;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c1<0) {
            ಹಿಂತಿರುಗಿ -1;      /* string 1 ends before string 2 */
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c2<0) {
            ಹಿಂತಿರುಗಿ 1;       /* string 2 ends before string 1 */
        }
        /* c1!=c2 && c1>=0 && c2>=0 */

        /* get complete code points for c1, c2 for lookups if either is a surrogate */
        cp1=c1;
        ಆದರೆ(U_IS_SURROGATE(c1)) {
            char16_t c;

            ಆದರೆ(U_IS_SURROGATE_LEAD(c1)) {
                ಆದರೆ(s1!=limit1 && U16_IS_TRAIL(c=*s1)) {
                    /* advance ++s1; only below if cp1 decomposes/case-folds */
                    cp1=U16_GET_SUPPLEMENTARY(c1, c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c1) */ {
                ಆದರೆ(start1<=(s1-2) && U16_IS_LEAD(c=*(s1-2))) {
                    cp1=U16_GET_SUPPLEMENTARY(c, c1);
                }
            }
        }

        cp2=c2;
        ಆದರೆ(U_IS_SURROGATE(c2)) {
            char16_t c;

            ಆದರೆ(U_IS_SURROGATE_LEAD(c2)) {
                ಆದರೆ(s2!=limit2 && U16_IS_TRAIL(c=*s2)) {
                    /* advance ++s2; only below if cp2 decomposes/case-folds */
                    cp2=U16_GET_SUPPLEMENTARY(c2, c);
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c2) */ {
                ಆದರೆ(start2<=(s2-2) && U16_IS_LEAD(c=*(s2-2))) {
                    cp2=U16_GET_SUPPLEMENTARY(c, c2);
                }
            }
        }

        /*
         * go down one level for each string
         * continue with the main loop as soon as there is a real change
         */

        ಆದರೆ( level1==0 && (options&U_COMPARE_IGNORE_CASE) &&
            (length = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(cp1, &p, options)) >= 0
        ) {
            /* cp1 case-folds to the code point "length" or to p[length] */
            ಆದರೆ(U_IS_SURROGATE(c1)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s1;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c1) */ {
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
            stack1[0].ಆರಂಭ=start1;
            stack1[0].s=s1;
            stack1[0].ಎಲ್ಲೆ=limit1;
            ++level1;

            /* copy the folding result to fold1[] */
            ಆದರೆ(length<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
                u_memcpy(fold1, p, length);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold1, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start1=s1=fold1;
            limit1=fold1+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            ಮುಂದುವರಿ;
        }

        ಆದರೆ( level2==0 && (options&U_COMPARE_IGNORE_CASE) &&
            (length = ucase_ಗಮ್ಯ_ತುಂಬಿದ_Folding(cp2, &p, options)) >= 0
        ) {
            /* cp2 case-folds to the code point "length" or to p[length] */
            ಆದರೆ(U_IS_SURROGATE(c2)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it case-folds */
                    ++s2;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c2) */ {
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
            stack2[0].ಆರಂಭ=start2;
            stack2[0].s=s2;
            stack2[0].ಎಲ್ಲೆ=limit2;
            ++level2;

            /* copy the folding result to fold2[] */
            ಆದರೆ(length<=UCASE_MAX_STRING_ಉದ್ದಳತೆ) {
                u_memcpy(fold2, p, length);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t i=0;
                U16_APPEND_UNSAFE(fold2, i, length);
                length=i;
            }

            /* set next level pointers to case folding */
            start2=s2=fold2;
            limit2=fold2+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            ಮುಂದುವರಿ;
        }

        ಆದರೆ( level1<2 && (options&_COMPARE_EQUIV) &&
            ಶೂನ್ಯಸೂಚಕ != (p = nfcImpl->getDecomposition(cp1, decomp1, length))
        ) {
            /* cp1 decomposes into p[length] */
            ಆದರೆ(U_IS_SURROGATE(c1)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c1)) {
                    /* advance beyond source surrogate pair if it decomposes */
                    ++s1;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c1) */ {
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
            stack1[level1].ಆರಂಭ=start1;
            stack1[level1].s=s1;
            stack1[level1].ಎಲ್ಲೆ=limit1;
            ++level1;

            /* set empty intermediate level if skipped */
            ಆದರೆ(level1<2) {
                stack1[level1++].ಆರಂಭ=ಶೂನ್ಯಸೂಚಕ;
            }

            /* set next level pointers to decomposition */
            start1=s1=p;
            limit1=p+length;

            /* get ready to read from decomposition, continue with loop */
            c1=-1;
            ಮುಂದುವರಿ;
        }

        ಆದರೆ( level2<2 && (options&_COMPARE_EQUIV) &&
            ಶೂನ್ಯಸೂಚಕ != (p = nfcImpl->getDecomposition(cp2, decomp2, length))
        ) {
            /* cp2 decomposes into p[length] */
            ಆದರೆ(U_IS_SURROGATE(c2)) {
                ಆದರೆ(U_IS_SURROGATE_LEAD(c2)) {
                    /* advance beyond source surrogate pair if it decomposes */
                    ++s2;
                } ಇಲ್ಲದಿದ್ದರೆ /* isTrail(c2) */ {
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
            stack2[level2].ಆರಂಭ=start2;
            stack2[level2].s=s2;
            stack2[level2].ಎಲ್ಲೆ=limit2;
            ++level2;

            /* set empty intermediate level if skipped */
            ಆದರೆ(level2<2) {
                stack2[level2++].ಆರಂಭ=ಶೂನ್ಯಸೂಚಕ;
            }

            /* set next level pointers to decomposition */
            start2=s2=p;
            limit2=p+length;

            /* get ready to read from decomposition, continue with loop */
            c2=-1;
            ಮುಂದುವರಿ;
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

        ಆದರೆ(c1>=0xd800 && c2>=0xd800 && (options&U_COMPARE_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ)) {
            /* subtract 0x2800 from BMP code points to make them smaller than supplementary ones */
            ಆದರೆ(
                (c1<=0xdbff && s1!=limit1 && U16_IS_TRAIL(*s1)) ||
                (U16_IS_TRAIL(c1) && start1!=(s1-1) && U16_IS_LEAD(*(s1-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c1-=0x2800;
            }

            ಆದರೆ(
                (c2<=0xdbff && s2!=limit2 && U16_IS_TRAIL(*s2)) ||
                (U16_IS_TRAIL(c2) && start2!=(s2-1) && U16_IS_LEAD(*(s2-2)))
            ) {
                /* part of a surrogate pair, leave >=d800 */
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* BMP code point - may be surrogate code point - make <d800 */
                c2-=0x2800;
            }
        }

        ಹಿಂತಿರುಗಿ c1-c2;
    }
}

ಸ್ಥಿರ
UBool _normalize(ಸ್ಥಿರಾಂಕ Normalizer2 *n2, ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length,
                UnicodeString &normalized, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UnicodeString str(length<0, s, length);

    // check if s fulfill the conditions
    int32_t ಹರವು_QCYes=n2->ಹರವು_ಶೀಘ್ರ_Check_Yes(str, *p_Error_ಸಂಕೇತ);
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    /*
     * ICU 2.4 had a further optimization:
     * If both strings were not in FCD, then they were both NFD'ed,
     * and the _COMPARE_EQUIV option was turned off.
     * It is not entirely clear that this is valid with the current
     * definition of the canonical caseless match.
     * Therefore, ICU 2.6 removes that optimization.
     */
    ಆದರೆ(ಹರವು_QCYes<str.length()) {
        UnicodeString unnormalized=str.ತಾತ್ಕಾಲಿಕ_Sub_String(ಹರವು_QCYes);
        normalized.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, str.get_ಬಫರ್(), ಹರವು_QCYes);
        n2->normalize_ಎರಡನೇ_And_Append(normalized, unnormalized, *p_Error_ಸಂಕೇತ);
        ಆದರೆ (U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ ನಿಜ;
        }
    }
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

U_CAPI int32_t U_EXPORT2
unorm_compare(ಸ್ಥಿರಾಂಕ char16_t *s1, int32_t length1,
              ಸ್ಥಿರಾಂಕ char16_t *s2, int32_t length2,
              uint32_t options,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* argument checking */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (s1 == ಶೂನ್ಯಸೂಚಕ || length1 < -1 || s2 == ಶೂನ್ಯಸೂಚಕ || length2 < -1) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    UnicodeString fcd1, fcd2;
    int32_t normOptions=(int32_t)(options>>UNORM_COMPARE_NORM_OPTIONS_ಸರಿಸು);
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
    ಆದರೆ(!(options&UNORM_ಒಳಹರಿವು_IS_FCD) || (options&U_FOLD_CASE_EXCLUDE_SPECIAL_I)) {
        ಸ್ಥಿರಾಂಕ Normalizer2 *n2;
        ಆದರೆ(options&U_FOLD_CASE_EXCLUDE_SPECIAL_I) {
            n2=Normalizer2::getNFDInstance(*p_Error_ಸಂಕೇತ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            n2=Normalizer2Factory::getFCDInstance(*p_Error_ಸಂಕೇತ);
        }
        ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ(normOptions&UNORM_UNICODE_3_2) {
            ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *uni32=uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ);
            FilteredNormalizer2 fn2(*n2, *uni32);
            ಆದರೆ(_normalize(&fn2, s1, length1, fcd1, p_Error_ಸಂಕೇತ)) {
                s1=fcd1.get_ಬಫರ್();
                length1=fcd1.length();
            }
            ಆದರೆ(_normalize(&fn2, s2, length2, fcd2, p_Error_ಸಂಕೇತ)) {
                s2=fcd2.get_ಬಫರ್();
                length2=fcd2.length();
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ(_normalize(n2, s1, length1, fcd1, p_Error_ಸಂಕೇತ)) {
                s1=fcd1.get_ಬಫರ್();
                length1=fcd1.length();
            }
            ಆದರೆ(_normalize(n2, s2, length2, fcd2, p_Error_ಸಂಕೇತ)) {
                s2=fcd2.get_ಬಫರ್();
                length2=fcd2.length();
            }
        }
    }

    ಆದರೆ(U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ unorm_cmpEquivFold(s1, length1, s2, length2, options, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ 0;
    }
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_NORMALIZATION */
