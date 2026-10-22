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
    gap_وٿي=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_شروعات=0xA8,

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
    UBool منزل_UIs_Single_Byte_گهڻائي;
    uint8_t منزل_UState;
    int8_t منزل_UQuote_دري, منزل_UDynamic_دري;
    uint8_t منزل_UByte_One;
    uint8_t منزل_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_گهڻائي;
    int8_t from_UDynamic_دري;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t ايندڙ_دري_Use_اشاريو;
    int8_t دري_Use[8];
} SCSUData;

ساكن ثابت int8_t initial_دري_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
ساكن ثابت int8_t initial_دري_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

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

        scsu->منزل_UIs_Single_Byte_گهڻائي=سچ;
        scsu->منزل_UState=readCommand;
        scsu->منزل_UQuote_دري=scsu->منزل_UDynamic_دري=0;
        scsu->منزل_UByte_One=0;

        cnv->منزل_ULength=0;
    }
    إذا(choice!=UCNV_RESET_منزل_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_گهڻائي=سچ;
        scsu->from_UDynamic_دري=0;

        scsu->ايندڙ_دري_Use_اشاريو=0;
        بدل(scsu->locale) {
        حالة l_ja:
            uprv_memcpy(scsu->دري_Use, initial_دري_Use_ja, 8);
            اكسر;
        افتراضي:
            uprv_memcpy(scsu->دري_Use, initial_دري_Use, 8);
            اكسر;
        }

        cnv->fromUChar32=0;
    }
}

