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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_NORMALIZATION

#അന്തര്ഭാവ "unicode/udata.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/uiter.h"
#അന്തര്ഭാവ "unicode/unorm.h"
#അന്തര്ഭാവ "unicode/unorm2.h"
#അന്തര്ഭാവ "normalizer2impl.h"
#അന്തര്ഭാവ "unormimp.h"
#അന്തര്ഭാവ "uprops.h"
#അന്തര്ഭാവ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(സ്ഥിരം char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    തിരികെ unorm2_quickCheck((സ്ഥിരം UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(സ്ഥിരം char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    എങ്കിൽ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        തിരികെ unorm2_quickCheck(
            reinterpret_cast<സ്ഥിരം UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } അല്ലെങ്കിൽ {
        തിരികെ unorm2_quickCheck((സ്ഥിരം UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(സ്ഥിരം char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    തിരികെ unorm2_isNormalized((സ്ഥിരം UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(സ്ഥിരം char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    എങ്കിൽ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        തിരികെ unorm2_isNormalized(
            reinterpret_cast<സ്ഥിരം UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } അല്ലെങ്കിൽ {
        തിരികെ unorm2_isNormalized((സ്ഥിരം UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(സ്ഥിരം char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    എങ്കിൽ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        തിരികെ unorm2_normalize(
            reinterpret_cast<സ്ഥിരം UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } അല്ലെങ്കിൽ {
        തിരികെ unorm2_normalize((സ്ഥിരം UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

സ്ഥിതം int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              സ്ഥിരം Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    എങ്കിൽ(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=അസത്യം;
    }
    എങ്കിൽ(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        തിരികെ u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    എങ്കിൽ(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ആകുമ്പോൾ((c=uiter_next32(src))>=0) {
            എങ്കിൽ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                നിർത്തുക;
            } അല്ലെങ്കിൽ {
                buffer.append(c);
            }
        }
    } അല്ലെങ്കിൽ {
        ആകുമ്പോൾ((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            എങ്കിൽ(n2->hasBoundaryBefore(c)) {
                നിർത്തുക;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    എങ്കിൽ(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        എങ്കിൽ(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        തിരികെ destString.length();
    } അല്ലെങ്കിൽ {
        /* just copy the source characters */
        തിരികെ buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

സ്ഥിതം int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    എങ്കിൽ(options&UNORM_UNICODE_3_2) {
        സ്ഥിരം UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            തിരികെ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        തിരികെ _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    തിരികെ _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    തിരികെ unorm_iterate(src, അസത്യം,
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
    തിരികെ unorm_iterate(src, സത്യം,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

സ്ഥിതം int32_t
_concatenate(സ്ഥിരം char16_t *left, int32_t ഇടത്_Length,
                  സ്ഥിരം char16_t *വലത്, int32_t വലത്_Length,
                  char16_t *dest, int32_t destCapacity,
                  സ്ഥിരം Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    എങ്കിൽ(U_FAILURE(*pErrorCode)) {
        തിരികെ 0;
    }
    എങ്കിൽ(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || ഇടത്_Length<-1 || വലത്==nullptr || വലത്_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* check for overlapping right and destination */
    എങ്കിൽ( dest!=nullptr &&
        ((വലത്>=dest && വലത്<(dest+destCapacity)) ||
         (വലത്_Length>0 && dest>=വലത് && dest<(വലത്+വലത്_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    എങ്കിൽ(left==dest) {
        destString.setTo(dest, ഇടത്_Length, destCapacity);
    } അല്ലെങ്കിൽ {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, ഇടത്_Length);
    }
    തിരികെ n2->append(destString, UnicodeString(വലത്_Length<0, വലത്, വലത്_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(സ്ഥിരം char16_t *left, int32_t ഇടത്_Length,
                  സ്ഥിരം char16_t *വലത്, int32_t വലത്_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    സ്ഥിരം Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    എങ്കിൽ(options&UNORM_UNICODE_3_2) {
        സ്ഥിരം UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        എങ്കിൽ(U_FAILURE(*pErrorCode)) {
            തിരികെ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        തിരികെ _concatenate(left, ഇടത്_Length, വലത്, വലത്_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    തിരികെ _concatenate(left, ഇടത്_Length, വലത്, വലത്_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
