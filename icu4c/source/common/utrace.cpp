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
স্থির UTraceData      *pTraceDataFunc  = nullptr;
স্থির ধ্রুব শূন্য      *gTraceContext   = nullptr;

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


স্থির ধ্রুব বর্ণ gExitFmt[]             = "Returns.";
স্থির ধ্রুব বর্ণ g_Exit_Fmt_মান[]        = "Returns %d.";
স্থির ধ্রুব বর্ণ gExitFmtStatus[]       = "Returns.  Status = %d.";
স্থির ধ্রুব বর্ণ g_Exit_Fmt_মান_Status[]  = "Returns %d.  Status = %d.";
স্থির ধ্রুব বর্ণ gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI শূন্য U_EXPORT2
utrace_exit(int32_t fn_সংখ্যা, int32_t returnType, ...) {
    যদি (pTraceExitFunc != nullptr) {
        va_list     args;
        ধ্রুব বর্ণ *fmt;

        বিকল্প (returnType) {
        পক্ষ 0:
            fmt = gExitFmt;
            ভঙ্গ;
        পক্ষ UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_মান;
            ভঙ্গ;
        পক্ষ UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            ভঙ্গ;
        পক্ষ UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_মান_Status;
            ভঙ্গ;
        পক্ষ UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            ভঙ্গ;
        স্বভাব:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_সংখ্যা, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI শূন্য U_EXPORT2 
utrace_data(int32_t fn_সংখ্যা, int32_t level, ধ্রুব বর্ণ *fmt, ...) {
    যদি (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_সংখ্যা, level, fmt, args);
           va_end(args);
    }
}


স্থির শূন্য outputChar(বর্ণ c, বর্ণ *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
        প্রতি(i=0; i<indent; i++) {
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
                           বর্ণ *outBuf, int32_t *outIx, int32_t capacity) {
    স্থির ধ্রুব বর্ণ gHexChars[] = "0123456789abcdef";
    int32_t shift_গুনতি;
    প্রতি  (shift_গুনতি=(charsToOutput-1)*4; shift_গুনতি >= 0; shift_গুনতি-=4) {
        বর্ণ c = gHexChars[(val >> shift_গুনতি) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
স্থির শূন্য outputPtrBytes(শূন্য *val, বর্ণ *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    বর্ণ* p = reinterpret_cast<বর্ণ*>(&val); /* point to current byte to output in the ptr val  */

#যদি !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += পরিমাণ(শূন্য *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    প্রতি (i=0; i<পরিমাণ(শূন্য *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

স্থির শূন্য outputString(ধ্রুব বর্ণ *s, বর্ণ *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    বর্ণ    c;
    যদি (s==nullptr) {
        s = "*NULL*";
    }
    কুরু {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } যাবত্ (c != 0);
}
        


স্থির শূন্য outputUString(ধ্রুব char16_t *s, int32_t len,
                          বর্ণ *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    যদি (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        প্রত্যাগম;
    }

    প্রতি (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        যদি (len == -1 && c==0) {
            ভঙ্গ;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(বর্ণ *outBuf, int32_t capacity, int32_t indent, ধ্রুব বর্ণ *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    বর্ণ      fmtC;
    বর্ণ      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    বর্ণ      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    প্রতি (;;) {
        fmtC = fmt[fmtIx++];
        যদি (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            যদি (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                ভঙ্গ;
            }
            অনুবর্ত;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        বিকল্প (fmtC) {
        পক্ষ 'c':
            /* single 8 bit char   */
            c = (বর্ণ)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            ভঙ্গ;

        পক্ষ 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, বর্ণ *);
            outputString((ধ্রুব বর্ণ *)ptrArg, outBuf, &outIx, capacity, indent);
            ভঙ্গ;

        পক্ষ 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, বর্ণ *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((ধ্রুব char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            ভঙ্গ;

        পক্ষ 'b':
            /*  8 bit int  */
            intArg = va_arg(args, পূর্ণাঙ্ক);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            ভঙ্গ;

        পক্ষ 'h':
            /*  16 bit int  */
            intArg = va_arg(args, পূর্ণাঙ্ক);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            ভঙ্গ;

        পক্ষ 'd':
            /*  32 bit int  */
            intArg = va_arg(args, পূর্ণাঙ্ক);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            ভঙ্গ;

        পক্ষ 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            ভঙ্গ;
            
        পক্ষ 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, বর্ণ *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            ভঙ্গ;

        পক্ষ 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            ভঙ্গ;

        পক্ষ 'v':
            {
                /* Vector of values, e.g. %vh */
                বর্ণ     vectorType;
                int32_t  vectorLen;
                ধ্রুব বর্ণ   *i8Ptr;
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
                i8Ptr = (ধ্রুব বর্ণ *)va_arg(args, শূন্য*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (শূন্য **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                যদি (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } অন্যথা {
                    প্রতি (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        বিকল্প (vectorType) {
                        পক্ষ 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            ভঙ্গ;
                        পক্ষ 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            ভঙ্গ;
                        পক্ষ 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            ভঙ্গ;
                        পক্ষ 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            ভঙ্গ;
                        পক্ষ 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            ভঙ্গ;
                        পক্ষ 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            ভঙ্গ;
                        পক্ষ 's':
                            charsToOutput = 0;
                            outputString((ধ্রুব বর্ণ *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ভঙ্গ;

                        পক্ষ 'S':
                            charsToOutput = 0;
                            outputUString((ধ্রুব char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ভঙ্গ;

                            
                        }
                        যদি (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        যদি (vectorLen == -1 && longArg == 0) {
                            ভঙ্গ;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            ভঙ্গ;


        স্বভাব:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    প্রত্যাগম outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(বর্ণ *outBuf, int32_t capacity,
                int32_t indent, ধ্রুব বর্ণ *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    প্রত্যাগম retVal;
}


U_CAPI শূন্য U_EXPORT2
utrace_setFunctions(ধ্রুব শূন্য *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI শূন্য U_EXPORT2
utrace_getFunctions(ধ্রুব শূন্য **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
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
    প্রত্যাগম utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    প্রত্যাগম সত্য;
}


স্থির ধ্রুব বর্ণ * ধ্রুব
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


স্থির ধ্রুব বর্ণ * ধ্রুব
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

    
স্থির ধ্রুব বর্ণ * ধ্রুব
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


স্থির ধ্রুব বর্ণ* ধ্রুব
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ধ্রুব বর্ণ * U_EXPORT2
utrace_functionName(int32_t fn_সংখ্যা) {
    যদি(UTRACE_FUNCTION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_FUNCTION_LIMIT) {
        প্রত্যাগম trFnName[fn_সংখ্যা];
    } অন্যথা যদি(UTRACE_CONVERSION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_CONVERSION_LIMIT) {
        প্রত্যাগম trConvNames[fn_সংখ্যা - UTRACE_CONVERSION_START];
    } অন্যথা যদি(UTRACE_COLLATION_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_COLLATION_LIMIT){
        প্রত্যাগম trCollNames[fn_সংখ্যা - UTRACE_COLLATION_START];
    } অন্যথা যদি(UTRACE_UDATA_START <= fn_সংখ্যা && fn_সংখ্যা < UTRACE_RES_DATA_LIMIT){
        প্রত্যাগম trResDataNames[fn_সংখ্যা - UTRACE_UDATA_START];
    } অন্যথা {
        প্রত্যাগম "[BOGUS Trace Function Number]";
    }
}

