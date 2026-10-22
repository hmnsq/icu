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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/uset.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "ucnv_imp.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_TILDE 0x7E          /* ~ */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_OPEN_BRACE 0x7B     /* { */
#ವ್ಯಾಖ್ಯಾನಿಸು UCNV_CLOSE_BRACE 0x7D   /* } */
#ವ್ಯಾಖ್ಯಾನಿಸು SB_ESCAPE    "\x7E\x7D"
#ವ್ಯಾಖ್ಯಾನಿಸು DB_ESCAPE    "\x7E\x7B"
#ವ್ಯಾಖ್ಯಾನಿಸು TILDE_ESCAPE "\x7E\x7E"
#ವ್ಯಾಖ್ಯಾನಿಸು ESC_LEN       2


#ವ್ಯಾಖ್ಯಾನಿಸು CONCAT_ESCAPE_MACRO(ವಾದಗಳು_2, ಗುರಿ_ಸೂಚ್ಯಂಕ,ಗುರಿ_ಉದ್ದಳತೆ,str_ಗಮ್ಯ_Append, err, len,ಮೂಲ_ಸೂಚ್ಯಂಕ) UPRV_ಖಂಡ_MACRO_BEGIN {      \
    ಆಗ(len-->0){                                                                                                         \
        ಆದರೆ(ಗುರಿ_ಸೂಚ್ಯಂಕ < ಗುರಿ_ಉದ್ದಳತೆ){                                                                                     \
            ವಾದಗಳು_2->ಗುರಿ[ಗುರಿ_ಸೂಚ್ಯಂಕ] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) *str_ಗಮ್ಯ_Append;                                                       \
            ಆದರೆ(ವಾದಗಳು_2->offsets!=ಶೂನ್ಯಸೂಚಕ){                                                                                        \
                *(offsets++) = ಮೂಲ_ಸೂಚ್ಯಂಕ-1;                                                                               \
            }                                                                                                               \
            ಗುರಿ_ಸೂಚ್ಯಂಕ++;                                                                                                  \
        }                                                                                                                   \
        ಇಲ್ಲದಿದ್ದರೆ{                                                                                                               \
            ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[(ಪೂರ್ಣಾಂಕ)ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) *str_ಗಮ್ಯ_Append; \
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;                                                                                  \
        }                                                                                                                   \
        str_ಗಮ್ಯ_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_ಖಂಡ_MACRO_ಕೊನೆ


ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ{
    UConverter* gbConverter;
    int32_t ಗುರಿ_ಸೂಚ್ಯಂಕ;
    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_ಗುರಿ_UChar_DBCS;
    UBool is_ಖಾಲಿಗಳು_Segment;
}UConverter_ದತ್ತಾಂಶ_HZ;


