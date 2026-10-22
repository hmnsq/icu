// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2002-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnv_u8.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2002jul01
*   created by: Markus W. Scherer
*
*   UTF-8 converter implementation. Used to be in ucnv_utf.c.
*
*   Also, CESU-8 implementation, see UTR 26.
*   The CESU-8 converter uses all the same functions as the
*   UTF-8 converter, with a branch for converting supplementary code points.
*/

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf8.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "ustr_imp.h"

/* Prototypes --------------------------------------------------------------- */

/* Keep these here to make finicky compilers happy */

U_CFUNC शून्य ucnv_fromUnicode_UTF8(UConverter_From_Unicode_तर्काः *तर्काः_2,
                                           UError_संहिता *err);
U_CFUNC शून्य ucnv_fromUnicode_UTF8_OFFSETS_LOGIC(UConverter_From_Unicode_तर्काः *तर्काः_2,
                                                        UError_संहिता *err);


/* UTF-8 -------------------------------------------------------------------- */

#परिभाषय अधिकतमम्_UCS2            0x0000FFFF

स्थिर ध्रुव uint32_t offsetsFromUTF8[5] = {0,
  static_cast<uint32_t>(0x00000000), static_cast<uint32_t>(0x00003080),
  static_cast<uint32_t>(0x000E2080), static_cast<uint32_t>(0x03C82080)
};

