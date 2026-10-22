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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "cmemory.h"
#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/uset.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "ucnv_imp.h"

#నిర్వచించు UCNV_TILDE 0x7E          /* ~ */
#నిర్వచించు UCNV_OPEN_BRACE 0x7B     /* { */
#నిర్వచించు UCNV_CLOSE_BRACE 0x7D   /* } */
#నిర్వచించు SB_ESCAPE    "\x7E\x7D"
#నిర్వచించు DB_ESCAPE    "\x7E\x7B"
#నిర్వచించు TILDE_ESCAPE "\x7E\x7E"
#నిర్వచించు ESC_LEN       2


#నిర్వచించు CONCAT_ESCAPE_MACRO(వాదనలు_2, లక్ష్యం_సూచీ,లక్ష్యం_నిడివి,str_గమ్యం_Append, err, len,ఆధారం_సూచీ) UPRV_ఖండం_MACRO_BEGIN {      \
    అయితే(len-->0){                                                                                                         \
        ఒకవేళ(లక్ష్యం_సూచీ < లక్ష్యం_నిడివి){                                                                                     \
            వాదనలు_2->లక్ష్యం[లక్ష్యం_సూచీ] = (గుర్తులేని అక్షరం) *str_గమ్యం_Append;                                                       \
            ఒకవేళ(వాదనలు_2->offsets!=శూన్యసూచిక){                                                                                        \
                *(offsets++) = ఆధారం_సూచీ-1;                                                                               \
            }                                                                                                               \
            లక్ష్యం_సూచీ++;                                                                                                  \
        }                                                                                                                   \
        లేకపోతే{                                                                                                               \
            వాదనలు_2->converter->char_Error_బఫరు[(పూర్ణాంకం)వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = (గుర్తులేని అక్షరం) *str_గమ్యం_Append; \
            *err =U_బఫరు_అధికప్రవాహం_ERROR;                                                                                  \
        }                                                                                                                   \
        str_గమ్యం_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_ఖండం_MACRO_చివర


రకనిర్వచనం నిర్మాణం{
    UConverter* gbConverter;
    int32_t లక్ష్యం_సూచీ;
    int32_t ఆధారం_సూచీ;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_లక్ష్యం_UChar_DBCS;
    UBool is_ఖాళీలు_Segment;
}UConverter_దత్తాంశం_HZ;


U_CDECL_BEGIN
స్థిర శూన్య  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_ఎక్కించు_వాదనలు *p_వాదనలు, UError_సంకేతం *error_సంకేతం){
    UConverter *gbConverter;
    ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_సంకేతం);  /* errorCode carries result */
        తిరిగి;
    }
    gbConverter = ucnv_open("GBK", error_సంకేతం);
    ఒకవేళ(U_FAILURE(*error_సంకేతం)) {
        తిరిగి;
    }
    cnv->గమ్యం_Unicode_స్థితి = 0;
    cnv->from_Unicode_స్థితి= 0;
    cnv->బహుళకం=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, పరిమాణం(UConverter_దత్తాంశం_HZ));
    ఒకవేళ(cnv->extraInfo != శూన్యసూచిక){
        ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    లేకపోతే {
        ucnv_close(gbConverter);
        *error_సంకేతం = U_జ్ఞాపకం_కేటాయింపు_ERROR;
        తిరిగి;
    }
}

స్థిర శూన్య  U_CALLCONV
_HZClose(UConverter *cnv){
    ఒకవేళ(cnv->extraInfo != శూన్యసూచిక) {
        ucnv_close (((UConverter_దత్తాంశం_HZ *) (cnv->extraInfo))->gbConverter);
        ఒకవేళ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = శూన్యసూచిక;
    }
}

