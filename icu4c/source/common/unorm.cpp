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

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"

#ଯଦି !UCONFIG_NO_NORMALIZATION

#ଅନ୍ତର୍ଭାଵ "unicode/udata.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/uiter.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unorm.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unorm2.h"
#ଅନ୍ତର୍ଭାଵ "normalizer2impl.h"
#ଅନ୍ତର୍ଭାଵ "unormimp.h"
#ଅନ୍ତର୍ଭାଵ "uprops.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(ଅଚଳ char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଫେରନ୍ତୁ unorm2_quickCheck((ଅଚଳ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(ଅଚଳ char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଯଦି(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ଫେରନ୍ତୁ unorm2_quickCheck(
            reinterpret_cast<ଅଚଳ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ unorm2_quickCheck((ଅଚଳ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ଅଚଳ char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଫେରନ୍ତୁ unorm2_isNormalized((ଅଚଳ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ଅଚଳ char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଯଦି(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ଫେରନ୍ତୁ unorm2_isNormalized(
            reinterpret_cast<ଅଚଳ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ unorm2_isNormalized((ଅଚଳ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ଅଚଳ char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଯଦି(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ଫେରନ୍ତୁ unorm2_normalize(
            reinterpret_cast<ଅଚଳ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ unorm2_normalize((ଅଚଳ UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

ସ୍ଥିର int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              ଅଚଳ Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    ଯଦି(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=ମିଥ୍ୟା;
    }
    ଯଦି(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    ଯଦି(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ଯେତେବେଳେ((c=uiter_next32(src))>=0) {
            ଯଦି(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                ଭାଙ୍ଗ;
            } ଅନ୍ୟଥା {
                buffer.append(c);
            }
        }
    } ଅନ୍ୟଥା {
        ଯେତେବେଳେ((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ଯଦି(n2->hasBoundaryBefore(c)) {
                ଭାଙ୍ଗ;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    ଯଦି(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        ଯଦି(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        ଫେରନ୍ତୁ destString.length();
    } ଅନ୍ୟଥା {
        /* just copy the source characters */
        ଫେରନ୍ତୁ buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

ସ୍ଥିର int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଯଦି(options&UNORM_UNICODE_3_2) {
        ଅଚଳ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ଫେରନ୍ତୁ _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    ଫେରନ୍ତୁ _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    ଫେରନ୍ତୁ unorm_iterate(src, ମିଥ୍ୟା,
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
    ଫେରନ୍ତୁ unorm_iterate(src, ସତ୍ୟ,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

ସ୍ଥିର int32_t
_concatenate(ଅଚଳ char16_t *left, int32_t ବାମ_Length,
                  ଅଚଳ char16_t *ଡାହାଣ, int32_t ଡାହାଣ_Length,
                  char16_t *dest, int32_t destCapacity,
                  ଅଚଳ Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    ଯଦି(U_FAILURE(*pErrorCode)) {
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || ବାମ_Length<-1 || ଡାହାଣ==nullptr || ଡାହାଣ_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* check for overlapping right and destination */
    ଯଦି( dest!=nullptr &&
        ((ଡାହାଣ>=dest && ଡାହାଣ<(dest+destCapacity)) ||
         (ଡାହାଣ_Length>0 && dest>=ଡାହାଣ && dest<(ଡାହାଣ+ଡାହାଣ_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ଫେରନ୍ତୁ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ଯଦି(left==dest) {
        destString.setTo(dest, ବାମ_Length, destCapacity);
    } ଅନ୍ୟଥା {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, ବାମ_Length);
    }
    ଫେରନ୍ତୁ n2->append(destString, UnicodeString(ଡାହାଣ_Length<0, ଡାହାଣ, ଡାହାଣ_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ଅଚଳ char16_t *left, int32_t ବାମ_Length,
                  ଅଚଳ char16_t *ଡାହାଣ, int32_t ଡାହାଣ_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    ଅଚଳ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ଯଦି(options&UNORM_UNICODE_3_2) {
        ଅଚଳ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ଯଦି(U_FAILURE(*pErrorCode)) {
            ଫେରନ୍ତୁ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ଫେରନ୍ତୁ _concatenate(left, ବାମ_Length, ଡାହାଣ, ଡାହାଣ_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    ଫେରନ୍ତୁ _concatenate(left, ବାମ_Length, ଡାହାଣ, ଡାହାଣ_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
