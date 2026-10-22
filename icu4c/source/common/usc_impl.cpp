// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
*   Copyright (C) 1999-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*
* File USC_IMPL.C
*
* Modification History:
*
*   Date        Name        Description
*   07/08/2002  Eric Mader  Creation.
******************************************************************************
*/

#అన్తర్భావ "unicode/uscript.h"
#అన్తర్భావ "usc_impl.h"
#అన్తర్భావ "cmemory.h"

#నిర్వచించు PAREN_దొంతర_లోతు 32

#నిర్వచించు MOD(sp) ((sp) % PAREN_దొంతర_లోతు)
#నిర్వచించు హద్దు_INC(sp) (((sp) < PAREN_దొంతర_లోతు)? (sp) + 1 : PAREN_దొంతర_లోతు)
#నిర్వచించు INC(sp,count) (MOD((sp) + (count)))
#నిర్వచించు INC1(sp) (INC(sp, 1))
#నిర్వచించు DEC(sp,count) (MOD((sp) + PAREN_దొంతర_లోతు - (count)))
#నిర్వచించు DEC1(sp) (DEC(sp, 1))
#నిర్వచించు దొంతర_IS_ఖాళీలు(script_నిరంతరం) ((script_నిరంతరం)->నెట్టు_లెక్క <= 0)
#నిర్వచించు దొంతర_IS_NOT_ఖాళీలు(script_నిరంతరం) (! దొంతర_IS_ఖాళీలు(script_నిరంతరం))
#నిర్వచించు శిఖరం_2(script_నిరంతరం) ((script_నిరంతరం)->paren_దొంతర[(script_నిరంతరం)->parenSP])
#నిర్వచించు SYNC_FIXUP(script_నిరంతరం) ((script_నిరంతరం)->fixup_లెక్క = 0)

నిర్మాణం Paren_దొంతర_నమోదు
{
    int32_t pair_సూచీ;
    UScript_సంకేతం script_సంకేతం;
};

నిర్మాణం UScript_నిరంతరం
{
    int32_t పాఠ్యం_నిడివి;
    స్థిరరాశి char16_t *పాఠ్యం_Array;

    int32_t script_ప్రారంభం;
    int32_t script_హద్దు;
    UScript_సంకేతం script_సంకేతం;

    నిర్మాణం Paren_దొంతర_నమోదు paren_దొంతర[PAREN_దొంతర_లోతు];
    int32_t parenSP;
    int32_t నెట్టు_లెక్క;
    int32_t fixup_లెక్క;
};

స్థిర int8_t ఎగువ_బిట్టు(int32_t value);

స్థిర స్థిరరాశి UChar32 pairedChars[] = {
    0x0028, 0x0029, /* ascii paired punctuation */
    0x003c, 0x003e,
    0x005b, 0x005d,
    0x007b, 0x007d,
    0x00ab, 0x00bb, /* guillemets */
    0x2018, 0x2019, /* general punctuation */
    0x201c, 0x201d,
    0x2039, 0x203a,
    0x3008, 0x3009, /* chinese paired punctuation */
    0x300a, 0x300b,
    0x300c, 0x300d,
    0x300e, 0x300f,
    0x3010, 0x3011,
    0x3014, 0x3015,
    0x3016, 0x3017,
    0x3018, 0x3019,
    0x301a, 0x301b
};

స్థిర శూన్య push(UScript_నిరంతరం *script_నిరంతరం, int32_t pair_సూచీ, UScript_సంకేతం script_సంకేతం)
{
    script_నిరంతరం->నెట్టు_లెక్క  = హద్దు_INC(script_నిరంతరం->నెట్టు_లెక్క);
    script_నిరంతరం->fixup_లెక్క = హద్దు_INC(script_నిరంతరం->fixup_లెక్క);
    
    script_నిరంతరం->parenSP = INC1(script_నిరంతరం->parenSP);
    script_నిరంతరం->paren_దొంతర[script_నిరంతరం->parenSP].pair_సూచీ  = pair_సూచీ;
    script_నిరంతరం->paren_దొంతర[script_నిరంతరం->parenSP].script_సంకేతం = script_సంకేతం;
}

