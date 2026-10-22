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

#تعریف_کن UCNV_TILDE 0x7E          /* ~ */
#تعریف_کن UCNV_OPEN_BRACE 0x7B     /* { */
#تعریف_کن UCNV_CLOSE_BRACE 0x7D   /* } */
#تعریف_کن SB_ESCAPE    "\x7E\x7D"
#تعریف_کن DB_ESCAPE    "\x7E\x7B"
#تعریف_کن TILDE_ESCAPE "\x7E\x7E"
#تعریف_کن ESC_LEN       2


#تعریف_کن CONCAT_ESCAPE_MACRO(آرگومانها_2, هدف_شاخص,هدف_درازی,str_مقصد_Append, err, len,منبع_شاخص) UPRV_بلوک_MACRO_BEGIN {      \
    طالما(len-->0){                                                                                                         \
        إذا(هدف_شاخص < هدف_درازی){                                                                                     \
            آرگومانها_2->هدف[هدف_شاخص] = (بیعلامت حرف) *str_مقصد_Append;                                                       \
            إذا(آرگومانها_2->offsets!=اشاره_تهی){                                                                                        \
                *(offsets++) = منبع_شاخص-1;                                                                               \
            }                                                                                                               \
            هدف_شاخص++;                                                                                                  \
        }                                                                                                                   \
        وإلا{                                                                                                               \
            آرگومانها_2->converter->char_Error_میانگیر[(صحيح)آرگومانها_2->converter->char_Error_میانگیر_درازی++] = (بیعلامت حرف) *str_مقصد_Append; \
            *err =U_میانگیر_سرریز_ERROR;                                                                                  \
        }                                                                                                                   \
        str_مقصد_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_بلوک_MACRO_انتها


تعريفنوع بنية{
    UConverter* gbConverter;
    int32_t هدف_شاخص;
    int32_t منبع_شاخص;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_هدف_UChar_DBCS;
    UBool is_خالی_Segment;
}UConverter_داده_HZ;


U_CDECL_BEGIN
ساكن فراغ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_بارگیری_آرگومانها *p_آرگومانها, UError_کد *error_کد){
    UConverter *gbConverter;
    إذا(p_آرگومانها->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_کد);  /* errorCode carries result */
        أرجع;
    }
    gbConverter = ucnv_open("GBK", error_کد);
    إذا(U_FAILURE(*error_کد)) {
        أرجع;
    }
    cnv->مقصد_Unicode_وضعیت = 0;
    cnv->from_Unicode_وضعیت= 0;
    cnv->نما=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, حجم(UConverter_داده_HZ));
    إذا(cnv->extraInfo != اشاره_تهی){
        ((UConverter_داده_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    وإلا {
        ucnv_close(gbConverter);
        *error_کد = U_حافظه_تخصیص_ERROR;
        أرجع;
    }
}

ساكن فراغ  U_CALLCONV
_HZClose(UConverter *cnv){
    إذا(cnv->extraInfo != اشاره_تهی) {
        ucnv_close (((UConverter_داده_HZ *) (cnv->extraInfo))->gbConverter);
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = اشاره_تهی;
    }
}

ساكن فراغ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    إذا(choice<=UCNV_RESET_مقصد_UNICODE) {
        cnv->مقصد_Unicode_وضعیت = 0;
        cnv->نما=0;
        إذا(cnv->extraInfo != اشاره_تهی){
            ((UConverter_داده_HZ*)cnv->extraInfo)->isStateDBCS = خطأ;
            ((UConverter_داده_HZ*)cnv->extraInfo)->is_خالی_Segment = خطأ;
        }
    }
    إذا(choice!=UCNV_RESET_مقصد_UNICODE) {
        cnv->from_Unicode_وضعیت= 0;
        cnv->fromUChar32=0x0000; 
        إذا(cnv->extraInfo != اشاره_تهی){
            ((UConverter_داده_HZ*)cnv->extraInfo)->isEscapeAppended = خطأ;
            ((UConverter_داده_HZ*)cnv->extraInfo)->هدف_شاخص = 0;
            ((UConverter_داده_HZ*)cnv->extraInfo)->منبع_شاخص = 0;
            ((UConverter_داده_HZ*)cnv->extraInfo)->is_هدف_UChar_DBCS = خطأ;
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
UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC(UConverter_مقصد_Unicode_آرگومانها *آرگومانها_2,
                                                            UError_کد* err){
    حرف موقت_Buf[2];
    ثابت حرف *my_منبع = ( حرف *) آرگومانها_2->منبع;
    char16_t *my_هدف = آرگومانها_2->هدف;
    ثابت حرف *my_منبع_حد = آرگومانها_2->منبع_حد;
    UChar32 هدف_Uni_Char = 0x0000;
    int32_t my_منبع_Char = 0x0000;
    UConverter_داده_HZ* my_داده=(UConverter_داده_HZ*)(آرگومانها_2->converter->extraInfo);
    موقت_Buf[0]=0; 
    موقت_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    طالما(my_منبع< my_منبع_حد){
        
        إذا(my_هدف < آرگومانها_2->هدف_حد){
            
            my_منبع_Char= (بیعلامت حرف) *my_منبع++;

            إذا(آرگومانها_2->converter->نما == UCNV_TILDE) {
                /* second byte after ~ */
                آرگومانها_2->converter->نما=0;
                بدل(my_منبع_Char) {
                حالة 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    استمر;
                حالة UCNV_TILDE:
                    إذا(آرگومانها_2->offsets) {
                        آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف]=(int32_t)(my_منبع - آرگومانها_2->منبع - 2);
                    }
                    *(my_هدف++)=(char16_t)my_منبع_Char;
                    my_داده->is_خالی_Segment = خطأ;
                    استمر;
                حالة UCNV_OPEN_BRACE:
                حالة UCNV_CLOSE_BRACE:
                    my_داده->isStateDBCS = (my_منبع_Char == UCNV_OPEN_BRACE);
                    إذا (my_داده->is_خالی_Segment) {
                        my_داده->is_خالی_Segment = خطأ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        آرگومانها_2->converter->مقصد_UCallback_Reason = UCNV_IRREGULAR;
                        آرگومانها_2->converter->مقصد_UBytes[0] = UCNV_TILDE;
                        آرگومانها_2->converter->مقصد_UBytes[1] = static_cast<uint8_t>(my_منبع_Char);
                        آرگومانها_2->converter->مقصد_ULength = 2;
                        آرگومانها_2->هدف = my_هدف;
                        آرگومانها_2->منبع = my_منبع;
                        أرجع;
                    }
                    my_داده->is_خالی_Segment = درست;
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
                    my_داده->is_خالی_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    آرگومانها_2->converter->مقصد_UBytes[0] = UCNV_TILDE;
                    إذا( my_داده->isStateDBCS ?
                            (0x21 <= my_منبع_Char && my_منبع_Char <= 0x7e) :
                            my_منبع_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        آرگومانها_2->converter->مقصد_ULength = 1;
                        --my_منبع;
                    } وإلا {
                        /* Include the current byte in the illegal sequence. */
                        آرگومانها_2->converter->مقصد_UBytes[1] = static_cast<uint8_t>(my_منبع_Char);
                        آرگومانها_2->converter->مقصد_ULength = 2;
                    }
                    آرگومانها_2->هدف = my_هدف;
                    آرگومانها_2->منبع = my_منبع;
                    أرجع;
                }
            } وإلا إذا(my_داده->isStateDBCS) {
                إذا(آرگومانها_2->converter->مقصد_Unicode_وضعیت == 0x00){
                    /* lead byte */
                    إذا(my_منبع_Char == UCNV_TILDE) {
                        آرگومانها_2->converter->نما = UCNV_TILDE;
                    } وإلا {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        آرگومانها_2->converter->مقصد_Unicode_وضعیت = (uint32_t) (my_منبع_Char | 0x100);
                        my_داده->is_خالی_Segment = خطأ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    استمر;
                }
                وإلا{
                    /* trail byte */
                    صحيح leadIsOk, trailIsOk;
                    uint32_t leadByte = آرگومانها_2->converter->مقصد_Unicode_وضعیت & 0xff;
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
                    trailIsOk = (uint8_t)(my_منبع_Char - 0x21) <= (0x7e - 0x21);
                    إذا (leadIsOk && trailIsOk) {
                        موقت_Buf[0] = (حرف) (leadByte+0x80) ;
                        موقت_Buf[1] = (حرف) (my_منبع_Char+0x80);
                        هدف_Uni_Char = ucnv_MBCSSimple_Get_بعدی_UChar(my_داده->gbConverter->shared_داده,
                            موقت_Buf, 2, آرگومانها_2->converter->useFallback);
                        my_منبع_Char= (leadByte << 8) | my_منبع_Char;
                    } وإلا إذا (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_منبع;
                        my_منبع_Char = (int32_t)leadByte;
                    } وإلا {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_منبع_Char= 0x10000 | (leadByte << 8) | my_منبع_Char;
                    }
                    آرگومانها_2->converter->مقصد_Unicode_وضعیت =0x00;
                }
            }
            وإلا{
                إذا(my_منبع_Char == UCNV_TILDE) {
                    آرگومانها_2->converter->نما = UCNV_TILDE;
                    استمر;
                } وإلا إذا(my_منبع_Char <= 0x7f) {
                    هدف_Uni_Char = (char16_t)my_منبع_Char;  /* ASCII */
                    my_داده->is_خالی_Segment = خطأ; /* the segment has something valid */
                } وإلا {
                    هدف_Uni_Char = 0xffff;
                    my_داده->is_خالی_Segment = خطأ; /* different error here, reset this to avoid spurious future error */
                }
            }
            إذا(هدف_Uni_Char < 0xfffe){
                إذا(آرگومانها_2->offsets) {
                    آرگومانها_2->offsets[my_هدف - آرگومانها_2->هدف]=(int32_t)(my_منبع - آرگومانها_2->منبع - 1-(my_داده->isStateDBCS));
                }

                *(my_هدف++)=(char16_t)هدف_Uni_Char;
            }
            وإلا /* targetUniChar>=0xfffe */ {
                إذا(هدف_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_یافت;
                }
                وإلا{
                    *err = U_ILLEGAL_CHAR_یافت;
                }
                إذا(my_منبع_Char > 0xff){
                    آرگومانها_2->converter->مقصد_UBytes[0] = (uint8_t)(my_منبع_Char >> 8);
                    آرگومانها_2->converter->مقصد_UBytes[1] = (uint8_t)my_منبع_Char;
                    آرگومانها_2->converter->مقصد_ULength=2;
                }
                وإلا{
                    آرگومانها_2->converter->مقصد_UBytes[0] = (uint8_t)my_منبع_Char;
                    آرگومانها_2->converter->مقصد_ULength=1;
                }
                اكسر;
            }
        }
        وإلا{
            *err =U_میانگیر_سرریز_ERROR;
            اكسر;
        }
    }

    آرگومانها_2->هدف = my_هدف;
    آرگومانها_2->منبع = my_منبع;
}


ساكن فراغ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_آرگومانها * آرگومانها_2,
                                                      UError_کد * err){
    ثابت char16_t *my_منبع = آرگومانها_2->منبع;
    حرف *my_هدف = آرگومانها_2->هدف;
    int32_t* offsets = آرگومانها_2->offsets;
    int32_t my_منبع_شاخص = 0;
    int32_t my_هدف_شاخص = 0;
    int32_t هدف_درازی = (int32_t)(آرگومانها_2->هدف_حد - my_هدف);
    int32_t my_منبع_درازی = (int32_t)(آرگومانها_2->منبع_حد - آرگومانها_2->منبع);
    uint32_t هدف_Uni_Char = 0x0000;
    UChar32 my_منبع_Char = 0x0000;
    UConverter_داده_HZ *my_Converter_داده=(UConverter_داده_HZ*)آرگومانها_2->converter->extraInfo;
    UBool is_هدف_UChar_DBCS = my_Converter_داده->is_هدف_UChar_DBCS;
    UBool old_Is_هدف_UChar_DBCS;
    صحيح len =0;
    ثابت حرف* escSeq=اشاره_تهی;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    إذا(آرگومانها_2->converter->fromUChar32!=0 && my_هدف_شاخص < هدف_درازی) {
        اذهب getTrail;
    }
    /*writing the char to the output stream */
    طالما (my_منبع_شاخص < my_منبع_درازی){
        هدف_Uni_Char = missingCharMarker;
        إذا (my_هدف_شاخص < هدف_درازی){
            
            my_منبع_Char = my_منبع[my_منبع_شاخص++];
            

            old_Is_هدف_UChar_DBCS = is_هدف_UChar_DBCS;
            إذا(my_منبع_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(آرگومانها_2, my_هدف_شاخص, هدف_درازی, escSeq,err,len,my_منبع_شاخص);
                استمر;
            } وإلا إذا(my_منبع_Char <= 0x7f) {
                هدف_Uni_Char = my_منبع_Char;
            } وإلا {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_داده->gbConverter->shared_داده,
                    my_منبع_Char,&هدف_Uni_Char,آرگومانها_2->converter->useFallback);
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
               my_Converter_داده->is_هدف_UChar_DBCS = is_هدف_UChar_DBCS = هدف_Uni_Char > 0x00FF;
                 إذا(old_Is_هدف_UChar_DBCS != is_هدف_UChar_DBCS || !my_Converter_داده->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    إذا(!is_هدف_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(آرگومانها_2, my_هدف_شاخص, هدف_درازی, escSeq,err,len,my_منبع_شاخص);
                        my_Converter_داده->isEscapeAppended = درست;
                    }
                    وإلا{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(آرگومانها_2, my_هدف_شاخص, هدف_درازی, escSeq,err,len,my_منبع_شاخص);
                        my_Converter_داده->isEscapeAppended = درست;
                        
                    }
                }
            
                إذا(is_هدف_UChar_DBCS){
                    إذا( my_هدف_شاخص <هدف_درازی){
                        my_هدف[my_هدف_شاخص++] =(حرف) (هدف_Uni_Char >> 8);
                        إذا(offsets){
                            *(offsets++) = my_منبع_شاخص-1;
                        }
                        إذا(my_هدف_شاخص < هدف_درازی){
                            my_هدف[my_هدف_شاخص++] =(حرف) هدف_Uni_Char;
                            إذا(offsets){
                                *(offsets++) = my_منبع_شاخص-1;
                            }
                        }وإلا{
                            آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = (حرف) هدف_Uni_Char;
                            *err = U_میانگیر_سرریز_ERROR;
                        } 
                    }وإلا{
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] =(حرف) (هدف_Uni_Char >> 8);
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = (حرف) هدف_Uni_Char;
                        *err = U_میانگیر_سرریز_ERROR;
                    }

                }وإلا{
                    إذا( my_هدف_شاخص <هدف_درازی){
                        my_هدف[my_هدف_شاخص++] = (حرف) (هدف_Uni_Char );
                        إذا(offsets){
                            *(offsets++) = my_منبع_شاخص-1;
                        }
                        
                    }وإلا{
                        آرگومانها_2->converter->char_Error_میانگیر[آرگومانها_2->converter->char_Error_میانگیر_درازی++] = (حرف) هدف_Uni_Char;
                        *err = U_میانگیر_سرریز_ERROR;
                    }
                }

            }
            وإلا{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                إذا(U16_IS_SURROGATE(my_منبع_Char)) {
                    إذا(U16_IS_SURROGATE_LEAD(my_منبع_Char)) {
                        آرگومانها_2->converter->fromUChar32=my_منبع_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        إذا(my_منبع_شاخص <  my_منبع_درازی) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) آرگومانها_2->منبع[my_منبع_شاخص];
                            إذا(U16_IS_TRAIL(trail)) {
                                ++my_منبع_شاخص;
                                my_منبع_Char=U16_GET_SUPPLEMENTARY(آرگومانها_2->converter->fromUChar32, trail);
                                آرگومانها_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_یافت;
                                /* exit this condition tree */
                            } وإلا {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_یافت;
                            }
                        } وإلا {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } وإلا {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_یافت;
                    }
                } وإلا {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_یافت;
                }

                آرگومانها_2->converter->fromUChar32=my_منبع_Char;
                اكسر;
            }
        }
        وإلا{
            *err = U_میانگیر_سرریز_ERROR;
            اكسر;
        }
        هدف_Uni_Char=missingCharMarker;
    }

    آرگومانها_2->هدف += my_هدف_شاخص;
    آرگومانها_2->منبع += my_منبع_شاخص;
    my_Converter_داده->is_هدف_UChar_DBCS = is_هدف_UChar_DBCS;
}

