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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_NORMALIZATION

#అన్తర్భావ "unicode/udata.h"
#అన్తర్భావ "unicode/ustring.h"
#అన్తర్భావ "unicode/uiter.h"
#అన్తర్భావ "unicode/unorm.h"
#అన్తర్భావ "unicode/unorm2.h"
#అన్తర్భావ "normalizer2impl.h"
#అన్తర్భావ "unormimp.h"
#అన్తర్భావ "uprops.h"
#అన్తర్భావ "ustr_imp.h"

U_NAMESPACE_USE

/* quick check functions ---------------------------------------------------- */

U_CAPI UNormalization_Check_ఫలితం U_EXPORT2
unorm_శీఘ్ర_Check(స్థిరరాశి char16_t *src,
                 int32_t src_నిడివి, 
                 UNormalization_బహుళకం బహుళకం,
                 UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    తిరిగి unorm2_శీఘ్ర_Check((స్థిరరాశి UNormalizer2 *)n2, src, src_నిడివి, p_Error_సంకేతం);
}

U_CAPI UNormalization_Check_ఫలితం U_EXPORT2
unorm_శీఘ్ర_Check_With_Options(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                            UNormalization_బహుళకం బహుళకం, int32_t options,
                            UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    ఒకవేళ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_సంకేతం));
        తిరిగి unorm2_శీఘ్ర_Check(
            reinterpret_cast<స్థిరరాశి UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_నిడివి, p_Error_సంకేతం);
    } లేకపోతే {
        తిరిగి unorm2_శీఘ్ర_Check((స్థిరరాశి UNormalizer2 *)n2, src, src_నిడివి, p_Error_సంకేతం);
    }
}

U_CAPI UBool U_EXPORT2
unorm_isNormalized(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                   UNormalization_బహుళకం బహుళకం,
                   UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    తిరిగి unorm2_isNormalized((స్థిరరాశి UNormalizer2 *)n2, src, src_నిడివి, p_Error_సంకేతం);
}

U_CAPI UBool U_EXPORT2
unorm_isNormalizedWithOptions(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                              UNormalization_బహుళకం బహుళకం, int32_t options,
                              UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    ఒకవేళ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_సంకేతం));
        తిరిగి unorm2_isNormalized(
            reinterpret_cast<స్థిరరాశి UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_నిడివి, p_Error_సంకేతం);
    } లేకపోతే {
        తిరిగి unorm2_isNormalized((స్థిరరాశి UNormalizer2 *)n2, src, src_నిడివి, p_Error_సంకేతం);
    }
}

/* normalize() API ---------------------------------------------------------- */

/** Public API for normalizing. */
U_CAPI int32_t U_EXPORT2
unorm_normalize(స్థిరరాశి char16_t *src, int32_t src_నిడివి,
                UNormalization_బహుళకం బహుళకం, int32_t options,
                char16_t *dest, int32_t dest_సామర్థ్యం,
                UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    ఒకవేళ(options&UNORM_UNICODE_3_2) {
        FilteredNormalizer2 fn2(*n2, *uniset_getUnicode32Instance(*p_Error_సంకేతం));
        తిరిగి unorm2_normalize(
            reinterpret_cast<స్థిరరాశి UNormalizer2 *>(static_cast<Normalizer2 *>(&fn2)),
            src, src_నిడివి, dest, dest_సామర్థ్యం, p_Error_సంకేతం);
    } లేకపోతే {
        తిరిగి unorm2_normalize((స్థిరరాశి UNormalizer2 *)n2,
            src, src_నిడివి, dest, dest_సామర్థ్యం, p_Error_సంకేతం);
    }
}


/* iteration functions ------------------------------------------------------ */

స్థిర int32_t
_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_సామర్థ్యం,
              స్థిరరాశి Normalizer2 *n2,
              UBool doNormalize, UBool *p_Needed_గమ్యం_Normalize,
              UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) || src==శూన్యసూచిక) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    ఒకవేళ(p_Needed_గమ్యం_Normalize!=శూన్యసూచిక) {
        *p_Needed_గమ్యం_Normalize=అబద్ధం;
    }
    ఒకవేళ(!(forward ? src->has_తదుపరి(src) : src->has_మునుపటి(src))) {
        తిరిగి u_terminateUChars(dest, dest_సామర్థ్యం, 0, p_Error_సంకేతం);
    }

    UnicodeString బఫరు;
    UChar32 c;
    ఒకవేళ(forward) {
        /* get one character and ignore its properties */
        బఫరు.append(uiter_next32(src));
        /* get all following characters until we see a boundary */
        అయితే((c=uiter_next32(src))>=0) {
            ఒకవేళ(n2->hasBoundaryBefore(c)) {
                /* back out the latest movement to stop at the boundary */
                src->move(src, -U16_నిడివి(c), UITER_CURRENT);
                ఆపు;
            } లేకపోతే {
                బఫరు.append(c);
            }
        }
    } లేకపోతే {
        అయితే((c=uiter_previous32(src))>=0) {
            /* always write this character to the front of the buffer */
            బఫరు.insert(0, c);
            /* stop if this just-copied character is a boundary */
            ఒకవేళ(n2->hasBoundaryBefore(c)) {
                ఆపు;
            }
        }
    }

    UnicodeString destString(dest, 0, dest_సామర్థ్యం);
    ఒకవేళ(బఫరు.length()>0 && doNormalize) {
        n2->normalize(బఫరు, destString, *p_Error_సంకేతం).extract(dest, dest_సామర్థ్యం, *p_Error_సంకేతం);
        ఒకవేళ(p_Needed_గమ్యం_Normalize!=శూన్యసూచిక && U_SUCCESS(*p_Error_సంకేతం)) {
            *p_Needed_గమ్యం_Normalize= destString!=బఫరు;
        }
        తిరిగి destString.length();
    } లేకపోతే {
        /* just copy the source characters */
        తిరిగి బఫరు.extract(dest, dest_సామర్థ్యం, *p_Error_సంకేతం);
    }
}

