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

#अन्तर्भाव "unicode/utypes.h"

#जर !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uiter.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "unicode/unorm2.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "unormimp.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(स्थिरांक char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    परत unorm2_quickCheck((स्थिरांक UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(स्थिरांक char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        परत unorm2_quickCheck(
            reinterpret_cast<स्थिरांक UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अन्यथा {
        परत unorm2_quickCheck((स्थिरांक UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(स्थिरांक char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    परत unorm2_isNormalized((स्थिरांक UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(स्थिरांक char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        परत unorm2_isNormalized(
            reinterpret_cast<स्थिरांक UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अन्यथा {
        परत unorm2_isNormalized((स्थिरांक UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(स्थिरांक char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        परत unorm2_normalize(
            reinterpret_cast<स्थिरांक UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } अन्यथा {
        परत unorm2_normalize((स्थिरांक UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

स्थिर int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              स्थिरांक Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    जर(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    जर(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=असत्य;
    }
    जर(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        परत u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    जर(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        पर्यंत((c=uiter_next32(src))>=0) {
            जर(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                थांबा;
            } अन्यथा {
                buffer.append(c);
            }
        }
    } अन्यथा {
        पर्यंत((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            जर(n2->hasBoundaryBefore(c)) {
                थांबा;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    जर(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        जर(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        परत destString.length();
    } अन्यथा {
        /* just copy the source characters */
        परत buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

स्थिर int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जर(options&UNORM_UNICODE_3_2) {
        स्थिरांक UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        जर(U_FAILURE(*pErrorCode)) {
            परत 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        परत _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    परत _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    परत unorm_iterate(src, असत्य,
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
    परत unorm_iterate(src, सत्य,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

स्थिर int32_t
_concatenate(स्थिरांक char16_t *left, int32_t डावा_Length,
                  स्थिरांक char16_t *उजवा, int32_t उजवा_Length,
                  char16_t *dest, int32_t destCapacity,
                  स्थिरांक Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    जर(U_FAILURE(*pErrorCode)) {
        परत 0;
    }
    जर(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || डावा_Length<-1 || उजवा==nullptr || उजवा_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* check for overlapping right and destination */
    जर( dest!=nullptr &&
        ((उजवा>=dest && उजवा<(dest+destCapacity)) ||
         (उजवा_Length>0 && dest>=उजवा && dest<(उजवा+उजवा_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        परत 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    जर(left==dest) {
        destString.setTo(dest, डावा_Length, destCapacity);
    } अन्यथा {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, डावा_Length);
    }
    परत n2->append(destString, UnicodeString(उजवा_Length<0, उजवा, उजवा_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(स्थिरांक char16_t *left, int32_t डावा_Length,
                  स्थिरांक char16_t *उजवा, int32_t उजवा_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    स्थिरांक Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जर(options&UNORM_UNICODE_3_2) {
        स्थिरांक UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        जर(U_FAILURE(*pErrorCode)) {
            परत 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        परत _concatenate(left, डावा_Length, उजवा, उजवा_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    परत _concatenate(left, डावा_Length, उजवा, उजवा_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
