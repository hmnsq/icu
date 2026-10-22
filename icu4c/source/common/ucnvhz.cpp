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

#അന്തര്ഭാവ "unicode/utypes.h"

#എങ്കിൽ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ucnv.h"
#അന്തര്ഭാവ "unicode/ucnv_cb.h"
#അന്തര്ഭാവ "unicode/uset.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "ucnv_bld.h"
#അന്തര്ഭാവ "ucnv_cnv.h"
#അന്തര്ഭാവ "ucnv_imp.h"

#define UCNV_TILDE 0x7E          /* ~ */
#define UCNV_OPEN_BRACE 0x7B     /* { */
#define UCNV_CLOSE_BRACE 0x7D   /* } */
#define SB_ESCAPE    "\x7E\x7D"
#define DB_ESCAPE    "\x7E\x7B"
#define TILDE_ESCAPE "\x7E\x7E"
#define ESC_LEN       2


#define CONCAT_ESCAPE_MACRO(args, targetIndex,targetLength,strToAppend, err, len,sourceIndex) UPRV_BLOCK_MACRO_BEGIN {      \
    ആകുമ്പോൾ(len-->0){                                                                                                         \
        എങ്കിൽ(targetIndex < targetLength){                                                                                     \
            args->target[targetIndex] = (unsigned അക്ഷരം) *strToAppend;                                                       \
            എങ്കിൽ(args->offsets!=nullptr){                                                                                        \
                *(offsets++) = sourceIndex-1;                                                                               \
            }                                                                                                               \
            targetIndex++;                                                                                                  \
        }                                                                                                                   \
        അല്ലെങ്കിൽ{                                                                                                               \
            args->converter->charErrorBuffer[(പൂർണ്ണസംഖ്യ)args->converter->charErrorBufferLength++] = (unsigned അക്ഷരം) *strToAppend; \
            *err =U_BUFFER_OVERFLOW_ERROR;                                                                                  \
        }                                                                                                                   \
        strToAppend++;                                                                                                      \
    }                                                                                                                       \
} UPRV_BLOCK_MACRO_END


തരനിർവചനം ഘടന{
    UConverter* gbConverter;
    int32_t targetIndex;
    int32_t sourceIndex;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool isEmptySegment;
}UConverterDataHZ;


