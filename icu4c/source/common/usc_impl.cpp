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

#شامل "unicode/uscript.h"
#شامل "usc_impl.h"
#شامل "cmemory.h"

#تعریف_کرو PAREN_ڈھیر_گہرائی 32

#تعریف_کرو MOD(sp) ((sp) % PAREN_ڈھیر_گہرائی)
#تعریف_کرو حد_بندی_INC(sp) (((sp) < PAREN_ڈھیر_گہرائی)? (sp) + 1 : PAREN_ڈھیر_گہرائی)
#تعریف_کرو INC(sp,count) (MOD((sp) + (count)))
#تعریف_کرو INC1(sp) (INC(sp, 1))
#تعریف_کرو DEC(sp,count) (MOD((sp) + PAREN_ڈھیر_گہرائی - (count)))
#تعریف_کرو DEC1(sp) (DEC(sp, 1))
#تعریف_کرو ڈھیر_IS_خالی_خانے(script_تسلسل) ((script_تسلسل)->ڈالو_گنتی <= 0)
#تعریف_کرو ڈھیر_IS_NOT_خالی_خانے(script_تسلسل) (! ڈھیر_IS_خالی_خانے(script_تسلسل))
#تعریف_کرو چوٹی_2(script_تسلسل) ((script_تسلسل)->paren_ڈھیر[(script_تسلسل)->parenSP])
#تعریف_کرو SYNC_FIXUP(script_تسلسل) ((script_تسلسل)->fixup_گنتی = 0)

ساخت Paren_ڈھیر_اندراج
{
    int32_t pair_اشاریہ;
    UScript_رمز script_رمز;
};

ساخت UScript_تسلسل
{
    int32_t متن_لمبائی;
    ثابت char16_t *متن_Array;

    int32_t script_آغاز;
    int32_t script_حد_بندی;
    UScript_رمز script_رمز;

    ساخت Paren_ڈھیر_اندراج paren_ڈھیر[PAREN_ڈھیر_گہرائی];
    int32_t parenSP;
    int32_t ڈالو_گنتی;
    int32_t fixup_گنتی;
};

ساکن int8_t بلند_ثنائی_ہندسہ(int32_t value);

