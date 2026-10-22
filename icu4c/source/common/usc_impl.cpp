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

#परिभाषय PAREN_राशिः_गभीरता 32

#परिभाषय MOD(sp) ((sp) % PAREN_राशिः_गभीरता)
#परिभाषय सीमा_INC(sp) (((sp) < PAREN_राशिः_गभीरता)? (sp) + 1 : PAREN_राशिः_गभीरता)
#परिभाषय INC(sp,count) (MOD((sp) + (count)))
#परिभाषय INC1(sp) (INC(sp, 1))
#परिभाषय DEC(sp,count) (MOD((sp) + PAREN_राशिः_गभीरता - (count)))
#परिभाषय DEC1(sp) (DEC(sp, 1))
#परिभाषय राशिः_IS_रिक्तानि(script_सततम्) ((script_सततम्)->प्रक्षिप_गणनम् <= 0)
#परिभाषय राशिः_IS_NOT_रिक्तानि(script_सततम्) (! राशिः_IS_रिक्तानि(script_सततम्))
#परिभाषय शिखरम्_2(script_सततम्) ((script_सततम्)->paren_राशिः[(script_सततम्)->parenSP])
#परिभाषय SYNC_FIXUP(script_सततम्) ((script_सततम्)->fixup_गणनम् = 0)

संरचना Paren_राशिः_प्रविष्टिः
{
    int32_t pair_सूचकांक;
    UScript_संहिता script_संहिता;
};

