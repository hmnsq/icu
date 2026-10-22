// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
*
*   Copyright (C) 2000-2016, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
******************************************************************************
*   file name:  ucnvscsu.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2000nov18
*   created by: Markus W. Scherer
*
*   This is an implementation of the Standard Compression Scheme for Unicode
*   as defined in https://www.unicode.org/reports/tr6/ .
*   Reserved commands and window settings are treated as illegal sequences and
*   will result in callback calls.
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"

#ਜੇ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ucnv_cb.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_bld.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucnv_cnv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
ਗਿਣਤੀ {
    SQ0=0x01, /* Quote from window pair 0 */
    SQ7=0x08, /* Quote from window pair 7 */
    SDX=0x0B, /* Define a window as extended */
    Srs=0x0C, /* reserved */
    SQU=0x0E, /* Quote a single Unicode character */
    SCU=0x0F, /* Change to Unicode mode */
    SC0=0x10, /* Select window 0 */
    SC7=0x17, /* Select window 7 */
    SD0=0x18, /* Define and select window 0 */
    SD7=0x1F, /* Define and select window 7 */

    UC0=0xE0, /* Select window 0 */
    UC7=0xE7, /* Select window 7 */
    UD0=0xE8, /* Define and select window 0 */
    UD7=0xEF, /* Define and select window 7 */
    UQU=0xF0, /* Quote a single Unicode character */
    UDX=0xF1, /* Define a Window as extended */
    Urs=0xF2  /* reserved */
};

ਗਿਣਤੀ {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_ਹੱਦ=0x68,
    gap_ਵਿੱਥ=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_ਸ਼ੁਰੂ=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_ਹੱਦ=0xF9
};

/* constant offsets for the 8 static windows */
ਸਥਿਰ ਅਚਲ uint32_t staticOffsets[8]={
    0x0000, /* ASCII for quoted tags */
    0x0080, /* Latin - 1 Supplement (for access to punctuation) */
    0x0100, /* Latin Extended-A */
    0x0300, /* Combining Diacritical Marks */
    0x2000, /* General Punctuation */
    0x2080, /* Currency Symbols */
    0x2100, /* Letterlike Symbols and Number Forms */
    0x3000  /* CJK Symbols and punctuation */
};

/* initial offsets for the 8 dynamic (sliding) windows */
ਸਥਿਰ ਅਚਲ uint32_t initialDynamicOffsets[8]={
    0x0080, /* Latin-1 */
    0x00C0, /* Latin Extended A */
    0x0400, /* Cyrillic */
    0x0600, /* Arabic */
    0x0900, /* Devanagari */
    0x3040, /* Hiragana */
    0x30A0, /* Katakana */
    0xFF00  /* Fullwidth ASCII */
};

/* Table of fixed predefined Offsets */
ਸਥਿਰ ਅਚਲ uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
ਗਿਣਤੀ {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t ਮੰਜ਼ਿਲ_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ;
    uint8_t ਮੰਜ਼ਿਲ_UState;
    int8_t ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ, ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ;
    uint8_t ਮੰਜ਼ਿਲ_UByte_One;
    uint8_t ਮੰਜ਼ਿਲ_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_ਬਹੁਲਕ;
    int8_t from_UDynamic_ਖਿੜਕੀ;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ;
    int8_t ਖਿੜਕੀ_Use[8];
} SCSUData;

ਸਥਿਰ ਅਚਲ int8_t initial_ਖਿੜਕੀ_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ਸਥਿਰ ਅਚਲ int8_t initial_ਖਿੜਕੀ_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

