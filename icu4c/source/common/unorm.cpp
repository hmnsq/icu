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

#અન્તર્ભાવ "unicode/utypes.h"

#જો !UCONFIG_NO_NORMALIZATION

#અન્તર્ભાવ "unicode/udata.h"
#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/uiter.h"
#અન્તર્ભાવ "unicode/unorm.h"
#અન્તર્ભાવ "unicode/unorm2.h"
#અન્તર્ભાવ "normalizer2impl.h"
#અન્તર્ભાવ "unormimp.h"
#અન્તર્ભાવ "uprops.h"
#અન્તર્ભાવ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(અચલ char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    પાછા unorm2_quickCheck((અચલ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(અચલ char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    જો(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        પાછા unorm2_quickCheck(
            reinterpret_cast<અચલ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } નહીંતર {
        પાછા unorm2_quickCheck((અચલ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(અચલ char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    પાછા unorm2_isNormalized((અચલ UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(અચલ char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    જો(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        પાછા unorm2_isNormalized(
            reinterpret_cast<અચલ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } નહીંતર {
        પાછા unorm2_isNormalized((અચલ UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(અચલ char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    જો(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        પાછા unorm2_normalize(
            reinterpret_cast<અચલ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } નહીંતર {
        પાછા unorm2_normalize((અચલ UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

સ્થિર int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              અચલ Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    જો(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=ખોટું;
    }
    જો(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        પાછા u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    જો(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        જ્યારે((c=uiter_next32(src))>=0) {
            જો(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                તોડો;
            } નહીંતર {
                buffer.append(c);
            }
        }
    } નહીંતર {
        જ્યારે((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            જો(n2->hasBoundaryBefore(c)) {
                તોડો;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    જો(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        જો(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        પાછા destString.length();
    } નહીંતર {
        /* just copy the source characters */
        પાછા buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

સ્થિર int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    જો(options&UNORM_UNICODE_3_2) {
        અચલ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            પાછા 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        પાછા _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    પાછા _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    પાછા unorm_iterate(src, ખોટું,
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
    પાછા unorm_iterate(src, સાચું,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

સ્થિર int32_t
_concatenate(અચલ char16_t *left, int32_t ડાબું_Length,
                  અચલ char16_t *જમણું, int32_t જમણું_Length,
                  char16_t *dest, int32_t destCapacity,
                  અચલ Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    જો(U_FAILURE(*pErrorCode)) {
        પાછા 0;
    }
    જો(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || ડાબું_Length<-1 || જમણું==nullptr || જમણું_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* check for overlapping right and destination */
    જો( dest!=nullptr &&
        ((જમણું>=dest && જમણું<(dest+destCapacity)) ||
         (જમણું_Length>0 && dest>=જમણું && dest<(જમણું+જમણું_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        પાછા 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    જો(left==dest) {
        destString.setTo(dest, ડાબું_Length, destCapacity);
    } નહીંતર {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, ડાબું_Length);
    }
    પાછા n2->append(destString, UnicodeString(જમણું_Length<0, જમણું, જમણું_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(અચલ char16_t *left, int32_t ડાબું_Length,
                  અચલ char16_t *જમણું, int32_t જમણું_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    અચલ Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    જો(options&UNORM_UNICODE_3_2) {
        અચલ UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        જો(U_FAILURE(*pErrorCode)) {
            પાછા 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        પાછા _concatenate(left, ડાબું_Length, જમણું, જમણું_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    પાછા _concatenate(left, ડાબું_Length, જમણું, જમણું_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