స్థిర శూన్య  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        cnv->గమ్యం_Unicode_స్థితి = 0;
        cnv->బహుళకం=0;
        ఒకవేళ(cnv->extraInfo != శూన్యసూచిక){
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->isStateDBCS = అబద్ధం;
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->is_ఖాళీలు_Segment = అబద్ధం;
        }
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        cnv->from_Unicode_స్థితి= 0;
        cnv->fromUChar32=0x0000; 
        ఒకవేళ(cnv->extraInfo != శూన్యసూచిక){
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->isEscapeAppended = అబద్ధం;
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->లక్ష్యం_సూచీ = 0;
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->ఆధారం_సూచీ = 0;
            ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->is_లక్ష్యం_UChar_DBCS = అబద్ధం;
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


స్థిర శూన్య  U_CALLCONV
UConverter_గమ్యం_Unicode_HZ_OFFSETS_LOGIC(UConverter_గమ్యం_Unicode_వాదనలు *వాదనలు_2,
                                                            UError_సంకేతం* err){
    అక్షరం తాత్కాలిక_Buf[2];
    స్థిరరాశి అక్షరం *my_ఆధారం = ( అక్షరం *) వాదనలు_2->ఆధారం;
    char16_t *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    స్థిరరాశి అక్షరం *my_ఆధారం_హద్దు = వాదనలు_2->ఆధారం_హద్దు;
    UChar32 లక్ష్యం_Uni_Char = 0x0000;
    int32_t my_ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_HZ* my_దత్తాంశం=(UConverter_దత్తాంశం_HZ*)(వాదనలు_2->converter->extraInfo);
    తాత్కాలిక_Buf[0]=0; 
    తాత్కాలిక_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    అయితే(my_ఆధారం< my_ఆధారం_హద్దు){
        
        ఒకవేళ(my_లక్ష్యం < వాదనలు_2->లక్ష్యం_హద్దు){
            
            my_ఆధారం_Char= (గుర్తులేని అక్షరం) *my_ఆధారం++;

            ఒకవేళ(వాదనలు_2->converter->బహుళకం == UCNV_TILDE) {
                /* second byte after ~ */
                వాదనలు_2->converter->బహుళకం=0;
                ఎంపిక(my_ఆధారం_Char) {
                స్థితి 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    కొనసాగు;
                స్థితి UCNV_TILDE:
                    ఒకవేళ(వాదనలు_2->offsets) {
                        వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం]=(int32_t)(my_ఆధారం - వాదనలు_2->ఆధారం - 2);
                    }
                    *(my_లక్ష్యం++)=(char16_t)my_ఆధారం_Char;
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం;
                    కొనసాగు;
                స్థితి UCNV_OPEN_BRACE:
                స్థితి UCNV_CLOSE_BRACE:
                    my_దత్తాంశం->isStateDBCS = (my_ఆధారం_Char == UCNV_OPEN_BRACE);
                    ఒకవేళ (my_దత్తాంశం->is_ఖాళీలు_Segment) {
                        my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        వాదనలు_2->converter->గమ్యం_UCallback_Reason = UCNV_IRREGULAR;
                        వాదనలు_2->converter->గమ్యం_UBytes[0] = UCNV_TILDE;
                        వాదనలు_2->converter->గమ్యం_UBytes[1] = static_cast<uint8_t>(my_ఆధారం_Char);
                        వాదనలు_2->converter->గమ్యం_ULength = 2;
                        వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                        వాదనలు_2->ఆధారం = my_ఆధారం;
                        తిరిగి;
                    }
                    my_దత్తాంశం->is_ఖాళీలు_Segment = నిజం;
                    కొనసాగు;
                అప్రమేయ:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = UCNV_TILDE;
                    ఒకవేళ( my_దత్తాంశం->isStateDBCS ?
                            (0x21 <= my_ఆధారం_Char && my_ఆధారం_Char <= 0x7e) :
                            my_ఆధారం_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        వాదనలు_2->converter->గమ్యం_ULength = 1;
                        --my_ఆధారం;
                    } లేకపోతే {
                        /* Include the current byte in the illegal sequence. */
                        వాదనలు_2->converter->గమ్యం_UBytes[1] = static_cast<uint8_t>(my_ఆధారం_Char);
                        వాదనలు_2->converter->గమ్యం_ULength = 2;
                    }
                    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
                    వాదనలు_2->ఆధారం = my_ఆధారం;
                    తిరిగి;
                }
            } లేకపోతే ఒకవేళ(my_దత్తాంశం->isStateDBCS) {
                ఒకవేళ(వాదనలు_2->converter->గమ్యం_Unicode_స్థితి == 0x00){
                    /* lead byte */
                    ఒకవేళ(my_ఆధారం_Char == UCNV_TILDE) {
                        వాదనలు_2->converter->బహుళకం = UCNV_TILDE;
                    } లేకపోతే {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        వాదనలు_2->converter->గమ్యం_Unicode_స్థితి = (uint32_t) (my_ఆధారం_Char | 0x100);
                        my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    కొనసాగు;
                }
                లేకపోతే{
                    /* trail byte */
                    పూర్ణాంకం leadIsOk, trailIsOk;
                    uint32_t leadByte = వాదనలు_2->converter->గమ్యం_Unicode_స్థితి & 0xff;
                    లక్ష్యం_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_ఆధారం_Char - 0x21) <= (0x7e - 0x21);
                    ఒకవేళ (leadIsOk && trailIsOk) {
                        తాత్కాలిక_Buf[0] = (అక్షరం) (leadByte+0x80) ;
                        తాత్కాలిక_Buf[1] = (అక్షరం) (my_ఆధారం_Char+0x80);
                        లక్ష్యం_Uni_Char = ucnv_MBCSSimple_Get_తదుపరి_UChar(my_దత్తాంశం->gbConverter->shared_దత్తాంశం,
                            తాత్కాలిక_Buf, 2, వాదనలు_2->converter->useFallback);
                        my_ఆధారం_Char= (leadByte << 8) | my_ఆధారం_Char;
                    } లేకపోతే ఒకవేళ (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_ఆధారం;
                        my_ఆధారం_Char = (int32_t)leadByte;
                    } లేకపోతే {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ఆధారం_Char= 0x10000 | (leadByte << 8) | my_ఆధారం_Char;
                    }
                    వాదనలు_2->converter->గమ్యం_Unicode_స్థితి =0x00;
                }
            }
            లేకపోతే{
                ఒకవేళ(my_ఆధారం_Char == UCNV_TILDE) {
                    వాదనలు_2->converter->బహుళకం = UCNV_TILDE;
                    కొనసాగు;
                } లేకపోతే ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                    లక్ష్యం_Uni_Char = (char16_t)my_ఆధారం_Char;  /* ASCII */
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం; /* the segment has something valid */
                } లేకపోతే {
                    లక్ష్యం_Uni_Char = 0xffff;
                    my_దత్తాంశం->is_ఖాళీలు_Segment = అబద్ధం; /* different error here, reset this to avoid spurious future error */
                }
            }
            ఒకవేళ(లక్ష్యం_Uni_Char < 0xfffe){
                ఒకవేళ(వాదనలు_2->offsets) {
                    వాదనలు_2->offsets[my_లక్ష్యం - వాదనలు_2->లక్ష్యం]=(int32_t)(my_ఆధారం - వాదనలు_2->ఆధారం - 1-(my_దత్తాంశం->isStateDBCS));
                }

                *(my_లక్ష్యం++)=(char16_t)లక్ష్యం_Uni_Char;
            }
            లేకపోతే /* targetUniChar>=0xfffe */ {
                ఒకవేళ(లక్ష్యం_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_దొరికింది;
                }
                లేకపోతే{
                    *err = U_ILLEGAL_CHAR_దొరికింది;
                }
                ఒకవేళ(my_ఆధారం_Char > 0xff){
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = (uint8_t)(my_ఆధారం_Char >> 8);
                    వాదనలు_2->converter->గమ్యం_UBytes[1] = (uint8_t)my_ఆధారం_Char;
                    వాదనలు_2->converter->గమ్యం_ULength=2;
                }
                లేకపోతే{
                    వాదనలు_2->converter->గమ్యం_UBytes[0] = (uint8_t)my_ఆధారం_Char;
                    వాదనలు_2->converter->గమ్యం_ULength=1;
                }
                ఆపు;
            }
        }
        లేకపోతే{
            *err =U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
    }

    వాదనలు_2->లక్ష్యం = my_లక్ష్యం;
    వాదనలు_2->ఆధారం = my_ఆధారం;
}


