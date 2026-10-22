// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*  
**********************************************************************
*   Copyright (C) 2000-2015, International Business Machines
*   Corporation and others.  All Rights Reserved.
**********************************************************************
*   file name:  ucnvhz.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000oct16
*   created by: Ram Viswanadha
*   10/31/2000  Ram     Implemented offsets logic function
*   
*/

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "cmemory.h"
#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/uset.h"
#شامل "unicode/utf16.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "ucnv_imp.h"

#تعریف_کرو UCNV_TILDE 0x7E          /* ~ */
#تعریف_کرو UCNV_OPEN_BRACE 0x7B     /* { */
#تعریف_کرو UCNV_CLOSE_BRACE 0x7D   /* } */
#تعریف_کرو SB_ESCAPE    "\x7E\x7D"
#تعریف_کرو DB_ESCAPE    "\x7E\x7B"
#تعریف_کرو TILDE_ESCAPE "\x7E\x7E"
#تعریف_کرو ESC_LEN       2


#تعریف_کرو CONCAT_ESCAPE_MACRO(دلائل_2, ہدف_اشاریہ,ہدف_لمبائی,str_مقصد_Append, err, len,ماخذ_اشاریہ) UPRV_بلاک_MACRO_BEGIN {      \
    جب(len-->0){                                                                                                         \
        اگر(ہدف_اشاریہ < ہدف_لمبائی){                                                                                     \
            دلائل_2->ہدف[ہدف_اشاریہ] = (غیرعلامتی حرف) *str_مقصد_Append;                                                       \
            اگر(دلائل_2->offsets!=خالی_اشاریہ){                                                                                        \
                *(offsets++) = ماخذ_اشاریہ-1;                                                                               \
            }                                                                                                               \
            ہدف_اشاریہ++;                                                                                                  \
        }                                                                                                                   \
        ورنہ{                                                                                                               \
            دلائل_2->converter->char_Error_بفر[(صحیح)دلائل_2->converter->char_Error_بفر_لمبائی++] = (غیرعلامتی حرف) *str_مقصد_Append; \
            *err =U_بفر_حد_سے_تجاوز_ERROR;                                                                                  \
        }                                                                                                                   \
        str_مقصد_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_بلاک_MACRO_انتہا


قسمتعریف ساخت{
    UConverter* gbConverter;
    int32_t ہدف_اشاریہ;
    int32_t ماخذ_اشاریہ;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_ہدف_UChar_DBCS;
    UBool is_خالی_خانے_Segment;
}UConverter_ڈیٹا_HZ;


U_CDECL_BEGIN
ساکن خالی  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_بھرو_دلائل *p_دلائل, UError_رمز *error_رمز){
    UConverter *gbConverter;
    اگر(p_دلائل->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_رمز);  /* errorCode carries result */
        واپس;
    }
    gbConverter = ucnv_open("GBK", error_رمز);
    اگر(U_FAILURE(*error_رمز)) {
        واپس;
    }
    cnv->مقصد_Unicode_حالت = 0;
    cnv->from_Unicode_حالت= 0;
    cnv->منوال=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, حجم(UConverter_ڈیٹا_HZ));
    اگر(cnv->extraInfo != خالی_اشاریہ){
        ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    ورنہ {
        ucnv_close(gbConverter);
        *error_رمز = U_حافظہ_تخصیص_ERROR;
        واپس;
    }
}

ساکن خالی  U_CALLCONV
_HZClose(UConverter *cnv){
    اگر(cnv->extraInfo != خالی_اشاریہ) {
        ucnv_close (((UConverter_ڈیٹا_HZ *) (cnv->extraInfo))->gbConverter);
        اگر(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = خالی_اشاریہ;
    }
}

ساکن خالی  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        cnv->مقصد_Unicode_حالت = 0;
        cnv->منوال=0;
        اگر(cnv->extraInfo != خالی_اشاریہ){
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->isStateDBCS = جھوٹ;
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->is_خالی_خانے_Segment = جھوٹ;
        }
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        cnv->from_Unicode_حالت= 0;
        cnv->fromUChar32=0x0000; 
        اگر(cnv->extraInfo != خالی_اشاریہ){
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->isEscapeAppended = جھوٹ;
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->ہدف_اشاریہ = 0;
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->ماخذ_اشاریہ = 0;
            ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->is_ہدف_UChar_DBCS = جھوٹ;
        }
    }
}