संरचना UScript_सततम्
{
    int32_t text_दीर्घता;
    ध्रुव char16_t *textArray;

    int32_t scriptStart;
    int32_t script_सीमा;
    UScript_संहिता script_संहिता;

    संरचना Paren_राशिः_प्रविष्टिः paren_राशिः[PAREN_राशिः_गभीरता];
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

स्थिर शून्य push(UScript_सततम् *script_सततम्, int32_t pair_सूचकांक, UScript_संहिता script_संहिता)
{
    script_सततम्->प्रक्षिप_गणनम्  = सीमा_INC(script_सततम्->प्रक्षिप_गणनम्);
    script_सततम्->fixup_गणनम् = सीमा_INC(script_सततम्->fixup_गणनम्);
    
    script_सततम्->parenSP = INC1(script_सततम्->parenSP);
    script_सततम्->paren_राशिः[script_सततम्->parenSP].pair_सूचकांक  = pair_सूचकांक;
    script_सततम्->paren_राशिः[script_सततम्->parenSP].script_संहिता = script_संहिता;
}

स्थिर शून्य pop(UScript_सततम् *script_सततम्)
{
    यदि (राशिः_IS_रिक्तानि(script_सततम्)) {
        प्रत्यागम;
    }
    
    यदि (script_सततम्->fixup_गणनम् > 0) {
        script_सततम्->fixup_गणनम् -= 1;
    }
    
    script_सततम्->प्रक्षिप_गणनम् -= 1;
    script_सततम्->parenSP = DEC1(script_सततम्->parenSP);
    
    /* If the stack is now empty, reset the stack
       pointers to their initial values.
     */
    यदि (राशिः_IS_रिक्तानि(script_सततम्)) {
        script_सततम्->parenSP = -1;
    }
}

स्थिर शून्य fixup(UScript_सततम् *script_सततम्, UScript_संहिता script_संहिता)
{
    int32_t fixupSP = DEC(script_सततम्->parenSP, script_सततम्->fixup_गणनम्);
    
    यावत् (script_सततम्->fixup_गणनम्-- > 0) {
        fixupSP = INC1(fixupSP);
        script_सततम्->paren_राशिः[fixupSP].script_संहिता = script_संहिता;
    }
}

स्थिर int8_t
उच्चः_Bit(int32_t value)
{
    int8_t bit = 0;

    यदि (value <= 0) {
        प्रत्यागम -32;
    }

    यदि (value >= 1 << 16) {
        value >>= 16;
        bit += 16;
    }

    यदि (value >= 1 << 8) {
        value >>= 8;
        bit += 8;
    }

    यदि (value >= 1 << 4) {
        value >>= 4;
        bit += 4;
    }

    यदि (value >= 1 << 2) {
        value >>= 2;
        bit += 2;
    }

    यदि (value >= 1 << 1) {
        //value >>= 1;
        bit += 1;
    }

    प्रत्यागम bit;
}

स्थिर int32_t
get_Pair_सूचकांक(UChar32 ch)
{
    int32_t paired_Char_गणनम् = UPRV_LENGTHOF(pairedChars);
    int32_t pairedCharPower = 1 << उच्चः_Bit(paired_Char_गणनम्);
    int32_t pairedCharExtra = paired_Char_गणनम् - pairedCharPower;

    int32_t probe = pairedCharPower;
    int32_t pair_सूचकांक = 0;

    यदि (ch >= pairedChars[pairedCharExtra]) {
        pair_सूचकांक = pairedCharExtra;
    }

    यावत् (probe > (1 << 0)) {
        probe >>= 1;

        यदि (ch >= pairedChars[pair_सूचकांक + probe]) {
            pair_सूचकांक += probe;
        }
    }

    यदि (pairedChars[pair_सूचकांक] != ch) {
        pair_सूचकांक = -1;
    }

    प्रत्यागम pair_सूचकांक;
}

स्थिर UBool
sameScript(UScript_संहिता scriptOne, UScript_संहिता scriptTwo)
{
    प्रत्यागम scriptOne <= USCRIPT_INHERITED || scriptTwo <= USCRIPT_INHERITED || scriptOne == scriptTwo;
}

U_CAPI UScript_सततम् * U_EXPORT2
uscript_open_सततम्(ध्रुव char16_t *src, int32_t length, UError_संहिता *p_Error_संहिता)
{
    UScript_सततम् *परिणाम = शून्यसूचकः;

    यदि (p_Error_संहिता == शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम शून्यसूचकः;
    }

    परिणाम = (UScript_सततम् *)uprv_malloc(परिमाण (UScript_सततम्));

    यदि (परिणाम == शून्यसूचकः) {
        *p_Error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम शून्यसूचकः;
    }

    uscript_समुच्चयः_सततम्_Text(परिणाम, src, length, p_Error_संहिता);

    /* Release the UScriptRun if uscript_setRunText() returns an error */
    यदि (U_FAILURE(*p_Error_संहिता)) {
        uprv_free(परिणाम);
        परिणाम = शून्यसूचकः;
    }

    प्रत्यागम परिणाम;
}

U_CAPI शून्य U_EXPORT2
uscript_close_सततम्(UScript_सततम् *script_सततम्)
{
    यदि (script_सततम् != शून्यसूचकः) {
        uprv_free(script_सततम्);
    }
}

U_CAPI शून्य U_EXPORT2
uscript_reset_सततम्(UScript_सततम् *script_सततम्)
{
    यदि (script_सततम् != शून्यसूचकः) {
        script_सततम्->scriptStart = 0;
        script_सततम्->script_सीमा = 0;
        script_सततम्->script_संहिता  = USCRIPT_INVALID_संहिता;
        script_सततम्->parenSP     = -1;
        script_सततम्->प्रक्षिप_गणनम्   =  0;
        script_सततम्->fixup_गणनम्  =  0;
    }
}

U_CAPI शून्य U_EXPORT2
uscript_समुच्चयः_सततम्_Text(UScript_सततम् *script_सततम्, ध्रुव char16_t *src, int32_t length, UError_संहिता *p_Error_संहिता)
{
    यदि (p_Error_संहिता == शून्यसूचकः || U_FAILURE(*p_Error_संहिता)) {
        प्रत्यागम;
    }

    यदि (script_सततम् == शून्यसूचकः || length < 0 || ((src == शून्यसूचकः) != (length == 0))) {
        *p_Error_संहिता = U_ILLEGAL_तर्कः_ERROR;
        प्रत्यागम;
    }

    script_सततम्->textArray  = src;
    script_सततम्->text_दीर्घता = length;

    uscript_reset_सततम्(script_सततम्);
}

U_CAPI UBool U_EXPORT2
uscript_अग्र_सततम्(UScript_सततम् *script_सततम्, int32_t *p_सततम्_Start, int32_t *p_सततम्_सीमा, UScript_संहिता *p_सततम्_Script)
{
    UError_संहिता error = U_ZERO_ERROR;

    /* if we've fallen off the end of the text, we're done */
    यदि (script_सततम् == शून्यसूचकः || script_सततम्->script_सीमा >= script_सततम्->text_दीर्घता) {
        प्रत्यागम असत्य;
    }
    
    SYNC_FIXUP(script_सततम्);
    script_सततम्->script_संहिता = USCRIPT_COMMON;

    प्रति (script_सततम्->scriptStart = script_सततम्->script_सीमा; script_सततम्->script_सीमा < script_सततम्->text_दीर्घता; script_सततम्->script_सीमा += 1) {
        char16_t उच्चः = script_सततम्->textArray[script_सततम्->script_सीमा];
        UChar32  ch   = उच्चः;
        UScript_संहिता sc;
        int32_t pair_सूचकांक;

        /*
         * if the character is a high surrogate and it's not the last one
         * in the text, see if it's followed by a low surrogate
         */
        यदि (उच्चः >= 0xD800 && उच्चः <= 0xDBFF && script_सततम्->script_सीमा < script_सततम्->text_दीर्घता - 1) {
            char16_t नीचः = script_सततम्->textArray[script_सततम्->script_सीमा + 1];

            /*
             * if it is followed by a low surrogate,
             * consume it and form the full character
             */
            यदि (नीचः >= 0xDC00 && नीचः <= 0xDFFF) {
                ch = (उच्चः - 0xD800) * 0x0400 + नीचः - 0xDC00 + 0x10000;
                script_सततम्->script_सीमा += 1;
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
        यदि (pair_सूचकांक >= 0) {
            यदि ((pair_सूचकांक & 1) == 0) {
                push(script_सततम्, pair_सूचकांक, script_सततम्->script_संहिता);
            } अन्यथा {
                int32_t pi = pair_सूचकांक & ~1;

                यावत् (राशिः_IS_NOT_रिक्तानि(script_सततम्) && शिखरम्_2(script_सततम्).pair_सूचकांक != pi) {
                    pop(script_सततम्);
                }

                यदि (राशिः_IS_NOT_रिक्तानि(script_सततम्)) {
                    sc = शिखरम्_2(script_सततम्).script_संहिता;
                }
            }
        }

        यदि (sameScript(script_सततम्->script_संहिता, sc)) {
            यदि (script_सततम्->script_संहिता <= USCRIPT_INHERITED && sc > USCRIPT_INHERITED) {
                script_सततम्->script_संहिता = sc;

                fixup(script_सततम्, script_सततम्->script_संहिता);
            }

            /*
             * if this character is a close paired character,
             * pop the matching open character from the stack
             */
            यदि (pair_सूचकांक >= 0 && (pair_सूचकांक & 1) != 0) {
                pop(script_सततम्);
            }
        } अन्यथा {
            /*
             * if the run broke on a surrogate pair,
             * end it before the high surrogate
             */
            यदि (ch >= 0x10000) {
                script_सततम्->script_सीमा -= 1;
            }

            भङ्ग;
        }
    }


    यदि (p_सततम्_Start != शून्यसूचकः) {
        *p_सततम्_Start = script_सततम्->scriptStart;
    }

    यदि (p_सततम्_सीमा != शून्यसूचकः) {
        *p_सततम्_सीमा = script_सततम्->script_सीमा;
    }

    यदि (p_सततम्_Script != शून्यसूचकः) {
        *p_सततम्_Script = script_सततम्->script_संहिता;
    }

    प्रत्यागम सत्य;
}
