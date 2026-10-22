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

#include "unicode/utrace.h"
#include "utracimp.h"
#include "cstring.h"
#include "uassert.h"
#include "ucln_cmn.h"


static UTraceEntry     *pTraceEntryFunc = nullptr;
static UTraceExit      *pTraceExitFunc  = nullptr;
static UTraceData      *pTraceDataFunc  = nullptr;
static const void      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
static int32_t
utrace_level = UTRACE_ERROR;

U_CAPI void U_EXPORT2
utrace_entry(int32_t fnNumber) {
    ᬬᬾᬦ᭄ (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fnNumber);
    }
}


static const var_na gExitFmt[]             = "Returns.";
static const var_na gExitFmtValue[]        = "Returns %d.";
static const var_na gExitFmtStatus[]       = "Returns.  Status = %d.";
static const var_na gExitFmtValueStatus[]  = "Returns %d.  Status = %d.";
static const var_na gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI void U_EXPORT2
utrace_exit(int32_t fnNumber, int32_t returnType, ...) {
    ᬬᬾᬦ᭄ (pTraceExitFunc != nullptr) {
        va_list     args;
        const var_na *fmt;

        switch (returnType) {
        pak_sa 0:
            fmt = gExitFmt;
            bha_nga;
        pak_sa UTRACE_EXITV_I32:
            fmt = gExitFmtValue;
            bha_nga;
        pak_sa UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            bha_nga;
        pak_sa UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = gExitFmtValueStatus;
            bha_nga;
        pak_sa UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            bha_nga;
        default:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fnNumber, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI void U_EXPORT2 
utrace_data(int32_t fnNumber, int32_t level, const var_na *fmt, ...) {
    ᬬᬾᬦ᭄ (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fnNumber, level, fmt, args);
           va_end(args);
    }
}


static void outputChar(var_na c, var_na *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    ᬬᬾᬦ᭄ (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        for(i=0; i<indent; i++) {
            ᬬᬾᬦ᭄ (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    ᬬᬾᬦ᭄ (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    ᬬᬾᬦ᭄ (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

static void outputHexBytes(int64_t val, int32_t charsToOutput,
                           var_na *outBuf, int32_t *outIx, int32_t capacity) {
    static const var_na gHexChars[] = "0123456789abcdef";
    int32_t shiftCount;
    for  (shiftCount=(charsToOutput-1)*4; shiftCount >= 0; shiftCount-=4) {
        var_na c = gHexChars[(val >> shiftCount) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
static void outputPtrBytes(void *val, var_na *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    var_na* p = reinterpret_cast<var_na*>(&val); /* point to current byte to output in the ptr val  */

#ᬬᬾᬦ᭄ !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += parimaa_na(void *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    for (i=0; i<parimaa_na(void *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

static void outputString(const var_na *s, var_na *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    var_na    c;
    ᬬᬾᬦ᭄ (s==nullptr) {
        s = "*NULL*";
    }
    do {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } ᬲᭂᬤᭂᬮ (c != 0);
}
        


static void outputUString(const char16_t *s, int32_t len,
                          var_na *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    ᬬᬾᬦ᭄ (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        ᬯᬮᬶ;
    }

    for (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        ᬬᬾᬦ᭄ (len == -1 && c==0) {
            bha_nga;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(var_na *outBuf, int32_t capacity, int32_t indent, const var_na *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    var_na      fmtC;
    var_na      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    var_na      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    for (;;) {
        fmtC = fmt[fmtIx++];
        ᬬᬾᬦ᭄ (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            ᬬᬾᬦ᭄ (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                bha_nga;
            }
            continue;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        switch (fmtC) {
        pak_sa 'c':
            /* single 8 bit char   */
            c = (var_na)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            bha_nga;

        pak_sa 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, var_na *);
            outputString((const var_na *)ptrArg, outBuf, &outIx, capacity, indent);
            bha_nga;

        pak_sa 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, var_na *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((const char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            bha_nga;

        pak_sa 'b':
            /*  8 bit int  */
            intArg = va_arg(args, puur_naa_nka);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            bha_nga;

        pak_sa 'h':
            /*  16 bit int  */
            intArg = va_arg(args, puur_naa_nka);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            bha_nga;

        pak_sa 'd':
            /*  32 bit int  */
            intArg = va_arg(args, puur_naa_nka);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            bha_nga;

        pak_sa 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            bha_nga;
            
        pak_sa 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, var_na *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            bha_nga;

        pak_sa 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            bha_nga;

        pak_sa 'v':
            {
                /* Vector of values, e.g. %vh */
                var_na     vectorType;
                int32_t  vectorLen;
                const var_na   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                void     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                ᬬᬾᬦ᭄ (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (const var_na *)va_arg(args, void*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (void **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                ᬬᬾᬦ᭄ (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } ᬮᬶᬬ {
                    for (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        switch (vectorType) {
                        pak_sa 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            bha_nga;
                        pak_sa 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            bha_nga;
                        pak_sa 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            bha_nga;
                        pak_sa 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            bha_nga;
                        pak_sa 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            bha_nga;
                        pak_sa 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            bha_nga;
                        pak_sa 's':
                            charsToOutput = 0;
                            outputString((const var_na *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            bha_nga;

                        pak_sa 'S':
                            charsToOutput = 0;
                            outputUString((const char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            bha_nga;

                            
                        }
                        ᬬᬾᬦ᭄ (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        ᬬᬾᬦ᭄ (vectorLen == -1 && longArg == 0) {
                            bha_nga;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            bha_nga;


        default:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    ᬯᬮᬶ outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(var_na *outBuf, int32_t capacity,
                int32_t indent, const var_na *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    ᬯᬮᬶ retVal;
}


U_CAPI void U_EXPORT2
utrace_setFunctions(const void *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI void U_EXPORT2
utrace_getFunctions(const void **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI void U_EXPORT2
utrace_setLevel(int32_t level) {
    ᬬᬾᬦ᭄ (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    ᬬᬾᬦ᭄ (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    ᬯᬮᬶ utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    ᬯᬮᬶ true;
}


static const var_na * const
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


static const var_na * const
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

    
static const var_na * const
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


static const var_na* const
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI const var_na * U_EXPORT2
utrace_functionName(int32_t fnNumber) {
    ᬬᬾᬦ᭄(UTRACE_FUNCTION_START <= fnNumber && fnNumber < UTRACE_FUNCTION_LIMIT) {
        ᬯᬮᬶ trFnName[fnNumber];
    } ᬮᬶᬬ ᬬᬾᬦ᭄(UTRACE_CONVERSION_START <= fnNumber && fnNumber < UTRACE_CONVERSION_LIMIT) {
        ᬯᬮᬶ trConvNames[fnNumber - UTRACE_CONVERSION_START];
    } ᬮᬶᬬ ᬬᬾᬦ᭄(UTRACE_COLLATION_START <= fnNumber && fnNumber < UTRACE_COLLATION_LIMIT){
        ᬯᬮᬶ trCollNames[fnNumber - UTRACE_COLLATION_START];
    } ᬮᬶᬬ ᬬᬾᬦ᭄(UTRACE_UDATA_START <= fnNumber && fnNumber < UTRACE_RES_DATA_LIMIT){
        ᬯᬮᬶ trResDataNames[fnNumber - UTRACE_UDATA_START];
    } ᬮᬶᬬ {
        ᬯᬮᬶ "[BOGUS Trace Function Number]";
    }
}

