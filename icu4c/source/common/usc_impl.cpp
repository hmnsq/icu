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

#ಅನ್ತರ್ಭಾವ "unicode/uscript.h"
#ಅನ್ತರ್ಭಾವ "usc_impl.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

#ವ್ಯಾಖ್ಯಾನಿಸು PAREN_ರಾಶಿ_ಆಳ 32

#ವ್ಯಾಖ್ಯಾನಿಸು MOD(sp) ((sp) % PAREN_ರಾಶಿ_ಆಳ)
#ವ್ಯಾಖ್ಯಾನಿಸು ಎಲ್ಲೆ_INC(sp) (((sp) < PAREN_ರಾಶಿ_ಆಳ)? (sp) + 1 : PAREN_ರಾಶಿ_ಆಳ)
#ವ್ಯಾಖ್ಯಾನಿಸು INC(sp,count) (MOD((sp) + (count)))
#ವ್ಯಾಖ್ಯಾನಿಸು INC1(sp) (INC(sp, 1))
#ವ್ಯಾಖ್ಯಾನಿಸು DEC(sp,count) (MOD((sp) + PAREN_ರಾಶಿ_ಆಳ - (count)))
#ವ್ಯಾಖ್ಯಾನಿಸು DEC1(sp) (DEC(sp, 1))
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_IS_ಖಾಲಿಗಳು(script_ಸತತ) ((script_ಸತತ)->ತಳ್ಳು_ಎಣಿಕೆ <= 0)
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_IS_NOT_ಖಾಲಿಗಳು(script_ಸತತ) (! ರಾಶಿ_IS_ಖಾಲಿಗಳು(script_ಸತತ))
#ವ್ಯಾಖ್ಯಾನಿಸು ತುದಿ_2(script_ಸತತ) ((script_ಸತತ)->paren_ರಾಶಿ[(script_ಸತತ)->parenSP])
#ವ್ಯಾಖ್ಯಾನಿಸು SYNC_FIXUP(script_ಸತತ) ((script_ಸತತ)->fixup_ಎಣಿಕೆ = 0)

ರಚನೆ Paren_ರಾಶಿ_ನಮೂದು
{
    int32_t pair_ಸೂಚ್ಯಂಕ;
    UScript_ಸಂಕೇತ script_ಸಂಕೇತ;
};

ರಚನೆ UScript_ಸತತ
{
    int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ;
    ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ_Array;

    int32_t script_ಆರಂಭ;
    int32_t script_ಎಲ್ಲೆ;
    UScript_ಸಂಕೇತ script_ಸಂಕೇತ;

    ರಚನೆ Paren_ರಾಶಿ_ನಮೂದು paren_ರಾಶಿ[PAREN_ರಾಶಿ_ಆಳ];
    int32_t parenSP;
    int32_t ತಳ್ಳು_ಎಣಿಕೆ;
    int32_t fixup_ಎಣಿಕೆ;
};

