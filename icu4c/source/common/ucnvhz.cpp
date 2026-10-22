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

#அந்தர்பாவ "unicode/utypes.h"

#எனில் !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ucnv.h"
#அந்தர்பாவ "unicode/ucnv_cb.h"
#அந்தர்பாவ "unicode/uset.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "ucnv_bld.h"
#அந்தர்பாவ "ucnv_cnv.h"
#அந்தர்பாவ "ucnv_imp.h"

#define UCNV_TILDE 0x7E          /* ~ */
#define UCNV_OPEN_BRACE 0x7B     /* { */
#define UCNV_CLOSE_BRACE 0x7D   /* } */
#define SB_ESCAPE    "\x7E\x7D"
#define DB_ESCAPE    "\x7E\x7B"
#define TILDE_ESCAPE "\x7E\x7E"
#define ESC_LEN       2


#define CONCAT_ESCAPE_MACRO(args, targetIndex,targetLength,strToAppend, err, len,sourceIndex) UPRV_BLOCK_MACRO_BEGIN {      \
    போது(len-->0){                                                                                                         \
        எனில்(targetIndex < targetLength){                                                                                     \
            args->target[targetIndex] = (unsigned எழுத்து) *strToAppend;                                                       \
            எனில்(args->offsets!=nullptr){                                                                                        \
                *(offsets++) = sourceIndex-1;                                                                               \
            }                                                                                                               \
            targetIndex++;                                                                                                  \
        }                                                                                                                   \
        இல்லையெனில்{                                                                                                               \
            args->converter->charErrorBuffer[(முழு)args->converter->charErrorBufferLength++] = (unsigned எழுத்து) *strToAppend; \
            *err =U_BUFFER_OVERFLOW_ERROR;                                                                                  \
        }                                                                                                                   \
        strToAppend++;                                                                                                      \
    }                                                                                                                       \
} UPRV_BLOCK_MACRO_END


வகைவரை கட்டமைப்பு{
    UConverter* gbConverter;
    int32_t targetIndex;
    int32_t sourceIndex;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool isEmptySegment;
}UConverterDataHZ;


