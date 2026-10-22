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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_NORMALIZATION

#অন্তর্ভাব "unicode/udata.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/uiter.h"
#অন্তর্ভাব "unicode/unorm.h"
#অন্তর্ভাব "unicode/unorm2.h"
#অন্তর্ভাব "normalizer2impl.h"
#অন্তর্ভাব "unormimp.h"
#অন্তর্ভাব "uprops.h"
#অন্তর্ভাব "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_ফলাফল U_EXPORT2
unorm_quickCheck(ধ্রুবক char16_t *src,
                 int32_t src_দৈর্ঘ্য, 
                 UNormalizationMode mode,
                 UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ফেরত unorm2_quickCheck((ধ্রুবক UNormalizer2 *)n2, src, src_দৈর্ঘ্য, pErrorCode);
}

U_CAPI UNormalization_Check_ফলাফল U_EXPORT2
unorm_quickCheckWithOptions(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                            UNormalizationMode mode, int32_t options,
                            UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    যদি(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ফেরত unorm2_quickCheck(
            reinterpret_cast<ধ্রুবক UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_দৈর্ঘ্য, pErrorCode);
    } অন্যথা {
        ফেরত unorm2_quickCheck((ধ্রুবক UNormalizer2 *)n2, src, src_দৈর্ঘ্য, pErrorCode);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                   UNormalizationMode mode,
                   UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    ফেরত unorm2_isNormalized((ধ্রুবক UNormalizer2 *)n2, src, src_দৈর্ঘ্য, pErrorCode);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                              UNormalizationMode mode, int32_t options,
                              UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    যদি(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ফেরত unorm2_isNormalized(
            reinterpret_cast<ধ্রুবক UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_দৈর্ঘ্য, pErrorCode);
    } অন্যথা {
        ফেরত unorm2_isNormalized((ধ্রুবক UNormalizer2 *)n2, src, src_দৈর্ঘ্য, pErrorCode);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ধ্রুবক char16_t *src, int32_t src_দৈর্ঘ্য,
                UNormalizationMode mode, int32_t options,
                char16_t *dest, int32_t destCapacity,
                UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    যদি(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*pErrorCode));
        ফেরত unorm2_normalize(
            reinterpret_cast<ধ্রুবক UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_দৈর্ঘ্য, dest, destCapacity, pErrorCode);
    } অন্যথা {
        ফেরত unorm2_normalize((ধ্রুবক UNormalizer2 *)n2,
            src, src_দৈর্ঘ্য, dest, destCapacity, pErrorCode);
    }
}


/* iteration functions ------------------------------------------------------ */

স্থির int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              ধ্রুবক Normalizer2 *n2,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0) || src==nullptr) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    যদি(pNeededToNormalize!=nullptr) {
        *pNeededToNormalize=মিথ্যা;
    }
    যদি(!(forward ? src->has_পরবর্তী(src) : src->hasPrevious(src))) {
        ফেরত u_terminateUChars(dest, destCapacity, 0, pErrorCode);
    }

    UnicodeString বাফার;
    UChar32 c;
    যদি(forward) {
        /* get one character and ignore its properties */
        বাফার.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        যখন((c=uiter_next32(src))>=0) {
            যদি(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_দৈর্ঘ্য(c), UITER_CURRENT);
                বিরতি;
            } অন্যথা {
                বাফার.append(c);
            }
        }
    } অন্যথা {
        যখন((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            বাফার.insert(0, c);
            /* stop if this just-copied character is a boundary */
            যদি(n2->hasBoundaryBefore(c)) {
                বিরতি;
            }
        }
    }

    UnicodeString destString(dest, 0, destCapacity);
    যদি(বাফার.length()>0 && doNormalize) {
        n2->normalize(বাফার, destString, *pErrorCode).extract(dest, destCapacity, *pErrorCode);
        যদি(pNeededToNormalize!=nullptr && U_SUCCESS(*pErrorCode)) {
            *pNeededToNormalize= destString!=বাফার;
        }
        ফেরত destString.length();
    } অন্যথা {
        /* just copy the source characters */
        ফেরত বাফার.extract(dest, destCapacity, *pErrorCode);
    }
}

