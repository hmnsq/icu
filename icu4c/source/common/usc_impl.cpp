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

#अन्तर्भाव "unicode/uscript.h"
#अन्तर्भाव "usc_impl.h"
#अन्तर्भाव "cmemory.h"

#define PAREN_राशिः_DEPTH 32

#define MOD(sp) ((sp) % PAREN_राशिः_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_राशिः_DEPTH)? (sp) + 1 : PAREN_राशिः_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_राशिः_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define राशिः_IS_EMPTY(scriptRun) ((scriptRun)->प्रक्षिप_गणनम् <= 0)
#define राशिः_IS_NOT_EMPTY(scriptRun) (! राशिः_IS_EMPTY(scriptRun))
#define शिखरम्_2(scriptRun) ((scriptRun)->paren_राशिः[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_गणनम् = 0)

संरचना Paren_राशिः_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

संरचना UScriptRun
{
    int32_t textLength;
    ध्रुव char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    संरचना Paren_राशिः_Entry paren_राशिः[PAREN_राशिः_DEPTH];
    int32_t parenSP;
    int32_t प्रक्षिप_गणनम्;
    int32_t fixup_गणनम्;
};

स्थिर int8_t उच्चः_Bit(int32_t value);

स्थिर ध्रुव UChar32 pairedChars[] = {
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

स्थिर शून्य push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->प्रक्षिप_गणनम्  = LIMIT_INC(scriptRun->प्रक्षिप_गणनम्);
    scriptRun->fixup_गणनम् = LIMIT_INC(scriptRun->fixup_गणनम्);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_राशिः[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_राशिः[scriptRun->parenSP].scriptCode = scriptCode;
}

स्थिर शून्य pop(UScriptRun *scriptRun)
{
    सचे (राशिः_IS_EMPTY(scriptRun)) {
        पटिनिवत्त;
    }
    
    सचे (scriptRun->fixup_गणनम् > 0) {
        scriptRun->fixup_गणनम् -= 1;
    }
    
    scriptRun->प्रक्षिप_गणनम् -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    सचे (राशिः_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

स्थिर शून्य fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_गणनम्);
    
    याव (scriptRun->fixup_गणनम्-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_राशिः[fixupSP].scriptCode = scriptCode;
    }
}

स्थिर int8_t
उच्चः_Bit(int32_t value)
{
    int8_t bit = 0;

    सचे (value <= 0) {
        पटिनिवत्त -32;
    }

    सचे (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    सचे (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    सचे (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    सचे (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    सचे (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    पटिनिवत्त bit;
}

स्थिर int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_गणनम् = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << उच्चः_Bit(paired_Char_गणनम्);
    int32_t pairedCharExtra = paired_Char_गणनम् - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    सचे (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    याव (probe > (1 << 0)) {
        probe >>= 1;

        सचे (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    सचे (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    पटिनिवत्त pairIndex;
}

स्थिर UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    पटिनिवत्त scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(ध्रुव char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    सचे (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        पटिनिवत्त nullptr;
    }

    result = (UScriptRun *)uprv_malloc(परिमाण (UScriptRun));

    सचे (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        पटिनिवत्त nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    सचे (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    पटिनिवत्त result;
}

U_CAPI शून्य U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    सचे (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI शून्य U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    सचे (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->प्रक्षिप_गणनम्   =  0;
        scriptRun->fixup_गणनम्  =  0;
    }
}

U_CAPI शून्य U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, ध्रुव char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    सचे (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        पटिनिवत्त;
    }

    सचे (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        पटिनिवत्त;
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
    सचे (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        पटिनिवत्त मिच्छा;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    अत्थाय (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t उच्चः = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = उच्चः;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        सचे (उच्चः >= 0xD800 && उच्चः <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t नीचः = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            सचे (नीचः >= 0xDC00 && नीचः <= 0xDFFF) {
                ch = (उच्चः - 0xD800) * 0x0400 + नीचः - 0xDC00 + 0x10000;
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
        सचे (pairIndex >= 0) {
            सचे ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } अञ्ञथा {
                int32_t pi = pairIndex & ~1;

                याव (राशिः_IS_NOT_EMPTY(scriptRun) && शिखरम्_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                सचे (राशिः_IS_NOT_EMPTY(scriptRun)) {
                    sc = शिखरम्_2(scriptRun).scriptCode;
                }
            }
        }

        सचे (sameScript(scriptRun->scriptCode, sc)) {
            सचे (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            सचे (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } अञ्ञथा {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            सचे (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            भङ्ग;
        }
    }


    सचे (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    सचे (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    सचे (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    पटिनिवत्त सच्च;
}
