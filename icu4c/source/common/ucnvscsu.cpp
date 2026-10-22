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
    gap_حد=0x68,
    gap_وتھ=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_شروع=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_حد=0xF9
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
    UBool منزل_UIs_Single_Byte_منوال;
    uint8_t منزل_UState;
    int8_t منزل_UQuote_کھڑکی, منزل_UDynamic_کھڑکی;
    uint8_t منزل_UByte_One;
    uint8_t منزل_UPadding[3];

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

ساكن ثابت int8_t initial_کھڑکی_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ساكن ثابت int8_t initial_کھڑکی_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

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

        scsu->منزل_UIs_Single_Byte_منوال=سچ;
        scsu->منزل_UState=readCommand;
        scsu->منزل_UQuote_کھڑکی=scsu->منزل_UDynamic_کھڑکی=0;
        scsu->منزل_UByte_One=0;

        cnv->منزل_ULength=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_منوال=سچ;
        scsu->from_UDynamic_کھڑکی=0;

        scsu->اگلا_کھڑکی_Use_اشاریہ=0;
        بدل(scsu->locale) {
        حالة l_ja:
            uprv_memcpy(scsu->کھڑکی_Use, initial_کھڑکی_Use_ja, 8);
            اكسر;
        افتراضي:
            uprv_memcpy(scsu->کھڑکی_Use, initial_کھڑکی_Use, 8);
            اكسر;
        }

        cnv->fromUChar32=0;
    }
}

