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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_NORMALIZATION

#समावेश "unicode/udata.h"
#समावेश "unicode/ustring.h"
#समावेश "unicode/uiter.h"
#समावेश "unicode/unorm.h"
#समावेश "unicode/unorm2.h"
#समावेश "normalizer2impl.h"
#समावेश "unormimp.h"
#समावेश "uprops.h"
#समावेश "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_परिणाम U_EXPORT2
unorm_quickCheck(स्थिर char16_t *src,
                 int32_t src_लंबाई, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    वापस unorm2_quickCheck((स्थिर UNormalizer2 *)n2, src, src_लंबाई, pErrorCode);
}

U_CAPI UNormalization_Check_परिणाम U_EXPORT2
unorm_quickCheckWithOptions(स्थिर char16_t *src, int32_t src_लंबाई,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    अगर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        वापस unorm2_quickCheck(
            reinterpret_cast<स्थिर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_लंबाई, pErrorCode);
    } अन्यथा {
        वापस unorm2_quickCheck((स्थिर UNormalizer2 *)n2, src, src_लंबाई, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(स्थिर char16_t *src, int32_t src_लंबाई,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    वापस unorm2_isNormalized((स्थिर UNormalizer2 *)n2, src, src_लंबाई, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(स्थिर char16_t *src, int32_t src_लंबाई,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    अगर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        वापस unorm2_isNormalized(
            reinterpret_cast<स्थिर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_लंबाई, pErrorCode);
    } अन्यथा {
        वापस unorm2_isNormalized((स्थिर UNormalizer2 *)n2, src, src_लंबाई, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(स्थिर char16_t *src, int32_t src_लंबाई,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    अगर(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        वापस unorm2_normalize(
            reinterpret_cast<स्थिर UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_लंबाई, dest, destCapacity, pErrorCode);
    } अन्यथा {
        वापस unorm2_normalize((स्थिर UNormalizer2 *)n2,
            src, src_लंबाई, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

अटल int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              स्थिर Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    अगर(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=false;
    }
    अगर(!(forward ? src->has_अग्र(src) : src->hasPrevious(src))) {
        वापस u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString बफर;
    UChar32 c;
    अगर(forward) {
        /* get one character and ignore its properties */
        बफर.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        जबतक((c=uiter_next32(src))>=0) {
            अगर(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_लंबाई(c), UITER_CURRENT);
                अवरोध;
            } अन्यथा {
                बफर.append(c);
            }
        }
    } अन्यथा {
        जबतक((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            बफर.insert(0, c);
            /* stop if this just-copied character is a boundary */
            अगर(n2->hasBoundaryBefore(c)) {
                अवरोध;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    अगर(बफर.length()>0 && doNormalize) {
        n2->normalize(बफर, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        अगर(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=बफर;
        }
        वापस destString.length();
    } अन्यथा {
        /* just copy the source characters */
        वापस बफर.extract(dest, destCapacity, *pErrorCode);
    }
}

अटल int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    अगर(options&UNORM_UNICODE_3_2) {
        स्थिर UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        वापस _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    वापस _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    वापस unorm_iterate(src, false,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_अग्र(UCharIterator *src,
           char16_t *dest, int32_t destCapacity,
           UNormalizationMode mode, int32_t options,
           UBool doNormalize, UBool *pNeededToNormalize,
           UErrorCode *pErrorCode) {
    वापस unorm_iterate(src, true,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

अटल int32_t
_concatenate(स्थिर char16_t *बाँए, int32_t बायाँ_लंबाई,
                  स्थिर char16_t *दाँए, int32_t दायाँ_लंबाई,
                  char16_t *dest, int32_t destCapacity,
                  स्थिर Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    अगर(U_FAILURE(*pErrorCode)) {
        वापस 0;
    }
    अगर(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        बाँए==nullptr || बायाँ_लंबाई<-1 || दाँए==nullptr || दायाँ_लंबाई<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* check for overlapping right and destination */
    अगर( dest!=nullptr &&
        ((दाँए>=dest && दाँए<(dest+destCapacity)) ||
         (दायाँ_लंबाई>0 && dest>=दाँए && dest<(दाँए+दायाँ_लंबाई)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        वापस 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    अगर(बाँए==dest) {
        destString.setTo(dest, बायाँ_लंबाई, destCapacity);
    } अन्यथा {
        destString.setTo(dest, 0, destCapacity);
        destString.append(बाँए, बायाँ_लंबाई);
    }
    वापस n2->append(destString, UnicodeString(दायाँ_लंबाई<0, दाँए, दायाँ_लंबाई), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(स्थिर char16_t *बाँए, int32_t बायाँ_लंबाई,
                  स्थिर char16_t *दाँए, int32_t दायाँ_लंबाई,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    स्थिर Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    अगर(options&UNORM_UNICODE_3_2) {
        स्थिर UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        अगर(U_FAILURE(*pErrorCode)) {
            वापस 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        वापस _concatenate(बाँए, बायाँ_लंबाई, दाँए, दायाँ_लंबाई,
            dest, destCapacity, &fn2, pErrorCode);
    }
    वापस _concatenate(बाँए, बायाँ_लंबाई, दाँए, दायाँ_लंबाई,
        dest, destCapacity, n2, pErrorCode);
}

#पूर्ण_अगर /* #if !UCONFIG_NO_NORMALIZATION */
