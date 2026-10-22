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

U_CAPI UNormalization_Check_نتیجہ U_EXPORT2
unorm_تیز_Check(ثابت char16_t *src,
                 int32_t src_لمائی, 
                 UNormalization_منوال منوال,
                 UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    أرجع unorm2_تیز_Check((ثابت UNormalizer2 *)n2, src, src_لمائی, p_Error_کوڈ);
}

U_CAPI UNormalization_Check_نتیجہ U_EXPORT2
unorm_تیز_Check_With_Options(ثابت char16_t *src, int32_t src_لمائی,
                            UNormalization_منوال منوال, int32_t options,
                            UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوڈ));
        أرجع unorm2_تیز_Check(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمائی, p_Error_کوڈ);
    } وإلا {
        أرجع unorm2_تیز_Check((ثابت UNormalizer2 *)n2, src, src_لمائی, p_Error_کوڈ);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t src_لمائی,
                   UNormalization_منوال منوال,
                   UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_لمائی, p_Error_کوڈ);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t src_لمائی,
                              UNormalization_منوال منوال, int32_t options,
                              UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوڈ));
        أرجع unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمائی, p_Error_کوڈ);
    } وإلا {
        أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_لمائی, p_Error_کوڈ);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t src_لمائی,
                UNormalization_منوال منوال, int32_t options,
                char16_t *dest, int32_t dest_سمرتھا,
                UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوڈ));
        أرجع unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_لمائی, dest, dest_سمرتھا, p_Error_کوڈ);
    } وإلا {
        أرجع unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, src_لمائی, dest, dest_سمرتھا, p_Error_کوڈ);
    }
}


/* iteration functions ------------------------------------------------------ */

ساكن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_سمرتھا,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0) || src==خالی_سوچک) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(p_Needed_منزل_Normalize!=خالی_سوچک) {
        *p_Needed_منزل_Normalize=خطأ;
    }
    إذا(!(forward ? src->has_اگلا(src) : src->has_پچھلا(src))) {
        أرجع u_terminateUChars(dest, dest_سمرتھا, 0, p_Error_کوڈ);
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
                src->move(src, -U16_لمائی(c), UITER_CURRENT);
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

    UnicodeString destString(dest, 0, dest_سمرتھا);
    إذا(بفر.length()>0 && doNormalize) {
        n2->normalize(بفر, destString, *p_Error_کوڈ).extract(dest, dest_سمرتھا, *p_Error_کوڈ);
        إذا(p_Needed_منزل_Normalize!=خالی_سوچک && U_SUCCESS(*p_Error_کوڈ)) {
            *p_Needed_منزل_Normalize= destString!=بفر;
        }
        أرجع destString.length();
    } وإلا {
        /* just copy the source characters */
        أرجع بفر.extract(dest, dest_سمرتھا, *p_Error_کوڈ);
    }
}

ساكن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_سمرتھا,
              UNormalization_منوال منوال, int32_t options,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سیٹ *uni32 = uniset_getUnicode32Instance(*p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _iterate(src, forward, dest, dest_سمرتھا,
            &fn2, doNormalize, p_Needed_منزل_Normalize, p_Error_کوڈ);
    }
    أرجع _iterate(src, forward, dest, dest_سمرتھا,
            n2, doNormalize, p_Needed_منزل_Normalize, p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
unorm_پچھلا(UCharIterator *src,
               char16_t *dest, int32_t dest_سمرتھا,
               UNormalization_منوال منوال, int32_t options,
               UBool doNormalize, UBool *p_Needed_منزل_Normalize,
               UError_کوڈ *p_Error_کوڈ) {
    أرجع unorm_iterate(src, خطأ,
                         dest, dest_سمرتھا,
                         منوال, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
unorm_اگلا(UCharIterator *src,
           char16_t *dest, int32_t dest_سمرتھا,
           UNormalization_منوال منوال, int32_t options,
           UBool doNormalize, UBool *p_Needed_منزل_Normalize,
           UError_کوڈ *p_Error_کوڈ) {
    أرجع unorm_iterate(src, سچ,
                         dest, dest_سمرتھا,
                         منوال, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_کوڈ);
}

/* Concatenation of normalized strings -------------------------------------- */

ساكن int32_t
_concatenate(ثابت char16_t *کھبے, int32_t بایاں_لمائی,
                  ثابت char16_t *سجے, int32_t دایاں_لمائی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  ثابت Normalizer2 *n2,
                  UError_کوڈ *p_Error_کوڈ) {
    إذا(U_FAILURE(*p_Error_کوڈ)) {
        أرجع 0;
    }
    إذا(dest_سمرتھا<0 || (dest==خالی_سوچک && dest_سمرتھا>0) ||
        کھبے==خالی_سوچک || بایاں_لمائی<-1 || سجے==خالی_سوچک || دایاں_لمائی<-1) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* check for overlapping right and destination */
    إذا( dest!=خالی_سوچک &&
        ((سجے>=dest && سجے<(dest+dest_سمرتھا)) ||
         (دایاں_لمائی>0 && dest>=سجے && dest<(سجے+دایاں_لمائی)))
    ) {
        *p_Error_کوڈ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    إذا(کھبے==dest) {
        destString.سیٹ_منزل(dest, بایاں_لمائی, dest_سمرتھا);
    } وإلا {
        destString.سیٹ_منزل(dest, 0, dest_سمرتھا);
        destString.append(کھبے, بایاں_لمائی);
    }
    أرجع n2->append(destString, UnicodeString(دایاں_لمائی<0, سجے, دایاں_لمائی), *p_Error_کوڈ).
           extract(dest, dest_سمرتھا, *p_Error_کوڈ);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *کھبے, int32_t بایاں_لمائی,
                  ثابت char16_t *سجے, int32_t دایاں_لمائی,
                  char16_t *dest, int32_t dest_سمرتھا,
                  UNormalization_منوال منوال, int32_t options,
                  UError_کوڈ *p_Error_کوڈ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(منوال, *p_Error_کوڈ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سیٹ *uni32 = uniset_getUnicode32Instance(*p_Error_کوڈ);
        إذا(U_FAILURE(*p_Error_کوڈ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _concatenate(کھبے, بایاں_لمائی, سجے, دایاں_لمائی,
            dest, dest_سمرتھا, &fn2, p_Error_کوڈ);
    }
    أرجع _concatenate(کھبے, بایاں_لمائی, سجے, دایاں_لمائی,
        dest, dest_سمرتھا, n2, p_Error_کوڈ);
}

#جے_انت /* #if !UCONFIG_NO_NORMALIZATION */
