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

#समावेश "unicode/uscript.h"
#समावेश "usc_impl.h"
#समावेश "cmemory.h"

#घोषणा PAREN_ढेर_DEPTH 32

#घोषणा MOD(sp) ((sp) % PAREN_ढेर_DEPTH)
#घोषणा LIMIT_INC(sp) (((sp) < PAREN_ढेर_DEPTH)? (sp) + 1 : PAREN_ढेर_DEPTH)
#घोषणा INC(sp,count) (MOD((sp) + (count)))
#घोषणा INC1(sp) (INC(sp, 1))
#घोषणा DEC(sp,count) (MOD((sp) + PAREN_ढेर_DEPTH - (count)))
#घोषणा DEC1(sp) (DEC(sp, 1))
#घोषणा ढेर_IS_EMPTY(scriptRun) ((scriptRun)->धकेलो_गिनती <= 0)
#घोषणा ढेर_IS_NOT_EMPTY(scriptRun) (! ढेर_IS_EMPTY(scriptRun))
#घोषणा शीर्ष_3(scriptRun) ((scriptRun)->paren_ढेर[(scriptRun)->parenSP])
#घोषणा SYNC_FIXUP(scriptRun) ((scriptRun)->fixup_गिनती = 0)

काष्ठा Paren_ढेर_Entry
{
    int32_t pair_सूचकांक;
    UScriptCode scriptCode;
};

काष्ठा UScriptRun
{
    int32_t text_लंबाई;
    स्थिर char16_t *textArray;

    int32_t scriptStart;
    int32_t scriptLimit;
    UScriptCode scriptCode;

    काष्ठा Paren_ढेर_Entry paren_ढेर[PAREN_ढेर_DEPTH];
    int32_t parenSP;
    int32_t धकेलो_गिनती;
    int32_t fixup_गिनती;
};

अटल int8_t ऊपरी_Bit(int32_t value);