U_CDECL_BEGIN
நிலையான வெறுமை  U_CALLCONV
_HZOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    UConverter *gbConverter;
    எனில்(pArgs->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", errorCode);  /* errorCode carries result */
        திரும்பு;
    }
    gbConverter = ucnv_open("GBK", errorCode);
    எனில்(U_FAILURE(*errorCode)) {
        திரும்பு;
    }
    cnv->toUnicodeStatus = 0;
    cnv->fromUnicodeStatus= 0;
    cnv->mode=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, அளவு(UConverterDataHZ));
    எனில்(cnv->extraInfo != nullptr){
        ((UConverterDataHZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    இல்லையெனில் {
        ucnv_close(gbConverter);
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
        திரும்பு;
    }
}

நிலையான வெறுமை  U_CALLCONV
_HZClose(UConverter *cnv){
    எனில்(cnv->extraInfo != nullptr) {
        ucnv_close (((UConverterDataHZ *) (cnv->extraInfo))->gbConverter);
        எனில்(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = nullptr;
    }
}

நிலையான வெறுமை  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    எனில்(choice<=UCNV_RESET_TO_UNICODE) {
        cnv->toUnicodeStatus = 0;
        cnv->mode=0;
        எனில்(cnv->extraInfo != nullptr){
            ((UConverterDataHZ*)cnv->extraInfo)->isStateDBCS = பொய்;
            ((UConverterDataHZ*)cnv->extraInfo)->isEmptySegment = பொய்;
        }
    }
    எனில்(choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUnicodeStatus= 0;
        cnv->fromUChar32=0x0000; 
        எனில்(cnv->extraInfo != nullptr){
            ((UConverterDataHZ*)cnv->extraInfo)->isEscapeAppended = பொய்;
            ((UConverterDataHZ*)cnv->extraInfo)->targetIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->sourceIndex = 0;
            ((UConverterDataHZ*)cnv->extraInfo)->isTargetUCharDBCS = பொய்;
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


நிலையான வெறுமை  U_CALLCONV
UConverter_toUnicode_HZ_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    எழுத்து tempBuf[2];
    மாறிலி எழுத்து *mySource = ( எழுத்து *) args->source;
    char16_t *myTarget = args->target;
    மாறிலி எழுத்து *mySourceLimit = args->sourceLimit;
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
    
    போது(mySource< mySourceLimit){
        
        எனில்(myTarget < args->targetLimit){
            
            mySourceChar= (unsigned எழுத்து) *mySource++;

            எனில்(args->converter->mode == UCNV_TILDE) {
                /* second byte after ~ */
                args->converter->mode=0;
                தேர்வு(mySourceChar) {
                நிலை 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    தொடர்;
                நிலை UCNV_TILDE:
                    எனில்(args->offsets) {
                        args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    myData->isEmptySegment = பொய்;
                    தொடர்;
                நிலை UCNV_OPEN_BRACE:
                நிலை UCNV_CLOSE_BRACE:
                    myData->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    எனில் (myData->isEmptySegment) {
                        myData->isEmptySegment = பொய்; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toUBytes[0] = UCNV_TILDE;
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                        args->target = myTarget;
                        args->source = mySource;
                        திரும்பு;
                    }
                    myData->isEmptySegment = உண்மை;
                    தொடர்;
                இயல்பு:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    myData->isEmptySegment = பொய்; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUBytes[0] = UCNV_TILDE;
                    எனில்( myData->isStateDBCS ?
                            (0x21 <= mySourceChar && mySourceChar <= 0x7e) :
                            mySourceChar <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        args->converter->toULength = 1;
                        --mySource;
                    } இல்லையெனில் {
                        /* Include the current byte in the illegal sequence. */
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                    }
                    args->target = myTarget;
                    args->source = mySource;
                    திரும்பு;
                }
            } இல்லையெனில் எனில்(myData->isStateDBCS) {
                எனில்(args->converter->toUnicodeStatus == 0x00){
                    /* lead byte */
                    எனில்(mySourceChar == UCNV_TILDE) {
                        args->converter->mode = UCNV_TILDE;
                    } இல்லையெனில் {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        args->converter->toUnicodeStatus = (uint32_t) (mySourceChar | 0x100);
                        myData->isEmptySegment = பொய்; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    தொடர்;
                }
                இல்லையெனில்{
                    /* trail byte */
                    முழு leadIsOk, trailIsOk;
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
                    எனில் (leadIsOk && trailIsOk) {
                        tempBuf[0] = (எழுத்து) (leadByte+0x80) ;
                        tempBuf[1] = (எழுத்து) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimpleGetNextUChar(myData->gbConverter->sharedData,
                            tempBuf, 2, args->converter->useFallback);
                        mySourceChar= (leadByte << 8) | mySourceChar;
                    } இல்லையெனில் எனில் (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --mySource;
                        mySourceChar = (int32_t)leadByte;
                    } இல்லையெனில் {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar= 0x10000 | (leadByte << 8) | mySourceChar;
                    }
                    args->converter->toUnicodeStatus =0x00;
                }
            }
            இல்லையெனில்{
                எனில்(mySourceChar == UCNV_TILDE) {
                    args->converter->mode = UCNV_TILDE;
                    தொடர்;
                } இல்லையெனில் எனில்(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    myData->isEmptySegment = பொய்; /* the segment has something valid */
                } இல்லையெனில் {
                    targetUniChar = 0xffff;
                    myData->isEmptySegment = பொய்; /* different error here, reset this to avoid spurious future error */
                }
            }
            எனில்(targetUniChar < 0xfffe){
                எனில்(args->offsets) {
                    args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 1-(myData->isStateDBCS));
                }

                *(myTarget++)=(char16_t)targetUniChar;
            }
            இல்லையெனில் /* targetUniChar>=0xfffe */ {
                எனில்(targetUniChar == 0xfffe){
                    *err = U_INVALID_CHAR_FOUND;
                }
                இல்லையெனில்{
                    *err = U_ILLEGAL_CHAR_FOUND;
                }
                எனில்(mySourceChar > 0xff){
                    args->converter->toUBytes[0] = (uint8_t)(mySourceChar >> 8);
                    args->converter->toUBytes[1] = (uint8_t)mySourceChar;
                    args->converter->toULength=2;
                }
                இல்லையெனில்{
                    args->converter->toUBytes[0] = (uint8_t)mySourceChar;
                    args->converter->toULength=1;
                }
                நிறுத்து;
            }
        }
        இல்லையெனில்{
            *err =U_BUFFER_OVERFLOW_ERROR;
            நிறுத்து;
        }
    }

    args->target = myTarget;
    args->source = mySource;
}


நிலையான வெறுமை  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                      UErrorCode * err){
    மாறிலி char16_t *mySource = args->source;
    எழுத்து *myTarget = args->target;
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
    முழு len =0;
    மாறிலி எழுத்து* escSeq=nullptr;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    எனில்(args->converter->fromUChar32!=0 && myTargetIndex < targetLength) {
        செல் getTrail;
    }
    /*writing the char to the output stream */
    போது (mySourceIndex < mySourceLength){
        targetUniChar = missingCharMarker;
        எனில் (myTargetIndex < targetLength){
            
            mySourceChar = mySource[mySourceIndex++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            எனில்(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                தொடர்;
            } இல்லையெனில் எனில்(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } இல்லையெனில் {
                int32_t length= ucnv_MBCSFromUChar32(myConverterData->gbConverter->sharedData,
                    mySourceChar,&targetUniChar,args->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                எனில்( length == 2 &&
                    (uint16_t)(targetUniChar - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(targetUniChar - 0xa1) <= (0xfe - 0xa1)
                ) {
                    targetUniChar -= 0x8080;
                } இல்லையெனில் {
                    targetUniChar = missingCharMarker;
                }
            }
            எனில் (targetUniChar != missingCharMarker){
               myConverterData->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 எனில்(oldIsTargetUCharDBCS != isTargetUCharDBCS || !myConverterData->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    எனில்(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = உண்மை;
                    }
                    இல்லையெனில்{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, myTargetIndex, targetLength, escSeq,err,len,mySourceIndex);
                        myConverterData->isEscapeAppended = உண்மை;
                        
                    }
                }
            
                எனில்(isTargetUCharDBCS){
                    எனில்( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] =(எழுத்து) (targetUniChar >> 8);
                        எனில்(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        எனில்(myTargetIndex < targetLength){
                            myTarget[myTargetIndex++] =(எழுத்து) targetUniChar;
                            எனில்(offsets){
                                *(offsets++) = mySourceIndex-1;
                            }
                        }இல்லையெனில்{
                            args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (எழுத்து) targetUniChar;
                            *err = U_BUFFER_OVERFLOW_ERROR;
                        } 
                    }இல்லையெனில்{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] =(எழுத்து) (targetUniChar >> 8);
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (எழுத்து) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }

                }இல்லையெனில்{
                    எனில்( myTargetIndex <targetLength){
                        myTarget[myTargetIndex++] = (எழுத்து) (targetUniChar );
                        எனில்(offsets){
                            *(offsets++) = mySourceIndex-1;
                        }
                        
                    }இல்லையெனில்{
                        args->converter->charErrorBuffer[args->converter->charErrorBufferLength++] = (எழுத்து) targetUniChar;
                        *err = U_BUFFER_OVERFLOW_ERROR;
                    }
                }

            }
            இல்லையெனில்{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                எனில்(U16_IS_SURROGATE(mySourceChar)) {
                    எனில்(U16_IS_SURROGATE_LEAD(mySourceChar)) {
                        args->converter->fromUChar32=mySourceChar;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        எனில்(mySourceIndex <  mySourceLength) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) args->source[mySourceIndex];
                            எனில்(U16_IS_TRAIL(trail)) {
                                ++mySourceIndex;
                                mySourceChar=U16_GET_SUPPLEMENTARY(args->converter->fromUChar32, trail);
                                args->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_FOUND;
                                /* exit this condition tree */
                            } இல்லையெனில் {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } இல்லையெனில் {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } இல்லையெனில் {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } இல்லையெனில் {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=mySourceChar;
                நிறுத்து;
            }
        }
        இல்லையெனில்{
            *err = U_BUFFER_OVERFLOW_ERROR;
            நிறுத்து;
        }
        targetUniChar=missingCharMarker;
    }

    args->target += myTargetIndex;
    args->source += mySourceIndex;
    myConverterData->isTargetUCharDBCS = isTargetUCharDBCS;
}

நிலையான வெறுமை U_CALLCONV
_HZ_WriteSub(UConverterFromUnicodeArgs *args, int32_t offsetIndex, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverterDataHZ *convData=(UConverterDataHZ *) cnv->extraInfo;
    எழுத்து *p;
    எழுத்து buffer[4];
    p = buffer;
    
    எனில்( convData->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        convData->isTargetUCharDBCS=பொய்;
    }
    *p++= (எழுத்து)cnv->subChars[0];

    ucnv_cbFromUWriteBytes(args,
                           buffer, (int32_t)(p - buffer),
                           offsetIndex, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
கட்டமைப்பு cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverterDataHZ mydata;
};


நிலையான UConverter *  U_CALLCONV
_HZ_SafeClone(மாறிலி UConverter *cnv, 
              வெறுமை *அடுக்கு_Buffer, 
              int32_t *pBufferSize, 
              UErrorCode *status)
{
    கட்டமைப்பு cloneHZStruct * localClone;
    int32_t size, bufferSizeNeeded = அளவு(கட்டமைப்பு cloneHZStruct);

    எனில் (U_FAILURE(*status)){
        திரும்பு nullptr;
    }

    எனில் (*pBufferSize == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *pBufferSize = bufferSizeNeeded;
        திரும்பு nullptr;
    }

    localClone = (கட்டமைப்பு cloneHZStruct *)அடுக்கு_Buffer;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, அளவு(UConverterDataHZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = உண்மை;

    /* deep-clone the sub-converter */
    size = (int32_t)அளவு(UConverter);
    ((UConverterDataHZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverterDataHZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, status);

    திரும்பு &localClone->cnv;
}

நிலையான வெறுமை U_CALLCONV
_HZ_GetUnicodeSet(மாறிலி UConverter *cnv,
                  மாறிலி USetAdder *sa,
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
நிலையான மாறிலி UConverterImpl _HZImpl={

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

நிலையான மாறிலி UConverterStaticData _HZStaticData={
    அளவு(UConverterStaticData),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        பொய், 
        பொய்,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

மாறிலி UConverterSharedData _HZData=
        UCNV_IMMUTABLE_SHARED_DATA_INITIALIZER(&_HZStaticData, &_HZImpl);

#endif /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
