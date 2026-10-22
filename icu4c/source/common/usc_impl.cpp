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

#define PAREN_सालानसान_DEPTH 32

#define MOD(sp) ((sp) % PAREN_सालानसान_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_सालानसान_DEPTH)? (sp) + 1 : PAREN_सालानसान_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_सालानसान_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define सालानसान_IS_EMPTY(scriptRun) ((scriptRun)->इतुलाक_बिलांग <= 0)
#define सालानसान_IS_NOT_EMPTY(scriptRun) (! सालानसान_IS_EMPTY(scriptRun))
#define इताअस_2(scriptRun) ((scriptRun)->paren_सालानसान[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_बिलांग = 0)

struct Paren_सालानसान_Entry
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

    struct Paren_सालानसान_Entry paren_सालानसान[PAREN_सालानसान_DEPTH];
    int32_t parenSP;
    int32_t इतुलाक_बिलांग;
    int32_t fixup_बिलांग;
};

static int8_t माताअस_Bit(int32_t value);

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
    scriptRun->इतुलाक_बिलांग  = LIMIT_INC(scriptRun->इतुलाक_बिलांग);
    scriptRun->fixup_बिलांग = LIMIT_INC(scriptRun->fixup_बिलांग);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_सालानसान[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_सालानसान[scriptRun->parenSP].scriptCode = scriptCode;
}

static void pop(UScriptRun *scriptRun)
{
    if (सालानसान_IS_EMPTY(scriptRun)) {
        return;
    }
    
    if (scriptRun->fixup_बिलांग > 0) {
        scriptRun->fixup_बिलांग -= 1;
    }
    
    scriptRun->इतुलाक_बिलांग -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    if (सालानसान_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

static void fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_बिलांग);
    
    while (scriptRun->fixup_बिलांग-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_सालानसान[fixupSP].scriptCode = scriptCode;
    }
}

static int8_t
माताअस_Bit(int32_t value)
{
    int8_t bit = 0;

    if (value <= 0) {
        return -32;
    }

    if (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    if (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    if (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    if (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    if (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    return bit;
}

static int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_बिलांग = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << माताअस_Bit(paired_Char_बिलांग);
    int32_t pairedCharExtra = paired_Char_बिलांग - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    if (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    while (probe > (1 << 0)) {
        probe >>= 1;

        if (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    if (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    return pairIndex;
}

static UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    return scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(const char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    if (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        return nullptr;
    }

    result = (UScriptRun *)uprv_malloc(sizeof (UScriptRun));

    if (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    if (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    return result;
}

U_CAPI void U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    if (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI void U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    if (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->इतुलाक_बिलांग   =  0;
        scriptRun->fixup_बिलांग  =  0;
    }
}

U_CAPI void U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, const char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    if (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        return;
    }

    if (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        return;
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
    if (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        return false;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    for (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t माताअस = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = माताअस;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        if (माताअस >= 0xD800 && माताअस <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t माबाबा = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            if (माबाबा >= 0xDC00 && माबाबा <= 0xDFFF) {
                ch = (माताअस - 0xD800) * 0x0400 + माबाबा - 0xDC00 + 0x10000;
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
        if (pairIndex >= 0) {
            if ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } else {
                int32_t pi = pairIndex & ~1;

                while (सालानसान_IS_NOT_EMPTY(scriptRun) && इताअस_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                if (सालानसान_IS_NOT_EMPTY(scriptRun)) {
                    sc = इताअस_2(scriptRun).scriptCode;
                }
            }
        }

        if (sameScript(scriptRun->scriptCode, sc)) {
            if (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            if (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } else {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            if (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            break;
        }
    }


    if (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    if (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    if (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    return true;
}
