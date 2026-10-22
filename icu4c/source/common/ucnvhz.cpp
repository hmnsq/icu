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

#وصف_ڪريو UCNV_TILDE 0x7E          /* ~ */
#وصف_ڪريو UCNV_OPEN_BRACE 0x7B     /* { */
#وصف_ڪريو UCNV_CLOSE_BRACE 0x7D   /* } */
#وصف_ڪريو SB_ESCAPE    "\x7E\x7D"
#وصف_ڪريو DB_ESCAPE    "\x7E\x7B"
#وصف_ڪريو TILDE_ESCAPE "\x7E\x7E"
#وصف_ڪريو ESC_LEN       2


#وصف_ڪريو CONCAT_ESCAPE_MACRO(دليلون_2, هدف_اشاريو,هدف_ڊيگهائي,str_منزل_Append, err, len,ذريعو_اشاريو) UPRV_بلاڪ_MACRO_BEGIN {      \
    طالما(len-->0){                                                                                                         \
        إذا(هدف_اشاريو < هدف_ڊيگهائي){                                                                                     \
            دليلون_2->هدف[هدف_اشاريو] = (بنانشان حرف) *str_منزل_Append;                                                       \
            إذا(دليلون_2->offsets!=خالي_اشارو){                                                                                        \
                *(offsets++) = ذريعو_اشاريو-1;                                                                               \
            }                                                                                                               \
            هدف_اشاريو++;                                                                                                  \
        }                                                                                                                   \
        وإلا{                                                                                                               \
            دليلون_2->converter->char_Error_بفر[(صحيح)دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = (بنانشان حرف) *str_منزل_Append; \
            *err =U_بفر_حد_کان_مٿي_ERROR;                                                                                  \
        }                                                                                                                   \
        str_منزل_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_بلاڪ_MACRO_پڇاڙي


تعريفنوع بنية{
    UConverter* gbConverter;
    int32_t هدف_اشاريو;
    int32_t ذريعو_اشاريو;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_هدف_UChar_DBCS;
    UBool is_خالي_جايون_Segment;
}UConverter_معلومات_HZ;


U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_ڀريو_دليلون *p_دليلون, UError_ڪوڊ *error_ڪوڊ){
    UConverter *gbConverter;
    إذا(p_دليلون->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_ڪوڊ);  /* errorCode carries result */
        أرجع;
    }
    gbConverter = ucnv_open("GBK", error_ڪوڊ);
    إذا(U_FAILURE(*error_ڪوڊ)) {
        أرجع;
    }
    cnv->منزل_Unicode_حالت = 0;
    cnv->from_Unicode_حالت= 0;
    cnv->گهڻائي=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, حجم(UConverter_معلومات_HZ));
    إذا(cnv->extraInfo != خالي_اشارو){
        ((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    وإلا {
        ucnv_close(gbConverter);
        *error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
        أرجع;
    }
}

ساكن فراغ  U_CALLCONV
_HZClose(UConverter *cnv){
    إذا(cnv->extraInfo != خالي_اشارو) {
        ucnv_close (((UConverter_معلومات_HZ *) (cnv->extraInfo))->gbConverter);
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = خالي_اشارو;
    }
}

ساكن فراغ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        cnv->منزل_Unicode_حالت = 0;
        cnv->گهڻائي=0;
        إذا(cnv->extraInfo != خالي_اشارو){
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->isStateDBCS = خطأ;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->is_خالي_جايون_Segment = خطأ;
        }
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        cnv->from_Unicode_حالت= 0;
        cnv->fromUChar32=0x0000; 
        إذا(cnv->extraInfo != خالي_اشارو){
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->isEscapeAppended = خطأ;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->هدف_اشاريو = 0;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->ذريعو_اشاريو = 0;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->is_هدف_UChar_DBCS = خطأ;
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
UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC(UConverter_منزل_Unicode_دليلون *دليلون_2,
                                                            UError_ڪوڊ* err){
    حرف عارضي_Buf[2];
    ثابت حرف *my_ذريعو = ( حرف *) دليلون_2->ذريعو;
    char16_t *my_هدف = دليلون_2->هدف;
    ثابت حرف *my_ذريعو_حد_بندي = دليلون_2->ذريعو_حد_بندي;
    UChar32 هدف_Uni_Char = 0x0000;
    int32_t my_ذريعو_Char = 0x0000;
    UConverter_معلومات_HZ* my_معلومات=(UConverter_معلومات_HZ*)(دليلون_2->converter->extraInfo);
    عارضي_Buf[0]=0; 
    عارضي_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    طالما(my_ذريعو< my_ذريعو_حد_بندي){
        
        إذا(my_هدف < دليلون_2->هدف_حد_بندي){
            
            my_ذريعو_Char= (بنانشان حرف) *my_ذريعو++;

            إذا(دليلون_2->converter->گهڻائي == UCNV_TILDE) {
                /* second byte after ~ */
                دليلون_2->converter->گهڻائي=0;
                بدل(my_ذريعو_Char) {
                حالة 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    استمر;
                حالة UCNV_TILDE:
                    إذا(دليلون_2->offsets) {
                        دليلون_2->offsets[my_هدف - دليلون_2->هدف]=(int32_t)(my_ذريعو - دليلون_2->ذريعو - 2);
                    }
                    *(my_هدف++)=(char16_t)my_ذريعو_Char;
                    my_معلومات->is_خالي_جايون_Segment = خطأ;
                    استمر;
                حالة UCNV_OPEN_BRACE:
                حالة UCNV_CLOSE_BRACE:
                    my_معلومات->isStateDBCS = (my_ذريعو_Char == UCNV_OPEN_BRACE);
                    إذا (my_معلومات->is_خالي_جايون_Segment) {
                        my_معلومات->is_خالي_جايون_Segment = خطأ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        دليلون_2->converter->منزل_UCallback_Reason = UCNV_IRREGULAR;
                        دليلون_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                        دليلون_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_ذريعو_Char);
                        دليلون_2->converter->منزل_ULength = 2;
                        دليلون_2->هدف = my_هدف;
                        دليلون_2->ذريعو = my_ذريعو;
                        أرجع;
                    }
                    my_معلومات->is_خالي_جايون_Segment = سچ;
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
                    my_معلومات->is_خالي_جايون_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    دليلون_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                    إذا( my_معلومات->isStateDBCS ?
                            (0x21 <= my_ذريعو_Char && my_ذريعو_Char <= 0x7e) :
                            my_ذريعو_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        دليلون_2->converter->منزل_ULength = 1;
                        --my_ذريعو;
                    } وإلا {
                        /* Include the current byte in the illegal sequence. */
                        دليلون_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_ذريعو_Char);
                        دليلون_2->converter->منزل_ULength = 2;
                    }
                    دليلون_2->هدف = my_هدف;
                    دليلون_2->ذريعو = my_ذريعو;
                    أرجع;
                }
            } وإلا إذا(my_معلومات->isStateDBCS) {
                إذا(دليلون_2->converter->منزل_Unicode_حالت == 0x00){
                    /* lead byte */
                    إذا(my_ذريعو_Char == UCNV_TILDE) {
                        دليلون_2->converter->گهڻائي = UCNV_TILDE;
                    } وإلا {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        دليلون_2->converter->منزل_Unicode_حالت = (uint32_t) (my_ذريعو_Char | 0x100);
                        my_معلومات->is_خالي_جايون_Segment = خطأ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    استمر;
                }
                وإلا{
                    /* trail byte */
                    صحيح leadIsOk, trailIsOk;
                    uint32_t leadByte = دليلون_2->converter->منزل_Unicode_حالت & 0xff;
                    هدف_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_ذريعو_Char - 0x21) <= (0x7e - 0x21);
                    إذا (leadIsOk && trailIsOk) {
                        عارضي_Buf[0] = (حرف) (leadByte+0x80) ;
                        عارضي_Buf[1] = (حرف) (my_ذريعو_Char+0x80);
                        هدف_Uni_Char = ucnv_MBCSSimple_Get_ايندڙ_UChar(my_معلومات->gbConverter->shared_معلومات,
                            عارضي_Buf, 2, دليلون_2->converter->useFallback);
                        my_ذريعو_Char= (leadByte << 8) | my_ذريعو_Char;
                    } وإلا إذا (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_ذريعو;
                        my_ذريعو_Char = (int32_t)leadByte;
                    } وإلا {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ذريعو_Char= 0x10000 | (leadByte << 8) | my_ذريعو_Char;
                    }
                    دليلون_2->converter->منزل_Unicode_حالت =0x00;
                }
            }
            وإلا{
                إذا(my_ذريعو_Char == UCNV_TILDE) {
                    دليلون_2->converter->گهڻائي = UCNV_TILDE;
                    استمر;
                } وإلا إذا(my_ذريعو_Char <= 0x7f) {
                    هدف_Uni_Char = (char16_t)my_ذريعو_Char;  /* ASCII */
                    my_معلومات->is_خالي_جايون_Segment = خطأ; /* the segment has something valid */
                } وإلا {
                    هدف_Uni_Char = 0xffff;
                    my_معلومات->is_خالي_جايون_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                }
            }
            إذا(هدف_Uni_Char < 0xfffe){
                إذا(دليلون_2->offsets) {
                    دليلون_2->offsets[my_هدف - دليلون_2->هدف]=(int32_t)(my_ذريعو - دليلون_2->ذريعو - 1-(my_معلومات->isStateDBCS));
                }

                *(my_هدف++)=(char16_t)هدف_Uni_Char;
            }
            وإلا /* targetUniChar>=0xfffe */ {
                إذا(هدف_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_مليو;
                }
                وإلا{
                    *err = U_ILLEGAL_CHAR_مليو;
                }
                إذا(my_ذريعو_Char > 0xff){
                    دليلون_2->converter->منزل_UBytes[0] = (uint8_t)(my_ذريعو_Char >> 8);
                    دليلون_2->converter->منزل_UBytes[1] = (uint8_t)my_ذريعو_Char;
                    دليلون_2->converter->منزل_ULength=2;
                }
                وإلا{
                    دليلون_2->converter->منزل_UBytes[0] = (uint8_t)my_ذريعو_Char;
                    دليلون_2->converter->منزل_ULength=1;
                }
                اكسر;
            }
        }
        وإلا{
            *err =U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
    }

    دليلون_2->هدف = my_هدف;
    دليلون_2->ذريعو = my_ذريعو;
}


ساكن فراغ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_دليلون * دليلون_2,
                                                      UError_ڪوڊ * err){
    ثابت char16_t *my_ذريعو = دليلون_2->ذريعو;
    حرف *my_هدف = دليلون_2->هدف;
    int32_t* offsets = دليلون_2->offsets;
    int32_t my_ذريعو_اشاريو = 0;
    int32_t my_هدف_اشاريو = 0;
    int32_t هدف_ڊيگهائي = (int32_t)(دليلون_2->هدف_حد_بندي - my_هدف);
    int32_t my_ذريعو_ڊيگهائي = (int32_t)(دليلون_2->ذريعو_حد_بندي - دليلون_2->ذريعو);
    uint32_t هدف_Uni_Char = 0x0000;
    UChar32 my_ذريعو_Char = 0x0000;
    UConverter_معلومات_HZ *my_Converter_معلومات=(UConverter_معلومات_HZ*)دليلون_2->converter->extraInfo;
    UBool is_هدف_UChar_DBCS = my_Converter_معلومات->is_هدف_UChar_DBCS;
    UBool old_Is_هدف_UChar_DBCS;
    صحيح len =0;
    ثابت حرف* escSeq=خالي_اشارو;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    إذا(دليلون_2->converter->fromUChar32!=0 && my_هدف_اشاريو < هدف_ڊيگهائي) {
        اذهب getTrail;
    }
    /*writing the char to the output stream */
    طالما (my_ذريعو_اشاريو < my_ذريعو_ڊيگهائي){
        هدف_Uni_Char = missingCharMarker;
        إذا (my_هدف_اشاريو < هدف_ڊيگهائي){
            
            my_ذريعو_Char = my_ذريعو[my_ذريعو_اشاريو++];
            

            old_Is_هدف_UChar_DBCS = is_هدف_UChar_DBCS;
            إذا(my_ذريعو_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(دليلون_2, my_هدف_اشاريو, هدف_ڊيگهائي, escSeq,err,len,my_ذريعو_اشاريو);
                استمر;
            } وإلا إذا(my_ذريعو_Char <= 0x7f) {
                هدف_Uni_Char = my_ذريعو_Char;
            } وإلا {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_معلومات->gbConverter->shared_معلومات,
                    my_ذريعو_Char,&هدف_Uni_Char,دليلون_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                إذا( length == 2 &&
                    (uint16_t)(هدف_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(هدف_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    هدف_Uni_Char -= 0x8080;
                } وإلا {
                    هدف_Uni_Char = missingCharMarker;
                }
            }
            إذا (هدف_Uni_Char != missingCharMarker){
               my_Converter_معلومات->is_هدف_UChar_DBCS = is_هدف_UChar_DBCS = هدف_Uni_Char > 0x00FF;
                 إذا(old_Is_هدف_UChar_DBCS != is_هدف_UChar_DBCS || !my_Converter_معلومات->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    إذا(!is_هدف_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دليلون_2, my_هدف_اشاريو, هدف_ڊيگهائي, escSeq,err,len,my_ذريعو_اشاريو);
                        my_Converter_معلومات->isEscapeAppended = سچ;
                    }
                    وإلا{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دليلون_2, my_هدف_اشاريو, هدف_ڊيگهائي, escSeq,err,len,my_ذريعو_اشاريو);
                        my_Converter_معلومات->isEscapeAppended = سچ;
                        
                    }
                }
            
                إذا(is_هدف_UChar_DBCS){
                    إذا( my_هدف_اشاريو <هدف_ڊيگهائي){
                        my_هدف[my_هدف_اشاريو++] =(حرف) (هدف_Uni_Char >> 8);
                        إذا(offsets){
                            *(offsets++) = my_ذريعو_اشاريو-1;
                        }
                        إذا(my_هدف_اشاريو < هدف_ڊيگهائي){
                            my_هدف[my_هدف_اشاريو++] =(حرف) هدف_Uni_Char;
                            إذا(offsets){
                                *(offsets++) = my_ذريعو_اشاريو-1;
                            }
                        }وإلا{
                            دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = (حرف) هدف_Uni_Char;
                            *err = U_بفر_حد_کان_مٿي_ERROR;
                        } 
                    }وإلا{
                        دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] =(حرف) (هدف_Uni_Char >> 8);
                        دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = (حرف) هدف_Uni_Char;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                    }

                }وإلا{
                    إذا( my_هدف_اشاريو <هدف_ڊيگهائي){
                        my_هدف[my_هدف_اشاريو++] = (حرف) (هدف_Uni_Char );
                        إذا(offsets){
                            *(offsets++) = my_ذريعو_اشاريو-1;
                        }
                        
                    }وإلا{
                        دليلون_2->converter->char_Error_بفر[دليلون_2->converter->char_Error_بفر_ڊيگهائي++] = (حرف) هدف_Uni_Char;
                        *err = U_بفر_حد_کان_مٿي_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(my_ذريعو_Char)) {
                    إذا(U16_IS_SURROGATE_LEAD(my_ذريعو_Char)) {
                        دليلون_2->converter->fromUChar32=my_ذريعو_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(my_ذريعو_اشاريو <  my_ذريعو_ڊيگهائي) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) دليلون_2->ذريعو[my_ذريعو_اشاريو];
                            إذا(U16_IS_TRAIL(trail)) {
                                ++my_ذريعو_اشاريو;
                                my_ذريعو_Char=U16_GET_SUPPLEMENTARY(دليلون_2->converter->fromUChar32, trail);
                                دليلون_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_مليو;
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_مليو;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_مليو;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_مليو;
                }

                دليلون_2->converter->fromUChar32=my_ذريعو_Char;
                اكسر;
            }
        }
        وإلا{
            *err = U_بفر_حد_کان_مٿي_ERROR;
            اكسر;
        }
        هدف_Uni_Char=missingCharMarker;
    }

    دليلون_2->هدف += my_هدف_اشاريو;
    دليلون_2->ذريعو += my_ذريعو_اشاريو;
    my_Converter_معلومات->is_هدف_UChar_DBCS = is_هدف_UChar_DBCS;
}

ساكن فراغ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_دليلون *دليلون_2, int32_t وٿي_اشاريو, UError_ڪوڊ *err) {
    UConverter *cnv = دليلون_2->converter;
    UConverter_معلومات_HZ *conv_معلومات=(UConverter_معلومات_HZ *) cnv->extraInfo;
    حرف *p;
    حرف بفر[4];
    p = بفر;
    
    إذا( conv_معلومات->is_هدف_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_معلومات->is_هدف_UChar_DBCS=خطأ;
    }
    *p++= (حرف)cnv->subChars[0];

    ucnv_cb_From_UWrite_بائيٽون(دليلون_2,
                           بفر, (int32_t)(p - بفر),
                           وٿي_اشاريو, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
بنية cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_معلومات_HZ mydata;
};


ساكن UConverter *  U_CALLCONV
HZ_محفوظ_Clone(ثابت UConverter *cnv, 
              فراغ *ڍير_بفر, 
              int32_t *p_بفر_سائيز, 
              UError_ڪوڊ *حالت)
{
    بنية cloneHZStruct * localClone;
    int32_t size, بفر_سائيز_Needed = حجم(بنية cloneHZStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }

    إذا (*p_بفر_سائيز == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_سائيز = بفر_سائيز_Needed;
        أرجع خالي_اشارو;
    }

    localClone = (بنية cloneHZStruct *)ڍير_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_معلومات_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    /* deep-clone the sub-converter */
    size = (int32_t)حجم(UConverter);
    ((UConverter_معلومات_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_محفوظ_Clone(((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, حالت);

    أرجع &localClone->cnv;
}

ساكن فراغ U_CALLCONV
HZ_Get_Unicode_سيٽ(ثابت UConverter *cnv,
                  ثابت USetAdder *sa,
                  UConverter_Unicode_سيٽ which,
                  UError_ڪوڊ *p_Error_ڪوڊ) {
    /* HZ converts all of ASCII */
    sa->جوڙيو_Range(sa->سيٽ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_سيٽ_For_Unicode(
        ((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter->shared_معلومات,
        sa, which, UCNV_سيٽ_FILTER_HZ,
        p_Error_ڪوڊ);
}
U_CDECL_پڇاڙي
ساكن ثابت UConverterImpl _HZImpl={

    UCNV_HZ,
    
    خالي_اشارو,
    خالي_اشارو,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    خالي_اشارو,
    
    خالي_اشارو,
    خالي_اشارو,
    _HZ_WriteSub,
    HZ_محفوظ_Clone,
    HZ_Get_Unicode_سيٽ,
    خالي_اشارو,
    خالي_اشارو
};

ساكن ثابت UConverter_Static_معلومات HZStatic_معلومات={
    حجم(UConverter_Static_معلومات),
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

ثابت UConverter_Shared_معلومات _HZData=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&HZStatic_معلومات, &_HZImpl);

#جيڪڏهن_پورو /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
