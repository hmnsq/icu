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

#अन्तर्भाव "unicode/utypes.h"

#यदि !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#अन्तर्भाव "unicode/ucnv.h"
#अन्तर्भाव "unicode/ucnv_cb.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "ucnv_bld.h"
#अन्तर्भाव "ucnv_cnv.h"
#अन्तर्भाव "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
गणना {
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

गणना {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_देहली=0x68,
    gap_विस्थापनम्=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reservedStart=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_देहली=0xF9
};

/* constant offsets for the 8 static windows */
स्थिर ध्रुव uint32_t staticOffsets[8]={
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
स्थिर ध्रुव uint32_t initialDynamicOffsets[8]={
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
स्थिर ध्रुव uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
गणना {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

प्रकारनिर्देश संरचना SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t गन्तव्यम्_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool गन्तव्यम्_UIs_Single_Byte_बहुलकम्;
    uint8_t गन्तव्यम्_UState;
    int8_t गन्तव्यम्_UQuote_गवाक्षः, गन्तव्यम्_UDynamic_गवाक्षः;
    uint8_t गन्तव्यम्_UByte_One;
    uint8_t गन्तव्यम्_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_बहुलकम्;
    int8_t from_UDynamic_गवाक्षः;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t अग्र_गवाक्षः_Use_सूचकांक;
    int8_t गवाक्षः_Use[8];
} SCSUData;

स्थिर ध्रुव int8_t initial_गवाक्षः_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
स्थिर ध्रुव int8_t initial_गवाक्षः_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

गणना {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
स्थिर शून्य U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    यदि(choice<=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->गन्तव्यम्_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->गन्तव्यम्_UIs_Single_Byte_बहुलकम्=सत्य;
        scsu->गन्तव्यम्_UState=readCommand;
        scsu->गन्तव्यम्_UQuote_गवाक्षः=scsu->गन्तव्यम्_UDynamic_गवाक्षः=0;
        scsu->गन्तव्यम्_UByte_One=0;

        cnv->गन्तव्यम्_ULength=0;
    }
    यदि(choice!=UCNV_RESET_गन्तव्यम्_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_बहुलकम्=सत्य;
        scsu->from_UDynamic_गवाक्षः=0;

        scsu->अग्र_गवाक्षः_Use_सूचकांक=0;
        विकल्प(scsu->locale) {
        पक्ष l_ja:
            uprv_memcpy(scsu->गवाक्षः_Use, initial_गवाक्षः_Use_ja, 8);
            भङ्ग;
        स्वभाव:
            uprv_memcpy(scsu->गवाक्षः_Use, initial_गवाक्षः_Use, 8);
            भङ्ग;
        }

        cnv->fromUChar32=0;
    }
}

स्थिर शून्य U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_आरोपय_तर्काः *p_तर्काः,
          UError_संहिता *p_Error_संहिता) {
    ध्रुव वर्ण *locale=p_तर्काः->locale;
    यदि(p_तर्काः->onlyTestIsLoadable) {
        प्रत्यागम;
    }
    cnv->extraInfo=uprv_malloc(परिमाण(SCSUData));
    यदि(cnv->extraInfo!=शून्यसूचकः) {
        यदि(locale!=शून्यसूचकः && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } अन्यथा {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } अन्यथा {
        *p_Error_संहिता=U_स्मृतिः_आवण्टनम्_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

स्थिर शून्य U_CALLCONV
_SCSUClose(UConverter *cnv) {
    यदि(cnv->extraInfo!=शून्यसूचकः) {
        यदि(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=शून्यसूचकः;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

स्थिर शून्य U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
                          UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    SCSUData *scsu;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    int32_t *offsets;
    UBool is_Single_Byte_बहुलकम्;
    uint8_t state, byteOne;
    int8_t quote_गवाक्षः, dynamic_गवाक्षः;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;
    offsets=p_तर्काः->offsets;

    /* get the state machine state */
    is_Single_Byte_बहुलकम्=scsu->गन्तव्यम्_UIs_Single_Byte_बहुलकम्;
    state=scsu->गन्तव्यम्_UState;
    quote_गवाक्षः=scsu->गन्तव्यम्_UQuote_गवाक्षः;
    dynamic_गवाक्षः=scsu->गन्तव्यम्_UDynamic_गवाक्षः;
    byteOne=scsu->गन्तव्यम्_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक=state==readCommand ? 0 : -1;
    अग्र_Source_सूचकांक=0;

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
    यदि(is_Single_Byte_बहुलकम्) {
        /* fast path for single-byte mode */
        यदि(state==readCommand) {
fastSingle:
            यावत्(source<source_सीमा && target<target_सीमा && (b=*source)>=0x20) {
                ++source;
                ++अग्र_Source_सूचकांक;
                यदि(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *target++=(char16_t)b;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                } अन्यथा {
                    /* write from dynamic window */
                    uint32_t c=scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]+(b&0x7f);
                    यदि(c<=0xffff) {
                        *target++=(char16_t)c;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                        }
                    } अन्यथा {
                        /* output surrogate pair */
                        *target++=(char16_t)(0xd7c0+(c>>10));
                        यदि(target<target_सीमा) {
                            *target++=(char16_t)(0xdc00|(c&0x3ff));
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                *offsets++=source_सूचकांक;
                            }
                        } अन्यथा {
                            /* target overflow */
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                            }
                            cnv->UChar_Error_बफर[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            गच्छ endloop;
                        }
                    }
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_बहुलकम्:
        यावत्(source<source_सीमा) {
            यदि(target>=target_सीमा) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            b=*source++;
            ++अग्र_Source_सूचकांक;
            विकल्प(state) {
            पक्ष readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                यदि((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *target++=(char16_t)b;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    source_सूचकांक=अग्र_Source_सूचकांक;
                    गच्छ fastSingle;
                } अन्यथा यदि(SC0<=b) {
                    यदि(b<=SC7) {
                        dynamic_गवाक्षः=(int8_t)(b-SC0);
                        source_सूचकांक=अग्र_Source_सूचकांक;
                        गच्छ fastSingle;
                    } अन्यथा /* if(SD0<=b && b<=SD7) */ {
                        dynamic_गवाक्षः=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } अन्यथा यदि(/* SQ0<=b && */ b<=SQ7) {
                    quote_गवाक्षः=(int8_t)(b-SQ0);
                    state=quoteOne;
                } अन्यथा यदि(b==SDX) {
                    state=definePairOne;
                } अन्यथा यदि(b==SQU) {
                    state=quotePairOne;
                } अन्यथा यदि(b==SCU) {
                    source_सूचकांक=अग्र_Source_सूचकांक;
                    is_Single_Byte_बहुलकम्=असत्य;
                    गच्छ fastUnicode;
                } अन्यथा /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    गच्छ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->गन्तव्यम्_UBytes[0]=b;
                cnv->गन्तव्यम्_ULength=1;
                भङ्ग;
            पक्ष quotePairOne:
                byteOne=b;
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=quotePairTwo;
                भङ्ग;
            पक्ष quotePairTwo:
                *target++=(char16_t)((byteOne<<8)|b);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
                state=readCommand;
                गच्छ fastSingle;
            पक्ष quoteOne:
                यदि(b<0x80) {
                    /* all static offsets are in the BMP */
                    *target++=(char16_t)(staticOffsets[quote_गवाक्षः]+b);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                } अन्यथा {
                    /* write from dynamic window */
                    uint32_t c=scsu->गन्तव्यम्_UDynamic_Offsets[quote_गवाक्षः]+(b&0x7f);
                    यदि(c<=0xffff) {
                        *target++=(char16_t)c;
                        यदि(offsets!=शून्यसूचकः) {
                            *offsets++=source_सूचकांक;
                        }
                    } अन्यथा {
                        /* output surrogate pair */
                        *target++=(char16_t)(0xd7c0+(c>>10));
                        यदि(target<target_सीमा) {
                            *target++=(char16_t)(0xdc00|(c&0x3ff));
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                                *offsets++=source_सूचकांक;
                            }
                        } अन्यथा {
                            /* target overflow */
                            यदि(offsets!=शून्यसूचकः) {
                                *offsets++=source_सूचकांक;
                            }
                            cnv->UChar_Error_बफर[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            गच्छ endloop;
                        }
                    }
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
                state=readCommand;
                गच्छ fastSingle;
            पक्ष definePairOne:
                dynamic_गवाक्षः=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=definePairTwo;
                भङ्ग;
            पक्ष definePairTwo:
                scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=0x10000+(byteOne<<15UL | b<<7UL);
                source_सूचकांक=अग्र_Source_सूचकांक;
                state=readCommand;
                गच्छ fastSingle;
            पक्ष defineOne:
                यदि(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->गन्तव्यम्_UBytes[1]=b;
                    cnv->गन्तव्यम्_ULength=2;
                    गच्छ endloop;
                } अन्यथा यदि(b<gap_देहली) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=b<<7UL;
                } अन्यथा यदि((uint8_t)(b-gap_देहली)<(reservedStart-gap_देहली)) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=(b<<7UL)+gap_विस्थापनम्;
                } अन्यथा यदि(b>=fixed_देहली) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=fixedOffsets[b-fixed_देहली];
                } अन्यथा {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->गन्तव्यम्_UBytes[1]=b;
                    cnv->गन्तव्यम्_ULength=2;
                    गच्छ endloop;
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
                state=readCommand;
                गच्छ fastSingle;
            }
        }
    } अन्यथा {
        /* fast path for Unicode mode */
        यदि(state==readCommand) {
fastUnicode:
            यावत्(source+1<source_सीमा && target<target_सीमा && (uint8_t)((b=*source)-UC0)>(Urs-UC0)) {
                *target++=(char16_t)((b<<8)|source[1]);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
                अग्र_Source_सूचकांक+=2;
                source+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target>=target_सीमा) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            b=*source++;
            ++अग्र_Source_सूचकांक;
            विकल्प(state) {
            पक्ष readCommand:
                यदि((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=quotePairTwo;
                } अन्यथा यदि(/* UC0<=b && */ b<=UC7) {
                    dynamic_गवाक्षः=(int8_t)(b-UC0);
                    source_सूचकांक=अग्र_Source_सूचकांक;
                    is_Single_Byte_बहुलकम्=सत्य;
                    गच्छ fastSingle;
                } अन्यथा यदि(/* UD0<=b && */ b<=UD7) {
                    dynamic_गवाक्षः=(int8_t)(b-UD0);
                    is_Single_Byte_बहुलकम्=सत्य;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=defineOne;
                    गच्छ single_Byte_बहुलकम्;
                } अन्यथा यदि(b==UDX) {
                    is_Single_Byte_बहुलकम्=सत्य;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=definePairOne;
                    गच्छ single_Byte_बहुलकम्;
                } अन्यथा यदि(b==UQU) {
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=quotePairOne;
                } अन्यथा /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    गच्छ endloop;
                }
                भङ्ग;
            पक्ष quotePairOne:
                byteOne=b;
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=quotePairTwo;
                भङ्ग;
            पक्ष quotePairTwo:
                *target++=(char16_t)((byteOne<<8)|b);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                source_सूचकांक=अग्र_Source_सूचकांक;
                state=readCommand;
                गच्छ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    यदि(U_FAILURE(*p_Error_संहिता) && *p_Error_संहिता!=U_बफर_अतिप्रवाहः_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } अन्यथा यदि(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->गन्तव्यम्_ULength=0;
    }
    scsu->गन्तव्यम्_UIs_Single_Byte_बहुलकम्=is_Single_Byte_बहुलकम्;
    scsu->गन्तव्यम्_UState=state;
    scsu->गन्तव्यम्_UQuote_गवाक्षः=quote_गवाक्षः;
    scsu->गन्तव्यम्_UDynamic_गवाक्षः=dynamic_गवाक्षः;
    scsu->गन्तव्यम्_UByte_One=byteOne;

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
    p_तर्काः->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
स्थिर शून्य U_CALLCONV
_SCSUToUnicode(UConverter_गन्तव्यम्_Unicode_तर्काः *p_तर्काः,
               UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    SCSUData *scsu;
    ध्रुव uint8_t *source, *source_सीमा;
    char16_t *target;
    ध्रुव char16_t *target_सीमा;
    UBool is_Single_Byte_बहुलकम्;
    uint8_t state, byteOne;
    int8_t quote_गवाक्षः, dynamic_गवाक्षः;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    source=(ध्रुव uint8_t *)p_तर्काः->source;
    source_सीमा=(ध्रुव uint8_t *)p_तर्काः->source_सीमा;
    target=p_तर्काः->target;
    target_सीमा=p_तर्काः->target_सीमा;

    /* get the state machine state */
    is_Single_Byte_बहुलकम्=scsu->गन्तव्यम्_UIs_Single_Byte_बहुलकम्;
    state=scsu->गन्तव्यम्_UState;
    quote_गवाक्षः=scsu->गन्तव्यम्_UQuote_गवाक्षः;
    dynamic_गवाक्षः=scsu->गन्तव्यम्_UDynamic_गवाक्षः;
    byteOne=scsu->गन्तव्यम्_UByte_One;

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
    यदि(is_Single_Byte_बहुलकम्) {
        /* fast path for single-byte mode */
        यदि(state==readCommand) {
fastSingle:
            यावत्(source<source_सीमा && target<target_सीमा && (b=*source)>=0x20) {
                ++source;
                यदि(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *target++=(char16_t)b;
                } अन्यथा {
                    /* write from dynamic window */
                    uint32_t c=scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]+(b&0x7f);
                    यदि(c<=0xffff) {
                        *target++=(char16_t)c;
                    } अन्यथा {
                        /* output surrogate pair */
                        *target++=(char16_t)(0xd7c0+(c>>10));
                        यदि(target<target_सीमा) {
                            *target++=(char16_t)(0xdc00|(c&0x3ff));
                        } अन्यथा {
                            /* target overflow */
                            cnv->UChar_Error_बफर[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            गच्छ endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_बहुलकम्:
        यावत्(source<source_सीमा) {
            यदि(target>=target_सीमा) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            b=*source++;
            विकल्प(state) {
            पक्ष readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                यदि((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *target++=(char16_t)b;
                    गच्छ fastSingle;
                } अन्यथा यदि(SC0<=b) {
                    यदि(b<=SC7) {
                        dynamic_गवाक्षः=(int8_t)(b-SC0);
                        गच्छ fastSingle;
                    } अन्यथा /* if(SD0<=b && b<=SD7) */ {
                        dynamic_गवाक्षः=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } अन्यथा यदि(/* SQ0<=b && */ b<=SQ7) {
                    quote_गवाक्षः=(int8_t)(b-SQ0);
                    state=quoteOne;
                } अन्यथा यदि(b==SDX) {
                    state=definePairOne;
                } अन्यथा यदि(b==SQU) {
                    state=quotePairOne;
                } अन्यथा यदि(b==SCU) {
                    is_Single_Byte_बहुलकम्=असत्य;
                    गच्छ fastUnicode;
                } अन्यथा /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    गच्छ endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->गन्तव्यम्_UBytes[0]=b;
                cnv->गन्तव्यम्_ULength=1;
                भङ्ग;
            पक्ष quotePairOne:
                byteOne=b;
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=quotePairTwo;
                भङ्ग;
            पक्ष quotePairTwo:
                *target++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                गच्छ fastSingle;
            पक्ष quoteOne:
                यदि(b<0x80) {
                    /* all static offsets are in the BMP */
                    *target++=(char16_t)(staticOffsets[quote_गवाक्षः]+b);
                } अन्यथा {
                    /* write from dynamic window */
                    uint32_t c=scsu->गन्तव्यम्_UDynamic_Offsets[quote_गवाक्षः]+(b&0x7f);
                    यदि(c<=0xffff) {
                        *target++=(char16_t)c;
                    } अन्यथा {
                        /* output surrogate pair */
                        *target++=(char16_t)(0xd7c0+(c>>10));
                        यदि(target<target_सीमा) {
                            *target++=(char16_t)(0xdc00|(c&0x3ff));
                        } अन्यथा {
                            /* target overflow */
                            cnv->UChar_Error_बफर[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_बफर_दीर्घता=1;
                            *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                            गच्छ endloop;
                        }
                    }
                }
                state=readCommand;
                गच्छ fastSingle;
            पक्ष definePairOne:
                dynamic_गवाक्षः=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=definePairTwo;
                भङ्ग;
            पक्ष definePairTwo:
                scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                गच्छ fastSingle;
            पक्ष defineOne:
                यदि(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->गन्तव्यम्_UBytes[1]=b;
                    cnv->गन्तव्यम्_ULength=2;
                    गच्छ endloop;
                } अन्यथा यदि(b<gap_देहली) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=b<<7UL;
                } अन्यथा यदि((uint8_t)(b-gap_देहली)<(reservedStart-gap_देहली)) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=(b<<7UL)+gap_विस्थापनम्;
                } अन्यथा यदि(b>=fixed_देहली) {
                    scsu->गन्तव्यम्_UDynamic_Offsets[dynamic_गवाक्षः]=fixedOffsets[b-fixed_देहली];
                } अन्यथा {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->गन्तव्यम्_UBytes[1]=b;
                    cnv->गन्तव्यम्_ULength=2;
                    गच्छ endloop;
                }
                state=readCommand;
                गच्छ fastSingle;
            }
        }
    } अन्यथा {
        /* fast path for Unicode mode */
        यदि(state==readCommand) {
fastUnicode:
            यावत्(source+1<source_सीमा && target<target_सीमा && (uint8_t)((b=*source)-UC0)>(Urs-UC0)) {
                *target++=(char16_t)((b<<8)|source[1]);
                source+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target>=target_सीमा) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            b=*source++;
            विकल्प(state) {
            पक्ष readCommand:
                यदि((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=quotePairTwo;
                } अन्यथा यदि(/* UC0<=b && */ b<=UC7) {
                    dynamic_गवाक्षः=(int8_t)(b-UC0);
                    is_Single_Byte_बहुलकम्=सत्य;
                    गच्छ fastSingle;
                } अन्यथा यदि(/* UD0<=b && */ b<=UD7) {
                    dynamic_गवाक्षः=(int8_t)(b-UD0);
                    is_Single_Byte_बहुलकम्=सत्य;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=defineOne;
                    गच्छ single_Byte_बहुलकम्;
                } अन्यथा यदि(b==UDX) {
                    is_Single_Byte_बहुलकम्=सत्य;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=definePairOne;
                    गच्छ single_Byte_बहुलकम्;
                } अन्यथा यदि(b==UQU) {
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    state=quotePairOne;
                } अन्यथा /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    cnv->गन्तव्यम्_UBytes[0]=b;
                    cnv->गन्तव्यम्_ULength=1;
                    गच्छ endloop;
                }
                भङ्ग;
            पक्ष quotePairOne:
                byteOne=b;
                cnv->गन्तव्यम्_UBytes[1]=b;
                cnv->गन्तव्यम्_ULength=2;
                state=quotePairTwo;
                भङ्ग;
            पक्ष quotePairTwo:
                *target++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                गच्छ fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    यदि(U_FAILURE(*p_Error_संहिता) && *p_Error_संहिता!=U_बफर_अतिप्रवाहः_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } अन्यथा यदि(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->गन्तव्यम्_ULength=0;
    }
    scsu->गन्तव्यम्_UIs_Single_Byte_बहुलकम्=is_Single_Byte_बहुलकम्;
    scsu->गन्तव्यम्_UState=state;
    scsu->गन्तव्यम्_UQuote_गवाक्षः=quote_गवाक्षः;
    scsu->गन्तव्यम्_UDynamic_गवाक्षः=dynamic_गवाक्षः;
    scsu->गन्तव्यम्_UByte_One=byteOne;

    /* write back the updated pointers */
    p_तर्काः->source=(ध्रुव वर्ण *)source;
    p_तर्काः->target=target;
}
U_CDECL_अन्तः
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
स्थिर int8_t
get_गवाक्षः(ध्रुव uint32_t offsets[8], uint32_t c) {
    पूर्णाङ्क i;
    प्रति(i=0; i<8; ++i) {
        यदि (c - offsets[i] <= 0x7f) {
            प्रत्यागम static_cast<int8_t>(i);
        }
    }
    प्रत्यागम -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
स्थिर UBool
is_अन्तर्_विस्थापनम्_गवाक्षः_Or_प्रत्यक्षम्(uint32_t विस्थापनम्, uint32_t c) {
    प्रत्यागम c<=विस्थापनम्+0x7f &&
          (c>=विस्थापनम् || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
स्थिर int8_t
get_अग्र_Dynamic_गवाक्षः(SCSUData *scsu) {
    int8_t गवाक्षः=scsu->गवाक्षः_Use[scsu->अग्र_गवाक्षः_Use_सूचकांक];
    यदि(++scsu->अग्र_गवाक्षः_Use_सूचकांक==8) {
        scsu->अग्र_गवाक्षः_Use_सूचकांक=0;
    }
    प्रत्यागम गवाक्षः;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
स्थिर शून्य
use_Dynamic_गवाक्षः(SCSUData *scsu, int8_t गवाक्षः) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    पूर्णाङ्क i, j;

    i=scsu->अग्र_गवाक्षः_Use_सूचकांक;
    कुरु {
        यदि(--i<0) {
            i=7;
        }
    } यावत्(scsu->गवाक्षः_Use[i]!=गवाक्षः);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    यदि(j==8) {
        j=0;
    }
    यावत्(j!=scsu->अग्र_गवाक्षः_Use_सूचकांक) {
        scsu->गवाक्षः_Use[i]=scsu->गवाक्षः_Use[j];
        i=j;
        यदि(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->गवाक्षः_Use[i]=गवाक्षः;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
स्थिर पूर्णाङ्क
get_Dynamic_विस्थापनम्(uint32_t c, uint32_t *p_विस्थापनम्) {
    पूर्णाङ्क i;

    प्रति(i=0; i<7; ++i) {
        यदि (c - fixedOffsets[i] <= 0x7f) {
            *p_विस्थापनम्=fixedOffsets[i];
            प्रत्यागम 0xf9+i;
        }
    }

    यदि(c<0x80) {
        /* No dynamic window for US-ASCII. */
        प्रत्यागम -1;
    } अन्यथा यदि(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_विस्थापनम्=c&0x7fffff80;
        प्रत्यागम static_cast<पूर्णाङ्क>(c >> 7);
    } अन्यथा यदि(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_विस्थापनम्=c&0x7fffff80;
        प्रत्यागम static_cast<पूर्णाङ्क>((c - gap_विस्थापनम्) >> 7);
    } अन्यथा {
        प्रत्यागम -1;
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

स्थिर शून्य U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_तर्काः *p_तर्काः,
                            UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    SCSUData *scsu;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;
    int32_t *offsets;

    UBool is_Single_Byte_बहुलकम्;
    uint8_t dynamic_गवाक्षः;
    uint32_t current_विस्थापनम्;

    uint32_t c, delta;

    int32_t source_सूचकांक, अग्र_Source_सूचकांक;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t विस्थापनम्;
    char16_t lead, trail;
    पूर्णाङ्क संहिता;
    int8_t गवाक्षः;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=(uint8_t *)p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);
    offsets=p_तर्काः->offsets;

    /* get the state machine state */
    is_Single_Byte_बहुलकम्=scsu->from_UIs_Single_Byte_बहुलकम्;
    dynamic_गवाक्षः=scsu->from_UDynamic_गवाक्षः;
    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    source_सूचकांक= c==0 ? 0 : -1;
    अग्र_Source_सूचकांक=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    यदि(is_Single_Byte_बहुलकम्) {
        यदि(c!=0 && target_क्षमता>0) {
            गच्छ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target_क्षमता<=0) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            c=*source++;
            ++अग्र_Source_सूचकांक;

            यदि((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *target++=(uint8_t)c;
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                --target_क्षमता;
            } अन्यथा यदि(c<0x20) {
                यदि((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *target++=(uint8_t)c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    --target_क्षमता;
                } अन्यथा {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि((delta=c-current_विस्थापनम्)<=0x7f) {
                /* use the current dynamic window */
                *target++=(uint8_t)(delta|0x80);
                यदि(offsets!=शून्यसूचकः) {
                    *offsets++=source_सूचकांक;
                }
                --target_क्षमता;
            } अन्यथा यदि(U16_IS_SURROGATE(c)) {
                यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    यदि(source<source_सीमा) {
                        /* test the following code unit */
                        trail=*source;
                        यदि(U16_IS_TRAIL(trail)) {
                            ++source;
                            ++अग्र_Source_सूचकांक;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                    } अन्यथा {
                        /* no more input */
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                यदि((delta=c-current_विस्थापनम्)<=0x7f) {
                    /* use the current dynamic window */
                    *target++=(uint8_t)(delta|0x80);
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    --target_क्षमता;
                } अन्यथा यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_गवाक्षः=गवाक्षः;
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(SC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    संहिता-=0x200;
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_गवाक्षः<<21)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=4;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_बहुलकम्=असत्य;
                    *target++=(uint8_t)SCU;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                    }
                    --target_क्षमता;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                गच्छ output_अष्टकानि;
            } अन्यथा यदि(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                गच्छ output_अष्टकानि;
            } अन्यथा {
                /* compress all other BMP characters */
                यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    यदि(source>=source_सीमा || is_अन्तर्_विस्थापनम्_गवाक्षः_Or_प्रत्यक्षम्(scsu->fromUDynamicOffsets[गवाक्षः], *source)) {
                        /* change to dynamic window */
                        dynamic_गवाक्षः=गवाक्षः;
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(SC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+गवाक्षः)<<8)|(c-scsu->fromUDynamicOffsets[गवाक्षः])|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    }
                } अन्यथा यदि((गवाक्षः=get_गवाक्षः(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+गवाक्षः)<<8)|(c-staticOffsets[गवाक्षः]);
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(SD0+dynamic_गवाक्षः)<<16)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=3;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (source >= source_सीमा || (uint32_t)(*source - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_बहुलकम्=असत्य;
                    c|=SCU<<16;
                    length=3;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    गच्छ output_अष्टकानि;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            source_सूचकांक=अग्र_Source_सूचकांक;
        }
    } अन्यथा {
        यदि(c!=0 && target_क्षमता>0) {
            गच्छ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target_क्षमता<=0) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            c=*source++;
            ++अग्र_Source_सूचकांक;

            यदि ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                यदि(target_क्षमता>=2) {
                    *target++=(uint8_t)(c>>8);
                    *target++=(uint8_t)c;
                    यदि(offsets!=शून्यसूचकः) {
                        *offsets++=source_सूचकांक;
                        *offsets++=source_सूचकांक;
                    }
                    target_क्षमता-=2;
                } अन्यथा {
                    length=2;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                यदि(!(source<source_सीमा && (uint32_t)(*source-0x3400)<(0xd800-0x3400))) {
                    यदि (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_बहुलकम्=सत्य;
                        c|=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|c;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_बहुलकम्=सत्य;
                        dynamic_गवाक्षः=गवाक्षः;
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_बहुलकम्=सत्य;
                        dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(UD0+dynamic_गवाक्षः)<<16)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                        length=3;
                        गच्छ output_अष्टकानि;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                गच्छ output_अष्टकानि;
            } अन्यथा यदि(c<0xe000) {
                /* c is a surrogate */
                यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    यदि(source<source_सीमा) {
                        /* test the following code unit */
                        trail=*source;
                        यदि(U16_IS_TRAIL(trail)) {
                            ++source;
                            ++अग्र_Source_सूचकांक;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                    } अन्यथा {
                        /* no more input */
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                /* compress supplementary character */
                यदि( (गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(source<source_सीमा && (uint32_t)(*source-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_बहुलकम्=सत्य;
                    dynamic_गवाक्षः=गवाक्षः;
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि(source<source_सीमा && lead==*source && /* too lazy to check trail in same window as source[1] */
                          (संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_बहुलकम्=सत्य;
                    संहिता-=0x200;
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_गवाक्षः<<21)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=4;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                गच्छ output_अष्टकानि;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            source_सूचकांक=अग्र_Source_सूचकांक;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_बहुलकम्=is_Single_Byte_बहुलकम्;
    scsu->from_UDynamic_गवाक्षः=dynamic_गवाक्षः;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    p_तर्काः->offsets=offsets;
    प्रत्यागम;

output_अष्टकानि:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    यदि(length<=target_क्षमता) {
        यदि(offsets==शून्यसूचकः) {
            विकल्प(length) {
                /* each branch falls through to the next one */
            पक्ष 4:
                *target++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            पक्ष 3:
                *target++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            पक्ष 2:
                *target++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            पक्ष 1:
                *target++=(uint8_t)c;
                U_FALLTHROUGH;
            स्वभाव:
                /* will never occur */
                भङ्ग;
            }
        } अन्यथा {
            विकल्प(length) {
                /* each branch falls through to the next one */
            पक्ष 4:
                *target++=(uint8_t)(c>>24);
                *offsets++=source_सूचकांक;
                U_FALLTHROUGH;
            पक्ष 3:
                *target++=(uint8_t)(c>>16);
                *offsets++=source_सूचकांक;
                U_FALLTHROUGH;
            पक्ष 2:
                *target++=(uint8_t)(c>>8);
                *offsets++=source_सूचकांक;
                U_FALLTHROUGH;
            पक्ष 1:
                *target++=(uint8_t)c;
                *offsets++=source_सूचकांक;
                U_FALLTHROUGH;
            स्वभाव:
                /* will never occur */
                भङ्ग;
            }
        }
        target_क्षमता-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        source_सूचकांक=अग्र_Source_सूचकांक;
        गच्छ loop;
    } अन्यथा {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=target_क्षमता;
        p=(uint8_t *)cnv->char_Error_बफर;
        विकल्प(length) {
            /* each branch falls through to the next one */
        पक्ष 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        पक्ष 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        पक्ष 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        पक्ष 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        स्वभाव:
            /* will never occur */
            भङ्ग;
        }
        cnv->char_Error_बफर_दीर्घता=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        विकल्प(target_क्षमता) {
            /* each branch falls through to the next one */
        पक्ष 3:
            *target++=(uint8_t)(c>>16);
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            U_FALLTHROUGH;
        पक्ष 2:
            *target++=(uint8_t)(c>>8);
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            U_FALLTHROUGH;
        पक्ष 1:
            *target++=(uint8_t)c;
            यदि(offsets!=शून्यसूचकः) {
                *offsets++=source_सूचकांक;
            }
            U_FALLTHROUGH;
        स्वभाव:
            भङ्ग;
        }

        /* target overflow */
        target_क्षमता=0;
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        c=0;
        गच्छ endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
स्थिर शून्य U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_तर्काः *p_तर्काः,
                 UError_संहिता *p_Error_संहिता) {
    UConverter *cnv;
    SCSUData *scsu;
    ध्रुव char16_t *source, *source_सीमा;
    uint8_t *target;
    int32_t target_क्षमता;

    UBool is_Single_Byte_बहुलकम्;
    uint8_t dynamic_गवाक्षः;
    uint32_t current_विस्थापनम्;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t विस्थापनम्;
    char16_t lead, trail;
    पूर्णाङ्क संहिता;
    int8_t गवाक्षः;

    /* set up the local pointers */
    cnv=p_तर्काः->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    source=p_तर्काः->source;
    source_सीमा=p_तर्काः->source_सीमा;
    target=(uint8_t *)p_तर्काः->target;
    target_क्षमता=(int32_t)(p_तर्काः->target_सीमा-p_तर्काः->target);

    /* get the state machine state */
    is_Single_Byte_बहुलकम्=scsu->from_UIs_Single_Byte_बहुलकम्;
    dynamic_गवाक्षः=scsu->from_UDynamic_गवाक्षः;
    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    यदि(is_Single_Byte_बहुलकम्) {
        यदि(c!=0 && target_क्षमता>0) {
            गच्छ getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target_क्षमता<=0) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            c=*source++;

            यदि((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *target++=(uint8_t)c;
                --target_क्षमता;
            } अन्यथा यदि(c<0x20) {
                यदि((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *target++=(uint8_t)c;
                    --target_क्षमता;
                } अन्यथा {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि((delta=c-current_विस्थापनम्)<=0x7f) {
                /* use the current dynamic window */
                *target++=(uint8_t)(delta|0x80);
                --target_क्षमता;
            } अन्यथा यदि(U16_IS_SURROGATE(c)) {
                यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    यदि(source<source_सीमा) {
                        /* test the following code unit */
                        trail=*source;
                        यदि(U16_IS_TRAIL(trail)) {
                            ++source;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                    } अन्यथा {
                        /* no more input */
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                यदि((delta=c-current_विस्थापनम्)<=0x7f) {
                    /* use the current dynamic window */
                    *target++=(uint8_t)(delta|0x80);
                    --target_क्षमता;
                } अन्यथा यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_गवाक्षः=गवाक्षः;
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(SC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    संहिता-=0x200;
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_गवाक्षः<<21)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=4;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_बहुलकम्=असत्य;
                    *target++=(uint8_t)SCU;
                    --target_क्षमता;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                गच्छ output_अष्टकानि;
            } अन्यथा यदि(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                गच्छ output_अष्टकानि;
            } अन्यथा {
                /* compress all other BMP characters */
                यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    यदि(source>=source_सीमा || is_अन्तर्_विस्थापनम्_गवाक्षः_Or_प्रत्यक्षम्(scsu->fromUDynamicOffsets[गवाक्षः], *source)) {
                        /* change to dynamic window */
                        dynamic_गवाक्षः=गवाक्षः;
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(SC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+गवाक्षः)<<8)|(c-scsu->fromUDynamicOffsets[गवाक्षः])|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    }
                } अन्यथा यदि((गवाक्षः=get_गवाक्षः(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+गवाक्षः)<<8)|(c-staticOffsets[गवाक्षः]);
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(SD0+dynamic_गवाक्षः)<<16)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=3;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि (c - 0x3400 < 0xd800 - 0x3400 &&
                           (source >= source_सीमा || static_cast<uint32_t>(*source - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_बहुलकम्=असत्य;
                    c|=SCU<<16;
                    length=3;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    गच्छ output_अष्टकानि;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } अन्यथा {
        यदि(c!=0 && target_क्षमता>0) {
            गच्छ getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        यावत्(source<source_सीमा) {
            यदि(target_क्षमता<=0) {
                /* target is full */
                *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
                भङ्ग;
            }
            c=*source++;

            यदि (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                यदि(target_क्षमता>=2) {
                    *target++=(uint8_t)(c>>8);
                    *target++=(uint8_t)c;
                    target_क्षमता-=2;
                } अन्यथा {
                    length=2;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा यदि (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                यदि(!(source<source_सीमा && (uint32_t)(*source-0x3400)<(0xd800-0x3400))) {
                    यदि (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_बहुलकम्=सत्य;
                        c|=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|c;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा यदि((गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_बहुलकम्=सत्य;
                        dynamic_गवाक्षः=गवाक्षः;
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                        length=2;
                        गच्छ output_अष्टकानि;
                    } अन्यथा यदि((संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_बहुलकम्=सत्य;
                        dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                        current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                        use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                        c=((uint32_t)(UD0+dynamic_गवाक्षः)<<16)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                        length=3;
                        गच्छ output_अष्टकानि;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                गच्छ output_अष्टकानि;
            } अन्यथा यदि(c<0xe000) {
                /* c is a surrogate */
                यदि(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    यदि(source<source_सीमा) {
                        /* test the following code unit */
                        trail=*source;
                        यदि(U16_IS_TRAIL(trail)) {
                            ++source;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } अन्यथा {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                            गच्छ endloop;
                        }
                    } अन्यथा {
                        /* no more input */
                        भङ्ग;
                    }
                } अन्यथा {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_संहिता=U_ILLEGAL_CHAR_प्राप्तम्;
                    गच्छ endloop;
                }

                /* compress supplementary character */
                यदि( (गवाक्षः=get_गवाक्षः(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(source<source_सीमा && (uint32_t)(*source-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_बहुलकम्=सत्य;
                    dynamic_गवाक्षः=गवाक्षः;
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः];
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)(UC0+dynamic_गवाक्षः)<<8)|(c-current_विस्थापनम्)|0x80;
                    length=2;
                    गच्छ output_अष्टकानि;
                } अन्यथा यदि(source<source_सीमा && lead==*source && /* too lazy to check trail in same window as source[1] */
                          (संहिता=get_Dynamic_विस्थापनम्(c, &विस्थापनम्))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_बहुलकम्=सत्य;
                    संहिता-=0x200;
                    dynamic_गवाक्षः=get_अग्र_Dynamic_गवाक्षः(scsu);
                    current_विस्थापनम्=scsu->fromUDynamicOffsets[dynamic_गवाक्षः]=विस्थापनम्;
                    use_Dynamic_गवाक्षः(scsu, dynamic_गवाक्षः);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_गवाक्षः<<21)|((uint32_t)संहिता<<8)|(c-current_विस्थापनम्)|0x80;
                    length=4;
                    गच्छ output_अष्टकानि;
                } अन्यथा {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    गच्छ output_अष्टकानि;
                }
            } अन्यथा /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                गच्छ output_अष्टकानि;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_बहुलकम्=is_Single_Byte_बहुलकम्;
    scsu->from_UDynamic_गवाक्षः=dynamic_गवाक्षः;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_तर्काः->source=source;
    p_तर्काः->target=(वर्ण *)target;
    प्रत्यागम;

output_अष्टकानि:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    यदि(length<=target_क्षमता) {
        विकल्प(length) {
            /* each branch falls through to the next one */
        पक्ष 4:
            *target++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        पक्ष 3:
            *target++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        पक्ष 2:
            *target++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        पक्ष 1:
            *target++=(uint8_t)c;
            U_FALLTHROUGH;
        स्वभाव:
            /* will never occur */
            भङ्ग;
        }
        target_क्षमता-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        गच्छ loop;
    } अन्यथा {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=target_क्षमता;
        p=(uint8_t *)cnv->char_Error_बफर;
        विकल्प(length) {
            /* each branch falls through to the next one */
        पक्ष 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        पक्ष 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        पक्ष 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        पक्ष 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        स्वभाव:
            /* will never occur */
            भङ्ग;
        }
        cnv->char_Error_बफर_दीर्घता=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        विकल्प(target_क्षमता) {
            /* each branch falls through to the next one */
        पक्ष 3:
            *target++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        पक्ष 2:
            *target++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        पक्ष 1:
            *target++=(uint8_t)c;
            U_FALLTHROUGH;
        स्वभाव:
            भङ्ग;
        }

        /* target overflow */
        target_क्षमता=0;
        *p_Error_संहिता=U_बफर_अतिप्रवाहः_ERROR;
        c=0;
        गच्छ endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

स्थिर ध्रुव वर्ण *  U_CALLCONV
_SCSUGetName(ध्रुव UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    विकल्प(scsu->locale) {
    पक्ष l_ja:
        प्रत्यागम "SCSU,locale=ja";
    स्वभाव:
        प्रत्यागम "SCSU";
    }
}

/* structure for SafeClone calculations */
संरचना cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

स्थिर UConverter *  U_CALLCONV
_SCSUSafeClone(ध्रुव UConverter *cnv, 
               शून्य *राशिः_बफर, 
               int32_t *p_बफर_आकार, 
               UError_संहिता *स्थितिः)
{
    संरचना cloneSCSUStruct * localClone;
    int32_t बफर_आकार_Needed = परिमाण(संरचना cloneSCSUStruct);

    यदि (U_FAILURE(*स्थितिः)){
        प्रत्यागम शून्यसूचकः;
    }

    यदि (*p_बफर_आकार == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_बफर_आकार = बफर_आकार_Needed;
        प्रत्यागम शून्यसूचकः;
    }

    localClone = (संरचना cloneSCSUStruct *)राशिः_बफर;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, परिमाण(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = सत्य;

    प्रत्यागम &localClone->cnv;
}
U_CDECL_अन्तः

स्थिर ध्रुव UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    शून्यसूचकः,
    शून्यसूचकः,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    शून्यसूचकः,

    शून्यसूचकः,
    _SCSUGetName,
    शून्यसूचकः,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_समुच्चयः,
    शून्यसूचकः,
    शून्यसूचकः
};

स्थिर ध्रुव UConverter_Static_दत्त SCSUStatic_दत्त={
    परिमाण(UConverter_Static_दत्त),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    असत्य, असत्य,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

ध्रुव UConverter_Shared_दत्त _SCSUData=
        UCNV_IMMUTABLE_SHARED_दत्त_INITIALIZER(&SCSUStatic_दत्त, &_SCSUImpl);

#यदि_अन्तः
