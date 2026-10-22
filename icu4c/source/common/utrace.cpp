// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*   Copyright (C) 2003-2014, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  utrace.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:4
*/

#അന്തര്ഭാവ "unicode/utrace.h"
#അന്തര്ഭാവ "utracimp.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "uassert.h"
#അന്തര്ഭാവ "ucln_cmn.h"


സ്ഥിതം UTraceEntry     *pTraceEntryFunc = nullptr;
സ്ഥിതം UTraceExit      *pTraceExitFunc  = nullptr;
സ്ഥിതം UTraceData      *pTraceDataFunc  = nullptr;
സ്ഥിതം സ്ഥിരം ശൂന്യം      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
സ്ഥിതം int32_t
utrace_level = UTRACE_ERROR;

U_CAPI ശൂന്യം U_EXPORT2
utrace_entry(int32_t fn_സംഖ്യ) {
    എങ്കിൽ (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_സംഖ്യ);
    }
}


സ്ഥിതം സ്ഥിരം അക്ഷരം gExitFmt[]             = "Returns.";
സ്ഥിതം സ്ഥിരം അക്ഷരം g_Exit_Fmt_മൂല്യം[]        = "Returns %d.";
സ്ഥിതം സ്ഥിരം അക്ഷരം gExitFmtStatus[]       = "Returns.  Status = %d.";
സ്ഥിതം സ്ഥിരം അക്ഷരം g_Exit_Fmt_മൂല്യം_Status[]  = "Returns %d.  Status = %d.";
സ്ഥിതം സ്ഥിരം അക്ഷരം gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI ശൂന്യം U_EXPORT2
utrace_exit(int32_t fn_സംഖ്യ, int32_t returnType, ...) {
    എങ്കിൽ (pTraceExitFunc != nullptr) {
        va_list     args;
        സ്ഥിരം അക്ഷരം *fmt;

        തിരഞ്ഞെടുപ്പ് (returnType) {
        അവസ്ഥ 0:
            fmt = gExitFmt;
            നിർത്തുക;
        അവസ്ഥ UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_മൂല്യം;
            നിർത്തുക;
        അവസ്ഥ UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            നിർത്തുക;
        അവസ്ഥ UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_മൂല്യം_Status;
            നിർത്തുക;
        അവസ്ഥ UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_സംഖ്യ, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI ശൂന്യം U_EXPORT2 
utrace_data(int32_t fn_സംഖ്യ, int32_t level, സ്ഥിരം അക്ഷരം *fmt, ...) {
    എങ്കിൽ (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_സംഖ്യ, level, fmt, args);
           va_end(args);
    }
}


സ്ഥിതം ശൂന്യം outputChar(അക്ഷരം c, അക്ഷരം *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i;
    /* Check whether a start of line indenting is needed.  Three cases:
     *   1.  At the start of the first line  (output index == 0).
     *   2.  At the start of subsequent lines  (preceding char in buffer == '\n')
     *   3.  When preflighting buffer len (buffer capacity is exceeded), when
     *       a \n is output.  Ideally we wouldn't do the indent until the following char
     *       is received, but that won't work because there's no place to remember that
     *       the preceding char was \n.  Meaning that we may overstimate the
     *       buffer size needed.  No harm done.
     */
    എങ്കിൽ (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        വേണ്ടി(i=0; i<indent; i++) {
            എങ്കിൽ (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    എങ്കിൽ (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    എങ്കിൽ (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

സ്ഥിതം ശൂന്യം outputHexBytes(int64_t val, int32_t charsToOutput,
                           അക്ഷരം *outBuf, int32_t *outIx, int32_t capacity) {
    സ്ഥിതം സ്ഥിരം അക്ഷരം gHexChars[] = "0123456789abcdef";
    int32_t shift_എണ്ണം;
    വേണ്ടി  (shift_എണ്ണം=(charsToOutput-1)*4; shift_എണ്ണം >= 0; shift_എണ്ണം-=4) {
        അക്ഷരം c = gHexChars[(val >> shift_എണ്ണം) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
സ്ഥിതം ശൂന്യം outputPtrBytes(ശൂന്യം *val, അക്ഷരം *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    അക്ഷരം* p = reinterpret_cast<അക്ഷരം*>(&val); /* point to current byte to output in the ptr val  */

#എങ്കിൽ !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += വലുപ്പം(ശൂന്യം *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    വേണ്ടി (i=0; i<വലുപ്പം(ശൂന്യം *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

സ്ഥിതം ശൂന്യം outputString(സ്ഥിരം അക്ഷരം *s, അക്ഷരം *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    അക്ഷരം    c;
    എങ്കിൽ (s==nullptr) {
        s = "*NULL*";
    }
    ചെയ്യുക {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } ആകുമ്പോൾ (c != 0);
}
        


സ്ഥിതം ശൂന്യം outputUString(സ്ഥിരം char16_t *s, int32_t len,
                          അക്ഷരം *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    എങ്കിൽ (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        തിരികെ;
    }

    വേണ്ടി (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        എങ്കിൽ (len == -1 && c==0) {
            നിർത്തുക;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(അക്ഷരം *outBuf, int32_t capacity, int32_t indent, സ്ഥിരം അക്ഷരം *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    അക്ഷരം      fmtC;
    അക്ഷരം      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    അക്ഷരം      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    വേണ്ടി (;;) {
        fmtC = fmt[fmtIx++];
        എങ്കിൽ (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            എങ്കിൽ (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                നിർത്തുക;
            }
            തുടരുക;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        തിരഞ്ഞെടുപ്പ് (fmtC) {
        അവസ്ഥ 'c':
            /* single 8 bit char   */
            c = (അക്ഷരം)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            നിർത്തുക;

        അവസ്ഥ 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, അക്ഷരം *);
            outputString((സ്ഥിരം അക്ഷരം *)ptrArg, outBuf, &outIx, capacity, indent);
            നിർത്തുക;

        അവസ്ഥ 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, അക്ഷരം *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((സ്ഥിരം char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            നിർത്തുക;

        അവസ്ഥ 'b':
            /*  8 bit int  */
            intArg = va_arg(args, പൂർണ്ണസംഖ്യ);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            നിർത്തുക;

        അവസ്ഥ 'h':
            /*  16 bit int  */
            intArg = va_arg(args, പൂർണ്ണസംഖ്യ);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            നിർത്തുക;

        അവസ്ഥ 'd':
            /*  32 bit int  */
            intArg = va_arg(args, പൂർണ്ണസംഖ്യ);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            നിർത്തുക;

        അവസ്ഥ 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            നിർത്തുക;
            
        അവസ്ഥ 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, അക്ഷരം *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            നിർത്തുക;

        അവസ്ഥ 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            നിർത്തുക;

        അവസ്ഥ 'v':
            {
                /* Vector of values, e.g. %vh */
                അക്ഷരം     vectorType;
                int32_t  vectorLen;
                സ്ഥിരം അക്ഷരം   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                ശൂന്യം     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                എങ്കിൽ (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (സ്ഥിരം അക്ഷരം *)va_arg(args, ശൂന്യം*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (ശൂന്യം **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                എങ്കിൽ (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } അല്ലെങ്കിൽ {
                    വേണ്ടി (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        തിരഞ്ഞെടുപ്പ് (vectorType) {
                        അവസ്ഥ 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            നിർത്തുക;
                        അവസ്ഥ 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            നിർത്തുക;
                        അവസ്ഥ 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            നിർത്തുക;
                        അവസ്ഥ 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            നിർത്തുക;
                        അവസ്ഥ 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            നിർത്തുക;
                        അവസ്ഥ 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            നിർത്തുക;
                        അവസ്ഥ 's':
                            charsToOutput = 0;
                            outputString((സ്ഥിരം അക്ഷരം *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            നിർത്തുക;

                        അവസ്ഥ 'S':
                            charsToOutput = 0;
                            outputUString((സ്ഥിരം char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            നിർത്തുക;

                            
                        }
                        എങ്കിൽ (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        എങ്കിൽ (vectorLen == -1 && longArg == 0) {
                            നിർത്തുക;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            നിർത്തുക;


        സ്ഥിരസ്ഥിതി:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    തിരികെ outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(അക്ഷരം *outBuf, int32_t capacity,
                int32_t indent, സ്ഥിരം അക്ഷരം *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    തിരികെ retVal;
}


U_CAPI ശൂന്യം U_EXPORT2
utrace_setFunctions(സ്ഥിരം ശൂന്യം *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI ശൂന്യം U_EXPORT2
utrace_getFunctions(സ്ഥിരം ശൂന്യം **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI ശൂന്യം U_EXPORT2
utrace_setLevel(int32_t level) {
    എങ്കിൽ (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    എങ്കിൽ (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    തിരികെ utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    തിരികെ സത്യം;
}


സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം
trConvNames[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    nullptr
};

    
സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം
trCollNames[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    nullptr
};


സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI സ്ഥിരം അക്ഷരം * U_EXPORT2
utrace_functionName(int32_t fn_സംഖ്യ) {
    എങ്കിൽ(UTRACE_FUNCTION_START <= fn_സംഖ്യ && fn_സംഖ്യ < UTRACE_FUNCTION_LIMIT) {
        തിരികെ trFnName[fn_സംഖ്യ];
    } അല്ലെങ്കിൽ എങ്കിൽ(UTRACE_CONVERSION_START <= fn_സംഖ്യ && fn_സംഖ്യ < UTRACE_CONVERSION_LIMIT) {
        തിരികെ trConvNames[fn_സംഖ്യ - UTRACE_CONVERSION_START];
    } അല്ലെങ്കിൽ എങ്കിൽ(UTRACE_COLLATION_START <= fn_സംഖ്യ && fn_സംഖ്യ < UTRACE_COLLATION_LIMIT){
        തിരികെ trCollNames[fn_സംഖ്യ - UTRACE_COLLATION_START];
    } അല്ലെങ്കിൽ എങ്കിൽ(UTRACE_UDATA_START <= fn_സംഖ്യ && fn_സംഖ്യ < UTRACE_RES_DATA_LIMIT){
        തിരികെ trResDataNames[fn_സംഖ്യ - UTRACE_UDATA_START];
    } അല്ലെങ്കിൽ {
        തിരികെ "[BOGUS Trace Function Number]";
    }
}