U_CDECL_BEGIN
സ്ഥിതം ശൂന്യം  U_CALLCONV
_HZOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    UConverter *gbConverter;
    എങ്കിൽ(pArgs->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", errorCode);  /* errorCode carries result */
        തിരികെ;
    }
    gbConverter = ucnv_open("GBK", errorCode);
    എങ്കിൽ(U_FAILURE(*errorCode)) {
        തിരികെ;
    }
    cnv->toUnicodeStatus = 0;
    cnv->fromUnicodeStatus= 0;
    cnv->mode=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, വലുപ്പം(UConverterDataHZ));
    എങ്കിൽ(cnv->extraInfo != nullptr){
        ((UConverterDataHZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    അല്ലെങ്കിൽ {
        ucnv_close(gbConverter);
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
        തിരികെ;
    }
}

സ്ഥിതം ശൂന്യം  U_CALLCONV
_HZClose(UConverter *cnv){
    എങ്കിൽ(cnv->extraInfo != nullptr) {
        ucnv_close (((UConverterDataHZ *) (cnv->extraInfo))->gbConverter);
        എങ്കിൽ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = nullptr;
    }
}

സ്ഥിതം ശൂന്യം  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    എങ്കിൽ(choice<=UCNV_RESET_TO_UNICODE) {
        cnv->toUnicodeStatus = 0;
        cnv->mode=0;
        എങ്കിൽ(cnv->extraInfo != nullptr){
            ((UConverterDataHZ*)cnv->extraInfo)->isStateDBCS = അസത്യം;
            ((UConverterDataHZ*)cnv->extraInfo)->isEmptySegment = അസത്യം;
        }
    }
    എങ്കിൽ(choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUnicodeStatus= 0;
        cnv->fromUChar32=0x0000; 
        എങ്കിൽ(cnv->extraInfo != nullptr){
            ((UConverterDataHZ*)cnv->extraInfo)->isEscapeAppended = അസത്യം;
            ((UConverterDataHZ*)cnv->extraInfo)->targetIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->sourceIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->isTargetUCharDBCS = അസത്യം;
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


സ്ഥിതം ശൂന്യം  U_CALLCONV
UConverter_toUnicode_HZ_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    അക്ഷരം tempBuf[2];
    സ്ഥിരം അക്ഷരം *mySource = ( അക്ഷരം *) args->source;
    char16_t *myTarget = args->target;
    സ്ഥിരം അക്ഷരം *mySourceLimit = args->sourceLimit;
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
    
    ആകുമ്പോൾ(mySource< mySourceLimit){
        
        എങ്കിൽ(myTarget < args->targetLimit){
            
            mySourceChar= (unsigned അക്ഷരം) *mySource++;

            എങ്കിൽ(args->converter->mode == UCNV_TILDE) {
                /* second byte after ~ */
                args->converter->mode=0;
                തിരഞ്ഞെടുപ്പ്(mySourceChar) {
                അവസ്ഥ 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    തുടരുക;
                അവസ്ഥ UCNV_TILDE:
                    എങ്കിൽ(args->offsets) {
                        args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    myData->isEmptySegment = അസത്യം;
                    തുടരുക;
                അവസ്ഥ UCNV_OPEN_BRACE:
                അവസ്ഥ UCNV_CLOSE_BRACE:
                    myData->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    എങ്കിൽ (myData->isEmptySegment) {
                        myData->isEmptySegment = അസത്യം; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toUBytes[0] = UCNV_TILDE;
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                        args->target = myTarget;
                        args->source = mySource;
                        തിരികെ;
                    }
                    myData->isEmptySegment = സത്യം;
                    തുടരുക;
                സ്ഥിരസ്ഥിതി:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    myData->isEmptySegment = അസത്യം; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUBytes[0] = UCNV_TILDE;
                    എങ്കിൽ( myData->isStateDBCS ?
                            (0x21 <= mySourceChar && mySourceChar <= 0x7e) :
                            mySourceChar <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        args->converter->toULength = 1;
                        --mySource;
                    } അല്ലെങ്കിൽ {
                        /* Include the current byte in the illegal sequence. */
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                    }
                    args->target = myTarget;
                    args->source = mySource;
                    തിരികെ;
                }
            } അല്ലെങ്കിൽ എങ്കിൽ(myData->isStateDBCS) {
                എങ്കിൽ(args->converter->toUnicodeStatus == 0x00){
                    /* lead byte */
                    എങ്കിൽ(mySourceChar == UCNV_TILDE) {
                        args->converter->mode = UCNV_TILDE;
                    } അല്ലെങ്കിൽ {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        args->converter->toUnicodeStatus = (uint32_t) (mySourceChar | 0x100);
                        myData->isEmptySegment = അസത്യം; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    തുടരുക;
                }
                അല്ലെങ്കിൽ{
                    /* trail byte */
                    പൂർണ്ണസംഖ്യ leadIsOk, trailIsOk;
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
                    എങ്കിൽ (leadIsOk && trailIsOk) {
                        tempBuf[0] = (അക്ഷരം) (leadByte+0x80) ;
                        tempBuf[1] = (അക്ഷരം) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimpleGetNextUChar(myData->gbConverter->sharedData,
                            tempBuf, 2, args->converter->useFallback);
                        mySourceChar= (leadByte << 8) | mySourceChar;
                    } അല്ലെങ്കിൽ എങ്കിൽ (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --mySource;
                        mySourceChar = (int32_t)leadByte;
                    } അല്ലെങ്കിൽ {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar= 0x10000 | (leadByte << 8) | mySourceChar;
                    }
                    args->converter->toUnicodeStatus =0x00;
                }
            }
            അല്ലെങ്കിൽ{
                എങ്കിൽ(mySourceChar == UCNV_TILDE) {
                    args->converter->mode = UCNV_TILDE;
                    തുടരുക;
                } അല്ലെങ്കിൽ എങ്കിൽ(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    myData->isEmptySegment = അസത്യം; /* the segment has something valid */
                } അല്ലെങ്കിൽ {
                    targetUniChar = 0xffff;
                    myData->isEmptySegment = അസത്യം; /* different error here, reset this to avoid spurious future error */
                }
            }
            എങ്കിൽ(targetUniChar < 0xfffe){
                എങ്കിൽ(args->offsets) {
                    args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 1-(myData->isStateDBCS));
                }

                *(myTarget++)=(char16_t)targetUniChar;
            }
            അല്ലെങ്കിൽ /* targetUniChar>=0xfffe */ {
                എങ്കിൽ(targetUniChar == 0xfffe){
                    *err = U_INVALID_CHAR_FOUND;
                }
                അല്ലെങ്കിൽ{
                    *err = U_ILLEGAL_CHAR_FOUND;
                }
                എങ്കിൽ(mySourceChar > 0xff){
                    args->converter->toUBytes[0] = (uint8_t)(mySourceChar >> 8);
                    args->converter->toUBytes[1] = (uint8_t)mySourceChar;
                    args->converter->toULength=2;
                }
                അല്ലെങ്കിൽ{
                    args->converter->toUBytes[0] = (uint8_t)mySourceChar;
                    args->converter->toULength=1;
                }
                നിർത്തുക;
            }
        }
        അല്ലെങ്കിൽ{
            *err =U_BUFFER_OVERFLOW_ERROR;
            നിർത്തുക;
        }
    }

    args->target = myTarget;
    args->source = mySource;
}


സ്ഥിതം ശൂന്യം  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                      UErrorCode * err){
    സ്ഥിരം char16_t *mySource = args->source;
    അക്ഷരം *myTarget = args->target;
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
    പൂർണ്ണസംഖ്യ len =0;
    സ്ഥിരം അക്ഷരം* escSeq=nullptr;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    എങ്കിൽ(args->converter->fromUChar32!=0 && myTargetIndex < targetLength) {
        പോകുക getTrail;
    }
    /*writing the char to the output stream */
    ആകുമ്പോൾ (mySourceIndex < mySourceLength){
        targetUniChar = missingCharMarker;
        എങ്കിൽ (myTargetIndex < targetLength){
            
            mySourceChar = mySource[mySourceIndex++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            എങ്കിൽ(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                തുടരുക;
            } അല്ലെങ്കിൽ എങ്കിൽ(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } അല്ലെങ്കിൽ {
                int32_t length= ucnv_MBCSFromUChar32(myConverterData->gbConverter->sharedData,
                    mySourceChar,&targetUniChar,args->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                എങ്കിൽ( length == 2 &&
                    (uint16_t)(targetUniChar - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(targetUniChar - 0xa1) <= (0xfe - 0xa1)
                ) {
                    targetUniChar -= 0x8080;
                } അല്ലെങ്കിൽ {
                    targetUniChar = missingCharMarker;
                }
            }
            എങ്കിൽ (targetUniChar != missingCharMarker){
               myConverterData->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 എങ്കിൽ(oldIsTargetUCharDBCS != isTargetUCharDBCS || !myConverterData->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    എങ്കിൽ(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = സത്യം;
                    }
                    അല്ലെങ്കിൽ{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = സത്യം;
                        
                    }
                }
            
                എങ്കിൽ(isTargetUCharDBCS){
                    എങ്കിൽ( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] =(അക്ഷരം) (targetUniChar >> 8);
                        എങ്കിൽ(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        എങ്കിൽ(myTargetIndex < targetLength){
                            myTarget[myTargetIndex++] =(അക്ഷരം) targetUniChar;
                            എങ്കിൽ(offsets){
                                *(offsets++) = mySourceIndex-1;
                            }
                        }അല്ലെങ്കിൽ{
                            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (അക്ഷരം) targetUniChar;
                            *err = U_BUFFER_OVERFLOW_ERROR;
                        } 
                    }അല്ലെങ്കിൽ{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =(അക്ഷരം) (targetUniChar >> 8);
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (അക്ഷരം) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }

                }അല്ലെങ്കിൽ{
                    എങ്കിൽ( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] = (അക്ഷരം) (targetUniChar );
                        എങ്കിൽ(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        
                    }അല്ലെങ്കിൽ{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (അക്ഷരം) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }

            }
            അല്ലെങ്കിൽ{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                എങ്കിൽ(U16_IS_SURROGATE(mySourceChar)) {
                    എങ്കിൽ(U16_IS_SURROGATE_LEAD(mySourceChar)) {
                        args->converter->fromUChar32=mySourceChar;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        എങ്കിൽ(mySourceIndex <  mySourceLength) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) args->source[mySourceIndex];
                            എങ്കിൽ(U16_IS_TRAIL(trail)) {
                                ++mySourceIndex;
                                mySourceChar=U16_GET_SUPPLEMENTARY(args->converter->fromUChar32, trail);
                                args->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_FOUND;
                                /* exit this condition tree */
                            } അല്ലെങ്കിൽ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } അല്ലെങ്കിൽ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } അല്ലെങ്കിൽ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } അല്ലെങ്കിൽ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=mySourceChar;
                നിർത്തുക;
            }
        }
        അല്ലെങ്കിൽ{
            *err = U_BUFFER_OVERFLOW_ERROR;
            നിർത്തുക;
        }
        targetUniChar=missingCharMarker;
    }

    args->target += myTargetIndex;
    args->source += mySourceIndex;
    myConverterData->isTargetUCharDBCS = isTargetUCharDBCS;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
_HZ_WriteSub(UConverterFromUnicodeArgs *args, int32_t offsetIndex, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverterDataHZ *convData=(UConverterDataHZ *) cnv->extraInfo;
    അക്ഷരം *p;
    അക്ഷരം buffer[4];
    p = buffer;
    
    എങ്കിൽ( convData->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        convData->isTargetUCharDBCS=അസത്യം;
    }
    *p++= (അക്ഷരം)cnv->subChars[0];

    ucnv_cbFromUWriteBytes(args,
                           buffer, (int32_t)(p - buffer),
                           offsetIndex, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
ഘടന cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverterDataHZ mydata;
};


സ്ഥിതം UConverter *  U_CALLCONV
_HZ_SafeClone(സ്ഥിരം UConverter *cnv, 
              ശൂന്യം *അട്ടി_Buffer, 
              int32_t *pBufferSize, 
              UErrorCode *status)
{
    ഘടന cloneHZStruct * localClone;
    int32_t size, bufferSizeNeeded = വലുപ്പം(ഘടന cloneHZStruct);

    എങ്കിൽ (U_FAILURE(*status)){
        തിരികെ nullptr;
    }

    എങ്കിൽ (*pBufferSize == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *pBufferSize = bufferSizeNeeded;
        തിരികെ nullptr;
    }

    localClone = (ഘടന cloneHZStruct *)അട്ടി_Buffer;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, വലുപ്പം(UConverterDataHZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = സത്യം;

    /* deep-clone the sub-converter */
    size = (int32_t)വലുപ്പം(UConverter);
    ((UConverterDataHZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverterDataHZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, status);

    തിരികെ &localClone->cnv;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
_HZ_GetUnicodeSet(സ്ഥിരം UConverter *cnv,
                  സ്ഥിരം USetAdder *sa,
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
സ്ഥിതം സ്ഥിരം UConverterImpl _HZImpl={

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

സ്ഥിതം സ്ഥിരം UConverterStaticData _HZStaticData={
    വലുപ്പം(UConverterStaticData),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        അസത്യം, 
        അസത്യം,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

സ്ഥിരം UConverterSharedData _HZData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_HZStaticData, &_HZImpl);

#endif /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
