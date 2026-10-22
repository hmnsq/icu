// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u32.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-32 converter implementation. Used to be in ucnv_utf.c.
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"

#परिभाषय अधिकतमम्_UCS2            0x0000FFFF
#परिभाषय अधिकतमम्_UTF             0x0010FFFF
#परिभाषय HALF_सरणम्              10
#परिभाषय HALF_आधारः               0x0010000
#परिभाषय HALF_आवरणम्               0x3FF
#परिभाषय SURROGATE_उच्चः_START    0xD800
#परिभाषय SURROGATE_नीचः_START     0xDC00

/* -SURROGATE_LOW_START + HALF_BASE */
#परिभाषय SURROGATE_नीचः_आधारः      9216

गणना {
    UCNV_NEED_गन्तव्यम्_WRITE_BOM=1
};

/* UTF-32BE ----------------------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_UTF32_BE(UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                UError_संहिता * err)
{
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = तर्काः_2->converter->गन्तव्यम्_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    यदि (तर्काः_2->converter->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा) {
        i = तर्काः_2->converter->गन्तव्यम्_ULength;       /* restore # of bytes consumed */
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;

        ch = तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः - 1;/*Stores the previously calculated ch from a previous call*/
        तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा) {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t)) {
            यदि (mySource < source_सीमा) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                गन्तव्यम्_UBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = ch + 1;
                तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= अधिकतमम्_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= अधिकतमम्_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                यदि (myTarget < target_सीमा) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    तर्काः_2->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    तर्काः_2->converter->UChar_Error_बफर_दीर्घता = 1;
                    *err = U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_प्राप्तम्;
            भङ्ग;
        }
    }

donefornow:
    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err)) {
        /* End of target buffer */
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = (ध्रुव वर्ण *) mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_UTF32_BE_विस्थापनम्_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                             UError_संहिता * err)
{
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    int32_t *myOffsets = तर्काः_2->offsets;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = तर्काः_2->converter->गन्तव्यम्_UBytes;
    uint32_t ch, i;
    int32_t विस्थापनम्_अङ्कः = 0;

    /* Restore state of current sequence */
    यदि (तर्काः_2->converter->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा) {
        i = तर्काः_2->converter->गन्तव्यम्_ULength;       /* restore # of bytes consumed */
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;

        ch = तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः - 1;/*Stores the previously calculated ch from a previous call*/
        तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा) {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t)) {
            यदि (mySource < source_सीमा) {
                ch = (ch << 8) | (uint8_t)(*mySource);
                गन्तव्यम्_UBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = ch + 1;
                तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= अधिकतमम्_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= अधिकतमम्_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = विस्थापनम्_अङ्कः;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *myOffsets++ = विस्थापनम्_अङ्कः;
                ch = U16_TRAIL(ch);
                यदि (myTarget < target_सीमा)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = विस्थापनम्_अङ्कः;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    तर्काः_2->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    तर्काः_2->converter->UChar_Error_बफर_दीर्घता = 1;
                    *err = U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_प्राप्तम्;
            भङ्ग;
        }
        विस्थापनम्_अङ्कः += i;
    }