U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು, UError_ಸಂಕೇತ *error_ಸಂಕೇತ){
    UConverter *gbConverter;
    ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_ಸಂಕೇತ);  /* errorCode carries result */
        ಹಿಂತಿರುಗಿ;
    }
    gbConverter = ucnv_open("GBK", error_ಸಂಕೇತ);
    ಆದರೆ(U_FAILURE(*error_ಸಂಕೇತ)) {
        ಹಿಂತಿರುಗಿ;
    }
    cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
    cnv->from_Unicode_ಸ್ಥಿತಿ= 0;
    cnv->ಬಹುಲಕ=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_HZ));
    ಆದರೆ(cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ){
        ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
        ucnv_close(gbConverter);
        *error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
        ಹಿಂತಿರುಗಿ;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_HZClose(UConverter *cnv){
    ಆದರೆ(cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ) {
        ucnv_close (((UConverter_ದತ್ತಾಂಶ_HZ *) (cnv->extraInfo))->gbConverter);
        ಆದರೆ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = ಶೂನ್ಯಸೂಚಕ;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        cnv->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = 0;
        cnv->ಬಹುಲಕ=0;
        ಆದರೆ(cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ){
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->isStateDBCS = ಸುಳ್ಳು;
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;
        }
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        cnv->from_Unicode_ಸ್ಥಿತಿ= 0;
        cnv->fromUChar32=0x0000; 
        ಆದರೆ(cnv->extraInfo != ಶೂನ್ಯಸೂಚಕ){
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->isEscapeAppended = ಸುಳ್ಳು;
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->ಗುರಿ_ಸೂಚ್ಯಂಕ = 0;
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->ಮೂಲ_ಸೂಚ್ಯಂಕ = 0;
            ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->is_ಗುರಿ_UChar_DBCS = ಸುಳ್ಳು;
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


ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
UConverter_ಗಮ್ಯ_Unicode_HZ_OFFSETS_LOGIC(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *ವಾದಗಳು_2,
                                                            UError_ಸಂಕೇತ* err){
    ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ_Buf[2];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ = ( ಅಕ್ಷರ *) ವಾದಗಳು_2->ಮೂಲ;
    char16_t *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *my_ಮೂಲ_ಎಲ್ಲೆ = ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ;
    UChar32 ಗುರಿ_Uni_Char = 0x0000;
    int32_t my_ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_HZ* my_ದತ್ತಾಂಶ=(UConverter_ದತ್ತಾಂಶ_HZ*)(ವಾದಗಳು_2->converter->extraInfo);
    ತಾತ್ಕಾಲಿಕ_Buf[0]=0; 
    ತಾತ್ಕಾಲಿಕ_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    ಆಗ(my_ಮೂಲ< my_ಮೂಲ_ಎಲ್ಲೆ){
        
        ಆದರೆ(my_ಗುರಿ < ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ){
            
            my_ಮೂಲ_Char= (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) *my_ಮೂಲ++;

            ಆದರೆ(ವಾದಗಳು_2->converter->ಬಹುಲಕ == UCNV_TILDE) {
                /* second byte after ~ */
                ವಾದಗಳು_2->converter->ಬಹುಲಕ=0;
                ಆಯ್ಕೆ(my_ಮೂಲ_Char) {
                ಸ್ಥಿತಿ 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    ಮುಂದುವರಿ;
                ಸ್ಥಿತಿ UCNV_TILDE:
                    ಆದರೆ(ವಾದಗಳು_2->offsets) {
                        ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ]=(int32_t)(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 2);
                    }
                    *(my_ಗುರಿ++)=(char16_t)my_ಮೂಲ_Char;
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು;
                    ಮುಂದುವರಿ;
                ಸ್ಥಿತಿ UCNV_OPEN_BRACE:
                ಸ್ಥಿತಿ UCNV_CLOSE_BRACE:
                    my_ದತ್ತಾಂಶ->isStateDBCS = (my_ಮೂಲ_Char == UCNV_OPEN_BRACE);
                    ಆದರೆ (my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment) {
                        my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UCallback_Reason = UCNV_IRREGULAR;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = UCNV_TILDE;
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[1] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 2;
                        ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                        ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                        ಹಿಂತಿರುಗಿ;
                    }
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ನಿಜ;
                    ಮುಂದುವರಿ;
                ಪೂರ್ವನಿಯೋಜಿತ:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = UCNV_TILDE;
                    ಆದರೆ( my_ದತ್ತಾಂಶ->isStateDBCS ?
                            (0x21 <= my_ಮೂಲ_Char && my_ಮೂಲ_Char <= 0x7e) :
                            my_ಮೂಲ_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 1;
                        --my_ಮೂಲ;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Include the current byte in the illegal sequence. */
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[1] = static_cast<uint8_t>(my_ಮೂಲ_Char);
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength = 2;
                    }
                    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
                    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
                    ಹಿಂತಿರುಗಿ;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ದತ್ತಾಂಶ->isStateDBCS) {
                ಆದರೆ(ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ == 0x00){
                    /* lead byte */
                    ಆದರೆ(my_ಮೂಲ_Char == UCNV_TILDE) {
                        ವಾದಗಳು_2->converter->ಬಹುಲಕ = UCNV_TILDE;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ = (uint32_t) (my_ಮೂಲ_Char | 0x100);
                        my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    ಮುಂದುವರಿ;
                }
                ಇಲ್ಲದಿದ್ದರೆ{
                    /* trail byte */
                    ಪೂರ್ಣಾಂಕ leadIsOk, trailIsOk;
                    uint32_t leadByte = ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ & 0xff;
                    ಗುರಿ_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_ಮೂಲ_Char - 0x21) <= (0x7e - 0x21);
                    ಆದರೆ (leadIsOk && trailIsOk) {
                        ತಾತ್ಕಾಲಿಕ_Buf[0] = (ಅಕ್ಷರ) (leadByte+0x80) ;
                        ತಾತ್ಕಾಲಿಕ_Buf[1] = (ಅಕ್ಷರ) (my_ಮೂಲ_Char+0x80);
                        ಗುರಿ_Uni_Char = ucnv_MBCSSimple_Get_ಮುಂದಿನ_UChar(my_ದತ್ತಾಂಶ->gbConverter->shared_ದತ್ತಾಂಶ,
                            ತಾತ್ಕಾಲಿಕ_Buf, 2, ವಾದಗಳು_2->converter->useFallback);
                        my_ಮೂಲ_Char= (leadByte << 8) | my_ಮೂಲ_Char;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_ಮೂಲ;
                        my_ಮೂಲ_Char = (int32_t)leadByte;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ಮೂಲ_Char= 0x10000 | (leadByte << 8) | my_ಮೂಲ_Char;
                    }
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_Unicode_ಸ್ಥಿತಿ =0x00;
                }
            }
            ಇಲ್ಲದಿದ್ದರೆ{
                ಆದರೆ(my_ಮೂಲ_Char == UCNV_TILDE) {
                    ವಾದಗಳು_2->converter->ಬಹುಲಕ = UCNV_TILDE;
                    ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                    ಗುರಿ_Uni_Char = (char16_t)my_ಮೂಲ_Char;  /* ASCII */
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು; /* the segment has something valid */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗುರಿ_Uni_Char = 0xffff;
                    my_ದತ್ತಾಂಶ->is_ಖಾಲಿಗಳು_Segment = ಸುಳ್ಳು; /* different error here, reset this to avoid spurious future error */
                }
            }
            ಆದರೆ(ಗುರಿ_Uni_Char < 0xfffe){
                ಆದರೆ(ವಾದಗಳು_2->offsets) {
                    ವಾದಗಳು_2->offsets[my_ಗುರಿ - ವಾದಗಳು_2->ಗುರಿ]=(int32_t)(my_ಮೂಲ - ವಾದಗಳು_2->ಮೂಲ - 1-(my_ದತ್ತಾಂಶ->isStateDBCS));
                }

                *(my_ಗುರಿ++)=(char16_t)ಗುರಿ_Uni_Char;
            }
            ಇಲ್ಲದಿದ್ದರೆ /* targetUniChar>=0xfffe */ {
                ಆದರೆ(ಗುರಿ_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                }
                ಇಲ್ಲದಿದ್ದರೆ{
                    *err = U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                }
                ಆದರೆ(my_ಮೂಲ_Char > 0xff){
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = (uint8_t)(my_ಮೂಲ_Char >> 8);
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[1] = (uint8_t)my_ಮೂಲ_Char;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength=2;
                }
                ಇಲ್ಲದಿದ್ದರೆ{
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_UBytes[0] = (uint8_t)my_ಮೂಲ_Char;
                    ವಾದಗಳು_2->converter->ಗಮ್ಯ_ULength=1;
                }
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ{
            *err =U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
    }

    ವಾದಗಳು_2->ಗುರಿ = my_ಗುರಿ;
    ವಾದಗಳು_2->ಮೂಲ = my_ಮೂಲ;
}


ಸ್ಥಿರ ಶೂನ್ಯ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_ವಾದಗಳು * ವಾದಗಳು_2,
                                                      UError_ಸಂಕೇತ * err){
    ಸ್ಥಿರಾಂಕ char16_t *my_ಮೂಲ = ವಾದಗಳು_2->ಮೂಲ;
    ಅಕ್ಷರ *my_ಗುರಿ = ವಾದಗಳು_2->ಗುರಿ;
    int32_t* offsets = ವಾದಗಳು_2->offsets;
    int32_t my_ಮೂಲ_ಸೂಚ್ಯಂಕ = 0;
    int32_t my_ಗುರಿ_ಸೂಚ್ಯಂಕ = 0;
    int32_t ಗುರಿ_ಉದ್ದಳತೆ = (int32_t)(ವಾದಗಳು_2->ಗುರಿ_ಎಲ್ಲೆ - my_ಗುರಿ);
    int32_t my_ಮೂಲ_ಉದ್ದಳತೆ = (int32_t)(ವಾದಗಳು_2->ಮೂಲ_ಎಲ್ಲೆ - ವಾದಗಳು_2->ಮೂಲ);
    uint32_t ಗುರಿ_Uni_Char = 0x0000;
    UChar32 my_ಮೂಲ_Char = 0x0000;
    UConverter_ದತ್ತಾಂಶ_HZ *my_Converter_ದತ್ತಾಂಶ=(UConverter_ದತ್ತಾಂಶ_HZ*)ವಾದಗಳು_2->converter->extraInfo;
    UBool is_ಗುರಿ_UChar_DBCS = my_Converter_ದತ್ತಾಂಶ->is_ಗುರಿ_UChar_DBCS;
    UBool old_Is_ಗುರಿ_UChar_DBCS;
    ಪೂರ್ಣಾಂಕ len =0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* escSeq=ಶೂನ್ಯಸೂಚಕ;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    ಆದರೆ(ವಾದಗಳು_2->converter->fromUChar32!=0 && my_ಗುರಿ_ಸೂಚ್ಯಂಕ < ಗುರಿ_ಉದ್ದಳತೆ) {
        ಹೋಗು getTrail;
    }
    /*writing the char to the output stream */
    ಆಗ (my_ಮೂಲ_ಸೂಚ್ಯಂಕ < my_ಮೂಲ_ಉದ್ದಳತೆ){
        ಗುರಿ_Uni_Char = missingCharMarker;
        ಆದರೆ (my_ಗುರಿ_ಸೂಚ್ಯಂಕ < ಗುರಿ_ಉದ್ದಳತೆ){
            
            my_ಮೂಲ_Char = my_ಮೂಲ[my_ಮೂಲ_ಸೂಚ್ಯಂಕ++];
            

            old_Is_ಗುರಿ_UChar_DBCS = is_ಗುರಿ_UChar_DBCS;
            ಆದರೆ(my_ಮೂಲ_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(ವಾದಗಳು_2, my_ಗುರಿ_ಸೂಚ್ಯಂಕ, ಗುರಿ_ಉದ್ದಳತೆ, escSeq,err,len,my_ಮೂಲ_ಸೂಚ್ಯಂಕ);
                ಮುಂದುವರಿ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(my_ಮೂಲ_Char <= 0x7f) {
                ಗುರಿ_Uni_Char = my_ಮೂಲ_Char;
            } ಇಲ್ಲದಿದ್ದರೆ {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_ದತ್ತಾಂಶ->gbConverter->shared_ದತ್ತಾಂಶ,
                    my_ಮೂಲ_Char,&ಗುರಿ_Uni_Char,ವಾದಗಳು_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                ಆದರೆ( length == 2 &&
                    (uint16_t)(ಗುರಿ_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(ಗುರಿ_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    ಗುರಿ_Uni_Char -= 0x8080;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗುರಿ_Uni_Char = missingCharMarker;
                }
            }
            ಆದರೆ (ಗುರಿ_Uni_Char != missingCharMarker){
               my_Converter_ದತ್ತಾಂಶ->is_ಗುರಿ_UChar_DBCS = is_ಗುರಿ_UChar_DBCS = ಗುರಿ_Uni_Char > 0x00FF;
                 ಆದರೆ(old_Is_ಗುರಿ_UChar_DBCS != is_ಗುರಿ_UChar_DBCS || !my_Converter_ದತ್ತಾಂಶ->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    ಆದರೆ(!is_ಗುರಿ_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(ವಾದಗಳು_2, my_ಗುರಿ_ಸೂಚ್ಯಂಕ, ಗುರಿ_ಉದ್ದಳತೆ, escSeq,err,len,my_ಮೂಲ_ಸೂಚ್ಯಂಕ);
                        my_Converter_ದತ್ತಾಂಶ->isEscapeAppended = ನಿಜ;
                    }
                    ಇಲ್ಲದಿದ್ದರೆ{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(ವಾದಗಳು_2, my_ಗುರಿ_ಸೂಚ್ಯಂಕ, ಗುರಿ_ಉದ್ದಳತೆ, escSeq,err,len,my_ಮೂಲ_ಸೂಚ್ಯಂಕ);
                        my_Converter_ದತ್ತಾಂಶ->isEscapeAppended = ನಿಜ;
                        
                    }
                }
            
                ಆದರೆ(is_ಗುರಿ_UChar_DBCS){
                    ಆದರೆ( my_ಗುರಿ_ಸೂಚ್ಯಂಕ <ಗುರಿ_ಉದ್ದಳತೆ){
                        my_ಗುರಿ[my_ಗುರಿ_ಸೂಚ್ಯಂಕ++] =(ಅಕ್ಷರ) (ಗುರಿ_Uni_Char >> 8);
                        ಆದರೆ(offsets){
                            *(offsets++) = my_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        ಆದರೆ(my_ಗುರಿ_ಸೂಚ್ಯಂಕ < ಗುರಿ_ಉದ್ದಳತೆ){
                            my_ಗುರಿ[my_ಗುರಿ_ಸೂಚ್ಯಂಕ++] =(ಅಕ್ಷರ) ಗುರಿ_Uni_Char;
                            ಆದರೆ(offsets){
                                *(offsets++) = my_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                            }
                        }ಇಲ್ಲದಿದ್ದರೆ{
                            ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = (ಅಕ್ಷರ) ಗುರಿ_Uni_Char;
                            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                        } 
                    }ಇಲ್ಲದಿದ್ದರೆ{
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] =(ಅಕ್ಷರ) (ಗುರಿ_Uni_Char >> 8);
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = (ಅಕ್ಷರ) ಗುರಿ_Uni_Char;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }

                }ಇಲ್ಲದಿದ್ದರೆ{
                    ಆದರೆ( my_ಗುರಿ_ಸೂಚ್ಯಂಕ <ಗುರಿ_ಉದ್ದಳತೆ){
                        my_ಗುರಿ[my_ಗುರಿ_ಸೂಚ್ಯಂಕ++] = (ಅಕ್ಷರ) (ಗುರಿ_Uni_Char );
                        ಆದರೆ(offsets){
                            *(offsets++) = my_ಮೂಲ_ಸೂಚ್ಯಂಕ-1;
                        }
                        
                    }ಇಲ್ಲದಿದ್ದರೆ{
                        ವಾದಗಳು_2->converter->char_Error_ಬಫರ್[ವಾದಗಳು_2->converter->char_Error_ಬಫರ್_ಉದ್ದಳತೆ++] = (ಅಕ್ಷರ) ಗುರಿ_Uni_Char;
                        *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                    }
                }

            }
            ಇಲ್ಲದಿದ್ದರೆ{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                ಆದರೆ(U16_IS_SURROGATE(my_ಮೂಲ_Char)) {
                    ಆದರೆ(U16_IS_SURROGATE_LEAD(my_ಮೂಲ_Char)) {
                        ವಾದಗಳು_2->converter->fromUChar32=my_ಮೂಲ_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ಆದರೆ(my_ಮೂಲ_ಸೂಚ್ಯಂಕ <  my_ಮೂಲ_ಉದ್ದಳತೆ) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) ವಾದಗಳು_2->ಮೂಲ[my_ಮೂಲ_ಸೂಚ್ಯಂಕ];
                            ಆದರೆ(U16_IS_TRAIL(trail)) {
                                ++my_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                my_ಮೂಲ_Char=U16_GET_SUPPLEMENTARY(ವಾದಗಳು_2->converter->fromUChar32, trail);
                                ವಾದಗಳು_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                                /* exit this condition tree */
                            } ಇಲ್ಲದಿದ್ದರೆ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_ಸಿಕ್ಕಿತು;
                }

                ವಾದಗಳು_2->converter->fromUChar32=my_ಮೂಲ_Char;
                ನಿಲ್ಲಿಸು;
            }
        }
        ಇಲ್ಲದಿದ್ದರೆ{
            *err = U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
            ನಿಲ್ಲಿಸು;
        }
        ಗುರಿ_Uni_Char=missingCharMarker;
    }

    ವಾದಗಳು_2->ಗುರಿ += my_ಗುರಿ_ಸೂಚ್ಯಂಕ;
    ವಾದಗಳು_2->ಮೂಲ += my_ಮೂಲ_ಸೂಚ್ಯಂಕ;
    my_Converter_ದತ್ತಾಂಶ->is_ಗುರಿ_UChar_DBCS = is_ಗುರಿ_UChar_DBCS;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_ವಾದಗಳು *ವಾದಗಳು_2, int32_t ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, UError_ಸಂಕೇತ *err) {
    UConverter *cnv = ವಾದಗಳು_2->converter;
    UConverter_ದತ್ತಾಂಶ_HZ *conv_ದತ್ತಾಂಶ=(UConverter_ದತ್ತಾಂಶ_HZ *) cnv->extraInfo;
    ಅಕ್ಷರ *p;
    ಅಕ್ಷರ ಬಫರ್[4];
    p = ಬಫರ್;
    
    ಆದರೆ( conv_ದತ್ತಾಂಶ->is_ಗುರಿ_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_ದತ್ತಾಂಶ->is_ಗುರಿ_UChar_DBCS=ಸುಳ್ಳು;
    }
    *p++= (ಅಕ್ಷರ)cnv->subChars[0];

    ucnv_cb_From_UWrite_ಬೈಟುಗಳು(ವಾದಗಳು_2,
                           ಬಫರ್, (int32_t)(p - ಬಫರ್),
                           ಸ್ಥಾನಾಂತರ_ಸೂಚ್ಯಂಕ, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
ರಚನೆ cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_ದತ್ತಾಂಶ_HZ mydata;
};


ಸ್ಥಿರ UConverter *  U_CALLCONV
HZ_ಸುರಕ್ಷಿತ_Clone(ಸ್ಥಿರಾಂಕ UConverter *cnv, 
              ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್, 
              int32_t *p_ಬಫರ್_ಗಾತ್ರ, 
              UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ರಚನೆ cloneHZStruct * localClone;
    int32_t size, ಬಫರ್_ಗಾತ್ರ_Needed = ಗಾತ್ರ(ರಚನೆ cloneHZStruct);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (*p_ಬಫರ್_ಗಾತ್ರ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ಬಫರ್_ಗಾತ್ರ = ಬಫರ್_ಗಾತ್ರ_Needed;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    localClone = (ರಚನೆ cloneHZStruct *)ರಾಶಿ_ಬಫರ್;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ಗಾತ್ರ(UConverter_ದತ್ತಾಂಶ_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ನಿಜ;

    /* deep-clone the sub-converter */
    size = (int32_t)ಗಾತ್ರ(UConverter);
    ((UConverter_ದತ್ತಾಂಶ_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_ಸುರಕ್ಷಿತ_Clone(((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, ಸ್ಥಿತಿ_2);

    ಹಿಂತಿರುಗಿ &localClone->cnv;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
HZ_Get_Unicode_ಗಣ(ಸ್ಥಿರಾಂಕ UConverter *cnv,
                  ಸ್ಥಿರಾಂಕ USetAdder *sa,
                  UConverter_Unicode_ಗಣ which,
                  UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    /* HZ converts all of ASCII */
    sa->ಕೂಡಿಸು_Range(sa->ಗಣ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_ಗಣ_For_Unicode(
        ((UConverter_ದತ್ತಾಂಶ_HZ*)cnv->extraInfo)->gbConverter->shared_ದತ್ತಾಂಶ,
        sa, which, UCNV_ಗಣ_FILTER_HZ,
        p_Error_ಸಂಕೇತ);
}
U_CDECL_ಕೊನೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _HZImpl={

    UCNV_HZ,
    
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_ಗಮ್ಯ_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_ಗಮ್ಯ_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    ಶೂನ್ಯಸೂಚಕ,
    
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,
    _HZ_WriteSub,
    HZ_ಸುರಕ್ಷಿತ_Clone,
    HZ_Get_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ HZStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        ಸುಳ್ಳು, 
        ಸುಳ್ಳು,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _HZData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&HZStatic_ದತ್ತಾಂಶ, &_HZImpl);

#ಆದರೆ_ಅಂತ್ಯ /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
