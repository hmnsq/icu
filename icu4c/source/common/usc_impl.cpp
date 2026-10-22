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

#define PAREN_ರಾಶಿ_DEPTH 32

#define MOD(sp) ((sp) % PAREN_ರಾಶಿ_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_ರಾಶಿ_DEPTH)? (sp) + 1 : PAREN_ರಾಶಿ_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_ರಾಶಿ_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define ರಾಶಿ_IS_EMPTY(scriptRun) ((scriptRun)->ತಳ್ಳು_ಎಣಿಕೆ <= 0)
#define ರಾಶಿ_IS_NOT_EMPTY(scriptRun) (! ರಾಶಿ_IS_EMPTY(scriptRun))
#define ತುದಿ_2(scriptRun) ((scriptRun)->paren_ರಾಶಿ[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_ಎಣಿಕೆ = 0)

ಸಂರಚನಾ Paren_ರಾಶಿ_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

ಸಂರಚನಾ UScriptRun
{
    int32_t textLength;
    ಧ್ರುವ char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    ಸಂರಚನಾ Paren_ರಾಶಿ_Entry paren_ರಾಶಿ[PAREN_ರಾಶಿ_DEPTH];
    int32_t parenSP;
    int32_t ತಳ್ಳು_ಎಣಿಕೆ;
    int32_t fixup_ಎಣಿಕೆ;
};

ಸ್ಥಿರ int8_t ಮೇಲಿನ_Bit(int32_t value);

ಸ್ಥಿರ ಧ್ರುವ UChar32 pairedChars[] = {
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

ಸ್ಥಿರ ಶೂನ್ಯ push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->ತಳ್ಳು_ಎಣಿಕೆ  = LIMIT_INC(scriptRun->ತಳ್ಳು_ಎಣಿಕೆ);
    scriptRun->fixup_ಎಣಿಕೆ = LIMIT_INC(scriptRun->fixup_ಎಣಿಕೆ);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_ರಾಶಿ[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_ರಾಶಿ[scriptRun->parenSP].scriptCode = scriptCode;
}

ಸ್ಥಿರ ಶೂನ್ಯ pop(UScriptRun *scriptRun)
{
    ಯದಿ (ರಾಶಿ_IS_EMPTY(scriptRun)) {
        ಪ್ರತ್ಯಾಗಮ;
    }
    
    ಯದಿ (scriptRun->fixup_ಎಣಿಕೆ > 0) {
        scriptRun->fixup_ಎಣಿಕೆ -= 1;
    }
    
    scriptRun->ತಳ್ಳು_ಎಣಿಕೆ -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    ಯದಿ (ರಾಶಿ_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_ಎಣಿಕೆ);
    
    ಯಾವತ್ (scriptRun->fixup_ಎಣಿಕೆ-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_ರಾಶಿ[fixupSP].scriptCode = scriptCode;
    }
}

ಸ್ಥಿರ int8_t
ಮೇಲಿನ_Bit(int32_t value)
{
    int8_t bit = 0;

    ಯದಿ (value <= 0) {
        ಪ್ರತ್ಯಾಗಮ -32;
    }

    ಯದಿ (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    ಯದಿ (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    ಯದಿ (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    ಯದಿ (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    ಯದಿ (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    ಪ್ರತ್ಯಾಗಮ bit;
}

ಸ್ಥಿರ int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_ಎಣಿಕೆ = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ಮೇಲಿನ_Bit(paired_Char_ಎಣಿಕೆ);
    int32_t pairedCharExtra = paired_Char_ಎಣಿಕೆ - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    ಯದಿ (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    ಯಾವತ್ (probe > (1 << 0)) {
        probe >>= 1;

        ಯದಿ (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    ಯದಿ (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    ಪ್ರತ್ಯಾಗಮ pairIndex;
}

ಸ್ಥಿರ UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    ಪ್ರತ್ಯಾಗಮ scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(ಧ್ರುವ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    ಯದಿ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    result = (UScriptRun *)uprv_malloc(ಪರಿಮಾಣ (UScriptRun));

    ಯದಿ (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    ಯದಿ (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    ಯದಿ (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    ಯದಿ (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->ತಳ್ಳು_ಎಣಿಕೆ   =  0;
        scriptRun->fixup_ಎಣಿಕೆ  =  0;
    }
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, ಧ್ರುವ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    ಯದಿ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ಪ್ರತ್ಯಾಗಮ;
    }

    scriptRun->textArray  = src;
    scriptRun->textLength = length;

    uscript_resetRun(scriptRun);
}

U_CAPI UBool U_EXPORT2
uscript_nextRun(UScriptRun *scriptRun, int32_t *pRunStart, int32_t *pRunLimit, UScriptCode *pRunScript)
{
    UErrorCode error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    ಯದಿ (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    ಪ್ರತಿ (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t ಮೇಲಿನ = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = ಮೇಲಿನ;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        ಯದಿ (ಮೇಲಿನ >= 0xD800 && ಮೇಲಿನ <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t ಕೆಳಗಿನ = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            ಯದಿ (ಕೆಳಗಿನ >= 0xDC00 && ಕೆಳಗಿನ <= 0xDFFF) {
                ch = (ಮೇಲಿನ - 0xD800) * 0x0400 + ಕೆಳಗಿನ - 0xDC00 + 0x10000;
                scriptRun->scriptLimit += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pairIndex = getPairIndex(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        ಯದಿ (pairIndex >= 0) {
            ಯದಿ ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } ಅನ್ಯಥಾ {
                int32_t pi = pairIndex & ~1;

                ಯಾವತ್ (ರಾಶಿ_IS_NOT_EMPTY(scriptRun) && ತುದಿ_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                ಯದಿ (ರಾಶಿ_IS_NOT_EMPTY(scriptRun)) {
                    sc = ತುದಿ_2(scriptRun).scriptCode;
                }
            }
        }

        ಯದಿ (sameScript(scriptRun->scriptCode, sc)) {
            ಯದಿ (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            ಯದಿ (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } ಅನ್ಯಥಾ {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            ಯದಿ (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            ಭಙ್ಗ;
        }
    }


    ಯದಿ (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    ಯದಿ (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    ಯದಿ (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}
