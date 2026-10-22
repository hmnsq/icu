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

#تعریف_کن PAREN_پشته_عمق 32

#تعریف_کن MOD(sp) ((sp) % PAREN_پشته_عمق)
#تعریف_کن حد_INC(sp) (((sp) < PAREN_پشته_عمق)? (sp) + 1 : PAREN_پشته_عمق)
#تعریف_کن INC(sp,count) (MOD((sp) + (count)))
#تعریف_کن INC1(sp) (INC(sp, 1))
#تعریف_کن DEC(sp,count) (MOD((sp) + PAREN_پشته_عمق - (count)))
#تعریف_کن DEC1(sp) (DEC(sp, 1))
#تعریف_کن پشته_IS_خالی(script_پیاپی) ((script_پیاپی)->افزودن_شمار <= 0)
#تعریف_کن پشته_IS_NOT_خالی(script_پیاپی) (! پشته_IS_خالی(script_پیاپی))
#تعریف_کن بالاترین_2(script_پیاپی) ((script_پیاپی)->paren_پشته[(script_پیاپی)->parenSP])
#تعریف_کن SYNC_FIXUP(script_پیاپی) ((script_پیاپی)->fixup_شمار = 0)

بنية Paren_پشته_مدخل
{
    int32_t pair_شاخص;
    UScript_کد script_کد;
};

بنية UScript_پیاپی
{
    int32_t متن_درازی;
    ثابت char16_t *متن_Array;

    int32_t script_شروع;
    int32_t script_حد;
    UScript_کد script_کد;

    بنية Paren_پشته_مدخل paren_پشته[PAREN_پشته_عمق];
    int32_t parenSP;
    int32_t افزودن_شمار;
    int32_t fixup_شمار;
};

ساكن int8_t بالا_بیت(int32_t value);

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

ساكن فراغ push(UScript_پیاپی *script_پیاپی, int32_t pair_شاخص, UScript_کد script_کد)
{
    script_پیاپی->افزودن_شمار  = حد_INC(script_پیاپی->افزودن_شمار);
    script_پیاپی->fixup_شمار = حد_INC(script_پیاپی->fixup_شمار);
    
    script_پیاپی->parenSP = INC1(script_پیاپی->parenSP);
    script_پیاپی->paren_پشته[script_پیاپی->parenSP].pair_شاخص  = pair_شاخص;
    script_پیاپی->paren_پشته[script_پیاپی->parenSP].script_کد = script_کد;
}

