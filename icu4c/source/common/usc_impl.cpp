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

#ضمن "unicode/uscript.h"
#ضمن "usc_impl.h"
#ضمن "cmemory.h"

#تعریف_کړه PAREN_ډېری_ژوروالی 32

#تعریف_کړه MOD(sp) ((sp) % PAREN_ډېری_ژوروالی)
#تعریف_کړه حد_INC(sp) (((sp) < PAREN_ډېری_ژوروالی)? (sp) + 1 : PAREN_ډېری_ژوروالی)
#تعریف_کړه INC(sp,count) (MOD((sp) + (count)))
#تعریف_کړه INC1(sp) (INC(sp, 1))
#تعریف_کړه DEC(sp,count) (MOD((sp) + PAREN_ډېری_ژوروالی - (count)))
#تعریف_کړه DEC1(sp) (DEC(sp, 1))
#تعریف_کړه ډېری_IS_تشه(script_پرلپسې) ((script_پرلپسې)->ټېل_شمېر <= 0)
#تعریف_کړه ډېری_IS_NOT_تشه(script_پرلپسې) (! ډېری_IS_تشه(script_پرلپسې))
#تعریف_کړه سر_3(script_پرلپسې) ((script_پرلپسې)->paren_ډېری[(script_پرلپسې)->parenSP])
#تعریف_کړه SYNC_FIXUP(script_پرلپسې) ((script_پرلپسې)->fixup_شمېر = 0)

بنية Paren_ډېری_ثبت
{
    int32_t pair_شاخص;
    UScript_کوډ script_کوډ;
};

بنية UScript_پرلپسې
{
    int32_t متن_اوږدتیا;
    ثابت char16_t *متن_Array;

    int32_t script_پیل;
    int32_t script_حد;
    UScript_کوډ script_کوډ;

    بنية Paren_ډېری_ثبت paren_ډېری[PAREN_ډېری_ژوروالی];
    int32_t parenSP;
    int32_t ټېل_شمېر;
    int32_t fixup_شمېر;
};

ساكن int8_t لوړ_بټ(int32_t value);