ساكن فراغ U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_لدو_دلیلاں *p_دلیلاں,
          UError_کوڈ *p_Error_کوڈ) {
    ثابت حرف *locale=p_دلیلاں->locale;
    إذا(p_دلیلاں->onlyTestIsLoadable) {
        أرجع;
    }
    cnv->extraInfo=uprv_malloc(حجم(SCSUData));
    إذا(cnv->extraInfo!=خالی_سوچک) {
        إذا(locale!=خالی_سوچک && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } وإلا {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_کوڈ=U_حافظہ_ونڈ_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ساكن فراغ U_CALLCONV
_SCSUClose(UConverter *cnv) {
    إذا(cnv->extraInfo!=خالی_سوچک) {
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=خالی_سوچک;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ساكن فراغ U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
                          UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    char16_t *نشانہ;
    ثابت char16_t *نشانہ_حد_بندی;
    int32_t *offsets;
    UBool is_Single_Byte_منوال;
    uint8_t state, byteOne;
    int8_t quote_کھڑکی, dynamic_کھڑکی;

    int32_t سروت_اشاریہ, اگلا_سروت_اشاریہ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلیلاں->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    سروت_حد_بندی=(ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی;
    نشانہ=p_دلیلاں->نشانہ;
    نشانہ_حد_بندی=p_دلیلاں->نشانہ_حد_بندی;
    offsets=p_دلیلاں->offsets;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->منزل_UIs_Single_Byte_منوال;
    state=scsu->منزل_UState;
    quote_کھڑکی=scsu->منزل_UQuote_کھڑکی;
    dynamic_کھڑکی=scsu->منزل_UDynamic_کھڑکی;
    byteOne=scsu->منزل_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سروت_اشاریہ=state==readCommand ? 0 : -1;
    اگلا_سروت_اشاریہ=0;

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
    إذا(is_Single_Byte_منوال) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(سروت<سروت_حد_بندی && نشانہ<نشانہ_حد_بندی && (b=*سروت)>=0x20) {
                ++سروت;
                ++اگلا_سروت_اشاریہ;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *نشانہ++=(char16_t)b;
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *نشانہ++=(char16_t)c;
                        إذا(offsets!=خالی_سوچک) {
                            *offsets++=سروت_اشاریہ;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *نشانہ++=(char16_t)(0xd7c0+(c>>10));
                        إذا(نشانہ<نشانہ_حد_بندی) {
                            *نشانہ++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=خالی_سوچک) {
                                *offsets++=سروت_اشاریہ;
                                *offsets++=سروت_اشاریہ;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=خالی_سوچک) {
                                *offsets++=سروت_اشاریہ;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمائی=1;
                            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_منوال:
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ>=نشانہ_حد_بندی) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            b=*سروت++;
            ++اگلا_سروت_اشاریہ;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *نشانہ++=(char16_t)b;
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                    سروت_اشاریہ=اگلا_سروت_اشاریہ;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_کھڑکی=(int8_t)(b-SC0);
                        سروت_اشاریہ=اگلا_سروت_اشاریہ;
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کھڑکی=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_کھڑکی=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    سروت_اشاریہ=اگلا_سروت_اشاریہ;
                    is_Single_Byte_منوال=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
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
                *نشانہ++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=خالی_سوچک) {
                    *offsets++=سروت_اشاریہ;
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *نشانہ++=(char16_t)(staticOffsets[quote_کھڑکی]+b);
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_کھڑکی]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *نشانہ++=(char16_t)c;
                        إذا(offsets!=خالی_سوچک) {
                            *offsets++=سروت_اشاریہ;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *نشانہ++=(char16_t)(0xd7c0+(c>>10));
                        إذا(نشانہ<نشانہ_حد_بندی) {
                            *نشانہ++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=خالی_سوچک) {
                                *offsets++=سروت_اشاریہ;
                                *offsets++=سروت_اشاریہ;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=خالی_سوچک) {
                                *offsets++=سروت_اشاریہ;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمائی=1;
                            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_کھڑکی=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=0x10000+(byteOne<<15UL | b<<7UL);
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_حد)<(reserved_شروع-gap_حد)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=(b<<7UL)+gap_وتھ;
                } وإلا إذا(b>=fixed_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=fixedOffsets[b-fixed_حد];
                } وإلا {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                state=readCommand;
                اذهب fastSingle;
            }
        }
    } وإلا {
        /* fast path for Unicode mode */
        إذا(state==readCommand) {
fastUnicode:
            طالما(سروت+1<سروت_حد_بندی && نشانہ<نشانہ_حد_بندی && (uint8_t)((b=*سروت)-UC0)>(Urs-UC0)) {
                *نشانہ++=(char16_t)((b<<8)|سروت[1]);
                إذا(offsets!=خالی_سوچک) {
                    *offsets++=سروت_اشاریہ;
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                اگلا_سروت_اشاریہ+=2;
                سروت+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ>=نشانہ_حد_بندی) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            b=*سروت++;
            ++اگلا_سروت_اشاریہ;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_کھڑکی=(int8_t)(b-UC0);
                    سروت_اشاریہ=اگلا_سروت_اشاریہ;
                    is_Single_Byte_منوال=سچ;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_کھڑکی=(int8_t)(b-UD0);
                    is_Single_Byte_منوال=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_منوال;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_منوال=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_منوال;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
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
                *نشانہ++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=خالی_سوچک) {
                    *offsets++=سروت_اشاریہ;
                }
                سروت_اشاریہ=اگلا_سروت_اشاریہ;
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_کوڈ) && *p_Error_کوڈ!=U_بفر_حد_توں_ودھ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_کھڑکی=quote_کھڑکی;
    scsu->منزل_UDynamic_کھڑکی=dynamic_کھڑکی;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلیلاں->سروت=(ثابت حرف *)سروت;
    p_دلیلاں->نشانہ=نشانہ;
    p_دلیلاں->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_SCSUToUnicode(UConverter_منزل_Unicode_دلیلاں *p_دلیلاں,
               UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *سروت, *سروت_حد_بندی;
    char16_t *نشانہ;
    ثابت char16_t *نشانہ_حد_بندی;
    UBool is_Single_Byte_منوال;
    uint8_t state, byteOne;
    int8_t quote_کھڑکی, dynamic_کھڑکی;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دلیلاں->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    سروت=(ثابت uint8_t *)p_دلیلاں->سروت;
    سروت_حد_بندی=(ثابت uint8_t *)p_دلیلاں->سروت_حد_بندی;
    نشانہ=p_دلیلاں->نشانہ;
    نشانہ_حد_بندی=p_دلیلاں->نشانہ_حد_بندی;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->منزل_UIs_Single_Byte_منوال;
    state=scsu->منزل_UState;
    quote_کھڑکی=scsu->منزل_UQuote_کھڑکی;
    dynamic_کھڑکی=scsu->منزل_UDynamic_کھڑکی;
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
    إذا(is_Single_Byte_منوال) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(سروت<سروت_حد_بندی && نشانہ<نشانہ_حد_بندی && (b=*سروت)>=0x20) {
                ++سروت;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *نشانہ++=(char16_t)b;
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *نشانہ++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *نشانہ++=(char16_t)(0xd7c0+(c>>10));
                        إذا(نشانہ<نشانہ_حد_بندی) {
                            *نشانہ++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمائی=1;
                            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                            اذهب endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_منوال:
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ>=نشانہ_حد_بندی) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            b=*سروت++;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *نشانہ++=(char16_t)b;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_کھڑکی=(int8_t)(b-SC0);
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_کھڑکی=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_کھڑکی=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    is_Single_Byte_منوال=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
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
                *نشانہ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *نشانہ++=(char16_t)(staticOffsets[quote_کھڑکی]+b);
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_کھڑکی]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *نشانہ++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *نشانہ++=(char16_t)(0xd7c0+(c>>10));
                        إذا(نشانہ<نشانہ_حد_بندی) {
                            *نشانہ++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_لمائی=1;
                            *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_کھڑکی=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_حد)<(reserved_شروع-gap_حد)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=(b<<7UL)+gap_وتھ;
                } وإلا إذا(b>=fixed_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_کھڑکی]=fixedOffsets[b-fixed_حد];
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
            طالما(سروت+1<سروت_حد_بندی && نشانہ<نشانہ_حد_بندی && (uint8_t)((b=*سروت)-UC0)>(Urs-UC0)) {
                *نشانہ++=(char16_t)((b<<8)|سروت[1]);
                سروت+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ>=نشانہ_حد_بندی) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            b=*سروت++;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_کھڑکی=(int8_t)(b-UC0);
                    is_Single_Byte_منوال=سچ;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_کھڑکی=(int8_t)(b-UD0);
                    is_Single_Byte_منوال=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_منوال;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_منوال=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_منوال;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
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
                *نشانہ++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_کوڈ) && *p_Error_کوڈ!=U_بفر_حد_توں_ودھ_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_کھڑکی=quote_کھڑکی;
    scsu->منزل_UDynamic_کھڑکی=dynamic_کھڑکی;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دلیلاں->سروت=(ثابت حرف *)سروت;
    p_دلیلاں->نشانہ=نشانہ;
}
U_CDECL_سرا
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
get_کھڑکی(ثابت uint32_t offsets[8], uint32_t c) {
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
is_وچ_وتھ_کھڑکی_Or_سدھا(uint32_t وتھ, uint32_t c) {
    أرجع c<=وتھ+0x7f &&
          (c>=وتھ || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ساكن int8_t
get_اگلا_Dynamic_کھڑکی(SCSUData *scsu) {
    int8_t کھڑکی=scsu->کھڑکی_Use[scsu->اگلا_کھڑکی_Use_اشاریہ];
    إذا(++scsu->اگلا_کھڑکی_Use_اشاریہ==8) {
        scsu->اگلا_کھڑکی_Use_اشاریہ=0;
    }
    أرجع کھڑکی;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ساكن فراغ
use_Dynamic_کھڑکی(SCSUData *scsu, int8_t کھڑکی) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    صحيح i, j;

    i=scsu->اگلا_کھڑکی_Use_اشاریہ;
    افعل {
        إذا(--i<0) {
            i=7;
        }
    } طالما(scsu->کھڑکی_Use[i]!=کھڑکی);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    إذا(j==8) {
        j=0;
    }
    طالما(j!=scsu->اگلا_کھڑکی_Use_اشاریہ) {
        scsu->کھڑکی_Use[i]=scsu->کھڑکی_Use[j];
        i=j;
        إذا(++j==8) { j=0; }
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
ساكن صحيح
get_Dynamic_وتھ(uint32_t c, uint32_t *p_وتھ) {
    صحيح i;

    لكل(i=0; i<7; ++i) {
        إذا (c - fixedOffsets[i] <= 0x7f) {
            *p_وتھ=fixedOffsets[i];
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
        *p_وتھ=c&0x7fffff80;
        أرجع static_cast<صحيح>(c >> 7);
    } وإلا إذا(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_وتھ=c&0x7fffff80;
        أرجع static_cast<صحيح>((c - gap_وتھ) >> 7);
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
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_دلیلاں *p_دلیلاں,
                            UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ;
    int32_t نشانہ_سمرتھا;
    int32_t *offsets;

    UBool is_Single_Byte_منوال;
    uint8_t dynamic_کھڑکی;
    uint32_t current_وتھ;

    uint32_t c, delta;

    int32_t سروت_اشاریہ, اگلا_سروت_اشاریہ;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t وتھ;
    char16_t lead, trail;
    صحيح کوڈ;
    int8_t کھڑکی;

    /* set up the local pointers */
    cnv=p_دلیلاں->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    سروت=p_دلیلاں->سروت;
    سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
    نشانہ=(uint8_t *)p_دلیلاں->نشانہ;
    نشانہ_سمرتھا=(int32_t)(p_دلیلاں->نشانہ_حد_بندی-p_دلیلاں->نشانہ);
    offsets=p_دلیلاں->offsets;

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->from_UIs_Single_Byte_منوال;
    dynamic_کھڑکی=scsu->from_UDynamic_کھڑکی;
    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    سروت_اشاریہ= c==0 ? 0 : -1;
    اگلا_سروت_اشاریہ=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_منوال) {
        إذا(c!=0 && نشانہ_سمرتھا>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ_سمرتھا<=0) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            c=*سروت++;
            ++اگلا_سروت_اشاریہ;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *نشانہ++=(uint8_t)c;
                إذا(offsets!=خالی_سوچک) {
                    *offsets++=سروت_اشاریہ;
                }
                --نشانہ_سمرتھا;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *نشانہ++=(uint8_t)c;
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                    --نشانہ_سمرتھا;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا((delta=c-current_وتھ)<=0x7f) {
                /* use the current dynamic window */
                *نشانہ++=(uint8_t)(delta|0x80);
                إذا(offsets!=خالی_سوچک) {
                    *offsets++=سروت_اشاریہ;
                }
                --نشانہ_سمرتھا;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(سروت<سروت_حد_بندی) {
                        /* test the following code unit */
                        trail=*سروت;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سروت;
                            ++اگلا_سروت_اشاریہ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_وتھ)<=0x7f) {
                    /* use the current dynamic window */
                    *نشانہ++=(uint8_t)(delta|0x80);
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                    --نشانہ_سمرتھا;
                } وإلا إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کھڑکی=کھڑکی;
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    کوڈ-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=4;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_منوال=خطأ;
                    *نشانہ++=(uint8_t)SCU;
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                    }
                    --نشانہ_سمرتھا;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بائٹاں;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بائٹاں;
            } وإلا {
                /* compress all other BMP characters */
                إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(سروت>=سروت_حد_بندی || is_وچ_وتھ_کھڑکی_Or_سدھا(scsu->fromUDynamicOffsets[کھڑکی], *سروت)) {
                        /* change to dynamic window */
                        dynamic_کھڑکی=کھڑکی;
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-scsu->fromUDynamicOffsets[کھڑکی])|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    }
                } وإلا إذا((کھڑکی=get_کھڑکی(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-staticOffsets[کھڑکی]);
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SD0+dynamic_کھڑکی)<<16)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=3;
                    اذهب output_بائٹاں;
                } وإلا إذا ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (سروت >= سروت_حد_بندی || (uint32_t)(*سروت - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_منوال=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بائٹاں;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            سروت_اشاریہ=اگلا_سروت_اشاریہ;
        }
    } وإلا {
        إذا(c!=0 && نشانہ_سمرتھا>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ_سمرتھا<=0) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            c=*سروت++;
            ++اگلا_سروت_اشاریہ;

            إذا ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                إذا(نشانہ_سمرتھا>=2) {
                    *نشانہ++=(uint8_t)(c>>8);
                    *نشانہ++=(uint8_t)c;
                    إذا(offsets!=خالی_سوچک) {
                        *offsets++=سروت_اشاریہ;
                        *offsets++=سروت_اشاریہ;
                    }
                    نشانہ_سمرتھا-=2;
                } وإلا {
                    length=2;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(سروت<سروت_حد_بندی && (uint32_t)(*سروت-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_منوال=سچ;
                        c|=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|c;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=کھڑکی;
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UD0+dynamic_کھڑکی)<<16)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                        length=3;
                        اذهب output_بائٹاں;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بائٹاں;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(سروت<سروت_حد_بندی) {
                        /* test the following code unit */
                        trail=*سروت;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سروت;
                            ++اگلا_سروت_اشاریہ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(سروت<سروت_حد_بندی && (uint32_t)(*سروت-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_منوال=سچ;
                    dynamic_کھڑکی=کھڑکی;
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا(سروت<سروت_حد_بندی && lead==*سروت && /* too lazy to check trail in same window as source[1] */
                          (کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_منوال=سچ;
                    کوڈ-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=4;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائٹاں;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بائٹاں;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            سروت_اشاریہ=اگلا_سروت_اشاریہ;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_منوال=is_Single_Byte_منوال;
    scsu->from_UDynamic_کھڑکی=dynamic_کھڑکی;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دلیلاں->سروت=سروت;
    p_دلیلاں->نشانہ=(حرف *)نشانہ;
    p_دلیلاں->offsets=offsets;
    أرجع;

output_بائٹاں:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=نشانہ_سمرتھا) {
        إذا(offsets==خالی_سوچک) {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *نشانہ++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            حالة 3:
                *نشانہ++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            حالة 2:
                *نشانہ++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            حالة 1:
                *نشانہ++=(uint8_t)c;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        } وإلا {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *نشانہ++=(uint8_t)(c>>24);
                *offsets++=سروت_اشاریہ;
                U_FALLTHROUGH;
            حالة 3:
                *نشانہ++=(uint8_t)(c>>16);
                *offsets++=سروت_اشاریہ;
                U_FALLTHROUGH;
            حالة 2:
                *نشانہ++=(uint8_t)(c>>8);
                *offsets++=سروت_اشاریہ;
                U_FALLTHROUGH;
            حالة 1:
                *نشانہ++=(uint8_t)c;
                *offsets++=سروت_اشاریہ;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        }
        نشانہ_سمرتھا-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        سروت_اشاریہ=اگلا_سروت_اشاریہ;
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
        length-=نشانہ_سمرتھا;
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
        cnv->char_Error_بفر_لمائی=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        بدل(نشانہ_سمرتھا) {
            /* each branch falls through to the next one */
        حالة 3:
            *نشانہ++=(uint8_t)(c>>16);
            إذا(offsets!=خالی_سوچک) {
                *offsets++=سروت_اشاریہ;
            }
            U_FALLTHROUGH;
        حالة 2:
            *نشانہ++=(uint8_t)(c>>8);
            إذا(offsets!=خالی_سوچک) {
                *offsets++=سروت_اشاریہ;
            }
            U_FALLTHROUGH;
        حالة 1:
            *نشانہ++=(uint8_t)c;
            إذا(offsets!=خالی_سوچک) {
                *offsets++=سروت_اشاریہ;
            }
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        نشانہ_سمرتھا=0;
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
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
_SCSUFromUnicode(UConverter_From_Unicode_دلیلاں *p_دلیلاں,
                 UError_کوڈ *p_Error_کوڈ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *سروت, *سروت_حد_بندی;
    uint8_t *نشانہ;
    int32_t نشانہ_سمرتھا;

    UBool is_Single_Byte_منوال;
    uint8_t dynamic_کھڑکی;
    uint32_t current_وتھ;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t وتھ;
    char16_t lead, trail;
    صحيح کوڈ;
    int8_t کھڑکی;

    /* set up the local pointers */
    cnv=p_دلیلاں->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    سروت=p_دلیلاں->سروت;
    سروت_حد_بندی=p_دلیلاں->سروت_حد_بندی;
    نشانہ=(uint8_t *)p_دلیلاں->نشانہ;
    نشانہ_سمرتھا=(int32_t)(p_دلیلاں->نشانہ_حد_بندی-p_دلیلاں->نشانہ);

    /* get the state machine state */
    is_Single_Byte_منوال=scsu->from_UIs_Single_Byte_منوال;
    dynamic_کھڑکی=scsu->from_UDynamic_کھڑکی;
    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_منوال) {
        إذا(c!=0 && نشانہ_سمرتھا>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ_سمرتھا<=0) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            c=*سروت++;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *نشانہ++=(uint8_t)c;
                --نشانہ_سمرتھا;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *نشانہ++=(uint8_t)c;
                    --نشانہ_سمرتھا;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا((delta=c-current_وتھ)<=0x7f) {
                /* use the current dynamic window */
                *نشانہ++=(uint8_t)(delta|0x80);
                --نشانہ_سمرتھا;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(سروت<سروت_حد_بندی) {
                        /* test the following code unit */
                        trail=*سروت;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سروت;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_وتھ)<=0x7f) {
                    /* use the current dynamic window */
                    *نشانہ++=(uint8_t)(delta|0x80);
                    --نشانہ_سمرتھا;
                } وإلا إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_کھڑکی=کھڑکی;
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    کوڈ-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=4;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_منوال=خطأ;
                    *نشانہ++=(uint8_t)SCU;
                    --نشانہ_سمرتھا;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بائٹاں;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بائٹاں;
            } وإلا {
                /* compress all other BMP characters */
                إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(سروت>=سروت_حد_بندی || is_وچ_وتھ_کھڑکی_Or_سدھا(scsu->fromUDynamicOffsets[کھڑکی], *سروت)) {
                        /* change to dynamic window */
                        dynamic_کھڑکی=کھڑکی;
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(SC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-scsu->fromUDynamicOffsets[کھڑکی])|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    }
                } وإلا إذا((کھڑکی=get_کھڑکی(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+کھڑکی)<<8)|(c-staticOffsets[کھڑکی]);
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(SD0+dynamic_کھڑکی)<<16)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=3;
                    اذهب output_بائٹاں;
                } وإلا إذا (c - 0x3400 < 0xd800 - 0x3400 &&
                           (سروت >= سروت_حد_بندی || static_cast<uint32_t>(*سروت - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_منوال=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بائٹاں;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } وإلا {
        إذا(c!=0 && نشانہ_سمرتھا>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(سروت<سروت_حد_بندی) {
            إذا(نشانہ_سمرتھا<=0) {
                /* target is full */
                *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
                اكسر;
            }
            c=*سروت++;

            إذا (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                إذا(نشانہ_سمرتھا>=2) {
                    *نشانہ++=(uint8_t)(c>>8);
                    *نشانہ++=(uint8_t)c;
                    نشانہ_سمرتھا-=2;
                } وإلا {
                    length=2;
                    اذهب output_بائٹاں;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(سروت<سروت_حد_بندی && (uint32_t)(*سروت-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_منوال=سچ;
                        c|=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|c;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا إذا((کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=کھڑکی;
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                        length=2;
                        اذهب output_بائٹاں;
                    } وإلا إذا((کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_منوال=سچ;
                        dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                        current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                        use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                        c=((uint32_t)(UD0+dynamic_کھڑکی)<<16)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                        length=3;
                        اذهب output_بائٹاں;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بائٹاں;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(سروت<سروت_حد_بندی) {
                        /* test the following code unit */
                        trail=*سروت;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++سروت;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_کوڈ=U_ILLEGAL_CHAR_لبھیا;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (کھڑکی=get_کھڑکی(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(سروت<سروت_حد_بندی && (uint32_t)(*سروت-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_منوال=سچ;
                    dynamic_کھڑکی=کھڑکی;
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی];
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)(UC0+dynamic_کھڑکی)<<8)|(c-current_وتھ)|0x80;
                    length=2;
                    اذهب output_بائٹاں;
                } وإلا إذا(سروت<سروت_حد_بندی && lead==*سروت && /* too lazy to check trail in same window as source[1] */
                          (کوڈ=get_Dynamic_وتھ(c, &وتھ))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_منوال=سچ;
                    کوڈ-=0x200;
                    dynamic_کھڑکی=get_اگلا_Dynamic_کھڑکی(scsu);
                    current_وتھ=scsu->fromUDynamicOffsets[dynamic_کھڑکی]=وتھ;
                    use_Dynamic_کھڑکی(scsu, dynamic_کھڑکی);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_کھڑکی<<21)|((uint32_t)کوڈ<<8)|(c-current_وتھ)|0x80;
                    length=4;
                    اذهب output_بائٹاں;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائٹاں;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بائٹاں;
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
    p_دلیلاں->سروت=سروت;
    p_دلیلاں->نشانہ=(حرف *)نشانہ;
    أرجع;

output_بائٹاں:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=نشانہ_سمرتھا) {
        بدل(length) {
            /* each branch falls through to the next one */
        حالة 4:
            *نشانہ++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالة 3:
            *نشانہ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *نشانہ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *نشانہ++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            /* will never occur */
            اكسر;
        }
        نشانہ_سمرتھا-=length;

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
        length-=نشانہ_سمرتھا;
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
        cnv->char_Error_بفر_لمائی=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        بدل(نشانہ_سمرتھا) {
            /* each branch falls through to the next one */
        حالة 3:
            *نشانہ++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *نشانہ++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *نشانہ++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        نشانہ_سمرتھا=0;
        *p_Error_کوڈ=U_بفر_حد_توں_ودھ_ERROR;
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
               فراغ *ڈھیر_بفر, 
               int32_t *p_بفر_ناپ, 
               UError_کوڈ *حالت)
{
    بنية cloneSCSUStruct * localClone;
    int32_t بفر_ناپ_Needed = حجم(بنية cloneSCSUStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع خالی_سوچک;
    }

    إذا (*p_بفر_ناپ == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_ناپ = بفر_ناپ_Needed;
        أرجع خالی_سوچک;
    }

    localClone = (بنية cloneSCSUStruct *)ڈھیر_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    أرجع &localClone->cnv;
}
U_CDECL_سرا

ساكن ثابت UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    خالی_سوچک,
    خالی_سوچک,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    خالی_سوچک,

    خالی_سوچک,
    _SCSUGetName,
    خالی_سوچک,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_سیٹ,
    خالی_سوچک,
    خالی_سوچک
};

ساكن ثابت UConverter_Static_انکڑے SCSUStatic_انکڑے={
    حجم(UConverter_Static_انکڑے),
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

ثابت UConverter_Shared_انکڑے _SCSUData=
        UCNV_IMMUTABLE_SHARED_انکڑے_INITIALIZER(&SCSUStatic_انکڑے, &_SCSUImpl);

#جے_انت
