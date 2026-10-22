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

U_CAPI UNormalization_Check_نتيجو U_EXPORT2
unorm_تيز_Check(ثابت char16_t *src,
                 int32_t src_ڊيگهائي, 
                 UNormalization_گهڻائي گهڻائي,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    أرجع unorm2_تيز_Check((ثابت UNormalizer2 *)n2, src, src_ڊيگهائي, p_Error_ڪوڊ);
}

U_CAPI UNormalization_Check_نتيجو U_EXPORT2
unorm_تيز_Check_With_Options(ثابت char16_t *src, int32_t src_ڊيگهائي,
                            UNormalization_گهڻائي گهڻائي, int32_t options,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ڪوڊ));
        أرجع unorm2_تيز_Check(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ڊيگهائي, p_Error_ڪوڊ);
    } وإلا {
        أرجع unorm2_تيز_Check((ثابت UNormalizer2 *)n2, src, src_ڊيگهائي, p_Error_ڪوڊ);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t src_ڊيگهائي,
                   UNormalization_گهڻائي گهڻائي,
                   UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_ڊيگهائي, p_Error_ڪوڊ);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t src_ڊيگهائي,
                              UNormalization_گهڻائي گهڻائي, int32_t options,
                              UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ڪوڊ));
        أرجع unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ڊيگهائي, p_Error_ڪوڊ);
    } وإلا {
        أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_ڊيگهائي, p_Error_ڪوڊ);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t src_ڊيگهائي,
                UNormalization_گهڻائي گهڻائي, int32_t options,
                char16_t *dest, int32_t dest_صلاحيت,
                UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ڪوڊ));
        أرجع unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ڊيگهائي, dest, dest_صلاحيت, p_Error_ڪوڊ);
    } وإلا {
        أرجع unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, src_ڊيگهائي, dest, dest_صلاحيت, p_Error_ڪوڊ);
    }
}


/* iteration functions ------------------------------------------------------ */

ساكن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_صلاحيت,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(dest_صلاحيت<0 || (dest==خالي_اشارو && dest_صلاحيت>0) || src==خالي_اشارو) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    إذا(p_Needed_منزل_Normalize!=خالي_اشارو) {
        *p_Needed_منزل_Normalize=خطأ;
    }
    إذا(!(forward ? src->has_ايندڙ(src) : src->has_اڳيون(src))) {
        أرجع u_terminateUChars(dest, dest_صلاحيت, 0, p_Error_ڪوڊ);
    }

    UnicodeString بفر;
    UChar32 c;
    إذا(forward) {
        /* get one character and ignore its properties */
        بفر.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        طالما((c=uiter_next32(src))>=0) {
            إذا(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_ڊيگهائي(c), UITER_CURRENT);
                اكسر;
            } وإلا {
                بفر.append(c);
            }
        }
    } وإلا {
        طالما((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            بفر.insert(0, c);
            /* stop if this just-copied character is a boundary */
            إذا(n2->hasBoundaryBefore(c)) {
                اكسر;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_صلاحيت);
    إذا(بفر.length()>0 && doNormalize) {
        n2->normalize(بفر, destString, *p_Error_ڪوڊ).extract(dest, dest_صلاحيت, *p_Error_ڪوڊ);
        إذا(p_Needed_منزل_Normalize!=خالي_اشارو && U_SUCCESS(*p_Error_ڪوڊ)) {
            *p_Needed_منزل_Normalize= destString!=بفر;
        }
        أرجع destString.length();
    } وإلا {
        /* just copy the source characters */
        أرجع بفر.extract(dest, dest_صلاحيت, *p_Error_ڪوڊ);
    }
}

ساكن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_صلاحيت,
              UNormalization_گهڻائي گهڻائي, int32_t options,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سيٽ *uni32 = uniset_getUnicode32Instance(*p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _iterate(src, forward, dest, dest_صلاحيت,
            &fn2, doNormalize, p_Needed_منزل_Normalize, p_Error_ڪوڊ);
    }
    أرجع _iterate(src, forward, dest, dest_صلاحيت,
            n2, doNormalize, p_Needed_منزل_Normalize, p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
unorm_اڳيون(UCharIterator *src,
               char16_t *dest, int32_t dest_صلاحيت,
               UNormalization_گهڻائي گهڻائي, int32_t options,
               UBool doNormalize, UBool *p_Needed_منزل_Normalize,
               UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع unorm_iterate(src, خطأ,
                         dest, dest_صلاحيت,
                         گهڻائي, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
unorm_ايندڙ(UCharIterator *src,
           char16_t *dest, int32_t dest_صلاحيت,
           UNormalization_گهڻائي گهڻائي, int32_t options,
           UBool doNormalize, UBool *p_Needed_منزل_Normalize,
           UError_ڪوڊ *p_Error_ڪوڊ) {
    أرجع unorm_iterate(src, سچ,
                         dest, dest_صلاحيت,
                         گهڻائي, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_ڪوڊ);
}

/* Concatenation of normalized strings -------------------------------------- */

ساكن int32_t
_concatenate(ثابت char16_t *کاٻي, int32_t کاٻو_ڊيگهائي,
                  ثابت char16_t *ساڄي, int32_t ساڄو_ڊيگهائي,
                  char16_t *dest, int32_t dest_صلاحيت,
                  ثابت Normalizer2 *n2,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    إذا(U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع 0;
    }
    إذا(dest_صلاحيت<0 || (dest==خالي_اشارو && dest_صلاحيت>0) ||
        کاٻي==خالي_اشارو || کاٻو_ڊيگهائي<-1 || ساڄي==خالي_اشارو || ساڄو_ڊيگهائي<-1) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* check for overlapping right and destination */
    إذا( dest!=خالي_اشارو &&
        ((ساڄي>=dest && ساڄي<(dest+dest_صلاحيت)) ||
         (ساڄو_ڊيگهائي>0 && dest>=ساڄي && dest<(ساڄي+ساڄو_ڊيگهائي)))
    ) {
        *p_Error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
        أرجع 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    إذا(کاٻي==dest) {
        destString.سيٽ_منزل(dest, کاٻو_ڊيگهائي, dest_صلاحيت);
    } وإلا {
        destString.سيٽ_منزل(dest, 0, dest_صلاحيت);
        destString.append(کاٻي, کاٻو_ڊيگهائي);
    }
    أرجع n2->append(destString, UnicodeString(ساڄو_ڊيگهائي<0, ساڄي, ساڄو_ڊيگهائي), *p_Error_ڪوڊ).
           extract(dest, dest_صلاحيت, *p_Error_ڪوڊ);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *کاٻي, int32_t کاٻو_ڊيگهائي,
                  ثابت char16_t *ساڄي, int32_t ساڄو_ڊيگهائي,
                  char16_t *dest, int32_t dest_صلاحيت,
                  UNormalization_گهڻائي گهڻائي, int32_t options,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(گهڻائي, *p_Error_ڪوڊ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سيٽ *uni32 = uniset_getUnicode32Instance(*p_Error_ڪوڊ);
        إذا(U_FAILURE(*p_Error_ڪوڊ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _concatenate(کاٻي, کاٻو_ڊيگهائي, ساڄي, ساڄو_ڊيگهائي,
            dest, dest_صلاحيت, &fn2, p_Error_ڪوڊ);
    }
    أرجع _concatenate(کاٻي, کاٻو_ڊيگهائي, ساڄي, ساڄو_ڊيگهائي,
        dest, dest_صلاحيت, n2, p_Error_ڪوڊ);
}

#جيڪڏهن_پورو /* #if !UCONFIG_NO_NORMALIZATION */