ਗਿਣਤੀ {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ਜੇ(choice<=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
        scsu->ਮੰਜ਼ਿਲ_UState=readCommand;
        scsu->ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ=scsu->ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ=0;
        scsu->ਮੰਜ਼ਿਲ_UByte_One=0;

        cnv->ਮੰਜ਼ਿਲ_ULength=0;
    }
    ਜੇ(choice!=UCNV_RESET_ਮੰਜ਼ਿਲ_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
        scsu->from_UDynamic_ਖਿੜਕੀ=0;

        scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ=0;
        ਚੋਣ(scsu->locale) {
        ਹਾਲਤ l_ja:
            uprv_memcpy(scsu->ਖਿੜਕੀ_Use, initial_ਖਿੜਕੀ_Use_ja, 8);
            ਰੋਕੋ;
        ਮੂਲ:
            uprv_memcpy(scsu->ਖਿੜਕੀ_Use, initial_ਖਿੜਕੀ_Use, 8);
            ਰੋਕੋ;
        }

        cnv->fromUChar32=0;
    }
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_ਲੱਦੋ_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    ਅਚਲ ਅੱਖਰ *locale=p_ਦਲੀਲਾਂ->locale;
    ਜੇ(p_ਦਲੀਲਾਂ->onlyTestIsLoadable) {
        ਵਾਪਸ;
    }
    cnv->extraInfo=uprv_malloc(ਆਕਾਰ(SCSUData));
    ਜੇ(cnv->extraInfo!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(locale!=ਖਾਲੀ_ਸੂਚਕ && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } ਨਹੀਂਤਾਂ {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } ਨਹੀਂਤਾਂ {
        *p_Error_ਕੋਡ=U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUClose(UConverter *cnv) {
    ਜੇ(cnv->extraInfo!=ਖਾਲੀ_ਸੂਚਕ) {
        ਜੇ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=ਖਾਲੀ_ਸੂਚਕ;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                          UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    SCSUData *scsu;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    int32_t *offsets;
    UBool is_Single_Byte_ਬਹੁਲਕ;
    uint8_t state, byteOne;
    int8_t quote_ਖਿੜਕੀ, dynamic_ਖਿੜਕੀ;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the state machine state */
    is_Single_Byte_ਬਹੁਲਕ=scsu->ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ;
    state=scsu->ਮੰਜ਼ਿਲ_UState;
    quote_ਖਿੜਕੀ=scsu->ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ;
    dynamic_ਖਿੜਕੀ=scsu->ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ;
    byteOne=scsu->ਮੰਜ਼ਿਲ_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ=state==readCommand ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /*
     * conversion "loop"
     *
     * For performance, this is not a normal C loop.
     * Instead, there are two code blocks for the two SCSU modes.
     * The function branches to either one, and a change of the mode is done with a goto to
     * the other branch.
     *
     * Each branch has two conventional loops:
     * - a fast-path loop for the most common codes in the mode
     * - a loop for all other codes in the mode
     * When the fast-path runs into a code that it cannot handle, its loop ends and it
     * runs into the following loop to handle the other codes.
     * The end of the input or output buffer is also handled by the slower loop.
     * The slow loop jumps (goto) to the fast-path loop again as soon as possible.
     *
     * The callback handling is done by returning with an error code.
     * The conversion framework actually calls the callback function.
     */
    ਜੇ(is_Single_Byte_ਬਹੁਲਕ) {
        /* fast path for single-byte mode */
        ਜੇ(state==readCommand) {
fastSingle:
            ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && (b=*ਸਰੋਤ)>=0x20) {
                ++ਸਰੋਤ;
                ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਜੇ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)b;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]+(b&0x7f);
                    ਜੇ(c<=0xffff) {
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)c;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* output surrogate pair */
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xd7c0+(c>>10));
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xdc00|(c&0x3ff));
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* target overflow */
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            }
                            cnv->UChar_Error_ਬਫਰ[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            ਜਾਓ endloop;
                        }
                    }
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ਬਹੁਲਕ:
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            b=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਚੋਣ(state) {
            ਹਾਲਤ readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ਜੇ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)b;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    ਜਾਓ fastSingle;
                } ਨਹੀਂਤਾਂ ਜੇ(SC0<=b) {
                    ਜੇ(b<=SC7) {
                        dynamic_ਖਿੜਕੀ=(int8_t)(b-SC0);
                        ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        ਜਾਓ fastSingle;
                    } ਨਹੀਂਤਾਂ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_ਖਿੜਕੀ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(/* SQ0<=b && */ b<=SQ7) {
                    quote_ਖਿੜਕੀ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SDX) {
                    state=definePairOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SQU) {
                    state=quotePairOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SCU) {
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    ਜਾਓ fastUnicode;
                } ਨਹੀਂਤਾਂ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    ਜਾਓ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=1;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairOne:
                byteOne=b;
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=quotePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairTwo:
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((byteOne<<8)|b);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ quoteOne:
                ਜੇ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)(staticOffsets[quote_ਖਿੜਕੀ]+b);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[quote_ਖਿੜਕੀ]+(b&0x7f);
                    ਜੇ(c<=0xffff) {
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)c;
                        ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                            *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* output surrogate pair */
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xd7c0+(c>>10));
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xdc00|(c&0x3ff));
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            }
                        } ਨਹੀਂਤਾਂ {
                            /* target overflow */
                            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            }
                            cnv->UChar_Error_ਬਫਰ[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            ਜਾਓ endloop;
                        }
                    }
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ definePairOne:
                dynamic_ਖਿੜਕੀ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=definePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ definePairTwo:
                scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=0x10000+(byteOne<<15UL | b<<7UL);
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ defineOne:
                ਜੇ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;
                    ਜਾਓ endloop;
                } ਨਹੀਂਤਾਂ ਜੇ(b<gap_ਹੱਦ) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=b<<7UL;
                } ਨਹੀਂਤਾਂ ਜੇ((uint8_t)(b-gap_ਹੱਦ)<(reserved_ਸ਼ੁਰੂ-gap_ਹੱਦ)) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=(b<<7UL)+gap_ਵਿੱਥ;
                } ਨਹੀਂਤਾਂ ਜੇ(b>=fixed_ਹੱਦ) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=fixedOffsets[b-fixed_ਹੱਦ];
                } ਨਹੀਂਤਾਂ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;
                    ਜਾਓ endloop;
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                state=readCommand;
                ਜਾਓ fastSingle;
            }
        }
    } ਨਹੀਂਤਾਂ {
        /* fast path for Unicode mode */
        ਜੇ(state==readCommand) {
fastUnicode:
            ਜਦੋਂ(ਸਰੋਤ+1<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && (uint8_t)((b=*ਸਰੋਤ)-UC0)>(Urs-UC0)) {
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((b<<8)|ਸਰੋਤ[1]);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ+=2;
                ਸਰੋਤ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            b=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            ਚੋਣ(state) {
            ਹਾਲਤ readCommand:
                ਜੇ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=quotePairTwo;
                } ਨਹੀਂਤਾਂ ਜੇ(/* UC0<=b && */ b<=UC7) {
                    dynamic_ਖਿੜਕੀ=(int8_t)(b-UC0);
                    ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    ਜਾਓ fastSingle;
                } ਨਹੀਂਤਾਂ ਜੇ(/* UD0<=b && */ b<=UD7) {
                    dynamic_ਖਿੜਕੀ=(int8_t)(b-UD0);
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=defineOne;
                    ਜਾਓ single_Byte_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ ਜੇ(b==UDX) {
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=definePairOne;
                    ਜਾਓ single_Byte_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ ਜੇ(b==UQU) {
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=quotePairOne;
                } ਨਹੀਂਤਾਂ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    ਜਾਓ endloop;
                }
                ਰੋਕੋ;
            ਹਾਲਤ quotePairOne:
                byteOne=b;
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=quotePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairTwo:
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((byteOne<<8)|b);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                state=readCommand;
                ਜਾਓ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) && *p_Error_ਕੋਡ!=U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ਨਹੀਂਤਾਂ ਜੇ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->ਮੰਜ਼ਿਲ_ULength=0;
    }
    scsu->ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ=is_Single_Byte_ਬਹੁਲਕ;
    scsu->ਮੰਜ਼ਿਲ_UState=state;
    scsu->ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ=quote_ਖਿੜਕੀ;
    scsu->ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ=dynamic_ਖਿੜਕੀ;
    scsu->ਮੰਜ਼ਿਲ_UByte_One=byteOne;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUToUnicode(UConverter_ਮੰਜ਼ਿਲ_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
               UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    SCSUData *scsu;
    ਅਚਲ uint8_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    char16_t *ਨਿਸ਼ਾਨਾ;
    ਅਚਲ char16_t *ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;
    UBool is_Single_Byte_ਬਹੁਲਕ;
    uint8_t state, byteOne;
    int8_t quote_ਖਿੜਕੀ, dynamic_ਖਿੜਕੀ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ਸਰੋਤ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=(ਅਚਲ uint8_t *)p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ;

    /* get the state machine state */
    is_Single_Byte_ਬਹੁਲਕ=scsu->ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ;
    state=scsu->ਮੰਜ਼ਿਲ_UState;
    quote_ਖਿੜਕੀ=scsu->ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ;
    dynamic_ਖਿੜਕੀ=scsu->ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ;
    byteOne=scsu->ਮੰਜ਼ਿਲ_UByte_One;

    /*
     * conversion "loop"
     *
     * For performance, this is not a normal C loop.
     * Instead, there are two code blocks for the two SCSU modes.
     * The function branches to either one, and a change of the mode is done with a goto to
     * the other branch.
     *
     * Each branch has two conventional loops:
     * - a fast-path loop for the most common codes in the mode
     * - a loop for all other codes in the mode
     * When the fast-path runs into a code that it cannot handle, its loop ends and it
     * runs into the following loop to handle the other codes.
     * The end of the input or output buffer is also handled by the slower loop.
     * The slow loop jumps (goto) to the fast-path loop again as soon as possible.
     *
     * The callback handling is done by returning with an error code.
     * The conversion framework actually calls the callback function.
     */
    ਜੇ(is_Single_Byte_ਬਹੁਲਕ) {
        /* fast path for single-byte mode */
        ਜੇ(state==readCommand) {
fastSingle:
            ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && (b=*ਸਰੋਤ)>=0x20) {
                ++ਸਰੋਤ;
                ਜੇ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)b;
                } ਨਹੀਂਤਾਂ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]+(b&0x7f);
                    ਜੇ(c<=0xffff) {
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)c;
                    } ਨਹੀਂਤਾਂ {
                        /* output surrogate pair */
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xd7c0+(c>>10));
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xdc00|(c&0x3ff));
                        } ਨਹੀਂਤਾਂ {
                            /* target overflow */
                            cnv->UChar_Error_ਬਫਰ[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            ਜਾਓ endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ਬਹੁਲਕ:
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            b=*ਸਰੋਤ++;
            ਚੋਣ(state) {
            ਹਾਲਤ readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ਜੇ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)b;
                    ਜਾਓ fastSingle;
                } ਨਹੀਂਤਾਂ ਜੇ(SC0<=b) {
                    ਜੇ(b<=SC7) {
                        dynamic_ਖਿੜਕੀ=(int8_t)(b-SC0);
                        ਜਾਓ fastSingle;
                    } ਨਹੀਂਤਾਂ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_ਖਿੜਕੀ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ(/* SQ0<=b && */ b<=SQ7) {
                    quote_ਖਿੜਕੀ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SDX) {
                    state=definePairOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SQU) {
                    state=quotePairOne;
                } ਨਹੀਂਤਾਂ ਜੇ(b==SCU) {
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    ਜਾਓ fastUnicode;
                } ਨਹੀਂਤਾਂ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    ਜਾਓ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=1;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairOne:
                byteOne=b;
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=quotePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairTwo:
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ quoteOne:
                ਜੇ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ਨਿਸ਼ਾਨਾ++=(char16_t)(staticOffsets[quote_ਖਿੜਕੀ]+b);
                } ਨਹੀਂਤਾਂ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[quote_ਖਿੜਕੀ]+(b&0x7f);
                    ਜੇ(c<=0xffff) {
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)c;
                    } ਨਹੀਂਤਾਂ {
                        /* output surrogate pair */
                        *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xd7c0+(c>>10));
                        ਜੇ(ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                            *ਨਿਸ਼ਾਨਾ++=(char16_t)(0xdc00|(c&0x3ff));
                        } ਨਹੀਂਤਾਂ {
                            /* target overflow */
                            cnv->UChar_Error_ਬਫਰ[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ਬਫਰ_ਲੰਮਾਈ=1;
                            *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                            ਜਾਓ endloop;
                        }
                    }
                }
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ definePairOne:
                dynamic_ਖਿੜਕੀ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=definePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ definePairTwo:
                scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                ਜਾਓ fastSingle;
            ਹਾਲਤ defineOne:
                ਜੇ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;
                    ਜਾਓ endloop;
                } ਨਹੀਂਤਾਂ ਜੇ(b<gap_ਹੱਦ) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=b<<7UL;
                } ਨਹੀਂਤਾਂ ਜੇ((uint8_t)(b-gap_ਹੱਦ)<(reserved_ਸ਼ੁਰੂ-gap_ਹੱਦ)) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=(b<<7UL)+gap_ਵਿੱਥ;
                } ਨਹੀਂਤਾਂ ਜੇ(b>=fixed_ਹੱਦ) {
                    scsu->ਮੰਜ਼ਿਲ_UDynamic_Offsets[dynamic_ਖਿੜਕੀ]=fixedOffsets[b-fixed_ਹੱਦ];
                } ਨਹੀਂਤਾਂ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=2;
                    ਜਾਓ endloop;
                }
                state=readCommand;
                ਜਾਓ fastSingle;
            }
        }
    } ਨਹੀਂਤਾਂ {
        /* fast path for Unicode mode */
        ਜੇ(state==readCommand) {
fastUnicode:
            ਜਦੋਂ(ਸਰੋਤ+1<ਸਰੋਤ_ਹੱਦਬੰਦੀ && ਨਿਸ਼ਾਨਾ<ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ && (uint8_t)((b=*ਸਰੋਤ)-UC0)>(Urs-UC0)) {
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((b<<8)|ਸਰੋਤ[1]);
                ਸਰੋਤ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ>=ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            b=*ਸਰੋਤ++;
            ਚੋਣ(state) {
            ਹਾਲਤ readCommand:
                ਜੇ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=quotePairTwo;
                } ਨਹੀਂਤਾਂ ਜੇ(/* UC0<=b && */ b<=UC7) {
                    dynamic_ਖਿੜਕੀ=(int8_t)(b-UC0);
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    ਜਾਓ fastSingle;
                } ਨਹੀਂਤਾਂ ਜੇ(/* UD0<=b && */ b<=UD7) {
                    dynamic_ਖਿੜਕੀ=(int8_t)(b-UD0);
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=defineOne;
                    ਜਾਓ single_Byte_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ ਜੇ(b==UDX) {
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=definePairOne;
                    ਜਾਓ single_Byte_ਬਹੁਲਕ;
                } ਨਹੀਂਤਾਂ ਜੇ(b==UQU) {
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    state=quotePairOne;
                } ਨਹੀਂਤਾਂ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    cnv->ਮੰਜ਼ਿਲ_UBytes[0]=b;
                    cnv->ਮੰਜ਼ਿਲ_ULength=1;
                    ਜਾਓ endloop;
                }
                ਰੋਕੋ;
            ਹਾਲਤ quotePairOne:
                byteOne=b;
                cnv->ਮੰਜ਼ਿਲ_UBytes[1]=b;
                cnv->ਮੰਜ਼ਿਲ_ULength=2;
                state=quotePairTwo;
                ਰੋਕੋ;
            ਹਾਲਤ quotePairTwo:
                *ਨਿਸ਼ਾਨਾ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                ਜਾਓ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ਜੇ(U_FAILURE(*p_Error_ਕੋਡ) && *p_Error_ਕੋਡ!=U_ਬਫਰ_ਉਛਾਲ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ਨਹੀਂਤਾਂ ਜੇ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->ਮੰਜ਼ਿਲ_ULength=0;
    }
    scsu->ਮੰਜ਼ਿਲ_UIs_Single_Byte_ਬਹੁਲਕ=is_Single_Byte_ਬਹੁਲਕ;
    scsu->ਮੰਜ਼ਿਲ_UState=state;
    scsu->ਮੰਜ਼ਿਲ_UQuote_ਖਿੜਕੀ=quote_ਖਿੜਕੀ;
    scsu->ਮੰਜ਼ਿਲ_UDynamic_ਖਿੜਕੀ=dynamic_ਖਿੜਕੀ;
    scsu->ਮੰਜ਼ਿਲ_UByte_One=byteOne;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=(ਅਚਲ ਅੱਖਰ *)ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=ਨਿਸ਼ਾਨਾ;
}
U_CDECL_ਸਿਰਾ
/* SCSU-from-Unicode conversion functions ----------------------------------- */

/*
 * This SCSU Encoder is fairly simple but uses all SCSU commands to achieve
 * reasonable results. The lookahead is minimal.
 * Many cases are simple:
 * A character fits directly into the current mode, a dynamic or static window,
 * or is not compressible. These cases are tested first.
 * Real compression heuristics are applied to the rest, in code branches for
 * single/Unicode mode and BMP/supplementary code points.
 * The heuristics used here are extremely simple.
 */

/* get the number of the window that this character is in, or -1 */
ਸਥਿਰ int8_t
get_ਖਿੜਕੀ(ਅਚਲ uint32_t offsets[8], uint32_t c) {
    ਪੂਰਨ i;
    ਲਈ(i=0; i<8; ++i) {
        ਜੇ (c - offsets[i] <= 0x7f) {
            ਵਾਪਸ static_cast<int8_t>(i);
        }
    }
    ਵਾਪਸ -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
ਸਥਿਰ UBool
is_ਵਿੱਚ_ਵਿੱਥ_ਖਿੜਕੀ_Or_ਸਿੱਧਾ(uint32_t ਵਿੱਥ, uint32_t c) {
    ਵਾਪਸ c<=ਵਿੱਥ+0x7f &&
          (c>=ਵਿੱਥ || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ਸਥਿਰ int8_t
get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(SCSUData *scsu) {
    int8_t ਖਿੜਕੀ=scsu->ਖਿੜਕੀ_Use[scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ];
    ਜੇ(++scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ==8) {
        scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ=0;
    }
    ਵਾਪਸ ਖਿੜਕੀ;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ਸਥਿਰ ਖਾਲੀ
use_Dynamic_ਖਿੜਕੀ(SCSUData *scsu, int8_t ਖਿੜਕੀ) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    ਪੂਰਨ i, j;

    i=scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ;
    ਕਰੋ {
        ਜੇ(--i<0) {
            i=7;
        }
    } ਜਦੋਂ(scsu->ਖਿੜਕੀ_Use[i]!=ਖਿੜਕੀ);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    ਜੇ(j==8) {
        j=0;
    }
    ਜਦੋਂ(j!=scsu->ਅਗਲਾ_ਖਿੜਕੀ_Use_ਸੂਚਕਾਂਕ) {
        scsu->ਖਿੜਕੀ_Use[i]=scsu->ਖਿੜਕੀ_Use[j];
        i=j;
        ਜੇ(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->ਖਿੜਕੀ_Use[i]=ਖਿੜਕੀ;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
ਸਥਿਰ ਪੂਰਨ
get_Dynamic_ਵਿੱਥ(uint32_t c, uint32_t *p_ਵਿੱਥ) {
    ਪੂਰਨ i;

    ਲਈ(i=0; i<7; ++i) {
        ਜੇ (c - fixedOffsets[i] <= 0x7f) {
            *p_ਵਿੱਥ=fixedOffsets[i];
            ਵਾਪਸ 0xf9+i;
        }
    }

    ਜੇ(c<0x80) {
        /* No dynamic window for US-ASCII. */
        ਵਾਪਸ -1;
    } ਨਹੀਂਤਾਂ ਜੇ(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_ਵਿੱਥ=c&0x7fffff80;
        ਵਾਪਸ static_cast<ਪੂਰਨ>(c >> 7);
    } ਨਹੀਂਤਾਂ ਜੇ(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_ਵਿੱਥ=c&0x7fffff80;
        ਵਾਪਸ static_cast<ਪੂਰਨ>((c - gap_ਵਿੱਥ) >> 7);
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ -1;
    }
}
U_CDECL_BEGIN
/*
 * Idea for compression:
 *  - save SCSUData and other state before really starting work
 *  - at endloop, see if compression could be better with just unicode mode
 *  - don't do this if a callback has been called
 *  - if unicode mode would be smaller, then override the results with it - may need SCU at the beginning
 *  - different buffer handling!
 *
 * Drawback or need for corrective handling:
 * it is desirable to encode U+feff as SQU fe ff for the SCSU signature, and
 * it is desirable to start a document in US-ASCII/Latin-1 for as long as possible
 * not only for compression but also for HTML/XML documents with following charset/encoding announcers.
 *
 * How to achieve both?
 *  - Only replace the result after an SDX or SCU?
 */

ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                            UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    SCSUData *scsu;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
    int32_t *offsets;

    UBool is_Single_Byte_ਬਹੁਲਕ;
    uint8_t dynamic_ਖਿੜਕੀ;
    uint32_t current_ਵਿੱਥ;

    uint32_t c, delta;

    int32_t ਸਰੋਤ_ਸੂਚਕਾਂਕ, ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t ਵਿੱਥ;
    char16_t lead, trail;
    ਪੂਰਨ ਕੋਡ;
    int8_t ਖਿੜਕੀ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);
    offsets=p_ਦਲੀਲਾਂ->offsets;

    /* get the state machine state */
    is_Single_Byte_ਬਹੁਲਕ=scsu->from_UIs_Single_Byte_ਬਹੁਲਕ;
    dynamic_ਖਿੜਕੀ=scsu->from_UDynamic_ਖਿੜਕੀ;
    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ਸਰੋਤ_ਸੂਚਕਾਂਕ= c==0 ? 0 : -1;
    ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    ਜੇ(is_Single_Byte_ਬਹੁਲਕ) {
        ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            ਜਾਓ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<=0) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

            ਜੇ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ ਜੇ(c<0x20) {
                ਜੇ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                } ਨਹੀਂਤਾਂ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ((delta=c-current_ਵਿੱਥ)<=0x7f) {
                /* use the current dynamic window */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(delta|0x80);
                ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                    *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                }
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE(c)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        trail=*ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ++ਸਰੋਤ;
                            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ਜੇ((delta=c-current_ਵਿੱਥ)<=0x7f) {
                    /* use the current dynamic window */
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(delta|0x80);
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(SC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ਕੋਡ-=0x200;
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_ਖਿੜਕੀ<<21)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)SCU;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ ਜੇ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ {
                /* compress all other BMP characters */
                ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ਜੇ(ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ || is_ਵਿੱਚ_ਵਿੱਥ_ਖਿੜਕੀ_Or_ਸਿੱਧਾ(scsu->fromUDynamicOffsets[ਖਿੜਕੀ], *ਸਰੋਤ)) {
                        /* change to dynamic window */
                        dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(SC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+ਖਿੜਕੀ)<<8)|(c-scsu->fromUDynamicOffsets[ਖਿੜਕੀ])|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+ਖਿੜਕੀ)<<8)|(c-staticOffsets[ਖਿੜਕੀ]);
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(SD0+dynamic_ਖਿੜਕੀ)<<16)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ || (uint32_t)(*ਸਰੋਤ - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    c|=SCU<<16;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            ਜਾਓ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<=0) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            c=*ਸਰੋਤ++;
            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;

            ਜੇ ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=2) {
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                    ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                        *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                    }
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                } ਨਹੀਂਤਾਂ {
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ਜੇ(!(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && (uint32_t)(*ਸਰੋਤ-0x3400)<(0xd800-0x3400))) {
                    ਜੇ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        c|=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|c;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(UD0+dynamic_ਖਿੜਕੀ)<<16)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=3;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xe000) {
                /* c is a surrogate */
                ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        trail=*ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ++ਸਰੋਤ;
                            ++ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                /* compress supplementary character */
                ਜੇ( (ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && (uint32_t)(*ਸਰੋਤ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && lead==*ਸਰੋਤ && /* too lazy to check trail in same window as source[1] */
                          (ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    ਕੋਡ-=0x200;
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_ਖਿੜਕੀ<<21)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ਬਹੁਲਕ=is_Single_Byte_ਬਹੁਲਕ;
    scsu->from_UDynamic_ਖਿੜਕੀ=dynamic_ਖਿੜਕੀ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    p_ਦਲੀਲਾਂ->offsets=offsets;
    ਵਾਪਸ;

output_ਬਾਈਟਾਂ:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਜੇ(offsets==ਖਾਲੀ_ਸੂਚਕ) {
            ਚੋਣ(length) {
                /* each branch falls through to the next one */
            ਹਾਲਤ 4:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            ਹਾਲਤ 3:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            ਹਾਲਤ 2:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            ਹਾਲਤ 1:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                U_FALLTHROUGH;
            ਮੂਲ:
                /* will never occur */
                ਰੋਕੋ;
            }
        } ਨਹੀਂਤਾਂ {
            ਚੋਣ(length) {
                /* each branch falls through to the next one */
            ਹਾਲਤ 4:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>24);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                U_FALLTHROUGH;
            ਹਾਲਤ 3:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>16);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                U_FALLTHROUGH;
            ਹਾਲਤ 2:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                U_FALLTHROUGH;
            ਹਾਲਤ 1:
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
                U_FALLTHROUGH;
            ਮੂਲ:
                /* will never occur */
                ਰੋਕੋ;
            }
        }
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ਸਰੋਤ_ਸੂਚਕਾਂਕ=ਅਗਲਾ_ਸਰੋਤ_ਸੂਚਕਾਂਕ;
        ਜਾਓ loop;
    } ਨਹੀਂਤਾਂ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        p=(uint8_t *)cnv->char_Error_ਬਫਰ;
        ਚੋਣ(length) {
            /* each branch falls through to the next one */
        ਹਾਲਤ 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ਹਾਲਤ 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        ਮੂਲ:
            /* will never occur */
            ਰੋਕੋ;
        }
        cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        ਚੋਣ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            /* each branch falls through to the next one */
        ਹਾਲਤ 3:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>16);
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
            ਜੇ(offsets!=ਖਾਲੀ_ਸੂਚਕ) {
                *offsets++=ਸਰੋਤ_ਸੂਚਕਾਂਕ;
            }
            U_FALLTHROUGH;
        ਮੂਲ:
            ਰੋਕੋ;
        }

        /* target overflow */
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        c=0;
        ਜਾਓ endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ਸਥਿਰ ਖਾਲੀ U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_ਦਲੀਲਾਂ *p_ਦਲੀਲਾਂ,
                 UError_ਕੋਡ *p_Error_ਕੋਡ) {
    UConverter *cnv;
    SCSUData *scsu;
    ਅਚਲ char16_t *ਸਰੋਤ, *ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    uint8_t *ਨਿਸ਼ਾਨਾ;
    int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;

    UBool is_Single_Byte_ਬਹੁਲਕ;
    uint8_t dynamic_ਖਿੜਕੀ;
    uint32_t current_ਵਿੱਥ;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t ਵਿੱਥ;
    char16_t lead, trail;
    ਪੂਰਨ ਕੋਡ;
    int8_t ਖਿੜਕੀ;

    /* set up the local pointers */
    cnv=p_ਦਲੀਲਾਂ->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ਸਰੋਤ=p_ਦਲੀਲਾਂ->ਸਰੋਤ;
    ਸਰੋਤ_ਹੱਦਬੰਦੀ=p_ਦਲੀਲਾਂ->ਸਰੋਤ_ਹੱਦਬੰਦੀ;
    ਨਿਸ਼ਾਨਾ=(uint8_t *)p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ;
    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=(int32_t)(p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ_ਹੱਦਬੰਦੀ-p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ);

    /* get the state machine state */
    is_Single_Byte_ਬਹੁਲਕ=scsu->from_UIs_Single_Byte_ਬਹੁਲਕ;
    dynamic_ਖਿੜਕੀ=scsu->from_UDynamic_ਖਿੜਕੀ;
    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    ਜੇ(is_Single_Byte_ਬਹੁਲਕ) {
        ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            ਜਾਓ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<=0) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            c=*ਸਰੋਤ++;

            ਜੇ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ ਜੇ(c<0x20) {
                ਜੇ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                } ਨਹੀਂਤਾਂ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ((delta=c-current_ਵਿੱਥ)<=0x7f) {
                /* use the current dynamic window */
                *ਨਿਸ਼ਾਨਾ++=(uint8_t)(delta|0x80);
                --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
            } ਨਹੀਂਤਾਂ ਜੇ(U16_IS_SURROGATE(c)) {
                ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        trail=*ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ++ਸਰੋਤ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ਜੇ((delta=c-current_ਵਿੱਥ)<=0x7f) {
                    /* use the current dynamic window */
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(delta|0x80);
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(SC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ਕੋਡ-=0x200;
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_ਖਿੜਕੀ<<21)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)SCU;
                    --ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ ਜੇ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ {
                /* compress all other BMP characters */
                ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ਜੇ(ਸਰੋਤ>=ਸਰੋਤ_ਹੱਦਬੰਦੀ || is_ਵਿੱਚ_ਵਿੱਥ_ਖਿੜਕੀ_Or_ਸਿੱਧਾ(scsu->fromUDynamicOffsets[ਖਿੜਕੀ], *ਸਰੋਤ)) {
                        /* change to dynamic window */
                        dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(SC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+ਖਿੜਕੀ)<<8)|(c-scsu->fromUDynamicOffsets[ਖਿੜਕੀ])|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    }
                } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+ਖਿੜਕੀ)<<8)|(c-staticOffsets[ਖਿੜਕੀ]);
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(SD0+dynamic_ਖਿੜਕੀ)<<16)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ (c - 0x3400 < 0xd800 - 0x3400 &&
                           (ਸਰੋਤ >= ਸਰੋਤ_ਹੱਦਬੰਦੀ || static_cast<uint32_t>(*ਸਰੋਤ - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ਬਹੁਲਕ=ਝੂਠ;
                    c|=SCU<<16;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ(c!=0 && ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>0) {
            ਜਾਓ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        ਜਦੋਂ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
            ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ<=0) {
                /* target is full */
                *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
                ਰੋਕੋ;
            }
            c=*ਸਰੋਤ++;

            ਜੇ (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ>=2) {
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
                    *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
                    ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=2;
                } ਨਹੀਂਤਾਂ {
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ ਜੇ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ਜੇ(!(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && (uint32_t)(*ਸਰੋਤ-0x3400)<(0xd800-0x3400))) {
                    ਜੇ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        c|=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|c;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ ਜੇ((ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=2;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    } ਨਹੀਂਤਾਂ ਜੇ((ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                        dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                        current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                        use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                        c=((uint32_t)(UD0+dynamic_ਖਿੜਕੀ)<<16)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                        length=3;
                        ਜਾਓ output_ਬਾਈਟਾਂ;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            } ਨਹੀਂਤਾਂ ਜੇ(c<0xe000) {
                /* c is a surrogate */
                ਜੇ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ) {
                        /* test the following code unit */
                        trail=*ਸਰੋਤ;
                        ਜੇ(U16_IS_TRAIL(trail)) {
                            ++ਸਰੋਤ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ਨਹੀਂਤਾਂ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                            ਜਾਓ endloop;
                        }
                    } ਨਹੀਂਤਾਂ {
                        /* no more input */
                        ਰੋਕੋ;
                    }
                } ਨਹੀਂਤਾਂ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ਕੋਡ=U_ILLEGAL_CHAR_ਮਿਲਿਆ;
                    ਜਾਓ endloop;
                }

                /* compress supplementary character */
                ਜੇ( (ਖਿੜਕੀ=get_ਖਿੜਕੀ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && (uint32_t)(*ਸਰੋਤ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    dynamic_ਖਿੜਕੀ=ਖਿੜਕੀ;
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ];
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)(UC0+dynamic_ਖਿੜਕੀ)<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=2;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ ਜੇ(ਸਰੋਤ<ਸਰੋਤ_ਹੱਦਬੰਦੀ && lead==*ਸਰੋਤ && /* too lazy to check trail in same window as source[1] */
                          (ਕੋਡ=get_Dynamic_ਵਿੱਥ(c, &ਵਿੱਥ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ਬਹੁਲਕ=ਸੱਚ;
                    ਕੋਡ-=0x200;
                    dynamic_ਖਿੜਕੀ=get_ਅਗਲਾ_Dynamic_ਖਿੜਕੀ(scsu);
                    current_ਵਿੱਥ=scsu->fromUDynamicOffsets[dynamic_ਖਿੜਕੀ]=ਵਿੱਥ;
                    use_Dynamic_ਖਿੜਕੀ(scsu, dynamic_ਖਿੜਕੀ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_ਖਿੜਕੀ<<21)|((uint32_t)ਕੋਡ<<8)|(c-current_ਵਿੱਥ)|0x80;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                } ਨਹੀਂਤਾਂ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ਜਾਓ output_ਬਾਈਟਾਂ;
                }
            } ਨਹੀਂਤਾਂ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                ਜਾਓ output_ਬਾਈਟਾਂ;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ਬਹੁਲਕ=is_Single_Byte_ਬਹੁਲਕ;
    scsu->from_UDynamic_ਖਿੜਕੀ=dynamic_ਖਿੜਕੀ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ਦਲੀਲਾਂ->ਸਰੋਤ=ਸਰੋਤ;
    p_ਦਲੀਲਾਂ->ਨਿਸ਼ਾਨਾ=(ਅੱਖਰ *)ਨਿਸ਼ਾਨਾ;
    ਵਾਪਸ;

output_ਬਾਈਟਾਂ:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ਜੇ(length<=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
        ਚੋਣ(length) {
            /* each branch falls through to the next one */
        ਹਾਲਤ 4:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ਹਾਲਤ 3:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
            U_FALLTHROUGH;
        ਮੂਲ:
            /* will never occur */
            ਰੋਕੋ;
        }
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ਜਾਓ loop;
    } ਨਹੀਂਤਾਂ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ;
        p=(uint8_t *)cnv->char_Error_ਬਫਰ;
        ਚੋਣ(length) {
            /* each branch falls through to the next one */
        ਹਾਲਤ 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ਹਾਲਤ 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        ਮੂਲ:
            /* will never occur */
            ਰੋਕੋ;
        }
        cnv->char_Error_ਬਫਰ_ਲੰਮਾਈ=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        ਚੋਣ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
            /* each branch falls through to the next one */
        ਹਾਲਤ 3:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ਹਾਲਤ 2:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ਹਾਲਤ 1:
            *ਨਿਸ਼ਾਨਾ++=(uint8_t)c;
            U_FALLTHROUGH;
        ਮੂਲ:
            ਰੋਕੋ;
        }

        /* target overflow */
        ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ=0;
        *p_Error_ਕੋਡ=U_ਬਫਰ_ਉਛਾਲ_ERROR;
        c=0;
        ਜਾਓ endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *  U_CALLCONV
_SCSUGetName(ਅਚਲ UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ਚੋਣ(scsu->locale) {
    ਹਾਲਤ l_ja:
        ਵਾਪਸ "SCSU,locale=ja";
    ਮੂਲ:
        ਵਾਪਸ "SCSU";
    }
}

/* structure for SafeClone calculations */
ਬਣਤਰ cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

ਸਥਿਰ UConverter *  U_CALLCONV
_SCSUSafeClone(ਅਚਲ UConverter *cnv, 
               ਖਾਲੀ *ਢੇਰ_ਬਫਰ, 
               int32_t *p_ਬਫਰ_ਮਾਪ, 
               UError_ਕੋਡ *ਸਥਿਤੀ)
{
    ਬਣਤਰ cloneSCSUStruct * localClone;
    int32_t ਬਫਰ_ਮਾਪ_Needed = ਆਕਾਰ(ਬਣਤਰ cloneSCSUStruct);

    ਜੇ (U_FAILURE(*ਸਥਿਤੀ)){
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    ਜੇ (*p_ਬਫਰ_ਮਾਪ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ਬਫਰ_ਮਾਪ = ਬਫਰ_ਮਾਪ_Needed;
        ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
    }

    localClone = (ਬਣਤਰ cloneSCSUStruct *)ਢੇਰ_ਬਫਰ;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ਆਕਾਰ(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ਸੱਚ;

    ਵਾਪਸ &localClone->cnv;
}
U_CDECL_ਸਿਰਾ

ਸਥਿਰ ਅਚਲ UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    ਖਾਲੀ_ਸੂਚਕ,

    ਖਾਲੀ_ਸੂਚਕ,
    _SCSUGetName,
    ਖਾਲੀ_ਸੂਚਕ,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_ਸੈੱਟ,
    ਖਾਲੀ_ਸੂਚਕ,
    ਖਾਲੀ_ਸੂਚਕ
};

ਸਥਿਰ ਅਚਲ UConverter_Static_ਅੰਕੜੇ SCSUStatic_ਅੰਕੜੇ={
    ਆਕਾਰ(UConverter_Static_ਅੰਕੜੇ),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    ਝੂਠ, ਝੂਠ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ਅਚਲ UConverter_Shared_ਅੰਕੜੇ _SCSUData=
        UCNV_IMMUTABLE_SHARED_ਅੰਕੜੇ_INITIALIZER(&SCSUStatic_ਅੰਕੜੇ, &_SCSUImpl);

#ਜੇ_ਅੰਤ