donefornow:
    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = (ध्रुव वर्ण *) mySource;
    तर्काः_2->offsets = myOffsets;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_BE(UConverter_From_Unicode_तर्काः * तर्काः_2,
                                  UError_संहिता * err)
{
    ध्रुव char16_t *mySource = तर्काः_2->source;
    अचिह्नित वर्ण *myTarget;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव अचिह्नित वर्ण *target_सीमा = (अचिह्नित वर्ण *) तर्काः_2->target_सीमा;
    UChar32 ch, ch2;
    अचिह्नित पूर्णाङ्क सूचकांक_गन्तव्यम्_Write;
    अचिह्नित वर्ण अस्थायी[परिमाण(uint32_t)];

    यदि(mySource >= source_सीमा) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(तर्काः_2->converter->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu };
        ucnv_from_UWrite_अष्टकानि(तर्काः_2->converter,
                             bom, 4,
                             &तर्काः_2->target, तर्काः_2->target_सीमा,
                             &तर्काः_2->offsets, -1,
                             err);
        तर्काः_2->converter->from_Unicode_स्थितिः=0;
    }

    myTarget = (अचिह्नित वर्ण *) तर्काः_2->target;
    अस्थायी[0] = 0;

    यदि (तर्काः_2->converter->fromUChar32) {
        ch = तर्काः_2->converter->fromUChar32;
        तर्काः_2->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा) {
        ch = *(mySource++);

        यदि (U_IS_SURROGATE(ch)) {
            यदि (U_IS_LEAD(ch)) {
lowsurogate:
                यदि (mySource < source_सीमा) {
                    ch2 = *mySource;
                    यदि (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_उच्चः_START) << HALF_सरणम्) + ch2 + SURROGATE_नीचः_आधारः;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        तर्काः_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    तर्काः_2->converter->fromUChar32 = ch;
                    यदि (तर्काः_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                तर्काः_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[1] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (सूचकांक_गन्तव्यम्_Write = 0; सूचकांक_गन्तव्यम्_Write <= परिमाण(uint32_t) - 1; सूचकांक_गन्तव्यम्_Write++) {
            यदि (myTarget < target_सीमा) {
                *(myTarget++) = अस्थायी[सूचकांक_गन्तव्यम्_Write];
            }
            अन्यथा {
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
    }

    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err)) {
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = (वर्ण *) myTarget;
    तर्काः_2->source = mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_from_Unicode_UTF32_BE_विस्थापनम्_LOGIC(UConverter_From_Unicode_तर्काः * तर्काः_2,
                                               UError_संहिता * err)
{
    ध्रुव char16_t *mySource = तर्काः_2->source;
    अचिह्नित वर्ण *myTarget;
    int32_t *myOffsets;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव अचिह्नित वर्ण *target_सीमा = (अचिह्नित वर्ण *) तर्काः_2->target_सीमा;
    UChar32 ch, ch2;
    int32_t विस्थापनम्_अङ्कः = 0;
    अचिह्नित पूर्णाङ्क सूचकांक_गन्तव्यम्_Write;
    अचिह्नित वर्ण अस्थायी[परिमाण(uint32_t)];

    यदि(mySource >= source_सीमा) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(तर्काः_2->converter->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu };
        ucnv_from_UWrite_अष्टकानि(तर्काः_2->converter,
                             bom, 4,
                             &तर्काः_2->target, तर्काः_2->target_सीमा,
                             &तर्काः_2->offsets, -1,
                             err);
        तर्काः_2->converter->from_Unicode_स्थितिः=0;
    }

    myTarget = (अचिह्नित वर्ण *) तर्काः_2->target;
    myOffsets = तर्काः_2->offsets;
    अस्थायी[0] = 0;

    यदि (तर्काः_2->converter->fromUChar32) {
        ch = तर्काः_2->converter->fromUChar32;
        तर्काः_2->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा) {
        ch = *(mySource++);

        यदि (U_IS_SURROGATE(ch)) {
            यदि (U_IS_LEAD(ch)) {
lowsurogate:
                यदि (mySource < source_सीमा) {
                    ch2 = *mySource;
                    यदि (U_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_उच्चः_START) << HALF_सरणम्) + ch2 + SURROGATE_नीचः_आधारः;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        तर्काः_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    तर्काः_2->converter->fromUChar32 = ch;
                    यदि (तर्काः_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                तर्काः_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[1] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[2] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[3] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (सूचकांक_गन्तव्यम्_Write = 0; सूचकांक_गन्तव्यम्_Write <= परिमाण(uint32_t) - 1; सूचकांक_गन्तव्यम्_Write++) {
            यदि (myTarget < target_सीमा) {
                *(myTarget++) = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *(myOffsets++) = विस्थापनम्_अङ्कः;
            }
            अन्यथा {
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        विस्थापनम्_अङ्कः = विस्थापनम्_अङ्कः + 1 + (अस्थायी[1] != 0);
    }

    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err)) {
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = (वर्ण *) myTarget;
    तर्काः_2->source = mySource;
    तर्काः_2->offsets = myOffsets;
}

स्थिर UChar32 U_CALLCONV
T_UConverter_get_अग्र_UChar_UTF32_BE(UConverter_गन्तव्यम्_Unicode_तर्काः* तर्काः_2,
                                   UError_संहिता* err)
{
    ध्रुव uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ध्रुव uint8_t *)तर्काः_2->source;
    यदि (mySource >= (ध्रुव uint8_t *)तर्काः_2->source_सीमा)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    length = (int32_t)((ध्रुव uint8_t *)तर्काः_2->source_सीमा - mySource);
    यदि (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(तर्काः_2->converter->गन्तव्यम्_UBytes, mySource, length);
        तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)length;
        तर्काः_2->source = (ध्रुव वर्ण *)(mySource + length);
        *err = U_TRUNCATED_CHAR_प्राप्तम्;
        प्रत्यागम 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[0] << 24)
            | ((UChar32)mySource[1] << 16)
            | ((UChar32)mySource[2] << 8)
            | ((UChar32)mySource[3]);

    तर्काः_2->source = (ध्रुव वर्ण *)(mySource + 4);
    यदि ((uint32_t)myUChar <= अधिकतमम्_UTF && !U_IS_SURROGATE(myUChar)) {
        प्रत्यागम myUChar;
    }

    uprv_memcpy(तर्काः_2->converter->गन्तव्यम्_UBytes, mySource, 4);
    तर्काः_2->converter->गन्तव्यम्_ULength = 4;

    *err = U_ILLEGAL_CHAR_प्राप्तम्;
    प्रत्यागम 0xffff;
}
U_CDECL_अन्तः
स्थिर ध्रुव UConverterImpl _UTF32BEImpl = {
    UCNV_UTF32_BigEndian,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    T_UConverter_गन्तव्यम्_Unicode_UTF32_BE,
    T_UConverter_गन्तव्यम्_Unicode_UTF32_BE_विस्थापनम्_LOGIC,
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_विस्थापनम्_LOGIC,
    T_UConverter_get_अग्र_UChar_UTF32_BE,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
स्थिर ध्रुव UConverter_Static_दत्त UTF32_BEStatic_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "UTF-32BE",
    1232,
    UCNV_IBM, UCNV_UTF32_BigEndian, 4, 4,
    { 0, 0, 0xff, 0xfd }, 4, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त _UTF32BEData =
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF32_BEStatic_दत्त, &_UTF32BEImpl);

/* UTF-32LE ---------------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_UTF32_LE(UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                UError_संहिता * err)
{
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = तर्काः_2->converter->गन्तव्यम्_UBytes;
    uint32_t ch, i;

    /* Restore state of current sequence */
    यदि (तर्काः_2->converter->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा)
    {
        i = तर्काः_2->converter->गन्तव्यम्_ULength;       /* restore # of bytes consumed */
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः - 1;
        तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t))
        {
            यदि (mySource < source_सीमा)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                गन्तव्यम्_UBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = ch + 1;
                तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= अधिकतमम्_UTF && !U_IS_SURROGATE(ch)) {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= अधिकतमम्_UCS2) {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                ch = U16_TRAIL(ch);
                यदि (myTarget < target_सीमा) {
                    *(myTarget++) = (char16_t)ch;
                }
                अन्यथा {
                    /* Put in overflow buffer (not handled here) */
                    तर्काः_2->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    तर्काः_2->converter->UChar_Error_बफर_दीर्घता = 1;
                    *err = U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा {
            तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_प्राप्तम्;
            भङ्ग;
        }
    }

donefornow:
    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = (ध्रुव वर्ण *) mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_गन्तव्यम्_Unicode_UTF32_LE_विस्थापनम्_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                             UError_संहिता * err)
{
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    int32_t *myOffsets = तर्काः_2->offsets;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = तर्काः_2->converter->गन्तव्यम्_UBytes;
    uint32_t ch, i;
    int32_t विस्थापनम्_अङ्कः = 0;

    /* Restore state of current sequence */
    यदि (तर्काः_2->converter->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा)
    {
        i = तर्काः_2->converter->गन्तव्यम्_ULength;       /* restore # of bytes consumed */
        तर्काः_2->converter->गन्तव्यम्_ULength = 0;

        /* Stores the previously calculated ch from a previous call*/
        ch = तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः - 1;
        तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        i = 0;
        ch = 0;
morebytes:
        यावत् (i < परिमाण(uint32_t))
        {
            यदि (mySource < source_सीमा)
            {
                ch |= ((uint8_t)(*mySource)) << (i * 8);
                गन्तव्यम्_UBytes[i++] = (वर्ण) *(mySource++);
            }
            अन्यथा
            {
                /* stores a partially calculated target*/
                /* + 1 to make 0 a valid character */
                तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = ch + 1;
                तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t) i;
                गच्छ donefornow;
            }
        }

        यदि (ch <= अधिकतमम्_UTF && !U_IS_SURROGATE(ch))
        {
            /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
            यदि (ch <= अधिकतमम्_UCS2) 
            {
                /* fits in 16 bits */
                *(myTarget++) = (char16_t) ch;
                *(myOffsets++) = विस्थापनम्_अङ्कः;
            }
            अन्यथा {
                /* write out the surrogates */
                *(myTarget++) = U16_LEAD(ch);
                *(myOffsets++) = विस्थापनम्_अङ्कः;
                ch = U16_TRAIL(ch);
                यदि (myTarget < target_सीमा)
                {
                    *(myTarget++) = (char16_t)ch;
                    *(myOffsets++) = विस्थापनम्_अङ्कः;
                }
                अन्यथा
                {
                    /* Put in overflow buffer (not handled here) */
                    तर्काः_2->converter->UChar_Error_बफर[0] = (char16_t) ch;
                    तर्काः_2->converter->UChar_Error_बफर_दीर्घता = 1;
                    *err = U_बफर_अतिप्रवाहः_ERROR;
                    भङ्ग;
                }
            }
        }
        अन्यथा
        {
            तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)i;
            *err = U_ILLEGAL_CHAR_प्राप्तम्;
            भङ्ग;
        }
        विस्थापनम्_अङ्कः += i;
    }

donefornow:
    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        /* End of target buffer */
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = (ध्रुव वर्ण *) mySource;
    तर्काः_2->offsets = myOffsets;
}

स्थिर शून्य U_CALLCONV
T_UConverter_fromUnicode_UTF32_LE(UConverter_From_Unicode_तर्काः * तर्काः_2,
                                  UError_संहिता * err)
{
    ध्रुव char16_t *mySource = तर्काः_2->source;
    अचिह्नित वर्ण *myTarget;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव अचिह्नित वर्ण *target_सीमा = (अचिह्नित वर्ण *) तर्काः_2->target_सीमा;
    UChar32 ch, ch2;
    अचिह्नित पूर्णाङ्क सूचकांक_गन्तव्यम्_Write;
    अचिह्नित वर्ण अस्थायी[परिमाण(uint32_t)];

    यदि(mySource >= source_सीमा) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(तर्काः_2->converter->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };
        ucnv_from_UWrite_अष्टकानि(तर्काः_2->converter,
                             bom, 4,
                             &तर्काः_2->target, तर्काः_2->target_सीमा,
                             &तर्काः_2->offsets, -1,
                             err);
        तर्काः_2->converter->from_Unicode_स्थितिः=0;
    }

    myTarget = (अचिह्नित वर्ण *) तर्काः_2->target;
    अस्थायी[3] = 0;

    यदि (तर्काः_2->converter->fromUChar32)
    {
        ch = तर्काः_2->converter->fromUChar32;
        तर्काः_2->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);

        यदि (U16_IS_SURROGATE(ch)) {
            यदि (U16_IS_LEAD(ch))
            {
lowsurogate:
                यदि (mySource < source_सीमा)
                {
                    ch2 = *mySource;
                    यदि (U16_IS_TRAIL(ch2)) {
                        ch = ((ch - SURROGATE_उच्चः_START) << HALF_सरणम्) + ch2 + SURROGATE_नीचः_आधारः;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        तर्काः_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    तर्काः_2->converter->fromUChar32 = ch;
                    यदि (तर्काः_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                तर्काः_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[2] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (सूचकांक_गन्तव्यम्_Write = 0; सूचकांक_गन्तव्यम्_Write <= परिमाण(uint32_t) - 1; सूचकांक_गन्तव्यम्_Write++)
        {
            यदि (myTarget < target_सीमा)
            {
                *(myTarget++) = अस्थायी[सूचकांक_गन्तव्यम्_Write];
            }
            अन्यथा
            {
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
    }

    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = (वर्ण *) myTarget;
    तर्काः_2->source = mySource;
}

स्थिर शून्य U_CALLCONV
T_UConverter_from_Unicode_UTF32_LE_विस्थापनम्_LOGIC(UConverter_From_Unicode_तर्काः * तर्काः_2,
                                               UError_संहिता * err)
{
    ध्रुव char16_t *mySource = तर्काः_2->source;
    अचिह्नित वर्ण *myTarget;
    int32_t *myOffsets;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    ध्रुव अचिह्नित वर्ण *target_सीमा = (अचिह्नित वर्ण *) तर्काः_2->target_सीमा;
    UChar32 ch, ch2;
    अचिह्नित पूर्णाङ्क सूचकांक_गन्तव्यम्_Write;
    अचिह्नित वर्ण अस्थायी[परिमाण(uint32_t)];
    int32_t विस्थापनम्_अङ्कः = 0;

    यदि(mySource >= source_सीमा) {
        /* no input, nothing to do */
        प्रत्यागम;
    }

    /* write the BOM if necessary */
    यदि(तर्काः_2->converter->from_Unicode_स्थितिः==UCNV_NEED_गन्तव्यम्_WRITE_BOM) {
        स्थिर ध्रुव वर्ण bom[]={ (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };
        ucnv_from_UWrite_अष्टकानि(तर्काः_2->converter,
                             bom, 4,
                             &तर्काः_2->target, तर्काः_2->target_सीमा,
                             &तर्काः_2->offsets, -1,
                             err);
        तर्काः_2->converter->from_Unicode_स्थितिः=0;
    }

    myTarget = (अचिह्नित वर्ण *) तर्काः_2->target;
    myOffsets = तर्काः_2->offsets;
    अस्थायी[3] = 0;

    यदि (तर्काः_2->converter->fromUChar32)
    {
        ch = तर्काः_2->converter->fromUChar32;
        तर्काः_2->converter->fromUChar32 = 0;
        गच्छ lowsurogate;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);

        यदि (U16_IS_SURROGATE(ch)) {
            यदि (U16_IS_LEAD(ch))
            {
lowsurogate:
                यदि (mySource < source_सीमा)
                {
                    ch2 = *mySource;
                    यदि (U16_IS_TRAIL(ch2))
                    {
                        ch = ((ch - SURROGATE_उच्चः_START) << HALF_सरणम्) + ch2 + SURROGATE_नीचः_आधारः;
                        mySource++;
                    }
                    अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        तर्काः_2->converter->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* ran out of source */
                    तर्काः_2->converter->fromUChar32 = ch;
                    यदि (तर्काः_2->flush) {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                    भङ्ग;
                }
            }
            अन्यथा {
                /* this is an unmatched trail code unit (2nd surrogate) */
                /* callback(illegal) */
                तर्काः_2->converter->fromUChar32 = ch;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }

        /* We cannot get any larger than 10FFFF because we are coming from UTF-16 */
        अस्थायी[2] = (uint8_t) (ch >> 16 & 0x1F);
        अस्थायी[1] = (uint8_t) (ch >> 8);  /* unsigned cast implicitly does (ch & FF) */
        अस्थायी[0] = (uint8_t) (ch);       /* unsigned cast implicitly does (ch & FF) */

        प्रति (सूचकांक_गन्तव्यम्_Write = 0; सूचकांक_गन्तव्यम्_Write <= परिमाण(uint32_t) - 1; सूचकांक_गन्तव्यम्_Write++)
        {
            यदि (myTarget < target_सीमा)
            {
                *(myTarget++) = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *(myOffsets++) = विस्थापनम्_अङ्कः;
            }
            अन्यथा
            {
                तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = अस्थायी[सूचकांक_गन्तव्यम्_Write];
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        विस्थापनम्_अङ्कः = विस्थापनम्_अङ्कः + 1 + (अस्थायी[2] != 0);
    }

    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = (वर्ण *) myTarget;
    तर्काः_2->source = mySource;
    तर्काः_2->offsets = myOffsets;
}

स्थिर UChar32 U_CALLCONV
T_UConverter_get_अग्र_UChar_UTF32_LE(UConverter_गन्तव्यम्_Unicode_तर्काः* तर्काः_2,
                                   UError_संहिता* err)
{
    ध्रुव uint8_t *mySource;
    UChar32 myUChar;
    int32_t length;

    mySource = (ध्रुव uint8_t *)तर्काः_2->source;
    यदि (mySource >= (ध्रुव uint8_t *)तर्काः_2->source_सीमा)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    length = (int32_t)((ध्रुव uint8_t *)तर्काः_2->source_सीमा - mySource);
    यदि (length < 4) 
    {
        /* got a partial character */
        uprv_memcpy(तर्काः_2->converter->गन्तव्यम्_UBytes, mySource, length);
        तर्काः_2->converter->गन्तव्यम्_ULength = (int8_t)length;
        तर्काः_2->source = (ध्रुव वर्ण *)(mySource + length);
        *err = U_TRUNCATED_CHAR_प्राप्तम्;
        प्रत्यागम 0xffff;
    }

    /* Don't even try to do a direct cast because the value may be on an odd address. */
    myUChar = ((UChar32)mySource[3] << 24)
            | ((UChar32)mySource[2] << 16)
            | ((UChar32)mySource[1] << 8)
            | ((UChar32)mySource[0]);

    तर्काः_2->source = (ध्रुव वर्ण *)(mySource + 4);
    यदि ((uint32_t)myUChar <= अधिकतमम्_UTF && !U_IS_SURROGATE(myUChar)) {
        प्रत्यागम myUChar;
    }

    uprv_memcpy(तर्काः_2->converter->गन्तव्यम्_UBytes, mySource, 4);
    तर्काः_2->converter->गन्तव्यम्_ULength = 4;

    *err = U_ILLEGAL_CHAR_प्राप्तम्;
    प्रत्यागम 0xffff;
}
U_CDECL_अन्तः
स्थिर ध्रुव UConverterImpl _UTF32LEImpl = {
    UCNV_UTF32_LittleEndian,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    T_UConverter_गन्तव्यम्_Unicode_UTF32_LE,
    T_UConverter_गन्तव्यम्_Unicode_UTF32_LE_विस्थापनम्_LOGIC,
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_विस्थापनम्_LOGIC,
    T_UConverter_get_अग्र_UChar_UTF32_LE,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

/* The 1232 CCSID refers to any version of Unicode with any endianness of UTF-32 */
स्थिर ध्रुव UConverter_Static_दत्त UTF32_LEStatic_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "UTF-32LE",
    1234,
    UCNV_IBM, UCNV_UTF32_LittleEndian, 4, 4,
    { 0xfd, 0xff, 0, 0 }, 4, असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverter_Shared_दत्त _UTF32LEData =
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF32_LEStatic_दत्त, &_UTF32LEImpl);

/* UTF-32 (Detect BOM) ------------------------------------------------------ */

/*
 * Detect a BOM at the beginning of the stream and select UTF-32BE or UTF-32LE
 * accordingly.
 *
 * State values:
 * 0    initial state
 * 1    saw 00
 * 2    saw 00 00
 * 3    saw 00 00 FE
 * 4    -
 * 5    saw FF
 * 6    saw FF FE
 * 7    saw FF FE 00
 * 8    UTF-32BE mode
 * 9    UTF-32LE mode
 *
 * During detection: state&3==number of matching bytes so far.
 *
 * On output, emit U+FEFF as the first code point.
 */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_UTF32Reset(UConverter *cnv, UConverterResetChoice choice) {
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode: state=0 */
        cnv->बहुलकम्=0;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset fromUnicode: prepare to output the UTF-32PE BOM */
        cnv->from_Unicode_स्थितिः=UCNV_NEED_गन्तव्यम्_WRITE_BOM;
    }
}

स्थिर शून्य U_CALLCONV
_UTF32Open(UConverter *cnv,
           UConverter_आरोपय_तर्काः *p_तर्काः,
           UError_संहिता *p_Error_संहिता) {
    (शून्य)p_तर्काः;
    (शून्य)p_Error_संहिता;
    _UTF32Reset(cnv, UCNV_RESET_BOTH);
}

स्थिर ध्रुव वर्ण utf32BOM[8]={ 0, 0, (वर्ण)0xfeu, (वर्ण)0xffu, (वर्ण)0xffu, (वर्ण)0xfeu, 0, 0 };

स्थिर शून्य U_CALLCONV
UTF32_गन्तव्यम्_Unicode_With_Offsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                           UError_संहिता *p_Error_संहिता) {
    UConverter *cnv=p_तर्काः->converter;
    ध्रुव वर्ण *source=p_तर्काः->source;
    ध्रुव वर्ण *source_सीमा=p_तर्काः->source_सीमा;
    int32_t *offsets=p_तर्काः->offsets;

    int32_t state, विस्थापनम्_Delta;
    वर्ण b;

    state=cnv->बहुलकम्;

    /*
     * If we detect a BOM in this buffer, then we must add the BOM size to the
     * offsets because the actual converter function will not see and count the BOM.
     * offsetDelta will have the number of the BOM bytes that are in the current buffer.
     */
    विस्थापनम्_Delta=0;

    यावत्(source<source_सीमा && U_SUCCESS(*p_Error_संहिता)) {
        विकल्प(state) {
        पक्ष 0:
            b=*source;
            यदि(b==0) {
                state=1; /* could be 00 00 FE FF */
            } अन्यथा यदि(b==(वर्ण)0xffu) {
                state=5; /* could be FF FE 00 00 */
            } अन्यथा {
                state=8; /* default to UTF-32BE */
                अनुवर्त;
            }
            ++source;
            भङ्ग;
        पक्ष 1:
        पक्ष 2:
        पक्ष 3:
        पक्ष 5:
        पक्ष 6:
        पक्ष 7:
            यदि(*source==utf32BOM[state]) {
                ++state;
                ++source;
                यदि(state==4) {
                    state=8; /* detect UTF-32BE */
                    विस्थापनम्_Delta=(int32_t)(source-p_तर्काः->source);
                } अन्यथा यदि(state==8) {
                    state=9; /* detect UTF-32LE */
                    विस्थापनम्_Delta=(int32_t)(source-p_तर्काः->source);
                }
            } अन्यथा {
                /* switch to UTF-32BE and pass the previous bytes */
                int32_t count=(int32_t)(source-p_तर्काः->source); /* number of bytes from this buffer */

                /* reset the source */
                source=p_तर्काः->source;

                यदि(count==(state&3)) {
                    /* simple: all in the same buffer, just reset source */
                } अन्यथा {
                    UBool oldFlush=p_तर्काः->flush;

                    /* some of the bytes are from a previous buffer, replay those first */
                    p_तर्काः->source=utf32BOM+(state&4); /* select the correct BOM */
                    p_तर्काः->source_सीमा=p_तर्काः->source+((state&3)-count); /* replay previous bytes */
                    p_तर्काः->flush=असत्य; /* this sourceLimit is not the real source stream limit */

                    /* no offsets: bytes from previous buffer, and not enough for output */
                    T_UConverter_गन्तव्यम्_Unicode_UTF32_BE(p_तर्काः, p_Error_संहिता);

                    /* restore real pointers; pArgs->source will be set in case 8/9 */
                    p_तर्काः->source_सीमा=source_सीमा;
                    p_तर्काः->flush=oldFlush;
                }
                state=8;
                अनुवर्त;
            }
            भङ्ग;
        पक्ष 8:
            /* call UTF-32BE */
            p_तर्काः->source=source;
            यदि(offsets==शून्यसूचकः) {
                T_UConverter_गन्तव्यम्_Unicode_UTF32_BE(p_तर्काः, p_Error_संहिता);
            } अन्यथा {
                T_UConverter_गन्तव्यम्_Unicode_UTF32_BE_विस्थापनम्_LOGIC(p_तर्काः, p_Error_संहिता);
            }
            source=p_तर्काः->source;
            भङ्ग;
        पक्ष 9:
            /* call UTF-32LE */
            p_तर्काः->source=source;
            यदि(offsets==शून्यसूचकः) {
                T_UConverter_गन्तव्यम्_Unicode_UTF32_LE(p_तर्काः, p_Error_संहिता);
            } अन्यथा {
                T_UConverter_गन्तव्यम्_Unicode_UTF32_LE_विस्थापनम्_LOGIC(p_तर्काः, p_Error_संहिता);
            }
            source=p_तर्काः->source;
            भङ्ग;
        स्वभाव:
            भङ्ग; /* does not occur */
        }
    }

    /* add BOM size to offsets - see comment at offsetDelta declaration */
    यदि(offsets!=शून्यसूचकः && विस्थापनम्_Delta!=0) {
        int32_t *offsets_सीमा=p_तर्काः->offsets;
        यावत्(offsets<offsets_सीमा) {
            *offsets++ += विस्थापनम्_Delta;
        }
    }

    p_तर्काः->source=source;

    यदि(source==source_सीमा && p_तर्काः->flush) {
        /* handle truncated input */
        विकल्प(state) {
        पक्ष 0:
            भङ्ग; /* no input at all, nothing to do */
        पक्ष 8:
            T_UConverter_गन्तव्यम्_Unicode_UTF32_BE(p_तर्काः, p_Error_संहिता);
            भङ्ग;
        पक्ष 9:
            T_UConverter_गन्तव्यम्_Unicode_UTF32_LE(p_तर्काः, p_Error_संहिता);
            भङ्ग;
        स्वभाव:
            /* handle 0<state<8: call UTF-32BE with too-short input */
            p_तर्काः->source=utf32BOM+(state&4); /* select the correct BOM */
            p_तर्काः->source_सीमा=p_तर्काः->source+(state&3); /* replay bytes */

            /* no offsets: not enough for output */
            T_UConverter_गन्तव्यम्_Unicode_UTF32_BE(p_तर्काः, p_Error_संहिता);
            p_तर्काः->source=source;
            p_तर्काः->source_सीमा=source_सीमा;
            state=8;
            भङ्ग;
        }
    }

    cnv->बहुलकम्=state;
}

स्थिर UChar32 U_CALLCONV
UTF32_Get_अग्र_UChar(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                   UError_संहिता *p_Error_संहिता) {
    विकल्प(p_तर्काः->converter->बहुलकम्) {
    पक्ष 8:
        प्रत्यागम T_UConverter_get_अग्र_UChar_UTF32_BE(p_तर्काः, p_Error_संहिता);
    पक्ष 9:
        प्रत्यागम T_UConverter_get_अग्र_UChar_UTF32_LE(p_तर्काः, p_Error_संहिता);
    स्वभाव:
        प्रत्यागम UCNV_GET_अग्र_UCHAR_USE_गन्तव्यम्_U;
    }
}
U_CDECL_अन्तः
स्थिर ध्रुव UConverterImpl _UTF32Impl = {
    UCNV_UTF32,

    शून्यसूचकः,
    शून्यसूचकः,

    _UTF32Open,
    शून्यसूचकः,
    _UTF32Reset,

    UTF32_गन्तव्यम्_Unicode_With_Offsets,
    UTF32_गन्तव्यम्_Unicode_With_Offsets,
#यदि U_IS_BIG_ENDIAN
    T_UConverter_fromUnicode_UTF32_BE,
    T_UConverter_from_Unicode_UTF32_BE_विस्थापनम्_LOGIC,
#अन्यथा
    T_UConverter_fromUnicode_UTF32_LE,
    T_UConverter_from_Unicode_UTF32_LE_विस्थापनम्_LOGIC,
#यदि_अन्तः
    UTF32_Get_अग्र_UChar,

    शून्यसूचकः, /* ### TODO implement getStarters for all Unicode encodings?! */
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

/* The 1236 CCSID refers to any version of Unicode with a BOM sensitive endianness of UTF-32 */
स्थिर ध्रुव UConverter_Static_दत्त UTF32_Static_दत्त = {
    परिमाण(UConverter_Static_दत्त),
    "UTF-32",
    1236,
    UCNV_IBM, UCNV_UTF32, 4, 4,
#यदि U_IS_BIG_ENDIAN
    { 0, 0, 0xff, 0xfd }, 4,
#अन्यथा
    { 0xfd, 0xff, 0, 0 }, 4,
#यदि_अन्तः
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त UTF32_दत्त = 
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF32_Static_दत्त, &_UTF32Impl);

#यदि_अन्तः
