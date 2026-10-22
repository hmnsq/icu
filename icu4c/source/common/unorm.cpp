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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_NORMALIZATION

#अन्तर्भाव "unicode/udata.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/uiter.h"
#अन्तर्भाव "unicode/unorm.h"
#अन्तर्भाव "unicode/unorm2.h"
#अन्तर्भाव "normalizer2impl.h"
#अन्तर्भाव "unormimp.h"
#अन्तर्भाव "uprops.h"
#अन्तर्भाव "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_परिणाम U_EXPORT2
unorm_शीघ्र_Check(ध्रुव char16_t *src,
                 int32_t src_दीर्घता, 
                 UNormalization_बहुलकम् बहुलकम्,
                 UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    प्रत्यागम unorm2_शीघ्र_Check((ध्रुव UNormalizer2 *)n2, src, src_दीर्घता, p_Error_संहिता);
}

U_CAPI UNormalization_Check_परिणाम U_EXPORT2
unorm_शीघ्र_Check_With_Options(ध्रुव char16_t *src, int32_t src_दीर्घता,
                            UNormalization_बहुलकम् बहुलकम्, int32_t options,
                            UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_संहिता));
        प्रत्यागम unorm2_शीघ्र_Check(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_दीर्घता, p_Error_संहिता);
    } अन्यथा {
        प्रत्यागम unorm2_शीघ्र_Check((ध्रुव UNormalizer2 *)n2, src, src_दीर्घता, p_Error_संहिता);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(ध्रुव char16_t *src, int32_t src_दीर्घता,
                   UNormalization_बहुलकम् बहुलकम्,
                   UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    प्रत्यागम unorm2_isNormalized((ध्रुव UNormalizer2 *)n2, src, src_दीर्घता, p_Error_संहिता);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(ध्रुव char16_t *src, int32_t src_दीर्घता,
                              UNormalization_बहुलकम् बहुलकम्, int32_t options,
                              UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_संहिता));
        प्रत्यागम unorm2_isNormalized(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_दीर्घता, p_Error_संहिता);
    } अन्यथा {
        प्रत्यागम unorm2_isNormalized((ध्रुव UNormalizer2 *)n2, src, src_दीर्घता, p_Error_संहिता);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(ध्रुव char16_t *src, int32_t src_दीर्घता,
                UNormalization_बहुलकम् बहुलकम्, int32_t options,
                char16_t *dest, int32_t dest_क्षमता,
                UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    यदि(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_संहिता));
        प्रत्यागम unorm2_normalize(
            reinterpret_cast<ध्रुव UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_दीर्घता, dest, dest_क्षमता, p_Error_संहिता);
    } अन्यथा {
        प्रत्यागम unorm2_normalize((ध्रुव UNormalizer2 *)n2,
            src, src_दीर्घता, dest, dest_क्षमता, p_Error_संहिता);
    }
}


/* iteration functions ------------------------------------------------------ */

