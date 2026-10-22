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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uscript.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "usc_impl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PAREN_ਢੇਰ_ਡੂੰਘਾਈ 32

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOD(sp) ((sp) % PAREN_ਢੇਰ_ਡੂੰਘਾਈ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਹੱਦਬੰਦੀ_INC(sp) (((sp) < PAREN_ਢੇਰ_ਡੂੰਘਾਈ)? (sp) + 1 : PAREN_ਢੇਰ_ਡੂੰਘਾਈ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INC(sp,count) (MOD((sp) + (count)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INC1(sp) (INC(sp, 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEC(sp,count) (MOD((sp) + PAREN_ਢੇਰ_ਡੂੰਘਾਈ - (count)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEC1(sp) (DEC(sp, 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_IS_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ) ((script_ਲੜੀਵਾਰ)->ਧੱਕੋ_ਗਿਣਤੀ <= 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਢੇਰ_IS_NOT_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ) (! ਢੇਰ_IS_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਖਰ_2(script_ਲੜੀਵਾਰ) ((script_ਲੜੀਵਾਰ)->paren_ਢੇਰ[(script_ਲੜੀਵਾਰ)->parenSP])
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNC_FIXUP(script_ਲੜੀਵਾਰ) ((script_ਲੜੀਵਾਰ)->fixup_ਗਿਣਤੀ = 0)

ਬਣਤਰ Paren_ਢੇਰ_ਇੰਦਰਾਜ
{
    int32_t pair_ਸੂਚਕਾਂਕ;
    UScript_ਕੋਡ script_ਕੋਡ;
};

ਬਣਤਰ UScript_ਲੜੀਵਾਰ
{
    int32_t ਪਾਠ_ਲੰਮਾਈ;
    ਅਚਲ char16_t *ਪਾਠ_Array;

    int32_t script_ਸ਼ੁਰੂ;
    int32_t script_ਹੱਦਬੰਦੀ;
    UScript_ਕੋਡ script_ਕੋਡ;

    ਬਣਤਰ Paren_ਢੇਰ_ਇੰਦਰਾਜ paren_ਢੇਰ[PAREN_ਢੇਰ_ਡੂੰਘਾਈ];
    int32_t parenSP;
    int32_t ਧੱਕੋ_ਗਿਣਤੀ;
    int32_t fixup_ਗਿਣਤੀ;
};

ਸਥਿਰ int8_t ਉੱਚਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(int32_t value);

ਸਥਿਰ ਅਚਲ UChar32 pairedChars[] = {
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

ਸਥਿਰ ਖਾਲੀ push(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ, int32_t pair_ਸੂਚਕਾਂਕ, UScript_ਕੋਡ script_ਕੋਡ)
{
    script_ਲੜੀਵਾਰ->ਧੱਕੋ_ਗਿਣਤੀ  = ਹੱਦਬੰਦੀ_INC(script_ਲੜੀਵਾਰ->ਧੱਕੋ_ਗਿਣਤੀ);
    script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ = ਹੱਦਬੰਦੀ_INC(script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ);
    
    script_ਲੜੀਵਾਰ->parenSP = INC1(script_ਲੜੀਵਾਰ->parenSP);
    script_ਲੜੀਵਾਰ->paren_ਢੇਰ[script_ਲੜੀਵਾਰ->parenSP].pair_ਸੂਚਕਾਂਕ  = pair_ਸੂਚਕਾਂਕ;
    script_ਲੜੀਵਾਰ->paren_ਢੇਰ[script_ਲੜੀਵਾਰ->parenSP].script_ਕੋਡ = script_ਕੋਡ;
}

ਸਥਿਰ ਖਾਲੀ pop(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ)
{
    ਜੇ (ਢੇਰ_IS_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ)) {
        ਵਾਪਸ;
    }
    
    ਜੇ (script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ > 0) {
        script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ -= 1;
    }
    
    script_ਲੜੀਵਾਰ->ਧੱਕੋ_ਗਿਣਤੀ -= 1;
    script_ਲੜੀਵਾਰ->parenSP = DEC1(script_ਲੜੀਵਾਰ->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    ਜੇ (ਢੇਰ_IS_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ)) {
        script_ਲੜੀਵਾਰ->parenSP = -1;
    }
}

ਸਥਿਰ ਖਾਲੀ fixup(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ, UScript_ਕੋਡ script_ਕੋਡ)
{
    int32_t fixupSP = DEC(script_ਲੜੀਵਾਰ->parenSP, script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ);
    
    ਜਦੋਂ (script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ-- > 0) {
        fixupSP = INC1(fixupSP);
        script_ਲੜੀਵਾਰ->paren_ਢੇਰ[fixupSP].script_ਕੋਡ = script_ਕੋਡ;
    }
}

ਸਥਿਰ int8_t
ਉੱਚਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(int32_t value)
{
    int8_t bit = 0;

    ਜੇ (value <= 0) {
        ਵਾਪਸ -32;
    }

    ਜੇ (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    ਜੇ (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    ਜੇ (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    ਜੇ (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    ਜੇ (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    ਵਾਪਸ bit;
}

ਸਥਿਰ int32_t
get_Pair_ਸੂਚਕਾਂਕ(UChar32 ch)
{
    int32_t paired_Char_ਗਿਣਤੀ = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ਉੱਚਾ_ਦੋਆਧਾਰੀ_ਅੰਕ(paired_Char_ਗਿਣਤੀ);
    int32_t pairedCharExtra = paired_Char_ਗਿਣਤੀ - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_ਸੂਚਕਾਂਕ = 0;

    ਜੇ (ch >= pairedChars[pairedCharExtra]) {
        pair_ਸੂਚਕਾਂਕ = pairedCharExtra;
    }

    ਜਦੋਂ (probe > (1 << 0)) {
        probe >>= 1;

        ਜੇ (ch >= pairedChars[pair_ਸੂਚਕਾਂਕ + probe]) {
            pair_ਸੂਚਕਾਂਕ += probe;
        }
    }

    ਜੇ (pairedChars[pair_ਸੂਚਕਾਂਕ] != ch) {
        pair_ਸੂਚਕਾਂਕ = -1;
    }

    ਵਾਪਸ pair_ਸੂਚਕਾਂਕ;
}

ਸਥਿਰ UBool
sameScript(UScript_ਕੋਡ scriptOne, UScript_ਕੋਡ scriptTwo)
{
    ਵਾਪਸ scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_ਲੜੀਵਾਰ * U_EXPORT2
uscript_open_ਲੜੀਵਾਰ(ਅਚਲ char16_t *src, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    UScript_ਲੜੀਵਾਰ *ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;

    ਜੇ (p_Error_ਕੋਡ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਨਤੀਜਾ = (UScript_ਲੜੀਵਾਰ *)uprv_malloc(ਆਕਾਰ (UScript_ਲੜੀਵਾਰ));

    ਜੇ (ਨਤੀਜਾ == ਖਾਲੀ_ਸੂਚਕ) {
        *p_Error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    uscript_ਸੈੱਟ_ਲੜੀਵਾਰ_ਪਾਠ(ਨਤੀਜਾ, src, length, p_Error_ਕੋਡ);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    ਜੇ (U_FAILURE(*p_Error_ਕੋਡ)) {
        uprv_free(ਨਤੀਜਾ);
        ਨਤੀਜਾ = ਖਾਲੀ_ਸੂਚਕ;
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

U_CAPI ਖਾਲੀ U_EXPORT2
uscript_close_ਲੜੀਵਾਰ(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ)
{
    ਜੇ (script_ਲੜੀਵਾਰ != ਖਾਲੀ_ਸੂਚਕ) {
        uprv_free(script_ਲੜੀਵਾਰ);
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uscript_reset_ਲੜੀਵਾਰ(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ)
{
    ਜੇ (script_ਲੜੀਵਾਰ != ਖਾਲੀ_ਸੂਚਕ) {
        script_ਲੜੀਵਾਰ->script_ਸ਼ੁਰੂ = 0;
        script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ = 0;
        script_ਲੜੀਵਾਰ->script_ਕੋਡ  = USCRIPT_INVALID_ਕੋਡ;
        script_ਲੜੀਵਾਰ->parenSP     = -1;
        script_ਲੜੀਵਾਰ->ਧੱਕੋ_ਗਿਣਤੀ   =  0;
        script_ਲੜੀਵਾਰ->fixup_ਗਿਣਤੀ  =  0;
    }
}

U_CAPI ਖਾਲੀ U_EXPORT2
uscript_ਸੈੱਟ_ਲੜੀਵਾਰ_ਪਾਠ(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ, ਅਚਲ char16_t *src, int32_t length, UError_ਕੋਡ *p_Error_ਕੋਡ)
{
    ਜੇ (p_Error_ਕੋਡ == ਖਾਲੀ_ਸੂਚਕ || U_FAILURE(*p_Error_ਕੋਡ)) {
        ਵਾਪਸ;
    }

    ਜੇ (script_ਲੜੀਵਾਰ == ਖਾਲੀ_ਸੂਚਕ || length < 0 || ((src == ਖਾਲੀ_ਸੂਚਕ) != (length == 0))) {
        *p_Error_ਕੋਡ = U_ILLEGAL_ਦਲੀਲ_ERROR;
        ਵਾਪਸ;
    }

    script_ਲੜੀਵਾਰ->ਪਾਠ_Array  = src;
    script_ਲੜੀਵਾਰ->ਪਾਠ_ਲੰਮਾਈ = length;

    uscript_reset_ਲੜੀਵਾਰ(script_ਲੜੀਵਾਰ);
}

U_CAPI UBool U_EXPORT2
uscript_ਅਗਲਾ_ਲੜੀਵਾਰ(UScript_ਲੜੀਵਾਰ *script_ਲੜੀਵਾਰ, int32_t *p_ਲੜੀਵਾਰ_ਸ਼ੁਰੂ, int32_t *p_ਲੜੀਵਾਰ_ਹੱਦਬੰਦੀ, UScript_ਕੋਡ *p_ਲੜੀਵਾਰ_Script)
{
    UError_ਕੋਡ error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    ਜੇ (script_ਲੜੀਵਾਰ == ਖਾਲੀ_ਸੂਚਕ || script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ >= script_ਲੜੀਵਾਰ->ਪਾਠ_ਲੰਮਾਈ) {
        ਵਾਪਸ ਝੂਠ;
    }
    
    SYNC_FIXUP(script_ਲੜੀਵਾਰ);
    script_ਲੜੀਵਾਰ->script_ਕੋਡ = USCRIPT_COMMON;

    ਲਈ (script_ਲੜੀਵਾਰ->script_ਸ਼ੁਰੂ = script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ; script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ < script_ਲੜੀਵਾਰ->ਪਾਠ_ਲੰਮਾਈ; script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ += 1) {
        char16_t ਉੱਚਾ = script_ਲੜੀਵਾਰ->ਪਾਠ_Array[script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ];
        UChar32  ch   = ਉੱਚਾ;
        UScript_ਕੋਡ sc;
        int32_t pair_ਸੂਚਕਾਂਕ;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        ਜੇ (ਉੱਚਾ >= 0xD800 && ਉੱਚਾ <= 0xDBFF && script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ < script_ਲੜੀਵਾਰ->ਪਾਠ_ਲੰਮਾਈ - 1) {
            char16_t ਨੀਵਾਂ = script_ਲੜੀਵਾਰ->ਪਾਠ_Array[script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            ਜੇ (ਨੀਵਾਂ >= 0xDC00 && ਨੀਵਾਂ <= 0xDFFF) {
                ch = (ਉੱਚਾ - 0xD800) * 0x0400 + ਨੀਵਾਂ - 0xDC00 + 0x10000;
                script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_ਸੂਚਕਾਂਕ = get_Pair_ਸੂਚਕਾਂਕ(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        ਜੇ (pair_ਸੂਚਕਾਂਕ >= 0) {
            ਜੇ ((pair_ਸੂਚਕਾਂਕ & 1) == 0) {
                push(script_ਲੜੀਵਾਰ, pair_ਸੂਚਕਾਂਕ, script_ਲੜੀਵਾਰ->script_ਕੋਡ);
            } ਨਹੀਂਤਾਂ {
                int32_t pi = pair_ਸੂਚਕਾਂਕ & ~1;

                ਜਦੋਂ (ਢੇਰ_IS_NOT_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ) && ਸਿਖਰ_2(script_ਲੜੀਵਾਰ).pair_ਸੂਚਕਾਂਕ != pi) {
                    pop(script_ਲੜੀਵਾਰ);
                }

                ਜੇ (ਢੇਰ_IS_NOT_ਖਾਲੀ_ਥਾਵਾਂ(script_ਲੜੀਵਾਰ)) {
                    sc = ਸਿਖਰ_2(script_ਲੜੀਵਾਰ).script_ਕੋਡ;
                }
            }
        }

        ਜੇ (sameScript(script_ਲੜੀਵਾਰ->script_ਕੋਡ, sc)) {
            ਜੇ (script_ਲੜੀਵਾਰ->script_ਕੋਡ <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_ਲੜੀਵਾਰ->script_ਕੋਡ = sc;

                fixup(script_ਲੜੀਵਾਰ, script_ਲੜੀਵਾਰ->script_ਕੋਡ);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            ਜੇ (pair_ਸੂਚਕਾਂਕ >= 0 && (pair_ਸੂਚਕਾਂਕ & 1) != 0) {
                pop(script_ਲੜੀਵਾਰ);
            }
        } ਨਹੀਂਤਾਂ {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            ਜੇ (ch >= 0x10000) {
                script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ -= 1;
            }

            ਰੋਕੋ;
        }
    }


    ਜੇ (p_ਲੜੀਵਾਰ_ਸ਼ੁਰੂ != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਲੜੀਵਾਰ_ਸ਼ੁਰੂ = script_ਲੜੀਵਾਰ->script_ਸ਼ੁਰੂ;
    }

    ਜੇ (p_ਲੜੀਵਾਰ_ਹੱਦਬੰਦੀ != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਲੜੀਵਾਰ_ਹੱਦਬੰਦੀ = script_ਲੜੀਵਾਰ->script_ਹੱਦਬੰਦੀ;
    }

    ਜੇ (p_ਲੜੀਵਾਰ_Script != ਖਾਲੀ_ਸੂਚਕ) {
        *p_ਲੜੀਵਾਰ_Script = script_ਲੜੀਵਾਰ->script_ਕੋਡ;
    }

    ਵਾਪਸ ਸੱਚ;
}
