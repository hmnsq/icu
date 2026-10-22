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

#تعریف_کړه UCNV_TILDE 0x7E          /* ~ */
#تعریف_کړه UCNV_OPEN_BRACE 0x7B     /* { */
#تعریف_کړه UCNV_CLOSE_BRACE 0x7D   /* } */
#تعریف_کړه SB_ESCAPE    "\x7E\x7D"
#تعریف_کړه DB_ESCAPE    "\x7E\x7B"
#تعریف_کړه TILDE_ESCAPE "\x7E\x7E"
#تعریف_کړه ESC_LEN       2


#تعریف_کړه CONCAT_ESCAPE_MACRO(دلیلونه_2, موخه_شاخص,موخه_اوږدتیا,str_منزل_Append, err, len,سرچینه_شاخص) UPRV_بلاک_MACRO_BEGIN {      \
    طالما(len-->0){                                                                                                         \
        إذا(موخه_شاخص < موخه_اوږدتیا){                                                                                     \
            دلیلونه_2->موخه[موخه_شاخص] = (بینښه حرف) *str_منزل_Append;                                                       \
            إذا(دلیلونه_2->offsets!=تش_اشاره){                                                                                        \
                *(offsets++) = سرچینه_شاخص-1;                                                                               \
            }                                                                                                               \
            موخه_شاخص++;                                                                                                  \
        }                                                                                                                   \
        وإلا{                                                                                                               \
            دلیلونه_2->converter->char_Error_بفر[(صحيح)دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = (بینښه حرف) *str_منزل_Append; \
            *err =U_بفر_ډکېدنه_ERROR;                                                                                  \
        }                                                                                                                   \
        str_منزل_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_بلاک_MACRO_پای_ټکی


تعريفنوع بنية{
    UConverter* gbConverter;
    int32_t موخه_شاخص;
    int32_t سرچینه_شاخص;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_موخه_UChar_DBCS;
    UBool is_تشه_Segment;
}UConverter_معلومات_HZ;


U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_بارول_دلیلونه *p_دلیلونه, UError_کوډ *error_کوډ){
    UConverter *gbConverter;
    إذا(p_دلیلونه->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_کوډ);  /* errorCode carries result */
        أرجع;
    }
    gbConverter = ucnv_open("GBK", error_کوډ);
    إذا(U_FAILURE(*error_کوډ)) {
        أرجع;
    }
    cnv->منزل_Unicode_حالت = 0;
    cnv->from_Unicode_حالت= 0;
    cnv->ډېرپېښ=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, حجم(UConverter_معلومات_HZ));
    إذا(cnv->extraInfo != تش_اشاره){
        ((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    وإلا {
        ucnv_close(gbConverter);
        *error_کوډ = U_حافظه_ویش_ERROR;
        أرجع;
    }
}

ساكن فراغ  U_CALLCONV
_HZClose(UConverter *cnv){
    إذا(cnv->extraInfo != تش_اشاره) {
        ucnv_close (((UConverter_معلومات_HZ *) (cnv->extraInfo))->gbConverter);
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = تش_اشاره;
    }
}

ساكن فراغ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        cnv->منزل_Unicode_حالت = 0;
        cnv->ډېرپېښ=0;
        إذا(cnv->extraInfo != تش_اشاره){
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->isStateDBCS = خطأ;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->is_تشه_Segment = خطأ;
        }
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        cnv->from_Unicode_حالت= 0;
        cnv->fromUChar32=0x0000; 
        إذا(cnv->extraInfo != تش_اشاره){
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->isEscapeAppended = خطأ;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->موخه_شاخص = 0;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->سرچینه_شاخص = 0;
            ((UConverter_معلومات_HZ*)cnv->extraInfo)->is_موخه_UChar_DBCS = خطأ;
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
UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC(UConverter_منزل_Unicode_دلیلونه *دلیلونه_2,
                                                            UError_کوډ* err){
    حرف لنډمهاله_Buf[2];
    ثابت حرف *my_سرچینه = ( حرف *) دلیلونه_2->سرچینه;
    char16_t *my_موخه = دلیلونه_2->موخه;
    ثابت حرف *my_سرچینه_حد = دلیلونه_2->سرچینه_حد;
    UChar32 موخه_Uni_Char = 0x0000;
    int32_t my_سرچینه_Char = 0x0000;
    UConverter_معلومات_HZ* my_معلومات=(UConverter_معلومات_HZ*)(دلیلونه_2->converter->extraInfo);
    لنډمهاله_Buf[0]=0; 
    لنډمهاله_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    طالما(my_سرچینه< my_سرچینه_حد){
        
        إذا(my_موخه < دلیلونه_2->موخه_حد){
            
            my_سرچینه_Char= (بینښه حرف) *my_سرچینه++;

            إذا(دلیلونه_2->converter->ډېرپېښ == UCNV_TILDE) {
                /* second byte after ~ */
                دلیلونه_2->converter->ډېرپېښ=0;
                بدل(my_سرچینه_Char) {
                حالة 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    استمر;
                حالة UCNV_TILDE:
                    إذا(دلیلونه_2->offsets) {
                        دلیلونه_2->offsets[my_موخه - دلیلونه_2->موخه]=(int32_t)(my_سرچینه - دلیلونه_2->سرچینه - 2);
                    }
                    *(my_موخه++)=(char16_t)my_سرچینه_Char;
                    my_معلومات->is_تشه_Segment = خطأ;
                    استمر;
                حالة UCNV_OPEN_BRACE:
                حالة UCNV_CLOSE_BRACE:
                    my_معلومات->isStateDBCS = (my_سرچینه_Char == UCNV_OPEN_BRACE);
                    إذا (my_معلومات->is_تشه_Segment) {
                        my_معلومات->is_تشه_Segment = خطأ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        دلیلونه_2->converter->منزل_UCallback_Reason = UCNV_IRREGULAR;
                        دلیلونه_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                        دلیلونه_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_سرچینه_Char);
                        دلیلونه_2->converter->منزل_ULength = 2;
                        دلیلونه_2->موخه = my_موخه;
                        دلیلونه_2->سرچینه = my_سرچینه;
                        أرجع;
                    }
                    my_معلومات->is_تشه_Segment = ریښتیا;
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
                    my_معلومات->is_تشه_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    دلیلونه_2->converter->منزل_UBytes[0] = UCNV_TILDE;
                    إذا( my_معلومات->isStateDBCS ?
                            (0x21 <= my_سرچینه_Char && my_سرچینه_Char <= 0x7e) :
                            my_سرچینه_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        دلیلونه_2->converter->منزل_ULength = 1;
                        --my_سرچینه;
                    } وإلا {
                        /* Include the current byte in the illegal sequence. */
                        دلیلونه_2->converter->منزل_UBytes[1] = static_cast<uint8_t>(my_سرچینه_Char);
                        دلیلونه_2->converter->منزل_ULength = 2;
                    }
                    دلیلونه_2->موخه = my_موخه;
                    دلیلونه_2->سرچینه = my_سرچینه;
                    أرجع;
                }
            } وإلا إذا(my_معلومات->isStateDBCS) {
                إذا(دلیلونه_2->converter->منزل_Unicode_حالت == 0x00){
                    /* lead byte */
                    إذا(my_سرچینه_Char == UCNV_TILDE) {
                        دلیلونه_2->converter->ډېرپېښ = UCNV_TILDE;
                    } وإلا {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        دلیلونه_2->converter->منزل_Unicode_حالت = (uint32_t) (my_سرچینه_Char | 0x100);
                        my_معلومات->is_تشه_Segment = خطأ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    استمر;
                }
                وإلا{
                    /* trail byte */
                    صحيح leadIsOk, trailIsOk;
                    uint32_t leadByte = دلیلونه_2->converter->منزل_Unicode_حالت & 0xff;
                    موخه_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_سرچینه_Char - 0x21) <= (0x7e - 0x21);
                    إذا (leadIsOk && trailIsOk) {
                        لنډمهاله_Buf[0] = (حرف) (leadByte+0x80) ;
                        لنډمهاله_Buf[1] = (حرف) (my_سرچینه_Char+0x80);
                        موخه_Uni_Char = ucnv_MBCSSimple_Get_بل_UChar(my_معلومات->gbConverter->shared_معلومات,
                            لنډمهاله_Buf, 2, دلیلونه_2->converter->useFallback);
                        my_سرچینه_Char= (leadByte << 8) | my_سرچینه_Char;
                    } وإلا إذا (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_سرچینه;
                        my_سرچینه_Char = (int32_t)leadByte;
                    } وإلا {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_سرچینه_Char= 0x10000 | (leadByte << 8) | my_سرچینه_Char;
                    }
                    دلیلونه_2->converter->منزل_Unicode_حالت =0x00;
                }
            }
            وإلا{
                إذا(my_سرچینه_Char == UCNV_TILDE) {
                    دلیلونه_2->converter->ډېرپېښ = UCNV_TILDE;
                    استمر;
                } وإلا إذا(my_سرچینه_Char <= 0x7f) {
                    موخه_Uni_Char = (char16_t)my_سرچینه_Char;  /* ASCII */
                    my_معلومات->is_تشه_Segment = خطأ; /* the segment has something valid */
                } وإلا {
                    موخه_Uni_Char = 0xffff;
                    my_معلومات->is_تشه_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                }
            }
            إذا(موخه_Uni_Char < 0xfffe){
                إذا(دلیلونه_2->offsets) {
                    دلیلونه_2->offsets[my_موخه - دلیلونه_2->موخه]=(int32_t)(my_سرچینه - دلیلونه_2->سرچینه - 1-(my_معلومات->isStateDBCS));
                }

                *(my_موخه++)=(char16_t)موخه_Uni_Char;
            }
            وإلا /* targetUniChar>=0xfffe */ {
                إذا(موخه_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_وموندل;
                }
                وإلا{
                    *err = U_ILLEGAL_CHAR_وموندل;
                }
                إذا(my_سرچینه_Char > 0xff){
                    دلیلونه_2->converter->منزل_UBytes[0] = (uint8_t)(my_سرچینه_Char >> 8);
                    دلیلونه_2->converter->منزل_UBytes[1] = (uint8_t)my_سرچینه_Char;
                    دلیلونه_2->converter->منزل_ULength=2;
                }
                وإلا{
                    دلیلونه_2->converter->منزل_UBytes[0] = (uint8_t)my_سرچینه_Char;
                    دلیلونه_2->converter->منزل_ULength=1;
                }
                اكسر;
            }
        }
        وإلا{
            *err =U_بفر_ډکېدنه_ERROR;
            اكسر;
        }
    }

    دلیلونه_2->موخه = my_موخه;
    دلیلونه_2->سرچینه = my_سرچینه;
}


ساكن فراغ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_دلیلونه * دلیلونه_2,
                                                      UError_کوډ * err){
    ثابت char16_t *my_سرچینه = دلیلونه_2->سرچینه;
    حرف *my_موخه = دلیلونه_2->موخه;
    int32_t* offsets = دلیلونه_2->offsets;
    int32_t my_سرچینه_شاخص = 0;
    int32_t my_موخه_شاخص = 0;
    int32_t موخه_اوږدتیا = (int32_t)(دلیلونه_2->موخه_حد - my_موخه);
    int32_t my_سرچینه_اوږدتیا = (int32_t)(دلیلونه_2->سرچینه_حد - دلیلونه_2->سرچینه);
    uint32_t موخه_Uni_Char = 0x0000;
    UChar32 my_سرچینه_Char = 0x0000;
    UConverter_معلومات_HZ *my_Converter_معلومات=(UConverter_معلومات_HZ*)دلیلونه_2->converter->extraInfo;
    UBool is_موخه_UChar_DBCS = my_Converter_معلومات->is_موخه_UChar_DBCS;
    UBool old_Is_موخه_UChar_DBCS;
    صحيح len =0;
    ثابت حرف* escSeq=تش_اشاره;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    إذا(دلیلونه_2->converter->fromUChar32!=0 && my_موخه_شاخص < موخه_اوږدتیا) {
        اذهب getTrail;
    }
    /*writing the char to the output stream */
    طالما (my_سرچینه_شاخص < my_سرچینه_اوږدتیا){
        موخه_Uni_Char = missingCharMarker;
        إذا (my_موخه_شاخص < موخه_اوږدتیا){
            
            my_سرچینه_Char = my_سرچینه[my_سرچینه_شاخص++];
            

            old_Is_موخه_UChar_DBCS = is_موخه_UChar_DBCS;
            إذا(my_سرچینه_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(دلیلونه_2, my_موخه_شاخص, موخه_اوږدتیا, escSeq,err,len,my_سرچینه_شاخص);
                استمر;
            } وإلا إذا(my_سرچینه_Char <= 0x7f) {
                موخه_Uni_Char = my_سرچینه_Char;
            } وإلا {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_معلومات->gbConverter->shared_معلومات,
                    my_سرچینه_Char,&موخه_Uni_Char,دلیلونه_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                إذا( length == 2 &&
                    (uint16_t)(موخه_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(موخه_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    موخه_Uni_Char -= 0x8080;
                } وإلا {
                    موخه_Uni_Char = missingCharMarker;
                }
            }
            إذا (موخه_Uni_Char != missingCharMarker){
               my_Converter_معلومات->is_موخه_UChar_DBCS = is_موخه_UChar_DBCS = موخه_Uni_Char > 0x00FF;
                 إذا(old_Is_موخه_UChar_DBCS != is_موخه_UChar_DBCS || !my_Converter_معلومات->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    إذا(!is_موخه_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلیلونه_2, my_موخه_شاخص, موخه_اوږدتیا, escSeq,err,len,my_سرچینه_شاخص);
                        my_Converter_معلومات->isEscapeAppended = ریښتیا;
                    }
                    وإلا{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(دلیلونه_2, my_موخه_شاخص, موخه_اوږدتیا, escSeq,err,len,my_سرچینه_شاخص);
                        my_Converter_معلومات->isEscapeAppended = ریښتیا;
                        
                    }
                }
            
                إذا(is_موخه_UChar_DBCS){
                    إذا( my_موخه_شاخص <موخه_اوږدتیا){
                        my_موخه[my_موخه_شاخص++] =(حرف) (موخه_Uni_Char >> 8);
                        إذا(offsets){
                            *(offsets++) = my_سرچینه_شاخص-1;
                        }
                        إذا(my_موخه_شاخص < موخه_اوږدتیا){
                            my_موخه[my_موخه_شاخص++] =(حرف) موخه_Uni_Char;
                            إذا(offsets){
                                *(offsets++) = my_سرچینه_شاخص-1;
                            }
                        }وإلا{
                            دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = (حرف) موخه_Uni_Char;
                            *err = U_بفر_ډکېدنه_ERROR;
                        } 
                    }وإلا{
                        دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] =(حرف) (موخه_Uni_Char >> 8);
                        دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = (حرف) موخه_Uni_Char;
                        *err = U_بفر_ډکېدنه_ERROR;
                    }

                }وإلا{
                    إذا( my_موخه_شاخص <موخه_اوږدتیا){
                        my_موخه[my_موخه_شاخص++] = (حرف) (موخه_Uni_Char );
                        إذا(offsets){
                            *(offsets++) = my_سرچینه_شاخص-1;
                        }
                        
                    }وإلا{
                        دلیلونه_2->converter->char_Error_بفر[دلیلونه_2->converter->char_Error_بفر_اوږدتیا++] = (حرف) موخه_Uni_Char;
                        *err = U_بفر_ډکېدنه_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(my_سرچینه_Char)) {
                    إذا(U16_IS_SURROGATE_LEAD(my_سرچینه_Char)) {
                        دلیلونه_2->converter->fromUChar32=my_سرچینه_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(my_سرچینه_شاخص <  my_سرچینه_اوږدتیا) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) دلیلونه_2->سرچینه[my_سرچینه_شاخص];
                            إذا(U16_IS_TRAIL(trail)) {
                                ++my_سرچینه_شاخص;
                                my_سرچینه_Char=U16_GET_SUPPLEMENTARY(دلیلونه_2->converter->fromUChar32, trail);
                                دلیلونه_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_وموندل;
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_وموندل;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_وموندل;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_وموندل;
                }

                دلیلونه_2->converter->fromUChar32=my_سرچینه_Char;
                اكسر;
            }
        }
        وإلا{
            *err = U_بفر_ډکېدنه_ERROR;
            اكسر;
        }
        موخه_Uni_Char=missingCharMarker;
    }

    دلیلونه_2->موخه += my_موخه_شاخص;
    دلیلونه_2->سرچینه += my_سرچینه_شاخص;
    my_Converter_معلومات->is_موخه_UChar_DBCS = is_موخه_UChar_DBCS;
}

ساكن فراغ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_دلیلونه *دلیلونه_2, int32_t دننه_واټن_شاخص, UError_کوډ *err) {
    UConverter *cnv = دلیلونه_2->converter;
    UConverter_معلومات_HZ *conv_معلومات=(UConverter_معلومات_HZ *) cnv->extraInfo;
    حرف *p;
    حرف بفر[4];
    p = بفر;
    
    إذا( conv_معلومات->is_موخه_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_معلومات->is_موخه_UChar_DBCS=خطأ;
    }
    *p++= (حرف)cnv->subChars[0];

    ucnv_cb_From_UWrite_بایټونه(دلیلونه_2,
                           بفر, (int32_t)(p - بفر),
                           دننه_واټن_شاخص, err);
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
HZ_خوندي_Clone(ثابت UConverter *cnv, 
              فراغ *ډېری_بفر, 
              int32_t *p_بفر_اندازه, 
              UError_کوډ *حالت)
{
    بنية cloneHZStruct * localClone;
    int32_t size, بفر_اندازه_Needed = حجم(بنية cloneHZStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }

    إذا (*p_بفر_اندازه == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_اندازه = بفر_اندازه_Needed;
        أرجع تش_اشاره;
    }

    localClone = (بنية cloneHZStruct *)ډېری_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_معلومات_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ریښتیا;

    /* deep-clone the sub-converter */
    size = (int32_t)حجم(UConverter);
    ((UConverter_معلومات_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_خوندي_Clone(((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, حالت);

    أرجع &localClone->cnv;
}

ساكن فراغ U_CALLCONV
HZ_Get_Unicode_سېټ(ثابت UConverter *cnv,
                  ثابت USetAdder *sa,
                  UConverter_Unicode_سېټ which,
                  UError_کوډ *p_Error_کوډ) {
    /* HZ converts all of ASCII */
    sa->جمع_کړه_Range(sa->سېټ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_سېټ_For_Unicode(
        ((UConverter_معلومات_HZ*)cnv->extraInfo)->gbConverter->shared_معلومات,
        sa, which, UCNV_سېټ_FILTER_HZ,
        p_Error_کوډ);
}
U_CDECL_پای_ټکی
ساكن ثابت UConverterImpl _HZImpl={

    UCNV_HZ,
    
    تش_اشاره,
    تش_اشاره,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_منزل_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    تش_اشاره,
    
    تش_اشاره,
    تش_اشاره,
    _HZ_WriteSub,
    HZ_خوندي_Clone,
    HZ_Get_Unicode_سېټ,
    تش_اشاره,
    تش_اشاره
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

#د_که_پای /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
