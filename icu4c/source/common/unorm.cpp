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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_NORMALIZATION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/udata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uiter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unorm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unorm2.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "normalizer2impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unormimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uprops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_ਨਤੀਜਾ U_EXPORT2
unorm_ਤੇਜ਼_Check(ਅਚਲ char16_t *src,
                 int32_t src_ਲੰਮਾਈ, 
                 UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਵਾਪਸ unorm2_ਤੇਜ਼_Check((ਅਚਲ UNormalizer2 *)n2, src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

U_CAPI UNormalization_Check_ਨਤੀਜਾ U_EXPORT2
unorm_ਤੇਜ਼_Check_With_Options(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                            UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਜੇ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ਕੋਡ));
        ਵਾਪਸ unorm2_ਤੇਜ਼_Check(
            reinterpret_cast<ਅਚਲ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ unorm2_ਤੇਜ਼_Check((ਅਚਲ UNormalizer2 *)n2, src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                   UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ,
                   UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਵਾਪਸ unorm2_isNormalized((ਅਚਲ UNormalizer2 *)n2, src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                              UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਜੇ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ਕੋਡ));
        ਵਾਪਸ unorm2_isNormalized(
            reinterpret_cast<ਅਚਲ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ unorm2_isNormalized((ਅਚਲ UNormalizer2 *)n2, src, src_ਲੰਮਾਈ, p_Error_ਕੋਡ);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ਅਚਲ char16_t *src, int32_t src_ਲੰਮਾਈ,
                UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਜੇ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_ਕੋਡ));
        ਵਾਪਸ unorm2_normalize(
            reinterpret_cast<ਅਚਲ UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_ਲੰਮਾਈ, dest, dest_ਸਮਰੱਥਾ, p_Error_ਕੋਡ);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ unorm2_normalize((ਅਚਲ UNormalizer2 *)n2,
            src, src_ਲੰਮਾਈ, dest, dest_ਸਮਰੱਥਾ, p_Error_ਕੋਡ);
    }
}


/* iteration functions ------------------------------------------------------ */

