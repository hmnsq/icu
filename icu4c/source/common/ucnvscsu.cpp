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

#شامل "unicode/utypes.h"

#اگر !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#شامل "unicode/ucnv.h"
#شامل "unicode/ucnv_cb.h"
#شامل "unicode/utf16.h"
#شامل "ucnv_bld.h"
#شامل "ucnv_cnv.h"
#شامل "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
شمار {
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

شمار {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_حد=0x68,
    gap_انحراف=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_آغاز=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_حد=0xF9
};

/* constant offsets for the 8 static windows */
ساکن ثابت uint32_t staticOffsets[8]={
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
ساکن ثابت uint32_t initialDynamicOffsets[8]={
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
ساکن ثابت uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
شمار {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

قسمتعریف ساخت SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t مقصد_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool مقصد_UIs_Single_Byte_منوال;
    uint8_t مقصد_UState;
    int8_t مقصد_UQuote_کھڑکی, مقصد_UDynamic_کھڑکی;
    uint8_t مقصد_UByte_One;
    uint8_t مقصد_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_منوال;
    int8_t from_UDynamic_کھڑکی;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t اگلا_کھڑکی_Use_اشاریہ;
    int8_t کھڑکی_Use[8];
} SCSUData;

ساکن ثابت int8_t initial_کھڑکی_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ساکن ثابت int8_t initial_کھڑکی_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

شمار {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
ساکن خالی U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    اگر(choice<=UCNV_RESET_مقصد_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->مقصد_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->مقصد_UIs_Single_Byte_منوال=سچ;
        scsu->مقصد_UState=readCommand;
        scsu->مقصد_UQuote_کھڑکی=scsu->مقصد_UDynamic_کھڑکی=0;
        scsu->مقصد_UByte_One=0;

        cnv->مقصد_ULength=0;
    }
    اگر(choice!=UCNV_RESET_مقصد_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_منوال=سچ;
        scsu->from_UDynamic_کھڑکی=0;

        scsu->اگلا_کھڑکی_Use_اشاریہ=0;
        انتخاب(scsu->locale) {
        حالت l_ja:
            uprv_memcpy(scsu->کھڑکی_Use, initial_کھڑکی_Use_ja, 8);
            توڑو;
        طےشدہ:
            uprv_memcpy(scsu->کھڑکی_Use, initial_کھڑکی_Use, 8);
            توڑو;
        }

        cnv->fromUChar32=0;
    }
}

ساکن خالی U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_بھرو_دلائل *p_دلائل,
          UError_رمز *p_Error_رمز) {
    ثابت حرف *locale=p_دلائل->locale;
    اگر(p_دلائل->onlyTestIsLoadable) {
        واپس;
    }
    cnv->extraInfo=uprv_malloc(حجم(SCSUData));
    اگر(cnv->extraInfo!=خالی_اشاریہ) {
        اگر(locale!=خالی_اشاریہ && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } ورنہ {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } ورنہ {
        *p_Error_رمز=U_حافظہ_تخصیص_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ساکن خالی U_CALLCONV
_SCSUClose(UConverter *cnv) {
    اگر(cnv->extraInfo!=خالی_اشاریہ) {
        اگر(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=خالی_اشاریہ;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ساکن خالی U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_مقصد_Unicode_دلائل *p_دلائل,
                          UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    int32_t *offsets;
    UBool is_Single_Byte_منوال;
    uint8_t state, byteOne;
    int8_t quote_کھڑکی, dynamic_کھڑکی;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;
    offsets=p_دلائل->offsets;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->مقصد_UIs_Single_Byte_منوال;
    state=scsu->مقصد_UState;
    quote_کھڑکی=scsu->مقصد_UQuote_کھڑکی;
    dynamic_کھڑکی=scsu->مقصد_UDynamic_کھڑکی;
    byteOne=scsu->مقصد_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ=state==readCommand ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

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
    اگر(is_Single_Byte_منوال) {
        /* fast path for single-byte mode */
        اگر(state==readCommand) {
fastSingle:
            جب(ماخذ<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی && (b=*ماخذ)>=0x20) {
                ++ماخذ;
                ++اگلا_ماخذ_اشاریہ;
                اگر(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ہدف++=(char16_t)b;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                } ورنہ {
                    /* write from dynamic window */
                    uint32_t c=scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]+(b&0x7f);
                    اگر(c<=0xffff) {
                        *ہدف++=(char16_t)c;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                        }
                    } ورنہ {
                        /* output surrogate pair */
                        *ہدف++=(char16_t)(0xd7c0+(c>>10));
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=(char16_t)(0xdc00|(c&0x3ff));
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                *offsets++=ماخذ_اشاریہ;
                            }
                        } ورنہ {
                            /* target overflow */
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            جاؤ endloop;
                        }
                    }
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_منوال:
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف>=ہدف_حد_بندی) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            b=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;
            انتخاب(state) {
            حالت readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                اگر((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ہدف++=(char16_t)b;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                    جاؤ fastSingle;
                } ورنہ اگر(SC0<=b) {
                    اگر(b<=SC7) {
                        dynamic_کھڑکی=(int8_t)(b-SC0);
                        ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                        جاؤ fastSingle;
                    } ورنہ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کھڑکی=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ورنہ اگر(/* SQ0<=b && */ b<=SQ7) {
                    quote_کھڑکی=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ورنہ اگر(b==SDX) {
                    state=definePairOne;
                } ورنہ اگر(b==SQU) {
                    state=quotePairOne;
                } ورنہ اگر(b==SCU) {
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                    is_Single_Byte_منوال=جھوٹ;
                    جاؤ fastUnicode;
                } ورنہ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    جاؤ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->مقصد_UBytes[0]=b;
                cnv->مقصد_ULength=1;
                توڑو;
            حالت quotePairOne:
                byteOne=b;
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=quotePairTwo;
                توڑو;
            حالت quotePairTwo:
                *ہدف++=(char16_t)((byteOne<<8)|b);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                state=readCommand;
                جاؤ fastSingle;
            حالت quoteOne:
                اگر(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ہدف++=(char16_t)(staticOffsets[quote_کھڑکی]+b);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                } ورنہ {
                    /* write from dynamic window */
                    uint32_t c=scsu->مقصد_UDynamic_Offsets[quote_کھڑکی]+(b&0x7f);
                    اگر(c<=0xffff) {
                        *ہدف++=(char16_t)c;
                        اگر(offsets!=خالی_اشاریہ) {
                            *offsets++=ماخذ_اشاریہ;
                        }
                    } ورنہ {
                        /* output surrogate pair */
                        *ہدف++=(char16_t)(0xd7c0+(c>>10));
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=(char16_t)(0xdc00|(c&0x3ff));
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                                *offsets++=ماخذ_اشاریہ;
                            }
                        } ورنہ {
                            /* target overflow */
                            اگر(offsets!=خالی_اشاریہ) {
                                *offsets++=ماخذ_اشاریہ;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            جاؤ endloop;
                        }
                    }
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                state=readCommand;
                جاؤ fastSingle;
            حالت definePairOne:
                dynamic_کھڑکی=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=definePairTwo;
                توڑو;
            حالت definePairTwo:
                scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=0x10000+(byteOne<<15UL | b<<7UL);
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                state=readCommand;
                جاؤ fastSingle;
            حالت defineOne:
                اگر(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->مقصد_UBytes[1]=b;
                    cnv->مقصد_ULength=2;
                    جاؤ endloop;
                } ورنہ اگر(b<gap_حد) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=b<<7UL;
                } ورنہ اگر((uint8_t)(b-gap_حد)<(reserved_آغاز-gap_حد)) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=(b<<7UL)+gap_انحراف;
                } ورنہ اگر(b>=fixed_حد) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=fixedOffsets[b-fixed_حد];
                } ورنہ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->مقصد_UBytes[1]=b;
                    cnv->مقصد_ULength=2;
                    جاؤ endloop;
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                state=readCommand;
                جاؤ fastSingle;
            }
        }
    } ورنہ {
        /* fast path for Unicode mode */
        اگر(state==readCommand) {
fastUnicode:
            جب(ماخذ+1<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی && (uint8_t)((b=*ماخذ)-UC0)>(Urs-UC0)) {
                *ہدف++=(char16_t)((b<<8)|ماخذ[1]);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                اگلا_ماخذ_اشاریہ+=2;
                ماخذ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف>=ہدف_حد_بندی) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            b=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;
            انتخاب(state) {
            حالت readCommand:
                اگر((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=quotePairTwo;
                } ورنہ اگر(/* UC0<=b && */ b<=UC7) {
                    dynamic_کھڑکی=(int8_t)(b-UC0);
                    ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                    is_Single_Byte_منوال=سچ;
                    جاؤ fastSingle;
                } ورنہ اگر(/* UD0<=b && */ b<=UD7) {
                    dynamic_کھڑکی=(int8_t)(b-UD0);
                    is_Single_Byte_منوال=سچ;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=defineOne;
                    جاؤ single_Byte_منوال;
                } ورنہ اگر(b==UDX) {
                    is_Single_Byte_منوال=سچ;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=definePairOne;
                    جاؤ single_Byte_منوال;
                } ورنہ اگر(b==UQU) {
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=quotePairOne;
                } ورنہ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    جاؤ endloop;
                }
                توڑو;
            حالت quotePairOne:
                byteOne=b;
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=quotePairTwo;
                توڑو;
            حالت quotePairTwo:
                *ہدف++=(char16_t)((byteOne<<8)|b);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
                state=readCommand;
                جاؤ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    اگر(U_FAILURE(*p_Error_رمز) && *p_Error_رمز!=U_بفر_حد_سے_تجاوز_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ورنہ اگر(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->مقصد_ULength=0;
    }
    scsu->مقصد_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->مقصد_UState=state;
    scsu->مقصد_UQuote_کھڑکی=quote_کھڑکی;
    scsu->مقصد_UDynamic_کھڑکی=dynamic_کھڑکی;
    scsu->مقصد_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
    p_دلائل->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساکن خالی U_CALLCONV
_SCSUToUnicode(UConverter_مقصد_Unicode_دلائل *p_دلائل,
               UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *ماخذ, *ماخذ_حد_بندی;
    char16_t *ہدف;
    ثابت char16_t *ہدف_حد_بندی;
    UBool is_Single_Byte_منوال;
    uint8_t state, byteOne;
    int8_t quote_کھڑکی, dynamic_کھڑکی;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ماخذ=(ثابت uint8_t *)p_دلائل->ماخذ;
    ماخذ_حد_بندی=(ثابت uint8_t *)p_دلائل->ماخذ_حد_بندی;
    ہدف=p_دلائل->ہدف;
    ہدف_حد_بندی=p_دلائل->ہدف_حد_بندی;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->مقصد_UIs_Single_Byte_منوال;
    state=scsu->مقصد_UState;
    quote_کھڑکی=scsu->مقصد_UQuote_کھڑکی;
    dynamic_کھڑکی=scsu->مقصد_UDynamic_کھڑکی;
    byteOne=scsu->مقصد_UByte_One;

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
    اگر(is_Single_Byte_منوال) {
        /* fast path for single-byte mode */
        اگر(state==readCommand) {
fastSingle:
            جب(ماخذ<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی && (b=*ماخذ)>=0x20) {
                ++ماخذ;
                اگر(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *ہدف++=(char16_t)b;
                } ورنہ {
                    /* write from dynamic window */
                    uint32_t c=scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]+(b&0x7f);
                    اگر(c<=0xffff) {
                        *ہدف++=(char16_t)c;
                    } ورنہ {
                        /* output surrogate pair */
                        *ہدف++=(char16_t)(0xd7c0+(c>>10));
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=(char16_t)(0xdc00|(c&0x3ff));
                        } ورنہ {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            جاؤ endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_منوال:
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف>=ہدف_حد_بندی) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            b=*ماخذ++;
            انتخاب(state) {
            حالت readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                اگر((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ہدف++=(char16_t)b;
                    جاؤ fastSingle;
                } ورنہ اگر(SC0<=b) {
                    اگر(b<=SC7) {
                        dynamic_کھڑکی=(int8_t)(b-SC0);
                        جاؤ fastSingle;
                    } ورنہ /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کھڑکی=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } ورنہ اگر(/* SQ0<=b && */ b<=SQ7) {
                    quote_کھڑکی=(int8_t)(b-SQ0);
                    state=quoteOne;
                } ورنہ اگر(b==SDX) {
                    state=definePairOne;
                } ورنہ اگر(b==SQU) {
                    state=quotePairOne;
                } ورنہ اگر(b==SCU) {
                    is_Single_Byte_منوال=جھوٹ;
                    جاؤ fastUnicode;
                } ورنہ /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    جاؤ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->مقصد_UBytes[0]=b;
                cnv->مقصد_ULength=1;
                توڑو;
            حالت quotePairOne:
                byteOne=b;
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=quotePairTwo;
                توڑو;
            حالت quotePairTwo:
                *ہدف++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                جاؤ fastSingle;
            حالت quoteOne:
                اگر(b<0x80) {
                    /* all static offsets are in the BMP */
                    *ہدف++=(char16_t)(staticOffsets[quote_کھڑکی]+b);
                } ورنہ {
                    /* write from dynamic window */
                    uint32_t c=scsu->مقصد_UDynamic_Offsets[quote_کھڑکی]+(b&0x7f);
                    اگر(c<=0xffff) {
                        *ہدف++=(char16_t)c;
                    } ورنہ {
                        /* output surrogate pair */
                        *ہدف++=(char16_t)(0xd7c0+(c>>10));
                        اگر(ہدف<ہدف_حد_بندی) {
                            *ہدف++=(char16_t)(0xdc00|(c&0x3ff));
                        } ورنہ {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمبائی=1;
                            *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                            جاؤ endloop;
                        }
                    }
                }
                state=readCommand;
                جاؤ fastSingle;
            حالت definePairOne:
                dynamic_کھڑکی=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=definePairTwo;
                توڑو;
            حالت definePairTwo:
                scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                جاؤ fastSingle;
            حالت defineOne:
                اگر(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->مقصد_UBytes[1]=b;
                    cnv->مقصد_ULength=2;
                    جاؤ endloop;
                } ورنہ اگر(b<gap_حد) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=b<<7UL;
                } ورنہ اگر((uint8_t)(b-gap_حد)<(reserved_آغاز-gap_حد)) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=(b<<7UL)+gap_انحراف;
                } ورنہ اگر(b>=fixed_حد) {
                    scsu->مقصد_UDynamic_Offsets[dynamic_کھڑکی]=fixedOffsets[b-fixed_حد];
                } ورنہ {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->مقصد_UBytes[1]=b;
                    cnv->مقصد_ULength=2;
                    جاؤ endloop;
                }
                state=readCommand;
                جاؤ fastSingle;
            }
        }
    } ورنہ {
        /* fast path for Unicode mode */
        اگر(state==readCommand) {
fastUnicode:
            جب(ماخذ+1<ماخذ_حد_بندی && ہدف<ہدف_حد_بندی && (uint8_t)((b=*ماخذ)-UC0)>(Urs-UC0)) {
                *ہدف++=(char16_t)((b<<8)|ماخذ[1]);
                ماخذ+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف>=ہدف_حد_بندی) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            b=*ماخذ++;
            انتخاب(state) {
            حالت readCommand:
                اگر((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=quotePairTwo;
                } ورنہ اگر(/* UC0<=b && */ b<=UC7) {
                    dynamic_کھڑکی=(int8_t)(b-UC0);
                    is_Single_Byte_منوال=سچ;
                    جاؤ fastSingle;
                } ورنہ اگر(/* UD0<=b && */ b<=UD7) {
                    dynamic_کھڑکی=(int8_t)(b-UD0);
                    is_Single_Byte_منوال=سچ;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=defineOne;
                    جاؤ single_Byte_منوال;
                } ورنہ اگر(b==UDX) {
                    is_Single_Byte_منوال=سچ;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=definePairOne;
                    جاؤ single_Byte_منوال;
                } ورنہ اگر(b==UQU) {
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    state=quotePairOne;
                } ورنہ /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    cnv->مقصد_UBytes[0]=b;
                    cnv->مقصد_ULength=1;
                    جاؤ endloop;
                }
                توڑو;
            حالت quotePairOne:
                byteOne=b;
                cnv->مقصد_UBytes[1]=b;
                cnv->مقصد_ULength=2;
                state=quotePairTwo;
                توڑو;
            حالت quotePairTwo:
                *ہدف++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                جاؤ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    اگر(U_FAILURE(*p_Error_رمز) && *p_Error_رمز!=U_بفر_حد_سے_تجاوز_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } ورنہ اگر(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->مقصد_ULength=0;
    }
    scsu->مقصد_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->مقصد_UState=state;
    scsu->مقصد_UQuote_کھڑکی=quote_کھڑکی;
    scsu->مقصد_UDynamic_کھڑکی=dynamic_کھڑکی;
    scsu->مقصد_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلائل->ماخذ=(ثابت حرف *)ماخذ;
    p_دلائل->ہدف=ہدف;
}
U_CDECL_انتہا
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
ساکن int8_t
get_کھڑکی(ثابت uint32_t offsets[8], uint32_t c) {
    صحیح i;
    برائے(i=0; i<8; ++i) {
        اگر (c - offsets[i] <= 0x7f) {
            واپس static_cast<int8_t>(i);
        }
    }
    واپس -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
ساکن UBool
is_اندر_انحراف_کھڑکی_Or_براہ_راست(uint32_t انحراف, uint32_t c) {
    واپس c<=انحراف+0x7f &&
          (c>=انحراف || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ساکن int8_t
get_اگلا_Dynamic_کھڑکی(SCSUData *scsu) {
    int8_t کھڑکی=scsu->کھڑکی_Use[scsu->اگلا_کھڑکی_Use_اشاریہ];
    اگر(++scsu->اگلا_کھڑکی_Use_اشاریہ==8) {
        scsu->اگلا_کھڑکی_Use_اشاریہ=0;
    }
    واپس کھڑکی;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ساکن خالی
use_Dynamic_کھڑکی(SCSUData *scsu, int8_t کھڑکی) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    صحیح i, j;

    i=scsu->اگلا_کھڑکی_Use_اشاریہ;
    کرو {
        اگر(--i<0) {
            i=7;
        }
    } جب(scsu->کھڑکی_Use[i]!=کھڑکی);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    اگر(j==8) {
        j=0;
    }
    جب(j!=scsu->اگلا_کھڑکی_Use_اشاریہ) {
        scsu->کھڑکی_Use[i]=scsu->کھڑکی_Use[j];
        i=j;
        اگر(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->کھڑکی_Use[i]=کھڑکی;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
ساکن صحیح
get_Dynamic_انحراف(uint32_t c, uint32_t *p_انحراف) {
    صحیح i;

    برائے(i=0; i<7; ++i) {
        اگر (c - fixedOffsets[i] <= 0x7f) {
            *p_انحراف=fixedOffsets[i];
            واپس 0xf9+i;
        }
    }

    اگر(c<0x80) {
        /* No dynamic window for US-ASCII. */
        واپس -1;
    } ورنہ اگر(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_انحراف=c&0x7fffff80;
        واپس static_cast<صحیح>(c >> 7);
    } ورنہ اگر(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_انحراف=c&0x7fffff80;
        واپس static_cast<صحیح>((c - gap_انحراف) >> 7);
    } ورنہ {
        واپس -1;
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

ساکن خالی U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_دلائل *p_دلائل,
                            UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;
    int32_t *offsets;

    UBool is_Single_Byte_منوال;
    uint8_t dynamic_کھڑکی;
    uint32_t current_انحراف;

    uint32_t c, delta;

    int32_t ماخذ_اشاریہ, اگلا_ماخذ_اشاریہ;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t انحراف;
    char16_t lead, trail;
    صحیح رمز;
    int8_t کھڑکی;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);
    offsets=p_دلائل->offsets;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->from_UIs_Single_Byte_منوال;
    dynamic_کھڑکی=scsu->from_UDynamic_کھڑکی;
    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ماخذ_اشاریہ= c==0 ? 0 : -1;
    اگلا_ماخذ_اشاریہ=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    اگر(is_Single_Byte_منوال) {
        اگر(c!=0 && ہدف_استعداد>0) {
            جاؤ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف_استعداد<=0) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;

            اگر((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ہدف++=(uint8_t)c;
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                --ہدف_استعداد;
            } ورنہ اگر(c<0x20) {
                اگر((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ہدف++=(uint8_t)c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    --ہدف_استعداد;
                } ورنہ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر((delta=c-current_انحراف)<=0x7f) {
                /* use the current dynamic window */
                *ہدف++=(uint8_t)(delta|0x80);
                اگر(offsets!=خالی_اشاریہ) {
                    *offsets++=ماخذ_اشاریہ;
                }
                --ہدف_استعداد;
            } ورنہ اگر(U16_IS_SURROGATE(c)) {
                اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    اگر(ماخذ<ماخذ_حد_بندی) {
                        /* test the following code unit */
                        trail=*ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ++ماخذ;
                            ++اگلا_ماخذ_اشاریہ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                    } ورنہ {
                        /* no more input */
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                اگر((delta=c-current_انحراف)<=0x7f) {
                    /* use the current dynamic window */
                    *ہدف++=(uint8_t)(delta|0x80);
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    --ہدف_استعداد;
                } ورنہ اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کھڑکی=کھڑکی;
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    رمز-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=4;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_منوال=جھوٹ;
                    *ہدف++=(uint8_t)SCU;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                    }
                    --ہدف_استعداد;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                جاؤ output_بائٹیں;
            } ورنہ اگر(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                جاؤ output_بائٹیں;
            } ورنہ {
                /* compress all other BMP characters */
                اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    اگر(ماخذ>=ماخذ_حد_بندی || is_اندر_انحراف_کھڑکی_Or_براہ_راست(scsu->fromUDynamicOffsets[کھڑکی], *ماخذ)) {
                        /* change to dynamic window */
                        dynamic_کھڑکی=کھڑکی;
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-scsu->fromUDynamicOffsets[کھڑکی])|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    }
                } ورنہ اگر((کھڑکی=get_کھڑکی(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-staticOffsets[کھڑکی]);
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SD0+dynamic_کھڑکی)<<16)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=3;
                    جاؤ output_بائٹیں;
                } ورنہ اگر ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (ماخذ >= ماخذ_حد_بندی || (uint32_t)(*ماخذ - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_منوال=جھوٹ;
                    c|=SCU<<16;
                    length=3;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    جاؤ output_بائٹیں;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
        }
    } ورنہ {
        اگر(c!=0 && ہدف_استعداد>0) {
            جاؤ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف_استعداد<=0) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            c=*ماخذ++;
            ++اگلا_ماخذ_اشاریہ;

            اگر ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                اگر(ہدف_استعداد>=2) {
                    *ہدف++=(uint8_t)(c>>8);
                    *ہدف++=(uint8_t)c;
                    اگر(offsets!=خالی_اشاریہ) {
                        *offsets++=ماخذ_اشاریہ;
                        *offsets++=ماخذ_اشاریہ;
                    }
                    ہدف_استعداد-=2;
                } ورنہ {
                    length=2;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                اگر(!(ماخذ<ماخذ_حد_بندی && (uint32_t)(*ماخذ-0x3400)<(0xd800-0x3400))) {
                    اگر (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_منوال=سچ;
                        c|=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|c;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=کھڑکی;
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UD0+dynamic_کھڑکی)<<16)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                        length=3;
                        جاؤ output_بائٹیں;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                جاؤ output_بائٹیں;
            } ورنہ اگر(c<0xe000) {
                /* c is a surrogate */
                اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    اگر(ماخذ<ماخذ_حد_بندی) {
                        /* test the following code unit */
                        trail=*ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ++ماخذ;
                            ++اگلا_ماخذ_اشاریہ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                    } ورنہ {
                        /* no more input */
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                /* compress supplementary character */
                اگر( (کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ماخذ<ماخذ_حد_بندی && (uint32_t)(*ماخذ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_منوال=سچ;
                    dynamic_کھڑکی=کھڑکی;
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر(ماخذ<ماخذ_حد_بندی && lead==*ماخذ && /* too lazy to check trail in same window as source[1] */
                          (رمز=get_Dynamic_انحراف(c, &انحراف))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_منوال=سچ;
                    رمز-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=4;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    جاؤ output_بائٹیں;
                }
            } ورنہ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                جاؤ output_بائٹیں;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->from_UDynamic_کھڑکی=dynamic_کھڑکی;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    p_دلائل->offsets=offsets;
    واپس;

output_بائٹیں:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    اگر(length<=ہدف_استعداد) {
        اگر(offsets==خالی_اشاریہ) {
            انتخاب(length) {
                /* each branch falls through to the next one */
            حالت 4:
                *ہدف++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            حالت 3:
                *ہدف++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            حالت 2:
                *ہدف++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            حالت 1:
                *ہدف++=(uint8_t)c;
                U_FALLTHROUGH;
            طےشدہ:
                /* will never occur */
                توڑو;
            }
        } ورنہ {
            انتخاب(length) {
                /* each branch falls through to the next one */
            حالت 4:
                *ہدف++=(uint8_t)(c>>24);
                *offsets++=ماخذ_اشاریہ;
                U_FALLTHROUGH;
            حالت 3:
                *ہدف++=(uint8_t)(c>>16);
                *offsets++=ماخذ_اشاریہ;
                U_FALLTHROUGH;
            حالت 2:
                *ہدف++=(uint8_t)(c>>8);
                *offsets++=ماخذ_اشاریہ;
                U_FALLTHROUGH;
            حالت 1:
                *ہدف++=(uint8_t)c;
                *offsets++=ماخذ_اشاریہ;
                U_FALLTHROUGH;
            طےشدہ:
                /* will never occur */
                توڑو;
            }
        }
        ہدف_استعداد-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ماخذ_اشاریہ=اگلا_ماخذ_اشاریہ;
        جاؤ loop;
    } ورنہ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ہدف_استعداد;
        p=(uint8_t *)cnv->char_Error_بفر;
        انتخاب(length) {
            /* each branch falls through to the next one */
        حالت 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالت 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالت 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالت 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        طےشدہ:
            /* will never occur */
            توڑو;
        }
        cnv->char_Error_بفر_لمبائی=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        انتخاب(ہدف_استعداد) {
            /* each branch falls through to the next one */
        حالت 3:
            *ہدف++=(uint8_t)(c>>16);
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            U_FALLTHROUGH;
        حالت 2:
            *ہدف++=(uint8_t)(c>>8);
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            U_FALLTHROUGH;
        حالت 1:
            *ہدف++=(uint8_t)c;
            اگر(offsets!=خالی_اشاریہ) {
                *offsets++=ماخذ_اشاریہ;
            }
            U_FALLTHROUGH;
        طےشدہ:
            توڑو;
        }

        /* target overflow */
        ہدف_استعداد=0;
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        c=0;
        جاؤ endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساکن خالی U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_دلائل *p_دلائل,
                 UError_رمز *p_Error_رمز) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *ماخذ, *ماخذ_حد_بندی;
    uint8_t *ہدف;
    int32_t ہدف_استعداد;

    UBool is_Single_Byte_منوال;
    uint8_t dynamic_کھڑکی;
    uint32_t current_انحراف;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t انحراف;
    char16_t lead, trail;
    صحیح رمز;
    int8_t کھڑکی;

    /* set up the local pointers */
    cnv=p_دلائل->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ماخذ=p_دلائل->ماخذ;
    ماخذ_حد_بندی=p_دلائل->ماخذ_حد_بندی;
    ہدف=(uint8_t *)p_دلائل->ہدف;
    ہدف_استعداد=(int32_t)(p_دلائل->ہدف_حد_بندی-p_دلائل->ہدف);

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->from_UIs_Single_Byte_منوال;
    dynamic_کھڑکی=scsu->from_UDynamic_کھڑکی;
    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    اگر(is_Single_Byte_منوال) {
        اگر(c!=0 && ہدف_استعداد>0) {
            جاؤ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف_استعداد<=0) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            c=*ماخذ++;

            اگر((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *ہدف++=(uint8_t)c;
                --ہدف_استعداد;
            } ورنہ اگر(c<0x20) {
                اگر((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *ہدف++=(uint8_t)c;
                    --ہدف_استعداد;
                } ورنہ {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر((delta=c-current_انحراف)<=0x7f) {
                /* use the current dynamic window */
                *ہدف++=(uint8_t)(delta|0x80);
                --ہدف_استعداد;
            } ورنہ اگر(U16_IS_SURROGATE(c)) {
                اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    اگر(ماخذ<ماخذ_حد_بندی) {
                        /* test the following code unit */
                        trail=*ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ++ماخذ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                    } ورنہ {
                        /* no more input */
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                اگر((delta=c-current_انحراف)<=0x7f) {
                    /* use the current dynamic window */
                    *ہدف++=(uint8_t)(delta|0x80);
                    --ہدف_استعداد;
                } ورنہ اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کھڑکی=کھڑکی;
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    رمز-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=4;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_منوال=جھوٹ;
                    *ہدف++=(uint8_t)SCU;
                    --ہدف_استعداد;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                جاؤ output_بائٹیں;
            } ورنہ اگر(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                جاؤ output_بائٹیں;
            } ورنہ {
                /* compress all other BMP characters */
                اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    اگر(ماخذ>=ماخذ_حد_بندی || is_اندر_انحراف_کھڑکی_Or_براہ_راست(scsu->fromUDynamicOffsets[کھڑکی], *ماخذ)) {
                        /* change to dynamic window */
                        dynamic_کھڑکی=کھڑکی;
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-scsu->fromUDynamicOffsets[کھڑکی])|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    }
                } ورنہ اگر((کھڑکی=get_کھڑکی(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-staticOffsets[کھڑکی]);
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SD0+dynamic_کھڑکی)<<16)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=3;
                    جاؤ output_بائٹیں;
                } ورنہ اگر (c - 0x3400 < 0xd800 - 0x3400 &&
                           (ماخذ >= ماخذ_حد_بندی || static_cast<uint32_t>(*ماخذ - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_منوال=جھوٹ;
                    c|=SCU<<16;
                    length=3;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    جاؤ output_بائٹیں;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } ورنہ {
        اگر(c!=0 && ہدف_استعداد>0) {
            جاؤ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        جب(ماخذ<ماخذ_حد_بندی) {
            اگر(ہدف_استعداد<=0) {
                /* target is full */
                *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
                توڑو;
            }
            c=*ماخذ++;

            اگر (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                اگر(ہدف_استعداد>=2) {
                    *ہدف++=(uint8_t)(c>>8);
                    *ہدف++=(uint8_t)c;
                    ہدف_استعداد-=2;
                } ورنہ {
                    length=2;
                    جاؤ output_بائٹیں;
                }
            } ورنہ اگر (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                اگر(!(ماخذ<ماخذ_حد_بندی && (uint32_t)(*ماخذ-0x3400)<(0xd800-0x3400))) {
                    اگر (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_منوال=سچ;
                        c|=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|c;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ اگر((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=کھڑکی;
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                        length=2;
                        جاؤ output_بائٹیں;
                    } ورنہ اگر((رمز=get_Dynamic_انحراف(c, &انحراف))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                        current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UD0+dynamic_کھڑکی)<<16)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                        length=3;
                        جاؤ output_بائٹیں;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                جاؤ output_بائٹیں;
            } ورنہ اگر(c<0xe000) {
                /* c is a surrogate */
                اگر(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    اگر(ماخذ<ماخذ_حد_بندی) {
                        /* test the following code unit */
                        trail=*ماخذ;
                        اگر(U16_IS_TRAIL(trail)) {
                            ++ماخذ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } ورنہ {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                            جاؤ endloop;
                        }
                    } ورنہ {
                        /* no more input */
                        توڑو;
                    }
                } ورنہ {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_رمز=U_ILLEGAL_CHAR_ملا;
                    جاؤ endloop;
                }

                /* compress supplementary character */
                اگر( (کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ماخذ<ماخذ_حد_بندی && (uint32_t)(*ماخذ-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_منوال=سچ;
                    dynamic_کھڑکی=کھڑکی;
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_انحراف)|0x80;
                    length=2;
                    جاؤ output_بائٹیں;
                } ورنہ اگر(ماخذ<ماخذ_حد_بندی && lead==*ماخذ && /* too lazy to check trail in same window as source[1] */
                          (رمز=get_Dynamic_انحراف(c, &انحراف))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_منوال=سچ;
                    رمز-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_انحراف=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=انحراف;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)رمز<<8)|(c-current_انحراف)|0x80;
                    length=4;
                    جاؤ output_بائٹیں;
                } ورنہ {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    جاؤ output_بائٹیں;
                }
            } ورنہ /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                جاؤ output_بائٹیں;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->from_UDynamic_کھڑکی=dynamic_کھڑکی;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلائل->ماخذ=ماخذ;
    p_دلائل->ہدف=(حرف *)ہدف;
    واپس;

output_بائٹیں:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    اگر(length<=ہدف_استعداد) {
        انتخاب(length) {
            /* each branch falls through to the next one */
        حالت 4:
            *ہدف++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالت 3:
            *ہدف++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالت 2:
            *ہدف++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالت 1:
            *ہدف++=(uint8_t)c;
            U_FALLTHROUGH;
        طےشدہ:
            /* will never occur */
            توڑو;
        }
        ہدف_استعداد-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        جاؤ loop;
    } ورنہ {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=ہدف_استعداد;
        p=(uint8_t *)cnv->char_Error_بفر;
        انتخاب(length) {
            /* each branch falls through to the next one */
        حالت 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالت 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالت 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالت 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        طےشدہ:
            /* will never occur */
            توڑو;
        }
        cnv->char_Error_بفر_لمبائی=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        انتخاب(ہدف_استعداد) {
            /* each branch falls through to the next one */
        حالت 3:
            *ہدف++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالت 2:
            *ہدف++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالت 1:
            *ہدف++=(uint8_t)c;
            U_FALLTHROUGH;
        طےشدہ:
            توڑو;
        }

        /* target overflow */
        ہدف_استعداد=0;
        *p_Error_رمز=U_بفر_حد_سے_تجاوز_ERROR;
        c=0;
        جاؤ endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

ساکن ثابت حرف *  U_CALLCONV
_SCSUGetName(ثابت UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    انتخاب(scsu->locale) {
    حالت l_ja:
        واپس "SCSU,locale=ja";
    طےشدہ:
        واپس "SCSU";
    }
}

/* structure for SafeClone calculations */
ساخت cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

ساکن UConverter *  U_CALLCONV
_SCSUSafeClone(ثابت UConverter *cnv, 
               خالی *ڈھیر_بفر, 
               int32_t *p_بفر_سائز, 
               UError_رمز *حالت_2)
{
    ساخت cloneSCSUStruct * localClone;
    int32_t بفر_سائز_Needed = حجم(ساخت cloneSCSUStruct);

    اگر (U_FAILURE(*حالت_2)){
        واپس خالی_اشاریہ;
    }

    اگر (*p_بفر_سائز == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_سائز = بفر_سائز_Needed;
        واپس خالی_اشاریہ;
    }

    localClone = (ساخت cloneSCSUStruct *)ڈھیر_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    واپس &localClone->cnv;
}
U_CDECL_انتہا

ساکن ثابت UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    خالی_اشاریہ,
    خالی_اشاریہ,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    خالی_اشاریہ,

    خالی_اشاریہ,
    _SCSUGetName,
    خالی_اشاریہ,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_سیٹ,
    خالی_اشاریہ,
    خالی_اشاریہ
};

ساکن ثابت UConverter_Static_ڈیٹا SCSUStatic_ڈیٹا={
    حجم(UConverter_Static_ڈیٹا),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    جھوٹ, جھوٹ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_ڈیٹا _SCSUData=
        UCNV_IMMUTABLE_SHARED_ڈیٹا_INITIALIZER(&SCSUStatic_ڈیٹا, &_SCSUImpl);

#اگر_ختم
