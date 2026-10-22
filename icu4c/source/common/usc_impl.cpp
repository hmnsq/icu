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

#تعریف_کرو PAREN_ڈھیر_ڈونگھائی 32

#تعریف_کرو MOD(sp) ((sp) % PAREN_ڈھیر_ڈونگھائی)
#تعریف_کرو حد_بندی_INC(sp) (((sp) < PAREN_ڈھیر_ڈونگھائی)? (sp) + 1 : PAREN_ڈھیر_ڈونگھائی)
#تعریف_کرو INC(sp,count) (MOD((sp) + (count)))
#تعریف_کرو INC1(sp) (INC(sp, 1))
#تعریف_کرو DEC(sp,count) (MOD((sp) + PAREN_ڈھیر_ڈونگھائی - (count)))
#تعریف_کرو DEC1(sp) (DEC(sp, 1))
#تعریف_کرو ڈھیر_IS_خالی_تھاواں(script_لگاتار) ((script_لگاتار)->ڈالو_گنتی <= 0)
#تعریف_کرو ڈھیر_IS_NOT_خالی_تھاواں(script_لگاتار) (! ڈھیر_IS_خالی_تھاواں(script_لگاتار))
#تعریف_کرو چوٹی_2(script_لگاتار) ((script_لگاتار)->paren_ڈھیر[(script_لگاتار)->parenSP])
#تعریف_کرو SYNC_FIXUP(script_لگاتار) ((script_لگاتار)->fixup_گنتی = 0)

بنية Paren_ڈھیر_اندراج
{
    int32_t pair_اشاریہ;
    UScript_کوڈ script_کوڈ;
};

بنية UScript_لگاتار
{
    int32_t متن_لمائی;
    ثابت char16_t *متن_Array;

    int32_t script_شروع;
    int32_t script_حد_بندی;
    UScript_کوڈ script_کوڈ;

    بنية Paren_ڈھیر_اندراج paren_ڈھیر[PAREN_ڈھیر_ڈونگھائی];
    int32_t parenSP;
    int32_t ڈالو_گنتی;
    int32_t fixup_گنتی;
};

ساكن int8_t بلند_ثنائی_ہندسہ(int32_t value);

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

ساكن فراغ push(UScript_لگاتار *script_لگاتار, int32_t pair_اشاریہ, UScript_کوڈ script_کوڈ)
{
    script_لگاتار->ڈالو_گنتی  = حد_بندی_INC(script_لگاتار->ڈالو_گنتی);
    script_لگاتار->fixup_گنتی = حد_بندی_INC(script_لگاتار->fixup_گنتی);
    
    script_لگاتار->parenSP = INC1(script_لگاتار->parenSP);
    script_لگاتار->paren_ڈھیر[script_لگاتار->parenSP].pair_اشاریہ  = pair_اشاریہ;
    script_لگاتار->paren_ڈھیر[script_لگاتار->parenSP].script_کوڈ = script_کوڈ;
}

