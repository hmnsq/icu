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

#అన్తర్భావ "unicode/utypes.h"

#ఒకవేళ !UCONFIG_NO_CONVERSION && !UCONFIG_ONLY_HTML_CONVERSION

#అన్తర్భావ "unicode/ucnv.h"
#అన్తర్భావ "unicode/ucnv_cb.h"
#అన్తర్భావ "unicode/utf16.h"
#అన్తర్భావ "ucnv_bld.h"
#అన్తర్భావ "ucnv_cnv.h"
#అన్తర్భావ "cmemory.h"

/* SCSU definitions --------------------------------------------------------- */

/* SCSU command byte values */
గణన {
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

గణన {
    /*
     * Unicode code points from 3400 to E000 are not adressible by
     * dynamic window, since in these areas no short run alphabets are
     * found. Therefore add gapOffset to all values from gapThreshold.
     */
    gap_పరిమితి=0x68,
    gap_స్థానభ్రంశం=0xAC00,

    /* values between reservedStart and fixedThreshold are reserved */
    reserved_ప్రారంభం=0xA8,

    /* use table of predefined fixed offsets for values from fixedThreshold */
    fixed_పరిమితి=0xF9
};

/* constant offsets for the 8 static windows */
స్థిర స్థిరరాశి uint32_t staticOffsets[8]={
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
స్థిర స్థిరరాశి uint32_t initialDynamicOffsets[8]={
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
స్థిర స్థిరరాశి uint32_t fixedOffsets[]={
    /* 0xF9 */ 0x00C0, /* Latin-1 Letters + half of Latin Extended A */
    /* 0xFA */ 0x0250, /* IPA extensions */
    /* 0xFB */ 0x0370, /* Greek */
    /* 0xFC */ 0x0530, /* Armenian */
    /* 0xFD */ 0x3040, /* Hiragana */
    /* 0xFE */ 0x30A0, /* Katakana */
    /* 0xFF */ 0xFF60  /* Halfwidth Katakana */
};

/* state values */
గణన {
    readCommand,
    quotePairOne,
    quotePairTwo,
    quoteOne,
    definePairOne,
    definePairTwo,
    defineOne
};

రకనిర్వచనం నిర్మాణం SCSUData {
    /* dynamic window offsets, initialize to default values from initialDynamicOffsets */
    uint32_t గమ్యం_UDynamic_Offsets[8];
    uint32_t fromUDynamicOffsets[8];

    /* state machine state - toUnicode */
    UBool గమ్యం_UIs_Single_Byte_బహుళకం;
    uint8_t గమ్యం_UState;
    int8_t గమ్యం_UQuote_కిటికీ, గమ్యం_UDynamic_కిటికీ;
    uint8_t గమ్యం_UByte_One;
    uint8_t గమ్యం_UPadding[3];

    /* state machine state - fromUnicode */
    UBool from_UIs_Single_Byte_బహుళకం;
    int8_t from_UDynamic_కిటికీ;

    /*
     * windowUse[] keeps track of the use of the dynamic windows:
     * At nextWindowUseIndex there is the least recently used window,
     * and the following windows (in a wrapping manner) are more and more
     * recently used.
     * At nextWindowUseIndex-1 there is the most recently used window.
     */
    uint8_t locale;
    int8_t తదుపరి_కిటికీ_Use_సూచీ;
    int8_t కిటికీ_Use[8];
} SCSUData;

స్థిర స్థిరరాశి int8_t initial_కిటికీ_Use[8]={ 7, 0, 3, 2, 4, 5, 6, 1 };
స్థిర స్థిరరాశి int8_t initial_కిటికీ_Use_ja[8]={ 3, 2, 4, 1, 0, 7, 5, 6 };

గణన {
    lGeneric, l_ja
};

/* SCSU setup functions ----------------------------------------------------- */
U_CDECL_BEGIN
స్థిర శూన్య U_CALLCONV
_SCSUReset(UConverter *cnv, UConverterResetChoice choice) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ఒకవేళ(choice<=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset toUnicode */
        uprv_memcpy(scsu->గమ్యం_UDynamic_Offsets, initialDynamicOffsets, 32);

        scsu->గమ్యం_UIs_Single_Byte_బహుళకం=నిజం;
        scsu->గమ్యం_UState=readCommand;
        scsu->గమ్యం_UQuote_కిటికీ=scsu->గమ్యం_UDynamic_కిటికీ=0;
        scsu->గమ్యం_UByte_One=0;

        cnv->గమ్యం_ULength=0;
    }
    ఒకవేళ(choice!=UCNV_RESET_గమ్యం_UNICODE) {
        /* reset fromUnicode */
        uprv_memcpy(scsu->fromUDynamicOffsets, initialDynamicOffsets, 32);

        scsu->from_UIs_Single_Byte_బహుళకం=నిజం;
        scsu->from_UDynamic_కిటికీ=0;

        scsu->తదుపరి_కిటికీ_Use_సూచీ=0;
        ఎంపిక(scsu->locale) {
        స్థితి l_ja:
            uprv_memcpy(scsu->కిటికీ_Use, initial_కిటికీ_Use_ja, 8);
            ఆపు;
        అప్రమేయ:
            uprv_memcpy(scsu->కిటికీ_Use, initial_కిటికీ_Use, 8);
            ఆపు;
        }

        cnv->fromUChar32=0;
    }
}

స్థిర శూన్య U_CALLCONV
_SCSUOpen(UConverter *cnv,
          UConverter_ఎక్కించు_వాదనలు *p_వాదనలు,
          UError_సంకేతం *p_Error_సంకేతం) {
    స్థిరరాశి అక్షరం *locale=p_వాదనలు->locale;
    ఒకవేళ(p_వాదనలు->onlyTestIsLoadable) {
        తిరిగి;
    }
    cnv->extraInfo=uprv_malloc(పరిమాణం(SCSUData));
    ఒకవేళ(cnv->extraInfo!=శూన్యసూచిక) {
        ఒకవేళ(locale!=శూన్యసూచిక && locale[0]=='j' && locale[1]=='a' && (locale[2]==0 || locale[2]=='_')) {
            ((SCSUData *)cnv->extraInfo)->locale=l_ja;
        } లేకపోతే {
            ((SCSUData *)cnv->extraInfo)->locale=lGeneric;
        }
        _SCSUReset(cnv, UCNV_RESET_BOTH);
    } లేకపోతే {
        *p_Error_సంకేతం=U_జ్ఞాపకం_కేటాయింపు_ERROR;
    }

    /* Set the substitution character U+fffd as a Unicode string. */
    cnv->subUChars[0]=0xfffd;
    cnv->subCharLen=-1;
}

స్థిర శూన్య U_CALLCONV
_SCSUClose(UConverter *cnv) {
    ఒకవేళ(cnv->extraInfo!=శూన్యసూచిక) {
        ఒకవేళ(!cnv->isExtraLocal) {
            uprv_free(cnv->extraInfo);
        }
        cnv->extraInfo=శూన్యసూచిక;
    }
}

/* SCSU-to-Unicode conversion functions ------------------------------------- */

స్థిర శూన్య U_CALLCONV
_SCSUToUnicodeWithOffsets(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
                          UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    SCSUData *scsu;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    int32_t *offsets;
    UBool is_Single_Byte_బహుళకం;
    uint8_t state, byteOne;
    int8_t quote_కిటికీ, dynamic_కిటికీ;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;
    offsets=p_వాదనలు->offsets;

    /* get the state machine state */
    is_Single_Byte_బహుళకం=scsu->గమ్యం_UIs_Single_Byte_బహుళకం;
    state=scsu->గమ్యం_UState;
    quote_కిటికీ=scsu->గమ్యం_UQuote_కిటికీ;
    dynamic_కిటికీ=scsu->గమ్యం_UDynamic_కిటికీ;
    byteOne=scsu->గమ్యం_UByte_One;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ=state==readCommand ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

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
    ఒకవేళ(is_Single_Byte_బహుళకం) {
        /* fast path for single-byte mode */
        ఒకవేళ(state==readCommand) {
fastSingle:
            అయితే(ఆధారం<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు && (b=*ఆధారం)>=0x20) {
                ++ఆధారం;
                ++తదుపరి_ఆధారం_సూచీ;
                ఒకవేళ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *లక్ష్యం++=(char16_t)b;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                } లేకపోతే {
                    /* write from dynamic window */
                    uint32_t c=scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]+(b&0x7f);
                    ఒకవేళ(c<=0xffff) {
                        *లక్ష్యం++=(char16_t)c;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                        }
                    } లేకపోతే {
                        /* output surrogate pair */
                        *లక్ష్యం++=(char16_t)(0xd7c0+(c>>10));
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=(char16_t)(0xdc00|(c&0x3ff));
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                *offsets++=ఆధారం_సూచీ;
                            }
                        } లేకపోతే {
                            /* target overflow */
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                            }
                            cnv->UChar_Error_బఫరు[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            వెళ్ళు endloop;
                        }
                    }
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_బహుళకం:
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            b=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;
            ఎంపిక(state) {
            స్థితి readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ఒకవేళ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *లక్ష్యం++=(char16_t)b;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                    వెళ్ళు fastSingle;
                } లేకపోతే ఒకవేళ(SC0<=b) {
                    ఒకవేళ(b<=SC7) {
                        dynamic_కిటికీ=(int8_t)(b-SC0);
                        ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                        వెళ్ళు fastSingle;
                    } లేకపోతే /* if(SD0<=b && b<=SD7) */ {
                        dynamic_కిటికీ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } లేకపోతే ఒకవేళ(/* SQ0<=b && */ b<=SQ7) {
                    quote_కిటికీ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } లేకపోతే ఒకవేళ(b==SDX) {
                    state=definePairOne;
                } లేకపోతే ఒకవేళ(b==SQU) {
                    state=quotePairOne;
                } లేకపోతే ఒకవేళ(b==SCU) {
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    వెళ్ళు fastUnicode;
                } లేకపోతే /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    వెళ్ళు endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->గమ్యం_UBytes[0]=b;
                cnv->గమ్యం_ULength=1;
                ఆపు;
            స్థితి quotePairOne:
                byteOne=b;
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=quotePairTwo;
                ఆపు;
            స్థితి quotePairTwo:
                *లక్ష్యం++=(char16_t)((byteOne<<8)|b);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి quoteOne:
                ఒకవేళ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *లక్ష్యం++=(char16_t)(staticOffsets[quote_కిటికీ]+b);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                } లేకపోతే {
                    /* write from dynamic window */
                    uint32_t c=scsu->గమ్యం_UDynamic_Offsets[quote_కిటికీ]+(b&0x7f);
                    ఒకవేళ(c<=0xffff) {
                        *లక్ష్యం++=(char16_t)c;
                        ఒకవేళ(offsets!=శూన్యసూచిక) {
                            *offsets++=ఆధారం_సూచీ;
                        }
                    } లేకపోతే {
                        /* output surrogate pair */
                        *లక్ష్యం++=(char16_t)(0xd7c0+(c>>10));
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=(char16_t)(0xdc00|(c&0x3ff));
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                                *offsets++=ఆధారం_సూచీ;
                            }
                        } లేకపోతే {
                            /* target overflow */
                            ఒకవేళ(offsets!=శూన్యసూచిక) {
                                *offsets++=ఆధారం_సూచీ;
                            }
                            cnv->UChar_Error_బఫరు[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            వెళ్ళు endloop;
                        }
                    }
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి definePairOne:
                dynamic_కిటికీ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=definePairTwo;
                ఆపు;
            స్థితి definePairTwo:
                scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=0x10000+(byteOne<<15UL | b<<7UL);
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి defineOne:
                ఒకవేళ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->గమ్యం_UBytes[1]=b;
                    cnv->గమ్యం_ULength=2;
                    వెళ్ళు endloop;
                } లేకపోతే ఒకవేళ(b<gap_పరిమితి) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=b<<7UL;
                } లేకపోతే ఒకవేళ((uint8_t)(b-gap_పరిమితి)<(reserved_ప్రారంభం-gap_పరిమితి)) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=(b<<7UL)+gap_స్థానభ్రంశం;
                } లేకపోతే ఒకవేళ(b>=fixed_పరిమితి) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=fixedOffsets[b-fixed_పరిమితి];
                } లేకపోతే {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->గమ్యం_UBytes[1]=b;
                    cnv->గమ్యం_ULength=2;
                    వెళ్ళు endloop;
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                state=readCommand;
                వెళ్ళు fastSingle;
            }
        }
    } లేకపోతే {
        /* fast path for Unicode mode */
        ఒకవేళ(state==readCommand) {
fastUnicode:
            అయితే(ఆధారం+1<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు && (uint8_t)((b=*ఆధారం)-UC0)>(Urs-UC0)) {
                *లక్ష్యం++=(char16_t)((b<<8)|ఆధారం[1]);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                తదుపరి_ఆధారం_సూచీ+=2;
                ఆధారం+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            b=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;
            ఎంపిక(state) {
            స్థితి readCommand:
                ఒకవేళ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=quotePairTwo;
                } లేకపోతే ఒకవేళ(/* UC0<=b && */ b<=UC7) {
                    dynamic_కిటికీ=(int8_t)(b-UC0);
                    ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                    is_Single_Byte_బహుళకం=నిజం;
                    వెళ్ళు fastSingle;
                } లేకపోతే ఒకవేళ(/* UD0<=b && */ b<=UD7) {
                    dynamic_కిటికీ=(int8_t)(b-UD0);
                    is_Single_Byte_బహుళకం=నిజం;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=defineOne;
                    వెళ్ళు single_Byte_బహుళకం;
                } లేకపోతే ఒకవేళ(b==UDX) {
                    is_Single_Byte_బహుళకం=నిజం;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=definePairOne;
                    వెళ్ళు single_Byte_బహుళకం;
                } లేకపోతే ఒకవేళ(b==UQU) {
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=quotePairOne;
                } లేకపోతే /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    వెళ్ళు endloop;
                }
                ఆపు;
            స్థితి quotePairOne:
                byteOne=b;
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=quotePairTwo;
                ఆపు;
            స్థితి quotePairTwo:
                *లక్ష్యం++=(char16_t)((byteOne<<8)|b);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
                state=readCommand;
                వెళ్ళు fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) && *p_Error_సంకేతం!=U_బఫరు_అధికప్రవాహం_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } లేకపోతే ఒకవేళ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->గమ్యం_ULength=0;
    }
    scsu->గమ్యం_UIs_Single_Byte_బహుళకం=is_Single_Byte_బహుళకం;
    scsu->గమ్యం_UState=state;
    scsu->గమ్యం_UQuote_కిటికీ=quote_కిటికీ;
    scsu->గమ్యం_UDynamic_కిటికీ=dynamic_కిటికీ;
    scsu->గమ్యం_UByte_One=byteOne;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
    p_వాదనలు->offsets=offsets;
}

