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

#అన్తర్భావ "unicode/utrace.h"
#అన్తర్భావ "utracimp.h"
#అన్తర్భావ "cstring.h"
#అన్తర్భావ "uassert.h"
#అన్తర్భావ "ucln_cmn.h"


స్థిర UTrace_నమోదు     *p_Trace_నమోదు_Func = శూన్యసూచిక;
స్థిర UTraceExit      *pTraceExitFunc  = శూన్యసూచిక;
స్థిర UTrace_దత్తాంశం      *p_Trace_దత్తాంశం_Func  = శూన్యసూచిక;
స్థిర స్థిరరాశి శూన్య      *gTraceContext   = శూన్యసూచిక;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
స్థిర int32_t
utrace_level = UTRACE_ERROR;

U_CAPI శూన్య U_EXPORT2
utrace_నమోదు(int32_t fn_సంఖ్య) {
    ఒకవేళ (p_Trace_నమోదు_Func != శూన్యసూచిక) {
        (*p_Trace_నమోదు_Func)(gTraceContext, fn_సంఖ్య);
    }
}


స్థిర స్థిరరాశి అక్షరం gExitFmt[]             = "Returns.";
స్థిర స్థిరరాశి అక్షరం g_Exit_Fmt_విలువ[]        = "Returns %d.";
స్థిర స్థిరరాశి అక్షరం g_Exit_Fmt_స్థితి[]       = "Returns.  Status = %d.";
స్థిర స్థిరరాశి అక్షరం g_Exit_Fmt_విలువ_స్థితి[]  = "Returns %d.  Status = %d.";
స్థిర స్థిరరాశి అక్షరం g_Exit_Fmt_Ptr_స్థితి[]    = "Returns %d.  Status = %p.";