/**************************************HZ Encoding*************************************************
* Rules for HZ encoding
* 
*   In ASCII mode, a byte is interpreted as an ASCII character, unless a
*   '~' is encountered. The character '~' is an escape character. By
*   convention, it must be immediately followed ONLY by '~', '{' or '\n'
*   (<LF>), with the following special meaning.

*   1. The escape sequence '~~' is interpreted as a '~'.
*   2. The escape-to-GB sequence '~{' switches the mode from ASCII to GB.
*   3. The escape sequence '~\n' is a line-continuation marker to be
*     consumed with no output produced.
*   In GB mode, characters are interpreted two bytes at a time as (pure)
*   GB codes until the escape-from-GB code '~}' is read. This code
*   switches the mode from GB back to ASCII.  (Note that the escape-
*   from-GB code '~}' ($7E7D) is outside the defined GB range.)
*
*   Source: RFC 1842
*
*   Note that the formal syntax in RFC 1842 is invalid. I assume that the
*   intended definition of single-byte-segment is as follows (pedberg):
*   single-byte-segment = single-byte-seq 1*single-byte-char
*/


ساکن خالی  U_CALLCONV
UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC(UConverter_مقصد_Unicode_دلائل *دلائل_2,
                                                            UError_رمز* err){
    حرف عارضی_Buf[2];
    ثابت حرف *my_ماخذ = ( حرف *) دلائل_2->ماخذ;
    char16_t *my_ہدف = دلائل_2->ہدف;
    ثابت حرف *my_ماخذ_حد_بندی = دلائل_2->ماخذ_حد_بندی;
    UChar32 ہدف_Uni_Char = 0x0000;
    int32_t my_ماخذ_Char = 0x0000;
    UConverter_ڈیٹا_HZ* my_ڈیٹا=(UConverter_ڈیٹا_HZ*)(دلائل_2->converter->extraInfo);
    عارضی_Buf[0]=0; 
    عارضی_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    جب(my_ماخذ< my_ماخذ_حد_بندی){
        
        اگر(my_ہدف < دلائل_2->ہدف_حد_بندی){
            
            my_ماخذ_Char= (غیرعلامتی حرف) *my_ماخذ++;

            اگر(دلائل_2->converter->منوال == UCNV_TILDE) {
                /* second byte after ~ */
                دلائل_2->converter->منوال=0;
                انتخاب(my_ماخذ_Char) {
                حالت 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    جاری;
                حالت UCNV_TILDE:
                    اگر(دلائل_2->offsets) {
                        دلائل_2->offsets[my_ہدف - دلائل_2->ہدف]=(int32_t)(my_ماخذ - دلائل_2->ماخذ - 2);
                    }
                    *(my_ہدف++)=(char16_t)my_ماخذ_Char;
                    my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ;
                    جاری;
                حالت UCNV_OPEN_BRACE:
                حالت UCNV_CLOSE_BRACE:
                    my_ڈیٹا->isStateDBCS = (my_ماخذ_Char == UCNV_OPEN_BRACE);
                    اگر (my_ڈیٹا->is_خالی_خانے_Segment) {
                        my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        دلائل_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                        دلائل_2->converter->مقصد_UBytes[0] = UCNV_TILDE;
                        دلائل_2->converter->مقصد_UBytes[1] = static_cast<uint8_t>(my_ماخذ_Char);
                        دلائل_2->converter->مقصد_ULength = 2;
                        دلائل_2->ہدف = my_ہدف;
                        دلائل_2->ماخذ = my_ماخذ;
                        واپس;
                    }
                    my_ڈیٹا->is_خالی_خانے_Segment = سچ;
                    جاری;
                طےشدہ:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    دلائل_2->converter->مقصد_UBytes[0] = UCNV_TILDE;
                    اگر( my_ڈیٹا->isStateDBCS ?
                            (0x21 <= my_ماخذ_Char && my_ماخذ_Char <= 0x7e) :
                            my_ماخذ_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        دلائل_2->converter->مقصد_ULength = 1;
                        --my_ماخذ;
                    } ورنہ {
                        /* Include the current byte in the illegal sequence. */
                        دلائل_2->converter->مقصد_UBytes[1] = static_cast<uint8_t>(my_ماخذ_Char);
                        دلائل_2->converter->مقصد_ULength = 2;
                    }
                    دلائل_2->ہدف = my_ہدف;
                    دلائل_2->ماخذ = my_ماخذ;
                    واپس;
                }
            } ورنہ اگر(my_ڈیٹا->isStateDBCS) {
                اگر(دلائل_2->converter->مقصد_Unicode_حالت == 0x00){
                    /* lead byte */
                    اگر(my_ماخذ_Char == UCNV_TILDE) {
                        دلائل_2->converter->منوال = UCNV_TILDE;
                    } ورنہ {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        دلائل_2->converter->مقصد_Unicode_حالت = (uint32_t) (my_ماخذ_Char | 0x100);
                        my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    جاری;
                }
                ورنہ{
                    /* trail byte */
                    صحیح leadIsOk, trailIsOk;
                    uint32_t leadByte = دلائل_2->converter->مقصد_Unicode_حالت & 0xff;
                    ہدف_Uni_Char = 0xffff;
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     *
                     * In HZ DBCS, if the second byte is in the 21..7e range,
                     * we report only the first byte as the illegal sequence.
                     * Otherwise we convert or report the pair of bytes.
                     */
                    leadIsOk = (uint8_t)(leadByte - 0x21) <= (0x7d - 0x21);
                    trailIsOk = (uint8_t)(my_ماخذ_Char - 0x21) <= (0x7e - 0x21);
                    اگر (leadIsOk && trailIsOk) {
                        عارضی_Buf[0] = (حرف) (leadByte+0x80) ;
                        عارضی_Buf[1] = (حرف) (my_ماخذ_Char+0x80);
                        ہدف_Uni_Char = ucnv_MBCSSimple_Get_اگلا_UChar(my_ڈیٹا->gbConverter->shared_ڈیٹا,
                            عارضی_Buf, 2, دلائل_2->converter->useFallback);
                        my_ماخذ_Char= (leadByte << 8) | my_ماخذ_Char;
                    } ورنہ اگر (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_ماخذ;
                        my_ماخذ_Char = (int32_t)leadByte;
                    } ورنہ {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ماخذ_Char= 0x10000 | (leadByte << 8) | my_ماخذ_Char;
                    }
                    دلائل_2->converter->مقصد_Unicode_حالت =0x00;
                }
            }
            ورنہ{
                اگر(my_ماخذ_Char == UCNV_TILDE) {
                    دلائل_2->converter->منوال = UCNV_TILDE;
                    جاری;
                } ورنہ اگر(my_ماخذ_Char <= 0x7f) {
                    ہدف_Uni_Char = (char16_t)my_ماخذ_Char;  /* ASCII */
                    my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ; /* the segment has something valid */
                } ورنہ {
                    ہدف_Uni_Char = 0xffff;
                    my_ڈیٹا->is_خالی_خانے_Segment = جھوٹ; /* different error here, reset this to avoid spurious future error */
                }
            }
            اگر(ہدف_Uni_Char < 0xfffe){
                اگر(دلائل_2->offsets) {
                    دلائل_2->offsets[my_ہدف - دلائل_2->ہدف]=(int32_t)(my_ماخذ - دلائل_2->ماخذ - 1-(my_ڈیٹا->isStateDBCS));
                }

                *(my_ہدف++)=(char16_t)ہدف_Uni_Char;
            }
            ورنہ /* targetUniChar>=0xfffe */ {
                اگر(ہدف_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_ملا;
                }
                ورنہ{
                    *err = U_ILLEGAL_CHAR_ملا;
                }
                اگر(my_ماخذ_Char > 0xff){
                    دلائل_2->converter->مقصد_UBytes[0] = (uint8_t)(my_ماخذ_Char >> 8);
                    دلائل_2->converter->مقصد_UBytes[1] = (uint8_t)my_ماخذ_Char;
                    دلائل_2->converter->مقصد_ULength=2;
                }
                ورنہ{
                    دلائل_2->converter->مقصد_UBytes[0] = (uint8_t)my_ماخذ_Char;
                    دلائل_2->converter->مقصد_ULength=1;
                }
                توڑو;
            }
        }
        ورنہ{
            *err =U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
    }

    دلائل_2->ہدف = my_ہدف;
    دلائل_2->ماخذ = my_ماخذ;
}


ساکن خالی  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_دلائل * دلائل_2,
                                                      UError_رمز * err){
    ثابت char16_t *my_ماخذ = دلائل_2->ماخذ;
    حرف *my_ہدف = دلائل_2->ہدف;
    int32_t* offsets = دلائل_2->offsets;
    int32_t my_ماخذ_اشاریہ = 0;
    int32_t my_ہدف_اشاریہ = 0;
    int32_t ہدف_لمبائی = (int32_t)(دلائل_2->ہدف_حد_بندی - my_ہدف);
    int32_t my_ماخذ_لمبائی = (int32_t)(دلائل_2->ماخذ_حد_بندی - دلائل_2->ماخذ);
    uint32_t ہدف_Uni_Char = 0x0000;
    UChar32 my_ماخذ_Char = 0x0000;
    UConverter_ڈیٹا_HZ *my_Converter_ڈیٹا=(UConverter_ڈیٹا_HZ*)دلائل_2->converter->extraInfo;
    UBool is_ہدف_UChar_DBCS = my_Converter_ڈیٹا->is_ہدف_UChar_DBCS;
    UBool old_Is_ہدف_UChar_DBCS;
    صحیح len =0;
    ثابت حرف* escSeq=خالی_اشاریہ;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    اگر(دلائل_2->converter->fromUChar32!=0 && my_ہدف_اشاریہ < ہدف_لمبائی) {
        جاؤ getTrail;
    }
    /*writing the char to the output stream */
    جب (my_ماخذ_اشاریہ < my_ماخذ_لمبائی){
        ہدف_Uni_Char = missingCharMarker;
        اگر (my_ہدف_اشاریہ < ہدف_لمبائی){
            
            my_ماخذ_Char = my_ماخذ[my_ماخذ_اشاریہ++];
            

            old_Is_ہدف_UChar_DBCS = is_ہدف_UChar_DBCS;
            اگر(my_ماخذ_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(دلائل_2, my_ہدف_اشاریہ, ہدف_لمبائی, escSeq,err,len,my_ماخذ_اشاریہ);
                جاری;
            } ورنہ اگر(my_ماخذ_Char <= 0x7f) {
                ہدف_Uni_Char = my_ماخذ_Char;
            } ورنہ {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_ڈیٹا->gbConverter->shared_ڈیٹا,
                    my_ماخذ_Char,&ہدف_Uni_Char,دلائل_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                اگر( length == 2 &&
                    (uint16_t)(ہدف_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(ہدف_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    ہدف_Uni_Char -= 0x8080;
                } ورنہ {
                    ہدف_Uni_Char = missingCharMarker;
                }
            }
            اگر (ہدف_Uni_Char != missingCharMarker){
               my_Converter_ڈیٹا->is_ہدف_UChar_DBCS = is_ہدف_UChar_DBCS = ہدف_Uni_Char > 0x00FF;
                 اگر(old_Is_ہدف_UChar_DBCS != is_ہدف_UChar_DBCS || !my_Converter_ڈیٹا->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    اگر(!is_ہدف_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلائل_2, my_ہدف_اشاریہ, ہدف_لمبائی, escSeq,err,len,my_ماخذ_اشاریہ);
                        my_Converter_ڈیٹا->isEscapeAppended = سچ;
                    }
                    ورنہ{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلائل_2, my_ہدف_اشاریہ, ہدف_لمبائی, escSeq,err,len,my_ماخذ_اشاریہ);
                        my_Converter_ڈیٹا->isEscapeAppended = سچ;
                        
                    }
                }
            
                اگر(is_ہدف_UChar_DBCS){
                    اگر( my_ہدف_اشاریہ <ہدف_لمبائی){
                        my_ہدف[my_ہدف_اشاریہ++] =(حرف) (ہدف_Uni_Char >> 8);
                        اگر(offsets){
                            *(offsets++) = my_ماخذ_اشاریہ-1;
                        }
                        اگر(my_ہدف_اشاریہ < ہدف_لمبائی){
                            my_ہدف[my_ہدف_اشاریہ++] =(حرف) ہدف_Uni_Char;
                            اگر(offsets){
                                *(offsets++) = my_ماخذ_اشاریہ-1;
                            }
                        }ورنہ{
                            دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = (حرف) ہدف_Uni_Char;
                            *err = U_بفر_حد_سے_تجاوز_ERROR;
                        } 
                    }ورنہ{
                        دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] =(حرف) (ہدف_Uni_Char >> 8);
                        دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = (حرف) ہدف_Uni_Char;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                    }

                }ورنہ{
                    اگر( my_ہدف_اشاریہ <ہدف_لمبائی){
                        my_ہدف[my_ہدف_اشاریہ++] = (حرف) (ہدف_Uni_Char );
                        اگر(offsets){
                            *(offsets++) = my_ماخذ_اشاریہ-1;
                        }
                        
                    }ورنہ{
                        دلائل_2->converter->char_Error_بفر[دلائل_2->converter->char_Error_بفر_لمبائی++] = (حرف) ہدف_Uni_Char;
                        *err = U_بفر_حد_سے_تجاوز_ERROR;
                    }
                }

            }
            ورنہ{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                اگر(U16_IS_SURROGATE(my_ماخذ_Char)) {
                    اگر(U16_IS_SURROGATE_LEAD(my_ماخذ_Char)) {
                        دلائل_2->converter->fromUChar32=my_ماخذ_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        اگر(my_ماخذ_اشاریہ <  my_ماخذ_لمبائی) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) دلائل_2->ماخذ[my_ماخذ_اشاریہ];
                            اگر(U16_IS_TRAIL(trail)) {
                                ++my_ماخذ_اشاریہ;
                                my_ماخذ_Char=U16_GET_SUPPLEMENTARY(دلائل_2->converter->fromUChar32, trail);
                                دلائل_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_ملا;
                                /* exit this condition tree */
                            } ورنہ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_ملا;
                            }
                        } ورنہ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } ورنہ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_ملا;
                    }
                } ورنہ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_ملا;
                }

                دلائل_2->converter->fromUChar32=my_ماخذ_Char;
                توڑو;
            }
        }
        ورنہ{
            *err = U_بفر_حد_سے_تجاوز_ERROR;
            توڑو;
        }
        ہدف_Uni_Char=missingCharMarker;
    }

    دلائل_2->ہدف += my_ہدف_اشاریہ;
    دلائل_2->ماخذ += my_ماخذ_اشاریہ;
    my_Converter_ڈیٹا->is_ہدف_UChar_DBCS = is_ہدف_UChar_DBCS;
}