ساكن فراغ pop(UScript_پیاپی *script_پیاپی)
{
    إذا (پشته_IS_خالی(script_پیاپی)) {
        أرجع;
    }
    
    إذا (script_پیاپی->fixup_شمار > 0) {
        script_پیاپی->fixup_شمار -= 1;
    }
    
    script_پیاپی->افزودن_شمار -= 1;
    script_پیاپی->parenSP = DEC1(script_پیاپی->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    إذا (پشته_IS_خالی(script_پیاپی)) {
        script_پیاپی->parenSP = -1;
    }
}

ساكن فراغ fixup(UScript_پیاپی *script_پیاپی, UScript_کد script_کد)
{
    int32_t fixupSP = DEC(script_پیاپی->parenSP, script_پیاپی->fixup_شمار);
    
    طالما (script_پیاپی->fixup_شمار-- > 0) {
        fixupSP = INC1(fixupSP);
        script_پیاپی->paren_پشته[fixupSP].script_کد = script_کد;
    }
}

ساكن int8_t
بالا_بیت(int32_t value)
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
    int32_t paired_Char_شمار = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << بالا_بیت(paired_Char_شمار);
    int32_t pairedCharExtra = paired_Char_شمار - pairedCharPower;

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
sameScript(UScript_کد scriptOne, UScript_کد scriptTwo)
{
    أرجع scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_پیاپی * U_EXPORT2
uscript_open_پیاپی(ثابت char16_t *src, int32_t length, UError_کد *p_Error_کد)
{
    UScript_پیاپی *نتیجه = اشاره_تهی;

    إذا (p_Error_کد == اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع اشاره_تهی;
    }

    نتیجه = (UScript_پیاپی *)uprv_malloc(حجم (UScript_پیاپی));

    إذا (نتیجه == اشاره_تهی) {
        *p_Error_کد = U_حافظه_تخصیص_ERROR;
        أرجع اشاره_تهی;
    }

    uscript_مجموعه_پیاپی_متن(نتیجه, src, length, p_Error_کد);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    إذا (U_FAILURE(*p_Error_کد)) {
        uprv_free(نتیجه);
        نتیجه = اشاره_تهی;
    }

    أرجع نتیجه;
}

U_CAPI فراغ U_EXPORT2
uscript_close_پیاپی(UScript_پیاپی *script_پیاپی)
{
    إذا (script_پیاپی != اشاره_تهی) {
        uprv_free(script_پیاپی);
    }
}

U_CAPI فراغ U_EXPORT2
uscript_reset_پیاپی(UScript_پیاپی *script_پیاپی)
{
    إذا (script_پیاپی != اشاره_تهی) {
        script_پیاپی->script_شروع = 0;
        script_پیاپی->script_حد = 0;
        script_پیاپی->script_کد  = USCRIPT_INVALID_کد;
        script_پیاپی->parenSP     = -1;
        script_پیاپی->افزودن_شمار   =  0;
        script_پیاپی->fixup_شمار  =  0;
    }
}

U_CAPI فراغ U_EXPORT2
uscript_مجموعه_پیاپی_متن(UScript_پیاپی *script_پیاپی, ثابت char16_t *src, int32_t length, UError_کد *p_Error_کد)
{
    إذا (p_Error_کد == اشاره_تهی || U_FAILURE(*p_Error_کد)) {
        أرجع;
    }

    إذا (script_پیاپی == اشاره_تهی || length < 0 || ((src == اشاره_تهی) != (length == 0))) {
        *p_Error_کد = U_ILLEGAL_آرگومان_ERROR;
        أرجع;
    }

    script_پیاپی->متن_Array  = src;
    script_پیاپی->متن_درازی = length;

    uscript_reset_پیاپی(script_پیاپی);
}

U_CAPI UBool U_EXPORT2
uscript_بعدی_پیاپی(UScript_پیاپی *script_پیاپی, int32_t *p_پیاپی_شروع, int32_t *p_پیاپی_حد, UScript_کد *p_پیاپی_Script)
{
    UError_کد error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    إذا (script_پیاپی == اشاره_تهی || script_پیاپی->script_حد >= script_پیاپی->متن_درازی) {
        أرجع خطأ;
    }
    
    SYNC_FIXUP(script_پیاپی);
    script_پیاپی->script_کد = USCRIPT_COMMON;

    لكل (script_پیاپی->script_شروع = script_پیاپی->script_حد; script_پیاپی->script_حد < script_پیاپی->متن_درازی; script_پیاپی->script_حد += 1) {
        char16_t بالا = script_پیاپی->متن_Array[script_پیاپی->script_حد];
        UChar32  ch   = بالا;
        UScript_کد sc;
        int32_t pair_شاخص;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        إذا (بالا >= 0xD800 && بالا <= 0xDBFF && script_پیاپی->script_حد < script_پیاپی->متن_درازی - 1) {
            char16_t کم = script_پیاپی->متن_Array[script_پیاپی->script_حد + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            إذا (کم >= 0xDC00 && کم <= 0xDFFF) {
                ch = (بالا - 0xD800) * 0x0400 + کم - 0xDC00 + 0x10000;
                script_پیاپی->script_حد += 1;
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
                push(script_پیاپی, pair_شاخص, script_پیاپی->script_کد);
            } وإلا {
                int32_t pi = pair_شاخص & ~1;

                طالما (پشته_IS_NOT_خالی(script_پیاپی) && بالاترین_2(script_پیاپی).pair_شاخص != pi) {
                    pop(script_پیاپی);
                }

                إذا (پشته_IS_NOT_خالی(script_پیاپی)) {
                    sc = بالاترین_2(script_پیاپی).script_کد;
                }
            }
        }

        إذا (sameScript(script_پیاپی->script_کد, sc)) {
            إذا (script_پیاپی->script_کد <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_پیاپی->script_کد = sc;

                fixup(script_پیاپی, script_پیاپی->script_کد);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            إذا (pair_شاخص >= 0 && (pair_شاخص & 1) != 0) {
                pop(script_پیاپی);
            }
        } وإلا {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            إذا (ch >= 0x10000) {
                script_پیاپی->script_حد -= 1;
            }

            اكسر;
        }
    }


    إذا (p_پیاپی_شروع != اشاره_تهی) {
        *p_پیاپی_شروع = script_پیاپی->script_شروع;
    }

    إذا (p_پیاپی_حد != اشاره_تهی) {
        *p_پیاپی_حد = script_پیاپی->script_حد;
    }

    إذا (p_پیاپی_Script != اشاره_تهی) {
        *p_پیاپی_Script = script_پیاپی->script_کد;
    }

    أرجع درست;
}
