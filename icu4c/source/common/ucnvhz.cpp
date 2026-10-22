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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "cmemory.h"
#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/uset.h"
#ضمن "unicode/utf16.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "ucnv_imp.h"

#تعریف_کرو UCNV_TILDE 0x7E          /* ~ */
#تعریف_کرو UCNV_OPEN_BRACE 0x7B     /* { */
#تعریف_کرو UCNV_CLOSE_BRACE 0x7D   /* } */
#تعریف_کرو SB_ESCAPE    "\x7E\x7D"
#تعریف_کرو DB_ESCAPE    "\x7E\x7B"
#تعریف_کرو TILDE_ESCAPE "\x7E\x7E"
#تعریف_کرو ESC_LEN       2


#تعریف_کرو CONCAT_ESCAPE_MACRO(دلیلاں_2, نشانہ_اشاریہ,نشانہ_لمائی,str_منزل_Append, err, len,سروت_اشاریہ) UPRV_بلاک_MACRO_BEGIN {      \
    طالما(len-->0){                                                                                                         \
        إذا(نشانہ_اشاریہ < نشانہ_لمائی){                                                                                     \
            دلیلاں_2->نشانہ[نشانہ_اشاریہ] = (بنانشانی حرف) *str_منزل_Append;                                                       \
            إذا(دلیلاں_2->offsets!=خالی_سوچک){                                                                                        \
                *(offsets++) = سروت_اشاریہ-1;                                                                               \
            }                                                                                                               \
            نشانہ_اشاریہ++;                                                                                                  \
        }                                                                                                                   \
        وإلا{                                                                                                               \
            دلیلاں_2->converter->char_Error_بفر[(صحيح)دلیلاں_2->converter->char_Error_بفر_لمائی++] = (بنانشانی حرف) *str_منزل_Append; \
            *err =U_بفر_حد_توں_ودھ_ERROR;                                                                                  \
        }                                                                                                                   \
        str_منزل_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_بلاک_MACRO_سرا


تعريفنوع بنية{
    UConverter* gbConverter;
    int32_t نشانہ_اشاریہ;
    int32_t سروت_اشاریہ;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_نشانہ_UChar_DBCS;
    UBool is_خالی_تھاواں_Segment;
}UConverter_انکڑے_HZ;


U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_لدو_دلیلاں *p_دلیلاں, UError_کوڈ *error_کوڈ){
    UConverter *gbConverter;
    إذا(p_دلیلاں->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_کوڈ);  /* errorCode carries result */
        أرجع;
    }
    gbConverter = ucnv_open("GBK", error_کوڈ);
    إذا(U_FAILURE(*error_کوڈ)) {
        أرجع;
    }
    cnv->منزل_Unicode_حالت = 0;
    cnv->from_Unicode_حالت= 0;
    cnv->منوال=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, حجم(UConverter_انکڑے_HZ));
    إذا(cnv->extraInfo != خالی_سوچک){
        ((UConverter_انکڑے_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    وإلا {
        ucnv_close(gbConverter);
        *error_کوڈ = U_حافظہ_ونڈ_ERROR;
        أرجع;
    }
}

ساكن فراغ  U_CALLCONV
_HZClose(UConverter *cnv){
    إذا(cnv->extraInfo != خالی_سوچک) {
        ucnv_close (((UConverter_انکڑے_HZ *) (cnv->extraInfo))->gbConverter);
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = خالی_سوچک;
    }
}

ساكن فراغ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        cnv->منزل_Unicode_حالت = 0;
        cnv->منوال=0;
        إذا(cnv->extraInfo != خالی_سوچک){
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->isStateDBCS = خطأ;
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->is_خالی_تھاواں_Segment = خطأ;
        }
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        cnv->from_Unicode_حالت= 0;
        cnv->fromUChar32=0x0000; 
        إذا(cnv->extraInfo != خالی_سوچک){
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->isEscapeAppended = خطأ;
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->نشانہ_اشاریہ = 0;
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->سروت_اشاریہ = 0;
            ((UConverter_انکڑے_HZ*)cnv->extraInfo)->is_نشانہ_UChar_DBCS = خطأ;
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


ساكن فراغ  U_CALLCONV
UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC(UConverter_منزل_Unicode_دلیلاں *دلیلاں_2,
                                                            UError_کوڈ* err){
    حرف عارضی_Buf[2];
    ثابت حرف *my_سروت = ( حرف *) دلیلاں_2->سروت;
    char16_t *my_نشانہ = دلیلاں_2->نشانہ;
    ثابت حرف *my_سروت_حد_بندی = دلیلاں_2->سروت_حد_بندی;
    UChar32 نشانہ_Uni_Char = 0x0000;
    int32_t my_سروت_Char = 0x0000;
    UConverter_انکڑے_HZ* my_انکڑے=(UConverter_انکڑے_HZ*)(دلیلاں_2->converter->extraInfo);
    عارضی_Buf[0]=0; 
    عارضی_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    طالما(my_سروت< my_سروت_حد_بندی){
        
        إذا(my_نشانہ < دلیلاں_2->نشانہ_حد_بندی){
            
            my_سروت_Char= (بنانشانی حرف) *my_سروت++;

            إذا(دلیلاں_2->converter->منوال == UCNV_TILDE) {
                /* second byte after ~ */
                دلیلاں_2->converter->منوال=0;
                بدل(my_سروت_Char) {
                حالة 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    استمر;
                حالة UCNV_TILDE:
                    إذا(دلیلاں_2->offsets) {
                        دلیلاں_2->offsets[my_نشانہ - دلیلاں_2->نشانہ]=(int32_t)(my_سروت - دلیلاں_2->سروت - 2);
                    }
                    *(my_نشانہ++)=(char16_t)my_سروت_Char;
                    my_انکڑے->is_خالی_تھاواں_Segment = خطأ;
                    استمر;
                حالة UCNV_OPEN_BRACE:
                حالة UCNV_CLOSE_BRACE:
                    my_انکڑے->isStateDBCS = (my_سروت_Char == UCNV_OPEN_BRACE);
                    إذا (my_انکڑے->is_خالی_تھاواں_Segment) {
                        my_انکڑے->is_خالی_تھاواں_Segment = خطأ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        دلیلاں_2->converter->منزل_UCallback_Reason = UCNV_IRREGULAR;
                        دلیلاں_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                        دلیلاں_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_سروت_Char);
                        دلیلاں_2->converter->منزل_ULength = 2;
                        دلیلاں_2->نشانہ = my_نشانہ;
                        دلیلاں_2->سروت = my_سروت;
                        أرجع;
                    }
                    my_انکڑے->is_خالی_تھاواں_Segment = سچ;
                    استمر;
                افتراضي:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_انکڑے->is_خالی_تھاواں_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    دلیلاں_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                    إذا( my_انکڑے->isStateDBCS ?
                            (0x21 <= my_سروت_Char && my_سروت_Char <= 0x7e) :
                            my_سروت_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        دلیلاں_2->converter->منزل_ULength = 1;
                        --my_سروت;
                    } وإلا {
                        /* Include the current byte in the illegal sequence. */
                        دلیلاں_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_سروت_Char);
                        دلیلاں_2->converter->منزل_ULength = 2;
                    }
                    دلیلاں_2->نشانہ = my_نشانہ;
                    دلیلاں_2->سروت = my_سروت;
                    أرجع;
                }
            } وإلا إذا(my_انکڑے->isStateDBCS) {
                إذا(دلیلاں_2->converter->منزل_Unicode_حالت == 0x00){
                    /* lead byte */
                    إذا(my_سروت_Char == UCNV_TILDE) {
                        دلیلاں_2->converter->منوال = UCNV_TILDE;
                    } وإلا {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        دلیلاں_2->converter->منزل_Unicode_حالت = (uint32_t) (my_سروت_Char | 0x100);
                        my_انکڑے->is_خالی_تھاواں_Segment = خطأ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    استمر;
                }
                وإلا{
                    /* trail byte */
                    صحيح leadIsOk, trailIsOk;
                    uint32_t leadByte = دلیلاں_2->converter->منزل_Unicode_حالت & 0xff;
                    نشانہ_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_سروت_Char - 0x21) <= (0x7e - 0x21);
                    إذا (leadIsOk && trailIsOk) {
                        عارضی_Buf[0] = (حرف) (leadByte+0x80) ;
                        عارضی_Buf[1] = (حرف) (my_سروت_Char+0x80);
                        نشانہ_Uni_Char = ucnv_MBCSSimple_Get_اگلا_UChar(my_انکڑے->gbConverter->shared_انکڑے,
                            عارضی_Buf, 2, دلیلاں_2->converter->useFallback);
                        my_سروت_Char= (leadByte << 8) | my_سروت_Char;
                    } وإلا إذا (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_سروت;
                        my_سروت_Char = (int32_t)leadByte;
                    } وإلا {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_سروت_Char= 0x10000 | (leadByte << 8) | my_سروت_Char;
                    }
                    دلیلاں_2->converter->منزل_Unicode_حالت =0x00;
                }
            }
            وإلا{
                إذا(my_سروت_Char == UCNV_TILDE) {
                    دلیلاں_2->converter->منوال = UCNV_TILDE;
                    استمر;
                } وإلا إذا(my_سروت_Char <= 0x7f) {
                    نشانہ_Uni_Char = (char16_t)my_سروت_Char;  /* ASCII */
                    my_انکڑے->is_خالی_تھاواں_Segment = خطأ; /* the segment has something valid */
                } وإلا {
                    نشانہ_Uni_Char = 0xffff;
                    my_انکڑے->is_خالی_تھاواں_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                }
            }
            إذا(نشانہ_Uni_Char < 0xfffe){
                إذا(دلیلاں_2->offsets) {
                    دلیلاں_2->offsets[my_نشانہ - دلیلاں_2->نشانہ]=(int32_t)(my_سروت - دلیلاں_2->سروت - 1-(my_انکڑے->isStateDBCS));
                }

                *(my_نشانہ++)=(char16_t)نشانہ_Uni_Char;
            }
            وإلا /* targetUniChar>=0xfffe */ {
                إذا(نشانہ_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_لبھیا;
                }
                وإلا{
                    *err = U_ILLEGAL_CHAR_لبھیا;
                }
                إذا(my_سروت_Char > 0xff){
                    دلیلاں_2->converter->منزل_UBytes[0] = (uint8_t)(my_سروت_Char >> 8);
                    دلیلاں_2->converter->منزل_UBytes[1] = (uint8_t)my_سروت_Char;
                    دلیلاں_2->converter->منزل_ULength=2;
                }
                وإلا{
                    دلیلاں_2->converter->منزل_UBytes[0] = (uint8_t)my_سروت_Char;
                    دلیلاں_2->converter->منزل_ULength=1;
                }
                اكسر;
            }
        }
        وإلا{
            *err =U_بفر_حد_توں_ودھ_ERROR;
            اكسر;
        }
    }

    دلیلاں_2->نشانہ = my_نشانہ;
    دلیلاں_2->سروت = my_سروت;
}


ساكن فراغ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_دلیلاں * دلیلاں_2,
                                                      UError_کوڈ * err){
    ثابت char16_t *my_سروت = دلیلاں_2->سروت;
    حرف *my_نشانہ = دلیلاں_2->نشانہ;
    int32_t* offsets = دلیلاں_2->offsets;
    int32_t my_سروت_اشاریہ = 0;
    int32_t my_نشانہ_اشاریہ = 0;
    int32_t نشانہ_لمائی = (int32_t)(دلیلاں_2->نشانہ_حد_بندی - my_نشانہ);
    int32_t my_سروت_لمائی = (int32_t)(دلیلاں_2->سروت_حد_بندی - دلیلاں_2->سروت);
    uint32_t نشانہ_Uni_Char = 0x0000;
    UChar32 my_سروت_Char = 0x0000;
    UConverter_انکڑے_HZ *my_Converter_انکڑے=(UConverter_انکڑے_HZ*)دلیلاں_2->converter->extraInfo;
    UBool is_نشانہ_UChar_DBCS = my_Converter_انکڑے->is_نشانہ_UChar_DBCS;
    UBool old_Is_نشانہ_UChar_DBCS;
    صحيح len =0;
    ثابت حرف* escSeq=خالی_سوچک;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    إذا(دلیلاں_2->converter->fromUChar32!=0 && my_نشانہ_اشاریہ < نشانہ_لمائی) {
        اذهب getTrail;
    }
    /*writing the char to the output stream */
    طالما (my_سروت_اشاریہ < my_سروت_لمائی){
        نشانہ_Uni_Char = missingCharMarker;
        إذا (my_نشانہ_اشاریہ < نشانہ_لمائی){
            
            my_سروت_Char = my_سروت[my_سروت_اشاریہ++];
            

            old_Is_نشانہ_UChar_DBCS = is_نشانہ_UChar_DBCS;
            إذا(my_سروت_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(دلیلاں_2, my_نشانہ_اشاریہ, نشانہ_لمائی, escSeq,err,len,my_سروت_اشاریہ);
                استمر;
            } وإلا إذا(my_سروت_Char <= 0x7f) {
                نشانہ_Uni_Char = my_سروت_Char;
            } وإلا {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_انکڑے->gbConverter->shared_انکڑے,
                    my_سروت_Char,&نشانہ_Uni_Char,دلیلاں_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                إذا( length == 2 &&
                    (uint16_t)(نشانہ_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(نشانہ_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    نشانہ_Uni_Char -= 0x8080;
                } وإلا {
                    نشانہ_Uni_Char = missingCharMarker;
                }
            }
            إذا (نشانہ_Uni_Char != missingCharMarker){
               my_Converter_انکڑے->is_نشانہ_UChar_DBCS = is_نشانہ_UChar_DBCS = نشانہ_Uni_Char > 0x00FF;
                 إذا(old_Is_نشانہ_UChar_DBCS != is_نشانہ_UChar_DBCS || !my_Converter_انکڑے->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    إذا(!is_نشانہ_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلیلاں_2, my_نشانہ_اشاریہ, نشانہ_لمائی, escSeq,err,len,my_سروت_اشاریہ);
                        my_Converter_انکڑے->isEscapeAppended = سچ;
                    }
                    وإلا{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلیلاں_2, my_نشانہ_اشاریہ, نشانہ_لمائی, escSeq,err,len,my_سروت_اشاریہ);
                        my_Converter_انکڑے->isEscapeAppended = سچ;
                        
                    }
                }
            
                إذا(is_نشانہ_UChar_DBCS){
                    إذا( my_نشانہ_اشاریہ <نشانہ_لمائی){
                        my_نشانہ[my_نشانہ_اشاریہ++] =(حرف) (نشانہ_Uni_Char >> 8);
                        إذا(offsets){
                            *(offsets++) = my_سروت_اشاریہ-1;
                        }
                        إذا(my_نشانہ_اشاریہ < نشانہ_لمائی){
                            my_نشانہ[my_نشانہ_اشاریہ++] =(حرف) نشانہ_Uni_Char;
                            إذا(offsets){
                                *(offsets++) = my_سروت_اشاریہ-1;
                            }
                        }وإلا{
                            دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = (حرف) نشانہ_Uni_Char;
                            *err = U_بفر_حد_توں_ودھ_ERROR;
                        } 
                    }وإلا{
                        دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] =(حرف) (نشانہ_Uni_Char >> 8);
                        دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = (حرف) نشانہ_Uni_Char;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                    }

                }وإلا{
                    إذا( my_نشانہ_اشاریہ <نشانہ_لمائی){
                        my_نشانہ[my_نشانہ_اشاریہ++] = (حرف) (نشانہ_Uni_Char );
                        إذا(offsets){
                            *(offsets++) = my_سروت_اشاریہ-1;
                        }
                        
                    }وإلا{
                        دلیلاں_2->converter->char_Error_بفر[دلیلاں_2->converter->char_Error_بفر_لمائی++] = (حرف) نشانہ_Uni_Char;
                        *err = U_بفر_حد_توں_ودھ_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(my_سروت_Char)) {
                    إذا(U16_IS_SURROGATE_LEAD(my_سروت_Char)) {
                        دلیلاں_2->converter->fromUChar32=my_سروت_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(my_سروت_اشاریہ <  my_سروت_لمائی) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) دلیلاں_2->سروت[my_سروت_اشاریہ];
                            إذا(U16_IS_TRAIL(trail)) {
                                ++my_سروت_اشاریہ;
                                my_سروت_Char=U16_GET_SUPPLEMENTARY(دلیلاں_2->converter->fromUChar32, trail);
                                دلیلاں_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_لبھیا;
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_لبھیا;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_لبھیا;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_لبھیا;
                }

                دلیلاں_2->converter->fromUChar32=my_سروت_Char;
                اكسر;
            }
        }
        وإلا{
            *err = U_بفر_حد_توں_ودھ_ERROR;
            اكسر;
        }
        نشانہ_Uni_Char=missingCharMarker;
    }

    دلیلاں_2->نشانہ += my_نشانہ_اشاریہ;
    دلیلاں_2->سروت += my_سروت_اشاریہ;
    my_Converter_انکڑے->is_نشانہ_UChar_DBCS = is_نشانہ_UChar_DBCS;
}

ساكن فراغ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_دلیلاں *دلیلاں_2, int32_t وتھ_اشاریہ, UError_کوڈ *err) {
    UConverter *cnv = دلیلاں_2->converter;
    UConverter_انکڑے_HZ *conv_انکڑے=(UConverter_انکڑے_HZ *) cnv->extraInfo;
    حرف *p;
    حرف بفر[4];
    p = بفر;
    
    إذا( conv_انکڑے->is_نشانہ_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_انکڑے->is_نشانہ_UChar_DBCS=خطأ;
    }
    *p++= (حرف)cnv->subChars[0];

    ucnv_cb_From_UWrite_بائٹاں(دلیلاں_2,
                           بفر, (int32_t)(p - بفر),
                           وتھ_اشاریہ, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
بنية cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_انکڑے_HZ mydata;
};


ساكن UConverter *  U_CALLCONV
HZ_محفوظ_Clone(ثابت UConverter *cnv, 
              فراغ *ڈھیر_بفر, 
              int32_t *p_بفر_ناپ, 
              UError_کوڈ *حالت)
{
    بنية cloneHZStruct * localClone;
    int32_t size, بفر_ناپ_Needed = حجم(بنية cloneHZStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }

    إذا (*p_بفر_ناپ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_ناپ = بفر_ناپ_Needed;
        أرجع خالی_سوچک;
    }

    localClone = (بنية cloneHZStruct *)ڈھیر_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_انکڑے_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    /* deep-clone the sub-converter */
    size = (int32_t)حجم(UConverter);
    ((UConverter_انکڑے_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_محفوظ_Clone(((UConverter_انکڑے_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, حالت);

    أرجع &localClone->cnv;
}

ساكن فراغ U_CALLCONV
HZ_Get_Unicode_سیٹ(ثابت UConverter *cnv,
                  ثابت USetAdder *sa,
                  UConverter_Unicode_سیٹ which,
                  UError_کوڈ *p_Error_کوڈ) {
    /* HZ converts all of ASCII */
    sa->جوڑو_Range(sa->سیٹ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_سیٹ_For_Unicode(
        ((UConverter_انکڑے_HZ*)cnv->extraInfo)->gbConverter->shared_انکڑے,
        sa, which, UCNV_سیٹ_FILTER_HZ,
        p_Error_کوڈ);
}
U_CDECL_سرا
ساكن ثابت UConverterImpl _HZImpl={

    UCNV_HZ,
    
    خالی_سوچک,
    خالی_سوچک,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    خالی_سوچک,
    
    خالی_سوچک,
    خالی_سوچک,
    _HZ_WriteSub,
    HZ_محفوظ_Clone,
    HZ_Get_Unicode_سیٹ,
    خالی_سوچک,
    خالی_سوچک
};

ساكن ثابت UConverter_Static_انکڑے HZStatic_انکڑے={
    حجم(UConverter_Static_انکڑے),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        خطأ, 
        خطأ,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ثابت UConverter_Shared_انکڑے _HZData=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&HZStatic_انکڑے, &_HZImpl);

#جے_انت /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