స్థిర int32_t
unorm_iterate(UCharIterator *src, UBool forward,
              char16_t *dest, int32_t dest_సామర్థ్యం,
              UNormalization_బహుళకం బహుళకం, int32_t options,
              UBool doNormalize, UBool *p_Needed_గమ్యం_Normalize,
              UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    ఒకవేళ(options&UNORM_UNICODE_3_2) {
        స్థిరరాశి Unicode_సమితి *uni32 = uniset_getUnicode32Instance(*p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        తిరిగి _iterate(src, forward, dest, dest_సామర్థ్యం,
            &fn2, doNormalize, p_Needed_గమ్యం_Normalize, p_Error_సంకేతం);
    }
    తిరిగి _iterate(src, forward, dest, dest_సామర్థ్యం,
            n2, doNormalize, p_Needed_గమ్యం_Normalize, p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
unorm_మునుపటి(UCharIterator *src,
               char16_t *dest, int32_t dest_సామర్థ్యం,
               UNormalization_బహుళకం బహుళకం, int32_t options,
               UBool doNormalize, UBool *p_Needed_గమ్యం_Normalize,
               UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి unorm_iterate(src, అబద్ధం,
                         dest, dest_సామర్థ్యం,
                         బహుళకం, options,
                         doNormalize, p_Needed_గమ్యం_Normalize,
                         p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
unorm_తదుపరి(UCharIterator *src,
           char16_t *dest, int32_t dest_సామర్థ్యం,
           UNormalization_బహుళకం బహుళకం, int32_t options,
           UBool doNormalize, UBool *p_Needed_గమ్యం_Normalize,
           UError_సంకేతం *p_Error_సంకేతం) {
    తిరిగి unorm_iterate(src, నిజం,
                         dest, dest_సామర్థ్యం,
                         బహుళకం, options,
                         doNormalize, p_Needed_గమ్యం_Normalize,
                         p_Error_సంకేతం);
}

/* Concatenation of normalized strings -------------------------------------- */

స్థిర int32_t
_concatenate(స్థిరరాశి char16_t *ఎడమ, int32_t ఎడమ_నిడివి,
                  స్థిరరాశి char16_t *కుడి, int32_t కుడి_నిడివి,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  స్థిరరాశి Normalizer2 *n2,
                  UError_సంకేతం *p_Error_సంకేతం) {
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి 0;
    }
    ఒకవేళ(dest_సామర్థ్యం<0 || (dest==శూన్యసూచిక && dest_సామర్థ్యం>0) ||
        ఎడమ==శూన్యసూచిక || ఎడమ_నిడివి<-1 || కుడి==శూన్యసూచిక || కుడి_నిడివి<-1) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* check for overlapping right and destination */
    ఒకవేళ( dest!=శూన్యసూచిక &&
        ((కుడి>=dest && కుడి<(dest+dest_సామర్థ్యం)) ||
         (కుడి_నిడివి>0 && dest>=కుడి && dest<(కుడి+కుడి_నిడివి)))
    ) {
        *p_Error_సంకేతం=U_ILLEGAL_వాదం_ERROR;
        తిరిగి 0;
    }

    /* allow left==dest */
    UnicodeString destString;
    ఒకవేళ(ఎడమ==dest) {
        destString.సమితి_గమ్యం(dest, ఎడమ_నిడివి, dest_సామర్థ్యం);
    } లేకపోతే {
        destString.సమితి_గమ్యం(dest, 0, dest_సామర్థ్యం);
        destString.append(ఎడమ, ఎడమ_నిడివి);
    }
    తిరిగి n2->append(destString, UnicodeString(కుడి_నిడివి<0, కుడి, కుడి_నిడివి), *p_Error_సంకేతం).
           extract(dest, dest_సామర్థ్యం, *p_Error_సంకేతం);
}

U_CAPI int32_t U_EXPORT2
unorm_concatenate(స్థిరరాశి char16_t *ఎడమ, int32_t ఎడమ_నిడివి,
                  స్థిరరాశి char16_t *కుడి, int32_t కుడి_నిడివి,
                  char16_t *dest, int32_t dest_సామర్థ్యం,
                  UNormalization_బహుళకం బహుళకం, int32_t options,
                  UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి Normalizer2 *n2=Normalizer2Factory::getInstance(బహుళకం, *p_Error_సంకేతం);
    ఒకవేళ(options&UNORM_UNICODE_3_2) {
        స్థిరరాశి Unicode_సమితి *uni32 = uniset_getUnicode32Instance(*p_Error_సంకేతం);
        ఒకవేళ(U_FAILURE(*p_Error_సంకేతం)) {
            తిరిగి 0;
        }
        FilteredNormalizer2 fn2(*n2, *uni32);
        తిరిగి _concatenate(ఎడమ, ఎడమ_నిడివి, కుడి, కుడి_నిడివి,
            dest, dest_సామర్థ్యం, &fn2, p_Error_సంకేతం);
    }
    తిరిగి _concatenate(ఎడమ, ఎడమ_నిడివి, కుడి, కుడి_నిడివి,
        dest, dest_సామర్థ్యం, n2, p_Error_సంకేతం);
}

#అయితే_ముగింపు /* #if !UCONFIG_NO_NORMALIZATION */