ساكن فراغ pop(UScript_لگاتار *script_لگاتار)
{
    إذا (ڈھیر_IS_خالی_تھاواں(script_لگاتار)) {
        أرجع;
    }
    
    إذا (script_لگاتار->fixup_گنتی > 0) {
        script_لگاتار->fixup_گنتی -= 1;
    }
    
    script_لگاتار->ڈالو_گنتی -= 1;
    script_لگاتار->parenSP = DEC1(script_لگاتار->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    إذا (ڈھیر_IS_خالی_تھاواں(script_لگاتار)) {
        script_لگاتار->parenSP = -1;
    }
}

ساكن فراغ fixup(UScript_لگاتار *script_لگاتار, UScript_کوڈ script_کوڈ)
{
    int32_t fixupSP = DEC(script_لگاتار->parenSP, script_لگاتار->fixup_گنتی);
    
    طالما (script_لگاتار->fixup_گنتی-- > 0) {
        fixupSP = INC1(fixupSP);
        script_لگاتار->paren_ڈھیر[fixupSP].script_کوڈ = script_کوڈ;
    }
}

ساكن int8_t
بلند_ثنائی_ہندسہ(int32_t value)
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
get_Pair_اشاریہ(UChar32 ch)
{
    int32_t paired_Char_گنتی = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << بلند_ثنائی_ہندسہ(paired_Char_گنتی);
    int32_t pairedCharExtra = paired_Char_گنتی - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_اشاریہ = 0;

    إذا (ch >= pairedChars[pairedCharExtra]) {
        pair_اشاریہ = pairedCharExtra;
    }

    طالما (probe > (1 << 0)) {
        probe >>= 1;

        إذا (ch >= pairedChars[pair_اشاریہ + probe]) {
            pair_اشاریہ += probe;
        }
    }

    إذا (pairedChars[pair_اشاریہ] != ch) {
        pair_اشاریہ = -1;
    }

    أرجع pair_اشاریہ;
}

ساكن UBool
sameScript(UScript_کوڈ scriptOne, UScript_کوڈ scriptTwo)
{
    أرجع scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_لگاتار * U_EXPORT2
uscript_open_لگاتار(ثابت char16_t *src, int32_t length, UError_کوڈ *p_Error_کوڈ)
{
    UScript_لگاتار *نتیجہ = خالی_سوچک;

    إذا (p_Error_کوڈ == خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع خالی_سوچک;
    }

    نتیجہ = (UScript_لگاتار *)uprv_malloc(حجم (UScript_لگاتار));

    إذا (نتیجہ == خالی_سوچک) {
        *p_Error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع خالی_سوچک;
    }

    uscript_سیٹ_لگاتار_متن(نتیجہ, src, length, p_Error_کوڈ);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    إذا (U_FAILURE(*p_Error_کوڈ)) {
        uprv_free(نتیجہ);
        نتیجہ = خالی_سوچک;
    }

    أرجع نتیجہ;
}

U_CAPI فراغ U_EXPORT2
uscript_close_لگاتار(UScript_لگاتار *script_لگاتار)
{
    إذا (script_لگاتار != خالی_سوچک) {
        uprv_free(script_لگاتار);
    }
}

U_CAPI فراغ U_EXPORT2
uscript_reset_لگاتار(UScript_لگاتار *script_لگاتار)
{
    إذا (script_لگاتار != خالی_سوچک) {
        script_لگاتار->script_شروع = 0;
        script_لگاتار->script_حد_بندی = 0;
        script_لگاتار->script_کوڈ  = USCRIPT_INVALID_کوڈ;
        script_لگاتار->parenSP     = -1;
        script_لگاتار->ڈالو_گنتی   =  0;
        script_لگاتار->fixup_گنتی  =  0;
    }
}

U_CAPI فراغ U_EXPORT2
uscript_سیٹ_لگاتار_متن(UScript_لگاتار *script_لگاتار, ثابت char16_t *src, int32_t length, UError_کوڈ *p_Error_کوڈ)
{
    إذا (p_Error_کوڈ == خالی_سوچک || U_FAILURE(*p_Error_کوڈ)) {
        أرجع;
    }

    إذا (script_لگاتار == خالی_سوچک || length < 0 || ((src == خالی_سوچک) != (length == 0))) {
        *p_Error_کوڈ = U_ILLEGAL_دلیل_ERROR;
        أرجع;
    }

    script_لگاتار->متن_Array  = src;
    script_لگاتار->متن_لمائی = length;

    uscript_reset_لگاتار(script_لگاتار);
}

U_CAPI UBool U_EXPORT2
uscript_اگلا_لگاتار(UScript_لگاتار *script_لگاتار, int32_t *p_لگاتار_شروع, int32_t *p_لگاتار_حد_بندی, UScript_کوڈ *p_لگاتار_Script)
{
    UError_کوڈ error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    إذا (script_لگاتار == خالی_سوچک || script_لگاتار->script_حد_بندی >= script_لگاتار->متن_لمائی) {
        أرجع خطأ;
    }
    
    SYNC_FIXUP(script_لگاتار);
    script_لگاتار->script_کوڈ = USCRIPT_COMMON;

    لكل (script_لگاتار->script_شروع = script_لگاتار->script_حد_بندی; script_لگاتار->script_حد_بندی < script_لگاتار->متن_لمائی; script_لگاتار->script_حد_بندی += 1) {
        char16_t بلند = script_لگاتار->متن_Array[script_لگاتار->script_حد_بندی];
        UChar32  ch   = بلند;
        UScript_کوڈ sc;
        int32_t pair_اشاریہ;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        إذا (بلند >= 0xD800 && بلند <= 0xDBFF && script_لگاتار->script_حد_بندی < script_لگاتار->متن_لمائی - 1) {
            char16_t کم = script_لگاتار->متن_Array[script_لگاتار->script_حد_بندی + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            إذا (کم >= 0xDC00 && کم <= 0xDFFF) {
                ch = (بلند - 0xD800) * 0x0400 + کم - 0xDC00 + 0x10000;
                script_لگاتار->script_حد_بندی += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_اشاریہ = get_Pair_اشاریہ(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        إذا (pair_اشاریہ >= 0) {
            إذا ((pair_اشاریہ & 1) == 0) {
                push(script_لگاتار, pair_اشاریہ, script_لگاتار->script_کوڈ);
            } وإلا {
                int32_t pi = pair_اشاریہ & ~1;

                طالما (ڈھیر_IS_NOT_خالی_تھاواں(script_لگاتار) && چوٹی_2(script_لگاتار).pair_اشاریہ != pi) {
                    pop(script_لگاتار);
                }

                إذا (ڈھیر_IS_NOT_خالی_تھاواں(script_لگاتار)) {
                    sc = چوٹی_2(script_لگاتار).script_کوڈ;
                }
            }
        }

        إذا (sameScript(script_لگاتار->script_کوڈ, sc)) {
            إذا (script_لگاتار->script_کوڈ <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_لگاتار->script_کوڈ = sc;

                fixup(script_لگاتار, script_لگاتار->script_کوڈ);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            إذا (pair_اشاریہ >= 0 && (pair_اشاریہ & 1) != 0) {
                pop(script_لگاتار);
            }
        } وإلا {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            إذا (ch >= 0x10000) {
                script_لگاتار->script_حد_بندی -= 1;
            }

            اكسر;
        }
    }


    إذا (p_لگاتار_شروع != خالی_سوچک) {
        *p_لگاتار_شروع = script_لگاتار->script_شروع;
    }

    إذا (p_لگاتار_حد_بندی != خالی_سوچک) {
        *p_لگاتار_حد_بندی = script_لگاتار->script_حد_بندی;
    }

    إذا (p_لگاتار_Script != خالی_سوچک) {
        *p_لگاتار_Script = script_لگاتار->script_کوڈ;
    }

    أرجع سچ;
}
