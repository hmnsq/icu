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

#شامل "unicode/utrace.h"
#شامل "utracimp.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"


ایستا UTraceEntry     *pTraceEntryFunc = nullptr;
ایستا UTraceExit      *pTraceExitFunc  = nullptr;
ایستا UTraceData      *pTraceDataFunc  = nullptr;
ایستا ثابت تهی      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ایستا int32_t
utrace_level = UTRACE_ERROR;

U_CAPI تهی U_EXPORT2
utrace_entry(int32_t fn_عدد) {
    اگر (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_عدد);
    }
}


ایستا ثابت نویسه gExitFmt[]             = "Returns.";
ایستا ثابت نویسه g_Exit_Fmt_مقدار[]        = "Returns %d.";
ایستا ثابت نویسه gExitFmtStatus[]       = "Returns.  Status = %d.";
ایستا ثابت نویسه g_Exit_Fmt_مقدار_Status[]  = "Returns %d.  Status = %d.";
ایستا ثابت نویسه gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI تهی U_EXPORT2
utrace_exit(int32_t fn_عدد, int32_t returnType, ...) {
    اگر (pTraceExitFunc != nullptr) {
        va_list     args;
        ثابت نویسه *fmt;

        انتخاب (returnType) {
        حالت 0:
            fmt = gExitFmt;
            بشکن;
        حالت UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_مقدار;
            بشکن;
        حالت UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            بشکن;
        حالت UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_مقدار_Status;
            بشکن;
        حالت UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            بشکن;
        پیشفرض:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_عدد, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI تهی U_EXPORT2 
utrace_data(int32_t fn_عدد, int32_t level, ثابت نویسه *fmt, ...) {
    اگر (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_عدد, level, fmt, args);
           va_end(args);
    }
}


ایستا تهی outputChar(نویسه c, نویسه *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    اگر (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        برای(i=0; i<indent; i++) {
            اگر (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    اگر (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    اگر (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

ایستا تهی outputHexBytes(int64_t val, int32_t charsToOutput,
                           نویسه *outBuf, int32_t *outIx, int32_t capacity) {
    ایستا ثابت نویسه gHexChars[] = "0123456789abcdef";
    int32_t shift_شمار;
    برای  (shift_شمار=(charsToOutput-1)*4; shift_شمار >= 0; shift_شمار-=4) {
        نویسه c = gHexChars[(val >> shift_شمار) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ایستا تهی outputPtrBytes(تهی *val, نویسه *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    نویسه* p = reinterpret_cast<نویسه*>(&val); /* point to current byte to output in the ptr val  */

#اگر !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += اندازه(تهی *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    برای (i=0; i<اندازه(تهی *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

ایستا تهی outputString(ثابت نویسه *s, نویسه *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    نویسه    c;
    اگر (s==nullptr) {
        s = "*NULL*";
    }
    انجامبده {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } وقتی (c != 0);
}
        


ایستا تهی outputUString(ثابت char16_t *s, int32_t len,
                          نویسه *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    اگر (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        بازگشت;
    }

    برای (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        اگر (len == -1 && c==0) {
            بشکن;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(نویسه *outBuf, int32_t capacity, int32_t indent, ثابت نویسه *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    نویسه      fmtC;
    نویسه      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    نویسه      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    برای (;;) {
        fmtC = fmt[fmtIx++];
        اگر (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            اگر (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                بشکن;
            }
            ادامه;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        انتخاب (fmtC) {
        حالت 'c':
            /* single 8 bit char   */
            c = (نویسه)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            بشکن;

        حالت 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, نویسه *);
            outputString((ثابت نویسه *)ptrArg, outBuf, &outIx, capacity, indent);
            بشکن;

        حالت 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, نویسه *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            بشکن;

        حالت 'b':
            /*  8 bit int  */
            intArg = va_arg(args, صحیح);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            بشکن;

        حالت 'h':
            /*  16 bit int  */
            intArg = va_arg(args, صحیح);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            بشکن;

        حالت 'd':
            /*  32 bit int  */
            intArg = va_arg(args, صحیح);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            بشکن;

        حالت 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            بشکن;
            
        حالت 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, نویسه *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            بشکن;

        حالت 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            بشکن;

        حالت 'v':
            {
                /* Vector of values, e.g. %vh */
                نویسه     vectorType;
                int32_t  vectorLen;
                ثابت نویسه   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                تهی     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                اگر (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت نویسه *)va_arg(args, تهی*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (تهی **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                اگر (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } وگرنه {
                    برای (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        انتخاب (vectorType) {
                        حالت 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            بشکن;
                        حالت 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            بشکن;
                        حالت 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            بشکن;
                        حالت 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            بشکن;
                        حالت 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            بشکن;
                        حالت 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            بشکن;
                        حالت 's':
                            charsToOutput = 0;
                            outputString((ثابت نویسه *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            بشکن;

                        حالت 'S':
                            charsToOutput = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            بشکن;

                            
                        }
                        اگر (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        اگر (vectorLen == -1 && longArg == 0) {
                            بشکن;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            بشکن;


        پیشفرض:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    بازگشت outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(نویسه *outBuf, int32_t capacity,
                int32_t indent, ثابت نویسه *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    بازگشت retVal;
}


U_CAPI تهی U_EXPORT2
utrace_setFunctions(ثابت تهی *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI تهی U_EXPORT2
utrace_getFunctions(ثابت تهی **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI تهی U_EXPORT2
utrace_setLevel(int32_t level) {
    اگر (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    اگر (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    بازگشت utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    بازگشت درست;
}


ایستا ثابت نویسه * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


ایستا ثابت نویسه * ثابت
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

    
ایستا ثابت نویسه * ثابت
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


ایستا ثابت نویسه* ثابت
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ثابت نویسه * U_EXPORT2
utrace_functionName(int32_t fn_عدد) {
    اگر(UTRACE_FUNCTION_START <= fn_عدد && fn_عدد < UTRACE_FUNCTION_LIMIT) {
        بازگشت trFnName[fn_عدد];
    } وگرنه اگر(UTRACE_CONVERSION_START <= fn_عدد && fn_عدد < UTRACE_CONVERSION_LIMIT) {
        بازگشت trConvNames[fn_عدد - UTRACE_CONVERSION_START];
    } وگرنه اگر(UTRACE_COLLATION_START <= fn_عدد && fn_عدد < UTRACE_COLLATION_LIMIT){
        بازگشت trCollNames[fn_عدد - UTRACE_COLLATION_START];
    } وگرنه اگر(UTRACE_UDATA_START <= fn_عدد && fn_عدد < UTRACE_RES_DATA_LIMIT){
        بازگشت trResDataNames[fn_عدد - UTRACE_UDATA_START];
    } وگرنه {
        بازگشت "[BOGUS Trace Function Number]";
    }
}

