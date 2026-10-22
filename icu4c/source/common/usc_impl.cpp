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

#وصف_ڪريو PAREN_ڍير_اونهائي 32

#وصف_ڪريو MOD(sp) ((sp) % PAREN_ڍير_اونهائي)
#وصف_ڪريو حد_بندي_INC(sp) (((sp) < PAREN_ڍير_اونهائي)? (sp) + 1 : PAREN_ڍير_اونهائي)
#وصف_ڪريو INC(sp,count) (MOD((sp) + (count)))
#وصف_ڪريو INC1(sp) (INC(sp, 1))
#وصف_ڪريو DEC(sp,count) (MOD((sp) + PAREN_ڍير_اونهائي - (count)))
#وصف_ڪريو DEC1(sp) (DEC(sp, 1))
#وصف_ڪريو ڍير_IS_خالي_جايون(script_لاڳيتو) ((script_لاڳيتو)->ڌڪو_ڳڻپ <= 0)
#وصف_ڪريو ڍير_IS_NOT_خالي_جايون(script_لاڳيتو) (! ڍير_IS_خالي_جايون(script_لاڳيتو))
#وصف_ڪريو چوٽي_2(script_لاڳيتو) ((script_لاڳيتو)->paren_ڍير[(script_لاڳيتو)->parenSP])
#وصف_ڪريو SYNC_FIXUP(script_لاڳيتو) ((script_لاڳيتو)->fixup_ڳڻپ = 0)

بنية Paren_ڍير_داخلا
{
    int32_t pair_اشاريو;
    UScript_ڪوڊ script_ڪوڊ;
};

بنية UScript_لاڳيتو
{
    int32_t متن_ڊيگهائي;
    ثابت char16_t *متن_Array;

    int32_t script_شروعات;
    int32_t script_حد_بندي;
    UScript_ڪوڊ script_ڪوڊ;

    بنية Paren_ڍير_داخلا paren_ڍير[PAREN_ڍير_اونهائي];
    int32_t parenSP;
    int32_t ڌڪو_ڳڻپ;
    int32_t fixup_ڳڻپ;
};

ساكن int8_t مٿاهون_ٻٽو_انگ(int32_t value);

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

ساكن فراغ push(UScript_لاڳيتو *script_لاڳيتو, int32_t pair_اشاريو, UScript_ڪوڊ script_ڪوڊ)
{
    script_لاڳيتو->ڌڪو_ڳڻپ  = حد_بندي_INC(script_لاڳيتو->ڌڪو_ڳڻپ);
    script_لاڳيتو->fixup_ڳڻپ = حد_بندي_INC(script_لاڳيتو->fixup_ڳڻپ);
    
    script_لاڳيتو->parenSP = INC1(script_لاڳيتو->parenSP);
    script_لاڳيتو->paren_ڍير[script_لاڳيتو->parenSP].pair_اشاريو  = pair_اشاريو;
    script_لاڳيتو->paren_ڍير[script_لاڳيتو->parenSP].script_ڪوڊ = script_ڪوڊ;
}