ساكن ثابت UChar32 pairedChars[] = {
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

ساكن فراغ push(UScript_پرلپسې *script_پرلپسې, int32_t pair_شاخص, UScript_کوډ script_کوډ)
{
    script_پرلپسې->ټېل_شمېر  = حد_INC(script_پرلپسې->ټېل_شمېر);
    script_پرلپسې->fixup_شمېر = حد_INC(script_پرلپسې->fixup_شمېر);
    
    script_پرلپسې->parenSP = INC1(script_پرلپسې->parenSP);
    script_پرلپسې->paren_ډېری[script_پرلپسې->parenSP].pair_شاخص  = pair_شاخص;
    script_پرلپسې->paren_ډېری[script_پرلپسې->parenSP].script_کوډ = script_کوډ;
}

ساكن فراغ pop(UScript_پرلپسې *script_پرلپسې)
{
    إذا (ډېری_IS_تشه(script_پرلپسې)) {
        أرجع;
    }
    
    إذا (script_پرلپسې->fixup_شمېر > 0) {
        script_پرلپسې->fixup_شمېر -= 1;
    }
    
    script_پرلپسې->ټېل_شمېر -= 1;
    script_پرلپسې->parenSP = DEC1(script_پرلپسې->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    إذا (ډېری_IS_تشه(script_پرلپسې)) {
        script_پرلپسې->parenSP = -1;
    }
}

ساكن فراغ fixup(UScript_پرلپسې *script_پرلپسې, UScript_کوډ script_کوډ)
{
    int32_t fixupSP = DEC(script_پرلپسې->parenSP, script_پرلپسې->fixup_شمېر);
    
    طالما (script_پرلپسې->fixup_شمېر-- > 0) {
        fixupSP = INC1(fixupSP);
        script_پرلپسې->paren_ډېری[fixupSP].script_کوډ = script_کوډ;
    }
}

ساكن int8_t
لوړ_بټ(int32_t value)
{
    int8_t bit = 0;

    إذا (value <= 0) {
        أرجع -32;
    }

    إذا (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    إذا (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    إذا (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    إذا (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    إذا (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    أرجع bit;
}

ساكن int32_t
get_Pair_شاخص(UChar32 ch)
{
    int32_t paired_Char_شمېر = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << لوړ_بټ(paired_Char_شمېر);
    int32_t pairedCharExtra = paired_Char_شمېر - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_شاخص = 0;

    إذا (ch >= pairedChars[pairedCharExtra]) {
        pair_شاخص = pairedCharExtra;
    }

    طالما (probe > (1 << 0)) {
        probe >>= 1;

        إذا (ch >= pairedChars[pair_شاخص + probe]) {
            pair_شاخص += probe;
        }
    }

    إذا (pairedChars[pair_شاخص] != ch) {
        pair_شاخص = -1;
    }

    أرجع pair_شاخص;
}

ساكن UBool
sameScript(UScript_کوډ scriptOne, UScript_کوډ scriptTwo)
{
    أرجع scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_پرلپسې * U_EXPORT2
uscript_open_پرلپسې(ثابت char16_t *src, int32_t length, UError_کوډ *p_Error_کوډ)
{
    UScript_پرلپسې *پایله = تش_اشاره;

    إذا (p_Error_کوډ == تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع تش_اشاره;
    }

    پایله = (UScript_پرلپسې *)uprv_malloc(حجم (UScript_پرلپسې));

    إذا (پایله == تش_اشاره) {
        *p_Error_کوډ = U_حافظه_ویش_ERROR;
        أرجع تش_اشاره;
    }

    uscript_سېټ_پرلپسې_متن(پایله, src, length, p_Error_کوډ);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    إذا (U_FAILURE(*p_Error_کوډ)) {
        uprv_free(پایله);
        پایله = تش_اشاره;
    }

    أرجع پایله;
}

U_CAPI فراغ U_EXPORT2
uscript_close_پرلپسې(UScript_پرلپسې *script_پرلپسې)
{
    إذا (script_پرلپسې != تش_اشاره) {
        uprv_free(script_پرلپسې);
    }
}

U_CAPI فراغ U_EXPORT2
uscript_reset_پرلپسې(UScript_پرلپسې *script_پرلپسې)
{
    إذا (script_پرلپسې != تش_اشاره) {
        script_پرلپسې->script_پیل = 0;
        script_پرلپسې->script_حد = 0;
        script_پرلپسې->script_کوډ  = USCRIPT_INVALID_کوډ;
        script_پرلپسې->parenSP     = -1;
        script_پرلپسې->ټېل_شمېر   =  0;
        script_پرلپسې->fixup_شمېر  =  0;
    }
}

U_CAPI فراغ U_EXPORT2
uscript_سېټ_پرلپسې_متن(UScript_پرلپسې *script_پرلپسې, ثابت char16_t *src, int32_t length, UError_کوډ *p_Error_کوډ)
{
    إذا (p_Error_کوډ == تش_اشاره || U_FAILURE(*p_Error_کوډ)) {
        أرجع;
    }

    إذا (script_پرلپسې == تش_اشاره || length < 0 || ((src == تش_اشاره) != (length == 0))) {
        *p_Error_کوډ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    script_پرلپسې->متن_Array  = src;
    script_پرلپسې->متن_اوږدتیا = length;

    uscript_reset_پرلپسې(script_پرلپسې);
}

U_CAPI UBool U_EXPORT2
uscript_بل_پرلپسې(UScript_پرلپسې *script_پرلپسې, int32_t *p_پرلپسې_پیل, int32_t *p_پرلپسې_حد, UScript_کوډ *p_پرلپسې_Script)
{
    UError_کوډ error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    إذا (script_پرلپسې == تش_اشاره || script_پرلپسې->script_حد >= script_پرلپسې->متن_اوږدتیا) {
        أرجع خطأ;
    }
    
    SYNC_FIXUP(script_پرلپسې);
    script_پرلپسې->script_کوډ = USCRIPT_COMMON;

    لكل (script_پرلپسې->script_پیل = script_پرلپسې->script_حد; script_پرلپسې->script_حد < script_پرلپسې->متن_اوږدتیا; script_پرلپسې->script_حد += 1) {
        char16_t لوړ = script_پرلپسې->متن_Array[script_پرلپسې->script_حد];
        UChar32  ch   = لوړ;
        UScript_کوډ sc;
        int32_t pair_شاخص;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        إذا (لوړ >= 0xD800 && لوړ <= 0xDBFF && script_پرلپسې->script_حد < script_پرلپسې->متن_اوږدتیا - 1) {
            char16_t ټیټ = script_پرلپسې->متن_Array[script_پرلپسې->script_حد + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            إذا (ټیټ >= 0xDC00 && ټیټ <= 0xDFFF) {
                ch = (لوړ - 0xD800) * 0x0400 + ټیټ - 0xDC00 + 0x10000;
                script_پرلپسې->script_حد += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_شاخص = get_Pair_شاخص(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        إذا (pair_شاخص >= 0) {
            إذا ((pair_شاخص & 1) == 0) {
                push(script_پرلپسې, pair_شاخص, script_پرلپسې->script_کوډ);
            } وإلا {
                int32_t pi = pair_شاخص & ~1;

                طالما (ډېری_IS_NOT_تشه(script_پرلپسې) && سر_3(script_پرلپسې).pair_شاخص != pi) {
                    pop(script_پرلپسې);
                }

                إذا (ډېری_IS_NOT_تشه(script_پرلپسې)) {
                    sc = سر_3(script_پرلپسې).script_کوډ;
                }
            }
        }

        إذا (sameScript(script_پرلپسې->script_کوډ, sc)) {
            إذا (script_پرلپسې->script_کوډ <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_پرلپسې->script_کوډ = sc;

                fixup(script_پرلپسې, script_پرلپسې->script_کوډ);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            إذا (pair_شاخص >= 0 && (pair_شاخص & 1) != 0) {
                pop(script_پرلپسې);
            }
        } وإلا {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            إذا (ch >= 0x10000) {
                script_پرلپسې->script_حد -= 1;
            }

            اكسر;
        }
    }


    إذا (p_پرلپسې_پیل != تش_اشاره) {
        *p_پرلپسې_پیل = script_پرلپسې->script_پیل;
    }

    إذا (p_پرلپسې_حد != تش_اشاره) {
        *p_پرلپسې_حد = script_پرلپسې->script_حد;
    }

    إذا (p_پرلپسې_Script != تش_اشاره) {
        *p_پرلپسې_Script = script_پرلپسې->script_کوډ;
    }

    أرجع ریښتیا;
}
