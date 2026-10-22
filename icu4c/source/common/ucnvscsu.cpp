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

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"

#ಆದರೆ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ಅನ್ತರ್ಭಾವ "unicode/ucnv.h"
#ಅನ್ತರ್ಭಾವ "unicode/ucnv_cb.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ucnv_bld.h"
#ಅನ್ತರ್ಭಾವ "ucnv_cnv.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
ಎಣಿಕೆ {
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

ಎಣಿಕೆ {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_ಮಿತಿ=0x68,
    gap_ಸ್ಥಾನಾಂತರ=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_ಆರಂಭ=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_ಮಿತಿ=0xF9
};

/* constant offsets for the 8 static windows */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t staticOffsets[8]={
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t initialDynamicOffsets[8]={
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
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
ಎಣಿಕೆ {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t ಗಮ್ಯ_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ;
    uint8_t ಗಮ್ಯ_UState;
    int8_t ಗಮ್ಯ_UQuote_ಕಿಟಕಿ, ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ;
    uint8_t ಗಮ್ಯ_UByte_One;
    uint8_t ಗಮ್ಯ_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_ಬಹುಲಕ;
    int8_t from_UDynamic_ಕಿಟಕಿ;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ;
    int8_t ಕಿಟಕಿ_Use[8];
} SCSUData;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t initial_ಕಿಟಕಿ_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ int8_t initial_ಕಿಟಕಿ_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

ಎಣಿಕೆ {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ಆದರೆ(choice<=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->ಗಮ್ಯ_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ=ನಿಜ;
        scsu->ಗಮ್ಯ_UState=readCommand;
        scsu->ಗಮ್ಯ_UQuote_ಕಿಟಕಿ=scsu->ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ=0;
        scsu->ಗಮ್ಯ_UByte_One=0;

        cnv->ಗಮ್ಯ_ULength=0;
    }
    ಆದರೆ(choice!=UCNV_RESET_ಗಮ್ಯ_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_ಬಹುಲಕ=ನಿಜ;
        scsu->from_UDynamic_ಕಿಟಕಿ=0;

        scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ=0;
        ಆಯ್ಕೆ(scsu->locale) {
        ಸ್ಥಿತಿ l_ja:
            uprv_memcpy(scsu->ಕಿಟಕಿ_Use, initial_ಕಿಟಕಿ_Use_ja, 8);
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            uprv_memcpy(scsu->ಕಿಟಕಿ_Use, initial_ಕಿಟಕಿ_Use, 8);
            ನಿಲ್ಲಿಸು;
        }

        cnv->fromUChar32=0;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_ತುಂಬು_ವಾದಗಳು *p_ವಾದಗಳು,
          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *locale=p_ವಾದಗಳು->locale;
    ಆದರೆ(p_ವಾದಗಳು->onlyTestIsLoadable) {
        ಹಿಂತಿರುಗಿ;
    }
    cnv->extraInfo=uprv_malloc(ಗಾತ್ರ(SCSUData));
    ಆದರೆ(cnv->extraInfo!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(locale!=ಶೂನ್ಯಸೂಚಕ && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } ಇಲ್ಲದಿದ್ದರೆ {
        *p_Error_ಸಂಕೇತ=U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUClose(UConverter *cnv) {
    ಆದರೆ(cnv->extraInfo!=ಶೂನ್ಯಸೂಚಕ) {
        ಆದರೆ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=ಶೂನ್ಯಸೂಚಕ;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                          UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    SCSUData *scsu;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    int32_t *offsets;
    UBool is_Single_Byte_ಬಹುಲಕ;
    uint8_t state, byteOne;
    int8_t quote_ಕಿಟಕಿ, dynamic_ಕಿಟಕಿ;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;
    offsets=p_ವಾದಗಳು->offsets;

    /* get the state machine state */
    is_Single_Byte_ಬಹುಲಕ=scsu->ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ;
    state=scsu->ಗಮ್ಯ_UState;
    quote_ಕಿಟಕಿ=scsu->ಗಮ್ಯ_UQuote_ಕಿಟಕಿ;
    dynamic_ಕಿಟಕಿ=scsu->ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ;
    byteOne=scsu->ಗಮ್ಯ_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ=state==readCommand ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

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
    ಆದರೆ(is_Single_Byte_ಬಹುಲಕ) {
        /* fast path for single-byte mode */
        ಆದರೆ(state==readCommand) {
fastSingle:
            ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ && (b=*ಮೂಲ)>=0x20) {
                ++ಮೂಲ;
                ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಆದರೆ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ಗುರಿ++=(char16_t)b;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]+(b&0x7f);
                    ಆದರೆ(c<=0xffff) {
                        *ಗುರಿ++=(char16_t)c;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* output surrogate pair */
                        *ಗುರಿ++=(char16_t)(0xd7c0+(c>>10));
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=(char16_t)(0xdc00|(c&0x3ff));
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* target overflow */
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            }
                            cnv->UChar_Error_ಬಫರ್[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            ಹೋಗು endloop;
                        }
                    }
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ಬಹುಲಕ:
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            b=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆಯ್ಕೆ(state) {
            ಸ್ಥಿತಿ readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ಆದರೆ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ಗುರಿ++=(char16_t)b;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    ಹೋಗು fastSingle;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(SC0<=b) {
                    ಆದರೆ(b<=SC7) {
                        dynamic_ಕಿಟಕಿ=(int8_t)(b-SC0);
                        ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        ಹೋಗು fastSingle;
                    } ಇಲ್ಲದಿದ್ದರೆ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_ಕಿಟಕಿ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* SQ0<=b && */ b<=SQ7) {
                    quote_ಕಿಟಕಿ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SDX) {
                    state=definePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SQU) {
                    state=quotePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SCU) {
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    ಹೋಗು fastUnicode;
                } ಇಲ್ಲದಿದ್ದರೆ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    ಹೋಗು endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->ಗಮ್ಯ_UBytes[0]=b;
                cnv->ಗಮ್ಯ_ULength=1;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairOne:
                byteOne=b;
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=quotePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairTwo:
                *ಗುರಿ++=(char16_t)((byteOne<<8)|b);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ quoteOne:
                ಆದರೆ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ಗುರಿ++=(char16_t)(staticOffsets[quote_ಕಿಟಕಿ]+b);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ಗಮ್ಯ_UDynamic_Offsets[quote_ಕಿಟಕಿ]+(b&0x7f);
                    ಆದರೆ(c<=0xffff) {
                        *ಗುರಿ++=(char16_t)c;
                        ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                            *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* output surrogate pair */
                        *ಗುರಿ++=(char16_t)(0xd7c0+(c>>10));
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=(char16_t)(0xdc00|(c&0x3ff));
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            }
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* target overflow */
                            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            }
                            cnv->UChar_Error_ಬಫರ್[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            ಹೋಗು endloop;
                        }
                    }
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ definePairOne:
                dynamic_ಕಿಟಕಿ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=definePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ definePairTwo:
                scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=0x10000+(byteOne<<15UL | b<<7UL);
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ defineOne:
                ಆದರೆ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->ಗಮ್ಯ_UBytes[1]=b;
                    cnv->ಗಮ್ಯ_ULength=2;
                    ಹೋಗು endloop;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b<gap_ಮಿತಿ) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=b<<7UL;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint8_t)(b-gap_ಮಿತಿ)<(reserved_ಆರಂಭ-gap_ಮಿತಿ)) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=(b<<7UL)+gap_ಸ್ಥಾನಾಂತರ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b>=fixed_ಮಿತಿ) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=fixedOffsets[b-fixed_ಮಿತಿ];
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->ಗಮ್ಯ_UBytes[1]=b;
                    cnv->ಗಮ್ಯ_ULength=2;
                    ಹೋಗು endloop;
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                state=readCommand;
                ಹೋಗು fastSingle;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* fast path for Unicode mode */
        ಆದರೆ(state==readCommand) {
fastUnicode:
            ಆಗ(ಮೂಲ+1<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ && (uint8_t)((b=*ಮೂಲ)-UC0)>(Urs-UC0)) {
                *ಗುರಿ++=(char16_t)((b<<8)|ಮೂಲ[1]);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ+=2;
                ಮೂಲ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            b=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
            ಆಯ್ಕೆ(state) {
            ಸ್ಥಿತಿ readCommand:
                ಆದರೆ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=quotePairTwo;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* UC0<=b && */ b<=UC7) {
                    dynamic_ಕಿಟಕಿ=(int8_t)(b-UC0);
                    ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    ಹೋಗು fastSingle;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* UD0<=b && */ b<=UD7) {
                    dynamic_ಕಿಟಕಿ=(int8_t)(b-UD0);
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=defineOne;
                    ಹೋಗು single_Byte_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==UDX) {
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=definePairOne;
                    ಹೋಗು single_Byte_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==UQU) {
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=quotePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    ಹೋಗು endloop;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairOne:
                byteOne=b;
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=quotePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairTwo:
                *ಗುರಿ++=(char16_t)((byteOne<<8)|b);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                state=readCommand;
                ಹೋಗು fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) && *p_Error_ಸಂಕೇತ!=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->ಗಮ್ಯ_ULength=0;
    }
    scsu->ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ=is_Single_Byte_ಬಹುಲಕ;
    scsu->ಗಮ್ಯ_UState=state;
    scsu->ಗಮ್ಯ_UQuote_ಕಿಟಕಿ=quote_ಕಿಟಕಿ;
    scsu->ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ=dynamic_ಕಿಟಕಿ;
    scsu->ಗಮ್ಯ_UByte_One=byteOne;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUToUnicode(UConverter_ಗಮ್ಯ_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
               UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    SCSUData *scsu;
    ಸ್ಥಿರಾಂಕ uint8_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    char16_t *ಗುರಿ;
    ಸ್ಥಿರಾಂಕ char16_t *ಗುರಿ_ಎಲ್ಲೆ;
    UBool is_Single_Byte_ಬಹುಲಕ;
    uint8_t state, byteOne;
    int8_t quote_ಕಿಟಕಿ, dynamic_ಕಿಟಕಿ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ಮೂಲ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=(ಸ್ಥಿರಾಂಕ uint8_t *)p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ;

    /* get the state machine state */
    is_Single_Byte_ಬಹುಲಕ=scsu->ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ;
    state=scsu->ಗಮ್ಯ_UState;
    quote_ಕಿಟಕಿ=scsu->ಗಮ್ಯ_UQuote_ಕಿಟಕಿ;
    dynamic_ಕಿಟಕಿ=scsu->ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ;
    byteOne=scsu->ಗಮ್ಯ_UByte_One;

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
    ಆದರೆ(is_Single_Byte_ಬಹುಲಕ) {
        /* fast path for single-byte mode */
        ಆದರೆ(state==readCommand) {
fastSingle:
            ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ && (b=*ಮೂಲ)>=0x20) {
                ++ಮೂಲ;
                ಆದರೆ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ಗುರಿ++=(char16_t)b;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]+(b&0x7f);
                    ಆದರೆ(c<=0xffff) {
                        *ಗುರಿ++=(char16_t)c;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* output surrogate pair */
                        *ಗುರಿ++=(char16_t)(0xd7c0+(c>>10));
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=(char16_t)(0xdc00|(c&0x3ff));
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* target overflow */
                            cnv->UChar_Error_ಬಫರ್[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            ಹೋಗು endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ಬಹುಲಕ:
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            b=*ಮೂಲ++;
            ಆಯ್ಕೆ(state) {
            ಸ್ಥಿತಿ readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ಆದರೆ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ಗುರಿ++=(char16_t)b;
                    ಹೋಗು fastSingle;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(SC0<=b) {
                    ಆದರೆ(b<=SC7) {
                        dynamic_ಕಿಟಕಿ=(int8_t)(b-SC0);
                        ಹೋಗು fastSingle;
                    } ಇಲ್ಲದಿದ್ದರೆ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_ಕಿಟಕಿ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* SQ0<=b && */ b<=SQ7) {
                    quote_ಕಿಟಕಿ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SDX) {
                    state=definePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SQU) {
                    state=quotePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==SCU) {
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    ಹೋಗು fastUnicode;
                } ಇಲ್ಲದಿದ್ದರೆ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    ಹೋಗು endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->ಗಮ್ಯ_UBytes[0]=b;
                cnv->ಗಮ್ಯ_ULength=1;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairOne:
                byteOne=b;
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=quotePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairTwo:
                *ಗುರಿ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ quoteOne:
                ಆದರೆ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ಗುರಿ++=(char16_t)(staticOffsets[quote_ಕಿಟಕಿ]+b);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* write from dynamic window */
                    uint32_t c=scsu->ಗಮ್ಯ_UDynamic_Offsets[quote_ಕಿಟಕಿ]+(b&0x7f);
                    ಆದರೆ(c<=0xffff) {
                        *ಗುರಿ++=(char16_t)c;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* output surrogate pair */
                        *ಗುರಿ++=(char16_t)(0xd7c0+(c>>10));
                        ಆದರೆ(ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ) {
                            *ಗುರಿ++=(char16_t)(0xdc00|(c&0x3ff));
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* target overflow */
                            cnv->UChar_Error_ಬಫರ್[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_ಬಫರ್_ಉದ್ದಳತೆ=1;
                            *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                            ಹೋಗು endloop;
                        }
                    }
                }
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ definePairOne:
                dynamic_ಕಿಟಕಿ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=definePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ definePairTwo:
                scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                ಹೋಗು fastSingle;
            ಸ್ಥಿತಿ defineOne:
                ಆದರೆ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->ಗಮ್ಯ_UBytes[1]=b;
                    cnv->ಗಮ್ಯ_ULength=2;
                    ಹೋಗು endloop;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b<gap_ಮಿತಿ) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=b<<7UL;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((uint8_t)(b-gap_ಮಿತಿ)<(reserved_ಆರಂಭ-gap_ಮಿತಿ)) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=(b<<7UL)+gap_ಸ್ಥಾನಾಂತರ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b>=fixed_ಮಿತಿ) {
                    scsu->ಗಮ್ಯ_UDynamic_Offsets[dynamic_ಕಿಟಕಿ]=fixedOffsets[b-fixed_ಮಿತಿ];
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->ಗಮ್ಯ_UBytes[1]=b;
                    cnv->ಗಮ್ಯ_ULength=2;
                    ಹೋಗು endloop;
                }
                state=readCommand;
                ಹೋಗು fastSingle;
            }
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* fast path for Unicode mode */
        ಆದರೆ(state==readCommand) {
fastUnicode:
            ಆಗ(ಮೂಲ+1<ಮೂಲ_ಎಲ್ಲೆ && ಗುರಿ<ಗುರಿ_ಎಲ್ಲೆ && (uint8_t)((b=*ಮೂಲ)-UC0)>(Urs-UC0)) {
                *ಗುರಿ++=(char16_t)((b<<8)|ಮೂಲ[1]);
                ಮೂಲ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ>=ಗುರಿ_ಎಲ್ಲೆ) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            b=*ಮೂಲ++;
            ಆಯ್ಕೆ(state) {
            ಸ್ಥಿತಿ readCommand:
                ಆದರೆ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=quotePairTwo;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* UC0<=b && */ b<=UC7) {
                    dynamic_ಕಿಟಕಿ=(int8_t)(b-UC0);
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    ಹೋಗು fastSingle;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(/* UD0<=b && */ b<=UD7) {
                    dynamic_ಕಿಟಕಿ=(int8_t)(b-UD0);
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=defineOne;
                    ಹೋಗು single_Byte_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==UDX) {
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=definePairOne;
                    ಹೋಗು single_Byte_ಬಹುಲಕ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b==UQU) {
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    state=quotePairOne;
                } ಇಲ್ಲದಿದ್ದರೆ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    cnv->ಗಮ್ಯ_UBytes[0]=b;
                    cnv->ಗಮ್ಯ_ULength=1;
                    ಹೋಗು endloop;
                }
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairOne:
                byteOne=b;
                cnv->ಗಮ್ಯ_UBytes[1]=b;
                cnv->ಗಮ್ಯ_ULength=2;
                state=quotePairTwo;
                ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ quotePairTwo:
                *ಗುರಿ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                ಹೋಗು fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ಆದರೆ(U_FAILURE(*p_Error_ಸಂಕೇತ) && *p_Error_ಸಂಕೇತ!=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->ಗಮ್ಯ_ULength=0;
    }
    scsu->ಗಮ್ಯ_UIs_Single_Byte_ಬಹುಲಕ=is_Single_Byte_ಬಹುಲಕ;
    scsu->ಗಮ್ಯ_UState=state;
    scsu->ಗಮ್ಯ_UQuote_ಕಿಟಕಿ=quote_ಕಿಟಕಿ;
    scsu->ಗಮ್ಯ_UDynamic_ಕಿಟಕಿ=dynamic_ಕಿಟಕಿ;
    scsu->ಗಮ್ಯ_UByte_One=byteOne;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=ಗುರಿ;
}
U_CDECL_ಕೊನೆ
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
ಸ್ಥಿರ int8_t
get_ಕಿಟಕಿ(ಸ್ಥಿರಾಂಕ uint32_t offsets[8], uint32_t c) {
    ಪೂರ್ಣಾಂಕ i;
    ಗಾಗಿ(i=0; i<8; ++i) {
        ಆದರೆ (c - offsets[i] <= 0x7f) {
            ಹಿಂತಿರುಗಿ static_cast<int8_t>(i);
        }
    }
    ಹಿಂತಿರುಗಿ -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
ಸ್ಥಿರ UBool
is_ಒಳ_ಸ್ಥಾನಾಂತರ_ಕಿಟಕಿ_Or_ನೇರ(uint32_t ಸ್ಥಾನಾಂತರ, uint32_t c) {
    ಹಿಂತಿರುಗಿ c<=ಸ್ಥಾನಾಂತರ+0x7f &&
          (c>=ಸ್ಥಾನಾಂತರ || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ಸ್ಥಿರ int8_t
get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(SCSUData *scsu) {
    int8_t ಕಿಟಕಿ=scsu->ಕಿಟಕಿ_Use[scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ];
    ಆದರೆ(++scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ==8) {
        scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ=0;
    }
    ಹಿಂತಿರುಗಿ ಕಿಟಕಿ;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ಸ್ಥಿರ ಶೂನ್ಯ
use_Dynamic_ಕಿಟಕಿ(SCSUData *scsu, int8_t ಕಿಟಕಿ) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    ಪೂರ್ಣಾಂಕ i, j;

    i=scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ;
    ಮಾಡು {
        ಆದರೆ(--i<0) {
            i=7;
        }
    } ಆಗ(scsu->ಕಿಟಕಿ_Use[i]!=ಕಿಟಕಿ);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    ಆದರೆ(j==8) {
        j=0;
    }
    ಆಗ(j!=scsu->ಮುಂದಿನ_ಕಿಟಕಿ_Use_ಸೂಚ್ಯಂಕ) {
        scsu->ಕಿಟಕಿ_Use[i]=scsu->ಕಿಟಕಿ_Use[j];
        i=j;
        ಆದರೆ(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->ಕಿಟಕಿ_Use[i]=ಕಿಟಕಿ;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_Dynamic_ಸ್ಥಾನಾಂತರ(uint32_t c, uint32_t *p_ಸ್ಥಾನಾಂತರ) {
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ(i=0; i<7; ++i) {
        ಆದರೆ (c - fixedOffsets[i] <= 0x7f) {
            *p_ಸ್ಥಾನಾಂತರ=fixedOffsets[i];
            ಹಿಂತಿರುಗಿ 0xf9+i;
        }
    }

    ಆದರೆ(c<0x80) {
        /* No dynamic window for US-ASCII. */
        ಹಿಂತಿರುಗಿ -1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_ಸ್ಥಾನಾಂತರ=c&0x7fffff80;
        ಹಿಂತಿರುಗಿ static_cast<ಪೂರ್ಣಾಂಕ>(c >> 7);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_ಸ್ಥಾನಾಂತರ=c&0x7fffff80;
        ಹಿಂತಿರುಗಿ static_cast<ಪೂರ್ಣಾಂಕ>((c - gap_ಸ್ಥಾನಾಂತರ) >> 7);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ -1;
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

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                            UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    SCSUData *scsu;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
    int32_t *offsets;

    UBool is_Single_Byte_ಬಹುಲಕ;
    uint8_t dynamic_ಕಿಟಕಿ;
    uint32_t current_ಸ್ಥಾನಾಂತರ;

    uint32_t c, delta;

    int32_t ಮೂಲ_ಸೂಚ್ಯಂಕ, ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t ಸ್ಥಾನಾಂತರ;
    char16_t lead, trail;
    ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2;
    int8_t ಕಿಟಕಿ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);
    offsets=p_ವಾದಗಳು->offsets;

    /* get the state machine state */
    is_Single_Byte_ಬಹುಲಕ=scsu->from_UIs_Single_Byte_ಬಹುಲಕ;
    dynamic_ಕಿಟಕಿ=scsu->from_UDynamic_ಕಿಟಕಿ;
    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ಮೂಲ_ಸೂಚ್ಯಂಕ= c==0 ? 0 : -1;
    ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    ಆದರೆ(is_Single_Byte_ಬಹುಲಕ) {
        ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            ಹೋಗು getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ<=0) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

            ಆದರೆ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ಗುರಿ++=(uint8_t)c;
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0x20) {
                ಆದರೆ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ಗುರಿ++=(uint8_t)c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((delta=c-current_ಸ್ಥಾನಾಂತರ)<=0x7f) {
                /* use the current dynamic window */
                *ಗುರಿ++=(uint8_t)(delta|0x80);
                ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                    *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                }
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE(c)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        trail=*ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ++ಮೂಲ;
                            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ಆದರೆ((delta=c-current_ಸ್ಥಾನಾಂತರ)<=0x7f) {
                    /* use the current dynamic window */
                    *ಗುರಿ++=(uint8_t)(delta|0x80);
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(SC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ಸಂಕೇತ_2-=0x200;
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_ಕಿಟಕಿ<<21)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    *ಗುರಿ++=(uint8_t)SCU;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* compress all other BMP characters */
                ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ಆದರೆ(ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ || is_ಒಳ_ಸ್ಥಾನಾಂತರ_ಕಿಟಕಿ_Or_ನೇರ(scsu->fromUDynamicOffsets[ಕಿಟಕಿ], *ಮೂಲ)) {
                        /* change to dynamic window */
                        dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(SC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+ಕಿಟಕಿ)<<8)|(c-scsu->fromUDynamicOffsets[ಕಿಟಕಿ])|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+ಕಿಟಕಿ)<<8)|(c-staticOffsets[ಕಿಟಕಿ]);
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(SD0+dynamic_ಕಿಟಕಿ)<<16)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ || (uint32_t)(*ಮೂಲ - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    c|=SCU<<16;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            ಹೋಗು getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ<=0) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            c=*ಮೂಲ++;
            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;

            ಆದರೆ ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=2) {
                    *ಗುರಿ++=(uint8_t)(c>>8);
                    *ಗುರಿ++=(uint8_t)c;
                    ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                        *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                    }
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ಆದರೆ(!(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && (uint32_t)(*ಮೂಲ-0x3400)<(0xd800-0x3400))) {
                    ಆದರೆ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        c|=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|c;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(UD0+dynamic_ಕಿಟಕಿ)<<16)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=3;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xe000) {
                /* c is a surrogate */
                ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        trail=*ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ++ಮೂಲ;
                            ++ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                /* compress supplementary character */
                ಆದರೆ( (ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && (uint32_t)(*ಮೂಲ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && lead==*ಮೂಲ && /* too lazy to check trail in same window as source[1] */
                          (ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    ಸಂಕೇತ_2-=0x200;
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_ಕಿಟಕಿ<<21)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                ಹೋಗು output_ಬೈಟುಗಳು;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ಬಹುಲಕ=is_Single_Byte_ಬಹುಲಕ;
    scsu->from_UDynamic_ಕಿಟಕಿ=dynamic_ಕಿಟಕಿ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    p_ವಾದಗಳು->offsets=offsets;
    ಹಿಂತಿರುಗಿ;

output_ಬೈಟುಗಳು:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಆದರೆ(offsets==ಶೂನ್ಯಸೂಚಕ) {
            ಆಯ್ಕೆ(length) {
                /* each branch falls through to the next one */
            ಸ್ಥಿತಿ 4:
                *ಗುರಿ++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 3:
                *ಗುರಿ++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 2:
                *ಗುರಿ++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 1:
                *ಗುರಿ++=(uint8_t)c;
                U_FALLTHROUGH;
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* will never occur */
                ನಿಲ್ಲಿಸು;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆಯ್ಕೆ(length) {
                /* each branch falls through to the next one */
            ಸ್ಥಿತಿ 4:
                *ಗುರಿ++=(uint8_t)(c>>24);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 3:
                *ಗುರಿ++=(uint8_t)(c>>16);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 2:
                *ಗುರಿ++=(uint8_t)(c>>8);
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                U_FALLTHROUGH;
            ಸ್ಥಿತಿ 1:
                *ಗುರಿ++=(uint8_t)c;
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
                U_FALLTHROUGH;
            ಪೂರ್ವನಿಯೋಜಿತ:
                /* will never occur */
                ನಿಲ್ಲಿಸು;
            }
        }
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ಮೂಲ_ಸೂಚ್ಯಂಕ=ಮುಂದಿನ_ಮೂಲ_ಸೂಚ್ಯಂಕ;
        ಹೋಗು loop;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        p=(uint8_t *)cnv->char_Error_ಬಫರ್;
        ಆಯ್ಕೆ(length) {
            /* each branch falls through to the next one */
        ಸ್ಥಿತಿ 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* will never occur */
            ನಿಲ್ಲಿಸು;
        }
        cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        ಆಯ್ಕೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            /* each branch falls through to the next one */
        ಸ್ಥಿತಿ 3:
            *ಗುರಿ++=(uint8_t)(c>>16);
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *ಗುರಿ++=(uint8_t)(c>>8);
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *ಗುರಿ++=(uint8_t)c;
            ಆದರೆ(offsets!=ಶೂನ್ಯಸೂಚಕ) {
                *offsets++=ಮೂಲ_ಸೂಚ್ಯಂಕ;
            }
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
        }

        /* target overflow */
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        c=0;
        ಹೋಗು endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_ವಾದಗಳು *p_ವಾದಗಳು,
                 UError_ಸಂಕೇತ *p_Error_ಸಂಕೇತ) {
    UConverter *cnv;
    SCSUData *scsu;
    ಸ್ಥಿರಾಂಕ char16_t *ಮೂಲ, *ಮೂಲ_ಎಲ್ಲೆ;
    uint8_t *ಗುರಿ;
    int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ;

    UBool is_Single_Byte_ಬಹುಲಕ;
    uint8_t dynamic_ಕಿಟಕಿ;
    uint32_t current_ಸ್ಥಾನಾಂತರ;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t ಸ್ಥಾನಾಂತರ;
    char16_t lead, trail;
    ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2;
    int8_t ಕಿಟಕಿ;

    /* set up the local pointers */
    cnv=p_ವಾದಗಳು->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ಮೂಲ=p_ವಾದಗಳು->ಮೂಲ;
    ಮೂಲ_ಎಲ್ಲೆ=p_ವಾದಗಳು->ಮೂಲ_ಎಲ್ಲೆ;
    ಗುರಿ=(uint8_t *)p_ವಾದಗಳು->ಗುರಿ;
    ಗುರಿ_ಸಾಮರ್ಥ್ಯ=(int32_t)(p_ವಾದಗಳು->ಗುರಿ_ಎಲ್ಲೆ-p_ವಾದಗಳು->ಗುರಿ);

    /* get the state machine state */
    is_Single_Byte_ಬಹುಲಕ=scsu->from_UIs_Single_Byte_ಬಹುಲಕ;
    dynamic_ಕಿಟಕಿ=scsu->from_UDynamic_ಕಿಟಕಿ;
    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    ಆದರೆ(is_Single_Byte_ಬಹುಲಕ) {
        ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            ಹೋಗು getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ<=0) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            c=*ಮೂಲ++;

            ಆದರೆ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ಗುರಿ++=(uint8_t)c;
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0x20) {
                ಆದರೆ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ಗುರಿ++=(uint8_t)c;
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((delta=c-current_ಸ್ಥಾನಾಂತರ)<=0x7f) {
                /* use the current dynamic window */
                *ಗುರಿ++=(uint8_t)(delta|0x80);
                --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U16_IS_SURROGATE(c)) {
                ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        trail=*ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ++ಮೂಲ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ಆದರೆ((delta=c-current_ಸ್ಥಾನಾಂತರ)<=0x7f) {
                    /* use the current dynamic window */
                    *ಗುರಿ++=(uint8_t)(delta|0x80);
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(SC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ಸಂಕೇತ_2-=0x200;
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_ಕಿಟಕಿ<<21)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    *ಗುರಿ++=(uint8_t)SCU;
                    --ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* compress all other BMP characters */
                ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ಆದರೆ(ಮೂಲ>=ಮೂಲ_ಎಲ್ಲೆ || is_ಒಳ_ಸ್ಥಾನಾಂತರ_ಕಿಟಕಿ_Or_ನೇರ(scsu->fromUDynamicOffsets[ಕಿಟಕಿ], *ಮೂಲ)) {
                        /* change to dynamic window */
                        dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(SC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+ಕಿಟಕಿ)<<8)|(c-scsu->fromUDynamicOffsets[ಕಿಟಕಿ])|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+ಕಿಟಕಿ)<<8)|(c-staticOffsets[ಕಿಟಕಿ]);
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(SD0+dynamic_ಕಿಟಕಿ)<<16)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c - 0x3400 < 0xd800 - 0x3400 &&
                           (ಮೂಲ >= ಮೂಲ_ಎಲ್ಲೆ || static_cast<uint32_t>(*ಮೂಲ - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ಬಹುಲಕ=ಸುಳ್ಳು;
                    c|=SCU<<16;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ(c!=0 && ಗುರಿ_ಸಾಮರ್ಥ್ಯ>0) {
            ಹೋಗು getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        ಆಗ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
            ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ<=0) {
                /* target is full */
                *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
                ನಿಲ್ಲಿಸು;
            }
            c=*ಮೂಲ++;

            ಆದರೆ (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ>=2) {
                    *ಗುರಿ++=(uint8_t)(c>>8);
                    *ಗುರಿ++=(uint8_t)c;
                    ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=2;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ಆದರೆ(!(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && (uint32_t)(*ಮೂಲ-0x3400)<(0xd800-0x3400))) {
                    ಆದರೆ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        c|=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|c;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=2;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                        dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                        current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                        use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                        c=((uint32_t)(UD0+dynamic_ಕಿಟಕಿ)<<16)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                        length=3;
                        ಹೋಗು output_ಬೈಟುಗಳು;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                ಹೋಗು output_ಬೈಟುಗಳು;
            } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c<0xe000) {
                /* c is a surrogate */
                ಆದರೆ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ) {
                        /* test the following code unit */
                        trail=*ಮೂಲ;
                        ಆದರೆ(U16_IS_TRAIL(trail)) {
                            ++ಮೂಲ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                            ಹೋಗು endloop;
                        }
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        /* no more input */
                        ನಿಲ್ಲಿಸು;
                    }
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ಸಂಕೇತ=U_ILLEGAL_CHAR_ಸಿಕ್ಕಿತು;
                    ಹೋಗು endloop;
                }

                /* compress supplementary character */
                ಆದರೆ( (ಕಿಟಕಿ=get_ಕಿಟಕಿ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && (uint32_t)(*ಮೂಲ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    dynamic_ಕಿಟಕಿ=ಕಿಟಕಿ;
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ];
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)(UC0+dynamic_ಕಿಟಕಿ)<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=2;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಮೂಲ<ಮೂಲ_ಎಲ್ಲೆ && lead==*ಮೂಲ && /* too lazy to check trail in same window as source[1] */
                          (ಸಂಕೇತ_2=get_Dynamic_ಸ್ಥಾನಾಂತರ(c, &ಸ್ಥಾನಾಂತರ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ಬಹುಲಕ=ನಿಜ;
                    ಸಂಕೇತ_2-=0x200;
                    dynamic_ಕಿಟಕಿ=get_ಮುಂದಿನ_Dynamic_ಕಿಟಕಿ(scsu);
                    current_ಸ್ಥಾನಾಂತರ=scsu->fromUDynamicOffsets[dynamic_ಕಿಟಕಿ]=ಸ್ಥಾನಾಂತರ;
                    use_Dynamic_ಕಿಟಕಿ(scsu, dynamic_ಕಿಟಕಿ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_ಕಿಟಕಿ<<21)|((uint32_t)ಸಂಕೇತ_2<<8)|(c-current_ಸ್ಥಾನಾಂತರ)|0x80;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                } ಇಲ್ಲದಿದ್ದರೆ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    ಹೋಗು output_ಬೈಟುಗಳು;
                }
            } ಇಲ್ಲದಿದ್ದರೆ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                ಹೋಗು output_ಬೈಟುಗಳು;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ಬಹುಲಕ=is_Single_Byte_ಬಹುಲಕ;
    scsu->from_UDynamic_ಕಿಟಕಿ=dynamic_ಕಿಟಕಿ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_ವಾದಗಳು->ಮೂಲ=ಮೂಲ;
    p_ವಾದಗಳು->ಗುರಿ=(ಅಕ್ಷರ *)ಗುರಿ;
    ಹಿಂತಿರುಗಿ;

output_ಬೈಟುಗಳು:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ಆದರೆ(length<=ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
        ಆಯ್ಕೆ(length) {
            /* each branch falls through to the next one */
        ಸ್ಥಿತಿ 4:
            *ಗುರಿ++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 3:
            *ಗುರಿ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *ಗುರಿ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *ಗುರಿ++=(uint8_t)c;
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* will never occur */
            ನಿಲ್ಲಿಸು;
        }
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ಹೋಗು loop;
    } ಇಲ್ಲದಿದ್ದರೆ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ಗುರಿ_ಸಾಮರ್ಥ್ಯ;
        p=(uint8_t *)cnv->char_Error_ಬಫರ್;
        ಆಯ್ಕೆ(length) {
            /* each branch falls through to the next one */
        ಸ್ಥಿತಿ 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            /* will never occur */
            ನಿಲ್ಲಿಸು;
        }
        cnv->char_Error_ಬಫರ್_ಉದ್ದಳತೆ=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        ಆಯ್ಕೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
            /* each branch falls through to the next one */
        ಸ್ಥಿತಿ 3:
            *ಗುರಿ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 2:
            *ಗುರಿ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        ಸ್ಥಿತಿ 1:
            *ಗುರಿ++=(uint8_t)c;
            U_FALLTHROUGH;
        ಪೂರ್ವನಿಯೋಜಿತ:
            ನಿಲ್ಲಿಸು;
        }

        /* target overflow */
        ಗುರಿ_ಸಾಮರ್ಥ್ಯ=0;
        *p_Error_ಸಂಕೇತ=U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR;
        c=0;
        ಹೋಗು endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *  U_CALLCONV
_SCSUGetName(ಸ್ಥಿರಾಂಕ UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ಆಯ್ಕೆ(scsu->locale) {
    ಸ್ಥಿತಿ l_ja:
        ಹಿಂತಿರುಗಿ "SCSU,locale=ja";
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ "SCSU";
    }
}

/* structure for SafeClone calculations */
ರಚನೆ cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

ಸ್ಥಿರ UConverter *  U_CALLCONV
_SCSUSafeClone(ಸ್ಥಿರಾಂಕ UConverter *cnv, 
               ಶೂನ್ಯ *ರಾಶಿ_ಬಫರ್, 
               int32_t *p_ಬಫರ್_ಗಾತ್ರ, 
               UError_ಸಂಕೇತ *ಸ್ಥಿತಿ_2)
{
    ರಚನೆ cloneSCSUStruct * localClone;
    int32_t ಬಫರ್_ಗಾತ್ರ_Needed = ಗಾತ್ರ(ರಚನೆ cloneSCSUStruct);

    ಆದರೆ (U_FAILURE(*ಸ್ಥಿತಿ_2)){
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    ಆದರೆ (*p_ಬಫರ್_ಗಾತ್ರ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_ಬಫರ್_ಗಾತ್ರ = ಬಫರ್_ಗಾತ್ರ_Needed;
        ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
    }

    localClone = (ರಚನೆ cloneSCSUStruct *)ರಾಶಿ_ಬಫರ್;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, ಗಾತ್ರ(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ನಿಜ;

    ಹಿಂತಿರುಗಿ &localClone->cnv;
}
U_CDECL_ಕೊನೆ

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    ಶೂನ್ಯಸೂಚಕ,

    ಶೂನ್ಯಸೂಚಕ,
    _SCSUGetName,
    ಶೂನ್ಯಸೂಚಕ,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_ಗಣ,
    ಶೂನ್ಯಸೂಚಕ,
    ಶೂನ್ಯಸೂಚಕ
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ UConverter_Static_ದತ್ತಾಂಶ SCSUStatic_ದತ್ತಾಂಶ={
    ಗಾತ್ರ(UConverter_Static_ದತ್ತಾಂಶ),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    ಸುಳ್ಳು, ಸುಳ್ಳು,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ಸ್ಥಿರಾಂಕ UConverter_Shared_ದತ್ತಾಂಶ _SCSUData=
        UCNV_IMMUTABLE_SHARED_ದತ್ತಾಂಶ_INITIALIZER(&SCSUStatic_ದತ್ತಾಂಶ, &_SCSUImpl);

#ಆದರೆ_ಅಂತ್ಯ