స్థిర శూన్య  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_వాదనలు * వాదనలు_2,
                                                      UError_సంకేతం * err){
    స్థిరరాశి char16_t *my_ఆధారం = వాదనలు_2->ఆధారం;
    అక్షరం *my_లక్ష్యం = వాదనలు_2->లక్ష్యం;
    int32_t* offsets = వాదనలు_2->offsets;
    int32_t my_ఆధారం_సూచీ = 0;
    int32_t my_లక్ష్యం_సూచీ = 0;
    int32_t లక్ష్యం_నిడివి = (int32_t)(వాదనలు_2->లక్ష్యం_హద్దు - my_లక్ష్యం);
    int32_t my_ఆధారం_నిడివి = (int32_t)(వాదనలు_2->ఆధారం_హద్దు - వాదనలు_2->ఆధారం);
    uint32_t లక్ష్యం_Uni_Char = 0x0000;
    UChar32 my_ఆధారం_Char = 0x0000;
    UConverter_దత్తాంశం_HZ *my_Converter_దత్తాంశం=(UConverter_దత్తాంశం_HZ*)వాదనలు_2->converter->extraInfo;
    UBool is_లక్ష్యం_UChar_DBCS = my_Converter_దత్తాంశం->is_లక్ష్యం_UChar_DBCS;
    UBool old_Is_లక్ష్యం_UChar_DBCS;
    పూర్ణాంకం len =0;
    స్థిరరాశి అక్షరం* escSeq=శూన్యసూచిక;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    ఒకవేళ(వాదనలు_2->converter->fromUChar32!=0 && my_లక్ష్యం_సూచీ < లక్ష్యం_నిడివి) {
        వెళ్ళు getTrail;
    }
    /*writing the char to the output stream */
    అయితే (my_ఆధారం_సూచీ < my_ఆధారం_నిడివి){
        లక్ష్యం_Uni_Char = missingCharMarker;
        ఒకవేళ (my_లక్ష్యం_సూచీ < లక్ష్యం_నిడివి){
            
            my_ఆధారం_Char = my_ఆధారం[my_ఆధారం_సూచీ++];
            

            old_Is_లక్ష్యం_UChar_DBCS = is_లక్ష్యం_UChar_DBCS;
            ఒకవేళ(my_ఆధారం_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(వాదనలు_2, my_లక్ష్యం_సూచీ, లక్ష్యం_నిడివి, escSeq,err,len,my_ఆధారం_సూచీ);
                కొనసాగు;
            } లేకపోతే ఒకవేళ(my_ఆధారం_Char <= 0x7f) {
                లక్ష్యం_Uni_Char = my_ఆధారం_Char;
            } లేకపోతే {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_దత్తాంశం->gbConverter->shared_దత్తాంశం,
                    my_ఆధారం_Char,&లక్ష్యం_Uni_Char,వాదనలు_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                ఒకవేళ( length == 2 &&
                    (uint16_t)(లక్ష్యం_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(లక్ష్యం_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    లక్ష్యం_Uni_Char -= 0x8080;
                } లేకపోతే {
                    లక్ష్యం_Uni_Char = missingCharMarker;
                }
            }
            ఒకవేళ (లక్ష్యం_Uni_Char != missingCharMarker){
               my_Converter_దత్తాంశం->is_లక్ష్యం_UChar_DBCS = is_లక్ష్యం_UChar_DBCS = లక్ష్యం_Uni_Char > 0x00FF;
                 ఒకవేళ(old_Is_లక్ష్యం_UChar_DBCS != is_లక్ష్యం_UChar_DBCS || !my_Converter_దత్తాంశం->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    ఒకవేళ(!is_లక్ష్యం_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(వాదనలు_2, my_లక్ష్యం_సూచీ, లక్ష్యం_నిడివి, escSeq,err,len,my_ఆధారం_సూచీ);
                        my_Converter_దత్తాంశం->isEscapeAppended = నిజం;
                    }
                    లేకపోతే{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(వాదనలు_2, my_లక్ష్యం_సూచీ, లక్ష్యం_నిడివి, escSeq,err,len,my_ఆధారం_సూచీ);
                        my_Converter_దత్తాంశం->isEscapeAppended = నిజం;
                        
                    }
                }
            
                ఒకవేళ(is_లక్ష్యం_UChar_DBCS){
                    ఒకవేళ( my_లక్ష్యం_సూచీ <లక్ష్యం_నిడివి){
                        my_లక్ష్యం[my_లక్ష్యం_సూచీ++] =(అక్షరం) (లక్ష్యం_Uni_Char >> 8);
                        ఒకవేళ(offsets){
                            *(offsets++) = my_ఆధారం_సూచీ-1;
                        }
                        ఒకవేళ(my_లక్ష్యం_సూచీ < లక్ష్యం_నిడివి){
                            my_లక్ష్యం[my_లక్ష్యం_సూచీ++] =(అక్షరం) లక్ష్యం_Uni_Char;
                            ఒకవేళ(offsets){
                                *(offsets++) = my_ఆధారం_సూచీ-1;
                            }
                        }లేకపోతే{
                            వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = (అక్షరం) లక్ష్యం_Uni_Char;
                            *err = U_బఫరు_అధికప్రవాహం_ERROR;
                        } 
                    }లేకపోతే{
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] =(అక్షరం) (లక్ష్యం_Uni_Char >> 8);
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = (అక్షరం) లక్ష్యం_Uni_Char;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }

                }లేకపోతే{
                    ఒకవేళ( my_లక్ష్యం_సూచీ <లక్ష్యం_నిడివి){
                        my_లక్ష్యం[my_లక్ష్యం_సూచీ++] = (అక్షరం) (లక్ష్యం_Uni_Char );
                        ఒకవేళ(offsets){
                            *(offsets++) = my_ఆధారం_సూచీ-1;
                        }
                        
                    }లేకపోతే{
                        వాదనలు_2->converter->char_Error_బఫరు[వాదనలు_2->converter->char_Error_బఫరు_నిడివి++] = (అక్షరం) లక్ష్యం_Uni_Char;
                        *err = U_బఫరు_అధికప్రవాహం_ERROR;
                    }
                }

            }
            లేకపోతే{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                ఒకవేళ(U16_IS_SURROGATE(my_ఆధారం_Char)) {
                    ఒకవేళ(U16_IS_SURROGATE_LEAD(my_ఆధారం_Char)) {
                        వాదనలు_2->converter->fromUChar32=my_ఆధారం_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ఒకవేళ(my_ఆధారం_సూచీ <  my_ఆధారం_నిడివి) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) వాదనలు_2->ఆధారం[my_ఆధారం_సూచీ];
                            ఒకవేళ(U16_IS_TRAIL(trail)) {
                                ++my_ఆధారం_సూచీ;
                                my_ఆధారం_Char=U16_GET_SUPPLEMENTARY(వాదనలు_2->converter->fromUChar32, trail);
                                వాదనలు_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_దొరికింది;
                                /* exit this condition tree */
                            } లేకపోతే {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_దొరికింది;
                            }
                        } లేకపోతే {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } లేకపోతే {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_దొరికింది;
                    }
                } లేకపోతే {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_దొరికింది;
                }

                వాదనలు_2->converter->fromUChar32=my_ఆధారం_Char;
                ఆపు;
            }
        }
        లేకపోతే{
            *err = U_బఫరు_అధికప్రవాహం_ERROR;
            ఆపు;
        }
        లక్ష్యం_Uni_Char=missingCharMarker;
    }

    వాదనలు_2->లక్ష్యం += my_లక్ష్యం_సూచీ;
    వాదనలు_2->ఆధారం += my_ఆధారం_సూచీ;
    my_Converter_దత్తాంశం->is_లక్ష్యం_UChar_DBCS = is_లక్ష్యం_UChar_DBCS;
}

