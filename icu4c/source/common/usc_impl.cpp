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

#define PAREN_پشته_DEPTH 32

#define MOD(sp) ((sp) % PAREN_پشته_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_پشته_DEPTH)? (sp) + 1 : PAREN_پشته_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_پشته_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define پشته_IS_EMPTY(scriptRun) ((scriptRun)->افزودن_شمار <= 0)
#define پشته_IS_NOT_EMPTY(scriptRun) (! پشته_IS_EMPTY(scriptRun))
#define بالاترین_2(scriptRun) ((scriptRun)->paren_پشته[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_شمار = 0)

ساختار Paren_پشته_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

ساختار UScriptRun
{
    int32_t textLength;
    ثابت char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    ساختار Paren_پشته_Entry paren_پشته[PAREN_پشته_DEPTH];
    int32_t parenSP;
    int32_t افزودن_شمار;
    int32_t fixup_شمار;
};

ایستا int8_t بالا_Bit(int32_t value);

ایستا ثابت UChar32 pairedChars[] = {
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

ایستا تهی push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->افزودن_شمار  = LIMIT_INC(scriptRun->افزودن_شمار);
    scriptRun->fixup_شمار = LIMIT_INC(scriptRun->fixup_شمار);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_پشته[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_پشته[scriptRun->parenSP].scriptCode = scriptCode;
}

ایستا تهی pop(UScriptRun *scriptRun)
{
    اگر (پشته_IS_EMPTY(scriptRun)) {
        بازگشت;
    }
    
    اگر (scriptRun->fixup_شمار > 0) {
        scriptRun->fixup_شمار -= 1;
    }
    
    scriptRun->افزودن_شمار -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    اگر (پشته_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

ایستا تهی fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_شمار);
    
    وقتی (scriptRun->fixup_شمار-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_پشته[fixupSP].scriptCode = scriptCode;
    }
}

ایستا int8_t
بالا_Bit(int32_t value)
{
    int8_t bit = 0;

    اگر (value <= 0) {
        بازگشت -32;
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

    بازگشت bit;
}

ایستا int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_شمار = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << بالا_Bit(paired_Char_شمار);
    int32_t pairedCharExtra = paired_Char_شمار - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    اگر (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    وقتی (probe > (1 << 0)) {
        probe >>= 1;

        اگر (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    اگر (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    بازگشت pairIndex;
}

ایستا UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    بازگشت scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(ثابت char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    اگر (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت nullptr;
    }

    result = (UScriptRun *)uprv_malloc(اندازه (UScriptRun));

    اگر (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        بازگشت nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    اگر (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    بازگشت result;
}

U_CAPI تهی U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    اگر (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI تهی U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    اگر (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->افزودن_شمار   =  0;
        scriptRun->fixup_شمار  =  0;
    }
}

U_CAPI تهی U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, ثابت char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    اگر (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        بازگشت;
    }

    اگر (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        بازگشت;
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
    اگر (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        بازگشت نادرست;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    برای (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t بالا = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = بالا;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        اگر (بالا >= 0xD800 && بالا <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t کم = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            اگر (کم >= 0xDC00 && کم <= 0xDFFF) {
                ch = (بالا - 0xD800) * 0x0400 + کم - 0xDC00 + 0x10000;
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
        اگر (pairIndex >= 0) {
            اگر ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } وگرنه {
                int32_t pi = pairIndex & ~1;

                وقتی (پشته_IS_NOT_EMPTY(scriptRun) && بالاترین_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                اگر (پشته_IS_NOT_EMPTY(scriptRun)) {
                    sc = بالاترین_2(scriptRun).scriptCode;
                }
            }
        }

        اگر (sameScript(scriptRun->scriptCode, sc)) {
            اگر (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            اگر (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } وگرنه {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            اگر (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            بشکن;
        }
    }


    اگر (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    اگر (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    اگر (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    بازگشت درست;
}