/*
 * Identical to _SCSUToUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
స్థిర శూన్య U_CALLCONV
_SCSUToUnicode(UConverter_గమ్యం_Unicode_వాదనలు *p_వాదనలు,
               UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    SCSUData *scsu;
    స్థిరరాశి uint8_t *ఆధారం, *ఆధారం_హద్దు;
    char16_t *లక్ష్యం;
    స్థిరరాశి char16_t *లక్ష్యం_హద్దు;
    UBool is_Single_Byte_బహుళకం;
    uint8_t state, byteOne;
    int8_t quote_కిటికీ, dynamic_కిటికీ;

    uint8_t b;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    ఆధారం=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=(స్థిరరాశి uint8_t *)p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=p_వాదనలు->లక్ష్యం;
    లక్ష్యం_హద్దు=p_వాదనలు->లక్ష్యం_హద్దు;

    /* get the state machine state */
    is_Single_Byte_బహుళకం=scsu->గమ్యం_UIs_Single_Byte_బహుళకం;
    state=scsu->గమ్యం_UState;
    quote_కిటికీ=scsu->గమ్యం_UQuote_కిటికీ;
    dynamic_కిటికీ=scsu->గమ్యం_UDynamic_కిటికీ;
    byteOne=scsu->గమ్యం_UByte_One;

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
    ఒకవేళ(is_Single_Byte_బహుళకం) {
        /* fast path for single-byte mode */
        ఒకవేళ(state==readCommand) {
fastSingle:
            అయితే(ఆధారం<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు && (b=*ఆధారం)>=0x20) {
                ++ఆధారం;
                ఒకవేళ(b<=0x7f) {
                    /* write US-ASCII graphic character or DEL */
                    *లక్ష్యం++=(char16_t)b;
                } లేకపోతే {
                    /* write from dynamic window */
                    uint32_t c=scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]+(b&0x7f);
                    ఒకవేళ(c<=0xffff) {
                        *లక్ష్యం++=(char16_t)c;
                    } లేకపోతే {
                        /* output surrogate pair */
                        *లక్ష్యం++=(char16_t)(0xd7c0+(c>>10));
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=(char16_t)(0xdc00|(c&0x3ff));
                        } లేకపోతే {
                            /* target overflow */
                            cnv->UChar_Error_బఫరు[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            వెళ్ళు endloop;
                        }
                    }
                }
            }
        }

        /* normal state machine for single-byte mode, minus handling for what fastSingle covers */
