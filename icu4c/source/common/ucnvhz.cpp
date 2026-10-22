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

#समावेश "unicode/utypes.h"

#अगर !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#समावेश "cmemory.h"
#समावेश "unicode/ucnv.h"
#समावेश "unicode/ucnv_cb.h"
#समावेश "unicode/uset.h"
#समावेश "unicode/utf16.h"
#समावेश "ucnv_bld.h"
#समावेश "ucnv_cnv.h"
#समावेश "ucnv_imp.h"

#घोषणा UCNV_TILDE 0x7E          /* ~ */
#घोषणा UCNV_OPEN_BRACE 0x7B     /* { */
#घोषणा UCNV_CLOSE_BRACE 0x7D   /* } */
#घोषणा SB_ESCAPE    "\x7E\x7D"
#घोषणा DB_ESCAPE    "\x7E\x7B"
#घोषणा TILDE_ESCAPE "\x7E\x7E"
#घोषणा ESC_LEN       2


#घोषणा CONCAT_ESCAPE_MACRO(args, target_सूचकांक,target_लंबाई,strToAppend, err, len,source_सूचकांक) UPRV_BLOCK_MACRO_BEGIN {      \
    जबतक(len-->0){                                                                                                         \
        अगर(target_सूचकांक < target_लंबाई){                                                                                     \
            args->target[target_सूचकांक] = (अचिन्हित अक्षर) *strToAppend;                                                       \
            अगर(args->offsets!=nullptr){                                                                                        \
                *(offsets++) = source_सूचकांक-1;                                                                               \
            }                                                                                                               \
            target_सूचकांक++;                                                                                                  \
        }                                                                                                                   \
        अन्यथा{                                                                                                               \
            args->converter->char_Error_बफर[(पूर्णांक)args->converter->char_Error_बफर_लंबाई++] = (अचिन्हित अक्षर) *strToAppend; \
            *err =U_बफर_OVERFLOW_ERROR;                                                                                  \
        }                                                                                                                   \
        strToAppend++;                                                                                                      \
    }                                                                                                                       \
} UPRV_BLOCK_MACRO_END


प्रकार काष्ठा{
    UConverter* gbConverter;
    int32_t target_सूचकांक;
    int32_t source_सूचकांक;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool isEmptySegment;
}UConverter_तथ्य_HZ;


