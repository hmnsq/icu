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

#ଅନ୍ତର୍ଭାଵ "unicode/utrace.h"
#ଅନ୍ତର୍ଭାଵ "utracimp.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"
#ଅନ୍ତର୍ଭାଵ "ucln_cmn.h"


ସ୍ଥିର UTraceEntry     *pTraceEntryFunc = nullptr;
ସ୍ଥିର UTraceExit      *pTraceExitFunc  = nullptr;
ସ୍ଥିର UTraceData      *pTraceDataFunc  = nullptr;
ସ୍ଥିର ଅଚଳ ଶୂନ୍ୟ      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ସ୍ଥିର int32_t
utrace_level = UTRACE_ERROR;

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrace_entry(int32_t fn_ସଂଖ୍ୟା) {
    ଯଦି (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_ସଂଖ୍ୟା);
    }
}


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର gExitFmt[]             = "Returns.";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର g_Exit_Fmt_ମୂଲ୍ୟ[]        = "Returns %d.";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର gExitFmtStatus[]       = "Returns.  Status = %d.";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର g_Exit_Fmt_ମୂଲ୍ୟ_Status[]  = "Returns %d.  Status = %d.";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrace_exit(int32_t fn_ସଂଖ୍ୟା, int32_t returnType, ...) {
    ଯଦି (pTraceExitFunc != nullptr) {
        va_list     args;
        ଅଚଳ ଅକ୍ଷର *fmt;

        ବାଛ (returnType) {
        ସ୍ଥିତି 0:
            fmt = gExitFmt;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_ମୂଲ୍ୟ;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_ମୂଲ୍ୟ_Status;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_ସଂଖ୍ୟା, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI ଶୂନ୍ୟ U_EXPORT2 
utrace_data(int32_t fn_ସଂଖ୍ୟା, int32_t level, ଅଚଳ ଅକ୍ଷର *fmt, ...) {
    ଯଦି (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_ସଂଖ୍ୟା, level, fmt, args);
           va_end(args);
    }
}


ସ୍ଥିର ଶୂନ୍ୟ outputChar(ଅକ୍ଷର c, ଅକ୍ଷର *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    ଯଦି (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        ପାଇଁ(i=0; i<indent; i++) {
            ଯଦି (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    ଯଦି (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    ଯଦି (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

ସ୍ଥିର ଶୂନ୍ୟ outputHexBytes(int64_t val, int32_t charsToOutput,
                           ଅକ୍ଷର *outBuf, int32_t *outIx, int32_t capacity) {
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର gHexChars[] = "0123456789abcdef";
    int32_t shift_ଗଣନା;
    ପାଇଁ  (shift_ଗଣନା=(charsToOutput-1)*4; shift_ଗଣନା >= 0; shift_ଗଣନା-=4) {
        ଅକ୍ଷର c = gHexChars[(val >> shift_ଗଣନା) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ସ୍ଥିର ଶୂନ୍ୟ outputPtrBytes(ଶୂନ୍ୟ *val, ଅକ୍ଷର *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    ଅକ୍ଷର* p = reinterpret_cast<ଅକ୍ଷର*>(&val); /* point to current byte to output in the ptr val  */

#ଯଦି !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += ଆକାର(ଶୂନ୍ୟ *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    ପାଇଁ (i=0; i<ଆକାର(ଶୂନ୍ୟ *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

ସ୍ଥିର ଶୂନ୍ୟ outputString(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    ଅକ୍ଷର    c;
    ଯଦି (s==nullptr) {
        s = "*NULL*";
    }
    କର {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } ଯେତେବେଳେ (c != 0);
}
        


ସ୍ଥିର ଶୂନ୍ୟ outputUString(ଅଚଳ char16_t *s, int32_t len,
                          ଅକ୍ଷର *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    ଯଦି (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        ଫେରନ୍ତୁ;
    }

    ପାଇଁ (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        ଯଦି (len == -1 && c==0) {
            ଭାଙ୍ଗ;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(ଅକ୍ଷର *outBuf, int32_t capacity, int32_t indent, ଅଚଳ ଅକ୍ଷର *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    ଅକ୍ଷର      fmtC;
    ଅକ୍ଷର      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    ଅକ୍ଷର      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    ପାଇଁ (;;) {
        fmtC = fmt[fmtIx++];
        ଯଦି (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            ଯଦି (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                ଭାଙ୍ଗ;
            }
            ଜାରି;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        ବାଛ (fmtC) {
        ସ୍ଥିତି 'c':
            /* single 8 bit char   */
            c = (ଅକ୍ଷର)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, ଅକ୍ଷର *);
            outputString((ଅଚଳ ଅକ୍ଷର *)ptrArg, outBuf, &outIx, capacity, indent);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, ଅକ୍ଷର *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((ଅଚଳ char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'b':
            /*  8 bit int  */
            intArg = va_arg(args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'h':
            /*  16 bit int  */
            intArg = va_arg(args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'd':
            /*  32 bit int  */
            intArg = va_arg(args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            ଭାଙ୍ଗ;
            
        ସ୍ଥିତି 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, ଅକ୍ଷର *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            ଭାଙ୍ଗ;

        ସ୍ଥିତି 'v':
            {
                /* Vector of values, e.g. %vh */
                ଅକ୍ଷର     vectorType;
                int32_t  vectorLen;
                ଅଚଳ ଅକ୍ଷର   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                ଶୂନ୍ୟ     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                ଯଦି (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (ଅଚଳ ଅକ୍ଷର *)va_arg(args, ଶୂନ୍ୟ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (ଶୂନ୍ୟ **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                ଯଦି (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } ଅନ୍ୟଥା {
                    ପାଇଁ (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        ବାଛ (vectorType) {
                        ସ୍ଥିତି 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            ଭାଙ୍ଗ;
                        ସ୍ଥିତି 's':
                            charsToOutput = 0;
                            outputString((ଅଚଳ ଅକ୍ଷର *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ଭାଙ୍ଗ;

                        ସ୍ଥିତି 'S':
                            charsToOutput = 0;
                            outputUString((ଅଚଳ char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ଭାଙ୍ଗ;

                            
                        }
                        ଯଦି (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        ଯଦି (vectorLen == -1 && longArg == 0) {
                            ଭାଙ୍ଗ;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            ଭାଙ୍ଗ;


        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    ଫେରନ୍ତୁ outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(ଅକ୍ଷର *outBuf, int32_t capacity,
                int32_t indent, ଅଚଳ ଅକ୍ଷର *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    ଫେରନ୍ତୁ retVal;
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrace_setFunctions(ଅଚଳ ଶୂନ୍ୟ *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrace_getFunctions(ଅଚଳ ଶୂନ୍ୟ **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI ଶୂନ୍ୟ U_EXPORT2
utrace_setLevel(int32_t level) {
    ଯଦି (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    ଯଦି (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    ଫେରନ୍ତୁ utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    ଫେରନ୍ତୁ ସତ୍ୟ;
}


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ
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

    
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ
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


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* ଅଚଳ
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ଅଚଳ ଅକ୍ଷର * U_EXPORT2
utrace_functionName(int32_t fn_ସଂଖ୍ୟା) {
    ଯଦି(UTRACE_FUNCTION_START <= fn_ସଂଖ୍ୟା && fn_ସଂଖ୍ୟା < UTRACE_FUNCTION_LIMIT) {
        ଫେରନ୍ତୁ trFnName[fn_ସଂଖ୍ୟା];
    } ଅନ୍ୟଥା ଯଦି(UTRACE_CONVERSION_START <= fn_ସଂଖ୍ୟା && fn_ସଂଖ୍ୟା < UTRACE_CONVERSION_LIMIT) {
        ଫେରନ୍ତୁ trConvNames[fn_ସଂଖ୍ୟା - UTRACE_CONVERSION_START];
    } ଅନ୍ୟଥା ଯଦି(UTRACE_COLLATION_START <= fn_ସଂଖ୍ୟା && fn_ସଂଖ୍ୟା < UTRACE_COLLATION_LIMIT){
        ଫେରନ୍ତୁ trCollNames[fn_ସଂଖ୍ୟା - UTRACE_COLLATION_START];
    } ଅନ୍ୟଥା ଯଦି(UTRACE_UDATA_START <= fn_ସଂଖ୍ୟା && fn_ସଂଖ୍ୟା < UTRACE_RES_DATA_LIMIT){
        ଫେରନ୍ତୁ trResDataNames[fn_ସଂଖ୍ୟା - UTRACE_UDATA_START];
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ "[BOGUS Trace Function Number]";
    }
}

