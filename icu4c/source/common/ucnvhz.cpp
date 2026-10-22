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


#define CONCAT_ESCAPE_MACRO(args, targetIndex,targetLength,strToAppend, err, len,sourceIndex) UPRV_BLOCK_MACRO_BEGIN {      \
    যাবত্(len-->0){                                                                                                         \
        যদি(targetIndex < targetLength){                                                                                     \
            args->target[targetIndex] = (unsigned বর্ণ) *strToAppend;                                                       \
            যদি(args->offsets!=nullptr){                                                                                        \
                *(offsets++) = sourceIndex-1;                                                                               \
            }                                                                                                               \
            targetIndex++;                                                                                                  \
        }                                                                                                                   \
        অন্যথা{                                                                                                               \
            args->converter->charErrorBuffer[(পূর্ণাঙ্ক)args->converter->charErrorBufferLength++] = (unsigned বর্ণ) *strToAppend; \
            *err =U_BUFFER_OVERFLOW_ERROR;                                                                                  \
        }                                                                                                                   \
        strToAppend++;                                                                                                      \
    }                                                                                                                       \
} UPRV_BLOCK_MACRO_END


প্রকারনির্দেশ সংরচনা{
    UConverter* gbConverter;
    int32_t targetIndex;
    int32_t sourceIndex;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool isEmptySegment;
}UConverterDataHZ;


