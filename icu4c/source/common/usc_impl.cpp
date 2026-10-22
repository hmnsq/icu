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

#அந்தர்பாவ "unicode/uscript.h"
#அந்தர்பாவ "usc_impl.h"
#அந்தர்பாவ "cmemory.h"

#define PAREN_அடுக்கு_DEPTH 32

#define MOD(sp) ((sp) % PAREN_அடுக்கு_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_அடுக்கு_DEPTH)? (sp) + 1 : PAREN_அடுக்கு_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_அடுக்கு_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define அடுக்கு_IS_EMPTY(scriptRun) ((scriptRun)->தள்ளு_மொத்தம் <= 0)
#define அடுக்கு_IS_NOT_EMPTY(scriptRun) (! அடுக்கு_IS_EMPTY(scriptRun))
#define உச்சி_2(scriptRun) ((scriptRun)->paren_அடுக்கு[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_மொத்தம் = 0)

கட்டமைப்பு Paren_அடுக்கு_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

கட்டமைப்பு UScriptRun
{
    int32_t textLength;
    மாறிலி char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    கட்டமைப்பு Paren_அடுக்கு_Entry paren_அடுக்கு[PAREN_அடுக்கு_DEPTH];
    int32_t parenSP;
    int32_t தள்ளு_மொத்தம்;
    int32_t fixup_மொத்தம்;
};

நிலையான int8_t உயர்வு_Bit(int32_t value);

நிலையான மாறிலி UChar32 pairedChars[] = {
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

நிலையான வெறுமை push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->தள்ளு_மொத்தம்  = LIMIT_INC(scriptRun->தள்ளு_மொத்தம்);
    scriptRun->fixup_மொத்தம் = LIMIT_INC(scriptRun->fixup_மொத்தம்);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_அடுக்கு[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_அடுக்கு[scriptRun->parenSP].scriptCode = scriptCode;
}

நிலையான வெறுமை pop(UScriptRun *scriptRun)
{
    எனில் (அடுக்கு_IS_EMPTY(scriptRun)) {
        திரும்பு;
    }
    
    எனில் (scriptRun->fixup_மொத்தம் > 0) {
        scriptRun->fixup_மொத்தம் -= 1;
    }
    
    scriptRun->தள்ளு_மொத்தம் -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    எனில் (அடுக்கு_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

நிலையான வெறுமை fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_மொத்தம்);
    
    போது (scriptRun->fixup_மொத்தம்-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_அடுக்கு[fixupSP].scriptCode = scriptCode;
    }
}

நிலையான int8_t
உயர்வு_Bit(int32_t value)
{
    int8_t bit = 0;

    எனில் (value <= 0) {
        திரும்பு -32;
    }

    எனில் (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    எனில் (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    எனில் (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    எனில் (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    எனில் (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    திரும்பு bit;
}

நிலையான int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_மொத்தம் = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << உயர்வு_Bit(paired_Char_மொத்தம்);
    int32_t pairedCharExtra = paired_Char_மொத்தம் - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    எனில் (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    போது (probe > (1 << 0)) {
        probe >>= 1;

        எனில் (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    எனில் (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    திரும்பு pairIndex;
}

நிலையான UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    திரும்பு scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(மாறிலி char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    எனில் (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு nullptr;
    }

    result = (UScriptRun *)uprv_malloc(அளவு (UScriptRun));

    எனில் (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    எனில் (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    திரும்பு result;
}

U_CAPI வெறுமை U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    எனில் (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI வெறுமை U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    எனில் (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->தள்ளு_மொத்தம்   =  0;
        scriptRun->fixup_மொத்தம்  =  0;
    }
}

U_CAPI வெறுமை U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, மாறிலி char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    எனில் (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        திரும்பு;
    }

    எனில் (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        திரும்பு;
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
    எனில் (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        திரும்பு பொய்;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    க்கு (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t உயர்வு = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = உயர்வு;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        எனில் (உயர்வு >= 0xD800 && உயர்வு <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t கீழ் = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            எனில் (கீழ் >= 0xDC00 && கீழ் <= 0xDFFF) {
                ch = (உயர்வு - 0xD800) * 0x0400 + கீழ் - 0xDC00 + 0x10000;
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
        எனில் (pairIndex >= 0) {
            எனில் ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } இல்லையெனில் {
                int32_t pi = pairIndex & ~1;

                போது (அடுக்கு_IS_NOT_EMPTY(scriptRun) && உச்சி_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                எனில் (அடுக்கு_IS_NOT_EMPTY(scriptRun)) {
                    sc = உச்சி_2(scriptRun).scriptCode;
                }
            }
        }

        எனில் (sameScript(scriptRun->scriptCode, sc)) {
            எனில் (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            எனில் (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } இல்லையெனில் {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            எனில் (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            நிறுத்து;
        }
    }


    எனில் (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    எனில் (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    எனில் (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    திரும்பு உண்மை;
}