single_Byte_బహుళకం:
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            b=*ఆధారం++;
            ఎంపిక(state) {
            స్థితి readCommand:
                /* redundant conditions are commented out */
                /* here: b<0x20 because otherwise we would be in fastSingle */
                ఒకవేళ((1UL<<b)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *లక్ష్యం++=(char16_t)b;
                    వెళ్ళు fastSingle;
                } లేకపోతే ఒకవేళ(SC0<=b) {
                    ఒకవేళ(b<=SC7) {
                        dynamic_కిటికీ=(int8_t)(b-SC0);
                        వెళ్ళు fastSingle;
                    } లేకపోతే /* if(SD0<=b && b<=SD7) */ {
                        dynamic_కిటికీ=(int8_t)(b-SD0);
                        state=defineOne;
                    }
                } లేకపోతే ఒకవేళ(/* SQ0<=b && */ b<=SQ7) {
                    quote_కిటికీ=(int8_t)(b-SQ0);
                    state=quoteOne;
                } లేకపోతే ఒకవేళ(b==SDX) {
                    state=definePairOne;
                } లేకపోతే ఒకవేళ(b==SQU) {
                    state=quotePairOne;
                } లేకపోతే ఒకవేళ(b==SCU) {
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    వెళ్ళు fastUnicode;
                } లేకపోతే /* Srs */ {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    వెళ్ళు endloop;
                }

                /* store the first byte of a multibyte sequence in toUBytes[] */
                cnv->గమ్యం_UBytes[0]=b;
                cnv->గమ్యం_ULength=1;
                ఆపు;
            స్థితి quotePairOne:
                byteOne=b;
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=quotePairTwo;
                ఆపు;
            స్థితి quotePairTwo:
                *లక్ష్యం++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి quoteOne:
                ఒకవేళ(b<0x80) {
                    /* all static offsets are in the BMP */
                    *లక్ష్యం++=(char16_t)(staticOffsets[quote_కిటికీ]+b);
                } లేకపోతే {
                    /* write from dynamic window */
                    uint32_t c=scsu->గమ్యం_UDynamic_Offsets[quote_కిటికీ]+(b&0x7f);
                    ఒకవేళ(c<=0xffff) {
                        *లక్ష్యం++=(char16_t)c;
                    } లేకపోతే {
                        /* output surrogate pair */
                        *లక్ష్యం++=(char16_t)(0xd7c0+(c>>10));
                        ఒకవేళ(లక్ష్యం<లక్ష్యం_హద్దు) {
                            *లక్ష్యం++=(char16_t)(0xdc00|(c&0x3ff));
                        } లేకపోతే {
                            /* target overflow */
                            cnv->UChar_Error_బఫరు[0]=(char16_t)(0xdc00|(c&0x3ff));
                            cnv->UChar_Error_బఫరు_నిడివి=1;
                            *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                            వెళ్ళు endloop;
                        }
                    }
                }
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి definePairOne:
                dynamic_కిటికీ=(int8_t)((b>>5)&7);
                byteOne=(uint8_t)(b&0x1f);
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=definePairTwo;
                ఆపు;
            స్థితి definePairTwo:
                scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=0x10000+(byteOne<<15UL | b<<7UL);
                state=readCommand;
                వెళ్ళు fastSingle;
            స్థితి defineOne:
                ఒకవేళ(b==0) {
                    /* callback(illegal): Reserved window offset value 0 */
                    cnv->గమ్యం_UBytes[1]=b;
                    cnv->గమ్యం_ULength=2;
                    వెళ్ళు endloop;
                } లేకపోతే ఒకవేళ(b<gap_పరిమితి) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=b<<7UL;
                } లేకపోతే ఒకవేళ((uint8_t)(b-gap_పరిమితి)<(reserved_ప్రారంభం-gap_పరిమితి)) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=(b<<7UL)+gap_స్థానభ్రంశం;
                } లేకపోతే ఒకవేళ(b>=fixed_పరిమితి) {
                    scsu->గమ్యం_UDynamic_Offsets[dynamic_కిటికీ]=fixedOffsets[b-fixed_పరిమితి];
                } లేకపోతే {
                    /* callback(illegal): Reserved window offset value 0xa8..0xf8 */
                    cnv->గమ్యం_UBytes[1]=b;
                    cnv->గమ్యం_ULength=2;
                    వెళ్ళు endloop;
                }
                state=readCommand;
                వెళ్ళు fastSingle;
            }
        }
    } లేకపోతే {
        /* fast path for Unicode mode */
        ఒకవేళ(state==readCommand) {
fastUnicode:
            అయితే(ఆధారం+1<ఆధారం_హద్దు && లక్ష్యం<లక్ష్యం_హద్దు && (uint8_t)((b=*ఆధారం)-UC0)>(Urs-UC0)) {
                *లక్ష్యం++=(char16_t)((b<<8)|ఆధారం[1]);
                ఆధారం+=2;
            }
        }

        /* normal state machine for Unicode mode */
/* unicodeByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం>=లక్ష్యం_హద్దు) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            b=*ఆధారం++;
            ఎంపిక(state) {
            స్థితి readCommand:
                ఒకవేళ((uint8_t)(b-UC0)>(Urs-UC0)) {
                    byteOne=b;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=quotePairTwo;
                } లేకపోతే ఒకవేళ(/* UC0<=b && */ b<=UC7) {
                    dynamic_కిటికీ=(int8_t)(b-UC0);
                    is_Single_Byte_బహుళకం=నిజం;
                    వెళ్ళు fastSingle;
                } లేకపోతే ఒకవేళ(/* UD0<=b && */ b<=UD7) {
                    dynamic_కిటికీ=(int8_t)(b-UD0);
                    is_Single_Byte_బహుళకం=నిజం;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=defineOne;
                    వెళ్ళు single_Byte_బహుళకం;
                } లేకపోతే ఒకవేళ(b==UDX) {
                    is_Single_Byte_బహుళకం=నిజం;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=definePairOne;
                    వెళ్ళు single_Byte_బహుళకం;
                } లేకపోతే ఒకవేళ(b==UQU) {
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    state=quotePairOne;
                } లేకపోతే /* Urs */ {
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    cnv->గమ్యం_UBytes[0]=b;
                    cnv->గమ్యం_ULength=1;
                    వెళ్ళు endloop;
                }
                ఆపు;
            స్థితి quotePairOne:
                byteOne=b;
                cnv->గమ్యం_UBytes[1]=b;
                cnv->గమ్యం_ULength=2;
                state=quotePairTwo;
                ఆపు;
            స్థితి quotePairTwo:
                *లక్ష్యం++=(char16_t)((byteOne<<8)|b);
                state=readCommand;
                వెళ్ళు fastUnicode;
            }
        }
    }