स्थिर UBool has_CESU8_दत्त(ध्रुव UConverter *cnv)
{
#यदि UCONFIG_ONLY_HTML_CONVERSION
    प्रत्यागम असत्य;
#अन्यथा
    प्रत्यागम cnv->shared_दत्त == &CESU8_दत्त;
#यदि_अन्तः
}
U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV ucnv_गन्तव्यम्_Unicode_UTF8 (UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                  UError_संहिता * err)
{
    UConverter *cnv = तर्काः_2->converter;
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = cnv->गन्तव्यम्_UBytes;
    UBool isCESU8 = has_CESU8_दत्त(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, अन्तर्_अष्टकानि;

    /* Restore size of current sequence */
    यदि (cnv->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा)
    {
        अन्तर्_अष्टकानि = cnv->बहुलकम्;            /* restore # of bytes to consume */
        i = cnv->गन्तव्यम्_ULength;             /* restore # of bytes consumed */
        cnv->गन्तव्यम्_ULength = 0;

        ch = cnv->गन्तव्यम्_Unicode_स्थितिः;/*Stores the previously calculated ch from a previous call*/
        cnv->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }


    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);
        यदि (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
        }
        अन्यथा
        {
            /* store the first char */
            गन्तव्यम्_UBytes[0] = (वर्ण)ch;
            अन्तर्_अष्टकानि = U8_गणनम्_अष्टकानि_NON_ASCII(ch); /* lookup current sequence length */
            i = 1;

morebytes:
            यावत् (i < अन्तर्_अष्टकानि)
            {
                यदि (mySource < source_सीमा)
                {
                    गन्तव्यम्_UBytes[i] = (वर्ण) (ch2 = *mySource);
                    यदि (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, अन्तर्_अष्टकानि) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        भङ्ग; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अन्यथा
                {
                    /* stores a partially calculated target*/
                    cnv->गन्तव्यम्_Unicode_स्थितिः = ch;
                    cnv->बहुलकम् = अन्तर्_अष्टकानि;
                    cnv->गन्तव्यम्_ULength = (int8_t) i;
                    गच्छ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            यदि (i == अन्तर्_अष्टकानि && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[अन्तर्_अष्टकानि];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                यदि (ch <= अधिकतमम्_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                }
                अन्यथा
                {
                    /* write out the surrogates */
                    *(myTarget++) = U16_LEAD(ch);
                    ch = U16_TRAIL(ch);
                    यदि (myTarget < target_सीमा)
                    {
                        *(myTarget++) = (char16_t)ch;
                    }
                    अन्यथा
                    {
                        /* Put in overflow buffer (not handled here) */
                        cnv->UChar_Error_बफर[0] = (char16_t) ch;
                        cnv->UChar_Error_बफर_दीर्घता = 1;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                        भङ्ग;
                    }
                }
            }
            अन्यथा
            {
                cnv->गन्तव्यम्_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
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

स्थिर शून्य  U_CALLCONV ucnv_गन्तव्यम्_Unicode_UTF8_OFFSETS_LOGIC (UConverter_गन्तव्यम्_Unicode_तर्काः * तर्काः_2,
                                                UError_संहिता * err)
{
    UConverter *cnv = तर्काः_2->converter;
    ध्रुव अचिह्नित वर्ण *mySource = (अचिह्नित वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    int32_t *myOffsets = तर्काः_2->offsets;
    int32_t विस्थापनम्_अङ्कः = 0;
    ध्रुव अचिह्नित वर्ण *source_सीमा = (अचिह्नित वर्ण *) तर्काः_2->source_सीमा;
    ध्रुव char16_t *target_सीमा = तर्काः_2->target_सीमा;
    अचिह्नित वर्ण *गन्तव्यम्_UBytes = cnv->गन्तव्यम्_UBytes;
    UBool isCESU8 = has_CESU8_दत्त(cnv);
    uint32_t ch, ch2 = 0;
    int32_t i, अन्तर्_अष्टकानि;

    /* Restore size of current sequence */
    यदि (cnv->गन्तव्यम्_ULength > 0 && myTarget < target_सीमा)
    {
        अन्तर्_अष्टकानि = cnv->बहुलकम्;            /* restore # of bytes to consume */
        i = cnv->गन्तव्यम्_ULength;             /* restore # of bytes consumed */
        cnv->गन्तव्यम्_ULength = 0;

        ch = cnv->गन्तव्यम्_Unicode_स्थितिः;/*Stores the previously calculated ch from a previous call*/
        cnv->गन्तव्यम्_Unicode_स्थितिः = 0;
        गच्छ morebytes;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);
        यदि (U8_IS_SINGLE(ch))        /* Simple case */
        {
            *(myTarget++) = (char16_t) ch;
            *(myOffsets++) = विस्थापनम्_अङ्कः++;
        }
        अन्यथा
        {
            गन्तव्यम्_UBytes[0] = (वर्ण)ch;
            अन्तर्_अष्टकानि = U8_गणनम्_अष्टकानि_NON_ASCII(ch);
            i = 1;

morebytes:
            यावत् (i < अन्तर्_अष्टकानि)
            {
                यदि (mySource < source_सीमा)
                {
                    गन्तव्यम्_UBytes[i] = (वर्ण) (ch2 = *mySource);
                    यदि (!icu::UTF8::isValidTrail(ch, static_cast<uint8_t>(ch2), i, अन्तर्_अष्टकानि) &&
                            !(isCESU8 && i == 1 && ch == 0xed && U8_IS_TRAIL(ch2)))
                    {
                        भङ्ग; /* i < inBytes */
                    }
                    ch = (ch << 6) + ch2;
                    ++mySource;
                    i++;
                }
                अन्यथा
                {
                    cnv->गन्तव्यम्_Unicode_स्थितिः = ch;
                    cnv->बहुलकम् = अन्तर्_अष्टकानि;
                    cnv->गन्तव्यम्_ULength = (int8_t)i;
                    गच्छ donefornow;
                }
            }

            // In CESU-8, only surrogates, not supplementary code points, are encoded directly.
            यदि (i == अन्तर्_अष्टकानि && (!isCESU8 || i <= 3))
            {
                /* Remove the accumulated high bits */
                ch -= offsetsFromUTF8[अन्तर्_अष्टकानि];

                /* Normal valid byte when the loop has not prematurely terminated (i < inBytes) */
                यदि (ch <= अधिकतमम्_UCS2) 
                {
                    /* fits in 16 bits */
                    *(myTarget++) = (char16_t) ch;
                    *(myOffsets++) = विस्थापनम्_अङ्कः;
                }
                अन्यथा
                {
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
                        cnv->UChar_Error_बफर[0] = (char16_t) ch;
                        cnv->UChar_Error_बफर_दीर्घता = 1;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }
                विस्थापनम्_अङ्कः += i;
            }
            अन्यथा
            {
                cnv->गन्तव्यम्_ULength = (int8_t)i;
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }
    }

donefornow:
    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {   /* End of target buffer */
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = (ध्रुव वर्ण *) mySource;
    तर्काः_2->offsets = myOffsets;
}
U_CDECL_अन्तः

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8 (UConverter_From_Unicode_तर्काः * तर्काः_2,
                                    UError_संहिता * err)
{
    UConverter *cnv = तर्काः_2->converter;
    ध्रुव char16_t *mySource = तर्काः_2->source;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    uint8_t *myTarget = (uint8_t *) तर्काः_2->target;
    ध्रुव uint8_t *target_सीमा = (uint8_t *) तर्काः_2->target_सीमा;
    uint8_t *अस्थायी_Ptr;
    UChar32 ch;
    uint8_t अस्थायी_Buf[4];
    int32_t सूचकांक_गन्तव्यम्_Write;
    UBool isNotCESU8 = !has_CESU8_दत्त(cnv);

    यदि (cnv->fromUChar32 && myTarget < target_सीमा)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        गच्छ lowsurrogate;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);

        यदि (ch < 0x80)        /* Single byte */
        {
            *(myTarget++) = (uint8_t) ch;
        }
        अन्यथा यदि (ch < 0x800)  /* Double byte */
        {
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            यदि (myTarget < target_सीमा)
            {
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->char_Error_बफर[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_बफर_दीर्घता = 1;
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        अन्यथा {
            /* Check for surrogates */
            यदि(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                यदि (mySource < source_सीमा) {
                    /* test both code units */
                    यदि(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        /* exit this condition tree */
                    }
                    अन्यथा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    भङ्ग;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            अस्थायी_Ptr = (((target_सीमा - myTarget) >= 4) ? myTarget : अस्थायी_Buf);

            यदि (ch <= अधिकतमम्_UCS2) {
                सूचकांक_गन्तव्यम्_Write = 2;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                सूचकांक_गन्तव्यम्_Write = 3;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                अस्थायी_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            अस्थायी_Ptr[सूचकांक_गन्तव्यम्_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            अस्थायी_Ptr[सूचकांक_गन्तव्यम्_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            यदि (अस्थायी_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (सूचकांक_गन्तव्यम्_Write + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                प्रति (; अस्थायी_Ptr <= (अस्थायी_Buf + सूचकांक_गन्तव्यम्_Write); अस्थायी_Ptr++) {
                    यदि (myTarget < target_सीमा) {
                        *(myTarget++) = *अस्थायी_Ptr;
                    }
                    अन्यथा {
                        cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++] = *अस्थायी_Ptr;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }
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

U_CFUNC शून्य  U_CALLCONV ucnv_fromUnicode_UTF8_OFFSETS_LOGIC (UConverter_From_Unicode_तर्काः * तर्काः_2,
                                                  UError_संहिता * err)
{
    UConverter *cnv = तर्काः_2->converter;
    ध्रुव char16_t *mySource = तर्काः_2->source;
    int32_t *myOffsets = तर्काः_2->offsets;
    ध्रुव char16_t *source_सीमा = तर्काः_2->source_सीमा;
    uint8_t *myTarget = (uint8_t *) तर्काः_2->target;
    ध्रुव uint8_t *target_सीमा = (uint8_t *) तर्काः_2->target_सीमा;
    uint8_t *अस्थायी_Ptr;
    UChar32 ch;
    int32_t विस्थापनम्_अङ्कः, अग्र_Source_सूचकांक;
    int32_t सूचकांक_गन्तव्यम्_Write;
    uint8_t अस्थायी_Buf[4];
    UBool isNotCESU8 = !has_CESU8_दत्त(cnv);

    यदि (cnv->fromUChar32 && myTarget < target_सीमा)
    {
        ch = cnv->fromUChar32;
        cnv->fromUChar32 = 0;
        विस्थापनम्_अङ्कः = -1;
        अग्र_Source_सूचकांक = 0;
        गच्छ lowsurrogate;
    } अन्यथा {
        विस्थापनम्_अङ्कः = 0;
    }

    यावत् (mySource < source_सीमा && myTarget < target_सीमा)
    {
        ch = *(mySource++);

        यदि (ch < 0x80)        /* Single byte */
        {
            *(myOffsets++) = विस्थापनम्_अङ्कः++;
            *(myTarget++) = (वर्ण) ch;
        }
        अन्यथा यदि (ch < 0x800)  /* Double byte */
        {
            *(myOffsets++) = विस्थापनम्_अङ्कः;
            *(myTarget++) = (uint8_t) ((ch >> 6) | 0xc0);
            यदि (myTarget < target_सीमा)
            {
                *(myOffsets++) = विस्थापनम्_अङ्कः++;
                *(myTarget++) = (uint8_t) ((ch & 0x3f) | 0x80);
            }
            अन्यथा
            {
                cnv->char_Error_बफर[0] = (uint8_t) ((ch & 0x3f) | 0x80);
                cnv->char_Error_बफर_दीर्घता = 1;
                *err = U_बफर_अतिप्रवाहः_ERROR;
            }
        }
        अन्यथा
        /* Check for surrogates */
        {
            अग्र_Source_सूचकांक = विस्थापनम्_अङ्कः + 1;

            यदि(U16_IS_SURROGATE(ch) && isNotCESU8) {
lowsurrogate:
                यदि (mySource < source_सीमा) {
                    /* test both code units */
                    यदि(U16_IS_SURROGATE_LEAD(ch) && U16_IS_TRAIL(*mySource)) {
                        /* convert and consume this supplementary code point */
                        ch=U16_GET_SUPPLEMENTARY(ch, *mySource);
                        ++mySource;
                        ++अग्र_Source_सूचकांक;
                        /* exit this condition tree */
                    }
                    अन्यथा {
                        /* this is an unpaired trail or lead code unit */
                        /* callback(illegal) */
                        cnv->fromUChar32 = ch;
                        *err = U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                }
                अन्यथा {
                    /* no more input */
                    cnv->fromUChar32 = ch;
                    भङ्ग;
                }
            }

            /* Do we write the buffer directly for speed,
            or do we have to be careful about target buffer space? */
            अस्थायी_Ptr = (((target_सीमा - myTarget) >= 4) ? myTarget : अस्थायी_Buf);

            यदि (ch <= अधिकतमम्_UCS2) {
                सूचकांक_गन्तव्यम्_Write = 2;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 12) | 0xe0);
            }
            अन्यथा {
                सूचकांक_गन्तव्यम्_Write = 3;
                अस्थायी_Ptr[0] = (uint8_t) ((ch >> 18) | 0xf0);
                अस्थायी_Ptr[1] = (uint8_t) (((ch >> 12) & 0x3f) | 0x80);
            }
            अस्थायी_Ptr[सूचकांक_गन्तव्यम्_Write-1] = (uint8_t) (((ch >> 6) & 0x3f) | 0x80);
            अस्थायी_Ptr[सूचकांक_गन्तव्यम्_Write] = (uint8_t) ((ch & 0x3f) | 0x80);

            यदि (अस्थायी_Ptr == myTarget) {
                /* There was enough space to write the codepoint directly. */
                myTarget += (सूचकांक_गन्तव्यम्_Write + 1);
                myOffsets[0] = विस्थापनम्_अङ्कः;
                myOffsets[1] = विस्थापनम्_अङ्कः;
                myOffsets[2] = विस्थापनम्_अङ्कः;
                यदि (सूचकांक_गन्तव्यम्_Write >= 3) {
                    myOffsets[3] = विस्थापनम्_अङ्कः;
                }
                myOffsets += (सूचकांक_गन्तव्यम्_Write + 1);
            }
            अन्यथा {
                /* We might run out of room soon. Write it slowly. */
                प्रति (; अस्थायी_Ptr <= (अस्थायी_Buf + सूचकांक_गन्तव्यम्_Write); अस्थायी_Ptr++) {
                    यदि (myTarget < target_सीमा)
                    {
                        *(myOffsets++) = विस्थापनम्_अङ्कः;
                        *(myTarget++) = *अस्थायी_Ptr;
                    }
                    अन्यथा
                    {
                        cnv->char_Error_बफर[cnv->char_Error_बफर_दीर्घता++] = *अस्थायी_Ptr;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }
            }
            विस्थापनम्_अङ्कः = अग्र_Source_सूचकांक;
        }
    }

    यदि (mySource < source_सीमा && myTarget >= target_सीमा && U_SUCCESS(*err))
    {
        *err = U_बफर_अतिप्रवाहः_ERROR;
    }

    तर्काः_2->target = (वर्ण *) myTarget;
    तर्काः_2->source = mySource;
    तर्काः_2->offsets = myOffsets;
}

U_CDECL_BEGIN
स्थिर UChar32 U_CALLCONV ucnv_get_अग्र_UChar_UTF8(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                               UError_संहिता *err) {
    UConverter *cnv;
    ध्रुव uint8_t *sourceInitial;
    ध्रुव uint8_t *source;
    uint8_t myByte;
    UChar32 ch;
    int8_t i;

    /* UTF-8 only here, the framework handles CESU-8 to combine surrogate pairs */

    cnv = तर्काः_2->converter;
    sourceInitial = source = (ध्रुव uint8_t *)तर्काः_2->source;
    यदि (source >= (ध्रुव uint8_t *)तर्काः_2->source_सीमा)
    {
        /* no input */
        *err = U_सूचकांक_OUTOFBOUNDS_ERROR;
        प्रत्यागम 0xffff;
    }

    myByte = *(source++);
    यदि (U8_IS_SINGLE(myByte))
    {
        तर्काः_2->source = (ध्रुव वर्ण *)source;
        प्रत्यागम (UChar32)myByte;
    }

    uint16_t गणनम्_Trail_अष्टकानि = U8_गणनम्_TRAIL_अष्टकानि(myByte);
    यदि (गणनम्_Trail_अष्टकानि == 0) {
        cnv->गन्तव्यम्_UBytes[0] = myByte;
        cnv->गन्तव्यम्_ULength = 1;
        *err = U_ILLEGAL_CHAR_प्राप्तम्;
        तर्काः_2->source = (ध्रुव वर्ण *)source;
        प्रत्यागम 0xffff;
    }

    /*The byte sequence is longer than the buffer area passed*/
    यदि (((ध्रुव वर्ण *)source + गणनम्_Trail_अष्टकानि) > तर्काः_2->source_सीमा)
    {
        /* check if all of the remaining bytes are trail bytes */
        uint16_t extra_अष्टकानि_गन्तव्यम्_Write = गणनम्_Trail_अष्टकानि + 1;
        cnv->गन्तव्यम्_UBytes[0] = myByte;
        i = 1;
        *err = U_TRUNCATED_CHAR_प्राप्तम्;
        यावत्(source < (ध्रुव uint8_t *)तर्काः_2->source_सीमा) {
            uint8_t b = *source;
            यदि(icu::UTF8::isValidTrail(myByte, b, i, extra_अष्टकानि_गन्तव्यम्_Write)) {
                cnv->गन्तव्यम्_UBytes[i++] = b;
                ++source;
            } अन्यथा {
                /* error even before we run out of input */
                *err = U_ILLEGAL_CHAR_प्राप्तम्;
                भङ्ग;
            }
        }
        cnv->गन्तव्यम्_ULength = i;
        तर्काः_2->source = (ध्रुव वर्ण *)source;
        प्रत्यागम 0xffff;
    }

    ch = myByte << 6;
    यदि(गणनम्_Trail_अष्टकानि == 2) {
        uint8_t t1 = *source, t2;
        यदि(U8_IS_VALID_LEAD3_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source)) {
            तर्काः_2->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (((ch + t1) << 6) + t2) - offsetsFromUTF8[3];
        }
    } अन्यथा यदि(गणनम्_Trail_अष्टकानि == 1) {
        uint8_t t1 = *source;
        यदि(U8_IS_TRAIL(t1)) {
            तर्काः_2->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (ch + t1) - offsetsFromUTF8[2];
        }
    } अन्यथा {  // countTrailBytes == 3
        uint8_t t1 = *source, t2, t3;
        यदि(U8_IS_VALID_LEAD4_AND_T1(myByte, t1) && U8_IS_TRAIL(t2 = *++source) &&
                U8_IS_TRAIL(t3 = *++source)) {
            तर्काः_2->source = (ध्रुव वर्ण *)(source + 1);
            प्रत्यागम (((((ch + t1) << 6) + t2) << 6) + t3) - offsetsFromUTF8[4];
        }
    }
    तर्काः_2->source = (ध्रुव वर्ण *)source;

    प्रति(i = 0; sourceInitial < source; ++i) {
        cnv->गन्तव्यम्_UBytes[i] = *sourceInitial++;
    }
    cnv->गन्तव्यम्_ULength = i;
    *err = U_ILLEGAL_CHAR_प्राप्तम्;
    प्रत्यागम 0xffff;
} 
U_CDECL_अन्तः

/* UTF-8-from-UTF-8 conversion functions ------------------------------------ */

U_CDECL_BEGIN
/* "Convert" UTF-8 to UTF-8: Validate and copy. Modified from ucnv_DBCSFromUTF8(). */
स्थिर शून्य U_CALLCONV
ucnv_UTF8FromUTF8(UConverter_From_Unicode_तर्काः *pFromUArgs,
                  UConverter_गन्तव्यम्_Unicode_तर्काः *p_गन्तव्यम्_UArgs,
                  UError_संहिता *p_Error_संहिता) {
    UConverter *utf8;
    ध्रुव uint8_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t count;

    int8_t old_गन्तव्यम्_ULength, गन्तव्यम्_ULength, गन्तव्यम्_ULimit;

    UChar32 c;
    uint8_t b, t1, t2;

    /* set up the local pointers */
    utf8=p_गन्तव्यम्_UArgs->converter;
    source=(uint8_t *)p_गन्तव्यम्_UArgs->source;
    source_सीमा=(uint8_t *)p_गन्तव्यम्_UArgs->source_सीमा;
    target=(uint8_t *)pFromUArgs->target;
    target_क्षमता=(int32_t)(pFromUArgs->target_सीमा-pFromUArgs->target);

    /* get the converter state from the UTF-8 UConverter */
    यदि(utf8->गन्तव्यम्_ULength > 0) {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=utf8->गन्तव्यम्_ULength;
        गन्तव्यम्_ULimit=(int8_t)utf8->बहुलकम्;
        c=(UChar32)utf8->गन्तव्यम्_Unicode_स्थितिः;
    } अन्यथा {
        गन्तव्यम्_ULength=old_गन्तव्यम्_ULength=गन्तव्यम्_ULimit=0;
        c = 0;
    }

    count=(int32_t)(source_सीमा-source)+old_गन्तव्यम्_ULength;
    यदि(count<गन्तव्यम्_ULimit) {
        /*
         * Not enough input to complete the partial character.
         * Jump to moreBytes below - it will not output to target.
         */
    } अन्यथा यदि(target_क्षमता<गन्तव्यम्_ULimit) {
        /*
         * Not enough target capacity to output the partial character.
         * Let the standard converter handle this.
         */
        *p_Error_संहिता=U_USING_DEFAULT_WARNING;
        प्रत्यागम;
    } अन्यथा {
        // Use a single counter for source and target, counting the minimum of
        // the source length and the target capacity.
        // Let the standard converter handle edge cases.
        यदि(count>target_क्षमता) {
            count=target_क्षमता;
        }

        // The conversion loop checks count>0 only once per character.
        // If the buffer ends with a truncated sequence,
        // then we reduce the count to stop before that,
        // and collect the remaining bytes after the conversion loop.

        // Do not go back into the bytes that will be read for finishing a partial
        // sequence from the previous buffer.
        int32_t length=count-गन्तव्यम्_ULength;
        U8_TRUNCATE_IF_INCOMPLETE(source, 0, length);
        count=गन्तव्यम्_ULength+length;
    }

    यदि(c!=0) {
        utf8->गन्तव्यम्_Unicode_स्थितिः=0;
        utf8->गन्तव्यम्_ULength=0;
        गच्छ अधिकम्_अष्टकानि;
        /* See note in ucnv_SBCSFromUTF8() about this goto. */
    }

    /* conversion loop */
    यावत्(count>0) {
        b=*source++;
        यदि(U8_IS_SINGLE(b)) {
            /* convert ASCII */
            *target++=b;
            --count;
            अनुवर्त;
        } अन्यथा {
            यदि(b>=0xe0) {
                यदि( /* handle U+0800..U+FFFF inline */
                    b<0xf0 &&
                    U8_IS_VALID_LEAD3_AND_T1(b, t1=source[0]) &&
                    U8_IS_TRAIL(t2=source[1])
                ) {
                    source+=2;
                    *target++=b;
                    *target++=t1;
                    *target++=t2;
                    count-=3;
                    अनुवर्त;
                }
            } अन्यथा {
                यदि( /* handle U+0080..U+07FF inline */
                    b>=0xc2 &&
                    U8_IS_TRAIL(t1=*source)
                ) {
                    ++source;
                    *target++=b;
                    *target++=t1;
                    count-=2;
                    अनुवर्त;
                }
            }

            /* handle "complicated" and error cases, and continuing partial characters */
            old_गन्तव्यम्_ULength=0;
            गन्तव्यम्_ULength=1;
            गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि_NON_ASCII(b);
            c=b;
अधिकम्_अष्टकानि:
            यावत्(गन्तव्यम्_ULength<गन्तव्यम्_ULimit) {
                यदि(source<source_सीमा) {
                    b=*source;
                    यदि(icu::UTF8::isValidTrail(c, b, गन्तव्यम्_ULength, गन्तव्यम्_ULimit)) {
                        ++source;
                        ++गन्तव्यम्_ULength;
                        c=(c<<6)+b;
                    } अन्यथा {
                        भङ्ग; /* sequence too short, stop with toULength<toULimit */
                    }
                } अन्यथा {
                    /* store the partial UTF-8 character, compatible with the regular UTF-8 converter */
                    source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                    यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                        utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                    }
                    utf8->गन्तव्यम्_Unicode_स्थितिः=c;
                    utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                    utf8->बहुलकम्=गन्तव्यम्_ULimit;
                    p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                    pFromUArgs->target=(वर्ण *)target;
                    प्रत्यागम;
                }
            }

            यदि(गन्तव्यम्_ULength!=गन्तव्यम्_ULimit) {
                /* error handling: illegal UTF-8 byte sequence */
                source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                यावत्(old_गन्तव्यम्_ULength<गन्तव्यम्_ULength) {
                    utf8->गन्तव्यम्_UBytes[old_गन्तव्यम्_ULength++]=*source++;
                }
                utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
                pFromUArgs->target=(वर्ण *)target;
                *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                प्रत्यागम;
            }

            /* copy the legal byte sequence to the target */
            {
                int8_t i;

                प्रति(i=0; i<old_गन्तव्यम्_ULength; ++i) {
                    *target++=utf8->गन्तव्यम्_UBytes[i];
                }
                source-=(गन्तव्यम्_ULength-old_गन्तव्यम्_ULength);
                प्रति(; i<गन्तव्यम्_ULength; ++i) {
                    *target++=*source++;
                }
                count-=गन्तव्यम्_ULength;
            }
        }
    }
    U_ASSERT(count>=0);

    यदि(U_SUCCESS(*p_Error_संहिता) && source<source_सीमा) {
        यदि(target==(ध्रुव uint8_t *)pFromUArgs->target_सीमा) {
            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        } अन्यथा {
            b=*source;
            गन्तव्यम्_ULimit=U8_गणनम्_अष्टकानि(b);
            यदि(गन्तव्यम्_ULimit>(source_सीमा-source)) {
                /* collect a truncated byte sequence */
                गन्तव्यम्_ULength=0;
                c=b;
                प्रति(;;) {
                    utf8->गन्तव्यम्_UBytes[गन्तव्यम्_ULength++]=b;
                    यदि(++source==source_सीमा) {
                        /* partial byte sequence at end of source */
                        utf8->गन्तव्यम्_Unicode_स्थितिः=c;
                        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                        utf8->बहुलकम्=गन्तव्यम्_ULimit;
                        भङ्ग;
                    } अन्यथा यदि(!icu::UTF8::isValidTrail(c, b=*source, गन्तव्यम्_ULength, गन्तव्यम्_ULimit)) {
                        utf8->गन्तव्यम्_ULength=गन्तव्यम्_ULength;
                        *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                        भङ्ग;
                    }
                    c=(c<<6)+b;
                }
            } अन्यथा {
                /* partial-sequence target overflow: fall back to the pivoting implementation */
                *p_Error_संहिता=U_USING_DEFAULT_WARNING;
            }
        }
    }

    /* write back the updated pointers */
    p_गन्तव्यम्_UArgs->source=(वर्ण *)source;
    pFromUArgs->target=(वर्ण *)target;
}

U_CDECL_अन्तः

/* UTF-8 converter data ----------------------------------------------------- */

स्थिर ध्रुव UConverterImpl _UTF8Impl={
    UCNV_UTF8,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    ucnv_गन्तव्यम्_Unicode_UTF8,
    ucnv_गन्तव्यम्_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    ucnv_get_अग्र_UChar_UTF8,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Non_Surrogate_Unicode_समुच्चयः,

    ucnv_UTF8FromUTF8,
    ucnv_UTF8FromUTF8
};

/* The 1208 CCSID refers to any version of Unicode of UTF-8 */
स्थिर ध्रुव UConverter_Static_दत्त UTF8_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "UTF-8",
    1208, UCNV_IBM, UCNV_UTF8,
    1, 3, /* max 3 bytes per char16_t from UTF-8 (4 bytes from surrogate _pair_) */
    { 0xef, 0xbf, 0xbd, 0 },3,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverter_Shared_दत्त UTF8_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&UTF8_Static_दत्त, &_UTF8Impl);

/* CESU-8 converter data ---------------------------------------------------- */

स्थिर ध्रुव UConverterImpl _CESU8Impl={
    UCNV_CESU8,

    शून्यसूचकः,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,

    ucnv_गन्तव्यम्_Unicode_UTF8,
    ucnv_गन्तव्यम्_Unicode_UTF8_OFFSETS_LOGIC,
    ucnv_fromUnicode_UTF8,
    ucnv_fromUnicode_UTF8_OFFSETS_LOGIC,
    शून्यसूचकः,

    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    शून्यसूचकः,
    ucnv_get_Complete_Unicode_समुच्चयः,

    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त CESU8_Static_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "CESU-8",
    9400, /* CCSID for CESU-8 */
    UCNV_UNKNOWN, UCNV_CESU8, 1, 3,
    { 0xef, 0xbf, 0xbd, 0 },3,असत्य,असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};


ध्रुव UConverter_Shared_दत्त CESU8_दत्त=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&CESU8_Static_दत्त, &_CESU8Impl);

#यदि_अन्तः
