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

#include "unicode/utypes.h"

#ᱡᱩᱫᱤ !UCONFIG_NO_NORMALIZATION

#include "unicode/udata.h"
#include "unicode/ustring.h"
#include "unicode/uiter.h"
#include "unicode/unorm.h"
#include "unicode/unorm2.h"
#include "normalizer2impl.h"
#include "unormimp.h"
#include "uprops.h"
#include "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(const char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱨᱩᱣᱟᱹᱲ unorm2_quickCheck((const UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(const char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱡᱩᱫᱤ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ᱨᱩᱣᱟᱹᱲ unorm2_quickCheck(
            reinterpret_cast<const UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ unorm2_quickCheck((const UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(const char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱨᱩᱣᱟᱹᱲ unorm2_isNormalized((const UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(const char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱡᱩᱫᱤ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ᱨᱩᱣᱟᱹᱲ unorm2_isNormalized(
            reinterpret_cast<const UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ unorm2_isNormalized((const UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(const char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱡᱩᱫᱤ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ᱨᱩᱣᱟᱹᱲ unorm2_normalize(
            reinterpret_cast<const UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } ᱵᱟᱝ {
        ᱨᱩᱣᱟᱹᱲ unorm2_normalize((const UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

static int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              const Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ᱡᱩᱫᱤ(U_FAILURE(*pErrorCode)) {
        ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱡᱩᱫᱤ(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᱨᱩᱣᱟᱹᱲ 0;
    }

    ᱡᱩᱫᱤ(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=false;
    }
    ᱡᱩᱫᱤ(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        ᱨᱩᱣᱟᱹᱲ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    ᱡᱩᱫᱤ(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ᱡᱚᱠᱷᱚᱱ((c=uiter_next32(src))>=0) {
            ᱡᱩᱫᱤ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                bha_nga;
            } ᱵᱟᱝ {
                buffer.append(c);
            }
        }
    } ᱵᱟᱝ {
        ᱡᱚᱠᱷᱚᱱ((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ᱡᱩᱫᱤ(n2->hasBoundaryBefore(c)) {
                bha_nga;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    ᱡᱩᱫᱤ(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        ᱡᱩᱫᱤ(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        ᱨᱩᱣᱟᱹᱲ destString.length();
    } ᱵᱟᱝ {
        /* just copy the source characters */
        ᱨᱩᱣᱟᱹᱲ buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

static int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱡᱩᱫᱤ(options&UNORM_UNICODE_3_2) {
        const UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ᱡᱩᱫᱤ(U_FAILURE(*pErrorCode)) {
            ᱨᱩᱣᱟᱹᱲ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ᱨᱩᱣᱟᱹᱲ _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    ᱨᱩᱣᱟᱹᱲ _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    ᱨᱩᱣᱟᱹᱲ unorm_iterate(src, false,
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
    ᱨᱩᱣᱟᱹᱲ unorm_iterate(src, true,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

static int32_t
_concatenate(const char16_t *left, int32_t leftLength,
                  const char16_t *right, int32_t rightLength,
                  char16_t *dest, int32_t destCapacity,
                  const Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    ᱡᱩᱫᱤ(U_FAILURE(*pErrorCode)) {
        ᱨᱩᱣᱟᱹᱲ 0;
    }
    ᱡᱩᱫᱤ(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || leftLength<-1 || right==nullptr || rightLength<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᱨᱩᱣᱟᱹᱲ 0;
    }

    /* check for overlapping right and destination */
    ᱡᱩᱫᱤ( dest!=nullptr &&
        ((right>=dest && right<(dest+destCapacity)) ||
         (rightLength>0 && dest>=right && dest<(right+rightLength)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ᱨᱩᱣᱟᱹᱲ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ᱡᱩᱫᱤ(left==dest) {
        destString.setTo(dest, leftLength, destCapacity);
    } ᱵᱟᱝ {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, leftLength);
    }
    ᱨᱩᱣᱟᱹᱲ n2->append(destString, UnicodeString(rightLength<0, right, rightLength), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(const char16_t *left, int32_t leftLength,
                  const char16_t *right, int32_t rightLength,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    const Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ᱡᱩᱫᱤ(options&UNORM_UNICODE_3_2) {
        const UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        ᱡᱩᱫᱤ(U_FAILURE(*pErrorCode)) {
            ᱨᱩᱣᱟᱹᱲ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ᱨᱩᱣᱟᱹᱲ _concatenate(left, leftLength, right, rightLength,
            dest, destCapacity, &fn2, pErrorCode);
    }
    ᱨᱩᱣᱟᱹᱲ _concatenate(left, leftLength, right, rightLength,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
