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

#অন্তর্ভাব "unicode/utrace.h"
#অন্তর্ভাব "utracimp.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "uassert.h"
#অন্তর্ভাব "ucln_cmn.h"


স্থির UTraceEntry     *pTraceEntryFunc = nullptr;
স্থির UTraceExit      *pTraceExitFunc  = nullptr;
স্থির UTrace_তথ্য      *p_Trace_তথ্য_Func  = nullptr;
স্থির ধ্রুবক শূন্য      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
স্থির int32_t
utrace_level = UTRACE_ERROR;

U_CAPI শূন্য U_EXPORT2
utrace_entry(int32_t fn_সংখ্যা) {
    যদি (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_সংখ্যা);
    }
}


স্থির ধ্রুবক অক্ষর gExitFmt[]             = "Returns.";
স্থির ধ্রুবক অক্ষর g_Exit_Fmt_মান[]        = "Returns %d.";
স্থির ধ্রুবক অক্ষর gExitFmtStatus[]       = "Returns.  Status = %d.";
স্থির ধ্রুবক অক্ষর g_Exit_Fmt_মান_Status[]  = "Returns %d.  Status = %d.";
স্থির ধ্রুবক অক্ষর gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI শূন্য U_EXPORT2
utrace_exit(int32_t fn_সংখ্যা, int32_t returnType, ...) {
    যদি (pTraceExitFunc != nullptr) {
        va_list     args;
        ধ্রুবক অক্ষর *fmt;

        নির্বাচন (returnType) {
        ক্ষেত্র 0:
            fmt = gExitFmt;
            বিরতি;
        ক্ষেত্র UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_মান;
            বিরতি;
        ক্ষেত্র UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            বিরতি;
        ক্ষেত্র UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_মান_Status;
            বিরতি;
        ক্ষেত্র UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            বিরতি;
        পূর্বনির্ধারিত:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_সংখ্যা, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI শূন্য U_EXPORT2 
utrace_তথ্য(int32_t fn_সংখ্যা, int32_t level, ধ্রুবক অক্ষর *fmt, ...) {
    যদি (p_Trace_তথ্য_Func != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*p_Trace_তথ্য_Func)(gTraceContext, fn_সংখ্যা, level, fmt, args);
           va_end(args);
    }
}


স্থির শূন্য outputChar(অক্ষর c, অক্ষর *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    যদি (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        জন্য(i=0; i<indent; i++) {
            যদি (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    যদি (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    যদি (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

স্থির শূন্য outputHexBytes(int64_t val, int32_t charsToOutput,
                           অক্ষর *outBuf, int32_t *outIx, int32_t capacity) {
    স্থির ধ্রুবক অক্ষর gHexChars[] = "0123456789abcdef";
    int32_t shift_গুনতি;
    জন্য  (shift_গুনতি=(charsToOutput-1)*4; shift_গুনতি >= 0; shift_গুনতি-=4) {
        অক্ষর c = gHexChars[(val >> shift_গুনতি) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
স্থির শূন্য outputPtrBytes(শূন্য *val, অক্ষর *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    অক্ষর* p = reinterpret_cast<অক্ষর*>(&val); /* point to current byte to output in the ptr val  */

#যদি !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += আকার(শূন্য *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    জন্য (i=0; i<আকার(শূন্য *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

স্থির শূন্য outputString(ধ্রুবক অক্ষর *s, অক্ষর *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    অক্ষর    c;
    যদি (s==nullptr) {
        s = "*NULL*";
    }
    করো {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } যখন (c != 0);
}
        


স্থির শূন্য outputUString(ধ্রুবক char16_t *s, int32_t len,
                          অক্ষর *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    যদি (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        ফেরত;
    }

    জন্য (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        যদি (len == -1 && c==0) {
            বিরতি;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(অক্ষর *outBuf, int32_t capacity, int32_t indent, ধ্রুবক অক্ষর *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    অক্ষর      fmtC;
    অক্ষর      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    অক্ষর      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    জন্য (;;) {
        fmtC = fmt[fmtIx++];
        যদি (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            যদি (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                বিরতি;
            }
            চালু;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        নির্বাচন (fmtC) {
        ক্ষেত্র 'c':
            /* single 8 bit char   */
            c = (অক্ষর)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            বিরতি;

        ক্ষেত্র 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, অক্ষর *);
            outputString((ধ্রুবক অক্ষর *)ptrArg, outBuf, &outIx, capacity, indent);
            বিরতি;

        ক্ষেত্র 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, অক্ষর *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((ধ্রুবক char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            বিরতি;

        ক্ষেত্র 'b':
            /*  8 bit int  */
            intArg = va_arg(args, পূর্ণসংখ্যা);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            বিরতি;

        ক্ষেত্র 'h':
            /*  16 bit int  */
            intArg = va_arg(args, পূর্ণসংখ্যা);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            বিরতি;

        ক্ষেত্র 'd':
            /*  32 bit int  */
            intArg = va_arg(args, পূর্ণসংখ্যা);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            বিরতি;

        ক্ষেত্র 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            বিরতি;
            
        ক্ষেত্র 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, অক্ষর *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            বিরতি;

        ক্ষেত্র 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            বিরতি;

        ক্ষেত্র 'v':
            {
                /* Vector of values, e.g. %vh */
                অক্ষর     vectorType;
                int32_t  vectorLen;
                ধ্রুবক অক্ষর   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                শূন্য     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                যদি (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (ধ্রুবক অক্ষর *)va_arg(args, শূন্য*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (শূন্য **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                যদি (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } অন্যথা {
                    জন্য (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        নির্বাচন (vectorType) {
                        ক্ষেত্র 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            বিরতি;
                        ক্ষেত্র 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            বিরতি;
                        ক্ষেত্র 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            বিরতি;
                        ক্ষেত্র 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            বিরতি;
                        ক্ষেত্র 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            বিরতি;
                        ক্ষেত্র 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            বিরতি;
                        ক্ষেত্র 's':
                            charsToOutput = 0;
                            outputString((ধ্রুবক অক্ষর *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            বিরতি;

                        ক্ষেত্র 'S':
                            charsToOutput = 0;
                            outputUString((ধ্রুবক char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            বিরতি;

                            
                        }
                        যদি (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        যদি (vectorLen == -1 && longArg == 0) {
                            বিরতি;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            বিরতি;


        পূর্বনির্ধারিত:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    ফেরত outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(অক্ষর *outBuf, int32_t capacity,
                int32_t indent, ধ্রুবক অক্ষর *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    ফেরত retVal;
}


U_CAPI শূন্য U_EXPORT2
utrace_setFunctions(ধ্রুবক শূন্য *context,
                    UTraceEntry *e, UTraceExit *x, UTrace_তথ্য *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    p_Trace_তথ্য_Func  = d;
    gTraceContext   = context;
}


U_CAPI শূন্য U_EXPORT2
utrace_getFunctions(ধ্রুবক শূন্য **context,
                    UTraceEntry **e, UTraceExit **x, UTrace_তথ্য **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = p_Trace_তথ্য_Func;
    *context = gTraceContext;
}

U_CAPI শূন্য U_EXPORT2
utrace_setLevel(int32_t level) {
    যদি (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    যদি (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    ফেরত utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    p_Trace_তথ্য_Func  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    ফেরত সত্য;
}


স্থির ধ্রুবক অক্ষর * ধ্রুবক
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


স্থির ধ্রুবক অক্ষর * ধ্রুবক
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

    
স্থির ধ্রুবক অক্ষর * ধ্রুবক
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


স্থির ধ্রুবক অক্ষর* ধ্রুবক
tr_Res_তথ্য_Names[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ধ্রুবক অক্ষর * U_EXPORT2
utrace_functionName(int32_t fn_সংখ্যা) {
    যদি(UTRACE_FUNCTION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_FUNCTION_LIMIT) {
        ফেরত trFnName[fn_সংখ্যা];
    } অন্যথা যদি(UTRACE_CONVERSION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_CONVERSION_LIMIT) {
        ফেরত trConvNames[fn_সংখ্যা - UTRACE_CONVERSION_START];
    } অন্যথা যদি(UTRACE_COLLATION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_COLLATION_LIMIT){
        ফেরত trCollNames[fn_সংখ্যা - UTRACE_COLLATION_START];
    } অন্যথা যদি(UTRACE_UDATA_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_RES_তথ্য_LIMIT){
        ফেরত tr_Res_তথ্য_Names[fn_সংখ্যা - UTRACE_UDATA_START];
    } অন্যথা {
        ফেরত "[BOGUS Trace Function Number]";
    }
}