U_CDECL_BEGIN
স্থির শূন্য  U_CALLCONV
_HZOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    UConverter *gbConverter;
    যদি(pArgs->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", errorCode);  /* errorCode carries result */
        প্রত্যাগম;
    }
    gbConverter = ucnv_open("GBK", errorCode);
    যদি(U_FAILURE(*errorCode)) {
        প্রত্যাগম;
    }
    cnv->toUnicodeStatus = 0;
    cnv->fromUnicodeStatus= 0;
    cnv->mode=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, পরিমাণ(UConverterDataHZ));
    যদি(cnv->extraInfo != nullptr){
        ((UConverterDataHZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    অন্যথা {
        ucnv_close(gbConverter);
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
        প্রত্যাগম;
    }
}

স্থির শূন্য  U_CALLCONV
_HZClose(UConverter *cnv){
    যদি(cnv->extraInfo != nullptr) {
        ucnv_close (((UConverterDataHZ *) (cnv->extraInfo))->gbConverter);
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
            ((UConverterDataHZ*)cnv->extraInfo)->isStateDBCS = অসত্য;
            ((UConverterDataHZ*)cnv->extraInfo)->isEmptySegment = অসত্য;
        }
    }
    যদি(choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUnicodeStatus= 0;
        cnv->fromUChar32=0x0000; 
        যদি(cnv->extraInfo != nullptr){
            ((UConverterDataHZ*)cnv->extraInfo)->isEscapeAppended = অসত্য;
            ((UConverterDataHZ*)cnv->extraInfo)->targetIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->sourceIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->isTargetUCharDBCS = অসত্য;
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
    বর্ণ tempBuf[2];
    ধ্রুব বর্ণ *mySource = ( বর্ণ *) args->source;
    char16_t *myTarget = args->target;
    ধ্রুব বর্ণ *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    int32_t mySourceChar = 0x0000;
    UConverterDataHZ* myData=(UConverterDataHZ*)(args->converter->extraInfo);
    tempBuf[0]=0; 
    tempBuf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    যাবত্(mySource< mySourceLimit){
        
        যদি(myTarget < args->targetLimit){
            
            mySourceChar= (unsigned বর্ণ) *mySource++;

            যদি(args->converter->mode == UCNV_TILDE) {
                /* second byte after ~ */
                args->converter->mode=0;
                বিকল্প(mySourceChar) {
                পক্ষ 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    অনুবর্ত;
                পক্ষ UCNV_TILDE:
                    যদি(args->offsets) {
                        args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    myData->isEmptySegment = অসত্য;
                    অনুবর্ত;
                পক্ষ UCNV_OPEN_BRACE:
                পক্ষ UCNV_CLOSE_BRACE:
                    myData->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    যদি (myData->isEmptySegment) {
                        myData->isEmptySegment = অসত্য; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toUBytes[0] = UCNV_TILDE;
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                        args->target = myTarget;
                        args->source = mySource;
                        প্রত্যাগম;
                    }
                    myData->isEmptySegment = সত্য;
                    অনুবর্ত;
                স্বভাব:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    myData->isEmptySegment = অসত্য; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUBytes[0] = UCNV_TILDE;
                    যদি( myData->isStateDBCS ?
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
                    প্রত্যাগম;
                }
            } অন্যথা যদি(myData->isStateDBCS) {
                যদি(args->converter->toUnicodeStatus == 0x00){
                    /* lead byte */
                    যদি(mySourceChar == UCNV_TILDE) {
                        args->converter->mode = UCNV_TILDE;
                    } অন্যথা {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        args->converter->toUnicodeStatus = (uint32_t) (mySourceChar | 0x100);
                        myData->isEmptySegment = অসত্য; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    অনুবর্ত;
                }
                অন্যথা{
                    /* trail byte */
                    পূর্ণাঙ্ক leadIsOk, trailIsOk;
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
                        tempBuf[0] = (বর্ণ) (leadByte+0x80) ;
                        tempBuf[1] = (বর্ণ) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimpleGetNextUChar(myData->gbConverter->sharedData,
                            tempBuf, 2, args->converter->useFallback);
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
                    অনুবর্ত;
                } অন্যথা যদি(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    myData->isEmptySegment = অসত্য; /* the segment has something valid */
                } অন্যথা {
                    targetUniChar = 0xffff;
                    myData->isEmptySegment = অসত্য; /* different error here, reset this to avoid spurious future error */
                }
            }
            যদি(targetUniChar < 0xfffe){
                যদি(args->offsets) {
                    args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 1-(myData->isStateDBCS));
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
                ভঙ্গ;
            }
        }
        অন্যথা{
            *err =U_BUFFER_OVERFLOW_ERROR;
            ভঙ্গ;
        }
    }

    args->target = myTarget;
    args->source = mySource;
}


স্থির শূন্য  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                      UErrorCode * err){
    ধ্রুব char16_t *mySource = args->source;
    বর্ণ *myTarget = args->target;
    int32_t* offsets = args->offsets;
    int32_t mySourceIndex = 0;
    int32_t myTargetIndex = 0;
    int32_t targetLength = (int32_t)(args->targetLimit - myTarget);
    int32_t mySourceLength = (int32_t)(args->sourceLimit - args->source);
    uint32_t targetUniChar = 0x0000;
    UChar32 mySourceChar = 0x0000;
    UConverterDataHZ *myConverterData=(UConverterDataHZ*)args->converter->extraInfo;
    UBool isTargetUCharDBCS = myConverterData->isTargetUCharDBCS;
    UBool oldIsTargetUCharDBCS;
    পূর্ণাঙ্ক len =0;
    ধ্রুব বর্ণ* escSeq=nullptr;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    যদি(args->converter->fromUChar32!=0 && myTargetIndex < targetLength) {
        গচ্ছ getTrail;
    }
    /*writing the char to the output stream */
    যাবত্ (mySourceIndex < mySourceLength){
        targetUniChar = missingCharMarker;
        যদি (myTargetIndex < targetLength){
            
            mySourceChar = mySource[mySourceIndex++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            যদি(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                অনুবর্ত;
            } অন্যথা যদি(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } অন্যথা {
                int32_t length= ucnv_MBCSFromUChar32(myConverterData->gbConverter->sharedData,
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
               myConverterData->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 যদি(oldIsTargetUCharDBCS != isTargetUCharDBCS || !myConverterData->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    যদি(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = সত্য;
                    }
                    অন্যথা{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = সত্য;
                        
                    }
                }
            
                যদি(isTargetUCharDBCS){
                    যদি( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] =(বর্ণ) (targetUniChar >> 8);
                        যদি(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        যদি(myTargetIndex < targetLength){
                            myTarget[myTargetIndex++] =(বর্ণ) targetUniChar;
                            যদি(offsets){
                                *(offsets++) = mySourceIndex-1;
                            }
                        }অন্যথা{
                            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (বর্ণ) targetUniChar;
                            *err = U_BUFFER_OVERFLOW_ERROR;
                        } 
                    }অন্যথা{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =(বর্ণ) (targetUniChar >> 8);
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (বর্ণ) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }

                }অন্যথা{
                    যদি( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] = (বর্ণ) (targetUniChar );
                        যদি(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        
                    }অন্যথা{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (বর্ণ) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
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
                        যদি(mySourceIndex <  mySourceLength) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) args->source[mySourceIndex];
                            যদি(U16_IS_TRAIL(trail)) {
                                ++mySourceIndex;
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
                ভঙ্গ;
            }
        }
        অন্যথা{
            *err = U_BUFFER_OVERFLOW_ERROR;
            ভঙ্গ;
        }
        targetUniChar=missingCharMarker;
    }

    args->target += myTargetIndex;
    args->source += mySourceIndex;
    myConverterData->isTargetUCharDBCS = isTargetUCharDBCS;
}

স্থির শূন্য U_CALLCONV
_HZ_WriteSub(UConverterFromUnicodeArgs *args, int32_t offsetIndex, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverterDataHZ *convData=(UConverterDataHZ *) cnv->extraInfo;
    বর্ণ *p;
    বর্ণ buffer[4];
    p = buffer;
    
    যদি( convData->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        convData->isTargetUCharDBCS=অসত্য;
    }
    *p++= (বর্ণ)cnv->subChars[0];

    ucnv_cbFromUWriteBytes(args,
                           buffer, (int32_t)(p - buffer),
                           offsetIndex, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
সংরচনা cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverterDataHZ mydata;
};


স্থির UConverter *  U_CALLCONV
_HZ_SafeClone(ধ্রুব UConverter *cnv, 
              শূন্য *স্তূপ_Buffer, 
              int32_t *pBufferSize, 
              UErrorCode *status)
{
    সংরচনা cloneHZStruct * localClone;
    int32_t size, bufferSizeNeeded = পরিমাণ(সংরচনা cloneHZStruct);

    যদি (U_FAILURE(*status)){
        প্রত্যাগম nullptr;
    }

    যদি (*pBufferSize == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *pBufferSize = bufferSizeNeeded;
        প্রত্যাগম nullptr;
    }

    localClone = (সংরচনা cloneHZStruct *)স্তূপ_Buffer;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, পরিমাণ(UConverterDataHZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = সত্য;

    /* deep-clone the sub-converter */
    size = (int32_t)পরিমাণ(UConverter);
    ((UConverterDataHZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverterDataHZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, status);

    প্রত্যাগম &localClone->cnv;
}

স্থির শূন্য U_CALLCONV
_HZ_GetUnicodeSet(ধ্রুব UConverter *cnv,
                  ধ্রুব USetAdder *sa,
                  UConverterUnicodeSet which,
                  UErrorCode *pErrorCode) {
    /* HZ converts all of ASCII */
    sa->addRange(sa->set, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGetFilteredUnicodeSetForUnicode(
        ((UConverterDataHZ*)cnv->extraInfo)->gbConverter->sharedData,
        sa, which, UCNV_SET_FILTER_HZ,
        pErrorCode);
}
U_CDECL_END
স্থির ধ্রুব UConverterImpl _HZImpl={

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

স্থির ধ্রুব UConverterStaticData _HZStaticData={
    পরিমাণ(UConverterStaticData),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        অসত্য, 
        অসত্য,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ধ্রুব UConverterSharedData _HZData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_HZStaticData, &_HZImpl);

#endif /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