अटल स्थिर UChar32 pairedChars[] = {
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

अटल व्योम push(UScriptRun *scriptRun, int32_t pair_सूचकांक, UScriptCode scriptCode)
{
    scriptRun->धकेलो_गिनती  = LIMIT_INC(scriptRun->धकेलो_गिनती);
    scriptRun->fixup_गिनती = LIMIT_INC(scriptRun->fixup_गिनती);
    
    scriptRun->parenSP = INC1(scriptRun->parenSP);
    scriptRun->paren_ढेर[scriptRun->parenSP].pair_सूचकांक  = pair_सूचकांक;
    scriptRun->paren_ढेर[scriptRun->parenSP].scriptCode = scriptCode;
}

अटल व्योम pop(UScriptRun *scriptRun)
{
    अगर (ढेर_IS_EMPTY(scriptRun)) {
        वापस;
    }
    
    अगर (scriptRun->fixup_गिनती > 0) {
        scriptRun->fixup_गिनती -= 1;
    }
    
    scriptRun->धकेलो_गिनती -= 1;
    scriptRun->parenSP = DEC1(scriptRun->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    अगर (ढेर_IS_EMPTY(scriptRun)) {
        scriptRun->parenSP = -1;
    }
}

अटल व्योम fixup(UScriptRun *scriptRun, UScriptCode scriptCode)
{
    int32_t fixupSP = DEC(scriptRun->parenSP, scriptRun->fixup_गिनती);
    
    जबतक (scriptRun->fixup_गिनती-- > 0) {
        fixupSP = INC1(fixupSP);
        scriptRun->paren_ढेर[fixupSP].scriptCode = scriptCode;
    }
}

अटल int8_t
ऊपरी_Bit(int32_t value)
{
    int8_t bit = 0;

    अगर (value <= 0) {
        वापस -32;
    }

    अगर (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    अगर (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    अगर (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    अगर (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    अगर (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    वापस bit;
}

अटल int32_t
get_Pair_सूचकांक(UChar32 ch)
{
    int32_t paired_Char_गिनती = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << ऊपरी_Bit(paired_Char_गिनती);
    int32_t pairedCharExtra = paired_Char_गिनती - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_सूचकांक = 0;

    अगर (ch >= pairedChars[pairedCharExtra]) {
        pair_सूचकांक = pairedCharExtra;
    }

    जबतक (probe > (1 << 0)) {
        probe >>= 1;

        अगर (ch >= pairedChars[pair_सूचकांक + probe]) {
            pair_सूचकांक += probe;
        }
    }

    अगर (pairedChars[pair_सूचकांक] != ch) {
        pair_सूचकांक = -1;
    }

    वापस pair_सूचकांक;
}

अटल UBool
sameScript(UScriptCode scriptOne, UScriptCode scriptTwo)
{
    वापस scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScriptRun * U_EXPORT2
uscript_openRun(स्थिर char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    UScriptRun *परिणाम = nullptr;

    अगर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        वापस nullptr;
    }

    परिणाम = (UScriptRun *)uprv_malloc(माप (UScriptRun));

    अगर (परिणाम == nullptr) {
        *pErrorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस nullptr;
    }

    uscript_setRunText(परिणाम, src, length, pErrorCode);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    अगर (U_FAILURE(*pErrorCode)) {
        uprv_free(परिणाम);
        परिणाम = nullptr;
    }

    वापस परिणाम;
}

U_CAPI व्योम U_EXPORT2
uscript_closeRun(UScriptRun *scriptRun)
{
    अगर (scriptRun != nullptr) {
        uprv_free(scriptRun);
    }
}

U_CAPI व्योम U_EXPORT2
uscript_resetRun(UScriptRun *scriptRun)
{
    अगर (scriptRun != nullptr) {
        scriptRun->scriptStart = 0;
        scriptRun->scriptLimit = 0;
        scriptRun->scriptCode  = USCRIPT_INVALID_CODE;
        scriptRun->parenSP     = -1;
        scriptRun->धकेलो_गिनती   =  0;
        scriptRun->fixup_गिनती  =  0;
    }
}

U_CAPI व्योम U_EXPORT2
uscript_setRunText(UScriptRun *scriptRun, स्थिर char16_t *src, int32_t length, UErrorCode *pErrorCode)
{
    अगर (pErrorCode == nullptr || U_FAILURE(*pErrorCode)) {
        वापस;
    }

    अगर (scriptRun == nullptr || length < 0 || ((src == nullptr) != (length == 0))) {
        *pErrorCode = U_ILLEGAL_ARGUMENT_ERROR;
        वापस;
    }

    scriptRun->textArray  = src;
    scriptRun->text_लंबाई = length;

    uscript_resetRun(scriptRun);
}

U_CAPI UBool U_EXPORT2
uscript_अग्र_Run(UScriptRun *scriptRun, int32_t *pRunStart, int32_t *pRunLimit, UScriptCode *pRunScript)
{
    UErrorCode error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    अगर (scriptRun == nullptr || scriptRun->scriptLimit >= scriptRun->text_लंबाई) {
        वापस false;
    }
    
    SYNC_FIXUP(scriptRun);
    scriptRun->scriptCode = USCRIPT_COMMON;

    क्रम (scriptRun->scriptStart = scriptRun->scriptLimit; scriptRun->scriptLimit < scriptRun->text_लंबाई; scriptRun->scriptLimit += 1) {
        char16_t ऊपरी = scriptRun->textArray[scriptRun->scriptLimit];
        UChar32  ch   = ऊपरी;
        UScriptCode sc;
        int32_t pair_सूचकांक;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        अगर (ऊपरी >= 0xD800 && ऊपरी <= 0xDBFF && scriptRun->scriptLimit < scriptRun->text_लंबाई - 1) {
            char16_t निचला = scriptRun->textArray[scriptRun->scriptLimit + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            अगर (निचला >= 0xDC00 && निचला <= 0xDFFF) {
                ch = (ऊपरी - 0xD800) * 0x0400 + निचला - 0xDC00 + 0x10000;
                scriptRun->scriptLimit += 1;
            }
        }

        sc = uscript_getScript(ch, &error);
        pair_सूचकांक = get_Pair_सूचकांक(ch);

        /*
         * Paired character handling:
         *
         * if it's an open character, push it onto the stack.
         * if it's a close character, find the matching open on the
         * stack, and use that script code. Any non-matching open
         * characters above it on the stack will be poped.
         */
        अगर (pair_सूचकांक >= 0) {
            अगर ((pair_सूचकांक & 1) == 0) {
                push(scriptRun, pair_सूचकांक, scriptRun->scriptCode);
            } अन्यथा {
                int32_t pi = pair_सूचकांक & ~1;

                जबतक (ढेर_IS_NOT_EMPTY(scriptRun) && शीर्ष_3(scriptRun).pair_सूचकांक != pi) {
                    pop(scriptRun);
                }

                अगर (ढेर_IS_NOT_EMPTY(scriptRun)) {
                    sc = शीर्ष_3(scriptRun).scriptCode;
                }
            }
        }

        अगर (sameScript(scriptRun->scriptCode, sc)) {
            अगर (scriptRun->scriptCode <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                scriptRun->scriptCode = sc;

                fixup(scriptRun, scriptRun->scriptCode);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            अगर (pair_सूचकांक >= 0 && (pair_सूचकांक & 1) != 0) {
                pop(scriptRun);
            }
        } अन्यथा {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            अगर (ch >= 0x10000) {
                scriptRun->scriptLimit -= 1;
            }

            अवरोध;
        }
    }


    अगर (pRunStart != nullptr) {
        *pRunStart = scriptRun->scriptStart;
    }

    अगर (pRunLimit != nullptr) {
        *pRunLimit = scriptRun->scriptLimit;
    }

    अगर (pRunScript != nullptr) {
        *pRunScript = scriptRun->scriptCode;
    }

    वापस true;
}