స్థిర శూన్య U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_వాదనలు *వాదనలు_2, int32_t స్థానభ్రంశం_సూచీ, UError_సంకేతం *err) {
    UConverter *cnv = వాదనలు_2->converter;
    UConverter_దత్తాంశం_HZ *conv_దత్తాంశం=(UConverter_దత్తాంశం_HZ *) cnv->extraInfo;
    అక్షరం *p;
    అక్షరం బఫరు[4];
    p = బఫరు;
    
    ఒకవేళ( conv_దత్తాంశం->is_లక్ష్యం_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_దత్తాంశం->is_లక్ష్యం_UChar_DBCS=అబద్ధం;
    }
    *p++= (అక్షరం)cnv->subChars[0];

    ucnv_cb_From_UWrite_బైట్లు(వాదనలు_2,
                           బఫరు, (int32_t)(p - బఫరు),
                           స్థానభ్రంశం_సూచీ, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
నిర్మాణం cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_దత్తాంశం_HZ mydata;
};


స్థిర UConverter *  U_CALLCONV
HZ_సురక్షిత_Clone(స్థిరరాశి UConverter *cnv, 
              శూన్య *దొంతర_బఫరు, 
              int32_t *p_బఫరు_కొలత, 
              UError_సంకేతం *స్థితి_2)
{
    నిర్మాణం cloneHZStruct * localClone;
    int32_t size, బఫరు_కొలత_Needed = పరిమాణం(నిర్మాణం cloneHZStruct);

    ఒకవేళ (U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (*p_బఫరు_కొలత == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_బఫరు_కొలత = బఫరు_కొలత_Needed;
        తిరిగి శూన్యసూచిక;
    }

    localClone = (నిర్మాణం cloneHZStruct *)దొంతర_బఫరు;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, పరిమాణం(UConverter_దత్తాంశం_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = నిజం;

    /* deep-clone the sub-converter */
    size = (int32_t)పరిమాణం(UConverter);
    ((UConverter_దత్తాంశం_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_సురక్షిత_Clone(((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, స్థితి_2);

    తిరిగి &localClone->cnv;
}

స్థిర శూన్య U_CALLCONV
HZ_Get_Unicode_సమితి(స్థిరరాశి UConverter *cnv,
                  స్థిరరాశి USetAdder *sa,
                  UConverter_Unicode_సమితి which,
                  UError_సంకేతం *p_Error_సంకేతం) {
    /* HZ converts all of ASCII */
    sa->కూడు_Range(sa->సమితి, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_సమితి_For_Unicode(
        ((UConverter_దత్తాంశం_HZ*)cnv->extraInfo)->gbConverter->shared_దత్తాంశం,
        sa, which, UCNV_సమితి_FILTER_HZ,
        p_Error_సంకేతం);
}
U_CDECL_చివర
స్థిర స్థిరరాశి UConverterImpl _HZImpl={

    UCNV_HZ,
    
    శూన్యసూచిక,
    శూన్యసూచిక,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_గమ్యం_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_గమ్యం_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    శూన్యసూచిక,
    
    శూన్యసూచిక,
    శూన్యసూచిక,
    _HZ_WriteSub,
    HZ_సురక్షిత_Clone,
    HZ_Get_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం HZStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        అబద్ధం, 
        అబద్ధం,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

స్థిరరాశి UConverter_Shared_దత్తాంశం _HZData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&HZStatic_దత్తాంశం, &_HZImpl);

#అయితే_ముగింపు /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
