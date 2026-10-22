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

#અન્તર્ભાવ "unicode/utrace.h"
#અન્તર્ભાવ "utracimp.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "uassert.h"
#અન્તર્ભાવ "ucln_cmn.h"


સ્થિર UTraceEntry     *pTraceEntryFunc = nullptr;
સ્થિર UTraceExit      *pTraceExitFunc  = nullptr;
સ્થિર UTraceData      *pTraceDataFunc  = nullptr;
સ્થિર અચલ શૂન્ય      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
સ્થિર int32_t
utrace_level = UTRACE_ERROR;

U_CAPI શૂન્ય U_EXPORT2
utrace_entry(int32_t fn_સંખ્યા) {
    જો (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_સંખ્યા);
    }
}


સ્થિર અચલ અક્ષર gExitFmt[]             = "Returns.";
સ્થિર અચલ અક્ષર g_Exit_Fmt_કિંમત[]        = "Returns %d.";
સ્થિર અચલ અક્ષર gExitFmtStatus[]       = "Returns.  Status = %d.";
સ્થિર અચલ અક્ષર g_Exit_Fmt_કિંમત_Status[]  = "Returns %d.  Status = %d.";
સ્થિર અચલ અક્ષર gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI શૂન્ય U_EXPORT2
utrace_exit(int32_t fn_સંખ્યા, int32_t returnType, ...) {
    જો (pTraceExitFunc != nullptr) {
        va_list     args;
        અચલ અક્ષર *fmt;

        પસંદગી (returnType) {
        સ્થિતિ 0:
            fmt = gExitFmt;
            તોડો;
        સ્થિતિ UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_કિંમત;
            તોડો;
        સ્થિતિ UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            તોડો;
        સ્થિતિ UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_કિંમત_Status;
            તોડો;
        સ્થિતિ UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            તોડો;
        મૂળભૂત:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_સંખ્યા, fmt, args);
        va_end(args);
    }
}
 

 
U_CAPI શૂન્ય U_EXPORT2 
utrace_data(int32_t fn_સંખ્યા, int32_t level, અચલ અક્ષર *fmt, ...) {
    જો (pTraceDataFunc != nullptr) {
           va_list args;
           va_start(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_સંખ્યા, level, fmt, args);
           va_end(args);
    }
}


