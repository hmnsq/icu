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

#അന്തര്ഭാവ "unicode/uscript.h"
#അന്തര്ഭാവ "usc_impl.h"
#അന്തര്ഭാവ "cmemory.h"

#define PAREN_അട്ടി_DEPTH 32

#define MOD(sp) ((sp) % PAREN_അട്ടി_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_അട്ടി_DEPTH)? (sp) + 1 : PAREN_അട്ടി_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_അട്ടി_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define അട്ടി_IS_EMPTY(scriptRun) ((scriptRun)->തള്ളുക_എണ്ണം <= 0)
#define അട്ടി_IS_NOT_EMPTY(scriptRun) (! അട്ടി_IS_EMPTY(scriptRun))
#define മുകൾ_2(scriptRun) ((scriptRun)->paren_അട്ടി[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_എണ്ണം = 0)

ഘടന Paren_അട്ടി_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

ഘടന UScriptRun
{
    int32_t textLength;
    സ്ഥിരം char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    ഘടന Paren_അട്ടി_Entry paren_അട്ടി[PAREN_അട്ടി_DEPTH];
    int32_t parenSP;
    int32_t തള്ളുക_എണ്ണം;
    int32_t fixup_എണ്ണം;
};

സ്ഥിതം int8_t മുകളിലെ_Bit(int32_t value);

സ്ഥിതം സ്ഥിരം UChar32 pairedChars[] = {
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

സ്ഥിതം ശൂന്യം push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->തള്ളുക_എണ്ണം  = LIMIT_INC(scriptRun->തള്ളുക_എണ്ണം);
    scriptRun->fixup_എണ്ണം = LIMIT_INC(scriptRun->fixup_എണ്ണം);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_അട്ടി[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_അട്ടി[scriptRun->parenSP].scriptCode = scriptCode;
}

സ്ഥിതം ശൂന്യം pop(UScriptRun *scriptRun)
{
    എങ്കിൽ (അട്ടി_IS_EMPTY(scriptRun)) {
        തിരികെ;
    }
    
    എങ്കിൽ (scriptRun->fixup_എണ്ണം > 0) {
        scriptRun->fixup_എണ്ണം -= 1;
    }
    
    scriptRun->തള്ളുക_എണ്ണം -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    എങ്കിൽ (അട്ടി_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

സ്ഥിതം ശൂന്യം fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_എണ്ണം);
    
    ആകുമ്പോൾ (scriptRun->fixup_എണ്ണം-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_അട്ടി[fixupSP].scriptCode = scriptCode;
    }
}

സ്ഥിതം int8_t
മുകളിലെ_Bit(int32_t value)
{
    int8_t bit = 0;

    എങ്കിൽ (value <= 0) {
        തിരികെ -32;
    }

    എങ്കിൽ (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    എങ്കിൽ (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    എങ്കിൽ (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    എങ്കിൽ (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    എങ്കിൽ (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    തിരികെ bit;
}

സ്ഥിതം int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_എണ്ണം = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << മുകളിലെ_Bit(paired_Char_എണ്ണം);
    int32_t pairedCharExtra = paired_Char_എണ്ണം - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    എങ്കിൽ (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    ആകുമ്പോൾ (probe > (1 << 0)) {
        probe >>= 1;

        എങ്കിൽ (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    എങ്കിൽ (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    തിരികെ pairIndex;
}

സ്ഥിതം UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    തിരികെ scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(സ്ഥിരം char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    എങ്കിൽ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ nullptr;
    }

    result = (UScriptRun *)uprv_malloc(വലുപ്പം (UScriptRun));

    എങ്കിൽ (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    എങ്കിൽ (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    തിരികെ result;
}

U_CAPI ശൂന്യം U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    എങ്കിൽ (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI ശൂന്യം U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    എങ്കിൽ (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->തള്ളുക_എണ്ണം   =  0;
        scriptRun->fixup_എണ്ണം  =  0;
    }
}

U_CAPI ശൂന്യം U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, സ്ഥിരം char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    എങ്കിൽ (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        തിരികെ;
    }

    എങ്കിൽ (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        തിരികെ;
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
    എങ്കിൽ (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        തിരികെ അസത്യം;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    വേണ്ടി (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t മുകളിലെ = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = മുകളിലെ;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        എങ്കിൽ (മുകളിലെ >= 0xD800 && മുകളിലെ <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t താഴെയുള്ള = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            എങ്കിൽ (താഴെയുള്ള >= 0xDC00 && താഴെയുള്ള <= 0xDFFF) {
                ch = (മുകളിലെ - 0xD800) * 0x0400 + താഴെയുള്ള - 0xDC00 + 0x10000;
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
        എങ്കിൽ (pairIndex >= 0) {
            എങ്കിൽ ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } അല്ലെങ്കിൽ {
                int32_t pi = pairIndex & ~1;

                ആകുമ്പോൾ (അട്ടി_IS_NOT_EMPTY(scriptRun) && മുകൾ_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                എങ്കിൽ (അട്ടി_IS_NOT_EMPTY(scriptRun)) {
                    sc = മുകൾ_2(scriptRun).scriptCode;
                }
            }
        }

        എങ്കിൽ (sameScript(scriptRun->scriptCode, sc)) {
            എങ്കിൽ (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            എങ്കിൽ (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } അല്ലെങ്കിൽ {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            എങ്കിൽ (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            നിർത്തുക;
        }
    }


    എങ്കിൽ (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    എങ്കിൽ (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    എങ്കിൽ (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    തിരികെ സത്യം;
}