ساکن خالی U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_دلائل *دلائل_2, int32_t انحراف_اشاریہ, UError_رمز *err) {
    UConverter *cnv = دلائل_2->converter;
    UConverter_ڈیٹا_HZ *conv_ڈیٹا=(UConverter_ڈیٹا_HZ *) cnv->extraInfo;
    حرف *p;
    حرف بفر[4];
    p = بفر;
    
    اگر( conv_ڈیٹا->is_ہدف_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_ڈیٹا->is_ہدف_UChar_DBCS=جھوٹ;
    }
    *p++= (حرف)cnv->subChars[0];

    ucnv_cb_From_UWrite_بائٹیں(دلائل_2,
                           بفر, (int32_t)(p - بفر),
                           انحراف_اشاریہ, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
ساخت cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_ڈیٹا_HZ mydata;
};


ساکن UConverter *  U_CALLCONV
HZ_محفوظ_Clone(ثابت UConverter *cnv, 
              خالی *ڈھیر_بفر, 
              int32_t *p_بفر_سائز, 
              UError_رمز *حالت_2)
{
    ساخت cloneHZStruct * localClone;
    int32_t size, بفر_سائز_Needed = حجم(ساخت cloneHZStruct);

    اگر (U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }

    اگر (*p_بفر_سائز == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_سائز = بفر_سائز_Needed;
        واپس خالی_اشاریہ;
    }

    localClone = (ساخت cloneHZStruct *)ڈھیر_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_ڈیٹا_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    /* deep-clone the sub-converter */
    size = (int32_t)حجم(UConverter);
    ((UConverter_ڈیٹا_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_محفوظ_Clone(((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, حالت_2);

    واپس &localClone->cnv;
}

ساکن خالی U_CALLCONV
HZ_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                  ثابت USetAdder *sa,
                  UConverter_Unicode_سیٹ which,
                  UError_رمز *p_Error_رمز) {
    /* HZ converts all of ASCII */
    sa->جوڑو_Range(sa->سیٹ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_سیٹ_For_Unicode(
        ((UConverter_ڈیٹا_HZ*)cnv->extraInfo)->gbConverter->shared_ڈیٹا,
        sa, which, UCNV_سیٹ_FILTER_HZ,
        p_Error_رمز);
}
U_CDECL_انتہا
ساکن ثابت UConverterImpl _HZImpl={

    UCNV_HZ,
    
    خالی_اشاریہ,
    خالی_اشاریہ,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    خالی_اشاریہ,
    
    خالی_اشاریہ,
    خالی_اشاریہ,
    _HZ_WriteSub,
    HZ_محفوظ_Clone,
    HZ_Get_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا HZStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        جھوٹ, 
        جھوٹ,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ثابت UConverter_Shared_ڈیٹا _HZData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&HZStatic_ڈیٹا, &_HZImpl);

#اگر_ختم /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