ساكن فراغ U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_ڀريو_دليلون *p_دليلون,
          UError_ڪوڊ *p_Error_ڪوڊ) {
    ثابت حرف *locale=p_دليلون->locale;
    إذا(p_دليلون->onlyTestIsLoadable) {
        أرجع;
    }
    cnv->extraInfo=uprv_malloc(حجم(SCSUData));
    إذا(cnv->extraInfo!=خالي_اشارو) {
        إذا(locale!=خالي_اشارو && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } وإلا {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } وإلا {
        *p_Error_ڪوڊ=U_يادگيري_ورهاست_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

ساكن فراغ U_CALLCONV
_SCSUClose(UConverter *cnv) {
    إذا(cnv->extraInfo!=خالي_اشارو) {
        إذا(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=خالي_اشارو;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

ساكن فراغ U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_منزل_Unicode_دليلون *p_دليلون,
                          UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;
    int32_t *offsets;
    UBool is_Single_Byte_گهڻائي;
    uint8_t state, byteOne;
    int8_t quote_دري, dynamic_دري;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    ذريعو_حد_بندي=(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي;
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;
    offsets=p_دليلون->offsets;

    /* get the state machine state */
    is_Single_Byte_گهڻائي=scsu->منزل_UIs_Single_Byte_گهڻائي;
    state=scsu->منزل_UState;
    quote_دري=scsu->منزل_UQuote_دري;
    dynamic_دري=scsu->منزل_UDynamic_دري;
    byteOne=scsu->منزل_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو=state==readCommand ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

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
    إذا(is_Single_Byte_گهڻائي) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(ذريعو<ذريعو_حد_بندي && هدف<هدف_حد_بندي && (b=*ذريعو)>=0x20) {
                ++ذريعو;
                ++ايندڙ_ذريعو_اشاريو;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *هدف++=(char16_t)b;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_دري]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *هدف++=(char16_t)c;
                        إذا(offsets!=خالي_اشارو) {
                            *offsets++=ذريعو_اشاريو;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *هدف++=(char16_t)(0xd7c0+(c>>10));
                        إذا(هدف<هدف_حد_بندي) {
                            *هدف++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                                *offsets++=ذريعو_اشاريو;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_ڊيگهائي=1;
                            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_گهڻائي:
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف>=هدف_حد_بندي) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            b=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *هدف++=(char16_t)b;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_دري=(int8_t)(b-SC0);
                        ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_دري=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_دري=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                    is_Single_Byte_گهڻائي=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
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
                *هدف++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *هدف++=(char16_t)(staticOffsets[quote_دري]+b);
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_دري]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *هدف++=(char16_t)c;
                        إذا(offsets!=خالي_اشارو) {
                            *offsets++=ذريعو_اشاريو;
                        }
                    } وإلا {
                        /* output surrogate pair */
                        *هدف++=(char16_t)(0xd7c0+(c>>10));
                        إذا(هدف<هدف_حد_بندي) {
                            *هدف++=(char16_t)(0xdc00|(c&0x3ff));
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                                *offsets++=ذريعو_اشاريو;
                            }
                        } وإلا {
                            /* target overflow */
                            إذا(offsets!=خالي_اشارو) {
                                *offsets++=ذريعو_اشاريو;
                            }
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_ڊيگهائي=1;
                            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_دري=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_دري]=0x10000+(byteOne<<15UL | b<<7UL);
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_حد)<(reserved_شروعات-gap_حد)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=(b<<7UL)+gap_وٿي;
                } وإلا إذا(b>=fixed_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=fixedOffsets[b-fixed_حد];
                } وإلا {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                state=readCommand;
                اذهب fastSingle;
            }
        }
    } وإلا {
        /* fast path for Unicode mode */
        إذا(state==readCommand) {
fastUnicode:
            طالما(ذريعو+1<ذريعو_حد_بندي && هدف<هدف_حد_بندي && (uint8_t)((b=*ذريعو)-UC0)>(Urs-UC0)) {
                *هدف++=(char16_t)((b<<8)|ذريعو[1]);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                ايندڙ_ذريعو_اشاريو+=2;
                ذريعو+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف>=هدف_حد_بندي) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            b=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_دري=(int8_t)(b-UC0);
                    ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                    is_Single_Byte_گهڻائي=سچ;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_دري=(int8_t)(b-UD0);
                    is_Single_Byte_گهڻائي=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_گهڻائي;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_گهڻائي=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_گهڻائي;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
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
                *هدف++=(char16_t)((byteOne<<8)|b);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_ڪوڊ) && *p_Error_ڪوڊ!=U_بفر_حد_کان_مٿي_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_گهڻائي=is_Single_Byte_گهڻائي;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_دري=quote_دري;
    scsu->منزل_UDynamic_دري=dynamic_دري;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
    p_دليلون->هدف=هدف;
    p_دليلون->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
ساكن فراغ U_CALLCONV
_SCSUToUnicode(UConverter_منزل_Unicode_دليلون *p_دليلون,
               UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت uint8_t *ذريعو, *ذريعو_حد_بندي;
    char16_t *هدف;
    ثابت char16_t *هدف_حد_بندي;
    UBool is_Single_Byte_گهڻائي;
    uint8_t state, byteOne;
    int8_t quote_دري, dynamic_دري;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ذريعو=(ثابت uint8_t *)p_دليلون->ذريعو;
    ذريعو_حد_بندي=(ثابت uint8_t *)p_دليلون->ذريعو_حد_بندي;
    هدف=p_دليلون->هدف;
    هدف_حد_بندي=p_دليلون->هدف_حد_بندي;

    /* get the state machine state */
    is_Single_Byte_گهڻائي=scsu->منزل_UIs_Single_Byte_گهڻائي;
    state=scsu->منزل_UState;
    quote_دري=scsu->منزل_UQuote_دري;
    dynamic_دري=scsu->منزل_UDynamic_دري;
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
    إذا(is_Single_Byte_گهڻائي) {
        /* fast path for single-byte mode */
        إذا(state==readCommand) {
fastSingle:
            طالما(ذريعو<ذريعو_حد_بندي && هدف<هدف_حد_بندي && (b=*ذريعو)>=0x20) {
                ++ذريعو;
                إذا(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *هدف++=(char16_t)b;
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[dynamic_دري]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *هدف++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *هدف++=(char16_t)(0xd7c0+(c>>10));
                        إذا(هدف<هدف_حد_بندي) {
                            *هدف++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_ڊيگهائي=1;
                            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                            اذهب endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_گهڻائي:
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف>=هدف_حد_بندي) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            b=*ذريعو++;
            بدل(state) {
            حالة readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                إذا((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *هدف++=(char16_t)b;
                    اذهب fastSingle;
                } وإلا إذا(SC0<=b) {
                    إذا(b<=SC7) {
                        dynamic_دري=(int8_t)(b-SC0);
                        اذهب fastSingle;
                    } وإلا /* if(SD0<=b && b<=SD7) */ {
                        dynamic_دري=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } وإلا إذا(/* SQ0<=b && */ b<=SQ7) {
                    quote_دري=(int8_t)(b-SQ0);
                    state=quoteOne;
                } وإلا إذا(b==SDX) {
                    state=definePairOne;
                } وإلا إذا(b==SQU) {
                    state=quotePairOne;
                } وإلا إذا(b==SCU) {
                    is_Single_Byte_گهڻائي=خطأ;
                    اذهب fastUnicode;
                } وإلا /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
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
                *هدف++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastSingle;
            حالة quoteOne:
                إذا(b<0x80) {
                    /* all static offsets are in the BMP */
                    *هدف++=(char16_t)(staticOffsets[quote_دري]+b);
                } وإلا {
                    /* write from dynamic window */
                    uint32_t c=scsu->منزل_UDynamic_Offsets[quote_دري]+(b&0x7f);
                    إذا(c<=0xffff) {
                        *هدف++=(char16_t)c;
                    } وإلا {
                        /* output surrogate pair */
                        *هدف++=(char16_t)(0xd7c0+(c>>10));
                        إذا(هدف<هدف_حد_بندي) {
                            *هدف++=(char16_t)(0xdc00|(c&0x3ff));
                        } وإلا {
                            /* target overflow */
                            cnv->UChar_Error_بفر[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_بفر_ڊيگهائي=1;
                            *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                            اذهب endloop;
                        }
                    }
                }
                state=readCommand;
                اذهب fastSingle;
            حالة definePairOne:
                dynamic_دري=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->منزل_UBytes[1]=b;
                cnv->منزل_ULength=2;
                state=definePairTwo;
                اكسر;
            حالة definePairTwo:
                scsu->منزل_UDynamic_Offsets[dynamic_دري]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                اذهب fastSingle;
            حالة defineOne:
                إذا(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->منزل_UBytes[1]=b;
                    cnv->منزل_ULength=2;
                    اذهب endloop;
                } وإلا إذا(b<gap_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=b<<7UL;
                } وإلا إذا((uint8_t)(b-gap_حد)<(reserved_شروعات-gap_حد)) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=(b<<7UL)+gap_وٿي;
                } وإلا إذا(b>=fixed_حد) {
                    scsu->منزل_UDynamic_Offsets[dynamic_دري]=fixedOffsets[b-fixed_حد];
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
            طالما(ذريعو+1<ذريعو_حد_بندي && هدف<هدف_حد_بندي && (uint8_t)((b=*ذريعو)-UC0)>(Urs-UC0)) {
                *هدف++=(char16_t)((b<<8)|ذريعو[1]);
                ذريعو+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف>=هدف_حد_بندي) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            b=*ذريعو++;
            بدل(state) {
            حالة readCommand:
                إذا((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairTwo;
                } وإلا إذا(/* UC0<=b && */ b<=UC7) {
                    dynamic_دري=(int8_t)(b-UC0);
                    is_Single_Byte_گهڻائي=سچ;
                    اذهب fastSingle;
                } وإلا إذا(/* UD0<=b && */ b<=UD7) {
                    dynamic_دري=(int8_t)(b-UD0);
                    is_Single_Byte_گهڻائي=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=defineOne;
                    اذهب single_Byte_گهڻائي;
                } وإلا إذا(b==UDX) {
                    is_Single_Byte_گهڻائي=سچ;
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=definePairOne;
                    اذهب single_Byte_گهڻائي;
                } وإلا إذا(b==UQU) {
                    cnv->منزل_UBytes[0]=b;
                    cnv->منزل_ULength=1;
                    state=quotePairOne;
                } وإلا /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
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
                *هدف++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                اذهب fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    إذا(U_FAILURE(*p_Error_ڪوڊ) && *p_Error_ڪوڊ!=U_بفر_حد_کان_مٿي_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } وإلا إذا(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->منزل_ULength=0;
    }
    scsu->منزل_UIs_Single_Byte_گهڻائي=is_Single_Byte_گهڻائي;
    scsu->منزل_UState=state;
    scsu->منزل_UQuote_دري=quote_دري;
    scsu->منزل_UDynamic_دري=dynamic_دري;
    scsu->منزل_UByte_One=byteOne;

    /* write back the updated pointers */
    p_دليلون->ذريعو=(ثابت حرف *)ذريعو;
    p_دليلون->هدف=هدف;
}
U_CDECL_پڇاڙي
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
get_دري(ثابت uint32_t offsets[8], uint32_t c) {
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
is_اندريون_وٿي_دري_Or_سڌو(uint32_t وٿي, uint32_t c) {
    أرجع c<=وٿي+0x7f &&
          (c>=وٿي || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
ساكن int8_t
get_ايندڙ_Dynamic_دري(SCSUData *scsu) {
    int8_t دري=scsu->دري_Use[scsu->ايندڙ_دري_Use_اشاريو];
    إذا(++scsu->ايندڙ_دري_Use_اشاريو==8) {
        scsu->ايندڙ_دري_Use_اشاريو=0;
    }
    أرجع دري;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
ساكن فراغ
use_Dynamic_دري(SCSUData *scsu, int8_t دري) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    صحيح i, j;

    i=scsu->ايندڙ_دري_Use_اشاريو;
    افعل {
        إذا(--i<0) {
            i=7;
        }
    } طالما(scsu->دري_Use[i]!=دري);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    إذا(j==8) {
        j=0;
    }
    طالما(j!=scsu->ايندڙ_دري_Use_اشاريو) {
        scsu->دري_Use[i]=scsu->دري_Use[j];
        i=j;
        إذا(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->دري_Use[i]=دري;
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
get_Dynamic_وٿي(uint32_t c, uint32_t *p_وٿي) {
    صحيح i;

    لكل(i=0; i<7; ++i) {
        إذا (c - fixedOffsets[i] <= 0x7f) {
            *p_وٿي=fixedOffsets[i];
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
        *p_وٿي=c&0x7fffff80;
        أرجع static_cast<صحيح>(c >> 7);
    } وإلا إذا(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_وٿي=c&0x7fffff80;
        أرجع static_cast<صحيح>((c - gap_وٿي) >> 7);
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
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_دليلون *p_دليلون,
                            UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;
    int32_t *offsets;

    UBool is_Single_Byte_گهڻائي;
    uint8_t dynamic_دري;
    uint32_t current_وٿي;

    uint32_t c, delta;

    int32_t ذريعو_اشاريو, ايندڙ_ذريعو_اشاريو;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t وٿي;
    char16_t lead, trail;
    صحيح ڪوڊ;
    int8_t دري;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف=(uint8_t *)p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);
    offsets=p_دليلون->offsets;

    /* get the state machine state */
    is_Single_Byte_گهڻائي=scsu->from_UIs_Single_Byte_گهڻائي;
    dynamic_دري=scsu->from_UDynamic_دري;
    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ذريعو_اشاريو= c==0 ? 0 : -1;
    ايندڙ_ذريعو_اشاريو=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_گهڻائي) {
        إذا(c!=0 && هدف_صلاحيت>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف_صلاحيت<=0) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *هدف++=(uint8_t)c;
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                --هدف_صلاحيت;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *هدف++=(uint8_t)c;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    --هدف_صلاحيت;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا((delta=c-current_وٿي)<=0x7f) {
                /* use the current dynamic window */
                *هدف++=(uint8_t)(delta|0x80);
                إذا(offsets!=خالي_اشارو) {
                    *offsets++=ذريعو_اشاريو;
                }
                --هدف_صلاحيت;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(ذريعو<ذريعو_حد_بندي) {
                        /* test the following code unit */
                        trail=*ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++ذريعو;
                            ++ايندڙ_ذريعو_اشاريو;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_وٿي)<=0x7f) {
                    /* use the current dynamic window */
                    *هدف++=(uint8_t)(delta|0x80);
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    --هدف_صلاحيت;
                } وإلا إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_دري=دري;
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(SC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ڪوڊ-=0x200;
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_دري<<21)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=4;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_گهڻائي=خطأ;
                    *هدف++=(uint8_t)SCU;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                    }
                    --هدف_صلاحيت;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بائيٽون;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بائيٽون;
            } وإلا {
                /* compress all other BMP characters */
                إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(ذريعو>=ذريعو_حد_بندي || is_اندريون_وٿي_دري_Or_سڌو(scsu->fromUDynamicOffsets[دري], *ذريعو)) {
                        /* change to dynamic window */
                        dynamic_دري=دري;
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(SC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+دري)<<8)|(c-scsu->fromUDynamicOffsets[دري])|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    }
                } وإلا إذا((دري=get_دري(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+دري)<<8)|(c-staticOffsets[دري]);
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(SD0+dynamic_دري)<<16)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=3;
                    اذهب output_بائيٽون;
                } وإلا إذا ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (ذريعو >= ذريعو_حد_بندي || (uint32_t)(*ذريعو - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_گهڻائي=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بائيٽون;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
        }
    } وإلا {
        إذا(c!=0 && هدف_صلاحيت>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف_صلاحيت<=0) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            c=*ذريعو++;
            ++ايندڙ_ذريعو_اشاريو;

            إذا ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                إذا(هدف_صلاحيت>=2) {
                    *هدف++=(uint8_t)(c>>8);
                    *هدف++=(uint8_t)c;
                    إذا(offsets!=خالي_اشارو) {
                        *offsets++=ذريعو_اشاريو;
                        *offsets++=ذريعو_اشاريو;
                    }
                    هدف_صلاحيت-=2;
                } وإلا {
                    length=2;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(ذريعو<ذريعو_حد_بندي && (uint32_t)(*ذريعو-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_گهڻائي=سچ;
                        c|=((uint32_t)(UC0+dynamic_دري)<<8)|c;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_گهڻائي=سچ;
                        dynamic_دري=دري;
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(UC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_گهڻائي=سچ;
                        dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(UD0+dynamic_دري)<<16)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                        length=3;
                        اذهب output_بائيٽون;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بائيٽون;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(ذريعو<ذريعو_حد_بندي) {
                        /* test the following code unit */
                        trail=*ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++ذريعو;
                            ++ايندڙ_ذريعو_اشاريو;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (دري=get_دري(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ذريعو<ذريعو_حد_بندي && (uint32_t)(*ذريعو-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_گهڻائي=سچ;
                    dynamic_دري=دري;
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(UC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا(ذريعو<ذريعو_حد_بندي && lead==*ذريعو && /* too lazy to check trail in same window as source[1] */
                          (ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_گهڻائي=سچ;
                    ڪوڊ-=0x200;
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_دري<<21)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=4;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائيٽون;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بائيٽون;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_گهڻائي=is_Single_Byte_گهڻائي;
    scsu->from_UDynamic_دري=dynamic_دري;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف=(حرف *)هدف;
    p_دليلون->offsets=offsets;
    أرجع;

output_بائيٽون:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=هدف_صلاحيت) {
        إذا(offsets==خالي_اشارو) {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *هدف++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            حالة 3:
                *هدف++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            حالة 2:
                *هدف++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            حالة 1:
                *هدف++=(uint8_t)c;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        } وإلا {
            بدل(length) {
                /* each branch falls through to the next one */
            حالة 4:
                *هدف++=(uint8_t)(c>>24);
                *offsets++=ذريعو_اشاريو;
                U_FALLTHROUGH;
            حالة 3:
                *هدف++=(uint8_t)(c>>16);
                *offsets++=ذريعو_اشاريو;
                U_FALLTHROUGH;
            حالة 2:
                *هدف++=(uint8_t)(c>>8);
                *offsets++=ذريعو_اشاريو;
                U_FALLTHROUGH;
            حالة 1:
                *هدف++=(uint8_t)c;
                *offsets++=ذريعو_اشاريو;
                U_FALLTHROUGH;
            افتراضي:
                /* will never occur */
                اكسر;
            }
        }
        هدف_صلاحيت-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ذريعو_اشاريو=ايندڙ_ذريعو_اشاريو;
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
        length-=هدف_صلاحيت;
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
        cnv->char_Error_بفر_ڊيگهائي=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        بدل(هدف_صلاحيت) {
            /* each branch falls through to the next one */
        حالة 3:
            *هدف++=(uint8_t)(c>>16);
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            U_FALLTHROUGH;
        حالة 2:
            *هدف++=(uint8_t)(c>>8);
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            U_FALLTHROUGH;
        حالة 1:
            *هدف++=(uint8_t)c;
            إذا(offsets!=خالي_اشارو) {
                *offsets++=ذريعو_اشاريو;
            }
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        هدف_صلاحيت=0;
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
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
_SCSUFromUnicode(UConverter_From_Unicode_دليلون *p_دليلون,
                 UError_ڪوڊ *p_Error_ڪوڊ) {
    UConverter *cnv;
    SCSUData *scsu;
    ثابت char16_t *ذريعو, *ذريعو_حد_بندي;
    uint8_t *هدف;
    int32_t هدف_صلاحيت;

    UBool is_Single_Byte_گهڻائي;
    uint8_t dynamic_دري;
    uint32_t current_وٿي;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t وٿي;
    char16_t lead, trail;
    صحيح ڪوڊ;
    int8_t دري;

    /* set up the local pointers */
    cnv=p_دليلون->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ذريعو=p_دليلون->ذريعو;
    ذريعو_حد_بندي=p_دليلون->ذريعو_حد_بندي;
    هدف=(uint8_t *)p_دليلون->هدف;
    هدف_صلاحيت=(int32_t)(p_دليلون->هدف_حد_بندي-p_دليلون->هدف);

    /* get the state machine state */
    is_Single_Byte_گهڻائي=scsu->from_UIs_Single_Byte_گهڻائي;
    dynamic_دري=scsu->from_UDynamic_دري;
    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    إذا(is_Single_Byte_گهڻائي) {
        إذا(c!=0 && هدف_صلاحيت>0) {
            اذهب getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف_صلاحيت<=0) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            c=*ذريعو++;

            إذا((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *هدف++=(uint8_t)c;
                --هدف_صلاحيت;
            } وإلا إذا(c<0x20) {
                إذا((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *هدف++=(uint8_t)c;
                    --هدف_صلاحيت;
                } وإلا {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا((delta=c-current_وٿي)<=0x7f) {
                /* use the current dynamic window */
                *هدف++=(uint8_t)(delta|0x80);
                --هدف_صلاحيت;
            } وإلا إذا(U16_IS_SURROGATE(c)) {
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    إذا(ذريعو<ذريعو_حد_بندي) {
                        /* test the following code unit */
                        trail=*ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++ذريعو;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                إذا((delta=c-current_وٿي)<=0x7f) {
                    /* use the current dynamic window */
                    *هدف++=(uint8_t)(delta|0x80);
                    --هدف_صلاحيت;
                } وإلا إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_دري=دري;
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(SC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    ڪوڊ-=0x200;
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_دري<<21)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=4;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_گهڻائي=خطأ;
                    *هدف++=(uint8_t)SCU;
                    --هدف_صلاحيت;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                اذهب output_بائيٽون;
            } وإلا إذا(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                اذهب output_بائيٽون;
            } وإلا {
                /* compress all other BMP characters */
                إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    إذا(ذريعو>=ذريعو_حد_بندي || is_اندريون_وٿي_دري_Or_سڌو(scsu->fromUDynamicOffsets[دري], *ذريعو)) {
                        /* change to dynamic window */
                        dynamic_دري=دري;
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(SC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+دري)<<8)|(c-scsu->fromUDynamicOffsets[دري])|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    }
                } وإلا إذا((دري=get_دري(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+دري)<<8)|(c-staticOffsets[دري]);
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(SD0+dynamic_دري)<<16)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=3;
                    اذهب output_بائيٽون;
                } وإلا إذا (c - 0x3400 < 0xd800 - 0x3400 &&
                           (ذريعو >= ذريعو_حد_بندي || static_cast<uint32_t>(*ذريعو - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_گهڻائي=خطأ;
                    c|=SCU<<16;
                    length=3;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    اذهب output_بائيٽون;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } وإلا {
        إذا(c!=0 && هدف_صلاحيت>0) {
            اذهب getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        طالما(ذريعو<ذريعو_حد_بندي) {
            إذا(هدف_صلاحيت<=0) {
                /* target is full */
                *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
                اكسر;
            }
            c=*ذريعو++;

            إذا (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                إذا(هدف_صلاحيت>=2) {
                    *هدف++=(uint8_t)(c>>8);
                    *هدف++=(uint8_t)c;
                    هدف_صلاحيت-=2;
                } وإلا {
                    length=2;
                    اذهب output_بائيٽون;
                }
            } وإلا إذا (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                إذا(!(ذريعو<ذريعو_حد_بندي && (uint32_t)(*ذريعو-0x3400)<(0xd800-0x3400))) {
                    إذا (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_گهڻائي=سچ;
                        c|=((uint32_t)(UC0+dynamic_دري)<<8)|c;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا إذا((دري=get_دري(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_گهڻائي=سچ;
                        dynamic_دري=دري;
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(UC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                        length=2;
                        اذهب output_بائيٽون;
                    } وإلا إذا((ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_گهڻائي=سچ;
                        dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                        current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                        use_Dynamic_دري(scsu, dynamic_دري);
                        c=((uint32_t)(UD0+dynamic_دري)<<16)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                        length=3;
                        اذهب output_بائيٽون;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                اذهب output_بائيٽون;
            } وإلا إذا(c<0xe000) {
                /* c is a surrogate */
                إذا(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    إذا(ذريعو<ذريعو_حد_بندي) {
                        /* test the following code unit */
                        trail=*ذريعو;
                        إذا(U16_IS_TRAIL(trail)) {
                            ++ذريعو;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } وإلا {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                            اذهب endloop;
                        }
                    } وإلا {
                        /* no more input */
                        اكسر;
                    }
                } وإلا {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_ڪوڊ=U_ILLEGAL_CHAR_مليو;
                    اذهب endloop;
                }

                /* compress supplementary character */
                إذا( (دري=get_دري(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ذريعو<ذريعو_حد_بندي && (uint32_t)(*ذريعو-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_گهڻائي=سچ;
                    dynamic_دري=دري;
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري];
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)(UC0+dynamic_دري)<<8)|(c-current_وٿي)|0x80;
                    length=2;
                    اذهب output_بائيٽون;
                } وإلا إذا(ذريعو<ذريعو_حد_بندي && lead==*ذريعو && /* too lazy to check trail in same window as source[1] */
                          (ڪوڊ=get_Dynamic_وٿي(c, &وٿي))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_گهڻائي=سچ;
                    ڪوڊ-=0x200;
                    dynamic_دري=get_ايندڙ_Dynamic_دري(scsu);
                    current_وٿي=scsu->fromUDynamicOffsets[dynamic_دري]=وٿي;
                    use_Dynamic_دري(scsu, dynamic_دري);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_دري<<21)|((uint32_t)ڪوڊ<<8)|(c-current_وٿي)|0x80;
                    length=4;
                    اذهب output_بائيٽون;
                } وإلا {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    اذهب output_بائيٽون;
                }
            } وإلا /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                اذهب output_بائيٽون;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_گهڻائي=is_Single_Byte_گهڻائي;
    scsu->from_UDynamic_دري=dynamic_دري;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_دليلون->ذريعو=ذريعو;
    p_دليلون->هدف=(حرف *)هدف;
    أرجع;

output_بائيٽون:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    إذا(length<=هدف_صلاحيت) {
        بدل(length) {
            /* each branch falls through to the next one */
        حالة 4:
            *هدف++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        حالة 3:
            *هدف++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *هدف++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *هدف++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            /* will never occur */
            اكسر;
        }
        هدف_صلاحيت-=length;

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
        length-=هدف_صلاحيت;
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
        cnv->char_Error_بفر_ڊيگهائي=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        بدل(هدف_صلاحيت) {
            /* each branch falls through to the next one */
        حالة 3:
            *هدف++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        حالة 2:
            *هدف++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        حالة 1:
            *هدف++=(uint8_t)c;
            U_FALLTHROUGH;
        افتراضي:
            اكسر;
        }

        /* target overflow */
        هدف_صلاحيت=0;
        *p_Error_ڪوڊ=U_بفر_حد_کان_مٿي_ERROR;
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
               فراغ *ڍير_بفر, 
               int32_t *p_بفر_سائيز, 
               UError_ڪوڊ *حالت)
{
    بنية cloneSCSUStruct * localClone;
    int32_t بفر_سائيز_Needed = حجم(بنية cloneSCSUStruct);

    إذا (U_FAILURE(*حالت)){
        أرجع خالي_اشارو;
    }

    إذا (*p_بفر_سائيز == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_بفر_سائيز = بفر_سائيز_Needed;
        أرجع خالي_اشارو;
    }

    localClone = (بنية cloneSCSUStruct *)ڍير_بفر;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, حجم(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = سچ;

    أرجع &localClone->cnv;
}
U_CDECL_پڇاڙي

ساكن ثابت UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    خالي_اشارو,
    خالي_اشارو,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    خالي_اشارو,

    خالي_اشارو,
    _SCSUGetName,
    خالي_اشارو,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_سيٽ,
    خالي_اشارو,
    خالي_اشارو
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

#جيڪڏهن_پورو
