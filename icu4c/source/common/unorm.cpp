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

#ضمن "unicode/utypes.h"

#𐤀𐤌 !UCONFIG_NO_NORMALIZATION

#ضمن "unicode/udata.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/uiter.h"
#ضمن "unicode/unorm.h"
#ضمن "unicode/unorm2.h"
#ضمن "normalizer2impl.h"
#ضمن "unormimp.h"
#ضمن "uprops.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(ثابت char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    أرجع unorm2_quickCheck((ثابت UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(ثابت char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    𐤀𐤌(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        أرجع unorm2_quickCheck(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } وإلا {
        أرجع unorm2_quickCheck((ثابت UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    𐤀𐤌(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        أرجع unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } وإلا {
        أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    𐤀𐤌(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        أرجع unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } وإلا {
        أرجع unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

ساكن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    𐤀𐤌(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=خطأ;
    }
    𐤀𐤌(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        أرجع u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    𐤀𐤌(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        طالما((c=uiter_next32(src))>=0) {
            𐤀𐤌(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                اكسر;
            } وإلا {
                buffer.append(c);
            }
        }
    } وإلا {
        طالما((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            𐤀𐤌(n2->hasBoundaryBefore(c)) {
                اكسر;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    𐤀𐤌(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        𐤀𐤌(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        أرجع destString.length();
    } وإلا {
        /* just copy the source characters */
        أرجع buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

ساكن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    𐤀𐤌(options&UNORM_UNICODE_3_2) {
        ثابت UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        𐤀𐤌(U_FAILURE(*pErrorCode)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    أرجع _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    أرجع unorm_iterate(src, خطأ,
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
    أرجع unorm_iterate(src, true,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

ساكن int32_t
_concatenate(ثابت char16_t *left, int32_t leftLength,
                  ثابت char16_t *right, int32_t rightLength,
                  char16_t *dest, int32_t destCapacity,
                  ثابت Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    𐤀𐤌(U_FAILURE(*pErrorCode)) {
        أرجع 0;
    }
    𐤀𐤌(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || leftLength<-1 || right==nullptr || rightLength<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* check for overlapping right and destination */
    𐤀𐤌( dest!=nullptr &&
        ((right>=dest && right<(dest+destCapacity)) ||
         (rightLength>0 && dest>=right && dest<(right+rightLength)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        أرجع 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    𐤀𐤌(left==dest) {
        destString.setTo(dest, leftLength, destCapacity);
    } وإلا {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, leftLength);
    }
    أرجع n2->append(destString, UnicodeString(rightLength<0, right, rightLength), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *left, int32_t leftLength,
                  ثابت char16_t *right, int32_t rightLength,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    𐤀𐤌(options&UNORM_UNICODE_3_2) {
        ثابت UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        𐤀𐤌(U_FAILURE(*pErrorCode)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _concatenate(left, leftLength, right, rightLength,
            dest, destCapacity, &fn2, pErrorCode);
    }
    أرجع _concatenate(left, leftLength, right, rightLength,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