ساکن ثابت UChar32 pairedChars[] = {
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

ساکن خالی push(UScript_تسلسل *script_تسلسل, int32_t pair_اشاریہ, UScript_رمز script_رمز)
{
    script_تسلسل->ڈالو_گنتی  = حد_بندی_INC(script_تسلسل->ڈالو_گنتی);
    script_تسلسل->fixup_گنتی = حد_بندی_INC(script_تسلسل->fixup_گنتی);
    
    script_تسلسل->parenSP = INC1(script_تسلسل->parenSP);
    script_تسلسل->paren_ڈھیر[script_تسلسل->parenSP].pair_اشاریہ  = pair_اشاریہ;
    script_تسلسل->paren_ڈھیر[script_تسلسل->parenSP].script_رمز = script_رمز;
}

ساکن خالی pop(UScript_تسلسل *script_تسلسل)
{
    اگر (ڈھیر_IS_خالی_خانے(script_تسلسل)) {
        واپس;
    }
    
    اگر (script_تسلسل->fixup_گنتی > 0) {
        script_تسلسل->fixup_گنتی -= 1;
    }
    
    script_تسلسل->ڈالو_گنتی -= 1;
    script_تسلسل->parenSP = DEC1(script_تسلسل->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    اگر (ڈھیر_IS_خالی_خانے(script_تسلسل)) {
        script_تسلسل->parenSP = -1;
    }
}

ساکن خالی fixup(UScript_تسلسل *script_تسلسل, UScript_رمز script_رمز)
{
    int32_t fixupSP = DEC(script_تسلسل->parenSP, script_تسلسل->fixup_گنتی);
    
    جب (script_تسلسل->fixup_گنتی-- > 0) {
        fixupSP = INC1(fixupSP);
        script_تسلسل->paren_ڈھیر[fixupSP].script_رمز = script_رمز;
    }
}

ساکن int8_t
بلند_ثنائی_ہندسہ(int32_t value)
{
    int8_t bit = 0;

    اگر (value <= 0) {
        واپس -32;
    }

    اگر (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    اگر (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    اگر (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    اگر (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    اگر (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    واپس bit;
}

ساکن int32_t
get_Pair_اشاریہ(UChar32 ch)
{
    int32_t paired_Char_گنتی = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << بلند_ثنائی_ہندسہ(paired_Char_گنتی);
    int32_t pairedCharExtra = paired_Char_گنتی - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_اشاریہ = 0;

    اگر (ch >= pairedChars[pairedCharExtra]) {
        pair_اشاریہ = pairedCharExtra;
    }

    جب (probe > (1 << 0)) {
        probe >>= 1;

        اگر (ch >= pairedChars[pair_اشاریہ + probe]) {
            pair_اشاریہ += probe;
        }
    }

    اگر (pairedChars[pair_اشاریہ] != ch) {
        pair_اشاریہ = -1;
    }

    واپس pair_اشاریہ;
}

ساکن UBool
sameScript(UScript_رمز scriptOne, UScript_رمز scriptTwo)
{
    واپس scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_تسلسل * U_EXPORT2
uscript_open_تسلسل(ثابت char16_t *src, int32_t length, UError_رمز *p_Error_رمز)
{
    UScript_تسلسل *نتیجہ = خالی_اشاریہ;

    اگر (p_Error_رمز == خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس خالی_اشاریہ;
    }

    نتیجہ = (UScript_تسلسل *)uprv_malloc(حجم (UScript_تسلسل));

    اگر (نتیجہ == خالی_اشاریہ) {
        *p_Error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس خالی_اشاریہ;
    }

    uscript_سیٹ_تسلسل_متن(نتیجہ, src, length, p_Error_رمز);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    اگر (U_FAILURE(*p_Error_رمز)) {
        uprv_free(نتیجہ);
        نتیجہ = خالی_اشاریہ;
    }

    واپس نتیجہ;
}

U_CAPI خالی U_EXPORT2
uscript_close_تسلسل(UScript_تسلسل *script_تسلسل)
{
    اگر (script_تسلسل != خالی_اشاریہ) {
        uprv_free(script_تسلسل);
    }
}

U_CAPI خالی U_EXPORT2
uscript_reset_تسلسل(UScript_تسلسل *script_تسلسل)
{
    اگر (script_تسلسل != خالی_اشاریہ) {
        script_تسلسل->script_آغاز = 0;
        script_تسلسل->script_حد_بندی = 0;
        script_تسلسل->script_رمز  = USCRIPT_INVALID_رمز;
        script_تسلسل->parenSP     = -1;
        script_تسلسل->ڈالو_گنتی   =  0;
        script_تسلسل->fixup_گنتی  =  0;
    }
}

U_CAPI خالی U_EXPORT2
uscript_سیٹ_تسلسل_متن(UScript_تسلسل *script_تسلسل, ثابت char16_t *src, int32_t length, UError_رمز *p_Error_رمز)
{
    اگر (p_Error_رمز == خالی_اشاریہ || U_FAILURE(*p_Error_رمز)) {
        واپس;
    }

    اگر (script_تسلسل == خالی_اشاریہ || length < 0 || ((src == خالی_اشاریہ) != (length == 0))) {
        *p_Error_رمز = U_ILLEGAL_دلیل_ERROR;
        واپس;
    }

    script_تسلسل->متن_Array  = src;
    script_تسلسل->متن_لمبائی = length;

    uscript_reset_تسلسل(script_تسلسل);
}

U_CAPI UBool U_EXPORT2
uscript_اگلا_تسلسل(UScript_تسلسل *script_تسلسل, int32_t *p_تسلسل_آغاز, int32_t *p_تسلسل_حد_بندی, UScript_رمز *p_تسلسل_Script)
{
    UError_رمز error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    اگر (script_تسلسل == خالی_اشاریہ || script_تسلسل->script_حد_بندی >= script_تسلسل->متن_لمبائی) {
        واپس جھوٹ;
    }
    
    SYNC_FIXUP(script_تسلسل);
    script_تسلسل->script_رمز = USCRIPT_COMMON;

    برائے (script_تسلسل->script_آغاز = script_تسلسل->script_حد_بندی; script_تسلسل->script_حد_بندی < script_تسلسل->متن_لمبائی; script_تسلسل->script_حد_بندی += 1) {
        char16_t بلند = script_تسلسل->متن_Array[script_تسلسل->script_حد_بندی];
        UChar32  ch   = بلند;
        UScript_رمز sc;
        int32_t pair_اشاریہ;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        اگر (بلند >= 0xD800 && بلند <= 0xDBFF && script_تسلسل->script_حد_بندی < script_تسلسل->متن_لمبائی - 1) {
            char16_t کم = script_تسلسل->متن_Array[script_تسلسل->script_حد_بندی + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            اگر (کم >= 0xDC00 && کم <= 0xDFFF) {
                ch = (بلند - 0xD800) * 0x0400 + کم - 0xDC00 + 0x10000;
                script_تسلسل->script_حد_بندی += 1;
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
        اگر (pair_اشاریہ >= 0) {
            اگر ((pair_اشاریہ & 1) == 0) {
                push(script_تسلسل, pair_اشاریہ, script_تسلسل->script_رمز);
            } ورنہ {
                int32_t pi = pair_اشاریہ & ~1;

                جب (ڈھیر_IS_NOT_خالی_خانے(script_تسلسل) && چوٹی_2(script_تسلسل).pair_اشاریہ != pi) {
                    pop(script_تسلسل);
                }

                اگر (ڈھیر_IS_NOT_خالی_خانے(script_تسلسل)) {
                    sc = چوٹی_2(script_تسلسل).script_رمز;
                }
            }
        }

        اگر (sameScript(script_تسلسل->script_رمز, sc)) {
            اگر (script_تسلسل->script_رمز <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_تسلسل->script_رمز = sc;

                fixup(script_تسلسل, script_تسلسل->script_رمز);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            اگر (pair_اشاریہ >= 0 && (pair_اشاریہ & 1) != 0) {
                pop(script_تسلسل);
            }
        } ورنہ {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            اگر (ch >= 0x10000) {
                script_تسلسل->script_حد_بندی -= 1;
            }

            توڑو;
        }
    }


    اگر (p_تسلسل_آغاز != خالی_اشاریہ) {
        *p_تسلسل_آغاز = script_تسلسل->script_آغاز;
    }

    اگر (p_تسلسل_حد_بندی != خالی_اشاریہ) {
        *p_تسلسل_حد_بندی = script_تسلسل->script_حد_بندی;
    }

    اگر (p_تسلسل_Script != خالی_اشاریہ) {
        *p_تسلسل_Script = script_تسلسل->script_رمز;
    }

    واپس سچ;
}
