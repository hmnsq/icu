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

#إذا !UCONFIG_NO_NORMALIZATION

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

U_CAPI UNormalization_Check_نتیجه U_EXPORT2
unorm_سریع_Check(ثابت char16_t *src,
                 int32_t src_درازی, 
                 UNormalization_نما نما,
                 UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    أرجع unorm2_سریع_Check((ثابت UNormalizer2 *)n2, src, src_درازی, p_Error_کد);
}

U_CAPI UNormalization_Check_نتیجه U_EXPORT2
unorm_سریع_Check_With_Options(ثابت char16_t *src, int32_t src_درازی,
                            UNormalization_نما نما, int32_t options,
                            UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کد));
        أرجع unorm2_سریع_Check(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_درازی, p_Error_کد);
    } وإلا {
        أرجع unorm2_سریع_Check((ثابت UNormalizer2 *)n2, src, src_درازی, p_Error_کد);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t src_درازی,
                   UNormalization_نما نما,
                   UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_درازی, p_Error_کد);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t src_درازی,
                              UNormalization_نما نما, int32_t options,
                              UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کد));
        أرجع unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_درازی, p_Error_کد);
    } وإلا {
        أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_درازی, p_Error_کد);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t src_درازی,
                UNormalization_نما نما, int32_t options,
                char16_t *dest, int32_t dest_ظرفیت,
                UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کد));
        أرجع unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_درازی, dest, dest_ظرفیت, p_Error_کد);
    } وإلا {
        أرجع unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, src_درازی, dest, dest_ظرفیت, p_Error_کد);
    }
}


/* iteration functions ------------------------------------------------------ */

ساكن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ظرفیت,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
              UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0) || src==اشاره_تهی) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    إذا(p_Needed_مقصد_Normalize!=اشاره_تهی) {
        *p_Needed_مقصد_Normalize=خطأ;
    }
    إذا(!(forward ? src->has_بعدی(src) : src->has_قبلی(src))) {
        أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کد);
    }

    UnicodeString میانگیر;
    UChar32 c;
    إذا(forward) {
        /* get one character and ignore its properties */
        میانگیر.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        طالما((c=uiter_next32(src))>=0) {
            إذا(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_درازی(c), UITER_CURRENT);
                اكسر;
            } وإلا {
                میانگیر.append(c);
            }
        }
    } وإلا {
        طالما((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            میانگیر.insert(0, c);
            /* stop if this just-copied character is a boundary */
            إذا(n2->hasBoundaryBefore(c)) {
                اكسر;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_ظرفیت);
    إذا(میانگیر.length()>0 && doNormalize) {
        n2->normalize(میانگیر, destString, *p_Error_کد).extract(dest, dest_ظرفیت, *p_Error_کد);
        إذا(p_Needed_مقصد_Normalize!=اشاره_تهی && U_SUCCESS(*p_Error_کد)) {
            *p_Needed_مقصد_Normalize= destString!=میانگیر;
        }
        أرجع destString.length();
    } وإلا {
        /* just copy the source characters */
        أرجع میانگیر.extract(dest, dest_ظرفیت, *p_Error_کد);
    }
}

ساكن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ظرفیت,
              UNormalization_نما نما, int32_t options,
              UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
              UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_مجموعه *uni32 = uniset_getUnicode32Instance(*p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _iterate(src, forward, dest, dest_ظرفیت,
            &fn2, doNormalize, p_Needed_مقصد_Normalize, p_Error_کد);
    }
    أرجع _iterate(src, forward, dest, dest_ظرفیت,
            n2, doNormalize, p_Needed_مقصد_Normalize, p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
unorm_قبلی(UCharIterator *src,
               char16_t *dest, int32_t dest_ظرفیت,
               UNormalization_نما نما, int32_t options,
               UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
               UError_کد *p_Error_کد) {
    أرجع unorm_iterate(src, خطأ,
                         dest, dest_ظرفیت,
                         نما, options,
                         doNormalize, p_Needed_مقصد_Normalize,
                         p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
unorm_بعدی(UCharIterator *src,
           char16_t *dest, int32_t dest_ظرفیت,
           UNormalization_نما نما, int32_t options,
           UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
           UError_کد *p_Error_کد) {
    أرجع unorm_iterate(src, درست,
                         dest, dest_ظرفیت,
                         نما, options,
                         doNormalize, p_Needed_مقصد_Normalize,
                         p_Error_کد);
}

/* Concatenation of normalized strings -------------------------------------- */

ساكن int32_t
_concatenate(ثابت char16_t *چپ, int32_t چپ_درازی,
                  ثابت char16_t *راست, int32_t راست_درازی,
                  char16_t *dest, int32_t dest_ظرفیت,
                  ثابت Normalizer2 *n2,
                  UError_کد *p_Error_کد) {
    إذا(U_FAILURE(*p_Error_کد)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==اشاره_تهی && dest_ظرفیت>0) ||
        چپ==اشاره_تهی || چپ_درازی<-1 || راست==اشاره_تهی || راست_درازی<-1) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* check for overlapping right and destination */
    إذا( dest!=اشاره_تهی &&
        ((راست>=dest && راست<(dest+dest_ظرفیت)) ||
         (راست_درازی>0 && dest>=راست && dest<(راست+راست_درازی)))
    ) {
        *p_Error_کد=U_ILLEGAL_آرگومان_ERROR;
        أرجع 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    إذا(چپ==dest) {
        destString.مجموعه_مقصد(dest, چپ_درازی, dest_ظرفیت);
    } وإلا {
        destString.مجموعه_مقصد(dest, 0, dest_ظرفیت);
        destString.append(چپ, چپ_درازی);
    }
    أرجع n2->append(destString, UnicodeString(راست_درازی<0, راست, راست_درازی), *p_Error_کد).
           extract(dest, dest_ظرفیت, *p_Error_کد);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *چپ, int32_t چپ_درازی,
                  ثابت char16_t *راست, int32_t راست_درازی,
                  char16_t *dest, int32_t dest_ظرفیت,
                  UNormalization_نما نما, int32_t options,
                  UError_کد *p_Error_کد) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(نما, *p_Error_کد);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_مجموعه *uni32 = uniset_getUnicode32Instance(*p_Error_کد);
        إذا(U_FAILURE(*p_Error_کد)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _concatenate(چپ, چپ_درازی, راست, راست_درازی,
            dest, dest_ظرفیت, &fn2, p_Error_کد);
    }
    أرجع _concatenate(چپ, چپ_درازی, راست, راست_درازی,
        dest, dest_ظرفیت, n2, p_Error_کد);
}

#پایان_اگر /* #if !UCONFIG_NO_NORMALIZATION */
