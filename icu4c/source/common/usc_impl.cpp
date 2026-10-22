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

#অন্তর্ভাৱ "unicode/uscript.h"
#অন্তর্ভাৱ "usc_impl.h"
#অন্তর্ভাৱ "cmemory.h"

#define PAREN_স্তূপ_DEPTH 32

#define MOD(sp) ((sp) % PAREN_স্তূপ_DEPTH)
#define LIMIT_INC(sp) (((sp) < PAREN_স্তূপ_DEPTH)? (sp) + 1 : PAREN_স্তূপ_DEPTH)
#define INC(sp,count) (MOD((sp) + (count)))
#define INC1(sp) (INC(sp, 1))
#define DEC(sp,count) (MOD((sp) + PAREN_স্তূপ_DEPTH - (count)))
#define DEC1(sp) (DEC(sp, 1))
#define স্তূপ_IS_EMPTY(scriptRun) ((scriptRun)->ঠেলো_গুনতি <= 0)
#define স্তূপ_IS_NOT_EMPTY(scriptRun) (! স্তূপ_IS_EMPTY(scriptRun))
#define শীৰ্ষ_2(scriptRun) ((scriptRun)->paren_স্তূপ[(scriptRun)->parenSP])
#define SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_গুনতি = 0)

সংরচনা Paren_স্তূপ_Entry
{
    int32_t pairIndex;
    UScriptCode scriptCode;
};

সংরচনা UScriptRun
{
    int32_t textLength;
    ধ্রুৱ char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    সংরচনা Paren_স্তূপ_Entry paren_স্তূপ[PAREN_স্তূপ_DEPTH];
    int32_t parenSP;
    int32_t ঠেলো_গুনতি;
    int32_t fixup_গুনতি;
};

স্থির int8_t উপৰেৰ_Bit(int32_t value);

স্থির ধ্রুৱ UChar32 pairedChars[] = {
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

স্থির শূন্য push(UScriptRun *scriptRun, int32_t pairIndex, UScriptCode scriptCode)
{
    scriptRun->ঠেলো_গুনতি  = LIMIT_INC(scriptRun->ঠেলো_গুনতি);
    scriptRun->fixup_গুনতি = LIMIT_INC(scriptRun->fixup_গুনতি);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_স্তূপ[scriptRun->parenSP].pairIndex  = pairIndex;
    scriptRun->paren_স্তূপ[scriptRun->parenSP].scriptCode = scriptCode;
}

স্থির শূন্য pop(UScriptRun *scriptRun)
{
    যদি (স্তূপ_IS_EMPTY(scriptRun)) {
        ঘূৰাই;
    }
    
    যদি (scriptRun->fixup_গুনতি > 0) {
        scriptRun->fixup_গুনতি -= 1;
    }
    
    scriptRun->ঠেলো_গুনতি -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    যদি (স্তূপ_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

স্থির শূন্য fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_গুনতি);
    
    যেতিয়ালৈ (scriptRun->fixup_গুনতি-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_স্তূপ[fixupSP].scriptCode = scriptCode;
    }
}

স্থির int8_t
উপৰেৰ_Bit(int32_t value)
{
    int8_t bit = 0;

    যদি (value <= 0) {
        ঘূৰাই -32;
    }

    যদি (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    যদি (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    যদি (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    যদি (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    যদি (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    ঘূৰাই bit;
}

স্থির int32_t
getPairIndex(UChar32 ch)
{
    int32_t paired_Char_গুনতি = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << উপৰেৰ_Bit(paired_Char_গুনতি);
    int32_t pairedCharExtra = paired_Char_গুনতি - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pairIndex = 0;

    যদি (ch >= pairedChars[pairedCharExtra]) {
        pairIndex = pairedCharExtra;
    }

    যেতিয়ালৈ (probe > (1 << 0)) {
        probe >>= 1;

        যদি (ch >= pairedChars[pairIndex + probe]) {
            pairIndex += probe;
        }
    }

    যদি (pairedChars[pairIndex] != ch) {
        pairIndex = -1;
    }

    ঘূৰাই pairIndex;
}

স্থির UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    ঘূৰাই scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(ধ্রুৱ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *result = nullptr;

    যদি (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই nullptr;
    }

    result = (UScriptRun *)uprv_malloc(পরিমাণ (UScriptRun));

    যদি (result == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        ঘূৰাই nullptr;
    }

    uscript_setRunText(result, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    যদি (U_FAILURE(*pErrorCode)) {
        uprv_free(result);
        result = nullptr;
    }

    ঘূৰাই result;
}

U_CAPI শূন্য U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    যদি (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI শূন্য U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    যদি (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->ঠেলো_গুনতি   =  0;
        scriptRun->fixup_গুনতি  =  0;
    }
}

U_CAPI শূন্য U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, ধ্রুৱ char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    যদি (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        ঘূৰাই;
    }

    যদি (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        ঘূৰাই;
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
    যদি (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->textLength) {
        ঘূৰাই মিছা;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    বাবে (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->textLength; scriptRun->scriptLimit += 1) {
        char16_t উপৰেৰ = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = উপৰেৰ;
        UScriptCode sc;
        int32_t pairIndex;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        যদি (উপৰেৰ >= 0xD800 && উপৰেৰ <= 0xDBFF && scriptRun->scriptLimit < scriptRun->textLength - 1) {
            char16_t নিচেৰ = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            যদি (নিচেৰ >= 0xDC00 && নিচেৰ <= 0xDFFF) {
                ch = (উপৰেৰ - 0xD800) * 0x0400 + নিচেৰ - 0xDC00 + 0x10000;
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
        যদি (pairIndex >= 0) {
            যদি ((pairIndex & 1) == 0) {
                push(scriptRun, pairIndex, scriptRun->scriptCode);
            } অন্যথা {
                int32_t pi = pairIndex & ~1;

                যেতিয়ালৈ (স্তূপ_IS_NOT_EMPTY(scriptRun) && শীৰ্ষ_2(scriptRun).pairIndex != pi) {
                    pop(scriptRun);
                }

                যদি (স্তূপ_IS_NOT_EMPTY(scriptRun)) {
                    sc = শীৰ্ষ_2(scriptRun).scriptCode;
                }
            }
        }

        যদি (sameScript(scriptRun->scriptCode, sc)) {
            যদি (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            যদি (pairIndex >= 0 && (pairIndex & 1) != 0) {
                pop(scriptRun);
            }
        } অন্যথা {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            যদি (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            ভঙ্গ;
        }
    }


    যদি (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    যদি (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    যদি (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    ঘূৰাই সঁচা;
}
