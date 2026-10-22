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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/uset.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "ucnv_imp.h"

#परिभाषय UCNV_TILDE 0x7E          /* ~ */
#परिभाषय UCNV_OPEN_BRACE 0x7B     /* { */
#परिभाषय UCNV_CLOSE_BRACE 0x7D   /* } */
#परिभाषय SB_ESCAPE    "\x7E\x7D"
#परिभाषय DB_ESCAPE    "\x7E\x7B"
#परिभाषय TILDE_ESCAPE "\x7E\x7E"
#परिभाषय ESC_LEN       2


#परिभाषय CONCAT_ESCAPE_MACRO(तर्काः_2, target_सूचकांक,target_दीर्घता,str_गन्तव्यम्_Append, err, len,source_सूचकांक) UPRV_खण्डम्_MACRO_BEGIN {      \
    यावत्(len-->0){                                                                                                         \
        यदि(target_सूचकांक < target_दीर्घता){                                                                                     \
            तर्काः_2->target[target_सूचकांक] = (अचिह्नित वर्ण) *str_गन्तव्यम्_Append;                                                       \
            यदि(तर्काः_2->offsets!=शून्यसूचकः){                                                                                        \
                *(offsets++) = source_सूचकांक-1;                                                                               \
            }                                                                                                               \
            target_सूचकांक++;                                                                                                  \
        }                                                                                                                   \
        अन्यथा{                                                                                                               \
            तर्काः_2->converter->char_Error_बफर[(पूर्णाङ्क)तर्काः_2->converter->char_Error_बफर_दीर्घता++] = (अचिह्नित वर्ण) *str_गन्तव्यम्_Append; \
            *err =U_बफर_अतिप्रवाहः_ERROR;                                                                                  \
        }                                                                                                                   \
        str_गन्तव्यम्_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_खण्डम्_MACRO_अन्तः


प्रकारनिर्देश संरचना{
    UConverter* gbConverter;
    int32_t target_सूचकांक;
    int32_t source_सूचकांक;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool isTargetUCharDBCS;
    UBool is_रिक्तानि_Segment;
}UConverter_दत्त_HZ;


U_CDECL_BEGIN
स्थिर शून्य  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_आरोपय_तर्काः *p_तर्काः, UError_संहिता *error_संहिता){
    UConverter *gbConverter;
    यदि(p_तर्काः->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_संहिता);  /* errorCode carries result */
        प्रत्यागम;
    }
    gbConverter = ucnv_open("GBK", error_संहिता);
    यदि(U_FAILURE(*error_संहिता)) {
        प्रत्यागम;
    }
    cnv->गन्तव्यम्_Unicode_स्थितिः = 0;
    cnv->from_Unicode_स्थितिः= 0;
    cnv->बहुलकम्=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, परिमाण(UConverter_दत्त_HZ));
    यदि(cnv->extraInfo != शून्यसूचकः){
        ((UConverter_दत्त_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    अन्यथा {
        ucnv_close(gbConverter);
        *error_संहिता = U_स्मृतिः_आवण्टनम्_ERROR;
        प्रत्यागम;
    }
}

स्थिर शून्य  U_CALLCONV
_HZClose(UConverter *cnv){
    यदि(cnv->extraInfo != शून्यसूचकः) {
        ucnv_close (((UConverter_दत्त_HZ *) (cnv->extraInfo))->gbConverter);
        यदि(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = शून्यसूचकः;
    }
}

स्थिर शून्य  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        cnv->गन्तव्यम्_Unicode_स्थितिः = 0;
        cnv->बहुलकम्=0;
        यदि(cnv->extraInfo != शून्यसूचकः){
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->isStateDBCS = असत्य;
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->is_रिक्तानि_Segment = असत्य;
        }
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        cnv->from_Unicode_स्थितिः= 0;
        cnv->fromUChar32=0x0000; 
        यदि(cnv->extraInfo != शून्यसूचकः){
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->isEscapeAppended = असत्य;
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->target_सूचकांक = 0;
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->source_सूचकांक = 0;
            ((UConverter_दत्त_HZ*)cnv->extraInfo)->isTargetUCharDBCS = असत्य;
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


स्थिर शून्य  U_CALLCONV
UConverter_गन्तव्यम्_Unicode_HZ_OFFSETS_LOGIC(UConverter_गन्तव्यम्_Unicode_तर्काः *तर्काः_2,
                                                            UError_संहिता* err){
    वर्ण अस्थायी_Buf[2];
    ध्रुव वर्ण *mySource = ( वर्ण *) तर्काः_2->source;
    char16_t *myTarget = तर्काः_2->target;
    ध्रुव वर्ण *my_Source_सीमा = तर्काः_2->source_सीमा;
    UChar32 targetUniChar = 0x0000;
    int32_t mySourceChar = 0x0000;
    UConverter_दत्त_HZ* my_दत्त=(UConverter_दत्त_HZ*)(तर्काः_2->converter->extraInfo);
    अस्थायी_Buf[0]=0; 
    अस्थायी_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    यावत्(mySource< my_Source_सीमा){
        
        यदि(myTarget < तर्काः_2->target_सीमा){
            
            mySourceChar= (अचिह्नित वर्ण) *mySource++;

            यदि(तर्काः_2->converter->बहुलकम् == UCNV_TILDE) {
                /* second byte after ~ */
                तर्काः_2->converter->बहुलकम्=0;
                विकल्प(mySourceChar) {
                पक्ष 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    अनुवर्त;
                पक्ष UCNV_TILDE:
                    यदि(तर्काः_2->offsets) {
                        तर्काः_2->offsets[myTarget - तर्काः_2->target]=(int32_t)(mySource - तर्काः_2->source - 2);
                    }
                    *(myTarget++)=(char16_t)mySourceChar;
                    my_दत्त->is_रिक्तानि_Segment = असत्य;
                    अनुवर्त;
                पक्ष UCNV_OPEN_BRACE:
                पक्ष UCNV_CLOSE_BRACE:
                    my_दत्त->isStateDBCS = (mySourceChar == UCNV_OPEN_BRACE);
                    यदि (my_दत्त->is_रिक्तानि_Segment) {
                        my_दत्त->is_रिक्तानि_Segment = असत्य; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        तर्काः_2->converter->गन्तव्यम्_UCallback_Reason = UCNV_IRREGULAR;
                        तर्काः_2->converter->गन्तव्यम्_UBytes[0] = UCNV_TILDE;
                        तर्काः_2->converter->गन्तव्यम्_UBytes[1] = static_cast<uint8_t>(mySourceChar);
                        तर्काः_2->converter->गन्तव्यम्_ULength = 2;
                        तर्काः_2->target = myTarget;
                        तर्काः_2->source = mySource;
                        प्रत्यागम;
                    }
                    my_दत्त->is_रिक्तानि_Segment = सत्य;
                    अनुवर्त;
                स्वभाव:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_दत्त->is_रिक्तानि_Segment = असत्य; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = UCNV_TILDE;
                    यदि( my_दत्त->isStateDBCS ?
                            (0x21 <= mySourceChar && mySourceChar <= 0x7e) :
                            mySourceChar <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        तर्काः_2->converter->गन्तव्यम्_ULength = 1;
                        --mySource;
                    } अन्यथा {
                        /* Include the current byte in the illegal sequence. */
                        तर्काः_2->converter->गन्तव्यम्_UBytes[1] = static_cast<uint8_t>(mySourceChar);
                        तर्काः_2->converter->गन्तव्यम्_ULength = 2;
                    }
                    तर्काः_2->target = myTarget;
                    तर्काः_2->source = mySource;
                    प्रत्यागम;
                }
            } अन्यथा यदि(my_दत्त->isStateDBCS) {
                यदि(तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः == 0x00){
                    /* lead byte */
                    यदि(mySourceChar == UCNV_TILDE) {
                        तर्काः_2->converter->बहुलकम् = UCNV_TILDE;
                    } अन्यथा {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः = (uint32_t) (mySourceChar | 0x100);
                        my_दत्त->is_रिक्तानि_Segment = असत्य; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    अनुवर्त;
                }
                अन्यथा{
                    /* trail byte */
                    पूर्णाङ्क leadIsOk, trailIsOk;
                    uint32_t leadByte = तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः & 0xff;
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
                    यदि (leadIsOk && trailIsOk) {
                        अस्थायी_Buf[0] = (वर्ण) (leadByte+0x80) ;
                        अस्थायी_Buf[1] = (वर्ण) (mySourceChar+0x80);
                        targetUniChar = ucnv_MBCSSimple_Get_अग्र_UChar(my_दत्त->gbConverter->shared_दत्त,
                            अस्थायी_Buf, 2, तर्काः_2->converter->useFallback);
                        mySourceChar= (leadByte << 8) | mySourceChar;
                    } अन्यथा यदि (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --mySource;
                        mySourceChar = (int32_t)leadByte;
                    } अन्यथा {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        mySourceChar= 0x10000 | (leadByte << 8) | mySourceChar;
                    }
                    तर्काः_2->converter->गन्तव्यम्_Unicode_स्थितिः =0x00;
                }
            }
            अन्यथा{
                यदि(mySourceChar == UCNV_TILDE) {
                    तर्काः_2->converter->बहुलकम् = UCNV_TILDE;
                    अनुवर्त;
                } अन्यथा यदि(mySourceChar <= 0x7f) {
                    targetUniChar = (char16_t)mySourceChar;  /* ASCII */
                    my_दत्त->is_रिक्तानि_Segment = असत्य; /* the segment has something valid */
                } अन्यथा {
                    targetUniChar = 0xffff;
                    my_दत्त->is_रिक्तानि_Segment = असत्य; /* different error here, reset this to avoid spurious future error */
                }
            }
            यदि(targetUniChar < 0xfffe){
                यदि(तर्काः_2->offsets) {
                    तर्काः_2->offsets[myTarget - तर्काः_2->target]=(int32_t)(mySource - तर्काः_2->source - 1-(my_दत्त->isStateDBCS));
                }

                *(myTarget++)=(char16_t)targetUniChar;
            }
            अन्यथा /* targetUniChar>=0xfffe */ {
                यदि(targetUniChar == 0xfffe){
                    *err = U_INVALID_CHAR_प्राप्तम्;
                }
                अन्यथा{
                    *err = U_ILLEGAL_CHAR_प्राप्तम्;
                }
                यदि(mySourceChar > 0xff){
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = (uint8_t)(mySourceChar >> 8);
                    तर्काः_2->converter->गन्तव्यम्_UBytes[1] = (uint8_t)mySourceChar;
                    तर्काः_2->converter->गन्तव्यम्_ULength=2;
                }
                अन्यथा{
                    तर्काः_2->converter->गन्तव्यम्_UBytes[0] = (uint8_t)mySourceChar;
                    तर्काः_2->converter->गन्तव्यम्_ULength=1;
                }
                भङ्ग;
            }
        }
        अन्यथा{
            *err =U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
    }

    तर्काः_2->target = myTarget;
    तर्काः_2->source = mySource;
}


स्थिर शून्य  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_तर्काः * तर्काः_2,
                                                      UError_संहिता * err){
    ध्रुव char16_t *mySource = तर्काः_2->source;
    वर्ण *myTarget = तर्काः_2->target;
    int32_t* offsets = तर्काः_2->offsets;
    int32_t my_Source_सूचकांक = 0;
    int32_t my_Target_सूचकांक = 0;
    int32_t target_दीर्घता = (int32_t)(तर्काः_2->target_सीमा - myTarget);
    int32_t my_Source_दीर्घता = (int32_t)(तर्काः_2->source_सीमा - तर्काः_2->source);
    uint32_t targetUniChar = 0x0000;
    UChar32 mySourceChar = 0x0000;
    UConverter_दत्त_HZ *my_Converter_दत्त=(UConverter_दत्त_HZ*)तर्काः_2->converter->extraInfo;
    UBool isTargetUCharDBCS = my_Converter_दत्त->isTargetUCharDBCS;
    UBool oldIsTargetUCharDBCS;
    पूर्णाङ्क len =0;
    ध्रुव वर्ण* escSeq=शून्यसूचकः;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    यदि(तर्काः_2->converter->fromUChar32!=0 && my_Target_सूचकांक < target_दीर्घता) {
        गच्छ getTrail;
    }
    /*writing the char to the output stream */
    यावत् (my_Source_सूचकांक < my_Source_दीर्घता){
        targetUniChar = missingCharMarker;
        यदि (my_Target_सूचकांक < target_दीर्घता){
            
            mySourceChar = mySource[my_Source_सूचकांक++];
            

            oldIsTargetUCharDBCS = isTargetUCharDBCS;
            यदि(mySourceChar ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(तर्काः_2, my_Target_सूचकांक, target_दीर्घता, escSeq,err,len,my_Source_सूचकांक);
                अनुवर्त;
            } अन्यथा यदि(mySourceChar <= 0x7f) {
                targetUniChar = mySourceChar;
            } अन्यथा {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_दत्त->gbConverter->shared_दत्त,
                    mySourceChar,&targetUniChar,तर्काः_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                यदि( length == 2 &&
                    (uint16_t)(targetUniChar - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(targetUniChar - 0xa1) <= (0xfe - 0xa1)
                ) {
                    targetUniChar -= 0x8080;
                } अन्यथा {
                    targetUniChar = missingCharMarker;
                }
            }
            यदि (targetUniChar != missingCharMarker){
               my_Converter_दत्त->isTargetUCharDBCS = isTargetUCharDBCS = targetUniChar > 0x00FF;
                 यदि(oldIsTargetUCharDBCS != isTargetUCharDBCS || !my_Converter_दत्त->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    यदि(!isTargetUCharDBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(तर्काः_2, my_Target_सूचकांक, target_दीर्घता, escSeq,err,len,my_Source_सूचकांक);
                        my_Converter_दत्त->isEscapeAppended = सत्य;
                    }
                    अन्यथा{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(तर्काः_2, my_Target_सूचकांक, target_दीर्घता, escSeq,err,len,my_Source_सूचकांक);
                        my_Converter_दत्त->isEscapeAppended = सत्य;
                        
                    }
                }
            
                यदि(isTargetUCharDBCS){
                    यदि( my_Target_सूचकांक <target_दीर्घता){
                        myTarget[my_Target_सूचकांक++] =(वर्ण) (targetUniChar >> 8);
                        यदि(offsets){
                            *(offsets++) = my_Source_सूचकांक-1;
                        }
                        यदि(my_Target_सूचकांक < target_दीर्घता){
                            myTarget[my_Target_सूचकांक++] =(वर्ण) targetUniChar;
                            यदि(offsets){
                                *(offsets++) = my_Source_सूचकांक-1;
                            }
                        }अन्यथा{
                            तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = (वर्ण) targetUniChar;
                            *err = U_बफर_अतिप्रवाहः_ERROR;
                        } 
                    }अन्यथा{
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] =(वर्ण) (targetUniChar >> 8);
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = (वर्ण) targetUniChar;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }

                }अन्यथा{
                    यदि( my_Target_सूचकांक <target_दीर्घता){
                        myTarget[my_Target_सूचकांक++] = (वर्ण) (targetUniChar );
                        यदि(offsets){
                            *(offsets++) = my_Source_सूचकांक-1;
                        }
                        
                    }अन्यथा{
                        तर्काः_2->converter->char_Error_बफर[तर्काः_2->converter->char_Error_बफर_दीर्घता++] = (वर्ण) targetUniChar;
                        *err = U_बफर_अतिप्रवाहः_ERROR;
                    }
                }

            }
            अन्यथा{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                यदि(U16_IS_SURROGATE(mySourceChar)) {
                    यदि(U16_IS_SURROGATE_LEAD(mySourceChar)) {
                        तर्काः_2->converter->fromUChar32=mySourceChar;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        यदि(my_Source_सूचकांक <  my_Source_दीर्घता) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) तर्काः_2->source[my_Source_सूचकांक];
                            यदि(U16_IS_TRAIL(trail)) {
                                ++my_Source_सूचकांक;
                                mySourceChar=U16_GET_SUPPLEMENTARY(तर्काः_2->converter->fromUChar32, trail);
                                तर्काः_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_प्राप्तम्;
                                /* exit this condition tree */
                            } अन्यथा {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_प्राप्तम्;
                            }
                        } अन्यथा {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } अन्यथा {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_प्राप्तम्;
                    }
                } अन्यथा {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_प्राप्तम्;
                }

                तर्काः_2->converter->fromUChar32=mySourceChar;
                भङ्ग;
            }
        }
        अन्यथा{
            *err = U_बफर_अतिप्रवाहः_ERROR;
            भङ्ग;
        }
        targetUniChar=missingCharMarker;
    }

    तर्काः_2->target += my_Target_सूचकांक;
    तर्काः_2->source += my_Source_सूचकांक;
    my_Converter_दत्त->isTargetUCharDBCS = isTargetUCharDBCS;
}

स्थिर शून्य U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_तर्काः *तर्काः_2, int32_t विस्थापनम्_सूचकांक, UError_संहिता *err) {
    UConverter *cnv = तर्काः_2->converter;
    UConverter_दत्त_HZ *conv_दत्त=(UConverter_दत्त_HZ *) cnv->extraInfo;
    वर्ण *p;
    वर्ण बफर[4];
    p = बफर;
    
    यदि( conv_दत्त->isTargetUCharDBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_दत्त->isTargetUCharDBCS=असत्य;
    }
    *p++= (वर्ण)cnv->subChars[0];

    ucnv_cb_From_UWrite_अष्टकानि(तर्काः_2,
                           बफर, (int32_t)(p - बफर),
                           विस्थापनम्_सूचकांक, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
संरचना cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_दत्त_HZ mydata;
};


स्थिर UConverter *  U_CALLCONV
_HZ_SafeClone(ध्रुव UConverter *cnv, 
              शून्य *राशिः_बफर, 
              int32_t *p_बफर_आकार, 
              UError_संहिता *स्थितिः)
{
    संरचना cloneHZStruct * localClone;
    int32_t size, बफर_आकार_Needed = परिमाण(संरचना cloneHZStruct);

    यदि (U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    यदि (*p_बफर_आकार == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = बफर_आकार_Needed;
        प्रत्यागम शून्यसूचकः;
    }

    localClone = (संरचना cloneHZStruct *)राशिः_बफर;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, परिमाण(UConverter_दत्त_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = सत्य;

    /* deep-clone the sub-converter */
    size = (int32_t)परिमाण(UConverter);
    ((UConverter_दत्त_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_safeClone(((UConverter_दत्त_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, स्थितिः);

    प्रत्यागम &localClone->cnv;
}

स्थिर शून्य U_CALLCONV
HZ_Get_Unicode_समुच्चयः(ध्रुव UConverter *cnv,
                  ध्रुव USetAdder *sa,
                  UConverter_Unicode_समुच्चयः which,
                  UError_संहिता *p_Error_संहिता) {
    /* HZ converts all of ASCII */
    sa->सङ्कलय_Range(sa->समुच्चयः, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_समुच्चयः_For_Unicode(
        ((UConverter_दत्त_HZ*)cnv->extraInfo)->gbConverter->shared_दत्त,
        sa, which, UCNV_समुच्चयः_FILTER_HZ,
        p_Error_संहिता);
}
U_CDECL_अन्तः
स्थिर ध्रुव UConverterImpl _HZImpl={

    UCNV_HZ,
    
    शून्यसूचकः,
    शून्यसूचकः,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_गन्तव्यम्_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_गन्तव्यम्_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    शून्यसूचकः,
    
    शून्यसूचकः,
    शून्यसूचकः,
    _HZ_WriteSub,
    _HZ_SafeClone,
    HZ_Get_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त HZStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        असत्य, 
        असत्य,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ध्रुव UConverter_Shared_दत्त _HZData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&HZStatic_दत्त, &_HZImpl);

#यदि_अन्तः /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
