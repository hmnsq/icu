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

#ضمن "unicode/uscript.h"
#ضمن "usc_impl.h"
#ضمن "cmemory.h"

#define PAREN_دۆۋە_DEPTH 32

#define MOD(sp) ((sp) % PAREN_دۆۋە_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_دۆۋە_DEPTH)? (sp) + 1 : PAREN_دۆۋە_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_دۆۋە_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define دۆۋە_IS_EMPTY(scriptRun) ((scriptRun)->ئىتتىرىش_سانى <= 0)
#define دۆۋە_IS_NOT_EMPTY(scriptRun) (! دۆۋە_IS_EMPTY(scriptRun))
#define چوققا_2(scriptRun) ((scriptRun)->paren_دۆۋە[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_سانى = 0)

بنية Paren_دۆۋە_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

بنية UScriptRun
{
    int32_t textLength;
    ثابت char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    بنية Paren_دۆۋە_Entry paren_دۆۋە[PAREN_دۆۋە_DEPTH];
    int32_t parenSP;
    int32_t ئىتتىرىش_سانى;
    int32_t fixup_سانى;
};

ساكن int8_t يۇقىرى_Bit(int32_t value);

ساكن ثابت UChar32 pairedChars[] = {
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

ساكن فراغ push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->ئىتتىرىش_سانى  = LIMIT_INC(scriptRun->ئىتتىرىش_سانى);
    scriptRun->fixup_سانى = LIMIT_INC(scriptRun->fixup_سانى);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_دۆۋە[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_دۆۋە[scriptRun->parenSP].scriptCode = scriptCode;
}

ساكن فراغ pop(UScriptRun *scriptRun)
{
    إذا (دۆۋە_IS_EMPTY(scriptRun)) {
        أرجع;
    }
    
    إذا (scriptRun->fixup_سانى > 0) {
        scriptRun->fixup_سانى -= 1;
    }
    
    scriptRun->ئىتتىرىش_سانى -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    إذا (دۆۋە_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

ساكن فراغ fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_سانى);
    
    طالما (scriptRun->fixup_سانى-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_دۆۋە[fixupSP].scriptCode = scriptCode;
    }
}

ساكن int8_t
يۇقىرى_Bit(int32_t value)
{
    int8_t bit = 0;

    إذا (value <= 0) {
        أرجع -32;
    }

    إذا (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    إذا (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    إذا (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    إذا (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    إذا (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    أرجع bit;
}

ساكن int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_سانى = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << يۇقىرى_Bit(paired_Char_سانى);
    int32_t pairedCharExtra = paired_Char_سانى - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    إذا (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    طالما (probe > (1 << 0)) {
        probe >>= 1;

        إذا (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    إذا (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    أرجع pairIndex;
}

ساكن UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    أرجع scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(ثابت char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    إذا (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        أرجع nullptr;
    }

    result = (UScriptRun *)uprv_malloc(حجم (UScriptRun));

    إذا (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        أرجع nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    إذا (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    أرجع result;
}

U_CAPI فراغ U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    إذا (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI فراغ U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    إذا (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->ئىتتىرىش_سانى   =  0;
        scriptRun->fixup_سانى  =  0;
    }
}

U_CAPI فراغ U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, ثابت char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    إذا (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        أرجع;
    }

    إذا (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        أرجع;
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
    إذا (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        أرجع خطأ;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    لكل (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t يۇقىرى = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = يۇقىرى;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        إذا (يۇقىرى >= 0xD800 && يۇقىرى <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t تۆۋەن = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            إذا (تۆۋەن >= 0xDC00 && تۆۋەن <= 0xDFFF) {
                ch = (يۇقىرى - 0xD800) * 0x0400 + تۆۋەن - 0xDC00 + 0x10000;
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
        إذا (pairIndex >= 0) {
            إذا ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } وإلا {
                int32_t pi = pairIndex & ~1;

                طالما (دۆۋە_IS_NOT_EMPTY(scriptRun) && چوققا_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                إذا (دۆۋە_IS_NOT_EMPTY(scriptRun)) {
                    sc = چوققا_2(scriptRun).scriptCode;
                }
            }
        }

        إذا (sameScript(scriptRun->scriptCode, sc)) {
            إذا (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            إذا (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } وإلا {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            إذا (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            اكسر;
        }
    }


    إذا (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    إذا (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    إذا (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    أرجع true;
}
