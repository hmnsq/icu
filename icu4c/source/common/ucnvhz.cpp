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

#অন্তর্ভাব "unicode/utypes.h"

#যদি !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ucnv.h"
#অন্তর্ভাব "unicode/ucnv_cb.h"
#অন্তর্ভাব "unicode/uset.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "ucnv_bld.h"
#অন্তর্ভাব "ucnv_cnv.h"
#অন্তর্ভাব "ucnv_imp.h"

#define UCNV_TILDE 0x7E          /* ~ */
#define UCNV_OPEN_BRACE 0x7B     /* { */
#define UCNV_CLOSE_BRACE 0x7D   /* } */
#define SB_ESCAPE    "\x7E\x7D"
#define DB_ESCAPE    "\x7E\x7B"
#define TILDE_ESCAPE "\x7E\x7E"
#define ESC_LEN       2


#define CONCAT_ESCAPE_MACRO(args, target_সূচক,target_দৈর্ঘ্য,strToAppend, err, len,source_সূচক) UPRV_BLOCK_MACRO_BEGIN {      \
    যখন(len-->0){                                                                                                         \
        যদি(target_সূচক < target_দৈর্ঘ্য){                                                                                     \
            args->target[target_সূচক] = (unsigned অক্ষর) *strToAppend;                                                       \
            যদি(args->offsets!=nullptr){                                                                                        \
                *(offsets++) = source_সূচক-1;                                                                               \
            }                                                                                                               \
            target_সূচক++;                                                                                                  \
        }                                                                                                                   \
        অন্যথা{                                                                                                               \
            args->converter->char_Error_বাফার[(পূর্ণসংখ্যা)args->converter->char_Error_বাফার_দৈর্ঘ্য++] = (unsigned অক্ষর) *strToAppend; \
            *err =U_বাফার_OVERFLOW_ERROR;                                                                                  \
        }                                                                                                                   \
        strToAppend++;                                                                                                      \
    }                                                                                                                       \
} UPRV_BLOCK_MACRO_END


প্রকারলেখ কাঠামো{
    UConverter* gbConverter;
    int32_t target_সূচক;
    int32_t source_সূচক;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool isEmptySegment;
}UConverter_তথ্য_HZ;


