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

#அந்தர்பாவ "unicode/utrace.h"
#அந்தர்பாவ "utracimp.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "uassert.h"
#அந்தர்பாவ "ucln_cmn.h"


நிலையான UTraceEntry     *pTraceEntryFunc = nullptr;
நிலையான UTraceExit      *pTraceExitFunc  = nullptr;
நிலையான UTraceData      *pTraceDataFunc  = nullptr;
நிலையான மாறிலி வெறுமை      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
நிலையான int32_t
utrace_level = UTRACE_ERROR;

U_CAPI வெறுமை U_EXPORT2
utrace_entry(int32_t fn_எண்) {
    எனில் (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_எண்);
    }
}


நிலையான மாறிலி எழுத்து gExitFmt[]             = "Returns.";
நிலையான மாறிலி எழுத்து g_Exit_Fmt_மதிப்பு[]        = "Returns %d.";
நிலையான மாறிலி எழுத்து gExitFmtStatus[]       = "Returns.  Status = %d.";
நிலையான மாறிலி எழுத்து g_Exit_Fmt_மதிப்பு_Status[]  = "Returns %d.  Status = %d.";
நிலையான மாறிலி எழுத்து gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI வெறுமை U_EXPORT2
utrace_exit(int32_t fn_எண், int32_t returnType, ...) {
    எனில் (pTraceExitFunc != nullptr) {
        va_list     args;
        மாறிலி எழுத்து *fmt;

        தேர்வு (returnType) {
        நிலை 0:
            fmt = gExitFmt;
            நிறுத்து;
        நிலை UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_மதிப்பு;
            நிறுத்து;
        நிலை UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            நிறுத்து;
        நிலை UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_மதிப்பு_Status;
            நிறுத்து;
        நிலை UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            நிறுத்து;
        இயல்பு:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_எண், fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI வெறுமை U_EXPORT2 
utrace_data(int32_t fn_எண், int32_t level, மாறிலி எழுத்து *fmt, ...) {
    எனில் (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_எண், level, fmt, args);
           va_end(args);
    }
}


நிலையான வெறுமை outputChar(எழுத்து c, எழுத்து *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    எனில் (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        க்கு(i=0; i<indent; i++) {
            எனில் (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    எனில் (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    எனில் (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

நிலையான வெறுமை outputHexBytes(int64_t val, int32_t charsToOutput,
                           எழுத்து *outBuf, int32_t *outIx, int32_t capacity) {
    நிலையான மாறிலி எழுத்து gHexChars[] = "0123456789abcdef";
    int32_t shift_மொத்தம்;
    க்கு  (shift_மொத்தம்=(charsToOutput-1)*4; shift_மொத்தம் >= 0; shift_மொத்தம்-=4) {
        எழுத்து c = gHexChars[(val >> shift_மொத்தம்) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
நிலையான வெறுமை outputPtrBytes(வெறுமை *val, எழுத்து *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    எழுத்து* p = reinterpret_cast<எழுத்து*>(&val); /* point to current byte to output in the ptr val  */

#எனில் !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += அளவு(வெறுமை *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    க்கு (i=0; i<அளவு(வெறுமை *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

நிலையான வெறுமை outputString(மாறிலி எழுத்து *s, எழுத்து *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    எழுத்து    c;
    எனில் (s==nullptr) {
        s = "*NULL*";
    }
    செய் {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } போது (c != 0);
}
        


நிலையான வெறுமை outputUString(மாறிலி char16_t *s, int32_t len,
                          எழுத்து *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    எனில் (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        திரும்பு;
    }

    க்கு (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        எனில் (len == -1 && c==0) {
            நிறுத்து;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(எழுத்து *outBuf, int32_t capacity, int32_t indent, மாறிலி எழுத்து *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    எழுத்து      fmtC;
    எழுத்து      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    எழுத்து      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    க்கு (;;) {
        fmtC = fmt[fmtIx++];
        எனில் (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            எனில் (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                நிறுத்து;
            }
            தொடர்;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        தேர்வு (fmtC) {
        நிலை 'c':
            /* single 8 bit char   */
            c = (எழுத்து)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            நிறுத்து;

        நிலை 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, எழுத்து *);
            outputString((மாறிலி எழுத்து *)ptrArg, outBuf, &outIx, capacity, indent);
            நிறுத்து;

        நிலை 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, எழுத்து *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((மாறிலி char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            நிறுத்து;

        நிலை 'b':
            /*  8 bit int  */
            intArg = va_arg(args, முழு);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            நிறுத்து;

        நிலை 'h':
            /*  16 bit int  */
            intArg = va_arg(args, முழு);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            நிறுத்து;

        நிலை 'd':
            /*  32 bit int  */
            intArg = va_arg(args, முழு);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            நிறுத்து;

        நிலை 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            நிறுத்து;
            
        நிலை 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, எழுத்து *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            நிறுத்து;

        நிலை 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            நிறுத்து;

        நிலை 'v':
            {
                /* Vector of values, e.g. %vh */
                எழுத்து     vectorType;
                int32_t  vectorLen;
                மாறிலி எழுத்து   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                வெறுமை     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                எனில் (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (மாறிலி எழுத்து *)va_arg(args, வெறுமை*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (வெறுமை **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                எனில் (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } இல்லையெனில் {
                    க்கு (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        தேர்வு (vectorType) {
                        நிலை 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            நிறுத்து;
                        நிலை 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            நிறுத்து;
                        நிலை 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            நிறுத்து;
                        நிலை 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            நிறுத்து;
                        நிலை 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            நிறுத்து;
                        நிலை 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            நிறுத்து;
                        நிலை 's':
                            charsToOutput = 0;
                            outputString((மாறிலி எழுத்து *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            நிறுத்து;

                        நிலை 'S':
                            charsToOutput = 0;
                            outputUString((மாறிலி char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            நிறுத்து;

                            
                        }
                        எனில் (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        எனில் (vectorLen == -1 && longArg == 0) {
                            நிறுத்து;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            நிறுத்து;


        இயல்பு:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    திரும்பு outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(எழுத்து *outBuf, int32_t capacity,
                int32_t indent, மாறிலி எழுத்து *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    திரும்பு retVal;
}


U_CAPI வெறுமை U_EXPORT2
utrace_setFunctions(மாறிலி வெறுமை *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI வெறுமை U_EXPORT2
utrace_getFunctions(மாறிலி வெறுமை **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI வெறுமை U_EXPORT2
utrace_setLevel(int32_t level) {
    எனில் (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    எனில் (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    திரும்பு utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    திரும்பு உண்மை;
}


நிலையான மாறிலி எழுத்து * மாறிலி
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


நிலையான மாறிலி எழுத்து * மாறிலி
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

    
நிலையான மாறிலி எழுத்து * மாறிலி
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


நிலையான மாறிலி எழுத்து* மாறிலி
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI மாறிலி எழுத்து * U_EXPORT2
utrace_functionName(int32_t fn_எண்) {
    எனில்(UTRACE_FUNCTION_START <= fn_எண் && fn_எண் < UTRACE_FUNCTION_LIMIT) {
        திரும்பு trFnName[fn_எண்];
    } இல்லையெனில் எனில்(UTRACE_CONVERSION_START <= fn_எண் && fn_எண் < UTRACE_CONVERSION_LIMIT) {
        திரும்பு trConvNames[fn_எண் - UTRACE_CONVERSION_START];
    } இல்லையெனில் எனில்(UTRACE_COLLATION_START <= fn_எண் && fn_எண் < UTRACE_COLLATION_LIMIT){
        திரும்பு trCollNames[fn_எண் - UTRACE_COLLATION_START];
    } இல்லையெனில் எனில்(UTRACE_UDATA_START <= fn_எண் && fn_எண் < UTRACE_RES_DATA_LIMIT){
        திரும்பு trResDataNames[fn_எண் - UTRACE_UDATA_START];
    } இல்லையெனில் {
        திரும்பு "[BOGUS Trace Function Number]";
    }
}

