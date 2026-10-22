// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (c) 1996-2014, International Business Machines
* Corporation and others. All Rights Reserved.
******************************************************************************
* File unorm.cpp
*
* Created by: Vladimir Weinstein 12052000
*
* Modification history :
*
* Date        Name        Description
* 02/01/01    synwee      Added normalization quickcheck enum and method.
* 02/12/01    synwee      Commented out quickcheck util api has been approved
*                         Added private method for doing FCD checks
* 02/23/01    synwee      Modified quickcheck and checkFCE to run through 
*                         string for codepoints < 0x300 for the normalization 
*                         mode NFC.
* 05/25/01+   Markus Scherer total rewrite, implement all normalization here
*                         instead of just wrappers around normlzr.cpp,
*                         load unorm.dat, support Unicode 3.1 with
*                         supplementary code points, etc.
* 2009-nov..2010-jan  Markus Scherer  total rewrite, new Normalizer2 API & code
*/

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_NORMALIZATION

#ಅನ್ತರ್ಭಾವ "unicode/udata.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/uiter.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm.h"
#ಅನ್ತರ್ಭಾವ "unicode/unorm2.h"
#ಅನ್ತರ್ಭಾವ "normalizer2impl.h"
#ಅನ್ತರ್ಭಾವ "unormimp.h"
#ಅನ್ತರ್ಭಾವ "uprops.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_ಫಲಿತಾಂಶ U_EXPORT2
unorm_ಶೀಘ್ರ_Check(ಸ್ಥಿರಾಂಕ char16_t *src,
                 int32_t src_ಉದ್ದಳತೆ, 
                 UNormalization_ಬಹುಲಕ ಬಹುಲಕ,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ unorm2_ಶೀಘ್ರ_Check((ಸ್ಥಿರಾಂಕ UNormalizer2 *)n2, src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

U_CAPI UNormalization_Check_ಫಲಿತಾಂಶ U_EXPORT2
unorm_ಶೀಘ್ರ_Check_With_Options(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                            UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ));
        ಹಿಂತಿರುಗಿ unorm2_ಶೀಘ್ರ_Check(
            reinterpret_cast<ಸ್ಥಿರಾಂಕ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ unorm2_ಶೀಘ್ರ_Check((ಸ್ಥಿರಾಂಕ UNormalizer2 *)n2, src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                   UNormalization_ಬಹುಲಕ ಬಹುಲಕ,
                   UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಹಿಂತಿರುಗಿ unorm2_isNormalized((ಸ್ಥಿರಾಂಕ UNormalizer2 *)n2, src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                              UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ));
        ಹಿಂತಿರುಗಿ unorm2_isNormalized(
            reinterpret_cast<ಸ್ಥಿರಾಂಕ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ unorm2_isNormalized((ಸ್ಥಿರಾಂಕ UNormalizer2 *)n2, src, src_ಉದ್ದಳತೆ, p_Error_ಸಂಕೇತ);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಉದ್ದಳತೆ,
                UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ));
        ಹಿಂತಿರುಗಿ unorm2_normalize(
            reinterpret_cast<ಸ್ಥಿರಾಂಕ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ಉದ್ದಳತೆ, dest, dest_ಸಾಮರ್ಥ್ಯ, p_Error_ಸಂಕೇತ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ unorm2_normalize((ಸ್ಥಿರಾಂಕ UNormalizer2 *)n2,
            src, src_ಉದ್ದಳತೆ, dest, dest_ಸಾಮರ್ಥ್ಯ, p_Error_ಸಂಕೇತ);
    }
}


/* iteration functions ------------------------------------------------------ */

