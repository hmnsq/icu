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

#यदि !UCONFIG_NO_NORMALIZATION

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
unorm_quickCheck(अचर char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    फर्काऊ unorm2_quickCheck((अचर UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(अचर char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        फर्काऊ unorm2_quickCheck(
            reinterpret_cast<अचर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अन्यथा {
        फर्काऊ unorm2_quickCheck((अचर UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(अचर char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    फर्काऊ unorm2_isNormalized((अचर UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(अचर char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        फर्काऊ unorm2_isNormalized(
            reinterpret_cast<अचर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } अन्यथा {
        फर्काऊ unorm2_isNormalized((अचर UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(अचर char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        फर्काऊ unorm2_normalize(
            reinterpret_cast<अचर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } अन्यथा {
        फर्काऊ unorm2_normalize((अचर UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

स्थिर int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              अचर Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    यदि(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=असत्य;
    }
    यदि(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        फर्काऊ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    यदि(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        जबसम्म((c=uiter_next32(src))>=0) {
            यदि(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                रोक;
            } अन्यथा {
                buffer.append(c);
            }
        }
    } अन्यथा {
        जबसम्म((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            यदि(n2->hasBoundaryBefore(c)) {
                रोक;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    यदि(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        यदि(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        फर्काऊ destString.length();
    } अन्यथा {
        /* just copy the source characters */
        फर्काऊ buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

स्थिर int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    यदि(options&UNORM_UNICODE_3_2) {
        अचर UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        यदि(U_FAILURE(*pErrorCode)) {
            फर्काऊ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        फर्काऊ _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    फर्काऊ _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    फर्काऊ unorm_iterate(src, असत्य,
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
    फर्काऊ unorm_iterate(src, सत्य,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

स्थिर int32_t
_concatenate(अचर char16_t *left, int32_t बायाँ_Length,
                  अचर char16_t *दायाँ, int32_t दायाँ_Length,
                  char16_t *dest, int32_t destCapacity,
                  अचर Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    यदि(U_FAILURE(*pErrorCode)) {
        फर्काऊ 0;
    }
    यदि(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || बायाँ_Length<-1 || दायाँ==nullptr || दायाँ_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* check for overlapping right and destination */
    यदि( dest!=nullptr &&
        ((दायाँ>=dest && दायाँ<(dest+destCapacity)) ||
         (दायाँ_Length>0 && dest>=दायाँ && dest<(दायाँ+दायाँ_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        फर्काऊ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    यदि(left==dest) {
        destString.setTo(dest, बायाँ_Length, destCapacity);
    } अन्यथा {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, बायाँ_Length);
    }
    फर्काऊ n2->append(destString, UnicodeString(दायाँ_Length<0, दायाँ, दायाँ_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(अचर char16_t *left, int32_t बायाँ_Length,
                  अचर char16_t *दायाँ, int32_t दायाँ_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    अचर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    यदि(options&UNORM_UNICODE_3_2) {
        अचर UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        यदि(U_FAILURE(*pErrorCode)) {
            फर्काऊ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        फर्काऊ _concatenate(left, बायाँ_Length, दायाँ, दायाँ_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    फर्काऊ _concatenate(left, बायाँ_Length, दायाँ, दायाँ_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