স্থির int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t destCapacity,
              UNormalizationMode mode, int32_t options,
              UBool doNormalize, UBool *pNeededToNormalize,
              UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    যদি(options&UNORM_UNICODE_3_2) {
        ধ্রুবক UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            ফেরত 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ফেরত _iterate(src, forward, dest, destCapacity,
            &fn2, doNormalize, pNeededToNormalize, pErrorCode);
    }
    ফেরত _iterate(src, forward, dest, destCapacity,
            n2, doNormalize, pNeededToNormalize, pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t destCapacity,
               UNormalizationMode mode, int32_t options,
               UBool doNormalize, UBool *pNeededToNormalize,
               UErrorCode *pErrorCode) {
    ফেরত unorm_iterate(src, মিথ্যা,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_পরবর্তী(UCharIterator *src,
           char16_t *dest, int32_t destCapacity,
           UNormalizationMode mode, int32_t options,
           UBool doNormalize, UBool *pNeededToNormalize,
           UErrorCode *pErrorCode) {
    ফেরত unorm_iterate(src, সত্য,
                         dest, destCapacity,
                         mode, options,
                         doNormalize, pNeededToNormalize,
                         pErrorCode);
}

/* Concatenation of normalized strings -------------------------------------- */

স্থির int32_t
_concatenate(ধ্রুবক char16_t *left, int32_t বাম_দৈর্ঘ্য,
                  ধ্রুবক char16_t *ডান, int32_t ডান_দৈর্ঘ্য,
                  char16_t *dest, int32_t destCapacity,
                  ধ্রুবক Normalizer2 *n2,
                  UErrorCode *pErrorCode) {
    যদি(U_FAILURE(*pErrorCode)) {
        ফেরত 0;
    }
    যদি(destCapacity<0 || (dest==nullptr && destCapacity>0) ||
        left==nullptr || বাম_দৈর্ঘ্য<-1 || ডান==nullptr || ডান_দৈর্ঘ্য<-1) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* check for overlapping right and destination */
    যদি( dest!=nullptr &&
        ((ডান>=dest && ডান<(dest+destCapacity)) ||
         (ডান_দৈর্ঘ্য>0 && dest>=ডান && dest<(ডান+ডান_দৈর্ঘ্য)))
    ) {
        *pErrorCode=U_ILLEGAL_ARGUMENT_ERROR;
        ফেরত 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    যদি(left==dest) {
        destString.setTo(dest, বাম_দৈর্ঘ্য, destCapacity);
    } অন্যথা {
        destString.setTo(dest, 0, destCapacity);
        destString.append(left, বাম_দৈর্ঘ্য);
    }
    ফেরত n2->append(destString, UnicodeString(ডান_দৈর্ঘ্য<0, ডান, ডান_দৈর্ঘ্য), *pErrorCode).
           extract(dest, destCapacity, *pErrorCode);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ধ্রুবক char16_t *left, int32_t বাম_দৈর্ঘ্য,
                  ধ্রুবক char16_t *ডান, int32_t ডান_দৈর্ঘ্য,
                  char16_t *dest, int32_t destCapacity,
                  UNormalizationMode mode, int32_t options,
                  UErrorCode *pErrorCode) {
    ধ্রুবক Normalizer2 *n2=Normalizer2Factory::getInstance(mode, *pErrorCode);
    যদি(options&UNORM_UNICODE_3_2) {
        ধ্রুবক UnicodeSet *uni32 = uniset_getUnicode32Instance(*pErrorCode);
        যদি(U_FAILURE(*pErrorCode)) {
            ফেরত 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ফেরত _concatenate(left, বাম_দৈর্ঘ্য, ডান, ডান_দৈর্ঘ্য,
            dest, destCapacity, &fn2, pErrorCode);
    }
    ফেরত _concatenate(left, বাম_দৈর্ঘ্য, ডান, ডান_দৈর্ঘ্য,
        dest, destCapacity, n2, pErrorCode);
}

#endif /* #if !UCONFIG_NO_NORMALIZATION */