endloop:

    /* set the converter state back into UConverter */
    ఒకవేళ(U_FAILURE(*p_Error_సంకేతం) && *p_Error_సంకేతం!=U_బఫరు_అధికప్రవాహం_ERROR) {
        /* reset to deal with the next character */
        state=readCommand;
    } లేకపోతే ఒకవేళ(state==readCommand) {
        /* not in a multi-byte sequence, reset toULength */
        cnv->గమ్యం_ULength=0;
    }
    scsu->గమ్యం_UIs_Single_Byte_బహుళకం=is_Single_Byte_బహుళకం;
    scsu->గమ్యం_UState=state;
    scsu->గమ్యం_UQuote_కిటికీ=quote_కిటికీ;
    scsu->గమ్యం_UDynamic_కిటికీ=dynamic_కిటికీ;
    scsu->గమ్యం_UByte_One=byteOne;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=(స్థిరరాశి అక్షరం *)ఆధారం;
    p_వాదనలు->లక్ష్యం=లక్ష్యం;
}
U_CDECL_చివర
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
స్థిర int8_t
get_కిటికీ(స్థిరరాశి uint32_t offsets[8], uint32_t c) {
    పూర్ణాంకం i;
    కొరకు(i=0; i<8; ++i) {
        ఒకవేళ (c - offsets[i] <= 0x7f) {
            తిరిగి static_cast<int8_t>(i);
        }
    }
    తిరిగి -1;
}

/* is the character in the dynamic window starting at the offset, or in the direct-encoded range? */
స్థిర UBool
is_లోపల_స్థానభ్రంశం_కిటికీ_Or_ప్రత్యక్ష(uint32_t స్థానభ్రంశం, uint32_t c) {
    తిరిగి c<=స్థానభ్రంశం+0x7f &&
          (c>=స్థానభ్రంశం || (c<=0x7f &&
                        (c>=0x20 || (1UL<<c)&0x2601)));
                                /* binary 0010 0110 0000 0001,
                                   check for b==0xd || b==0xa || b==9 || b==0 */
}

/*
 * getNextDynamicWindow returns the next dynamic window to be redefined
 */
స్థిర int8_t
get_తదుపరి_Dynamic_కిటికీ(SCSUData *scsu) {
    int8_t కిటికీ=scsu->కిటికీ_Use[scsu->తదుపరి_కిటికీ_Use_సూచీ];
    ఒకవేళ(++scsu->తదుపరి_కిటికీ_Use_సూచీ==8) {
        scsu->తదుపరి_కిటికీ_Use_సూచీ=0;
    }
    తిరిగి కిటికీ;
}

/*
 * useDynamicWindow() adjusts
 * windowUse[] and nextWindowUseIndex for the algorithm to choose
 * the next dynamic window to be defined;
 * a subclass may override it and provide its own algorithm.
 */
స్థిర శూన్య
use_Dynamic_కిటికీ(SCSUData *scsu, int8_t కిటికీ) {
    /*
     * move the existing window, which just became the most recently used one,
     * up in windowUse[] to nextWindowUseIndex-1
     */

    /* first, find the index of the window - backwards to favor the more recently used windows */
    పూర్ణాంకం i, j;

    i=scsu->తదుపరి_కిటికీ_Use_సూచీ;
    చేయి {
        ఒకవేళ(--i<0) {
            i=7;
        }
    } అయితే(scsu->కిటికీ_Use[i]!=కిటికీ);

    /* now copy each windowUse[i+1] to [i] */
    j=i+1;
    ఒకవేళ(j==8) {
        j=0;
    }
    అయితే(j!=scsu->తదుపరి_కిటికీ_Use_సూచీ) {
        scsu->కిటికీ_Use[i]=scsu->కిటికీ_Use[j];
        i=j;
        ఒకవేళ(++j==8) { j=0; }
    }

    /* finally, set the window into the most recently used index */
    scsu->కిటికీ_Use[i]=కిటికీ;
}

/*
 * calculate the offset and the code for a dynamic window that contains the character
 * takes fixed offsets into account
 * the offset of the window is stored in the offset variable,
 * the code is returned
 *
 * return offset code: -1 none  <=0xff code for SDn/UDn  else code for SDX/UDX, subtract 0x200 to get the true code
 */
