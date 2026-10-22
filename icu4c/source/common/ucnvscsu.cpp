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

#ضمن "unicode/utypes.h"

#إذا !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#ضمن "unicode/ucnv.h"
#ضمن "unicode/ucnv_cb.h"
#ضمن "unicode/utf16.h"
#ضمن "ucnv_bld.h"
#ضمن "ucnv_cnv.h"
#ضمن "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
تعداد {
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

تعداد {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_پوله=0x68,
    gap_دننه_واټن=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_پیل=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_پوله=0xF9
};

/* constant offsets for the 8 static windows */
ساكن ثابت uint32_t staticOffsets[8]={
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
ساكن ثابت uint32_t initialDynamicOffsets[8]={
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
ساكن ثابت uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
تعداد {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

تعريفنوع بنية SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t منزل_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool منزل_UIs_Single_Byte_ډېرپېښ;
    uint8_t منزل_UState;
    int8_t منزل_UQuote_کړکۍ, منزل_UDynamic_کړکۍ;
    uint8_t منزل_UByte_One;
    uint8_t منزل_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_ډېرپېښ;
    int8_t from_UDynamic_کړکۍ;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t بل_کړکۍ_Use_شاخص;
    int8_t کړکۍ_Use[8];
} SCSUData;

ساكن ثابت int8_t initial_کړکۍ_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ساكن ثابت int8_t initial_کړکۍ_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

تعداد {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
ساكن فراغ U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    إذا(choice<=UCNV_RESET_منزل_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->منزل_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->منزل_UIs_Single_Byte_ډېرپېښ=ریښتیا;
        scsu->منزل_UState=readCommand;
        scsu->منزل_UQuote_کړکۍ=scsu->منزل_UDynamic_کړکۍ=0;
        scsu->منزل_UByte_One=0;

        cnv->منزل_ULength=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_ډېرپېښ=ریښتیا;
        scsu->from_UDynamic_کړکۍ=0;

        scsu->بل_کړکۍ_Use_شاخص=0;
        بدل(scsu->locale) {
        حالة l_ja:
            uprv_memcpy(scsu->کړکۍ_Use, initial_کړکۍ_Use_ja, 8);
            اكسر;
        افتراضي:
            uprv_memcpy(scsu->کړکۍ_Use, initial_کړکۍ_Use, 8);
            اكسر;
        }

        cnv->fromUChar32=0;
    }
}

ساكن فراغ U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_بارول_دلیلونه *p_دلیلونه,
          UError_کوډ *p_Error_کوډ) {
    ثابت حرف *locale=p_دلیلونه->locale;
    إذا(p_دلیلونه->onlyTestIsLoadable) {
        أرجع;
    }
    cnv->extraInfo=uprv_malloc(حجم(SCSUData));
    إذا(cnv->extraInfo!=تش_اشاره) {
        إذا(locale!=تش_اشاره && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } وإلا {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کوډ=U_حافظه_ویش_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ساكن فراغ U_CALLCONV
_SCSUClose(UConverter *cnv) {
    إذا(cnv->extraInfo!=تش_اشاره) {
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=تش_اشاره;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ساكن فراغ U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
                          UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    char16_t *موخه;
    ثابت char16_t *موخه_حد;
    int32_t *offsets;
    UBool is_Single_Byte_ډېرپېښ;
    uint8_t state, byteOne;
    int8_t quote_کړکۍ, dynamic_کړکۍ;

    int32_t سرچینه_شاخص, بل_سرچینه_شاخص;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلیلونه->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_دلیلونه->سرچینه_حد;
    موخه=p_دلیلونه->موخه;
    موخه_حد=p_دلیلونه->موخه_حد;
    offsets=p_دلیلونه->offsets;

    /* get the state machine state */
    is_Single_Byte_ډېرپېښ=scsu->منزل_UIs_Single_Byte_ډېرپېښ;
    state=scsu->منزل_UState;
    quote_کړکۍ=scsu->منزل_UQuote_کړکۍ;
    dynamic_کړکۍ=scsu->منزل_UDynamic_کړکۍ;
    byteOne=scsu->منزل_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سرچینه_شاخص=state==readCommand ? 0 : -1;
    بل_سرچینه_شاخص=0;

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
    إذا(is_Single_Byte_ډېرپېښ) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(سرچینه<سرچینه_حد && موخه<موخه_حد && (b=*سرچینه)>=0x20) {
                ++سرچینه;
                ++بل_سرچینه_شاخص;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *موخه++=(char16_t)b;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *موخه++=(char16_t)c;
                        إذا(offsets!=تش_اشاره) {
                            *offsets++=سرچینه_شاخص;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *موخه++=(char16_t)(0xd7c0+(c>>10));
                        إذا(موخه<موخه_حد) {
                            *موخه++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=تش_اشاره) {
                                *offsets++=سرچینه_شاخص;
                                *offsets++=سرچینه_شاخص;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=تش_اشاره) {
                                *offsets++=سرچینه_شاخص;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_اوږدتیا=1;
                            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ډېرپېښ:
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه>=موخه_حد) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            b=*سرچینه++;
            ++بل_سرچینه_شاخص;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *موخه++=(char16_t)b;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    سرچینه_شاخص=بل_سرچینه_شاخص;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_کړکۍ=(int8_t)(b-SC0);
                        سرچینه_شاخص=بل_سرچینه_شاخص;
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کړکۍ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_کړکۍ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    سرچینه_شاخص=بل_سرچینه_شاخص;
                    is_Single_Byte_ډېرپېښ=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    اذهب endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->منزل_UBytes[0]=b;
                cnv->منزل_ULength=1;
                اكسر;
            حالة quotePairOne:
                byteOne=b;
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=quotePairTwo;
                اكسر;
            حالة quotePairTwo:
                *موخه++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=تش_اشاره) {
                    *offsets++=سرچینه_شاخص;
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *موخه++=(char16_t)(staticOffsets[quote_کړکۍ]+b);
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_کړکۍ]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *موخه++=(char16_t)c;
                        إذا(offsets!=تش_اشاره) {
                            *offsets++=سرچینه_شاخص;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *موخه++=(char16_t)(0xd7c0+(c>>10));
                        إذا(موخه<موخه_حد) {
                            *موخه++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=تش_اشاره) {
                                *offsets++=سرچینه_شاخص;
                                *offsets++=سرچینه_شاخص;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=تش_اشاره) {
                                *offsets++=سرچینه_شاخص;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_اوږدتیا=1;
                            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_کړکۍ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=0x10000+(byteOne<<15UL | b<<7UL);
                سرچینه_شاخص=بل_سرچینه_شاخص;
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_پوله) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_پوله)<(reserved_پیل-gap_پوله)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=(b<<7UL)+gap_دننه_واټن;
                } وإلا إذا(b>=fixed_پوله) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=fixedOffsets[b-fixed_پوله];
                } وإلا {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
                state=readCommand;
                اذهب fastSingle;
            }
        }
    } وإلا {
        /* fast path for Unicode mode */
        إذا(state==readCommand) {
fastUnicode:
            طالما(سرچینه+1<سرچینه_حد && موخه<موخه_حد && (uint8_t)((b=*سرچینه)-UC0)>(Urs-UC0)) {
                *موخه++=(char16_t)((b<<8)|سرچینه[1]);
                إذا(offsets!=تش_اشاره) {
                    *offsets++=سرچینه_شاخص;
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
                بل_سرچینه_شاخص+=2;
                سرچینه+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه>=موخه_حد) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            b=*سرچینه++;
            ++بل_سرچینه_شاخص;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_کړکۍ=(int8_t)(b-UC0);
                    سرچینه_شاخص=بل_سرچینه_شاخص;
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_کړکۍ=(int8_t)(b-UD0);
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_ډېرپېښ;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_ډېرپېښ;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    اذهب endloop;
                }
                اكسر;
            حالة quotePairOne:
                byteOne=b;
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=quotePairTwo;
                اكسر;
            حالة quotePairTwo:
                *موخه++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=تش_اشاره) {
                    *offsets++=سرچینه_شاخص;
                }
                سرچینه_شاخص=بل_سرچینه_شاخص;
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_کوډ) && *p_Error_کوډ!=U_بفر_ډکېدنه_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_ډېرپېښ=is_Single_Byte_ډېرپېښ;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_کړکۍ=quote_کړکۍ;
    scsu->منزل_UDynamic_کړکۍ=dynamic_کړکۍ;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;
    p_دلیلونه->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_SCSUToUnicode(UConverter_منزل_Unicode_دلیلونه *p_دلیلونه,
               UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *سرچینه, *سرچینه_حد;
    char16_t *موخه;
    ثابت char16_t *موخه_حد;
    UBool is_Single_Byte_ډېرپېښ;
    uint8_t state, byteOne;
    int8_t quote_کړکۍ, dynamic_کړکۍ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلیلونه->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    سرچینه=(ثابت uint8_t *)p_دلیلونه->سرچینه;
    سرچینه_حد=(ثابت uint8_t *)p_دلیلونه->سرچینه_حد;
    موخه=p_دلیلونه->موخه;
    موخه_حد=p_دلیلونه->موخه_حد;

    /* get the state machine state */
    is_Single_Byte_ډېرپېښ=scsu->منزل_UIs_Single_Byte_ډېرپېښ;
    state=scsu->منزل_UState;
    quote_کړکۍ=scsu->منزل_UQuote_کړکۍ;
    dynamic_کړکۍ=scsu->منزل_UDynamic_کړکۍ;
    byteOne=scsu->منزل_UByte_One;

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
    إذا(is_Single_Byte_ډېرپېښ) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(سرچینه<سرچینه_حد && موخه<موخه_حد && (b=*سرچینه)>=0x20) {
                ++سرچینه;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *موخه++=(char16_t)b;
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *موخه++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *موخه++=(char16_t)(0xd7c0+(c>>10));
                        إذا(موخه<موخه_حد) {
                            *موخه++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_اوږدتیا=1;
                            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                            اذهب endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_ډېرپېښ:
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه>=موخه_حد) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            b=*سرچینه++;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *موخه++=(char16_t)b;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_کړکۍ=(int8_t)(b-SC0);
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کړکۍ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_کړکۍ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    is_Single_Byte_ډېرپېښ=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    اذهب endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->منزل_UBytes[0]=b;
                cnv->منزل_ULength=1;
                اكسر;
            حالة quotePairOne:
                byteOne=b;
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=quotePairTwo;
                اكسر;
            حالة quotePairTwo:
                *موخه++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *موخه++=(char16_t)(staticOffsets[quote_کړکۍ]+b);
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_کړکۍ]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *موخه++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *موخه++=(char16_t)(0xd7c0+(c>>10));
                        إذا(موخه<موخه_حد) {
                            *موخه++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_اوږدتیا=1;
                            *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_کړکۍ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_پوله) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_پوله)<(reserved_پیل-gap_پوله)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=(b<<7UL)+gap_دننه_واټن;
                } وإلا إذا(b>=fixed_پوله) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کړکۍ]=fixedOffsets[b-fixed_پوله];
                } وإلا {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                }
                state=readCommand;
                اذهب fastSingle;
            }
        }
    } وإلا {
        /* fast path for Unicode mode */
        إذا(state==readCommand) {
fastUnicode:
            طالما(سرچینه+1<سرچینه_حد && موخه<موخه_حد && (uint8_t)((b=*سرچینه)-UC0)>(Urs-UC0)) {
                *موخه++=(char16_t)((b<<8)|سرچینه[1]);
                سرچینه+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه>=موخه_حد) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            b=*سرچینه++;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_کړکۍ=(int8_t)(b-UC0);
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_کړکۍ=(int8_t)(b-UD0);
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_ډېرپېښ;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_ډېرپېښ;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    اذهب endloop;
                }
                اكسر;
            حالة quotePairOne:
                byteOne=b;
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=quotePairTwo;
                اكسر;
            حالة quotePairTwo:
                *موخه++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_کوډ) && *p_Error_کوډ!=U_بفر_ډکېدنه_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_ډېرپېښ=is_Single_Byte_ډېرپېښ;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_کړکۍ=quote_کړکۍ;
    scsu->منزل_UDynamic_کړکۍ=dynamic_کړکۍ;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=(ثابت حرف *)سرچینه;
    p_دلیلونه->موخه=موخه;
}
U_CDECL_پای_ټکی
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
ساكن int8_t
get_کړکۍ(ثابت uint32_t offsets[8], uint32_t c) {
    صحيح i;
    لكل(i=0; i<8; ++i) {
        إذا (c - offsets[i] <= 0x7f) {
            أرجع static_cast<int8_t>(i);
        }
    }
    أرجع -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
ساكن UBool
is_دننه_دننه_واټن_کړکۍ_Or_مستقیم(uint32_t دننه_واټن, uint32_t c) {
    أرجع c<=دننه_واټن+0x7f &&
          (c>=دننه_واټن || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ساكن int8_t
get_بل_Dynamic_کړکۍ(SCSUData *scsu) {
    int8_t کړکۍ=scsu->کړکۍ_Use[scsu->بل_کړکۍ_Use_شاخص];
    إذا(++scsu->بل_کړکۍ_Use_شاخص==8) {
        scsu->بل_کړکۍ_Use_شاخص=0;
    }
    أرجع کړکۍ;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ساكن فراغ
use_Dynamic_کړکۍ(SCSUData *scsu, int8_t کړکۍ) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    صحيح i, j;

    i=scsu->بل_کړکۍ_Use_شاخص;
    افعل {
        إذا(--i<0) {
            i=7;
        }
    } طالما(scsu->کړکۍ_Use[i]!=کړکۍ);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    إذا(j==8) {
        j=0;
    }
    طالما(j!=scsu->بل_کړکۍ_Use_شاخص) {
        scsu->کړکۍ_Use[i]=scsu->کړکۍ_Use[j];
        i=j;
        إذا(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->کړکۍ_Use[i]=کړکۍ;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
ساكن صحيح
get_Dynamic_دننه_واټن(uint32_t c, uint32_t *p_دننه_واټن) {
    صحيح i;

    لكل(i=0; i<7; ++i) {
        إذا (c - fixedOffsets[i] <= 0x7f) {
            *p_دننه_واټن=fixedOffsets[i];
            أرجع 0xf9+i;
        }
    }

    إذا(c<0x80) {
        /* No dynamic window for US-ASCII. */
        أرجع -1;
    } وإلا إذا(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_دننه_واټن=c&0x7fffff80;
        أرجع static_cast<صحيح>(c >> 7);
    } وإلا إذا(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_دننه_واټن=c&0x7fffff80;
        أرجع static_cast<صحيح>((c - gap_دننه_واټن) >> 7);
    } وإلا {
        أرجع -1;
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

ساكن فراغ U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_دلیلونه *p_دلیلونه,
                            UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه;
    int32_t موخه_ظرفیت;
    int32_t *offsets;

    UBool is_Single_Byte_ډېرپېښ;
    uint8_t dynamic_کړکۍ;
    uint32_t current_دننه_واټن;

    uint32_t c, delta;

    int32_t سرچینه_شاخص, بل_سرچینه_شاخص;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t دننه_واټن;
    char16_t lead, trail;
    صحيح کوډ;
    int8_t کړکۍ;

    /* set up the local pointers */
    cnv=p_دلیلونه->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    سرچینه=p_دلیلونه->سرچینه;
    سرچینه_حد=p_دلیلونه->سرچینه_حد;
    موخه=(uint8_t *)p_دلیلونه->موخه;
    موخه_ظرفیت=(int32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);
    offsets=p_دلیلونه->offsets;

    /* get the state machine state */
    is_Single_Byte_ډېرپېښ=scsu->from_UIs_Single_Byte_ډېرپېښ;
    dynamic_کړکۍ=scsu->from_UDynamic_کړکۍ;
    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سرچینه_شاخص= c==0 ? 0 : -1;
    بل_سرچینه_شاخص=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_ډېرپېښ) {
        إذا(c!=0 && موخه_ظرفیت>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه_ظرفیت<=0) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            c=*سرچینه++;
            ++بل_سرچینه_شاخص;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *موخه++=(uint8_t)c;
                إذا(offsets!=تش_اشاره) {
                    *offsets++=سرچینه_شاخص;
                }
                --موخه_ظرفیت;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *موخه++=(uint8_t)c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    --موخه_ظرفیت;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بایټونه;
                }
            } وإلا إذا((delta=c-current_دننه_واټن)<=0x7f) {
                /* use the current dynamic window */
                *موخه++=(uint8_t)(delta|0x80);
                إذا(offsets!=تش_اشاره) {
                    *offsets++=سرچینه_شاخص;
                }
                --موخه_ظرفیت;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(سرچینه<سرچینه_حد) {
                        /* test the following code unit */
                        trail=*سرچینه;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سرچینه;
                            ++بل_سرچینه_شاخص;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_دننه_واټن)<=0x7f) {
                    /* use the current dynamic window */
                    *موخه++=(uint8_t)(delta|0x80);
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    --موخه_ظرفیت;
                } وإلا إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کړکۍ=کړکۍ;
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(SC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    کوډ-=0x200;
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کړکۍ<<21)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=4;
                    اذهب output_بایټونه;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ډېرپېښ=خطأ;
                    *موخه++=(uint8_t)SCU;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                    }
                    --موخه_ظرفیت;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بایټونه;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بایټونه;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بایټونه;
            } وإلا {
                /* compress all other BMP characters */
                إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(سرچینه>=سرچینه_حد || is_دننه_دننه_واټن_کړکۍ_Or_مستقیم(scsu->fromUDynamicOffsets[کړکۍ], *سرچینه)) {
                        /* change to dynamic window */
                        dynamic_کړکۍ=کړکۍ;
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(SC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کړکۍ)<<8)|(c-scsu->fromUDynamicOffsets[کړکۍ])|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    }
                } وإلا إذا((کړکۍ=get_کړکۍ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کړکۍ)<<8)|(c-staticOffsets[کړکۍ]);
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(SD0+dynamic_کړکۍ)<<16)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=3;
                    اذهب output_بایټونه;
                } وإلا إذا ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (سرچینه >= سرچینه_حد || (uint32_t)(*سرچینه - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ډېرپېښ=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بایټونه;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بایټونه;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            سرچینه_شاخص=بل_سرچینه_شاخص;
        }
    } وإلا {
        إذا(c!=0 && موخه_ظرفیت>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه_ظرفیت<=0) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            c=*سرچینه++;
            ++بل_سرچینه_شاخص;

            إذا ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                إذا(موخه_ظرفیت>=2) {
                    *موخه++=(uint8_t)(c>>8);
                    *موخه++=(uint8_t)c;
                    إذا(offsets!=تش_اشاره) {
                        *offsets++=سرچینه_شاخص;
                        *offsets++=سرچینه_شاخص;
                    }
                    موخه_ظرفیت-=2;
                } وإلا {
                    length=2;
                    اذهب output_بایټونه;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(سرچینه<سرچینه_حد && (uint32_t)(*سرچینه-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        c|=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|c;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        dynamic_کړکۍ=کړکۍ;
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(UD0+dynamic_کړکۍ)<<16)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                        length=3;
                        اذهب output_بایټونه;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بایټونه;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(سرچینه<سرچینه_حد) {
                        /* test the following code unit */
                        trail=*سرچینه;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سرچینه;
                            ++بل_سرچینه_شاخص;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(سرچینه<سرچینه_حد && (uint32_t)(*سرچینه-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    dynamic_کړکۍ=کړکۍ;
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا(سرچینه<سرچینه_حد && lead==*سرچینه && /* too lazy to check trail in same window as source[1] */
                          (کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    کوډ-=0x200;
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کړکۍ<<21)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=4;
                    اذهب output_بایټونه;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بایټونه;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بایټونه;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            سرچینه_شاخص=بل_سرچینه_شاخص;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ډېرپېښ=is_Single_Byte_ډېرپېښ;
    scsu->from_UDynamic_کړکۍ=dynamic_کړکۍ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=سرچینه;
    p_دلیلونه->موخه=(حرف *)موخه;
    p_دلیلونه->offsets=offsets;
    أرجع;

output_بایټونه:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=موخه_ظرفیت) {
        إذا(offsets==تش_اشاره) {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *موخه++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            حالة 3:
                *موخه++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            حالة 2:
                *موخه++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            حالة 1:
                *موخه++=(uint8_t)c;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        } وإلا {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *موخه++=(uint8_t)(c>>24);
                *offsets++=سرچینه_شاخص;
                U_FALLTHROUGH;
            حالة 3:
                *موخه++=(uint8_t)(c>>16);
                *offsets++=سرچینه_شاخص;
                U_FALLTHROUGH;
            حالة 2:
                *موخه++=(uint8_t)(c>>8);
                *offsets++=سرچینه_شاخص;
                U_FALLTHROUGH;
            حالة 1:
                *موخه++=(uint8_t)c;
                *offsets++=سرچینه_شاخص;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        }
        موخه_ظرفیت-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        سرچینه_شاخص=بل_سرچینه_شاخص;
        اذهب loop;
    } وإلا {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=موخه_ظرفیت;
        p=(uint8_t *)cnv->char_Error_بفر;
        بدل(length) {
            /* each branch falls through to the next one */
        حالة 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالة 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            /* will never occur */
            اكسر;
        }
        cnv->char_Error_بفر_اوږدتیا=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        بدل(موخه_ظرفیت) {
            /* each branch falls through to the next one */
        حالة 3:
            *موخه++=(uint8_t)(c>>16);
            إذا(offsets!=تش_اشاره) {
                *offsets++=سرچینه_شاخص;
            }
            U_FALLTHROUGH;
        حالة 2:
            *موخه++=(uint8_t)(c>>8);
            إذا(offsets!=تش_اشاره) {
                *offsets++=سرچینه_شاخص;
            }
            U_FALLTHROUGH;
        حالة 1:
            *موخه++=(uint8_t)c;
            إذا(offsets!=تش_اشاره) {
                *offsets++=سرچینه_شاخص;
            }
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        موخه_ظرفیت=0;
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        c=0;
        اذهب endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_دلیلونه *p_دلیلونه,
                 UError_کوډ *p_Error_کوډ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *سرچینه, *سرچینه_حد;
    uint8_t *موخه;
    int32_t موخه_ظرفیت;

    UBool is_Single_Byte_ډېرپېښ;
    uint8_t dynamic_کړکۍ;
    uint32_t current_دننه_واټن;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t دننه_واټن;
    char16_t lead, trail;
    صحيح کوډ;
    int8_t کړکۍ;

    /* set up the local pointers */
    cnv=p_دلیلونه->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    سرچینه=p_دلیلونه->سرچینه;
    سرچینه_حد=p_دلیلونه->سرچینه_حد;
    موخه=(uint8_t *)p_دلیلونه->موخه;
    موخه_ظرفیت=(int32_t)(p_دلیلونه->موخه_حد-p_دلیلونه->موخه);

    /* get the state machine state */
    is_Single_Byte_ډېرپېښ=scsu->from_UIs_Single_Byte_ډېرپېښ;
    dynamic_کړکۍ=scsu->from_UDynamic_کړکۍ;
    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_ډېرپېښ) {
        إذا(c!=0 && موخه_ظرفیت>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه_ظرفیت<=0) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            c=*سرچینه++;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *موخه++=(uint8_t)c;
                --موخه_ظرفیت;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *موخه++=(uint8_t)c;
                    --موخه_ظرفیت;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بایټونه;
                }
            } وإلا إذا((delta=c-current_دننه_واټن)<=0x7f) {
                /* use the current dynamic window */
                *موخه++=(uint8_t)(delta|0x80);
                --موخه_ظرفیت;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(سرچینه<سرچینه_حد) {
                        /* test the following code unit */
                        trail=*سرچینه;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سرچینه;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_دننه_واټن)<=0x7f) {
                    /* use the current dynamic window */
                    *موخه++=(uint8_t)(delta|0x80);
                    --موخه_ظرفیت;
                } وإلا إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کړکۍ=کړکۍ;
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(SC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    کوډ-=0x200;
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کړکۍ<<21)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=4;
                    اذهب output_بایټونه;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_ډېرپېښ=خطأ;
                    *موخه++=(uint8_t)SCU;
                    --موخه_ظرفیت;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بایټونه;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بایټونه;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بایټونه;
            } وإلا {
                /* compress all other BMP characters */
                إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(سرچینه>=سرچینه_حد || is_دننه_دننه_واټن_کړکۍ_Or_مستقیم(scsu->fromUDynamicOffsets[کړکۍ], *سرچینه)) {
                        /* change to dynamic window */
                        dynamic_کړکۍ=کړکۍ;
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(SC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کړکۍ)<<8)|(c-scsu->fromUDynamicOffsets[کړکۍ])|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    }
                } وإلا إذا((کړکۍ=get_کړکۍ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کړکۍ)<<8)|(c-staticOffsets[کړکۍ]);
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(SD0+dynamic_کړکۍ)<<16)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=3;
                    اذهب output_بایټونه;
                } وإلا إذا (c - 0x3400 < 0xd800 - 0x3400 &&
                           (سرچینه >= سرچینه_حد || static_cast<uint32_t>(*سرچینه - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_ډېرپېښ=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بایټونه;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بایټونه;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } وإلا {
        إذا(c!=0 && موخه_ظرفیت>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سرچینه<سرچینه_حد) {
            إذا(موخه_ظرفیت<=0) {
                /* target is full */
                *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
                اكسر;
            }
            c=*سرچینه++;

            إذا (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                إذا(موخه_ظرفیت>=2) {
                    *موخه++=(uint8_t)(c>>8);
                    *موخه++=(uint8_t)c;
                    موخه_ظرفیت-=2;
                } وإلا {
                    length=2;
                    اذهب output_بایټونه;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(سرچینه<سرچینه_حد && (uint32_t)(*سرچینه-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        c|=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|c;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا إذا((کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        dynamic_کړکۍ=کړکۍ;
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                        length=2;
                        اذهب output_بایټونه;
                    } وإلا إذا((کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_ډېرپېښ=ریښتیا;
                        dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                        current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                        use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                        c=((uint32_t)(UD0+dynamic_کړکۍ)<<16)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                        length=3;
                        اذهب output_بایټونه;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بایټونه;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(سرچینه<سرچینه_حد) {
                        /* test the following code unit */
                        trail=*سرچینه;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سرچینه;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوډ=U_ILLEGAL_CHAR_وموندل;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (کړکۍ=get_کړکۍ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(سرچینه<سرچینه_حد && (uint32_t)(*سرچینه-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    dynamic_کړکۍ=کړکۍ;
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ];
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)(UC0+dynamic_کړکۍ)<<8)|(c-current_دننه_واټن)|0x80;
                    length=2;
                    اذهب output_بایټونه;
                } وإلا إذا(سرچینه<سرچینه_حد && lead==*سرچینه && /* too lazy to check trail in same window as source[1] */
                          (کوډ=get_Dynamic_دننه_واټن(c, &دننه_واټن))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_ډېرپېښ=ریښتیا;
                    کوډ-=0x200;
                    dynamic_کړکۍ=get_بل_Dynamic_کړکۍ(scsu);
                    current_دننه_واټن=scsu->fromUDynamicOffsets[dynamic_کړکۍ]=دننه_واټن;
                    use_Dynamic_کړکۍ(scsu, dynamic_کړکۍ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کړکۍ<<21)|((uint32_t)کوډ<<8)|(c-current_دننه_واټن)|0x80;
                    length=4;
                    اذهب output_بایټونه;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بایټونه;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بایټونه;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_ډېرپېښ=is_Single_Byte_ډېرپېښ;
    scsu->from_UDynamic_کړکۍ=dynamic_کړکۍ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلیلونه->سرچینه=سرچینه;
    p_دلیلونه->موخه=(حرف *)موخه;
    أرجع;

output_بایټونه:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=موخه_ظرفیت) {
        بدل(length) {
            /* each branch falls through to the next one */
        حالة 4:
            *موخه++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالة 3:
            *موخه++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *موخه++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *موخه++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            /* will never occur */
            اكسر;
        }
        موخه_ظرفیت-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        اذهب loop;
    } وإلا {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=موخه_ظرفیت;
        p=(uint8_t *)cnv->char_Error_بفر;
        بدل(length) {
            /* each branch falls through to the next one */
        حالة 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالة 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            /* will never occur */
            اكسر;
        }
        cnv->char_Error_بفر_اوږدتیا=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        بدل(موخه_ظرفیت) {
            /* each branch falls through to the next one */
        حالة 3:
            *موخه++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *موخه++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *موخه++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        موخه_ظرفیت=0;
        *p_Error_کوډ=U_بفر_ډکېدنه_ERROR;
        c=0;
        اذهب endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

ساكن ثابت حرف *  U_CALLCONV
_SCSUGetName(ثابت UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    بدل(scsu->locale) {
    حالة l_ja:
        أرجع "SCSU,locale=ja";
    افتراضي:
        أرجع "SCSU";
    }
}

/* structure for SafeClone calculations */
بنية cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

ساكن UConverter *  U_CALLCONV
_SCSUSafeClone(ثابت UConverter *cnv, 
               فراغ *ډېری_بفر, 
               int32_t *p_بفر_اندازه, 
               UError_کوډ *حالت)
{
    بنية cloneSCSUStruct * localClone;
    int32_t بفر_اندازه_Needed = حجم(بنية cloneSCSUStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع تش_اشاره;
    }

    إذا (*p_بفر_اندازه == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_اندازه = بفر_اندازه_Needed;
        أرجع تش_اشاره;
    }

    localClone = (بنية cloneSCSUStruct *)ډېری_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = ریښتیا;

    أرجع &localClone->cnv;
}
U_CDECL_پای_ټکی

ساكن ثابت UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    تش_اشاره,
    تش_اشاره,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    تش_اشاره,

    تش_اشاره,
    _SCSUGetName,
    تش_اشاره,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_سېټ,
    تش_اشاره,
    تش_اشاره
};

ساكن ثابت UConverter_Static_معلومات SCSUStatic_معلومات={
    حجم(UConverter_Static_معلومات),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    خطأ, خطأ,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ثابت UConverter_Shared_معلومات _SCSUData=
        UCNV_IMMUTABLE_SHARED_معلومات_INITIALIZER(&SCSUStatic_معلومات, &_SCSUImpl);

#د_که_پای
