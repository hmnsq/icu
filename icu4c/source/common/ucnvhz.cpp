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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uset.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_imp.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_TILDE 0x7E          /* ~ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_OPEN_BRACE 0x7B     /* { */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UCNV_CLOSE_BRACE 0x7D   /* } */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SB_ESCAPE    "\x7E\x7D"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DB_ESCAPE    "\x7E\x7B"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TILDE_ESCAPE "\x7E\x7E"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESC_LEN       2


#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONCAT_ESCAPE_MACRO(ਦਲੀਲਾਂ_2, ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ,ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ,str_ਮੰਜ਼ਿਲ_Append, err, len,ਸਰੋਤ_ਸੂਚਕਾਂਕ) UPRV_ਖੰਡ_MACRO_BEGIN {      \
    ਜਦੋਂ(len-->0){                                                                                                         \
        ਜੇ(ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ < ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ){                                                                                     \
            ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ[ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) *str_ਮੰਜ਼ਿਲ_Append;                                                       \
            ਜੇ(ਦਲੀਲਾਂ_2->offsets!=ਖਾਲੀ_ਸੂਚਕ){                                                                                        \
                *(offsets++) = ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;                                                                               \
            }                                                                                                               \
            ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ++;                                                                                                  \
        }                                                                                                                   \
        ਨਹੀਂਤਾਂ{                                                                                                               \
            ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[(ਪੂਰਨ)ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) *str_ਮੰਜ਼ਿਲ_Append; \
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;                                                                                  \
        }                                                                                                                   \
        str_ਮੰਜ਼ਿਲ_Append++;                                                                                                      \
    }                                                                                                                       \
} UPRV_ਖੰਡ_MACRO_ਸਿਰਾ


ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ{
    UConverter* gbConverter;
    int32_t ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ;
    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    UBool isEscapeAppended;
    UBool isStateDBCS;
    UBool is_ਨਿਸ਼ਾਨਾ_UChar_DBCS;
    UBool is_ਖਾਲੀ_ਥਾਵਾਂ_Segment;
}UConverter_ਅੰਕੜੇ_HZ;


