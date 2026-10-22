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

#define PAREN_चळत_DEPTH 32

#define MOD(sp) ((sp) % PAREN_चळत_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_चळत_DEPTH)? (sp) + 1 : PAREN_चळत_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_चळत_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define चळत_IS_EMPTY(scriptRun) ((scriptRun)->ढकला_मोजणी <= 0)
#define चळत_IS_NOT_EMPTY(scriptRun) (! चळत_IS_EMPTY(scriptRun))
#define शीर्ष_2(scriptRun) ((scriptRun)->paren_चळत[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_मोजणी = 0)

संरचना Paren_चळत_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

संरचना UScriptRun
{
    int32_t textLength;
    स्थिरांक char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    संरचना Paren_चळत_Entry paren_चळत[PAREN_चळत_DEPTH];
    int32_t parenSP;
    int32_t ढकला_मोजणी;
    int32_t fixup_मोजणी;
};

स्थिर int8_t वरचा_Bit(int32_t value);

स्थिर स्थिरांक UChar32 pairedChars[] = {
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
    scriptRun->ढकला_मोजणी  = LIMIT_INC(scriptRun->ढकला_मोजणी);
    scriptRun->fixup_मोजणी = LIMIT_INC(scriptRun->fixup_मोजणी);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_चळत[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_चळत[scriptRun->parenSP].scriptCode = scriptCode;
}

स्थिर शून्य pop(UScriptRun *scriptRun)
{
    जर (चळत_IS_EMPTY(scriptRun)) {
        परत;
    }
    
    जर (scriptRun->fixup_मोजणी > 0) {
        scriptRun->fixup_मोजणी -= 1;
    }
    
    scriptRun->ढकला_मोजणी -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    जर (चळत_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

स्थिर शून्य fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_मोजणी);
    
    पर्यंत (scriptRun->fixup_मोजणी-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_चळत[fixupSP].scriptCode = scriptCode;
    }
}

स्थिर int8_t
वरचा_Bit(int32_t value)
{
    int8_t bit = 0;

    जर (value <= 0) {
        परत -32;
    }

    जर (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    जर (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    जर (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    जर (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    जर (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    परत bit;
}

स्थिर int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_मोजणी = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << वरचा_Bit(paired_Char_मोजणी);
    int32_t pairedCharExtra = paired_Char_मोजणी - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    जर (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    पर्यंत (probe > (1 << 0)) {
        probe >>= 1;

        जर (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    जर (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    परत pairIndex;
}

स्थिर UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    परत scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(स्थिरांक char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    जर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        परत nullptr;
    }

    result = (UScriptRun *)uprv_malloc(आकार (UScriptRun));

    जर (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        परत nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    जर (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    परत result;
}

U_CAPI शून्य U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    जर (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI शून्य U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    जर (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->ढकला_मोजणी   =  0;
        scriptRun->fixup_मोजणी  =  0;
    }
}

U_CAPI शून्य U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, स्थिरांक char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    जर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        परत;
    }

    जर (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        परत;
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
    जर (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        परत असत्य;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    साठी (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t वरचा = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = वरचा;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        जर (वरचा >= 0xD800 && वरचा <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t खालचा = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            जर (खालचा >= 0xDC00 && खालचा <= 0xDFFF) {
                ch = (वरचा - 0xD800) * 0x0400 + खालचा - 0xDC00 + 0x10000;
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
        जर (pairIndex >= 0) {
            जर ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } अन्यथा {
                int32_t pi = pairIndex & ~1;

                पर्यंत (चळत_IS_NOT_EMPTY(scriptRun) && शीर्ष_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                जर (चळत_IS_NOT_EMPTY(scriptRun)) {
                    sc = शीर्ष_2(scriptRun).scriptCode;
                }
            }
        }

        जर (sameScript(scriptRun->scriptCode, sc)) {
            जर (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            जर (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } अन्यथा {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            जर (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            थांबा;
        }
    }


    जर (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    जर (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    जर (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    परत सत्य;
}
