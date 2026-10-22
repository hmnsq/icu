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

#ضمن "unicode/utrace.h"
#ضمن "utracimp.h"
#ضمن "cstring.h"
#ضمن "uassert.h"
#ضمن "ucln_cmn.h"


ساكن UTraceEntry     *pTraceEntryFunc = nullptr;
ساكن UTraceExit      *pTraceExitFunc  = nullptr;
ساكن UTraceData      *pTraceDataFunc  = nullptr;
ساكن ثابت فراغ      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساكن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI فراغ U_EXPORT2
utrace_entry(int32_t fnNumber) {
    𐡄𐡍 (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fnNumber);
    }
}


ساكن ثابت حرف gExitFmt[]             = "Returns.";
ساكن ثابت حرف gExitFmtValue[]        = "Returns %d.";
ساكن ثابت حرف gExitFmtStatus[]       = "Returns.  Status = %d.";
ساكن ثابت حرف gExitFmtValueStatus[]  = "Returns %d.  Status = %d.";
ساكن ثابت حرف gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fnNumber, int32_t returnType, ...) {
    𐡄𐡍 (pTraceExitFunc != nullptr) {
        va_list     args;
        ثابت حرف *fmt;

        بدل (returnType) {
        حالة 0:
            fmt = gExitFmt;
            اكسر;
        حالة UTRACE_EXITV_I32:
            fmt = gExitFmtValue;
            اكسر;
        حالة UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            اكسر;
        حالة UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = gExitFmtValueStatus;
            اكسر;
        حالة UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            اكسر;
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fnNumber, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI فراغ U_EXPORT2 
utrace_data(int32_t fnNumber, int32_t level, ثابت حرف *fmt, ...) {
    𐡄𐡍 (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fnNumber, level, fmt, args);
           va_end(args);
    }
}


ساكن فراغ outputChar(حرف c, حرف *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    𐡄𐡍 (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        لكل(i=0; i<indent; i++) {
            𐡄𐡍 (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    𐡄𐡍 (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    𐡄𐡍 (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

ساكن فراغ outputHexBytes(int64_t val, int32_t charsToOutput,
                           حرف *outBuf, int32_t *outIx, int32_t capacity) {
    ساكن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t shiftCount;
    لكل  (shiftCount=(charsToOutput-1)*4; shiftCount >= 0; shiftCount-=4) {
        حرف c = gHexChars[(val >> shiftCount) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساكن فراغ outputPtrBytes(فراغ *val, حرف *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#𐡄𐡍 !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(فراغ *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    لكل (i=0; i<حجم(فراغ *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

ساكن فراغ outputString(ثابت حرف *s, حرف *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    𐡄𐡍 (s==nullptr) {
        s = "*NULL*";
    }
    𐡏𐡁𐡃 {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } طالما (c != 0);
}
        


ساكن فراغ outputUString(ثابت char16_t *s, int32_t len,
                          حرف *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    𐡄𐡍 (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        أرجع;
    }

    لكل (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        𐡄𐡍 (len == -1 && c==0) {
            اكسر;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *outBuf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    حرف      fmtC;
    حرف      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    حرف      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    لكل (;;) {
        fmtC = fmt[fmtIx++];
        𐡄𐡍 (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            𐡄𐡍 (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                اكسر;
            }
            استمر;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        بدل (fmtC) {
        حالة 'c':
            /* single 8 bit char   */
            c = (حرف)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            اكسر;

        حالة 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, حرف *);
            outputString((ثابت حرف *)ptrArg, outBuf, &outIx, capacity, indent);
            اكسر;

        حالة 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, حرف *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            اكسر;

        حالة 'b':
            /*  8 bit int  */
            intArg = va_arg(args, صحيح);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            اكسر;

        حالة 'h':
            /*  16 bit int  */
            intArg = va_arg(args, صحيح);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            اكسر;

        حالة 'd':
            /*  32 bit int  */
            intArg = va_arg(args, صحيح);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            اكسر;

        حالة 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            اكسر;
            
        حالة 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, حرف *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            اكسر;

        حالة 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            اكسر;

        حالة 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     vectorType;
                int32_t  vectorLen;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                فراغ     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                𐡄𐡍 (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(args, فراغ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (فراغ **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                𐡄𐡍 (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } وإلا {
                    لكل (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        بدل (vectorType) {
                        حالة 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            اكسر;
                        حالة 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            اكسر;
                        حالة 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            اكسر;
                        حالة 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            اكسر;
                        حالة 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            اكسر;
                        حالة 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            اكسر;
                        حالة 's':
                            charsToOutput = 0;
                            outputString((ثابت حرف *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                        حالة 'S':
                            charsToOutput = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                            
                        }
                        𐡄𐡍 (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        𐡄𐡍 (vectorLen == -1 && longArg == 0) {
                            اكسر;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            اكسر;


        افتراضي:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    أرجع outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *outBuf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    أرجع retVal;
}


U_CAPI فراغ U_EXPORT2
utrace_setFunctions(ثابت فراغ *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI فراغ U_EXPORT2
utrace_getFunctions(ثابت فراغ **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI فراغ U_EXPORT2
utrace_setLevel(int32_t level) {
    𐡄𐡍 (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    𐡄𐡍 (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    أرجع utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    أرجع 𐡒𐡔𐡈;
}


ساكن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


ساكن ثابت حرف * ثابت
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

    
ساكن ثابت حرف * ثابت
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


ساكن ثابت حرف* ثابت
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fnNumber) {
    𐡄𐡍(UTRACE_FUNCTION_START <= fnNumber && fnNumber < UTRACE_FUNCTION_LIMIT) {
        أرجع trFnName[fnNumber];
    } وإلا 𐡄𐡍(UTRACE_CONVERSION_START <= fnNumber && fnNumber < UTRACE_CONVERSION_LIMIT) {
        أرجع trConvNames[fnNumber - UTRACE_CONVERSION_START];
    } وإلا 𐡄𐡍(UTRACE_COLLATION_START <= fnNumber && fnNumber < UTRACE_COLLATION_LIMIT){
        أرجع trCollNames[fnNumber - UTRACE_COLLATION_START];
    } وإلا 𐡄𐡍(UTRACE_UDATA_START <= fnNumber && fnNumber < UTRACE_RES_DATA_LIMIT){
        أرجع trResDataNames[fnNumber - UTRACE_UDATA_START];
    } وإلا {
        أرجع "[BOGUS Trace Function Number]";
    }
}

