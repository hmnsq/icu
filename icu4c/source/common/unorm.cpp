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

U_CAPI UNormalization_Check_پایله U_EXPORT2
unorm_چټک_Check(ثابت char16_t *src,
                 int32_t src_اوږدتیا, 
                 UNormalization_ډېرپېښ ډېرپېښ,
                 UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    أرجع unorm2_چټک_Check((ثابت UNormalizer2 *)n2, src, src_اوږدتیا, p_Error_کوډ);
}

U_CAPI UNormalization_Check_پایله U_EXPORT2
unorm_چټک_Check_With_Options(ثابت char16_t *src, int32_t src_اوږدتیا,
                            UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                            UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوډ));
        أرجع unorm2_چټک_Check(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_اوږدتیا, p_Error_کوډ);
    } وإلا {
        أرجع unorm2_چټک_Check((ثابت UNormalizer2 *)n2, src, src_اوږدتیا, p_Error_کوډ);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ثابت char16_t *src, int32_t src_اوږدتیا,
                   UNormalization_ډېرپېښ ډېرپېښ,
                   UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_اوږدتیا, p_Error_کوډ);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ثابت char16_t *src, int32_t src_اوږدتیا,
                              UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                              UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوډ));
        أرجع unorm2_isNormalized(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_اوږدتیا, p_Error_کوډ);
    } وإلا {
        أرجع unorm2_isNormalized((ثابت UNormalizer2 *)n2, src, src_اوږدتیا, p_Error_کوډ);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ثابت char16_t *src, int32_t src_اوږدتیا,
                UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                char16_t *dest, int32_t dest_ظرفیت,
                UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    إذا(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_کوډ));
        أرجع unorm2_normalize(
            reinterpret_cast<ثابت UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_اوږدتیا, dest, dest_ظرفیت, p_Error_کوډ);
    } وإلا {
        أرجع unorm2_normalize((ثابت UNormalizer2 *)n2,
            src, src_اوږدتیا, dest, dest_ظرفیت, p_Error_کوډ);
    }
}


/* iteration functions ------------------------------------------------------ */

ساكن int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ظرفیت,
              ثابت Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==تش_اشاره && dest_ظرفیت>0) || src==تش_اشاره) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    إذا(p_Needed_منزل_Normalize!=تش_اشاره) {
        *p_Needed_منزل_Normalize=خطأ;
    }
    إذا(!(forward ? src->has_بل(src) : src->has_پخوانی(src))) {
        أرجع u_terminateUChars(dest, dest_ظرفیت, 0, p_Error_کوډ);
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
                src->move(src, -U16_اوږدتیا(c), UITER_CURRENT);
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

    UnicodeString destString(dest, 0, dest_ظرفیت);
    إذا(بفر.length()>0 && doNormalize) {
        n2->normalize(بفر, destString, *p_Error_کوډ).extract(dest, dest_ظرفیت, *p_Error_کوډ);
        إذا(p_Needed_منزل_Normalize!=تش_اشاره && U_SUCCESS(*p_Error_کوډ)) {
            *p_Needed_منزل_Normalize= destString!=بفر;
        }
        أرجع destString.length();
    } وإلا {
        /* just copy the source characters */
        أرجع بفر.extract(dest, dest_ظرفیت, *p_Error_کوډ);
    }
}

ساكن int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ظرفیت,
              UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
              UBool doNormalize, UBool *p_Needed_منزل_Normalize,
              UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سېټ *uni32 = uniset_getUnicode32Instance(*p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _iterate(src, forward, dest, dest_ظرفیت,
            &fn2, doNormalize, p_Needed_منزل_Normalize, p_Error_کوډ);
    }
    أرجع _iterate(src, forward, dest, dest_ظرفیت,
            n2, doNormalize, p_Needed_منزل_Normalize, p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
unorm_پخوانی(UCharIterator *src,
               char16_t *dest, int32_t dest_ظرفیت,
               UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
               UBool doNormalize, UBool *p_Needed_منزل_Normalize,
               UError_کوډ *p_Error_کوډ) {
    أرجع unorm_iterate(src, خطأ,
                         dest, dest_ظرفیت,
                         ډېرپېښ, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
unorm_بل(UCharIterator *src,
           char16_t *dest, int32_t dest_ظرفیت,
           UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
           UBool doNormalize, UBool *p_Needed_منزل_Normalize,
           UError_کوډ *p_Error_کوډ) {
    أرجع unorm_iterate(src, ریښتیا,
                         dest, dest_ظرفیت,
                         ډېرپېښ, options,
                         doNormalize, p_Needed_منزل_Normalize,
                         p_Error_کوډ);
}

/* Concatenation of normalized strings -------------------------------------- */

ساكن int32_t
_concatenate(ثابت char16_t *کیڼ, int32_t کیڼ_اوږدتیا,
                  ثابت char16_t *ښی, int32_t ښی_اوږدتیا,
                  char16_t *dest, int32_t dest_ظرفیت,
                  ثابت Normalizer2 *n2,
                  UError_کوډ *p_Error_کوډ) {
    إذا(U_FAILURE(*p_Error_کوډ)) {
        أرجع 0;
    }
    إذا(dest_ظرفیت<0 || (dest==تش_اشاره && dest_ظرفیت>0) ||
        کیڼ==تش_اشاره || کیڼ_اوږدتیا<-1 || ښی==تش_اشاره || ښی_اوږدتیا<-1) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* check for overlapping right and destination */
    إذا( dest!=تش_اشاره &&
        ((ښی>=dest && ښی<(dest+dest_ظرفیت)) ||
         (ښی_اوږدتیا>0 && dest>=ښی && dest<(ښی+ښی_اوږدتیا)))
    ) {
        *p_Error_کوډ=U_ILLEGAL_دلیل_ERROR;
        أرجع 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    إذا(کیڼ==dest) {
        destString.سېټ_منزل(dest, کیڼ_اوږدتیا, dest_ظرفیت);
    } وإلا {
        destString.سېټ_منزل(dest, 0, dest_ظرفیت);
        destString.append(کیڼ, کیڼ_اوږدتیا);
    }
    أرجع n2->append(destString, UnicodeString(ښی_اوږدتیا<0, ښی, ښی_اوږدتیا), *p_Error_کوډ).
           extract(dest, dest_ظرفیت, *p_Error_کوډ);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ثابت char16_t *کیڼ, int32_t کیڼ_اوږدتیا,
                  ثابت char16_t *ښی, int32_t ښی_اوږدتیا,
                  char16_t *dest, int32_t dest_ظرفیت,
                  UNormalization_ډېرپېښ ډېرپېښ, int32_t options,
                  UError_کوډ *p_Error_کوډ) {
    ثابت Normalizer2 *n2=Normalizer2Factory::getInstance(ډېرپېښ, *p_Error_کوډ);
    إذا(options&UNORM_UNICODE_3_2) {
        ثابت Unicode_سېټ *uni32 = uniset_getUnicode32Instance(*p_Error_کوډ);
        إذا(U_FAILURE(*p_Error_کوډ)) {
            أرجع 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        أرجع _concatenate(کیڼ, کیڼ_اوږدتیا, ښی, ښی_اوږدتیا,
            dest, dest_ظرفیت, &fn2, p_Error_کوډ);
    }
    أرجع _concatenate(کیڼ, کیڼ_اوږدتیا, ښی, ښی_اوږدتیا,
        dest, dest_ظرفیت, n2, p_Error_کوډ);
}

#د_که_پای /* #if !UCONFIG_NO_NORMALIZATION */