U_CDECL_BEGIN
স্থির শূন্য  U_CALLCONV
_HZOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    UConverter *gbConverter;
    যদি(pArgs->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", errorCode);  /* errorCode carries result */
        ফেরত;
    }
    gbConverter = ucnv_open("GBK", errorCode);
    যদি(U_FAILURE(*errorCode)) {
        ফেরত;
    }
    cnv->toUnicodeStatus = 0;
    cnv->fromUnicodeStatus= 0;
    cnv->mode=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, আকার(UConverter_তথ্য_HZ));
    যদি(cnv->extraInfo != nullptr){
        ((UConverter_তথ্য_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    অন্যথা {
        ucnv_close(gbConverter);
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
        ফেরত;
    }
}

স্থির শূন্য  U_CALLCONV
_HZClose(UConverter *cnv){
    যদি(cnv->extraInfo != nullptr) {
        ucnv_close (((UConverter_তথ্য_HZ *) (cnv->extraInfo))->gbConverter);
        যদি(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = nullptr;
    }
}

স্থির শূন্য  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    যদি(choice<=UCNV_RESET_TO_UNICODE) {
        cnv->toUnicodeStatus = 0;
        cnv->mode=0;
        যদি(cnv->extraInfo != nullptr){
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->isStateDBCS = মিথ্যা;
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->isEmptySegment = মিথ্যা;
        }
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUnicodeStatus= 0;
        cnv->fromUChar32=0x0000; 
        যদি(cnv->extraInfo != nullptr){
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->isEscapeAppended = মিথ্যা;
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->target_সূচক = 0;
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->source_সূচক = 0;
            ((UConverter_তথ্য_HZ*)cnv->extraInfo)->isTargetUCharDBCS = মিথ্যা;
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


স্থির শূন্য  U_CALLCONV
UConverter_toUnicode_HZ_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    অক্ষর অস্থায়ী_Buf[2];
    ধ্রুবক অক্ষর *mySource = ( অক্ষর *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুবক অক্ষর *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    int32_t mySourceChar = 0x0000;
    UConverter_তথ্য_HZ* my_তথ্য=(UConverter_তথ্য_HZ*)(args->converter->extraInfo);
    অস্থায়ী_Buf[0]=0; 
    অস্থায়ী_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    যখন(mySource< mySourceLimit){
        
        যদি(myTarget < args->targetLimit){
            
            mySourceChar= (unsigned অক্ষর) *mySource++;

            যদি(args->converter->mode == UCNV_TILDE) {
                /* second byte after ~ */
                args->converter->mode=0;
                নির্বাচন(mySourceChar) {
                ক্ষেত্র 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    চালু;
                ক্ষেত্র UCNV_TILDE:
                    যদি(args->offsets) {
                        args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    my_তথ্য->isEmptySegment = মিথ্যা;
                    চালু;
                ক্ষেত্র UCNV_OPEN_BRACE:
                ক্ষেত্র UCNV_CLOSE_BRACE:
                    my_তথ্য->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    যদি (my_তথ্য->isEmptySegment) {
                        my_তথ্য->isEmptySegment = মিথ্যা; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toUBytes[0] = UCNV_TILDE;
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                        args->target = myTarget;
                        args->source = mySource;
                        ফেরত;
                    }
                    my_তথ্য->isEmptySegment = সত্য;
                    চালু;
                পূর্বনির্ধারিত:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_তথ্য->isEmptySegment = মিথ্যা; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUBytes[0] = UCNV_TILDE;
                    যদি( my_তথ্য->isStateDBCS ?
                            (0x21 <= mySourceChar && mySourceChar <= 0x7e) :
                            mySourceChar <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        args->converter->toULength = 1;
                        --mySource;
                    } অন্যথা {
                        /* Include the current byte in the illegal sequence. */
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                    }
                    args->target = myTarget;
                    args->source = mySource;
                    ফেরত;
                }
            } অন্যথা যদি(my_তথ্য->isStateDBCS) {
                যদি(args->converter->toUnicodeStatus == 0x00){
                    /* lead byte */
                    যদি(mySourceChar == UCNV_TILDE) {
                        args->converter->mode = UCNV_TILDE;
                    } অন্যথা {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        args->converter->toUnicodeStatus = (uint32_t) (mySourceChar | 0x100);
                        my_তথ্য->isEmptySegment = মিথ্যা; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    চালু;
                }
                অন্যথা{
                    /* trail byte */
                    পূর্ণসংখ্যা leadIsOk, trailIsOk;
                    uint32_t leadByte = args->converter->toUnicodeStatus & 0xff;
                    targetUniChar = 0xffff;
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
                    trailIsOk = (uint8_t)(mySourceChar - 0x21) <= (0x7e - 0x21);
                    যদি (leadIsOk && trailIsOk) {
                        অস্থায়ী_Buf[0] = (অক্ষর) (leadByte+0x80) ;
                        অস্থায়ী_Buf[1] = (অক্ষর) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_পরবর্তী_UChar(my_তথ্য->gbConverter->shared_তথ্য,
                            অস্থায়ী_Buf, 2, args->converter->useFallback);
                        mySourceChar= (leadByte << 8) | mySourceChar;
                    } অন্যথা যদি (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --mySource;
                        mySourceChar = (int32_t)leadByte;
                    } অন্যথা {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar= 0x10000 | (leadByte << 8) | mySourceChar;
                    }
                    args->converter->toUnicodeStatus =0x00;
                }
            }
            অন্যথা{
                যদি(mySourceChar == UCNV_TILDE) {
                    args->converter->mode = UCNV_TILDE;
                    চালু;
                } অন্যথা যদি(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    my_তথ্য->isEmptySegment = মিথ্যা; /* the segment has something valid */
                } অন্যথা {
                    targetUniChar = 0xffff;
                    my_তথ্য->isEmptySegment = মিথ্যা; /* different error here, reset this to avoid spurious future error */
                }
            }
            যদি(targetUniChar < 0xfffe){
                যদি(args->offsets) {
                    args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 1-(my_তথ্য->isStateDBCS));
                }

                *(myTarget++)=(char16_t)targetUniChar;
            }
            অন্যথা /* targetUniChar>=0xfffe */ {
                যদি(targetUniChar == 0xfffe){
                    *err = U_INVALID_CHAR_FOUND;
                }
                অন্যথা{
                    *err = U_ILLEGAL_CHAR_FOUND;
                }
                যদি(mySourceChar > 0xff){
                    args->converter->toUBytes[0] = (uint8_t)(mySourceChar >> 8);
                    args->converter->toUBytes[1] = (uint8_t)mySourceChar;
                    args->converter->toULength=2;
                }
                অন্যথা{
                    args->converter->toUBytes[0] = (uint8_t)mySourceChar;
                    args->converter->toULength=1;
                }
                বিরতি;
            }
        }
        অন্যথা{
            *err =U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
    }

    args->target = myTarget;
    args->source = mySource;
}


স্থির শূন্য  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                      UErrorCode * err){
    ধ্রুবক char16_t *mySource = args->source;
    অক্ষর *myTarget = args->target;
    int32_t* offsets = args->offsets;
    int32_t my_Source_সূচক = 0;
    int32_t my_Target_সূচক = 0;
    int32_t target_দৈর্ঘ্য = (int32_t)(args->targetLimit - myTarget);
    int32_t my_Source_দৈর্ঘ্য = (int32_t)(args->sourceLimit - args->source);
    uint32_t targetUniChar = 0x0000;
    UChar32 mySourceChar = 0x0000;
    UConverter_তথ্য_HZ *my_Converter_তথ্য=(UConverter_তথ্য_HZ*)args->converter->extraInfo;
    UBool isTargetUCharDBCS = my_Converter_তথ্য->isTargetUCharDBCS;
    UBool oldIsTargetUCharDBCS;
    পূর্ণসংখ্যা len =0;
    ধ্রুবক অক্ষর* escSeq=nullptr;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    যদি(args->converter->fromUChar32!=0 && my_Target_সূচক < target_দৈর্ঘ্য) {
        যাও getTrail;
    }
    /*writing the char to the output stream */
    যখন (my_Source_সূচক < my_Source_দৈর্ঘ্য){
        targetUniChar = missingCharMarker;
        যদি (my_Target_সূচক < target_দৈর্ঘ্য){
            
            mySourceChar = mySource[my_Source_সূচক++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            যদি(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(args, my_Target_সূচক, target_দৈর্ঘ্য, escSeq,err,len,my_Source_সূচক);
                চালু;
            } অন্যথা যদি(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } অন্যথা {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_তথ্য->gbConverter->shared_তথ্য,
                    mySourceChar,&targetUniChar,args->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                যদি( length == 2 &&
                    (uint16_t)(targetUniChar - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(targetUniChar - 0xa1) <= (0xfe - 0xa1)
                ) {
                    targetUniChar -= 0x8080;
                } অন্যথা {
                    targetUniChar = missingCharMarker;
                }
            }
            যদি (targetUniChar != missingCharMarker){
               my_Converter_তথ্য->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 যদি(oldIsTargetUCharDBCS != isTargetUCharDBCS || !my_Converter_তথ্য->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    যদি(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, my_Target_সূচক, target_দৈর্ঘ্য, escSeq,err,len,my_Source_সূচক);
                        my_Converter_তথ্য->isEscapeAppended = সত্য;
                    }
                    অন্যথা{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, my_Target_সূচক, target_দৈর্ঘ্য, escSeq,err,len,my_Source_সূচক);
                        my_Converter_তথ্য->isEscapeAppended = সত্য;
                        
                    }
                }
            
                যদি(isTargetUCharDBCS){
                    যদি( my_Target_সূচক <target_দৈর্ঘ্য){
                        myTarget[my_Target_সূচক++] =(অক্ষর) (targetUniChar >> 8);
                        যদি(offsets){
                            *(offsets++) = my_Source_সূচক-1;
                        }
                        যদি(my_Target_সূচক < target_দৈর্ঘ্য){
                            myTarget[my_Target_সূচক++] =(অক্ষর) targetUniChar;
                            যদি(offsets){
                                *(offsets++) = my_Source_সূচক-1;
                            }
                        }অন্যথা{
                            args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = (অক্ষর) targetUniChar;
                            *err = U_বাফার_OVERFLOW_ERROR;
                        } 
                    }অন্যথা{
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] =(অক্ষর) (targetUniChar >> 8);
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = (অক্ষর) targetUniChar;
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }

                }অন্যথা{
                    যদি( my_Target_সূচক <target_দৈর্ঘ্য){
                        myTarget[my_Target_সূচক++] = (অক্ষর) (targetUniChar );
                        যদি(offsets){
                            *(offsets++) = my_Source_সূচক-1;
                        }
                        
                    }অন্যথা{
                        args->converter->char_Error_বাফার[args->converter->char_Error_বাফার_দৈর্ঘ্য++] = (অক্ষর) targetUniChar;
                        *err = U_বাফার_OVERFLOW_ERROR;
                    }
                }

            }
            অন্যথা{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                যদি(U16_IS_SURROGATE(mySourceChar)) {
                    যদি(U16_IS_SURROGATE_LEAD(mySourceChar)) {
                        args->converter->fromUChar32=mySourceChar;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        যদি(my_Source_সূচক <  my_Source_দৈর্ঘ্য) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) args->source[my_Source_সূচক];
                            যদি(U16_IS_TRAIL(trail)) {
                                ++my_Source_সূচক;
                                mySourceChar=U16_GET_SUPPLEMENTARY(args->converter->fromUChar32, trail);
                                args->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_FOUND;
                                /* exit this condition tree */
                            } অন্যথা {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } অন্যথা {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } অন্যথা {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } অন্যথা {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=mySourceChar;
                বিরতি;
            }
        }
        অন্যথা{
            *err = U_বাফার_OVERFLOW_ERROR;
            বিরতি;
        }
        targetUniChar=missingCharMarker;
    }

    args->target += my_Target_সূচক;
    args->source += my_Source_সূচক;
    my_Converter_তথ্য->isTargetUCharDBCS = isTargetUCharDBCS;
}

স্থির শূন্য U_CALLCONV
_HZ_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_সূচক, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverter_তথ্য_HZ *conv_তথ্য=(UConverter_তথ্য_HZ *) cnv->extraInfo;
    অক্ষর *p;
    অক্ষর বাফার[4];
    p = বাফার;
    
    যদি( conv_তথ্য->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_তথ্য->isTargetUCharDBCS=মিথ্যা;
    }
    *p++= (অক্ষর)cnv->subChars[0];

    ucnv_cbFromUWriteBytes(args,
                           বাফার, (int32_t)(p - বাফার),
                           offset_সূচক, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
কাঠামো cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_তথ্য_HZ mydata;
};


স্থির UConverter *  U_CALLCONV
_HZ_SafeClone(ধ্রুবক UConverter *cnv, 
              শূন্য *স্তূপ_বাফার, 
              int32_t *p_বাফার_আকার, 
              UErrorCode *status)
{
    কাঠামো cloneHZStruct * localClone;
    int32_t size, বাফার_আকার_Needed = আকার(কাঠামো cloneHZStruct);

    যদি (U_FAILURE(*status)){
        ফেরত nullptr;
    }

    যদি (*p_বাফার_আকার == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_বাফার_আকার = বাফার_আকার_Needed;
        ফেরত nullptr;
    }

    localClone = (কাঠামো cloneHZStruct *)স্তূপ_বাফার;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, আকার(UConverter_তথ্য_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = সত্য;

    /* deep-clone the sub-converter */
    size = (int32_t)আকার(UConverter);
    ((UConverter_তথ্য_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverter_তথ্য_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, status);

    ফেরত &localClone->cnv;
}

স্থির শূন্য U_CALLCONV
_HZ_GetUnicodeSet(ধ্রুবক UConverter *cnv,
                  ধ্রুবক USetAdder *sa,
                  UConverterUnicodeSet which,
                  UErrorCode *pErrorCode) {
    /* HZ converts all of ASCII */
    sa->addRange(sa->set, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGetFilteredUnicodeSetForUnicode(
        ((UConverter_তথ্য_HZ*)cnv->extraInfo)->gbConverter->shared_তথ্য,
        sa, which, UCNV_SET_FILTER_HZ,
        pErrorCode);
}
U_CDECL_END
স্থির ধ্রুবক UConverterImpl _HZImpl={

    UCNV_HZ,
    
    nullptr,
    nullptr,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_toUnicode_HZ_OFFSETS_LOGIC,
    UConverter_toUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    nullptr,
    
    nullptr,
    nullptr,
    _HZ_WriteSub,
    _HZ_SafeClone,
    _HZ_GetUnicodeSet,
    nullptr,
    nullptr
};

স্থির ধ্রুবক UConverter_Static_তথ্য HZStatic_তথ্য={
    আকার(UConverter_Static_তথ্য),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        মিথ্যা, 
        মিথ্যা,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ধ্রুবক UConverter_Shared_তথ্য _HZData=
        UCNV_IMMUTABLE_SHARED_তথ্য_INITIALIZER(&HZStatic_তথ্য, &_HZImpl);

#endif /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
