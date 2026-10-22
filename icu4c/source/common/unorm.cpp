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

#ಯದಿ !UCONFIG_NO_NORMALIZATION

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

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(ಧ್ರುವ char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ unorm2_quickCheck((ಧ್ರುವ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(ಧ್ರುವ char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಯದಿ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ಪ್ರತ್ಯಾಗಮ unorm2_quickCheck(
            reinterpret_cast<ಧ್ರುವ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ unorm2_quickCheck((ಧ್ರುವ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ಧ್ರುವ char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಪ್ರತ್ಯಾಗಮ unorm2_isNormalized((ಧ್ರುವ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ಧ್ರುವ char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಯದಿ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ಪ್ರತ್ಯಾಗಮ unorm2_isNormalized(
            reinterpret_cast<ಧ್ರುವ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ unorm2_isNormalized((ಧ್ರುವ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ಧ್ರುವ char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಯದಿ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ಪ್ರತ್ಯಾಗಮ unorm2_normalize(
            reinterpret_cast<ಧ್ರುವ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ unorm2_normalize((ಧ್ರುವ UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

ಸ್ಥಿರ int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              ಧ್ರುವ Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=ಅಸತ್ಯ;
    }
    ಯದಿ(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        ಪ್ರತ್ಯಾಗಮ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    ಯದಿ(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ಯಾವತ್((c=uiter_next32(src))>=0) {
            ಯದಿ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                ಭಙ್ಗ;
            } ಅನ್ಯಥಾ {
                buffer.append(c);
            }
        }
    } ಅನ್ಯಥಾ {
        ಯಾವತ್((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ಯದಿ(n2->hasBoundaryBefore(c)) {
                ಭಙ್ಗ;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    ಯದಿ(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        ಯದಿ(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        ಪ್ರತ್ಯಾಗಮ destString.length();
    } ಅನ್ಯಥಾ {
        /* just copy the source characters */
        ಪ್ರತ್ಯಾಗಮ buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

ಸ್ಥಿರ int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಯದಿ(options&UNORM_UNICODE_3_2) {
        ಧ್ರುವ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ಪ್ರತ್ಯಾಗಮ _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    ಪ್ರತ್ಯಾಗಮ _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ unorm_iterate(src, ಅಸತ್ಯ,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_next(UCharIterator *src,
           char16_t *dest, int32_t destCapacity,
           UNormalizationMode mode, int32_t options,
           UBool doNormalize, UBool *pNeededToNormalize,
           UErrorCode *pErrorCode) {
    ಪ್ರತ್ಯಾಗಮ unorm_iterate(src, ಸತ್ಯ,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

ಸ್ಥಿರ int32_t
_concatenate(ಧ್ರುವ char16_t *left, int32_t ಎಡ_Length,
                  ಧ್ರುವ char16_t *ಬಲ, int32_t ಬಲ_Length,
                  char16_t *dest, int32_t destCapacity,
                  ಧ್ರುವ Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    ಯದಿ(U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || ಎಡ_Length<-1 || ಬಲ==nullptr || ಬಲ_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* check for overlapping right and destination */
    ಯದಿ( dest!=nullptr &&
        ((ಬಲ>=dest && ಬಲ<(dest+destCapacity)) ||
         (ಬಲ_Length>0 && dest>=ಬಲ && dest<(ಬಲ+ಬಲ_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ಯದಿ(left==dest) {
        destString.setTo(dest, ಎಡ_Length, destCapacity);
    } ಅನ್ಯಥಾ {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, ಎಡ_Length);
    }
    ಪ್ರತ್ಯಾಗಮ n2->append(destString, UnicodeString(ಬಲ_Length<0, ಬಲ, ಬಲ_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ಧ್ರುವ char16_t *left, int32_t ಎಡ_Length,
                  ಧ್ರುವ char16_t *ಬಲ, int32_t ಬಲ_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    ಧ್ರುವ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ಯದಿ(options&UNORM_UNICODE_3_2) {
        ಧ್ರುವ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ಯದಿ(U_FAILURE(*pErrorCode)) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ಪ್ರತ್ಯಾಗಮ _concatenate(left, ಎಡ_Length, ಬಲ, ಬಲ_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    ಪ್ರತ್ಯಾಗಮ _concatenate(left, ಎಡ_Length, ಬಲ, ಬಲ_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