U_CDECL_BEGIN
अटल व्योम  U_CALLCONV
_HZOpen(UConverter *cnv, UConverterLoadArgs *pArgs, UErrorCode *errorCode){
    UConverter *gbConverter;
    अगर(pArgs->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", errorCode);  /* errorCode carries result */
        वापस;
    }
    gbConverter = ucnv_open("GBK", errorCode);
    अगर(U_FAILURE(*errorCode)) {
        वापस;
    }
    cnv->toUnicodeStatus = 0;
    cnv->fromUnicodeStatus= 0;
    cnv->mode=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, माप(UConverter_तथ्य_HZ));
    अगर(cnv->extraInfo != nullptr){
        ((UConverter_तथ्य_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    अन्यथा {
        ucnv_close(gbConverter);
        *errorCode = U_MEMORY_ALLOCATION_ERROR;
        वापस;
    }
}

अटल व्योम  U_CALLCONV
_HZClose(UConverter *cnv){
    अगर(cnv->extraInfo != nullptr) {
        ucnv_close (((UConverter_तथ्य_HZ *) (cnv->extraInfo))->gbConverter);
        अगर(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = nullptr;
    }
}

अटल व्योम  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    अगर(choice<=UCNV_RESET_TO_UNICODE) {
        cnv->toUnicodeStatus = 0;
        cnv->mode=0;
        अगर(cnv->extraInfo != nullptr){
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->isStateDBCS = false;
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->isEmptySegment = false;
        }
    }
    अगर(choice!=UCNV_RESET_TO_UNICODE) {
        cnv->fromUnicodeStatus= 0;
        cnv->fromUChar32=0x0000; 
        अगर(cnv->extraInfo != nullptr){
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->isEscapeAppended = false;
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->target_सूचकांक = 0;
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->source_सूचकांक = 0;
            ((UConverter_तथ्य_HZ*)cnv->extraInfo)->isTargetUCharDBCS = false;
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


अटल व्योम  U_CALLCONV
UConverter_toUnicode_HZ_OFFSETS_LOGIC(UConverterToUnicodeArgs *args,
                                                            UErrorCode* err){
    अक्षर अस्थायी_Buf[2];
    स्थिर अक्षर *mySource = ( अक्षर *) args->source;
    char16_t *myTarget = args->target;
    स्थिर अक्षर *mySourceLimit = args->sourceLimit;
    UChar32 targetUniChar = 0x0000;
    int32_t mySourceChar = 0x0000;
    UConverter_तथ्य_HZ* my_तथ्य=(UConverter_तथ्य_HZ*)(args->converter->extraInfo);
    अस्थायी_Buf[0]=0; 
    अस्थायी_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    जबतक(mySource< mySourceLimit){
        
        अगर(myTarget < args->targetLimit){
            
            mySourceChar= (अचिन्हित अक्षर) *mySource++;

            अगर(args->converter->mode == UCNV_TILDE) {
                /* second byte after ~ */
                args->converter->mode=0;
                चयन(mySourceChar) {
                हाल 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    जारी;
                हाल UCNV_TILDE:
                    अगर(args->offsets) {
                        args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    my_तथ्य->isEmptySegment = false;
                    जारी;
                हाल UCNV_OPEN_BRACE:
                हाल UCNV_CLOSE_BRACE:
                    my_तथ्य->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    अगर (my_तथ्य->isEmptySegment) {
                        my_तथ्य->isEmptySegment = false; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        args->converter->toUCallbackReason = UCNV_IRREGULAR;
                        args->converter->toUBytes[0] = UCNV_TILDE;
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                        args->target = myTarget;
                        args->source = mySource;
                        वापस;
                    }
                    my_तथ्य->isEmptySegment = true;
                    जारी;
                शेष:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_तथ्य->isEmptySegment = false; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    args->converter->toUBytes[0] = UCNV_TILDE;
                    अगर( my_तथ्य->isStateDBCS ?
                            (0x21 <= mySourceChar && mySourceChar <= 0x7e) :
                            mySourceChar <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        args->converter->toULength = 1;
                        --mySource;
                    } अन्यथा {
                        /* Include the current byte in the illegal sequence. */
                        args->converter->toUBytes[1] = static_cast<uint8_t>(mySourceChar);
                        args->converter->toULength = 2;
                    }
                    args->target = myTarget;
                    args->source = mySource;
                    वापस;
                }
            } अन्यथा अगर(my_तथ्य->isStateDBCS) {
                अगर(args->converter->toUnicodeStatus == 0x00){
                    /* lead byte */
                    अगर(mySourceChar == UCNV_TILDE) {
                        args->converter->mode = UCNV_TILDE;
                    } अन्यथा {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        args->converter->toUnicodeStatus = (uint32_t) (mySourceChar | 0x100);
                        my_तथ्य->isEmptySegment = false; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    जारी;
                }
                अन्यथा{
                    /* trail byte */
                    पूर्णांक leadIsOk, trailIsOk;
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
                    अगर (leadIsOk && trailIsOk) {
                        अस्थायी_Buf[0] = (अक्षर) (leadByte+0x80) ;
                        अस्थायी_Buf[1] = (अक्षर) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(my_तथ्य->gbConverter->shared_तथ्य,
                            अस्थायी_Buf, 2, args->converter->useFallback);
                        mySourceChar= (leadByte << 8) | mySourceChar;
                    } अन्यथा अगर (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --mySource;
                        mySourceChar = (int32_t)leadByte;
                    } अन्यथा {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar= 0x10000 | (leadByte << 8) | mySourceChar;
                    }
                    args->converter->toUnicodeStatus =0x00;
                }
            }
            अन्यथा{
                अगर(mySourceChar == UCNV_TILDE) {
                    args->converter->mode = UCNV_TILDE;
                    जारी;
                } अन्यथा अगर(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    my_तथ्य->isEmptySegment = false; /* the segment has something valid */
                } अन्यथा {
                    targetUniChar = 0xffff;
                    my_तथ्य->isEmptySegment = false; /* different error here, reset this to avoid spurious future error */
                }
            }
            अगर(targetUniChar < 0xfffe){
                अगर(args->offsets) {
                    args->offsets[myTarget - args->target]=(int32_t)(mySource - args->source - 1-(my_तथ्य->isStateDBCS));
                }

                *(myTarget++)=(char16_t)targetUniChar;
            }
            अन्यथा /* targetUniChar>=0xfffe */ {
                अगर(targetUniChar == 0xfffe){
                    *err = U_INVALID_CHAR_FOUND;
                }
                अन्यथा{
                    *err = U_ILLEGAL_CHAR_FOUND;
                }
                अगर(mySourceChar > 0xff){
                    args->converter->toUBytes[0] = (uint8_t)(mySourceChar >> 8);
                    args->converter->toUBytes[1] = (uint8_t)mySourceChar;
                    args->converter->toULength=2;
                }
                अन्यथा{
                    args->converter->toUBytes[0] = (uint8_t)mySourceChar;
                    args->converter->toULength=1;
                }
                अवरोध;
            }
        }
        अन्यथा{
            *err =U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
    }

    args->target = myTarget;
    args->source = mySource;
}


अटल व्योम  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverterFromUnicodeArgs * args,
                                                      UErrorCode * err){
    स्थिर char16_t *mySource = args->source;
    अक्षर *myTarget = args->target;
    int32_t* offsets = args->offsets;
    int32_t my_Source_सूचकांक = 0;
    int32_t my_Target_सूचकांक = 0;
    int32_t target_लंबाई = (int32_t)(args->targetLimit - myTarget);
    int32_t my_Source_लंबाई = (int32_t)(args->sourceLimit - args->source);
    uint32_t targetUniChar = 0x0000;
    UChar32 mySourceChar = 0x0000;
    UConverter_तथ्य_HZ *my_Converter_तथ्य=(UConverter_तथ्य_HZ*)args->converter->extraInfo;
    UBool isTargetUCharDBCS = my_Converter_तथ्य->isTargetUCharDBCS;
    UBool oldIsTargetUCharDBCS;
    पूर्णांक len =0;
    स्थिर अक्षर* escSeq=nullptr;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    अगर(args->converter->fromUChar32!=0 && my_Target_सूचकांक < target_लंबाई) {
        जाओ getTrail;
    }
    /*writing the char to the output stream */
    जबतक (my_Source_सूचकांक < my_Source_लंबाई){
        targetUniChar = missingCharMarker;
        अगर (my_Target_सूचकांक < target_लंबाई){
            
            mySourceChar = mySource[my_Source_सूचकांक++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            अगर(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(args, my_Target_सूचकांक, target_लंबाई, escSeq,err,len,my_Source_सूचकांक);
                जारी;
            } अन्यथा अगर(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } अन्यथा {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_तथ्य->gbConverter->shared_तथ्य,
                    mySourceChar,&targetUniChar,args->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                अगर( length == 2 &&
                    (uint16_t)(targetUniChar - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(targetUniChar - 0xa1) <= (0xfe - 0xa1)
                ) {
                    targetUniChar -= 0x8080;
                } अन्यथा {
                    targetUniChar = missingCharMarker;
                }
            }
            अगर (targetUniChar != missingCharMarker){
               my_Converter_तथ्य->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 अगर(oldIsTargetUCharDBCS != isTargetUCharDBCS || !my_Converter_तथ्य->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    अगर(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, my_Target_सूचकांक, target_लंबाई, escSeq,err,len,my_Source_सूचकांक);
                        my_Converter_तथ्य->isEscapeAppended = true;
                    }
                    अन्यथा{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(args, my_Target_सूचकांक, target_लंबाई, escSeq,err,len,my_Source_सूचकांक);
                        my_Converter_तथ्य->isEscapeAppended = true;
                        
                    }
                }
            
                अगर(isTargetUCharDBCS){
                    अगर( my_Target_सूचकांक <target_लंबाई){
                        myTarget[my_Target_सूचकांक++] =(अक्षर) (targetUniChar >> 8);
                        अगर(offsets){
                            *(offsets++) = my_Source_सूचकांक-1;
                        }
                        अगर(my_Target_सूचकांक < target_लंबाई){
                            myTarget[my_Target_सूचकांक++] =(अक्षर) targetUniChar;
                            अगर(offsets){
                                *(offsets++) = my_Source_सूचकांक-1;
                            }
                        }अन्यथा{
                            args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = (अक्षर) targetUniChar;
                            *err = U_बफर_OVERFLOW_ERROR;
                        } 
                    }अन्यथा{
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] =(अक्षर) (targetUniChar >> 8);
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = (अक्षर) targetUniChar;
                        *err = U_बफर_OVERFLOW_ERROR;
                    }

                }अन्यथा{
                    अगर( my_Target_सूचकांक <target_लंबाई){
                        myTarget[my_Target_सूचकांक++] = (अक्षर) (targetUniChar );
                        अगर(offsets){
                            *(offsets++) = my_Source_सूचकांक-1;
                        }
                        
                    }अन्यथा{
                        args->converter->char_Error_बफर[args->converter->char_Error_बफर_लंबाई++] = (अक्षर) targetUniChar;
                        *err = U_बफर_OVERFLOW_ERROR;
                    }
                }

            }
            अन्यथा{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                अगर(U16_IS_SURROGATE(mySourceChar)) {
                    अगर(U16_IS_SURROGATE_LEAD(mySourceChar)) {
                        args->converter->fromUChar32=mySourceChar;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        अगर(my_Source_सूचकांक <  my_Source_लंबाई) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) args->source[my_Source_सूचकांक];
                            अगर(U16_IS_TRAIL(trail)) {
                                ++my_Source_सूचकांक;
                                mySourceChar=U16_GET_SUPPLEMENTARY(args->converter->fromUChar32, trail);
                                args->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_FOUND;
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_FOUND;
                            }
                        } अन्यथा {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_FOUND;
                    }
                } अन्यथा {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_FOUND;
                }

                args->converter->fromUChar32=mySourceChar;
                अवरोध;
            }
        }
        अन्यथा{
            *err = U_बफर_OVERFLOW_ERROR;
            अवरोध;
        }
        targetUniChar=missingCharMarker;
    }

    args->target += my_Target_सूचकांक;
    args->source += my_Source_सूचकांक;
    my_Converter_तथ्य->isTargetUCharDBCS = isTargetUCharDBCS;
}

अटल व्योम U_CALLCONV
_HZ_WriteSub(UConverterFromUnicodeArgs *args, int32_t offset_सूचकांक, UErrorCode *err) {
    UConverter *cnv = args->converter;
    UConverter_तथ्य_HZ *conv_तथ्य=(UConverter_तथ्य_HZ *) cnv->extraInfo;
    अक्षर *p;
    अक्षर बफर[4];
    p = बफर;
    
    अगर( conv_तथ्य->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_तथ्य->isTargetUCharDBCS=false;
    }
    *p++= (अक्षर)cnv->subChars[0];

    ucnv_cbFromUWriteBytes(args,
                           बफर, (int32_t)(p - बफर),
                           offset_सूचकांक, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
काष्ठा cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_तथ्य_HZ mydata;
};


अटल UConverter *  U_CALLCONV
_HZ_SafeClone(स्थिर UConverter *cnv, 
              व्योम *ढेर_बफर, 
              int32_t *p_बफर_आकार, 
              UErrorCode *status)
{
    काष्ठा cloneHZStruct * localClone;
    int32_t size, बफर_आकार_Needed = माप(काष्ठा cloneHZStruct);

    अगर (U_FAILURE(*status)){
        वापस nullptr;
    }

    अगर (*p_बफर_आकार == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = बफर_आकार_Needed;
        वापस nullptr;
    }

    localClone = (काष्ठा cloneHZStruct *)ढेर_बफर;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, माप(UConverter_तथ्य_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = true;

    /* deep-clone the sub-converter */
    size = (int32_t)माप(UConverter);
    ((UConverter_तथ्य_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverter_तथ्य_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, status);

    वापस &localClone->cnv;
}

अटल व्योम U_CALLCONV
_HZ_GetUnicodeSet(स्थिर UConverter *cnv,
                  स्थिर USetAdder *sa,
                  UConverterUnicodeSet which,
                  UErrorCode *pErrorCode) {
    /* HZ converts all of ASCII */
    sa->addRange(sa->set, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGetFilteredUnicodeSetForUnicode(
        ((UConverter_तथ्य_HZ*)cnv->extraInfo)->gbConverter->shared_तथ्य,
        sa, which, UCNV_SET_FILTER_HZ,
        pErrorCode);
}
U_CDECL_END
अटल स्थिर UConverterImpl _HZImpl={

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

अटल स्थिर UConverter_Static_तथ्य HZStatic_तथ्य={
    माप(UConverter_Static_तथ्य),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        false, 
        false,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

स्थिर UConverter_Shared_तथ्य _HZData=
        UCNV_IMMUTABLE_SHARED_तथ्य_INITIALIZER(&HZStatic_तथ्य, &_HZImpl);

#पूर्ण_अगर /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