ساكن فراغ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_آرگومانها *آرگومانها_2, int32_t جابه_جایی_شاخص, UError_کد *err) {
    UConverter *cnv = آرگومانها_2->converter;
    UConverter_داده_HZ *conv_داده=(UConverter_داده_HZ *) cnv->extraInfo;
    حرف *p;
    حرف میانگیر[4];
    p = میانگیر;
    
    إذا( conv_داده->is_هدف_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_داده->is_هدف_UChar_DBCS=خطأ;
    }
    *p++= (حرف)cnv->subChars[0];

    ucnv_cb_From_UWrite_بایتها(آرگومانها_2,
                           میانگیر, (int32_t)(p - میانگیر),
                           جابه_جایی_شاخص, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
بنية cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_داده_HZ mydata;
};


ساكن UConverter *  U_CALLCONV
HZ_امن_Clone(ثابت UConverter *cnv, 
              فراغ *پشته_میانگیر, 
              int32_t *p_میانگیر_اندازه, 
              UError_کد *وضعیت)
{
    بنية cloneHZStruct * localClone;
    int32_t size, میانگیر_اندازه_Needed = حجم(بنية cloneHZStruct);

    إذا (U_FAILURE(*وضعیت)){
        أرجع اشاره_تهی;
    }

    إذا (*p_میانگیر_اندازه == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_میانگیر_اندازه = میانگیر_اندازه_Needed;
        أرجع اشاره_تهی;
    }

    localClone = (بنية cloneHZStruct *)پشته_میانگیر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(UConverter_داده_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = درست;

    /* deep-clone the sub-converter */
    size = (int32_t)حجم(UConverter);
    ((UConverter_داده_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_امن_Clone(((UConverter_داده_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, وضعیت);

    أرجع &localClone->cnv;
}

ساكن فراغ U_CALLCONV
HZ_Get_Unicode_مجموعه(ثابت UConverter *cnv,
                  ثابت USetAdder *sa,
                  UConverter_Unicode_مجموعه which,
                  UError_کد *p_Error_کد) {
    /* HZ converts all of ASCII */
    sa->جمع_کن_Range(sa->مجموعه, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_مجموعه_For_Unicode(
        ((UConverter_داده_HZ*)cnv->extraInfo)->gbConverter->shared_داده,
        sa, which, UCNV_مجموعه_FILTER_HZ,
        p_Error_کد);
}
U_CDECL_انتها
ساكن ثابت UConverterImpl _HZImpl={

    UCNV_HZ,
    
    اشاره_تهی,
    اشاره_تهی,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_مقصد_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    اشاره_تهی,
    
    اشاره_تهی,
    اشاره_تهی,
    _HZ_WriteSub,
    HZ_امن_Clone,
    HZ_Get_Unicode_مجموعه,
    اشاره_تهی,
    اشاره_تهی
};

ساكن ثابت UConverter_Static_داده HZStatic_داده={
    حجم(UConverter_Static_داده),
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

ثابت UConverter_Shared_داده _HZData=
        UCNV_IMMUTABLE_SHARED_داده_INITIALIZER(&HZStatic_داده, &_HZImpl);

#پایان_اگر /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
