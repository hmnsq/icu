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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_NORMALIZATION

#அந்தர்பாவ "unicode/udata.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/uiter.h"
#அந்தர்பாவ "unicode/unorm.h"
#அந்தர்பாவ "unicode/unorm2.h"
#அந்தர்பாவ "normalizer2impl.h"
#அந்தர்பாவ "unormimp.h"
#அந்தர்பாவ "uprops.h"
#அந்தர்பாவ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheck(மாறிலி char16_t *src,
                 int32_t srcLength, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    திரும்பு unorm2_quickCheck((மாறிலி UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UNormalizationCheckResult U_EXPORT2
unorm_quickCheckWithOptions(மாறிலி char16_t *src, int32_t srcLength,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    எனில்(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        திரும்பு unorm2_quickCheck(
            reinterpret_cast<மாறிலி UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } இல்லையெனில் {
        திரும்பு unorm2_quickCheck((மாறிலி UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(மாறிலி char16_t *src, int32_t srcLength,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    திரும்பு unorm2_isNormalized((மாறிலி UNormalizer2 *)n2, src, srcLength, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(மாறிலி char16_t *src, int32_t srcLength,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    எனில்(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        திரும்பு unorm2_isNormalized(
            reinterpret_cast<மாறிலி UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, pErrorCode);
    } இல்லையெனில் {
        திரும்பு unorm2_isNormalized((மாறிலி UNormalizer2 *)n2, src, srcLength, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(மாறிலி char16_t *src, int32_t srcLength,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    எனில்(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        திரும்பு unorm2_normalize(
            reinterpret_cast<மாறிலி UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, srcLength, dest, destCapacity, pErrorCode);
    } இல்லையெனில் {
        திரும்பு unorm2_normalize((மாறிலி UNormalizer2 *)n2,
            src, srcLength, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

நிலையான int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              மாறிலி Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    எனில்(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=பொய்;
    }
    எனில்(!(forward ? src->hasNext(src) : src->hasPrevious(src))) {
        திரும்பு u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString buffer;
    UChar32 c;
    எனில்(forward) {
        /* get one character and ignore its properties */
        buffer.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        போது((c=uiter_next32(src))>=0) {
            எனில்(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_LENGTH(c), UITER_CURRENT);
                நிறுத்து;
            } இல்லையெனில் {
                buffer.append(c);
            }
        }
    } இல்லையெனில் {
        போது((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            buffer.insert(0, c);
            /* stop if this just-copied character is a boundary */
            எனில்(n2->hasBoundaryBefore(c)) {
                நிறுத்து;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    எனில்(buffer.length()>0 && doNormalize) {
        n2->normalize(buffer, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        எனில்(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=buffer;
        }
        திரும்பு destString.length();
    } இல்லையெனில் {
        /* just copy the source characters */
        திரும்பு buffer.extract(dest, destCapacity, *pErrorCode);
    }
}

நிலையான int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    எனில்(options&UNORM_UNICODE_3_2) {
        மாறிலி UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            திரும்பு 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        திரும்பு _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    திரும்பு _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    திரும்பு unorm_iterate(src, பொய்,
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
    திரும்பு unorm_iterate(src, உண்மை,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

நிலையான int32_t
_concatenate(மாறிலி char16_t *left, int32_t இடது_Length,
                  மாறிலி char16_t *வலது, int32_t வலது_Length,
                  char16_t *dest, int32_t destCapacity,
                  மாறிலி Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    எனில்(U_FAILURE(*pErrorCode)) {
        திரும்பு 0;
    }
    எனில்(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || இடது_Length<-1 || வலது==nullptr || வலது_Length<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* check for overlapping right and destination */
    எனில்( dest!=nullptr &&
        ((வலது>=dest && வலது<(dest+destCapacity)) ||
         (வலது_Length>0 && dest>=வலது && dest<(வலது+வலது_Length)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    எனில்(left==dest) {
        destString.setTo(dest, இடது_Length, destCapacity);
    } இல்லையெனில் {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, இடது_Length);
    }
    திரும்பு n2->append(destString, UnicodeString(வலது_Length<0, வலது, வலது_Length), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(மாறிலி char16_t *left, int32_t இடது_Length,
                  மாறிலி char16_t *வலது, int32_t வலது_Length,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    மாறிலி Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    எனில்(options&UNORM_UNICODE_3_2) {
        மாறிலி UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        எனில்(U_FAILURE(*pErrorCode)) {
            திரும்பு 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        திரும்பு _concatenate(left, இடது_Length, வலது, வலது_Length,
            dest, destCapacity, &fn2, pErrorCode);
    }
    திரும்பு _concatenate(left, இடது_Length, வலது, வலது_Length,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