ساكن فراغ pop(UScript_لاڳيتو *script_لاڳيتو)
{
    إذا (ڍير_IS_خالي_جايون(script_لاڳيتو)) {
        أرجع;
    }
    
    إذا (script_لاڳيتو->fixup_ڳڻپ > 0) {
        script_لاڳيتو->fixup_ڳڻپ -= 1;
    }
    
    script_لاڳيتو->ڌڪو_ڳڻپ -= 1;
    script_لاڳيتو->parenSP = DEC1(script_لاڳيتو->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    إذا (ڍير_IS_خالي_جايون(script_لاڳيتو)) {
        script_لاڳيتو->parenSP = -1;
    }
}

ساكن فراغ fixup(UScript_لاڳيتو *script_لاڳيتو, UScript_ڪوڊ script_ڪوڊ)
{
    int32_t fixupSP = DEC(script_لاڳيتو->parenSP, script_لاڳيتو->fixup_ڳڻپ);
    
    طالما (script_لاڳيتو->fixup_ڳڻپ-- > 0) {
        fixupSP = INC1(fixupSP);
        script_لاڳيتو->paren_ڍير[fixupSP].script_ڪوڊ = script_ڪوڊ;
    }
}

ساكن int8_t
مٿاهون_ٻٽو_انگ(int32_t value)
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
get_Pair_اشاريو(UChar32 ch)
{
    int32_t paired_Char_ڳڻپ = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << مٿاهون_ٻٽو_انگ(paired_Char_ڳڻپ);
    int32_t pairedCharExtra = paired_Char_ڳڻپ - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_اشاريو = 0;

    إذا (ch >= pairedChars[pairedCharExtra]) {
        pair_اشاريو = pairedCharExtra;
    }

    طالما (probe > (1 << 0)) {
        probe >>= 1;

        إذا (ch >= pairedChars[pair_اشاريو + probe]) {
            pair_اشاريو += probe;
        }
    }

    إذا (pairedChars[pair_اشاريو] != ch) {
        pair_اشاريو = -1;
    }

    أرجع pair_اشاريو;
}

ساكن UBool
sameScript(UScript_ڪوڊ scriptOne, UScript_ڪوڊ scriptTwo)
{
    أرجع scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_لاڳيتو * U_EXPORT2
uscript_open_لاڳيتو(ثابت char16_t *src, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ)
{
    UScript_لاڳيتو *نتيجو = خالي_اشارو;

    إذا (p_Error_ڪوڊ == خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع خالي_اشارو;
    }

    نتيجو = (UScript_لاڳيتو *)uprv_malloc(حجم (UScript_لاڳيتو));

    إذا (نتيجو == خالي_اشارو) {
        *p_Error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع خالي_اشارو;
    }

    uscript_سيٽ_لاڳيتو_متن(نتيجو, src, length, p_Error_ڪوڊ);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    إذا (U_FAILURE(*p_Error_ڪوڊ)) {
        uprv_free(نتيجو);
        نتيجو = خالي_اشارو;
    }

    أرجع نتيجو;
}

U_CAPI فراغ U_EXPORT2
uscript_close_لاڳيتو(UScript_لاڳيتو *script_لاڳيتو)
{
    إذا (script_لاڳيتو != خالي_اشارو) {
        uprv_free(script_لاڳيتو);
    }
}

U_CAPI فراغ U_EXPORT2
uscript_reset_لاڳيتو(UScript_لاڳيتو *script_لاڳيتو)
{
    إذا (script_لاڳيتو != خالي_اشارو) {
        script_لاڳيتو->script_شروعات = 0;
        script_لاڳيتو->script_حد_بندي = 0;
        script_لاڳيتو->script_ڪوڊ  = USCRIPT_INVALID_ڪوڊ;
        script_لاڳيتو->parenSP     = -1;
        script_لاڳيتو->ڌڪو_ڳڻپ   =  0;
        script_لاڳيتو->fixup_ڳڻپ  =  0;
    }
}

U_CAPI فراغ U_EXPORT2
uscript_سيٽ_لاڳيتو_متن(UScript_لاڳيتو *script_لاڳيتو, ثابت char16_t *src, int32_t length, UError_ڪوڊ *p_Error_ڪوڊ)
{
    إذا (p_Error_ڪوڊ == خالي_اشارو || U_FAILURE(*p_Error_ڪوڊ)) {
        أرجع;
    }

    إذا (script_لاڳيتو == خالي_اشارو || length < 0 || ((src == خالي_اشارو) != (length == 0))) {
        *p_Error_ڪوڊ = U_ILLEGAL_دليل_ERROR;
        أرجع;
    }

    script_لاڳيتو->متن_Array  = src;
    script_لاڳيتو->متن_ڊيگهائي = length;

    uscript_reset_لاڳيتو(script_لاڳيتو);
}

U_CAPI UBool U_EXPORT2
uscript_ايندڙ_لاڳيتو(UScript_لاڳيتو *script_لاڳيتو, int32_t *p_لاڳيتو_شروعات, int32_t *p_لاڳيتو_حد_بندي, UScript_ڪوڊ *p_لاڳيتو_Script)
{
    UError_ڪوڊ error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    إذا (script_لاڳيتو == خالي_اشارو || script_لاڳيتو->script_حد_بندي >= script_لاڳيتو->متن_ڊيگهائي) {
        أرجع خطأ;
    }
    
    SYNC_FIXUP(script_لاڳيتو);
    script_لاڳيتو->script_ڪوڊ = USCRIPT_COMMON;

    لكل (script_لاڳيتو->script_شروعات = script_لاڳيتو->script_حد_بندي; script_لاڳيتو->script_حد_بندي < script_لاڳيتو->متن_ڊيگهائي; script_لاڳيتو->script_حد_بندي += 1) {
        char16_t مٿاهون = script_لاڳيتو->متن_Array[script_لاڳيتو->script_حد_بندي];
        UChar32  ch   = مٿاهون;
        UScript_ڪوڊ sc;
        int32_t pair_اشاريو;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        إذا (مٿاهون >= 0xD800 && مٿاهون <= 0xDBFF && script_لاڳيتو->script_حد_بندي < script_لاڳيتو->متن_ڊيگهائي - 1) {
            char16_t هيٺيون = script_لاڳيتو->متن_Array[script_لاڳيتو->script_حد_بندي + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            إذا (هيٺيون >= 0xDC00 && هيٺيون <= 0xDFFF) {
                ch = (مٿاهون - 0xD800) * 0x0400 + هيٺيون - 0xDC00 + 0x10000;
                script_لاڳيتو->script_حد_بندي += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_اشاريو = get_Pair_اشاريو(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        إذا (pair_اشاريو >= 0) {
            إذا ((pair_اشاريو & 1) == 0) {
                push(script_لاڳيتو, pair_اشاريو, script_لاڳيتو->script_ڪوڊ);
            } وإلا {
                int32_t pi = pair_اشاريو & ~1;

                طالما (ڍير_IS_NOT_خالي_جايون(script_لاڳيتو) && چوٽي_2(script_لاڳيتو).pair_اشاريو != pi) {
                    pop(script_لاڳيتو);
                }

                إذا (ڍير_IS_NOT_خالي_جايون(script_لاڳيتو)) {
                    sc = چوٽي_2(script_لاڳيتو).script_ڪوڊ;
                }
            }
        }

        إذا (sameScript(script_لاڳيتو->script_ڪوڊ, sc)) {
            إذا (script_لاڳيتو->script_ڪوڊ <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_لاڳيتو->script_ڪوڊ = sc;

                fixup(script_لاڳيتو, script_لاڳيتو->script_ڪوڊ);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            إذا (pair_اشاريو >= 0 && (pair_اشاريو & 1) != 0) {
                pop(script_لاڳيتو);
            }
        } وإلا {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            إذا (ch >= 0x10000) {
                script_لاڳيتو->script_حد_بندي -= 1;
            }

            اكسر;
        }
    }


    إذا (p_لاڳيتو_شروعات != خالي_اشارو) {
        *p_لاڳيتو_شروعات = script_لاڳيتو->script_شروعات;
    }

    إذا (p_لاڳيتو_حد_بندي != خالي_اشارو) {
        *p_لاڳيتو_حد_بندي = script_لاڳيتو->script_حد_بندي;
    }

    إذا (p_لاڳيتو_Script != خالي_اشارو) {
        *p_لاڳيتو_Script = script_لاڳيتو->script_ڪوڊ;
    }

    أرجع سچ;
}