સ્થિર શૂન્ય outputChar(અક્ષર c, અક્ષર *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    જો (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        માટે(i=0; i<indent; i++) {
            જો (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    જો (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    જો (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

સ્થિર શૂન્ય outputHexBytes(int64_t val, int32_t charsToOutput,
                           અક્ષર *outBuf, int32_t *outIx, int32_t capacity) {
    સ્થિર અચલ અક્ષર gHexChars[] = "0123456789abcdef";
    int32_t shift_ગણતરી;
    માટે  (shift_ગણતરી=(charsToOutput-1)*4; shift_ગણતરી >= 0; shift_ગણતરી-=4) {
        અક્ષર c = gHexChars[(val >> shift_ગણતરી) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
સ્થિર શૂન્ય outputPtrBytes(શૂન્ય *val, અક્ષર *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    અક્ષર* p = reinterpret_cast<અક્ષર*>(&val); /* point to current byte to output in the ptr val  */

#જો !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += કદ(શૂન્ય *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    માટે (i=0; i<કદ(શૂન્ય *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

સ્થિર શૂન્ય outputString(અચલ અક્ષર *s, અક્ષર *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    અક્ષર    c;
    જો (s==nullptr) {
        s = "*NULL*";
    }
    કરો {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } જ્યારે (c != 0);
}
        


સ્થિર શૂન્ય outputUString(અચલ char16_t *s, int32_t len,
                          અક્ષર *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    જો (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        પાછા;
    }

    માટે (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        જો (len == -1 && c==0) {
            તોડો;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(અક્ષર *outBuf, int32_t capacity, int32_t indent, અચલ અક્ષર *fmt, va_list args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    અક્ષર      fmtC;
    અક્ષર      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    અક્ષર      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    માટે (;;) {
        fmtC = fmt[fmtIx++];
        જો (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            જો (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                તોડો;
            }
            ચાલુ;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        પસંદગી (fmtC) {
        સ્થિતિ 'c':
            /* single 8 bit char   */
            c = (અક્ષર)va_arg(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            તોડો;

        સ્થિતિ 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(args, અક્ષર *);
            outputString((અચલ અક્ષર *)ptrArg, outBuf, &outIx, capacity, indent);
            તોડો;

        સ્થિતિ 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(args, અક્ષર *);             /* Ptr    */
            intArg = va_arg(args, int32_t);            /* Length */
            outputUString((અચલ char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            તોડો;

        સ્થિતિ 'b':
            /*  8 bit int  */
            intArg = va_arg(args, પૂર્ણાંક);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            તોડો;

        સ્થિતિ 'h':
            /*  16 bit int  */
            intArg = va_arg(args, પૂર્ણાંક);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            તોડો;

        સ્થિતિ 'd':
            /*  32 bit int  */
            intArg = va_arg(args, પૂર્ણાંક);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            તોડો;

        સ્થિતિ 'l':
            /*  64 bit long  */
            longArg = va_arg(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            તોડો;
            
        સ્થિતિ 'p':
            /*  Pointers.   */
            ptrArg = va_arg(args, અક્ષર *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            તોડો;

        સ્થિતિ 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            તોડો;

        સ્થિતિ 'v':
            {
                /* Vector of values, e.g. %vh */
                અક્ષર     vectorType;
                int32_t  vectorLen;
                અચલ અક્ષર   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                શૂન્ય     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                જો (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (અચલ અક્ષર *)va_arg(args, શૂન્ય*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (શૂન્ય **)i8Ptr;
                vectorLen = va_arg(args, int32_t);
                જો (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } નહીંતર {
                    માટે (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        પસંદગી (vectorType) {
                        સ્થિતિ 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            તોડો;
                        સ્થિતિ 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            તોડો;
                        સ્થિતિ 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            તોડો;
                        સ્થિતિ 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            તોડો;
                        સ્થિતિ 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            તોડો;
                        સ્થિતિ 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            તોડો;
                        સ્થિતિ 's':
                            charsToOutput = 0;
                            outputString((અચલ અક્ષર *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            તોડો;

                        સ્થિતિ 'S':
                            charsToOutput = 0;
                            outputUString((અચલ char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            તોડો;

                            
                        }
                        જો (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        જો (vectorLen == -1 && longArg == 0) {
                            તોડો;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            તોડો;


        મૂળભૂત:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    પાછા outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(અક્ષર *outBuf, int32_t capacity,
                int32_t indent, અચલ અક્ષર *fmt,  ...) {
    int32_t retVal;
    va_list args;
    va_start(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    va_end(args);
    પાછા retVal;
}


U_CAPI શૂન્ય U_EXPORT2
utrace_setFunctions(અચલ શૂન્ય *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI શૂન્ય U_EXPORT2
utrace_getFunctions(અચલ શૂન્ય **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI શૂન્ય U_EXPORT2
utrace_setLevel(int32_t level) {
    જો (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    જો (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    પાછા utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    પાછા સાચું;
}


સ્થિર અચલ અક્ષર * અચલ
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


સ્થિર અચલ અક્ષર * અચલ
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

    
સ્થિર અચલ અક્ષર * અચલ
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


સ્થિર અચલ અક્ષર* અચલ
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI અચલ અક્ષર * U_EXPORT2
utrace_functionName(int32_t fn_સંખ્યા) {
    જો(UTRACE_FUNCTION_START <= fn_સંખ્યા && fn_સંખ્યા < UTRACE_FUNCTION_LIMIT) {
        પાછા trFnName[fn_સંખ્યા];
    } નહીંતર જો(UTRACE_CONVERSION_START <= fn_સંખ્યા && fn_સંખ્યા < UTRACE_CONVERSION_LIMIT) {
        પાછા trConvNames[fn_સંખ્યા - UTRACE_CONVERSION_START];
    } નહીંતર જો(UTRACE_COLLATION_START <= fn_સંખ્યા && fn_સંખ્યા < UTRACE_COLLATION_LIMIT){
        પાછા trCollNames[fn_સંખ્યા - UTRACE_COLLATION_START];
    } નહીંતર જો(UTRACE_UDATA_START <= fn_સંખ્યા && fn_સંખ્યા < UTRACE_RES_DATA_LIMIT){
        પાછા trResDataNames[fn_સંખ્યા - UTRACE_UDATA_START];
    } નહીંતર {
        પાછા "[BOGUS Trace Function Number]";
    }
}