ಸ್ಥಿರ int8_t ಮೇಲಿನ_ಬಿಟ್(int32_t value);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UChar32 pairedChars[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ push(UScript_ಸತತ *script_ಸತತ, int32_t pair_ಸೂಚ್ಯಂಕ, UScript_ಸಂಕೇತ script_ಸಂಕೇತ)
{
    script_ಸತತ->ತಳ್ಳು_ಎಣಿಕೆ  = ಎಲ್ಲೆ_INC(script_ಸತತ->ತಳ್ಳು_ಎಣಿಕೆ);
    script_ಸತತ->fixup_ಎಣಿಕೆ = ಎಲ್ಲೆ_INC(script_ಸತತ->fixup_ಎಣಿಕೆ);
    
    script_ಸತತ->parenSP = INC1(script_ಸತತ->parenSP);
    script_ಸತತ->paren_ರಾಶಿ[script_ಸತತ->parenSP].pair_ಸೂಚ್ಯಂಕ  = pair_ಸೂಚ್ಯಂಕ;
    script_ಸತತ->paren_ರಾಶಿ[script_ಸತತ->parenSP].script_ಸಂಕೇತ = script_ಸಂಕೇತ;
}

ಸ್ಥಿರ ಶೂನ್ಯ pop(UScript_ಸತತ *script_ಸತತ)
{
    ಆದರೆ (ರಾಶಿ_IS_ಖಾಲಿಗಳು(script_ಸತತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    
    ಆದರೆ (script_ಸತತ->fixup_ಎಣಿಕೆ > 0) {
        script_ಸತತ->fixup_ಎಣಿಕೆ -= 1;
    }
    
    script_ಸತತ->ತಳ್ಳು_ಎಣಿಕೆ -= 1;
    script_ಸತತ->parenSP = DEC1(script_ಸತತ->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    ಆದರೆ (ರಾಶಿ_IS_ಖಾಲಿಗಳು(script_ಸತತ)) {
        script_ಸತತ->parenSP = -1;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ fixup(UScript_ಸತತ *script_ಸತತ, UScript_ಸಂಕೇತ script_ಸಂಕೇತ)
{
    int32_t fixupSP = DEC(script_ಸತತ->parenSP, script_ಸತತ->fixup_ಎಣಿಕೆ);
    
    ಆಗ (script_ಸತತ->fixup_ಎಣಿಕೆ-- > 0) {
        fixupSP = INC1(fixupSP);
        script_ಸತತ->paren_ರಾಶಿ[fixupSP].script_ಸಂಕೇತ = script_ಸಂಕೇತ;
    }
}

ಸ್ಥಿರ int8_t
ಮೇಲಿನ_ಬಿಟ್(int32_t value)
{
    int8_t bit = 0;

    ಆದರೆ (value <= 0) {
        ಹಿಂತಿರುಗಿ -32;
    }

    ಆದರೆ (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    ಆದರೆ (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    ಆದರೆ (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    ಆದರೆ (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    ಆದರೆ (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    ಹಿಂತಿರುಗಿ bit;
}

ಸ್ಥಿರ int32_t
get_Pair_ಸೂಚ್ಯಂಕ(UChar32 ch)
{
    int32_t paired_Char_ಎಣಿಕೆ = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ಮೇಲಿನ_ಬಿಟ್(paired_Char_ಎಣಿಕೆ);
    int32_t pairedCharExtra = paired_Char_ಎಣಿಕೆ - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_ಸೂಚ್ಯಂಕ = 0;

    ಆದರೆ (ch >= pairedChars[pairedCharExtra]) {
        pair_ಸೂಚ್ಯಂಕ = pairedCharExtra;
    }

    ಆಗ (probe > (1 << 0)) {
        probe >>= 1;

        ಆದರೆ (ch >= pairedChars[pair_ಸೂಚ್ಯಂಕ + probe]) {
            pair_ಸೂಚ್ಯಂಕ += probe;
        }
    }

    ಆದರೆ (pairedChars[pair_ಸೂಚ್ಯಂಕ] != ch) {
        pair_ಸೂಚ್ಯಂಕ = -1;
    }

    ಹಿಂತಿರುಗಿ pair_ಸೂಚ್ಯಂಕ;
}

ಸ್ಥಿರ UBool
sameScript(UScript_ಸಂಕೇತ scriptOne, UScript_ಸಂಕೇತ scriptTwo)
{
    ಹಿಂತಿರುಗಿ scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_ಸತತ * U_EXPORT2
uscript_open_ಸತತ(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    UScript_ಸತತ *ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;

    ಆದರೆ (p_Error_ಸಂಕೇತ == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಫಲಿತಾಂಶ = (UScript_ಸತತ *)uprv_malloc(ಗಾತ್ರ (UScript_ಸತತ));

    ಆದರೆ (ಫಲಿತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
        *p_Error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    uscript_ಗಣ_ಸತತ_ಪಠ್ಯ(ಫಲಿತಾಂಶ, src, length, p_Error_ಸಂಕೇತ);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    ಆದರೆ (U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        uprv_free(ಫಲಿತಾಂಶ);
        ಫಲಿತಾಂಶ = ಶೂನ್ಯಸೂಚಕ;
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_close_ಸತತ(UScript_ಸತತ *script_ಸತತ)
{
    ಆದರೆ (script_ಸತತ != ಶೂನ್ಯಸೂಚಕ) {
        uprv_free(script_ಸತತ);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_reset_ಸತತ(UScript_ಸತತ *script_ಸತತ)
{
    ಆದರೆ (script_ಸತತ != ಶೂನ್ಯಸೂಚಕ) {
        script_ಸತತ->script_ಆರಂಭ = 0;
        script_ಸತತ->script_ಎಲ್ಲೆ = 0;
        script_ಸತತ->script_ಸಂಕೇತ  = USCRIPT_INVALID_ಸಂಕೇತ;
        script_ಸತತ->parenSP     = -1;
        script_ಸತತ->ತಳ್ಳು_ಎಣಿಕೆ   =  0;
        script_ಸತತ->fixup_ಎಣಿಕೆ  =  0;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_ಗಣ_ಸತತ_ಪಠ್ಯ(UScript_ಸತತ *script_ಸತತ, ಸ್ಥಿರಾಂಕ char16_t *src, int32_t length, UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ)
{
    ಆದರೆ (p_Error_ಸಂಕೇತ == ಶೂನ್ಯಸೂಚಕ || U_FAILURE(*p_Error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (script_ಸತತ == ಶೂನ್ಯಸೂಚಕ || length < 0 || ((src == ಶೂನ್ಯಸೂಚಕ) != (length == 0))) {
        *p_Error_ಸಂಕೇತ = U_ILLEGAL_ವಾದ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }

    script_ಸತತ->ಪಠ್ಯ_Array  = src;
    script_ಸತತ->ಪಠ್ಯ_ಉದ್ದಳತೆ = length;

    uscript_reset_ಸತತ(script_ಸತತ);
}

U_CAPI UBool U_EXPORT2
uscript_ಮುಂದಿನ_ಸತತ(UScript_ಸತತ *script_ಸತತ, int32_t *p_ಸತತ_ಆರಂಭ, int32_t *p_ಸತತ_ಎಲ್ಲೆ, UScript_ಸಂಕೇತ *p_ಸತತ_Script)
{
    UError_ಸಂಕೇತ error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    ಆದರೆ (script_ಸತತ == ಶೂನ್ಯಸೂಚಕ || script_ಸತತ->script_ಎಲ್ಲೆ >= script_ಸತತ->ಪಠ್ಯ_ಉದ್ದಳತೆ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    
    SYNC_FIXUP(script_ಸತತ);
    script_ಸತತ->script_ಸಂಕೇತ = USCRIPT_COMMON;

    ಗಾಗಿ (script_ಸತತ->script_ಆರಂಭ = script_ಸತತ->script_ಎಲ್ಲೆ; script_ಸತತ->script_ಎಲ್ಲೆ < script_ಸತತ->ಪಠ್ಯ_ಉದ್ದಳತೆ; script_ಸತತ->script_ಎಲ್ಲೆ += 1) {
        char16_t ಮೇಲಿನ = script_ಸತತ->ಪಠ್ಯ_Array[script_ಸತತ->script_ಎಲ್ಲೆ];
        UChar32  ch   = ಮೇಲಿನ;
        UScript_ಸಂಕೇತ sc;
        int32_t pair_ಸೂಚ್ಯಂಕ;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        ಆದರೆ (ಮೇಲಿನ >= 0xD800 && ಮೇಲಿನ <= 0xDBFF && script_ಸತತ->script_ಎಲ್ಲೆ < script_ಸತತ->ಪಠ್ಯ_ಉದ್ದಳತೆ - 1) {
            char16_t ಕೆಳಗಿನ = script_ಸತತ->ಪಠ್ಯ_Array[script_ಸತತ->script_ಎಲ್ಲೆ + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            ಆದರೆ (ಕೆಳಗಿನ >= 0xDC00 && ಕೆಳಗಿನ <= 0xDFFF) {
                ch = (ಮೇಲಿನ - 0xD800) * 0x0400 + ಕೆಳಗಿನ - 0xDC00 + 0x10000;
                script_ಸತತ->script_ಎಲ್ಲೆ += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_ಸೂಚ್ಯಂಕ = get_Pair_ಸೂಚ್ಯಂಕ(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        ಆದರೆ (pair_ಸೂಚ್ಯಂಕ >= 0) {
            ಆದರೆ ((pair_ಸೂಚ್ಯಂಕ & 1) == 0) {
                push(script_ಸತತ, pair_ಸೂಚ್ಯಂಕ, script_ಸತತ->script_ಸಂಕೇತ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t pi = pair_ಸೂಚ್ಯಂಕ & ~1;

                ಆಗ (ರಾಶಿ_IS_NOT_ಖಾಲಿಗಳು(script_ಸತತ) && ತುದಿ_2(script_ಸತತ).pair_ಸೂಚ್ಯಂಕ != pi) {
                    pop(script_ಸತತ);
                }

                ಆದರೆ (ರಾಶಿ_IS_NOT_ಖಾಲಿಗಳು(script_ಸತತ)) {
                    sc = ತುದಿ_2(script_ಸತತ).script_ಸಂಕೇತ;
                }
            }
        }

        ಆದರೆ (sameScript(script_ಸತತ->script_ಸಂಕೇತ, sc)) {
            ಆದರೆ (script_ಸತತ->script_ಸಂಕೇತ <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_ಸತತ->script_ಸಂಕೇತ = sc;

                fixup(script_ಸತತ, script_ಸತತ->script_ಸಂಕೇತ);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            ಆದರೆ (pair_ಸೂಚ್ಯಂಕ >= 0 && (pair_ಸೂಚ್ಯಂಕ & 1) != 0) {
                pop(script_ಸತತ);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            ಆದರೆ (ch >= 0x10000) {
                script_ಸತತ->script_ಎಲ್ಲೆ -= 1;
            }

            ನಿಲ್ಲಿಸು;
        }
    }


    ಆದರೆ (p_ಸತತ_ಆರಂಭ != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಸತತ_ಆರಂಭ = script_ಸತತ->script_ಆರಂಭ;
    }

    ಆದರೆ (p_ಸತತ_ಎಲ್ಲೆ != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಸತತ_ಎಲ್ಲೆ = script_ಸತತ->script_ಎಲ್ಲೆ;
    }

    ಆದರೆ (p_ಸತತ_Script != ಶೂನ್ಯಸೂಚಕ) {
        *p_ಸತತ_Script = script_ಸತತ->script_ಸಂಕೇತ;
    }

    ಹಿಂತಿರುಗಿ ನಿಜ;
}