ಸ್ಥಿರ int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              ಸ್ಥಿರಾಂಕ Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_ಗಮ್ಯ_Normalize,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) || src==ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(p_Needed_ಗಮ್ಯ_Normalize!=ಶೂನ್ಯಸೂಚಕ) {
        *p_Needed_ಗಮ್ಯ_Normalize=ಸುಳ್ಳು;
    }
    ಆದರೆ(!(forward ? src->has_ಮುಂದಿನ(src) : src->has_ಹಿಂದಿನ(src))) {
        ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, 0, p_Error_ಸಂಕೇತ);
    }

    UnicodeString ಬಫರ್;
    UChar32 c;
    ಆದರೆ(forward) {
        /* get one character and ignore its properties */
        ಬಫರ್.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ಆಗ((c=uiter_next32(src))>=0) {
            ಆದರೆ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_ಉದ್ದಳತೆ(c), UITER_CURRENT);
                ನಿಲ್ಲಿಸು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಬಫರ್.append(c);
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಗ((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            ಬಫರ್.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ಆದರೆ(n2->hasBoundaryBefore(c)) {
                ನಿಲ್ಲಿಸು;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_ಸಾಮರ್ಥ್ಯ);
    ಆದರೆ(ಬಫರ್.length()>0 && doNormalize) {
        n2->normalize(ಬಫರ್, destString, *p_Error_ಸಂಕೇತ).extract(dest, dest_ಸಾಮರ್ಥ್ಯ, *p_Error_ಸಂಕೇತ);
        ಆದರೆ(p_Needed_ಗಮ್ಯ_Normalize!=ಶೂನ್ಯಸೂಚಕ && U_SUCCESS(*p_Error_ಸಂಕೇತ)) {
            *p_Needed_ಗಮ್ಯ_Normalize= destString!=ಬಫರ್;
        }
        ಹಿಂತಿರುಗಿ destString.length();
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* just copy the source characters */
        ಹಿಂತಿರುಗಿ ಬಫರ್.extract(dest, dest_ಸಾಮರ್ಥ್ಯ, *p_Error_ಸಂಕೇತ);
    }
}

ಸ್ಥಿರ int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
              UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
              UBool doNormalize, UBool *p_Needed_ಗಮ್ಯ_Normalize,
              UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(options&UNORM_UNICODE_3_2) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *uni32 = uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ಹಿಂತಿರುಗಿ _iterate(src, forward, dest, dest_ಸಾಮರ್ಥ್ಯ,
            &fn2, doNormalize, p_Needed_ಗಮ್ಯ_Normalize, p_Error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ _iterate(src, forward, dest, dest_ಸಾಮರ್ಥ್ಯ,
            n2, doNormalize, p_Needed_ಗಮ್ಯ_Normalize, p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
unorm_ಹಿಂದಿನ(UCharIterator *src,
               char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
               UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
               UBool doNormalize, UBool *p_Needed_ಗಮ್ಯ_Normalize,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ unorm_iterate(src, ಸುಳ್ಳು,
                         dest, dest_ಸಾಮರ್ಥ್ಯ,
                         ಬಹುಲಕ, options,
                         doNormalize, p_Needed_ಗಮ್ಯ_Normalize,
                         p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
unorm_ಮುಂದಿನ(UCharIterator *src,
           char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
           UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
           UBool doNormalize, UBool *p_Needed_ಗಮ್ಯ_Normalize,
           UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಹಿಂತಿರುಗಿ unorm_iterate(src, ನಿಜ,
                         dest, dest_ಸಾಮರ್ಥ್ಯ,
                         ಬಹುಲಕ, options,
                         doNormalize, p_Needed_ಗಮ್ಯ_Normalize,
                         p_Error_ಸಂಕೇತ);
}

/* Concatenation of normalized strings -------------------------------------- */

ಸ್ಥಿರ int32_t
_concatenate(ಸ್ಥಿರಾಂಕ char16_t *ಎಡ, int32_t ಎಡ_ಉದ್ದಳತೆ,
                  ಸ್ಥಿರಾಂಕ char16_t *ಬಲ, int32_t ಬಲ_ಉದ್ದಳತೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  ಸ್ಥಿರಾಂಕ Normalizer2 *n2,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ(dest_ಸಾಮರ್ಥ್ಯ<0 || (dest==ಶೂನ್ಯಸೂಚಕ && dest_ಸಾಮರ್ಥ್ಯ>0) ||
        ಎಡ==ಶೂನ್ಯಸೂಚಕ || ಎಡ_ಉದ್ದಳತೆ<-1 || ಬಲ==ಶೂನ್ಯಸೂಚಕ || ಬಲ_ಉದ್ದಳತೆ<-1) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* check for overlapping right and destination */
    ಆದರೆ( dest!=ಶೂನ್ಯಸೂಚಕ &&
        ((ಬಲ>=dest && ಬಲ<(dest+dest_ಸಾಮರ್ಥ್ಯ)) ||
         (ಬಲ_ಉದ್ದಳತೆ>0 && dest>=ಬಲ && dest<(ಬಲ+ಬಲ_ಉದ್ದಳತೆ)))
    ) {
        *p_Error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ಆದರೆ(ಎಡ==dest) {
        destString.ಗಣ_ಗಮ್ಯ(dest, ಎಡ_ಉದ್ದಳತೆ, dest_ಸಾಮರ್ಥ್ಯ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        destString.ಗಣ_ಗಮ್ಯ(dest, 0, dest_ಸಾಮರ್ಥ್ಯ);
        destString.append(ಎಡ, ಎಡ_ಉದ್ದಳತೆ);
    }
    ಹಿಂತಿರುಗಿ n2->append(destString, UnicodeString(ಬಲ_ಉದ್ದಳತೆ<0, ಬಲ, ಬಲ_ಉದ್ದಳತೆ), *p_Error_ಸಂಕೇತ).
           extract(dest, dest_ಸಾಮರ್ಥ್ಯ, *p_Error_ಸಂಕೇತ);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ಸ್ಥಿರಾಂಕ char16_t *ಎಡ, int32_t ಎಡ_ಉದ್ದಳತೆ,
                  ಸ್ಥಿರಾಂಕ char16_t *ಬಲ, int32_t ಬಲ_ಉದ್ದಳತೆ,
                  char16_t *dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                  UNormalization_ಬಹುಲಕ ಬಹುಲಕ, int32_t options,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ Normalizer2 *n2=Normalizer2Factory::getInstance(ಬಹುಲಕ, *p_Error_ಸಂಕೇತ);
    ಆದರೆ(options&UNORM_UNICODE_3_2) {
        ಸ್ಥಿರಾಂಕ Unicode_ಗಣ *uni32 = uniset_getUnicode32Instance(*p_Error_ಸಂಕೇತ);
        ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ)) {
            ಹಿಂತಿರುಗಿ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ಹಿಂತಿರುಗಿ _concatenate(ಎಡ, ಎಡ_ಉದ್ದಳತೆ, ಬಲ, ಬಲ_ಉದ್ದಳತೆ,
            dest, dest_ಸಾಮರ್ಥ್ಯ, &fn2, p_Error_ಸಂಕೇತ);
    }
    ಹಿಂತಿರುಗಿ _concatenate(ಎಡ, ಎಡ_ಉದ್ದಳತೆ, ಬಲ, ಬಲ_ಉದ್ದಳತೆ,
        dest, dest_ಸಾಮರ್ಥ್ಯ, n2, p_Error_ಸಂಕೇತ);
}

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_NORMALIZATION */