स्थिर int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_क्षमता,
              ध्रुव Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_गन्तव्यम्_Normalize,
              UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0) || src==शून्यसूचकः) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    यदि(p_Needed_गन्तव्यम्_Normalize!=शून्यसूचकः) {
        *p_Needed_गन्तव्यम्_Normalize=असत्य;
    }
    यदि(!(forward ? src->has_अग्र(src) : src->hasPrevious(src))) {
        प्रत्यागम u_terminateUChars(dest, dest_क्षमता, 0, p_Error_संहिता);
    }

    UnicodeString बफर;
    UChar32 c;
    यदि(forward) {
        /* get one character and ignore its properties */
        बफर.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        यावत्((c=uiter_next32(src))>=0) {
            यदि(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_दीर्घता(c), UITER_CURRENT);
                भङ्ग;
            } अन्यथा {
                बफर.append(c);
            }
        }
    } अन्यथा {
        यावत्((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            बफर.insert(0, c);
            /* stop if this just-copied character is a boundary */
            यदि(n2->hasBoundaryBefore(c)) {
                भङ्ग;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_क्षमता);
    यदि(बफर.length()>0 && doNormalize) {
        n2->normalize(बफर, destString, *p_Error_संहिता).extract(dest, dest_क्षमता, *p_Error_संहिता);
        यदि(p_Needed_गन्तव्यम्_Normalize!=शून्यसूचकः && U_SUCCESS(*p_Error_संहिता)) {
            *p_Needed_गन्तव्यम्_Normalize= destString!=बफर;
        }
        प्रत्यागम destString.length();
    } अन्यथा {
        /* just copy the source characters */
        प्रत्यागम बफर.extract(dest, dest_क्षमता, *p_Error_संहिता);
    }
}

स्थिर int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_क्षमता,
              UNormalization_बहुलकम् बहुलकम्, int32_t options,
              UBool doNormalize, UBool *p_Needed_गन्तव्यम्_Normalize,
              UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    यदि(options&UNORM_UNICODE_3_2) {
        ध्रुव Unicode_समुच्चयः *uni32 = uniset_getUnicode32Instance(*p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        प्रत्यागम _iterate(src, forward, dest, dest_क्षमता,
            &fn2, doNormalize, p_Needed_गन्तव्यम्_Normalize, p_Error_संहिता);
    }
    प्रत्यागम _iterate(src, forward, dest, dest_क्षमता,
            n2, doNormalize, p_Needed_गन्तव्यम्_Normalize, p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
unorm_previous(UCharIterator *src,
               char16_t *dest, int32_t dest_क्षमता,
               UNormalization_बहुलकम् बहुलकम्, int32_t options,
               UBool doNormalize, UBool *p_Needed_गन्तव्यम्_Normalize,
               UError_संहिता *p_Error_संहिता) {
    प्रत्यागम unorm_iterate(src, असत्य,
                         dest, dest_क्षमता,
                         बहुलकम्, options,
                         doNormalize, p_Needed_गन्तव्यम्_Normalize,
                         p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
unorm_अग्र(UCharIterator *src,
           char16_t *dest, int32_t dest_क्षमता,
           UNormalization_बहुलकम् बहुलकम्, int32_t options,
           UBool doNormalize, UBool *p_Needed_गन्तव्यम्_Normalize,
           UError_संहिता *p_Error_संहिता) {
    प्रत्यागम unorm_iterate(src, सत्य,
                         dest, dest_क्षमता,
                         बहुलकम्, options,
                         doNormalize, p_Needed_गन्तव्यम्_Normalize,
                         p_Error_संहिता);
}

/* Concatenation of normalized strings -------------------------------------- */

स्थिर int32_t
_concatenate(ध्रुव char16_t *वामतः, int32_t वामः_दीर्घता,
                  ध्रुव char16_t *दक्षिणतः, int32_t दक्षिणः_दीर्घता,
                  char16_t *dest, int32_t dest_क्षमता,
                  ध्रुव Normalizer2 *n2,
                  UError_संहिता *p_Error_संहिता) {
    यदि(U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम 0;
    }
    यदि(dest_क्षमता<0 || (dest==शून्यसूचकः && dest_क्षमता>0) ||
        वामतः==शून्यसूचकः || वामः_दीर्घता<-1 || दक्षिणतः==शून्यसूचकः || दक्षिणः_दीर्घता<-1) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* check for overlapping right and destination */
    यदि( dest!=शून्यसूचकः &&
        ((दक्षिणतः>=dest && दक्षिणतः<(dest+dest_क्षमता)) ||
         (दक्षिणः_दीर्घता>0 && dest>=दक्षिणतः && dest<(दक्षिणतः+दक्षिणः_दीर्घता)))
    ) {
        *p_Error_संहिता=U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    यदि(वामतः==dest) {
        destString.समुच्चयः_गन्तव्यम्(dest, वामः_दीर्घता, dest_क्षमता);
    } अन्यथा {
        destString.समुच्चयः_गन्तव्यम्(dest, 0, dest_क्षमता);
        destString.append(वामतः, वामः_दीर्घता);
    }
    प्रत्यागम n2->append(destString, UnicodeString(दक्षिणः_दीर्घता<0, दक्षिणतः, दक्षिणः_दीर्घता), *p_Error_संहिता).
           extract(dest, dest_क्षमता, *p_Error_संहिता);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(ध्रुव char16_t *वामतः, int32_t वामः_दीर्घता,
                  ध्रुव char16_t *दक्षिणतः, int32_t दक्षिणः_दीर्घता,
                  char16_t *dest, int32_t dest_क्षमता,
                  UNormalization_बहुलकम् बहुलकम्, int32_t options,
                  UError_संहिता *p_Error_संहिता) {
    ध्रुव Normalizer2 *n2=Normalizer2Factory::getInstance(बहुलकम्, *p_Error_संहिता);
    यदि(options&UNORM_UNICODE_3_2) {
        ध्रुव Unicode_समुच्चयः *uni32 = uniset_getUnicode32Instance(*p_Error_संहिता);
        यदि(U_FAILURE(*p_Error_संहिता)) {
            प्रत्यागम 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        प्रत्यागम _concatenate(वामतः, वामः_दीर्घता, दक्षिणतः, दक्षिणः_दीर्घता,
            dest, dest_क्षमता, &fn2, p_Error_संहिता);
    }
    प्रत्यागम _concatenate(वामतः, वामः_दीर्घता, दक्षिणतः, दक्षिणः_दीर्घता,
        dest, dest_क्षमता, n2, p_Error_संहिता);
}

#यदि_अन्तः /* #if !UCONFIG_NO_NORMALIZATION */