స్థిర పూర్ణాంకం
get_Dynamic_స్థానభ్రంశం(uint32_t c, uint32_t *p_స్థానభ్రంశం) {
    పూర్ణాంకం i;

    కొరకు(i=0; i<7; ++i) {
        ఒకవేళ (c - fixedOffsets[i] <= 0x7f) {
            *p_స్థానభ్రంశం=fixedOffsets[i];
            తిరిగి 0xf9+i;
        }
    }

    ఒకవేళ(c<0x80) {
        /* No dynamic window for US-ASCII. */
        తిరిగి -1;
    } లేకపోతే ఒకవేళ(c<0x3400 ||
              c - 0x10000 < 0x14000 - 0x10000 ||
              c - 0x1d000 <= 0x1ffff - 0x1d000
    ) {
        /* This character is in a code range for a "small", i.e., reasonably windowable, script. */
        *p_స్థానభ్రంశం=c&0x7fffff80;
        తిరిగి static_cast<పూర్ణాంకం>(c >> 7);
    } లేకపోతే ఒకవేళ(0xe000<=c && c!=0xfeff && c<0xfff0) {
        /* For these characters we need to take the gapOffset into account. */
        *p_స్థానభ్రంశం=c&0x7fffff80;
        తిరిగి static_cast<పూర్ణాంకం>((c - gap_స్థానభ్రంశం) >> 7);
    } లేకపోతే {
        తిరిగి -1;
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

స్థిర శూన్య U_CALLCONV
_SCSUFromUnicodeWithOffsets(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                            UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    SCSUData *scsu;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;
    int32_t *offsets;

    UBool is_Single_Byte_బహుళకం;
    uint8_t dynamic_కిటికీ;
    uint32_t current_స్థానభ్రంశం;

    uint32_t c, delta;

    int32_t ఆధారం_సూచీ, తదుపరి_ఆధారం_సూచీ;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t స్థానభ్రంశం;
    char16_t lead, trail;
    పూర్ణాంకం సంకేతం_2;
    int8_t కిటికీ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);
    offsets=p_వాదనలు->offsets;

    /* get the state machine state */
    is_Single_Byte_బహుళకం=scsu->from_UIs_Single_Byte_బహుళకం;
    dynamic_కిటికీ=scsu->from_UDynamic_కిటికీ;
    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];

    c=cnv->fromUChar32;

    /* sourceIndex=-1 if the current character began in the previous buffer */
    ఆధారం_సూచీ= c==0 ? 0 : -1;
    తదుపరి_ఆధారం_సూచీ=0;

    /* similar conversion "loop" as in toUnicode */
loop:
    ఒకవేళ(is_Single_Byte_బహుళకం) {
        ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
            వెళ్ళు getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం<=0) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;

            ఒకవేళ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *లక్ష్యం++=(uint8_t)c;
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే ఒకవేళ(c<0x20) {
                ఒకవేళ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *లక్ష్యం++=(uint8_t)c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    --లక్ష్యం_సామర్థ్యం;
                } లేకపోతే {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ((delta=c-current_స్థానభ్రంశం)<=0x7f) {
                /* use the current dynamic window */
                *లక్ష్యం++=(uint8_t)(delta|0x80);
                ఒకవేళ(offsets!=శూన్యసూచిక) {
                    *offsets++=ఆధారం_సూచీ;
                }
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే ఒకవేళ(U16_IS_SURROGATE(c)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                        /* test the following code unit */
                        trail=*ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ++ఆధారం;
                            ++తదుపరి_ఆధారం_సూచీ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                    } లేకపోతే {
                        /* no more input */
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ఒకవేళ((delta=c-current_స్థానభ్రంశం)<=0x7f) {
                    /* use the current dynamic window */
                    *లక్ష్యం++=(uint8_t)(delta|0x80);
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    --లక్ష్యం_సామర్థ్యం;
                } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_కిటికీ=కిటికీ;
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(SC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    సంకేతం_2-=0x200;
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_కిటికీ<<21)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    *లక్ష్యం++=(uint8_t)SCU;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                    }
                    --లక్ష్యం_సామర్థ్యం;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే ఒకవేళ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే {
                /* compress all other BMP characters */
                ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ఒకవేళ(ఆధారం>=ఆధారం_హద్దు || is_లోపల_స్థానభ్రంశం_కిటికీ_Or_ప్రత్యక్ష(scsu->fromUDynamicOffsets[కిటికీ], *ఆధారం)) {
                        /* change to dynamic window */
                        dynamic_కిటికీ=కిటికీ;
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(SC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+కిటికీ)<<8)|(c-scsu->fromUDynamicOffsets[కిటికీ])|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    }
                } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+కిటికీ)<<8)|(c-staticOffsets[కిటికీ]);
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(SD0+dynamic_కిటికీ)<<16)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ ((c - 0x3400) < (0xd800 - 0x3400) &&
                           (ఆధారం >= ఆధారం_హద్దు || (uint32_t)(*ఆధారం - 0x3400) < (0xd800 - 0x3400))
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    c|=SCU<<16;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
        }
    } లేకపోతే {
        ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
            వెళ్ళు getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం<=0) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            c=*ఆధారం++;
            ++తదుపరి_ఆధారం_సూచీ;

            ఒకవేళ ((c - 0x3400) < (0xd800 - 0x3400)) {
                /* not compressible, write character directly */
                ఒకవేళ(లక్ష్యం_సామర్థ్యం>=2) {
                    *లక్ష్యం++=(uint8_t)(c>>8);
                    *లక్ష్యం++=(uint8_t)c;
                    ఒకవేళ(offsets!=శూన్యసూచిక) {
                        *offsets++=ఆధారం_సూచీ;
                        *offsets++=ఆధారం_సూచీ;
                    }
                    లక్ష్యం_సామర్థ్యం-=2;
                } లేకపోతే {
                    length=2;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ఒకవేళ(!(ఆధారం<ఆధారం_హద్దు && (uint32_t)(*ఆధారం-0x3400)<(0xd800-0x3400))) {
                    ఒకవేళ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_బహుళకం=నిజం;
                        c|=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|c;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_బహుళకం=నిజం;
                        dynamic_కిటికీ=కిటికీ;
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_బహుళకం=నిజం;
                        dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(UD0+dynamic_కిటికీ)<<16)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=3;
                        వెళ్ళు output_బైట్లు;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే ఒకవేళ(c<0xe000) {
                /* c is a surrogate */
                ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                        /* test the following code unit */
                        trail=*ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ++ఆధారం;
                            ++తదుపరి_ఆధారం_సూచీ;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                    } లేకపోతే {
                        /* no more input */
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                /* compress supplementary character */
                ఒకవేళ( (కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ఆధారం<ఆధారం_హద్దు && (uint32_t)(*ఆధారం-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_బహుళకం=నిజం;
                    dynamic_కిటికీ=కిటికీ;
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ(ఆధారం<ఆధారం_హద్దు && lead==*ఆధారం && /* too lazy to check trail in same window as source[1] */
                          (సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_బహుళకం=నిజం;
                    సంకేతం_2-=0x200;
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_కిటికీ<<21)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                వెళ్ళు output_బైట్లు;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
            ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_బహుళకం=is_Single_Byte_బహుళకం;
    scsu->from_UDynamic_కిటికీ=dynamic_కిటికీ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    p_వాదనలు->offsets=offsets;
    తిరిగి;

output_బైట్లు:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
        ఒకవేళ(offsets==శూన్యసూచిక) {
            ఎంపిక(length) {
                /* each branch falls through to the next one */
            స్థితి 4:
                *లక్ష్యం++=(uint8_t)(c>>24);
                U_FALLTHROUGH;
            స్థితి 3:
                *లక్ష్యం++=(uint8_t)(c>>16);
                U_FALLTHROUGH;
            స్థితి 2:
                *లక్ష్యం++=(uint8_t)(c>>8);
                U_FALLTHROUGH;
            స్థితి 1:
                *లక్ష్యం++=(uint8_t)c;
                U_FALLTHROUGH;
            అప్రమేయ:
                /* will never occur */
                ఆపు;
            }
        } లేకపోతే {
            ఎంపిక(length) {
                /* each branch falls through to the next one */
            స్థితి 4:
                *లక్ష్యం++=(uint8_t)(c>>24);
                *offsets++=ఆధారం_సూచీ;
                U_FALLTHROUGH;
            స్థితి 3:
                *లక్ష్యం++=(uint8_t)(c>>16);
                *offsets++=ఆధారం_సూచీ;
                U_FALLTHROUGH;
            స్థితి 2:
                *లక్ష్యం++=(uint8_t)(c>>8);
                *offsets++=ఆధారం_సూచీ;
                U_FALLTHROUGH;
            స్థితి 1:
                *లక్ష్యం++=(uint8_t)c;
                *offsets++=ఆధారం_సూచీ;
                U_FALLTHROUGH;
            అప్రమేయ:
                /* will never occur */
                ఆపు;
            }
        }
        లక్ష్యం_సామర్థ్యం-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        ఆధారం_సూచీ=తదుపరి_ఆధారం_సూచీ;
        వెళ్ళు loop;
    } లేకపోతే {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=లక్ష్యం_సామర్థ్యం;
        p=(uint8_t *)cnv->char_Error_బఫరు;
        ఎంపిక(length) {
            /* each branch falls through to the next one */
        స్థితి 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        స్థితి 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        స్థితి 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        స్థితి 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        అప్రమేయ:
            /* will never occur */
            ఆపు;
        }
        cnv->char_Error_బఫరు_నిడివి=(int8_t)length;

        /* now output what fits into the regular target */
        c>>=8*length; /* length was reduced by targetCapacity */
        ఎంపిక(లక్ష్యం_సామర్థ్యం) {
            /* each branch falls through to the next one */
        స్థితి 3:
            *లక్ష్యం++=(uint8_t)(c>>16);
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            U_FALLTHROUGH;
        స్థితి 2:
            *లక్ష్యం++=(uint8_t)(c>>8);
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            U_FALLTHROUGH;
        స్థితి 1:
            *లక్ష్యం++=(uint8_t)c;
            ఒకవేళ(offsets!=శూన్యసూచిక) {
                *offsets++=ఆధారం_సూచీ;
            }
            U_FALLTHROUGH;
        అప్రమేయ:
            ఆపు;
        }

        /* target overflow */
        లక్ష్యం_సామర్థ్యం=0;
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        c=0;
        వెళ్ళు endloop;
    }
}

/*
 * Identical to _SCSUFromUnicodeWithOffsets but without offset handling.
 * If a change is made in the original function, then either
 * change this function the same way or
 * re-copy the original function and remove the variables
 * offsets, sourceIndex, and nextSourceIndex.
 */
స్థిర శూన్య U_CALLCONV
_SCSUFromUnicode(UConverter_From_Unicode_వాదనలు *p_వాదనలు,
                 UError_సంకేతం *p_Error_సంకేతం) {
    UConverter *cnv;
    SCSUData *scsu;
    స్థిరరాశి char16_t *ఆధారం, *ఆధారం_హద్దు;
    uint8_t *లక్ష్యం;
    int32_t లక్ష్యం_సామర్థ్యం;

    UBool is_Single_Byte_బహుళకం;
    uint8_t dynamic_కిటికీ;
    uint32_t current_స్థానభ్రంశం;

    uint32_t c, delta;

    int32_t length;

    /* variables for compression heuristics */
    uint32_t స్థానభ్రంశం;
    char16_t lead, trail;
    పూర్ణాంకం సంకేతం_2;
    int8_t కిటికీ;

    /* set up the local pointers */
    cnv=p_వాదనలు->converter;
    scsu=(SCSUData *)cnv->extraInfo;

    /* set up the local pointers */
    ఆధారం=p_వాదనలు->ఆధారం;
    ఆధారం_హద్దు=p_వాదనలు->ఆధారం_హద్దు;
    లక్ష్యం=(uint8_t *)p_వాదనలు->లక్ష్యం;
    లక్ష్యం_సామర్థ్యం=(int32_t)(p_వాదనలు->లక్ష్యం_హద్దు-p_వాదనలు->లక్ష్యం);

    /* get the state machine state */
    is_Single_Byte_బహుళకం=scsu->from_UIs_Single_Byte_బహుళకం;
    dynamic_కిటికీ=scsu->from_UDynamic_కిటికీ;
    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];

    c=cnv->fromUChar32;

    /* similar conversion "loop" as in toUnicode */
loop:
    ఒకవేళ(is_Single_Byte_బహుళకం) {
        ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
            వెళ్ళు getTrailSingle;
        }

        /* state machine for single-byte mode */
/* singleByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం<=0) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            c=*ఆధారం++;

            ఒకవేళ((c-0x20)<=0x5f) {
                /* pass US-ASCII graphic character through */
                *లక్ష్యం++=(uint8_t)c;
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే ఒకవేళ(c<0x20) {
                ఒకవేళ((1UL<<c)&0x2601 /* binary 0010 0110 0000 0001, check for b==0xd || b==0xa || b==9 || b==0 */) {
                    /* CR/LF/TAB/NUL */
                    *లక్ష్యం++=(uint8_t)c;
                    --లక్ష్యం_సామర్థ్యం;
                } లేకపోతే {
                    /* quote C0 control character */
                    c|=SQ0<<8;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ((delta=c-current_స్థానభ్రంశం)<=0x7f) {
                /* use the current dynamic window */
                *లక్ష్యం++=(uint8_t)(delta|0x80);
                --లక్ష్యం_సామర్థ్యం;
            } లేకపోతే ఒకవేళ(U16_IS_SURROGATE(c)) {
                ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrailSingle:
                    lead=(char16_t)c;
                    ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                        /* test the following code unit */
                        trail=*ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ++ఆధారం;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                    } లేకపోతే {
                        /* no more input */
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                /* compress supplementary character U+10000..U+10ffff */
                ఒకవేళ((delta=c-current_స్థానభ్రంశం)<=0x7f) {
                    /* use the current dynamic window */
                    *లక్ష్యం++=(uint8_t)(delta|0x80);
                    --లక్ష్యం_సామర్థ్యం;
                } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a dynamic window that contains this character, change to it */
                    dynamic_కిటికీ=కిటికీ;
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(SC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                    /* might check if there are more characters in this window to come */
                    /* define an extended window with this character */
                    సంకేతం_2-=0x200;
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)SDX<<24)|((uint32_t)dynamic_కిటికీ<<21)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* change to Unicode mode and output this (lead, trail) pair */
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    *లక్ష్యం++=(uint8_t)SCU;
                    --లక్ష్యం_సామర్థ్యం;
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ(c<0xa0) {
                /* quote C1 control character */
                c=(c&0x7f)|(SQ0+1)<<8; /* SQ0+1==SQ1 */
                length=2;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే ఒకవేళ(c==0xfeff || c>=0xfff0) {
                /* quote signature character=byte order mark and specials */
                c|=SQU<<16;
                length=3;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే {
                /* compress all other BMP characters */
                ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                    /* there is a window defined that contains this character - switch to it or quote from it? */
                    ఒకవేళ(ఆధారం>=ఆధారం_హద్దు || is_లోపల_స్థానభ్రంశం_కిటికీ_Or_ప్రత్యక్ష(scsu->fromUDynamicOffsets[కిటికీ], *ఆధారం)) {
                        /* change to dynamic window */
                        dynamic_కిటికీ=కిటికీ;
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(SC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే {
                        /* quote from dynamic window */
                        c=((uint32_t)(SQ0+కిటికీ)<<8)|(c-scsu->fromUDynamicOffsets[కిటికీ])|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    }
                } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(staticOffsets, c))>=0) {
                    /* quote from static window */
                    c=((uint32_t)(SQ0+కిటికీ)<<8)|(c-staticOffsets[కిటికీ]);
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                    /* define a dynamic window with this character */
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(SD0+dynamic_కిటికీ)<<16)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ (c - 0x3400 < 0xd800 - 0x3400 &&
                           (ఆధారం >= ఆధారం_హద్దు || static_cast<uint32_t>(*ఆధారం - 0x3400) < 0xd800 - 0x3400)
                ) {
                    /*
                     * this character is not compressible (a BMP ideograph or similar);
                     * switch to Unicode mode if this is the last character in the block
                     * or there is at least one more ideograph following immediately
                     */
                    is_Single_Byte_బహుళకం=అబద్ధం;
                    c|=SCU<<16;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* quote Unicode */
                    c|=SQU<<16;
                    length=3;
                    వెళ్ళు output_బైట్లు;
                }
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    } లేకపోతే {
        ఒకవేళ(c!=0 && లక్ష్యం_సామర్థ్యం>0) {
            వెళ్ళు getTrailUnicode;
        }

        /* state machine for Unicode mode */
/* unicodeByteMode: */
        అయితే(ఆధారం<ఆధారం_హద్దు) {
            ఒకవేళ(లక్ష్యం_సామర్థ్యం<=0) {
                /* target is full */
                *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
                ఆపు;
            }
            c=*ఆధారం++;

            ఒకవేళ (c - 0x3400 < 0xd800 - 0x3400) {
                /* not compressible, write character directly */
                ఒకవేళ(లక్ష్యం_సామర్థ్యం>=2) {
                    *లక్ష్యం++=(uint8_t)(c>>8);
                    *లక్ష్యం++=(uint8_t)c;
                    లక్ష్యం_సామర్థ్యం-=2;
                } లేకపోతే {
                    length=2;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే ఒకవేళ (c - 0x3400 >= 0xf300 - 0x3400 /* c<0x3400 || c>=0xf300 */) {
                /* compress BMP character if the following one is not an uncompressible ideograph */
                ఒకవేళ(!(ఆధారం<ఆధారం_హద్దు && (uint32_t)(*ఆధారం-0x3400)<(0xd800-0x3400))) {
                    ఒకవేళ (c - 0x30 < 10 || c - 0x61 < 26 || c - 0x41 < 26) {
                        /* ASCII digit or letter */
                        is_Single_Byte_బహుళకం=నిజం;
                        c|=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|c;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే ఒకవేళ((కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0) {
                        /* there is a dynamic window that contains this character, change to it */
                        is_Single_Byte_బహుళకం=నిజం;
                        dynamic_కిటికీ=కిటికీ;
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=2;
                        వెళ్ళు output_బైట్లు;
                    } లేకపోతే ఒకవేళ((సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0) {
                        /* define a dynamic window with this character */
                        is_Single_Byte_బహుళకం=నిజం;
                        dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                        current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                        use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                        c=((uint32_t)(UD0+dynamic_కిటికీ)<<16)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                        length=3;
                        వెళ్ళు output_బైట్లు;
                    }
                }

                /* don't know how to compress this character, just write it directly */
                length=2;
                వెళ్ళు output_బైట్లు;
            } లేకపోతే ఒకవేళ(c<0xe000) {
                /* c is a surrogate */
                ఒకవేళ(U16_IS_SURROGATE_LEAD(c)) {
getTrailUnicode:
                    lead=(char16_t)c;
                    ఒకవేళ(ఆధారం<ఆధారం_హద్దు) {
                        /* test the following code unit */
                        trail=*ఆధారం;
                        ఒకవేళ(U16_IS_TRAIL(trail)) {
                            ++ఆధారం;
                            c=U16_GET_SUPPLEMENTARY(c, trail);
                            /* convert this surrogate code point */
                            /* exit this condition tree */
                        } లేకపోతే {
                            /* this is an unmatched lead code unit (1st surrogate) */
                            /* callback(illegal) */
                            *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                            వెళ్ళు endloop;
                        }
                    } లేకపోతే {
                        /* no more input */
                        ఆపు;
                    }
                } లేకపోతే {
                    /* this is an unmatched trail code unit (2nd surrogate) */
                    /* callback(illegal) */
                    *p_Error_సంకేతం=U_ILLEGAL_CHAR_దొరికింది;
                    వెళ్ళు endloop;
                }

                /* compress supplementary character */
                ఒకవేళ( (కిటికీ=get_కిటికీ(scsu->fromUDynamicOffsets, c))>=0 &&
                    !(ఆధారం<ఆధారం_హద్దు && (uint32_t)(*ఆధారం-0x3400)<(0xd800-0x3400))
                ) {
                    /*
                     * there is a dynamic window that contains this character and
                     * the following character is not uncompressible,
                     * change to the window
                     */
                    is_Single_Byte_బహుళకం=నిజం;
                    dynamic_కిటికీ=కిటికీ;
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ];
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)(UC0+dynamic_కిటికీ)<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=2;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే ఒకవేళ(ఆధారం<ఆధారం_హద్దు && lead==*ఆధారం && /* too lazy to check trail in same window as source[1] */
                          (సంకేతం_2=get_Dynamic_స్థానభ్రంశం(c, &స్థానభ్రంశం))>=0
                ) {
                    /* two supplementary characters in (probably) the same window - define an extended one */
                    is_Single_Byte_బహుళకం=నిజం;
                    సంకేతం_2-=0x200;
                    dynamic_కిటికీ=get_తదుపరి_Dynamic_కిటికీ(scsu);
                    current_స్థానభ్రంశం=scsu->fromUDynamicOffsets[dynamic_కిటికీ]=స్థానభ్రంశం;
                    use_Dynamic_కిటికీ(scsu, dynamic_కిటికీ);
                    c=((uint32_t)UDX<<24)|((uint32_t)dynamic_కిటికీ<<21)|((uint32_t)సంకేతం_2<<8)|(c-current_స్థానభ్రంశం)|0x80;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                } లేకపోతే {
                    /* don't know how to compress this character, just write it directly */
                    c=((uint32_t)lead<<16)|trail;
                    length=4;
                    వెళ్ళు output_బైట్లు;
                }
            } లేకపోతే /* 0xe000<=c<0xf300 */ {
                /* quote to avoid SCSU tags */
                c|=UQU<<16;
                length=3;
                వెళ్ళు output_బైట్లు;
            }

            /* normal end of conversion: prepare for a new character */
            c=0;
        }
    }
endloop:

    /* set the converter state back into UConverter */
    scsu->from_UIs_Single_Byte_బహుళకం=is_Single_Byte_బహుళకం;
    scsu->from_UDynamic_కిటికీ=dynamic_కిటికీ;

    cnv->fromUChar32=c;

    /* write back the updated pointers */
    p_వాదనలు->ఆధారం=ఆధారం;
    p_వాదనలు->లక్ష్యం=(అక్షరం *)లక్ష్యం;
    తిరిగి;

output_బైట్లు:
    /* write the output character bytes from c and length [code copied from ucnvmbcs.c] */
    /* from the first if in the loop we know that targetCapacity>0 */
    ఒకవేళ(length<=లక్ష్యం_సామర్థ్యం) {
        ఎంపిక(length) {
            /* each branch falls through to the next one */
        స్థితి 4:
            *లక్ష్యం++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        స్థితి 3:
            *లక్ష్యం++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        స్థితి 2:
            *లక్ష్యం++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        స్థితి 1:
            *లక్ష్యం++=(uint8_t)c;
            U_FALLTHROUGH;
        అప్రమేయ:
            /* will never occur */
            ఆపు;
        }
        లక్ష్యం_సామర్థ్యం-=length;

        /* normal end of conversion: prepare for a new character */
        c=0;
        వెళ్ళు loop;
    } లేకపోతే {
        uint8_t *p;

        /*
         * We actually do this backwards here:
         * In order to save an intermediate variable, we output
         * first to the overflow buffer what does not fit into the
         * regular target.
         */
        /* we know that 0<=targetCapacity<length<=4 */
        /* targetCapacity==0 when SCU+supplementary where SCU used up targetCapacity==1 */
        length-=లక్ష్యం_సామర్థ్యం;
        p=(uint8_t *)cnv->char_Error_బఫరు;
        ఎంపిక(length) {
            /* each branch falls through to the next one */
        స్థితి 4:
            *p++=(uint8_t)(c>>24);
            U_FALLTHROUGH;
        స్థితి 3:
            *p++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        స్థితి 2:
            *p++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        స్థితి 1:
            *p=(uint8_t)c;
            U_FALLTHROUGH;
        అప్రమేయ:
            /* will never occur */
            ఆపు;
        }
        cnv->char_Error_బఫరు_నిడివి=(int8_t)length;

        /* now output what fits into the regular target */
        c = (length == 4) ? 0 : c >> 8*length; /* length was reduced by targetCapacity */
        ఎంపిక(లక్ష్యం_సామర్థ్యం) {
            /* each branch falls through to the next one */
        స్థితి 3:
            *లక్ష్యం++=(uint8_t)(c>>16);
            U_FALLTHROUGH;
        స్థితి 2:
            *లక్ష్యం++=(uint8_t)(c>>8);
            U_FALLTHROUGH;
        స్థితి 1:
            *లక్ష్యం++=(uint8_t)c;
            U_FALLTHROUGH;
        అప్రమేయ:
            ఆపు;
        }

        /* target overflow */
        లక్ష్యం_సామర్థ్యం=0;
        *p_Error_సంకేతం=U_బఫరు_అధికప్రవాహం_ERROR;
        c=0;
        వెళ్ళు endloop;
    }
}

/* miscellaneous ------------------------------------------------------------ */

స్థిర స్థిరరాశి అక్షరం *  U_CALLCONV
_SCSUGetName(స్థిరరాశి UConverter *cnv) {
    SCSUData *scsu=(SCSUData *)cnv->extraInfo;

    ఎంపిక(scsu->locale) {
    స్థితి l_ja:
        తిరిగి "SCSU,locale=ja";
    అప్రమేయ:
        తిరిగి "SCSU";
    }
}

/* structure for SafeClone calculations */
నిర్మాణం cloneSCSUStruct
{
    UConverter cnv;
    SCSUData mydata;
};

స్థిర UConverter *  U_CALLCONV
_SCSUSafeClone(స్థిరరాశి UConverter *cnv, 
               శూన్య *దొంతర_బఫరు, 
               int32_t *p_బఫరు_కొలత, 
               UError_సంకేతం *స్థితి_2)
{
    నిర్మాణం cloneSCSUStruct * localClone;
    int32_t బఫరు_కొలత_Needed = పరిమాణం(నిర్మాణం cloneSCSUStruct);

    ఒకవేళ (U_FAILURE(*స్థితి_2)){
        తిరిగి శూన్యసూచిక;
    }

    ఒకవేళ (*p_బఫరు_కొలత == 0){ /* 'preflighting' request - set needed size into *pBufferSize */
        *p_బఫరు_కొలత = బఫరు_కొలత_Needed;
        తిరిగి శూన్యసూచిక;
    }

    localClone = (నిర్మాణం cloneSCSUStruct *)దొంతర_బఫరు;
    /* ucnv.c/ucnv_safeClone() copied the main UConverter already */

    uprv_memcpy(&localClone->mydata, cnv->extraInfo, పరిమాణం(SCSUData));
    localClone->cnv.extraInfo = &localClone->mydata;
    localClone->cnv.isExtraLocal = నిజం;

    తిరిగి &localClone->cnv;
}
U_CDECL_చివర

స్థిర స్థిరరాశి UConverterImpl _SCSUImpl={
    UCNV_SCSU,

    శూన్యసూచిక,
    శూన్యసూచిక,

    _SCSUOpen,
    _SCSUClose,
    _SCSUReset,

    _SCSUToUnicode,
    _SCSUToUnicodeWithOffsets,
    _SCSUFromUnicode,
    _SCSUFromUnicodeWithOffsets,
    శూన్యసూచిక,

    శూన్యసూచిక,
    _SCSUGetName,
    శూన్యసూచిక,
    _SCSUSafeClone,
    ucnv_get_Complete_Unicode_సమితి,
    శూన్యసూచిక,
    శూన్యసూచిక
};

స్థిర స్థిరరాశి UConverter_Static_దత్తాంశం SCSUStatic_దత్తాంశం={
    పరిమాణం(UConverter_Static_దత్తాంశం),
    "SCSU",
    1212, /* CCSID for SCSU */
    UCNV_IBM, UCNV_SCSU,
    1, 3, /* one char16_t generates at least 1 byte and at most 3 bytes */
    /*
     * The subchar here is ignored because _SCSUOpen() sets U+fffd as a Unicode
     * substitution string.
     */
    { 0x0e, 0xff, 0xfd, 0 }, 3,
    అబద్ధం, అబద్ధం,
    0,
    0,
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } /* reserved */
};

స్థిరరాశి UConverter_Shared_దత్తాంశం _SCSUData=
        UCNV_IMMUTABLE_SHARED_దత్తాంశం_INITIALIZER(&SCSUStatic_దత్తాంశం, &_SCSUImpl);

#అయితే_ముగింపు
