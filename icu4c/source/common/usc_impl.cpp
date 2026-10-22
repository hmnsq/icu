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

#include "unicode/uscript.h"
#include "usc_impl.h"
#include "cmemory.h"

#define PAREN_STACK_DEPTH 32

#define MOD(sp) ((sp) % PAREN_STACK_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_STACK_DEPTH)? (sp) + 1 : PAREN_STACK_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_STACK_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define STACK_IS_EMPTY(scriptRun) ((scriptRun)->pushCount <= 0)
#define STACK_IS_NOT_EMPTY(scriptRun) (! STACK_IS_EMPTY(scriptRun))
#define TOP(scriptRun) ((scriptRun)->parenStack[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixupCount = 0)

struct ParenStackEntry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

struct UScriptRun
{
    int32_t textLength;
    const char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    struct ParenStackEntry parenStack[PAREN_STACK_DEPTH];
    int32_t parenSP;
    int32_t pushCount;
    int32_t fixupCount;
};

static int8_t highBit(int32_t value);

static const UChar32 pairedChars[] = {
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

static void push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->pushCount  = LIMIT_INC(scriptRun->pushCount);
    scriptRun->fixupCount = LIMIT_INC(scriptRun->fixupCount);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->parenStack[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->parenStack[scriptRun->parenSP].scriptCode = scriptCode;
}

static void pop(UScriptRun *scriptRun)
{
    nếu (STACK_IS_EMPTY(scriptRun)) {
        trả_về;
    }
    
    nếu (scriptRun->fixupCount > 0) {
        scriptRun->fixupCount -= 1;
    }
    
    scriptRun->pushCount -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    nếu (STACK_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

static void fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixupCount);
    
    trong_khi (scriptRun->fixupCount-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->parenStack[fixupSP].scriptCode = scriptCode;
    }
}

static int8_t
highBit(int32_t value)
{
    int8_t bit = 0;

    nếu (value <= 0) {
        trả_về -32;
    }

    nếu (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    nếu (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    nếu (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    nếu (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    nếu (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    trả_về bit;
}

static int32_t
getPairIndex(UChar32 ch)
{
    int32_t pairedCharCount = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << highBit(pairedCharCount);
    int32_t pairedCharExtra = pairedCharCount - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    nếu (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    trong_khi (probe > (1 << 0)) {
        probe >>= 1;

        nếu (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    nếu (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    trả_về pairIndex;
}

static UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    trả_về scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(const char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    nếu (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        trả_về nullptr;
    }

    result = (UScriptRun *)uprv_malloc(sizeof (UScriptRun));

    nếu (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        trả_về nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    nếu (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    trả_về result;
}

U_CAPI void U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    nếu (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI void U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    nếu (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->pushCount   =  0;
        scriptRun->fixupCount  =  0;
    }
}

U_CAPI void U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, const char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    nếu (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        trả_về;
    }

    nếu (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        trả_về;
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
    nếu (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        trả_về false;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    cho (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t high = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = high;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        nếu (high >= 0xD800 && high <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t low = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            nếu (low >= 0xDC00 && low <= 0xDFFF) {
                ch = (high - 0xD800) * 0x0400 + low - 0xDC00 + 0x10000;
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
        nếu (pairIndex >= 0) {
            nếu ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } không_thì {
                int32_t pi = pairIndex & ~1;

                trong_khi (STACK_IS_NOT_EMPTY(scriptRun) && TOP(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                nếu (STACK_IS_NOT_EMPTY(scriptRun)) {
                    sc = TOP(scriptRun).scriptCode;
                }
            }
        }

        nếu (sameScript(scriptRun->scriptCode, sc)) {
            nếu (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            nếu (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } không_thì {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            nếu (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            break;
        }
    }


    nếu (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    nếu (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    nếu (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    trả_về true;
}