U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_HZOpen(UConverter *cnv, UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ, UError_ਕੋਡ *error_ਕੋਡ){
    UConverter *gbConverter;
    ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
        ucnv_canCreateConverter("GBK", error_ਕੋਡ);  /* errorCode carries result */
        ਵਾਪਸ;
    }
    gbConverter = ucnv_open("GBK", error_ਕੋਡ);
    ਜੇ(U_FAILURE(*error_ਕੋਡ)) {
        ਵਾਪਸ;
    }
    cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
    cnv->from_Unicode_ਸਥਿਤੀ= 0;
    cnv->ਬਹੁਲਕ=0;
    cnv->fromUChar32=0x0000;
    cnv->extraInfo = uprv_calloc(1, ਆਕਾਰ(UConverter_ਅੰਕੜੇ_HZ));
    ਜੇ(cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ){
        ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->gbConverter = gbConverter;
    }
    ਨਹੀਂਤਾਂ {
        ucnv_close(gbConverter);
        *error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
        ਵਾਪਸ;
    }
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_HZClose(UConverter *cnv){
    ਜੇ(cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ) {
        ucnv_close (((UConverter_ਅੰਕੜੇ_HZ *) (cnv->extraInfo))->gbConverter);
        ਜੇ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo = ਖਾਲੀ_ਸੂਚਕ;
    }
}

ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
_HZReset(UConverter *cnv, UConverterResetChoice choice){
    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        cnv->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = 0;
        cnv->ਬਹੁਲਕ=0;
        ਜੇ(cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ){
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->isStateDBCS = ਝੂਠ;
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;
        }
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        cnv->from_Unicode_ਸਥਿਤੀ= 0;
        cnv->fromUChar32=0x0000; 
        ਜੇ(cnv->extraInfo != ਖਾਲੀ_ਸੂਚਕ){
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->isEscapeAppended = ਝੂਠ;
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ = 0;
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->ਸਰੋਤ_ਸੂਚਕਾਂਕ = 0;
            ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = ਝੂਠ;
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


ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
UConverter_ਮੰਜ਼ਿਲ_Unicode_HZ_OFFSETS_LOGIC(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2,
                                                            UError_ਕੋਡ* err){
    ਅੱਖਰ ਆਰਜ਼ੀ_Buf[2];
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ = ( ਅੱਖਰ *) ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    char16_t *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    ਅਚਲ ਅੱਖਰ *my_ਸਰੋਤ_ਹੱਦਬੰਦੀ = ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    UChar32 ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    int32_t my_ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_HZ* my_ਅੰਕੜੇ=(UConverter_ਅੰਕੜੇ_HZ*)(ਦਲੀਲਾਂ_2->converter->extraInfo);
    ਆਰਜ਼ੀ_Buf[0]=0; 
    ਆਰਜ਼ੀ_Buf[1]=0;

    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < args->target) || (mySourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    
    ਜਦੋਂ(my_ਸਰੋਤ< my_ਸਰੋਤ_ਹੱਦਬੰਦੀ){
        
        ਜੇ(my_ਨਿਸ਼ਾਨਾ < ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ){
            
            my_ਸਰੋਤ_Char= (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) *my_ਸਰੋਤ++;

            ਜੇ(ਦਲੀਲਾਂ_2->converter->ਬਹੁਲਕ == UCNV_TILDE) {
                /* second byte after ~ */
                ਦਲੀਲਾਂ_2->converter->ਬਹੁਲਕ=0;
                ਚੋਣ(my_ਸਰੋਤ_Char) {
                ਹਾਲਤ 0x0A:
                    /* no output for ~\n (line-continuation marker) */
                    ਜਾਰੀ;
                ਹਾਲਤ UCNV_TILDE:
                    ਜੇ(ਦਲੀਲਾਂ_2->offsets) {
                        ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ]=(int32_t)(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 2);
                    }
                    *(my_ਨਿਸ਼ਾਨਾ++)=(char16_t)my_ਸਰੋਤ_Char;
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ;
                    ਜਾਰੀ;
                ਹਾਲਤ UCNV_OPEN_BRACE:
                ਹਾਲਤ UCNV_CLOSE_BRACE:
                    my_ਅੰਕੜੇ->isStateDBCS = (my_ਸਰੋਤ_Char == UCNV_OPEN_BRACE);
                    ਜੇ (my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment) {
                        my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ; /* we are handling it, reset to avoid future spurious errors */
                        *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UCallback_Reason = UCNV_IRREGULAR;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = UCNV_TILDE;
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[1] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 2;
                        ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                        ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                        ਵਾਪਸ;
                    }
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਸੱਚ;
                    ਜਾਰੀ;
                ਮੂਲ:
                     /* if the first byte is equal to TILDE and the trail byte
                     * is not a valid byte then it is an error condition
                     */
                    /*
                     * Ticket 5691: consistent illegal sequences:
                     * - We include at least the first byte in the illegal sequence.
                     * - If any of the non-initial bytes could be the start of a character,
                     *   we stop the illegal sequence before the first one of those.
                     */
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ; /* different error here, reset this to avoid spurious future error */
                    *err = U_ILLEGAL_ESCAPE_SEQUENCE;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = UCNV_TILDE;
                    ਜੇ( my_ਅੰਕੜੇ->isStateDBCS ?
                            (0x21 <= my_ਸਰੋਤ_Char && my_ਸਰੋਤ_Char <= 0x7e) :
                            my_ਸਰੋਤ_Char <= 0x7f
                    ) {
                        /* The current byte could be the start of a character: Back it out. */
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 1;
                        --my_ਸਰੋਤ;
                    } ਨਹੀਂਤਾਂ {
                        /* Include the current byte in the illegal sequence. */
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[1] = static_cast<uint8_t>(my_ਸਰੋਤ_Char);
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength = 2;
                    }
                    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
                    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
                    ਵਾਪਸ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(my_ਅੰਕੜੇ->isStateDBCS) {
                ਜੇ(ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ == 0x00){
                    /* lead byte */
                    ਜੇ(my_ਸਰੋਤ_Char == UCNV_TILDE) {
                        ਦਲੀਲਾਂ_2->converter->ਬਹੁਲਕ = UCNV_TILDE;
                    } ਨਹੀਂਤਾਂ {
                        /* add another bit to distinguish a 0 byte from not having seen a lead byte */
                        ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ = (uint32_t) (my_ਸਰੋਤ_Char | 0x100);
                        my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ; /* the segment has something, either valid or will produce a different error, so reset this */
                    }
                    ਜਾਰੀ;
                }
                ਨਹੀਂਤਾਂ{
                    /* trail byte */
                    ਪੂਰਨ leadIsOk, trailIsOk;
                    uint32_t leadByte = ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ & 0xff;
                    ਨਿਸ਼ਾਨਾ_Uni_Char = 0xffff;
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
                    trailIsOk = (uint8_t)(my_ਸਰੋਤ_Char - 0x21) <= (0x7e - 0x21);
                    ਜੇ (leadIsOk && trailIsOk) {
                        ਆਰਜ਼ੀ_Buf[0] = (ਅੱਖਰ) (leadByte+0x80) ;
                        ਆਰਜ਼ੀ_Buf[1] = (ਅੱਖਰ) (my_ਸਰੋਤ_Char+0x80);
                        ਨਿਸ਼ਾਨਾ_Uni_Char = ucnv_MBCSSimple_Get_ਅਗਲਾ_UChar(my_ਅੰਕੜੇ->gbConverter->shared_ਅੰਕੜੇ,
                            ਆਰਜ਼ੀ_Buf, 2, ਦਲੀਲਾਂ_2->converter->useFallback);
                        my_ਸਰੋਤ_Char= (leadByte << 8) | my_ਸਰੋਤ_Char;
                    } ਨਹੀਂਤਾਂ ਜੇ (trailIsOk) {
                        /* report a single illegal byte and continue with the following DBCS starter byte */
                        --my_ਸਰੋਤ;
                        my_ਸਰੋਤ_Char = (int32_t)leadByte;
                    } ਨਹੀਂਤਾਂ {
                        /* report a pair of illegal bytes if the second byte is not a DBCS starter */
                        /* add another bit so that the code below writes 2 bytes in case of error */
                        my_ਸਰੋਤ_Char= 0x10000 | (leadByte << 8) | my_ਸਰੋਤ_Char;
                    }
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_Unicode_ਸਥਿਤੀ =0x00;
                }
            }
            ਨਹੀਂਤਾਂ{
                ਜੇ(my_ਸਰੋਤ_Char == UCNV_TILDE) {
                    ਦਲੀਲਾਂ_2->converter->ਬਹੁਲਕ = UCNV_TILDE;
                    ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = (char16_t)my_ਸਰੋਤ_Char;  /* ASCII */
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ; /* the segment has something valid */
                } ਨਹੀਂਤਾਂ {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = 0xffff;
                    my_ਅੰਕੜੇ->is_ਖਾਲੀ_ਥਾਵਾਂ_Segment = ਝੂਠ; /* different error here, reset this to avoid spurious future error */
                }
            }
            ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char < 0xfffe){
                ਜੇ(ਦਲੀਲਾਂ_2->offsets) {
                    ਦਲੀਲਾਂ_2->offsets[my_ਨਿਸ਼ਾਨਾ - ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ]=(int32_t)(my_ਸਰੋਤ - ਦਲੀਲਾਂ_2->ਸਰੋਤ - 1-(my_ਅੰਕੜੇ->isStateDBCS));
                }

                *(my_ਨਿਸ਼ਾਨਾ++)=(char16_t)ਨਿਸ਼ਾਨਾ_Uni_Char;
            }
            ਨਹੀਂਤਾਂ /* targetUniChar>=0xfffe */ {
                ਜੇ(ਨਿਸ਼ਾਨਾ_Uni_Char == 0xfffe){
                    *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                }
                ਨਹੀਂਤਾਂ{
                    *err = U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                }
                ਜੇ(my_ਸਰੋਤ_Char > 0xff){
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = (uint8_t)(my_ਸਰੋਤ_Char >> 8);
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[1] = (uint8_t)my_ਸਰੋਤ_Char;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength=2;
                }
                ਨਹੀਂਤਾਂ{
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_UBytes[0] = (uint8_t)my_ਸਰੋਤ_Char;
                    ਦਲੀਲਾਂ_2->converter->ਮੰਜ਼ਿਲ_ULength=1;
                }
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ{
            *err =U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ = my_ਨਿਸ਼ਾਨਾ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ = my_ਸਰੋਤ;
}


ਸਥਿਰ ਖਾਲੀ  U_CALLCONV
UConverter_fromUnicode_HZ_OFFSETS_LOGIC (UConverter_From_Unicode_ਦਲੀਲਾਂ * ਦਲੀਲਾਂ_2,
                                                      UError_ਕੋਡ * err){
    ਅਚਲ char16_t *my_ਸਰੋਤ = ਦਲੀਲਾਂ_2->ਸਰੋਤ;
    ਅੱਖਰ *my_ਨਿਸ਼ਾਨਾ = ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ;
    int32_t* offsets = ਦਲੀਲਾਂ_2->offsets;
    int32_t my_ਸਰੋਤ_ਸੂਚਕਾਂਕ = 0;
    int32_t my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ = 0;
    int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ = (int32_t)(ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ - my_ਨਿਸ਼ਾਨਾ);
    int32_t my_ਸਰੋਤ_ਲੰਮਾਈ = (int32_t)(ਦਲੀਲਾਂ_2->ਸਰੋਤ_ਹੱਦਬੰਦੀ - ਦਲੀਲਾਂ_2->ਸਰੋਤ);
    uint32_t ਨਿਸ਼ਾਨਾ_Uni_Char = 0x0000;
    UChar32 my_ਸਰੋਤ_Char = 0x0000;
    UConverter_ਅੰਕੜੇ_HZ *my_Converter_ਅੰਕੜੇ=(UConverter_ਅੰਕੜੇ_HZ*)ਦਲੀਲਾਂ_2->converter->extraInfo;
    UBool is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = my_Converter_ਅੰਕੜੇ->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS;
    UBool old_Is_ਨਿਸ਼ਾਨਾ_UChar_DBCS;
    ਪੂਰਨ len =0;
    ਅਚਲ ਅੱਖਰ* escSeq=ਖਾਲੀ_ਸੂਚਕ;
    
    /* Calling code already handles this situation. */
    /*if ((args->converter == nullptr) || (args->targetLimit < myTarget) || (args->sourceLimit < args->source)){
        *err = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }*/
    ਜੇ(ਦਲੀਲਾਂ_2->converter->fromUChar32!=0 && my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ < ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ) {
        ਜਾਓ getTrail;
    }
    /*writing the char to the output stream */
    ਜਦੋਂ (my_ਸਰੋਤ_ਸੂਚਕਾਂਕ < my_ਸਰੋਤ_ਲੰਮਾਈ){
        ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
        ਜੇ (my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ < ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ){
            
            my_ਸਰੋਤ_Char = my_ਸਰੋਤ[my_ਸਰੋਤ_ਸੂਚਕਾਂਕ++];
            

            old_Is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = is_ਨਿਸ਼ਾਨਾ_UChar_DBCS;
            ਜੇ(my_ਸਰੋਤ_Char ==UCNV_TILDE){
                /*concatEscape(args, &myTargetIndex, &targetLength,"\x7E\x7E",err,2,&mySourceIndex);*/
                len = ESC_LEN;
                escSeq = TILDE_ESCAPE;
                CONCAT_ESCAPE_MACRO(ਦਲੀਲਾਂ_2, my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ, ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ, escSeq,err,len,my_ਸਰੋਤ_ਸੂਚਕਾਂਕ);
                ਜਾਰੀ;
            } ਨਹੀਂਤਾਂ ਜੇ(my_ਸਰੋਤ_Char <= 0x7f) {
                ਨਿਸ਼ਾਨਾ_Uni_Char = my_ਸਰੋਤ_Char;
            } ਨਹੀਂਤਾਂ {
                int32_t length= ucnv_MBCSFromUChar32(my_Converter_ਅੰਕੜੇ->gbConverter->shared_ਅੰਕੜੇ,
                    my_ਸਰੋਤ_Char,&ਨਿਸ਼ਾਨਾ_Uni_Char,ਦਲੀਲਾਂ_2->converter->useFallback);
                /* we can only use lead bytes 21..7D and trail bytes 21..7E */
                ਜੇ( length == 2 &&
                    (uint16_t)(ਨਿਸ਼ਾਨਾ_Uni_Char - 0xa1a1) <= (0xfdfe - 0xa1a1) &&
                    (uint8_t)(ਨਿਸ਼ਾਨਾ_Uni_Char - 0xa1) <= (0xfe - 0xa1)
                ) {
                    ਨਿਸ਼ਾਨਾ_Uni_Char -= 0x8080;
                } ਨਹੀਂਤਾਂ {
                    ਨਿਸ਼ਾਨਾ_Uni_Char = missingCharMarker;
                }
            }
            ਜੇ (ਨਿਸ਼ਾਨਾ_Uni_Char != missingCharMarker){
               my_Converter_ਅੰਕੜੇ->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = ਨਿਸ਼ਾਨਾ_Uni_Char > 0x00FF;
                 ਜੇ(old_Is_ਨਿਸ਼ਾਨਾ_UChar_DBCS != is_ਨਿਸ਼ਾਨਾ_UChar_DBCS || !my_Converter_ਅੰਕੜੇ->isEscapeAppended ){
                    /*Shifting from a double byte to single byte mode*/
                    ਜੇ(!is_ਨਿਸ਼ਾਨਾ_UChar_DBCS){
                        len =ESC_LEN;
                        escSeq = SB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(ਦਲੀਲਾਂ_2, my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ, ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ, escSeq,err,len,my_ਸਰੋਤ_ਸੂਚਕਾਂਕ);
                        my_Converter_ਅੰਕੜੇ->isEscapeAppended = ਸੱਚ;
                    }
                    ਨਹੀਂਤਾਂ{ /* Shifting from a single byte to double byte mode*/
                        len =ESC_LEN;
                        escSeq = DB_ESCAPE;
                        CONCAT_ESCAPE_MACRO(ਦਲੀਲਾਂ_2, my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ, ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ, escSeq,err,len,my_ਸਰੋਤ_ਸੂਚਕਾਂਕ);
                        my_Converter_ਅੰਕੜੇ->isEscapeAppended = ਸੱਚ;
                        
                    }
                }
            
                ਜੇ(is_ਨਿਸ਼ਾਨਾ_UChar_DBCS){
                    ਜੇ( my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ <ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ){
                        my_ਨਿਸ਼ਾਨਾ[my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ++] =(ਅੱਖਰ) (ਨਿਸ਼ਾਨਾ_Uni_Char >> 8);
                        ਜੇ(offsets){
                            *(offsets++) = my_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        ਜੇ(my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ < ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ){
                            my_ਨਿਸ਼ਾਨਾ[my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ++] =(ਅੱਖਰ) ਨਿਸ਼ਾਨਾ_Uni_Char;
                            ਜੇ(offsets){
                                *(offsets++) = my_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                            }
                        }ਨਹੀਂਤਾਂ{
                            ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = (ਅੱਖਰ) ਨਿਸ਼ਾਨਾ_Uni_Char;
                            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                        } 
                    }ਨਹੀਂਤਾਂ{
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] =(ਅੱਖਰ) (ਨਿਸ਼ਾਨਾ_Uni_Char >> 8);
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = (ਅੱਖਰ) ਨਿਸ਼ਾਨਾ_Uni_Char;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }

                }ਨਹੀਂਤਾਂ{
                    ਜੇ( my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ <ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ){
                        my_ਨਿਸ਼ਾਨਾ[my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ++] = (ਅੱਖਰ) (ਨਿਸ਼ਾਨਾ_Uni_Char );
                        ਜੇ(offsets){
                            *(offsets++) = my_ਸਰੋਤ_ਸੂਚਕਾਂਕ-1;
                        }
                        
                    }ਨਹੀਂਤਾਂ{
                        ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ[ਦਲੀਲਾਂ_2->converter->char_Error_ਬਫਰ_ਲੰਮਾਈ++] = (ਅੱਖਰ) ਨਿਸ਼ਾਨਾ_Uni_Char;
                        *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
                    }
                }

            }
            ਨਹੀਂਤਾਂ{
                /* oops.. the code point is unassigned */
                /*Handle surrogates */
                /*check if the char is a First surrogate*/
                ਜੇ(U16_IS_SURROGATE(my_ਸਰੋਤ_Char)) {
                    ਜੇ(U16_IS_SURROGATE_LEAD(my_ਸਰੋਤ_Char)) {
                        ਦਲੀਲਾਂ_2->converter->fromUChar32=my_ਸਰੋਤ_Char;
getTrail:
                        /*look ahead to find the trail surrogate*/
                        ਜੇ(my_ਸਰੋਤ_ਸੂਚਕਾਂਕ <  my_ਸਰੋਤ_ਲੰਮਾਈ) {
                            /* test the following code unit */
                            char16_t trail=(char16_t) ਦਲੀਲਾਂ_2->ਸਰੋਤ[my_ਸਰੋਤ_ਸੂਚਕਾਂਕ];
                            ਜੇ(U16_IS_TRAIL(trail)) {
                                ++my_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                my_ਸਰੋਤ_Char=U16_GET_SUPPLEMENTARY(ਦਲੀਲਾਂ_2->converter->fromUChar32, trail);
                                ਦਲੀਲਾਂ_2->converter->fromUChar32=0x00;
                                /* there are no surrogates in GB2312*/
                                *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                                /* exit this condition tree */
                            } ਨਹੀਂਤਾਂ {
                                /* this is an unmatched lead code unit (1st surrogate) */
                                /* callback(illegal) */
                                *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* no more input */
                            *err = U_ZERO_ERROR;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* this is an unmatched trail code unit (2nd surrogate) */
                        /* callback(illegal) */
                        *err=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* callback(unassigned) for a BMP code point */
                    *err = U_INVALID_CHAR_ਮਿਲਿਆ;
                }

                ਦਲੀਲਾਂ_2->converter->fromUChar32=my_ਸਰੋਤ_Char;
                ਰੋਕੋ;
            }
        }
        ਨਹੀਂਤਾਂ{
            *err = U_ਬਫਰ_ਉਛਾਲ_ERROR;
            ਰੋਕੋ;
        }
        ਨਿਸ਼ਾਨਾ_Uni_Char=missingCharMarker;
    }

    ਦਲੀਲਾਂ_2->ਨਿਸ਼ਾਨਾ += my_ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ;
    ਦਲੀਲਾਂ_2->ਸਰੋਤ += my_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
    my_Converter_ਅੰਕੜੇ->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS = is_ਨਿਸ਼ਾਨਾ_UChar_DBCS;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_HZ_WriteSub(UConverter_From_Unicode_ਦਲੀਲਾਂ *ਦਲੀਲਾਂ_2, int32_t ਵਿੱਥ_ਸੂਚਕਾਂਕ, UError_ਕੋਡ *err) {
    UConverter *cnv = ਦਲੀਲਾਂ_2->converter;
    UConverter_ਅੰਕੜੇ_HZ *conv_ਅੰਕੜੇ=(UConverter_ਅੰਕੜੇ_HZ *) cnv->extraInfo;
    ਅੱਖਰ *p;
    ਅੱਖਰ ਬਫਰ[4];
    p = ਬਫਰ;
    
    ਜੇ( conv_ਅੰਕੜੇ->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS){
        *p++= UCNV_TILDE;
        *p++= UCNV_CLOSE_BRACE;
        conv_ਅੰਕੜੇ->is_ਨਿਸ਼ਾਨਾ_UChar_DBCS=ਝੂਠ;
    }
    *p++= (ਅੱਖਰ)cnv->subChars[0];

    ucnv_cb_From_UWrite_ਬਾਈਟਾਂ(ਦਲੀਲਾਂ_2,
                           ਬਫਰ, (int32_t)(p - ਬਫਰ),
                           ਵਿੱਥ_ਸੂਚਕਾਂਕ, err);
}

/*
 * Structure for cloning an HZ converter into a single memory block.
 */
ਬਣਤਰ cloneHZStruct
{
    UConverter cnv;
    UConverter subCnv;
    UConverter_ਅੰਕੜੇ_HZ mydata;
};


ਸਥਿਰ UConverter *  U_CALLCONV
HZ_ਸੁਰੱਖਿਅਤ_Clone(ਅਚਲ UConverter *cnv, 
              ਖਾਲੀ *ਢੇਰ_ਬਫਰ, 
              int32_t *p_ਬਫਰ_ਮਾਪ, 
              UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਬਣਤਰ cloneHZStruct * localClone;
    int32_t size, ਬਫਰ_ਮਾਪ_Needed = ਆਕਾਰ(ਬਣਤਰ cloneHZStruct);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (*p_ਬਫਰ_ਮਾਪ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ਬਫਰ_ਮਾਪ = ਬਫਰ_ਮਾਪ_Needed;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    localClone = (ਬਣਤਰ cloneHZStruct *)ਢੇਰ_ਬਫਰ;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ਆਕਾਰ(UConverter_ਅੰਕੜੇ_HZ));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ਸੱਚ;

    /* deep-clone the sub-converter */
    size = (int32_t)ਆਕਾਰ(UConverter);
    ((UConverter_ਅੰਕੜੇ_HZ*)localClone->cnv.extraInfo)->gbConverter =
        ucnv_ਸੁਰੱਖਿਅਤ_Clone(((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->gbConverter, &localClone->subCnv, &size, ਸਥਿਤੀ);

    ਵਾਪਸ &localClone->cnv;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
HZ_Get_Unicode_ਸੈੱਟ(ਅਚਲ UConverter *cnv,
                  ਅਚਲ USetAdder *sa,
                  UConverter_Unicode_ਸੈੱਟ which,
                  UError_ਕੋਡ *p_Error_ਕੋਡ) {
    /* HZ converts all of ASCII */
    sa->ਜੋੜੋ_Range(sa->ਸੈੱਟ, 0, 0x7f);

    /* add all of the code points that the sub-converter handles */
    ucnv_MBCSGet_Filtered_Unicode_ਸੈੱਟ_For_Unicode(
        ((UConverter_ਅੰਕੜੇ_HZ*)cnv->extraInfo)->gbConverter->shared_ਅੰਕੜੇ,
        sa, which, UCNV_ਸੈੱਟ_FILTER_HZ,
        p_Error_ਕੋਡ);
}
U_CDECL_ਸਿਰਾ
ਸਥਿਰ ਅਚਲ UConverterImpl _HZImpl={

    UCNV_HZ,
    
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    
    _HZOpen,
    _HZClose,
    _HZReset,
    
    UConverter_ਮੰਜ਼ਿਲ_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_ਮੰਜ਼ਿਲ_Unicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    UConverter_fromUnicode_HZ_OFFSETS_LOGIC,
    ਖਾਲੀ_ਸੂਚਕ,
    
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,
    _HZ_WriteSub,
    HZ_ਸੁਰੱਖਿਅਤ_Clone,
    HZ_Get_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ HZStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
        "HZ",
         0, 
         UCNV_IBM, 
         UCNV_HZ, 
         1, 
         4,
        { 0x1a, 0, 0, 0 },
        1,
        ਝੂਠ, 
        ਝੂਠ,
        0,
        0,
        { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, /* reserved */

};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _HZData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&HZStatic_ਅੰਕੜੇ, &_HZImpl);

#ਜੇ_ਅੰਤ /* #if !UCONFIG_NO_CONVERSION && !UCONFIG_NO_LEGACY_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION */
