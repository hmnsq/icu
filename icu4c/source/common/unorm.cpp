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

#जइ !UCONFIG_NO_NORMALIZATION

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
unorm_quickCheck(ध्रुव char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    प्रत्यागम unorm2_quickCheck((ध्रुव UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(ध्रुव char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जइ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        प्रत्यागम unorm2_quickCheck(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अण्णहा {
        प्रत्यागम unorm2_quickCheck((ध्रुव UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ध्रुव char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    प्रत्यागम unorm2_isNormalized((ध्रुव UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ध्रुव char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जइ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        प्रत्यागम unorm2_isNormalized(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अण्णहा {
        प्रत्यागम unorm2_isNormalized((ध्रुव UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ध्रुव char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जइ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        प्रत्यागम unorm2_normalize(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } अण्णहा {
        प्रत्यागम unorm2_normalize((ध्रुव UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

स्थिर int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              ध्रुव Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    जइ(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    जइ(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=मिच्छ;
    }
    जइ(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        प्रत्यागम u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    जइ(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        जाव((c=uiter_next32(src))>=0) {
            जइ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                भङ्ग;
            } अण्णहा {
                buffer.append(c);
            }
        }
    } अण्णहा {
        जाव((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            जइ(n2->hasBoundaryBefore(c)) {
                भङ्ग;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    जइ(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        जइ(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        प्रत्यागम destString.length();
    } अण्णहा {
        /* just copy the source characters */
        प्रत्यागम buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

स्थिर int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जइ(options&UNORM_UNICODE_3_2) {
        ध्रुव UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        जइ(U_FAILURE(*pErrorCode)) {
            प्रत्यागम 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        प्रत्यागम _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    प्रत्यागम _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    प्रत्यागम unorm_iterate(src, मिच्छ,
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
    प्रत्यागम unorm_iterate(src, सच्च,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

स्थिर int32_t
_concatenate(ध्रुव char16_t *left, int32_t वामः_Length,
                  ध्रुव char16_t *दक्षिणः, int32_t दक्षिणः_Length,
                  char16_t *dest, int32_t destCapacity,
                  ध्रुव Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    जइ(U_FAILURE(*pErrorCode)) {
        प्रत्यागम 0;
    }
    जइ(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || वामः_Length<-1 || दक्षिणः==nullptr || दक्षिणः_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* check for overlapping right and destination */
    जइ( dest!=nullptr &&
        ((दक्षिणः>=dest && दक्षिणः<(dest+destCapacity)) ||
         (दक्षिणः_Length>0 && dest>=दक्षिणः && dest<(दक्षिणः+दक्षिणः_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        प्रत्यागम 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    जइ(left==dest) {
        destString.setTo(dest, वामः_Length, destCapacity);
    } अण्णहा {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, वामः_Length);
    }
    प्रत्यागम n2->append(destString, UnicodeString(दक्षिणः_Length<0, दक्षिणः, दक्षिणः_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ध्रुव char16_t *left, int32_t वामः_Length,
                  ध्रुव char16_t *दक्षिणः, int32_t दक्षिणः_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    जइ(options&UNORM_UNICODE_3_2) {
        ध्रुव UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        जइ(U_FAILURE(*pErrorCode)) {
            प्रत्यागम 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        प्रत्यागम _concatenate(left, वामः_Length, दक्षिणः, दक्षिणः_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    प्रत्यागम _concatenate(left, वामः_Length, दक्षिणः, दक्षिणः_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