U_CAPI శూన్య U_EXPORT2
utrace_exit(int32_t fn_సంఖ్య, int32_t returnType, ...) {
    ఒకవేళ (pTraceExitFunc != శూన్యసూచిక) {
        va_list     వాదనలు_2;
        స్థిరరాశి అక్షరం *fmt;

        ఎంపిక (returnType) {
        స్థితి 0:
            fmt = gExitFmt;
            ఆపు;
        స్థితి UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_విలువ;
            ఆపు;
        స్థితి UTRACE_EXITV_స్థితి:
            fmt = g_Exit_Fmt_స్థితి;
            ఆపు;
        స్థితి UTRACE_EXITV_I32 | UTRACE_EXITV_స్థితి:
            fmt = g_Exit_Fmt_విలువ_స్థితి;
            ఆపు;
        స్థితి UTRACE_EXITV_PTR | UTRACE_EXITV_స్థితి:
            fmt = g_Exit_Fmt_Ptr_స్థితి;
            ఆపు;
        అప్రమేయ:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(వాదనలు_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_సంఖ్య, fmt, వాదనలు_2);
        va_end(వాదనలు_2);
    }
}
 

 
U_CAPI శూన్య U_EXPORT2 
utrace_దత్తాంశం(int32_t fn_సంఖ్య, int32_t level, స్థిరరాశి అక్షరం *fmt, ...) {
    ఒకవేళ (p_Trace_దత్తాంశం_Func != శూన్యసూచిక) {
           va_list వాదనలు_2;
           va_start(వాదనలు_2, fmt ); 
           (*p_Trace_దత్తాంశం_Func)(gTraceContext, fn_సంఖ్య, level, fmt, వాదనలు_2);
           va_end(వాదనలు_2);
    }
}


స్థిర శూన్య outputChar(అక్షరం c, అక్షరం *బహిర్గతం_Buf, int32_t *బహిర్గతం_Ix, int32_t capacity, int32_t indent) {
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
    ఒకవేళ (*బహిర్గతం_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *బహిర్గతం_Ix < capacity && బహిర్గతం_Buf[(*బహిర్గతం_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *బహిర్గతం_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        కొరకు(i=0; i<indent; i++) {
            ఒకవేళ (*బహిర్గతం_Ix < capacity) {
                బహిర్గతం_Buf[*బహిర్గతం_Ix] = ' ';
            }
            (*బహిర్గతం_Ix)++;
        }
    }

    ఒకవేళ (*బహిర్గతం_Ix < capacity) {
        బహిర్గతం_Buf[*బహిర్గతం_Ix] = c;
    }
    ఒకవేళ (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*బహిర్గతం_Ix)++;
    }
}

స్థిర శూన్య output_Hex_బైట్లు(int64_t val, int32_t chars_గమ్యం_Output,
                           అక్షరం *బహిర్గతం_Buf, int32_t *బహిర్గతం_Ix, int32_t capacity) {
    స్థిర స్థిరరాశి అక్షరం gHexChars[] = "0123456789abcdef";
    int32_t జరుపు_లెక్క;
    కొరకు  (జరుపు_లెక్క=(chars_గమ్యం_Output-1)*4; జరుపు_లెక్క >= 0; జరుపు_లెక్క-=4) {
        అక్షరం c = gHexChars[(val >> జరుపు_లెక్క) & 0xf];
        outputChar(c, బహిర్గతం_Buf, బహిర్గతం_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
స్థిర శూన్య output_Ptr_బైట్లు(శూన్య *val, అక్షరం *బహిర్గతం_Buf, int32_t *బహిర్గతం_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    అక్షరం* p = reinterpret_cast<అక్షరం*>(&val); /* point to current byte to output in the ptr val  */

#ఒకవేళ !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += పరిమాణం(శూన్య *) - 1;
#అయితే_ముగింపు

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    కొరకు (i=0; i<పరిమాణం(శూన్య *); i++) {
        output_Hex_బైట్లు(*p, 2, బహిర్గతం_Buf, బహిర్గతం_Ix, capacity);
        p += incVal;
    }
}

స్థిర శూన్య outputString(స్థిరరాశి అక్షరం *s, అక్షరం *బహిర్గతం_Buf, int32_t *బహిర్గతం_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    అక్షరం    c;
    ఒకవేళ (s==శూన్యసూచిక) {
        s = "*NULL*";
    }
    చేయి {
        c = s[i++];
        outputChar(c, బహిర్గతం_Buf, బహిర్గతం_Ix, capacity, indent);
    } అయితే (c != 0);
}
        


స్థిర శూన్య outputUString(స్థిరరాశి char16_t *s, int32_t len,
                          అక్షరం *బహిర్గతం_Buf, int32_t *బహిర్గతం_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    ఒకవేళ (s==శూన్యసూచిక) {
        outputString(శూన్యసూచిక, బహిర్గతం_Buf, బహిర్గతం_Ix, capacity, indent);
        తిరిగి;
    }

    కొరకు (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_బైట్లు(c, 4, బహిర్గతం_Buf, బహిర్గతం_Ix, capacity);
        outputChar(' ', బహిర్గతం_Buf, బహిర్గతం_Ix, capacity, indent);
        ఒకవేళ (len == -1 && c==0) {
            ఆపు;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(అక్షరం *బహిర్గతం_Buf, int32_t capacity, int32_t indent, స్థిరరాశి అక్షరం *fmt, va_list వాదనలు_2) {
    int32_t   బహిర్గతం_Ix  = 0;
    int32_t   fmtIx  = 0;
    అక్షరం      fmtC;
    అక్షరం      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    అక్షరం      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    కొరకు (;;) {
        fmtC = fmt[fmtIx++];
        ఒకవేళ (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            ఒకవేళ (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                ఆపు;
            }
            కొనసాగు;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        ఎంపిక (fmtC) {
        స్థితి 'c':
            /* single 8 bit char   */
            c = (అక్షరం)va_arg(వాదనలు_2, int32_t);
            outputChar(c, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            ఆపు;

        స్థితి 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(వాదనలు_2, అక్షరం *);
            outputString((స్థిరరాశి అక్షరం *)ptrArg, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            ఆపు;

        స్థితి 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(వాదనలు_2, అక్షరం *);             /* Ptr    */
            intArg = va_arg(వాదనలు_2, int32_t);            /* Length */
            outputUString((స్థిరరాశి char16_t *)ptrArg, intArg, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            ఆపు;

        స్థితి 'b':
            /*  8 bit int  */
            intArg = va_arg(వాదనలు_2, పూర్ణాంకం);
            output_Hex_బైట్లు(intArg, 2, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
            ఆపు;

        స్థితి 'h':
            /*  16 bit int  */
            intArg = va_arg(వాదనలు_2, పూర్ణాంకం);
            output_Hex_బైట్లు(intArg, 4, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
            ఆపు;

        స్థితి 'd':
            /*  32 bit int  */
            intArg = va_arg(వాదనలు_2, పూర్ణాంకం);
            output_Hex_బైట్లు(intArg, 8, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
            ఆపు;

        స్థితి 'l':
            /*  64 bit long  */
            longArg = va_arg(వాదనలు_2, int64_t);
            output_Hex_బైట్లు(longArg, 16, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
            ఆపు;
            
        స్థితి 'p':
            /*  Pointers.   */
            ptrArg = va_arg(వాదనలు_2, అక్షరం *);
            output_Ptr_బైట్లు(ptrArg, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
            ఆపు;

        స్థితి 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            fmtIx--;
            ఆపు;

        స్థితి 'v':
            {
                /* Vector of values, e.g. %vh */
                అక్షరం     సదిశ_Type;
                int32_t  సదిశ_Len;
                స్థిరరాశి అక్షరం   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                శూన్య     **ptrPtr;
                int32_t   chars_గమ్యం_Output = 0;
                int32_t   i;
                
                సదిశ_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                ఒకవేళ (సదిశ_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (స్థిరరాశి అక్షరం *)va_arg(వాదనలు_2, శూన్య*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (శూన్య **)i8Ptr;
                సదిశ_Len = va_arg(వాదనలు_2, int32_t);
                ఒకవేళ (ptrPtr == శూన్యసూచిక) {
                    outputString("*NULL* ", బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                } లేకపోతే {
                    కొరకు (i=0; i<సదిశ_Len || సదిశ_Len==-1; i++) { 
                        ఎంపిక (సదిశ_Type) {
                        స్థితి 'b':
                            chars_గమ్యం_Output = 2;
                            longArg = *i8Ptr++;
                            ఆపు;
                        స్థితి 'h':
                            chars_గమ్యం_Output = 4;
                            longArg = *i16Ptr++;
                            ఆపు;
                        స్థితి 'd':
                            chars_గమ్యం_Output = 8;
                            longArg = *i32Ptr++;
                            ఆపు;
                        స్థితి 'l':
                            chars_గమ్యం_Output = 16;
                            longArg = *i64Ptr++;
                            ఆపు;
                        స్థితి 'p':
                            chars_గమ్యం_Output = 0;
                            output_Ptr_బైట్లు(*ptrPtr, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
                            longArg = *ptrPtr==శూన్యసూచిక? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            ఆపు;
                        స్థితి 'c':
                            chars_గమ్యం_Output = 0;
                            outputChar(*i8Ptr, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            ఆపు;
                        స్థితి 's':
                            chars_గమ్యం_Output = 0;
                            outputString((స్థిరరాశి అక్షరం *)*ptrPtr, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                            outputChar('\n', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                            longArg = *ptrPtr==శూన్యసూచిక? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ఆపు;

                        స్థితి 'S':
                            chars_గమ్యం_Output = 0;
                            outputUString((స్థిరరాశి char16_t *)*ptrPtr, -1, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                            outputChar('\n', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                            longArg = *ptrPtr==శూన్యసూచిక? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ఆపు;

                            
                        }
                        ఒకవేళ (chars_గమ్యం_Output > 0) {
                            output_Hex_బైట్లు(longArg, chars_గమ్యం_Output, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
                            outputChar(' ', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                        }
                        ఒకవేళ (సదిశ_Len == -1 && longArg == 0) {
                            ఆపు;
                        }
                    }
                }
                outputChar('[', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
                output_Hex_బైట్లు(సదిశ_Len, 8, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity);
                outputChar(']', బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
            }
            ఆపు;


        అప్రమేయ:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);
        }
    }
    outputChar(0, బహిర్గతం_Buf, &బహిర్గతం_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    తిరిగి బహిర్గతం_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(అక్షరం *బహిర్గతం_Buf, int32_t capacity,
                int32_t indent, స్థిరరాశి అక్షరం *fmt,  ...) {
    int32_t retVal;
    va_list వాదనలు_2;
    va_start(వాదనలు_2, fmt ); 
    retVal = utrace_vformat(బహిర్గతం_Buf, capacity, indent, fmt, వాదనలు_2);
    va_end(వాదనలు_2);
    తిరిగి retVal;
}


U_CAPI శూన్య U_EXPORT2
utrace_సమితి_Functions(స్థిరరాశి శూన్య *context,
                    UTrace_నమోదు *e, UTraceExit *x, UTrace_దత్తాంశం *d) {
    p_Trace_నమోదు_Func = e;
    pTraceExitFunc  = x;
    p_Trace_దత్తాంశం_Func  = d;
    gTraceContext   = context;
}


U_CAPI శూన్య U_EXPORT2
utrace_getFunctions(స్థిరరాశి శూన్య **context,
                    UTrace_నమోదు **e, UTraceExit **x, UTrace_దత్తాంశం **d) {
    *e = p_Trace_నమోదు_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_దత్తాంశం_Func;
    *context = gTraceContext;
}

U_CAPI శూన్య U_EXPORT2
utrace_సమితి_Level(int32_t level) {
    ఒకవేళ (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    ఒకవేళ (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    తిరిగి utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    p_Trace_నమోదు_Func = శూన్యసూచిక;
    pTraceExitFunc  = శూన్యసూచిక;
    p_Trace_దత్తాంశం_Func  = శూన్యసూచిక;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = శూన్యసూచిక;
    తిరిగి నిజం;
}


స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి
trFnName[] = {
    "u_init",
    "u_cleanup",
    శూన్యసూచిక
};


స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి
tr_Conv_పేర్లు[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    శూన్యసూచిక
};

    
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి
tr_Coll_పేర్లు[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    శూన్యసూచిక
};


స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి
tr_Res_దత్తాంశం_పేర్లు[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    శూన్యసూచిక
};

                
U_CAPI స్థిరరాశి అక్షరం * U_EXPORT2
utrace_functionName(int32_t fn_సంఖ్య) {
    ఒకవేళ(UTRACE_FUNCTION_ప్రారంభం <= fn_సంఖ్య && fn_సంఖ్య < UTRACE_FUNCTION_హద్దు) {
        తిరిగి trFnName[fn_సంఖ్య];
    } లేకపోతే ఒకవేళ(UTRACE_CONVERSION_ప్రారంభం <= fn_సంఖ్య && fn_సంఖ్య < UTRACE_CONVERSION_హద్దు) {
        తిరిగి tr_Conv_పేర్లు[fn_సంఖ్య - UTRACE_CONVERSION_ప్రారంభం];
    } లేకపోతే ఒకవేళ(UTRACE_COLLATION_ప్రారంభం <= fn_సంఖ్య && fn_సంఖ్య < UTRACE_COLLATION_హద్దు){
        తిరిగి tr_Coll_పేర్లు[fn_సంఖ్య - UTRACE_COLLATION_ప్రారంభం];
    } లేకపోతే ఒకవేళ(UTRACE_UDATA_ప్రారంభం <= fn_సంఖ్య && fn_సంఖ్య < UTRACE_RES_దత్తాంశం_హద్దు){
        తిరిగి tr_Res_దత్తాంశం_పేర్లు[fn_సంఖ్య - UTRACE_UDATA_ప్రారంభం];
    } లేకపోతే {
        తిరిగి "[BOGUS Trace Function Number]";
    }
}