స్థిర శూన్య pop(UScript_నిరంతరం *script_నిరంతరం)
{
    ఒకవేళ (దొంతర_IS_ఖాళీలు(script_నిరంతరం)) {
        తిరిగి;
    }
    
    ఒకవేళ (script_నిరంతరం->fixup_లెక్క > 0) {
        script_నిరంతరం->fixup_లెక్క -= 1;
    }
    
    script_నిరంతరం->నెట్టు_లెక్క -= 1;
    script_నిరంతరం->parenSP = DEC1(script_నిరంతరం->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    ఒకవేళ (దొంతర_IS_ఖాళీలు(script_నిరంతరం)) {
        script_నిరంతరం->parenSP = -1;
    }
}

స్థిర శూన్య fixup(UScript_నిరంతరం *script_నిరంతరం, UScript_సంకేతం script_సంకేతం)
{
    int32_t fixupSP = DEC(script_నిరంతరం->parenSP, script_నిరంతరం->fixup_లెక్క);
    
    అయితే (script_నిరంతరం->fixup_లెక్క-- > 0) {
        fixupSP = INC1(fixupSP);
        script_నిరంతరం->paren_దొంతర[fixupSP].script_సంకేతం = script_సంకేతం;
    }
}

స్థిర int8_t
ఎగువ_బిట్టు(int32_t value)
{
    int8_t bit = 0;

    ఒకవేళ (value <= 0) {
        తిరిగి -32;
    }

    ఒకవేళ (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    ఒకవేళ (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    ఒకవేళ (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    ఒకవేళ (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    ఒకవేళ (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    తిరిగి bit;
}

స్థిర int32_t
get_Pair_సూచీ(UChar32 ch)
{
    int32_t paired_Char_లెక్క = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ఎగువ_బిట్టు(paired_Char_లెక్క);
    int32_t pairedCharExtra = paired_Char_లెక్క - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_సూచీ = 0;

    ఒకవేళ (ch >= pairedChars[pairedCharExtra]) {
        pair_సూచీ = pairedCharExtra;
    }

    అయితే (probe > (1 << 0)) {
        probe >>= 1;

        ఒకవేళ (ch >= pairedChars[pair_సూచీ + probe]) {
            pair_సూచీ += probe;
        }
    }

    ఒకవేళ (pairedChars[pair_సూచీ] != ch) {
        pair_సూచీ = -1;
    }

    తిరిగి pair_సూచీ;
}

స్థిర UBool
sameScript(UScript_సంకేతం scriptOne, UScript_సంకేతం scriptTwo)
{
    తిరిగి scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_నిరంతరం * U_EXPORT2
uscript_open_నిరంతరం(స్థిరరాశి char16_t *src, int32_t length, UError_సంకేతం *p_Error_సంకేతం)
{
    UScript_నిరంతరం *ఫలితం = శూన్యసూచిక;

    ఒకవేళ (p_Error_సంకేతం == శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి శూన్యసూచిక;
    }

    ఫలితం = (UScript_నిరంతరం *)uprv_malloc(పరిమాణం (UScript_నిరంతరం));

    ఒకవేళ (ఫలితం == శూన్యసూచిక) {
        *p_Error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి శూన్యసూచిక;
    }

    uscript_సమితి_నిరంతరం_పాఠ్యం(ఫలితం, src, length, p_Error_సంకేతం);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    ఒకవేళ (U_FAILURE(*p_Error_సంకేతం)) {
        uprv_free(ఫలితం);
        ఫలితం = శూన్యసూచిక;
    }

    తిరిగి ఫలితం;
}

U_CAPI శూన్య U_EXPORT2
uscript_close_నిరంతరం(UScript_నిరంతరం *script_నిరంతరం)
{
    ఒకవేళ (script_నిరంతరం != శూన్యసూచిక) {
        uprv_free(script_నిరంతరం);
    }
}

U_CAPI శూన్య U_EXPORT2
uscript_reset_నిరంతరం(UScript_నిరంతరం *script_నిరంతరం)
{
    ఒకవేళ (script_నిరంతరం != శూన్యసూచిక) {
        script_నిరంతరం->script_ప్రారంభం = 0;
        script_నిరంతరం->script_హద్దు = 0;
        script_నిరంతరం->script_సంకేతం  = USCRIPT_INVALID_సంకేతం;
        script_నిరంతరం->parenSP     = -1;
        script_నిరంతరం->నెట్టు_లెక్క   =  0;
        script_నిరంతరం->fixup_లెక్క  =  0;
    }
}

U_CAPI శూన్య U_EXPORT2
uscript_సమితి_నిరంతరం_పాఠ్యం(UScript_నిరంతరం *script_నిరంతరం, స్థిరరాశి char16_t *src, int32_t length, UError_సంకేతం *p_Error_సంకేతం)
{
    ఒకవేళ (p_Error_సంకేతం == శూన్యసూచిక || U_FAILURE(*p_Error_సంకేతం)) {
        తిరిగి;
    }

    ఒకవేళ (script_నిరంతరం == శూన్యసూచిక || length < 0 || ((src == శూన్యసూచిక) != (length == 0))) {
        *p_Error_సంకేతం = U_ILLEGAL_వాదం_ERROR;
        తిరిగి;
    }

    script_నిరంతరం->పాఠ్యం_Array  = src;
    script_నిరంతరం->పాఠ్యం_నిడివి = length;

    uscript_reset_నిరంతరం(script_నిరంతరం);
}

U_CAPI UBool U_EXPORT2
uscript_తదుపరి_నిరంతరం(UScript_నిరంతరం *script_నిరంతరం, int32_t *p_నిరంతరం_ప్రారంభం, int32_t *p_నిరంతరం_హద్దు, UScript_సంకేతం *p_నిరంతరం_Script)
{
    UError_సంకేతం error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    ఒకవేళ (script_నిరంతరం == శూన్యసూచిక || script_నిరంతరం->script_హద్దు >= script_నిరంతరం->పాఠ్యం_నిడివి) {
        తిరిగి అబద్ధం;
    }
    
    SYNC_FIXUP(script_నిరంతరం);
    script_నిరంతరం->script_సంకేతం = USCRIPT_COMMON;

    కొరకు (script_నిరంతరం->script_ప్రారంభం = script_నిరంతరం->script_హద్దు; script_నిరంతరం->script_హద్దు < script_నిరంతరం->పాఠ్యం_నిడివి; script_నిరంతరం->script_హద్దు += 1) {
        char16_t ఎగువ = script_నిరంతరం->పాఠ్యం_Array[script_నిరంతరం->script_హద్దు];
        UChar32  ch   = ఎగువ;
        UScript_సంకేతం sc;
        int32_t pair_సూచీ;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        ఒకవేళ (ఎగువ >= 0xD800 && ఎగువ <= 0xDBFF && script_నిరంతరం->script_హద్దు < script_నిరంతరం->పాఠ్యం_నిడివి - 1) {
            char16_t దిగువ = script_నిరంతరం->పాఠ్యం_Array[script_నిరంతరం->script_హద్దు + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            ఒకవేళ (దిగువ >= 0xDC00 && దిగువ <= 0xDFFF) {
                ch = (ఎగువ - 0xD800) * 0x0400 + దిగువ - 0xDC00 + 0x10000;
                script_నిరంతరం->script_హద్దు += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_సూచీ = get_Pair_సూచీ(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        ఒకవేళ (pair_సూచీ >= 0) {
            ఒకవేళ ((pair_సూచీ & 1) == 0) {
                push(script_నిరంతరం, pair_సూచీ, script_నిరంతరం->script_సంకేతం);
            } లేకపోతే {
                int32_t pi = pair_సూచీ & ~1;

                అయితే (దొంతర_IS_NOT_ఖాళీలు(script_నిరంతరం) && శిఖరం_2(script_నిరంతరం).pair_సూచీ != pi) {
                    pop(script_నిరంతరం);
                }

                ఒకవేళ (దొంతర_IS_NOT_ఖాళీలు(script_నిరంతరం)) {
                    sc = శిఖరం_2(script_నిరంతరం).script_సంకేతం;
                }
            }
        }

        ఒకవేళ (sameScript(script_నిరంతరం->script_సంకేతం, sc)) {
            ఒకవేళ (script_నిరంతరం->script_సంకేతం <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_నిరంతరం->script_సంకేతం = sc;

                fixup(script_నిరంతరం, script_నిరంతరం->script_సంకేతం);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            ఒకవేళ (pair_సూచీ >= 0 && (pair_సూచీ & 1) != 0) {
                pop(script_నిరంతరం);
            }
        } లేకపోతే {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            ఒకవేళ (ch >= 0x10000) {
                script_నిరంతరం->script_హద్దు -= 1;
            }

            ఆపు;
        }
    }


    ఒకవేళ (p_నిరంతరం_ప్రారంభం != శూన్యసూచిక) {
        *p_నిరంతరం_ప్రారంభం = script_నిరంతరం->script_ప్రారంభం;
    }

    ఒకవేళ (p_నిరంతరం_హద్దు != శూన్యసూచిక) {
        *p_నిరంతరం_హద్దు = script_నిరంతరం->script_హద్దు;
    }

    ఒకవేళ (p_నిరంతరం_Script != శూన్యసూచిక) {
        *p_నిరంతరం_Script = script_నిరంతరం->script_సంకేతం;
    }

    తిరిగి నిజం;
}