ਸਥਿਰ int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
              ਅਚਲ Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_ਮੰਜ਼ਿਲ_Normalize,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) || src==ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    ਜੇ(p_Needed_ਮੰਜ਼ਿਲ_Normalize!=ਖਾਲੀ_ਸੂਚਕ) {
        *p_Needed_ਮੰਜ਼ਿਲ_Normalize=ਝੂਠ;
    }
    ਜੇ(!(forward ? src->has_ਅਗਲਾ(src) : src->has_ਪਿਛਲਾ(src))) {
        ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, 0, p_Error_ਕੋਡ);
    }

    UnicodeString ਬਫਰ;
    UChar32 c;
    ਜੇ(forward) {
        /* get one character and ignore its properties */
        ਬਫਰ.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        ਜਦੋਂ((c=uiter_next32(src))>=0) {
            ਜੇ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_ਲੰਮਾਈ(c), UITER_CURRENT);
                ਰੋਕੋ;
            } ਨਹੀਂਤਾਂ {
                ਬਫਰ.append(c);
            }
        }
    } ਨਹੀਂਤਾਂ {
        ਜਦੋਂ((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            ਬਫਰ.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ਜੇ(n2->hasBoundaryBefore(c)) {
                ਰੋਕੋ;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_ਸਮਰੱਥਾ);
    ਜੇ(ਬਫਰ.length()>0 && doNormalize) {
        n2->normalize(ਬਫਰ, destString, *p_Error_ਕੋਡ).extract(dest, dest_ਸਮਰੱਥਾ, *p_Error_ਕੋਡ);
        ਜੇ(p_Needed_ਮੰਜ਼ਿਲ_Normalize!=ਖਾਲੀ_ਸੂਚਕ && U_SUCCESS(*p_Error_ਕੋਡ)) {
            *p_Needed_ਮੰਜ਼ਿਲ_Normalize= destString!=ਬਫਰ;
        }
        ਵਾਪਸ destString.length();
    } ਨਹੀਂਤਾਂ {
        /* just copy the source characters */
        ਵਾਪਸ ਬਫਰ.extract(dest, dest_ਸਮਰੱਥਾ, *p_Error_ਕੋਡ);
    }
}

ਸਥਿਰ int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
              UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
              UBool doNormalize, UBool *p_Needed_ਮੰਜ਼ਿਲ_Normalize,
              UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਜੇ(options&UNORM_UNICODE_3_2) {
        ਅਚਲ Unicode_ਸੈੱਟ *uni32 = uniset_getUnicode32Instance(*p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ਵਾਪਸ _iterate(src, forward, dest, dest_ਸਮਰੱਥਾ,
            &fn2, doNormalize, p_Needed_ਮੰਜ਼ਿਲ_Normalize, p_Error_ਕੋਡ);
    }
    ਵਾਪਸ _iterate(src, forward, dest, dest_ਸਮਰੱਥਾ,
            n2, doNormalize, p_Needed_ਮੰਜ਼ਿਲ_Normalize, p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
unorm_ਪਿਛਲਾ(UCharIterator *src,
               char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
               UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
               UBool doNormalize, UBool *p_Needed_ਮੰਜ਼ਿਲ_Normalize,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ unorm_iterate(src, ਝੂਠ,
                         dest, dest_ਸਮਰੱਥਾ,
                         ਬਹੁਲਕ, options,
                         doNormalize, p_Needed_ਮੰਜ਼ਿਲ_Normalize,
                         p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
unorm_ਅਗਲਾ(UCharIterator *src,
           char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
           UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
           UBool doNormalize, UBool *p_Needed_ਮੰਜ਼ਿਲ_Normalize,
           UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਵਾਪਸ unorm_iterate(src, ਸੱਚ,
                         dest, dest_ਸਮਰੱਥਾ,
                         ਬਹੁਲਕ, options,
                         doNormalize, p_Needed_ਮੰਜ਼ਿਲ_Normalize,
                         p_Error_ਕੋਡ);
}

/* Concatenation of normalized strings -------------------------------------- */

ਸਥਿਰ int32_t
_concatenate(ਅਚਲ char16_t *ਖੱਬੇ, int32_t ਖੱਬਾ_ਲੰਮਾਈ,
                  ਅਚਲ char16_t *ਸੱਜੇ, int32_t ਸੱਜਾ_ਲੰਮਾਈ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  ਅਚਲ Normalizer2 *n2,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ 0;
    }
    ਜੇ(dest_ਸਮਰੱਥਾ<0 || (dest==ਖਾਲੀ_ਸੂਚਕ && dest_ਸਮਰੱਥਾ>0) ||
        ਖੱਬੇ==ਖਾਲੀ_ਸੂਚਕ || ਖੱਬਾ_ਲੰਮਾਈ<-1 || ਸੱਜੇ==ਖਾਲੀ_ਸੂਚਕ || ਸੱਜਾ_ਲੰਮਾਈ<-1) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* check for overlapping right and destination */
    ਜੇ( dest!=ਖਾਲੀ_ਸੂਚਕ &&
        ((ਸੱਜੇ>=dest && ਸੱਜੇ<(dest+dest_ਸਮਰੱਥਾ)) ||
         (ਸੱਜਾ_ਲੰਮਾਈ>0 && dest>=ਸੱਜੇ && dest<(ਸੱਜੇ+ਸੱਜਾ_ਲੰਮਾਈ)))
    ) {
        *p_Error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ਜੇ(ਖੱਬੇ==dest) {
        destString.ਸੈੱਟ_ਮੰਜ਼ਿਲ(dest, ਖੱਬਾ_ਲੰਮਾਈ, dest_ਸਮਰੱਥਾ);
    } ਨਹੀਂਤਾਂ {
        destString.ਸੈੱਟ_ਮੰਜ਼ਿਲ(dest, 0, dest_ਸਮਰੱਥਾ);
        destString.append(ਖੱਬੇ, ਖੱਬਾ_ਲੰਮਾਈ);
    }
    ਵਾਪਸ n2->append(destString, UnicodeString(ਸੱਜਾ_ਲੰਮਾਈ<0, ਸੱਜੇ, ਸੱਜਾ_ਲੰਮਾਈ), *p_Error_ਕੋਡ).
           extract(dest, dest_ਸਮਰੱਥਾ, *p_Error_ਕੋਡ);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ਅਚਲ char16_t *ਖੱਬੇ, int32_t ਖੱਬਾ_ਲੰਮਾਈ,
                  ਅਚਲ char16_t *ਸੱਜੇ, int32_t ਸੱਜਾ_ਲੰਮਾਈ,
                  char16_t *dest, int32_t dest_ਸਮਰੱਥਾ,
                  UNormalization_ਬਹੁਲਕ ਬਹੁਲਕ, int32_t options,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ Normalizer2 *n2=Normalizer2Factory::getInstance(ਬਹੁਲਕ, *p_Error_ਕੋਡ);
    ਜੇ(options&UNORM_UNICODE_3_2) {
        ਅਚਲ Unicode_ਸੈੱਟ *uni32 = uniset_getUnicode32Instance(*p_Error_ਕੋਡ);
        ਜੇ(U_FAILURE(*p_Error_ਕੋਡ)) {
            ਵਾਪਸ 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        ਵਾਪਸ _concatenate(ਖੱਬੇ, ਖੱਬਾ_ਲੰਮਾਈ, ਸੱਜੇ, ਸੱਜਾ_ਲੰਮਾਈ,
            dest, dest_ਸਮਰੱਥਾ, &fn2, p_Error_ਕੋਡ);
    }
    ਵਾਪਸ _concatenate(ਖੱਬੇ, ਖੱਬਾ_ਲੰਮਾਈ, ਸੱਜੇ, ਸੱਜਾ_ਲੰਮਾਈ,
        dest, dest_ਸਮਰੱਥਾ, n2, p_Error_ਕੋਡ);
}

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_NORMALIZATION */
