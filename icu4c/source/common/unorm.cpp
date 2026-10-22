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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_NORMALIZATION

#شامل "unicode/udata.h"
#شامل "unicode/ustring.h"
#شامل "unicode/uiter.h"
#شامل "unicode/unorm.h"
#شامل "unicode/unorm2.h"
#شامل "normalizer2impl.h"
#شامل "unormimp.h"
#شامل "uprops.h"
#شامل "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_نتیجہ U_EXPORT2
unorm_تیز_Check(ثابت char16_t *src,
                 int32_t src_لمبائی, 
                 UNormalization_منوال منوال,
                 UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    واپس unorm2_تیز_Check((ثابت UNormalizer2 *)n2, src, src_لمبائی, p_Error_رمز);
}

U_CAPI UNormalization_Check_نتیجہ U_EXPORT2
unorm_تیز_Check_With_Options(ثابت char16_t *src, int32_t src_لمبائی,
                            UNormalization_منوال منوال, int32_t options,
                            UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    اگر(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_رمز));
        واپس unorm2_تیز_Check(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمبائی, p_Error_رمز);
    } ورنہ {
        واپس unorm2_تیز_Check((ثابت UNormalizer2 *)n2, src, src_لمبائی, p_Error_رمز);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t src_لمبائی,
                   UNormalization_منوال منوال,
                   UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    واپس unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_لمبائی, p_Error_رمز);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t src_لمبائی,
                              UNormalization_منوال منوال, int32_t options,
                              UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    اگر(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_رمز));
        واپس unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمبائی, p_Error_رمز);
    } ورنہ {
        واپس unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_لمبائی, p_Error_رمز);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t src_لمبائی,
                UNormalization_منوال منوال, int32_t options,
                char16_t *dest, int32_t dest_استعداد,
                UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    اگر(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_رمز));
        واپس unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمبائی, dest, dest_استعداد, p_Error_رمز);
    } ورنہ {
        واپس unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, src_لمبائی, dest, dest_استعداد, p_Error_رمز);
    }
}


/* iteration functions ------------------------------------------------------ */

ساکن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_استعداد,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
              UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(dest_استعداد<0 || (dest==خالی_اشاریہ && dest_استعداد>0) || src==خالی_اشاریہ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    اگر(p_Needed_مقصد_Normalize!=خالی_اشاریہ) {
        *p_Needed_مقصد_Normalize=جھوٹ;
    }
    اگر(!(forward ? src->has_اگلا(src) : src->has_پچھلا(src))) {
        واپس u_terminateUChars(dest, dest_استعداد, 0, p_Error_رمز);
    }

    UnicodeString بفر;
    UChar32 c;
    اگر(forward) {
        /* get one character and ignore its properties */
        بفر.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        جب((c=uiter_next32(src))>=0) {
            اگر(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_لمبائی(c), UITER_CURRENT);
                توڑو;
            } ورنہ {
                بفر.append(c);
            }
        }
    } ورنہ {
        جب((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            بفر.insert(0, c);
            /* stop if this just-copied character is a boundary */
            اگر(n2->hasBoundaryBefore(c)) {
                توڑو;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_استعداد);
    اگر(بفر.length()>0 && doNormalize) {
        n2->normalize(بفر, destString, *p_Error_رمز).extract(dest, dest_استعداد, *p_Error_رمز);
        اگر(p_Needed_مقصد_Normalize!=خالی_اشاریہ && U_SUCCESS(*p_Error_رمز)) {
            *p_Needed_مقصد_Normalize= destString!=بفر;
        }
        واپس destString.length();
    } ورنہ {
        /* just copy the source characters */
        واپس بفر.extract(dest, dest_استعداد, *p_Error_رمز);
    }
}

ساکن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_استعداد,
              UNormalization_منوال منوال, int32_t options,
              UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
              UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    اگر(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سیٹ *uni32 = uniset_getUnicode32Instance(*p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        واپس _iterate(src, forward, dest, dest_استعداد,
            &fn2, doNormalize, p_Needed_مقصد_Normalize, p_Error_رمز);
    }
    واپس _iterate(src, forward, dest, dest_استعداد,
            n2, doNormalize, p_Needed_مقصد_Normalize, p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
unorm_پچھلا(UCharIterator *src,
               char16_t *dest, int32_t dest_استعداد,
               UNormalization_منوال منوال, int32_t options,
               UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
               UError_رمز *p_Error_رمز) {
    واپس unorm_iterate(src, جھوٹ,
                         dest, dest_استعداد,
                         منوال, options,
                         doNormalize, p_Needed_مقصد_Normalize,
                         p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
unorm_اگلا(UCharIterator *src,
           char16_t *dest, int32_t dest_استعداد,
           UNormalization_منوال منوال, int32_t options,
           UBool doNormalize, UBool *p_Needed_مقصد_Normalize,
           UError_رمز *p_Error_رمز) {
    واپس unorm_iterate(src, سچ,
                         dest, dest_استعداد,
                         منوال, options,
                         doNormalize, p_Needed_مقصد_Normalize,
                         p_Error_رمز);
}

/* Concatenation of normalized strings -------------------------------------- */

ساکن int32_t
_concatenate(ثابت char16_t *بائیں, int32_t بایاں_لمبائی,
                  ثابت char16_t *دائیں, int32_t دایاں_لمبائی,
                  char16_t *dest, int32_t dest_استعداد,
                  ثابت Normalizer2 *n2,
                  UError_رمز *p_Error_رمز) {
    اگر(U_FAILURE(*p_Error_رمز)) {
        واپس 0;
    }
    اگر(dest_استعداد<0 || (dest==خالی_اشاریہ && dest_استعداد>0) ||
        بائیں==خالی_اشاریہ || بایاں_لمبائی<-1 || دائیں==خالی_اشاریہ || دایاں_لمبائی<-1) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* check for overlapping right and destination */
    اگر( dest!=خالی_اشاریہ &&
        ((دائیں>=dest && دائیں<(dest+dest_استعداد)) ||
         (دایاں_لمبائی>0 && dest>=دائیں && dest<(دائیں+دایاں_لمبائی)))
    ) {
        *p_Error_رمز=U_ILLEGAL_دلیل_ERROR;
        واپس 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    اگر(بائیں==dest) {
        destString.سیٹ_مقصد(dest, بایاں_لمبائی, dest_استعداد);
    } ورنہ {
        destString.سیٹ_مقصد(dest, 0, dest_استعداد);
        destString.append(بائیں, بایاں_لمبائی);
    }
    واپس n2->append(destString, UnicodeString(دایاں_لمبائی<0, دائیں, دایاں_لمبائی), *p_Error_رمز).
           extract(dest, dest_استعداد, *p_Error_رمز);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *بائیں, int32_t بایاں_لمبائی,
                  ثابت char16_t *دائیں, int32_t دایاں_لمبائی,
                  char16_t *dest, int32_t dest_استعداد,
                  UNormalization_منوال منوال, int32_t options,
                  UError_رمز *p_Error_رمز) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_رمز);
    اگر(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سیٹ *uni32 = uniset_getUnicode32Instance(*p_Error_رمز);
        اگر(U_FAILURE(*p_Error_رمز)) {
            واپس 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        واپس _concatenate(بائیں, بایاں_لمبائی, دائیں, دایاں_لمبائی,
            dest, dest_استعداد, &fn2, p_Error_رمز);
    }
    واپس _concatenate(بائیں, بایاں_لمبائی, دائیں, دایاں_لمبائی,
        dest, dest_استعداد, n2, p_Error_رمز);
}

#اگر_ختم /* #if !UCONFIG_NO_NORMALIZATION */
