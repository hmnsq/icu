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

#અન્તર્ભાવ "unicode/uscript.h"
#અન્તર્ભાવ "usc_impl.h"
#અન્તર્ભાવ "cmemory.h"

#define PAREN_ઢગલો_DEPTH 32

#define MOD(sp) ((sp) % PAREN_ઢગલો_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_ઢગલો_DEPTH)? (sp) + 1 : PAREN_ઢગલો_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_ઢગલો_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define ઢગલો_IS_EMPTY(scriptRun) ((scriptRun)->ધકેલો_ગણતરી <= 0)
#define ઢગલો_IS_NOT_EMPTY(scriptRun) (! ઢગલો_IS_EMPTY(scriptRun))
#define ટોચ_2(scriptRun) ((scriptRun)->paren_ઢગલો[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_ગણતરી = 0)

સંરચના Paren_ઢગલો_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

સંરચના UScriptRun
{
    int32_t textLength;
    અચલ char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    સંરચના Paren_ઢગલો_Entry paren_ઢગલો[PAREN_ઢગલો_DEPTH];
    int32_t parenSP;
    int32_t ધકેલો_ગણતરી;
    int32_t fixup_ગણતરી;
};

સ્થિર int8_t ઉપલું_Bit(int32_t value);

સ્થિર અચલ UChar32 pairedChars[] = {
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

સ્થિર શૂન્ય push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->ધકેલો_ગણતરી  = LIMIT_INC(scriptRun->ધકેલો_ગણતરી);
    scriptRun->fixup_ગણતરી = LIMIT_INC(scriptRun->fixup_ગણતરી);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_ઢગલો[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_ઢગલો[scriptRun->parenSP].scriptCode = scriptCode;
}

સ્થિર શૂન્ય pop(UScriptRun *scriptRun)
{
    જો (ઢગલો_IS_EMPTY(scriptRun)) {
        પાછા;
    }
    
    જો (scriptRun->fixup_ગણતરી > 0) {
        scriptRun->fixup_ગણતરી -= 1;
    }
    
    scriptRun->ધકેલો_ગણતરી -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    જો (ઢગલો_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

સ્થિર શૂન્ય fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_ગણતરી);
    
    જ્યારે (scriptRun->fixup_ગણતરી-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_ઢગલો[fixupSP].scriptCode = scriptCode;
    }
}

સ્થિર int8_t
ઉપલું_Bit(int32_t value)
{
    int8_t bit = 0;

    જો (value <= 0) {
        પાછા -32;
    }

    જો (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    જો (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    જો (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    જો (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    જો (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    પાછા bit;
}

સ્થિર int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_ગણતરી = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ઉપલું_Bit(paired_Char_ગણતરી);
    int32_t pairedCharExtra = paired_Char_ગણતરી - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    જો (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    જ્યારે (probe > (1 << 0)) {
        probe >>= 1;

        જો (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    જો (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    પાછા pairIndex;
}

સ્થિર UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    પાછા scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(અચલ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    જો (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        પાછા nullptr;
    }

    result = (UScriptRun *)uprv_malloc(કદ (UScriptRun));

    જો (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        પાછા nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    જો (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    પાછા result;
}

U_CAPI શૂન્ય U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    જો (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI શૂન્ય U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    જો (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->ધકેલો_ગણતરી   =  0;
        scriptRun->fixup_ગણતરી  =  0;
    }
}

U_CAPI શૂન્ય U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, અચલ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    જો (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        પાછા;
    }

    જો (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        પાછા;
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
    જો (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        પાછા ખોટું;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    માટે (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t ઉપલું = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = ઉપલું;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        જો (ઉપલું >= 0xD800 && ઉપલું <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t નીચલું = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            જો (નીચલું >= 0xDC00 && નીચલું <= 0xDFFF) {
                ch = (ઉપલું - 0xD800) * 0x0400 + નીચલું - 0xDC00 + 0x10000;
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
        જો (pairIndex >= 0) {
            જો ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } નહીંતર {
                int32_t pi = pairIndex & ~1;

                જ્યારે (ઢગલો_IS_NOT_EMPTY(scriptRun) && ટોચ_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                જો (ઢગલો_IS_NOT_EMPTY(scriptRun)) {
                    sc = ટોચ_2(scriptRun).scriptCode;
                }
            }
        }

        જો (sameScript(scriptRun->scriptCode, sc)) {
            જો (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            જો (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } નહીંતર {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            જો (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            તોડો;
        }
    }


    જો (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    જો (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    જો (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    પાછા સાચું;
}
