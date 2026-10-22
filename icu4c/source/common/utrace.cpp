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


ساكن UTrace_داخلا     *p_Trace_داخلا_Func = خالي_اشارو;
ساكن UTraceExit      *pTraceExitFunc  = خالي_اشارو;
ساكن UTrace_معلومات      *p_Trace_معلومات_Func  = خالي_اشارو;
ساكن ثابت فراغ      *gTraceContext   = خالي_اشارو;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساكن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI فراغ U_EXPORT2
utrace_داخلا(int32_t fn_عدد) {
    إذا (p_Trace_داخلا_Func != خالي_اشارو) {
        (*p_Trace_داخلا_Func)(gTraceContext, fn_عدد);
    }
}


ساكن ثابت حرف gExitFmt[]             = "Returns.";
ساكن ثابت حرف g_Exit_Fmt_قدر[]        = "Returns %d.";
ساكن ثابت حرف g_Exit_Fmt_حالت[]       = "Returns.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_قدر_حالت[]  = "Returns %d.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_Ptr_حالت[]    = "Returns %d.  Status = %p.";

U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fn_عدد, int32_t returnType, ...) {
    إذا (pTraceExitFunc != خالي_اشارو) {
        va_list     دليلون_2;
        ثابت حرف *fmt;

        بدل (returnType) {
        حالة 0:
            fmt = gExitFmt;
            اكسر;
        حالة UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_قدر;
            اكسر;
        حالة UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_حالت;
            اكسر;
        حالة UTRACE_EXITV_I32 | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_قدر_حالت;
            اكسر;
        حالة UTRACE_EXITV_PTR | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_Ptr_حالت;
            اكسر;
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(دليلون_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_عدد, fmt, دليلون_2);
        va_end(دليلون_2);
    }
}
 

 
U_CAPI فراغ U_EXPORT2 
utrace_معلومات(int32_t fn_عدد, int32_t level, ثابت حرف *fmt, ...) {
    إذا (p_Trace_معلومات_Func != خالي_اشارو) {
           va_list دليلون_2;
           va_start(دليلون_2, fmt ); 
           (*p_Trace_معلومات_Func)(gTraceContext, fn_عدد, level, fmt, دليلون_2);
           va_end(دليلون_2);
    }
}


ساكن فراغ outputChar(حرف c, حرف *ٻاهريون_Buf, int32_t *ٻاهريون_Ix, int32_t capacity, int32_t indent) {
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
    إذا (*ٻاهريون_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *ٻاهريون_Ix < capacity && ٻاهريون_Buf[(*ٻاهريون_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *ٻاهريون_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        لكل(i=0; i<indent; i++) {
            إذا (*ٻاهريون_Ix < capacity) {
                ٻاهريون_Buf[*ٻاهريون_Ix] = ' ';
            }
            (*ٻاهريون_Ix)++;
        }
    }

    إذا (*ٻاهريون_Ix < capacity) {
        ٻاهريون_Buf[*ٻاهريون_Ix] = c;
    }
    إذا (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*ٻاهريون_Ix)++;
    }
}

ساكن فراغ output_Hex_بائيٽون(int64_t val, int32_t chars_منزل_Output,
                           حرف *ٻاهريون_Buf, int32_t *ٻاهريون_Ix, int32_t capacity) {
    ساكن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t سرڪايو_ڳڻپ;
    لكل  (سرڪايو_ڳڻپ=(chars_منزل_Output-1)*4; سرڪايو_ڳڻپ >= 0; سرڪايو_ڳڻپ-=4) {
        حرف c = gHexChars[(val >> سرڪايو_ڳڻپ) & 0xf];
        outputChar(c, ٻاهريون_Buf, ٻاهريون_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساكن فراغ output_Ptr_بائيٽون(فراغ *val, حرف *ٻاهريون_Buf, int32_t *ٻاهريون_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#إذا !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(فراغ *) - 1;
#جيڪڏهن_پورو

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    لكل (i=0; i<حجم(فراغ *); i++) {
        output_Hex_بائيٽون(*p, 2, ٻاهريون_Buf, ٻاهريون_Ix, capacity);
        p += incVal;
    }
}

ساكن فراغ outputString(ثابت حرف *s, حرف *ٻاهريون_Buf, int32_t *ٻاهريون_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    إذا (s==خالي_اشارو) {
        s = "*NULL*";
    }
    افعل {
        c = s[i++];
        outputChar(c, ٻاهريون_Buf, ٻاهريون_Ix, capacity, indent);
    } طالما (c != 0);
}
        


ساكن فراغ outputUString(ثابت char16_t *s, int32_t len,
                          حرف *ٻاهريون_Buf, int32_t *ٻاهريون_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    إذا (s==خالي_اشارو) {
        outputString(خالي_اشارو, ٻاهريون_Buf, ٻاهريون_Ix, capacity, indent);
        أرجع;
    }

    لكل (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_بائيٽون(c, 4, ٻاهريون_Buf, ٻاهريون_Ix, capacity);
        outputChar(' ', ٻاهريون_Buf, ٻاهريون_Ix, capacity, indent);
        إذا (len == -1 && c==0) {
            اكسر;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *ٻاهريون_Buf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list دليلون_2) {
    int32_t   ٻاهريون_Ix  = 0;
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
        إذا (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            إذا (fmtC == 0) {
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
            c = (حرف)va_arg(دليلون_2, int32_t);
            outputChar(c, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            اكسر;

        حالة 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(دليلون_2, حرف *);
            outputString((ثابت حرف *)ptrArg, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            اكسر;

        حالة 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(دليلون_2, حرف *);             /* Ptr    */
            intArg = va_arg(دليلون_2, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            اكسر;

        حالة 'b':
            /*  8 bit int  */
            intArg = va_arg(دليلون_2, صحيح);
            output_Hex_بائيٽون(intArg, 2, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
            اكسر;

        حالة 'h':
            /*  16 bit int  */
            intArg = va_arg(دليلون_2, صحيح);
            output_Hex_بائيٽون(intArg, 4, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
            اكسر;

        حالة 'd':
            /*  32 bit int  */
            intArg = va_arg(دليلون_2, صحيح);
            output_Hex_بائيٽون(intArg, 8, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
            اكسر;

        حالة 'l':
            /*  64 bit long  */
            longArg = va_arg(دليلون_2, int64_t);
            output_Hex_بائيٽون(longArg, 16, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
            اكسر;
            
        حالة 'p':
            /*  Pointers.   */
            ptrArg = va_arg(دليلون_2, حرف *);
            output_Ptr_بائيٽون(ptrArg, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
            اكسر;

        حالة 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            fmtIx--;
            اكسر;

        حالة 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     ويڪٽر_Type;
                int32_t  ويڪٽر_Len;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                فراغ     **ptrPtr;
                int32_t   chars_منزل_Output = 0;
                int32_t   i;
                
                ويڪٽر_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                إذا (ويڪٽر_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(دليلون_2, فراغ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (فراغ **)i8Ptr;
                ويڪٽر_Len = va_arg(دليلون_2, int32_t);
                إذا (ptrPtr == خالي_اشارو) {
                    outputString("*NULL* ", ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                } وإلا {
                    لكل (i=0; i<ويڪٽر_Len || ويڪٽر_Len==-1; i++) { 
                        بدل (ويڪٽر_Type) {
                        حالة 'b':
                            chars_منزل_Output = 2;
                            longArg = *i8Ptr++;
                            اكسر;
                        حالة 'h':
                            chars_منزل_Output = 4;
                            longArg = *i16Ptr++;
                            اكسر;
                        حالة 'd':
                            chars_منزل_Output = 8;
                            longArg = *i32Ptr++;
                            اكسر;
                        حالة 'l':
                            chars_منزل_Output = 16;
                            longArg = *i64Ptr++;
                            اكسر;
                        حالة 'p':
                            chars_منزل_Output = 0;
                            output_Ptr_بائيٽون(*ptrPtr, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
                            longArg = *ptrPtr==خالي_اشارو? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            اكسر;
                        حالة 'c':
                            chars_منزل_Output = 0;
                            outputChar(*i8Ptr, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            اكسر;
                        حالة 's':
                            chars_منزل_Output = 0;
                            outputString((ثابت حرف *)*ptrPtr, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                            outputChar('\n', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                            longArg = *ptrPtr==خالي_اشارو? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                        حالة 'S':
                            chars_منزل_Output = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                            outputChar('\n', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                            longArg = *ptrPtr==خالي_اشارو? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                            
                        }
                        إذا (chars_منزل_Output > 0) {
                            output_Hex_بائيٽون(longArg, chars_منزل_Output, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
                            outputChar(' ', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                        }
                        إذا (ويڪٽر_Len == -1 && longArg == 0) {
                            اكسر;
                        }
                    }
                }
                outputChar('[', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
                output_Hex_بائيٽون(ويڪٽر_Len, 8, ٻاهريون_Buf, &ٻاهريون_Ix, capacity);
                outputChar(']', ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
            }
            اكسر;


        افتراضي:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);
        }
    }
    outputChar(0, ٻاهريون_Buf, &ٻاهريون_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    أرجع ٻاهريون_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *ٻاهريون_Buf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list دليلون_2;
    va_start(دليلون_2, fmt ); 
    retVal = utrace_vformat(ٻاهريون_Buf, capacity, indent, fmt, دليلون_2);
    va_end(دليلون_2);
    أرجع retVal;
}


U_CAPI فراغ U_EXPORT2
utrace_سيٽ_Functions(ثابت فراغ *context,
                    UTrace_داخلا *e, UTraceExit *x, UTrace_معلومات *d) {
    p_Trace_داخلا_Func = e;
    pTraceExitFunc  = x;
    p_Trace_معلومات_Func  = d;
    gTraceContext   = context;
}


U_CAPI فراغ U_EXPORT2
utrace_getFunctions(ثابت فراغ **context,
                    UTrace_داخلا **e, UTraceExit **x, UTrace_معلومات **d) {
    *e = p_Trace_داخلا_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_معلومات_Func;
    *context = gTraceContext;
}

U_CAPI فراغ U_EXPORT2
utrace_سيٽ_Level(int32_t level) {
    إذا (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    إذا (level > UTRACE_VERBOSE) {
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
    p_Trace_داخلا_Func = خالي_اشارو;
    pTraceExitFunc  = خالي_اشارو;
    p_Trace_معلومات_Func  = خالي_اشارو;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = خالي_اشارو;
    أرجع سچ;
}


ساكن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    خالي_اشارو
};


ساكن ثابت حرف * ثابت
tr_Conv_نالا[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    خالي_اشارو
};

    
ساكن ثابت حرف * ثابت
tr_Coll_نالا[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    خالي_اشارو
};


ساكن ثابت حرف* ثابت
tr_Res_معلومات_نالا[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    خالي_اشارو
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fn_عدد) {
    إذا(UTRACE_FUNCTION_شروعات <= fn_عدد && fn_عدد < UTRACE_FUNCTION_حد_بندي) {
        أرجع trFnName[fn_عدد];
    } وإلا إذا(UTRACE_CONVERSION_شروعات <= fn_عدد && fn_عدد < UTRACE_CONVERSION_حد_بندي) {
        أرجع tr_Conv_نالا[fn_عدد - UTRACE_CONVERSION_شروعات];
    } وإلا إذا(UTRACE_COLLATION_شروعات <= fn_عدد && fn_عدد < UTRACE_COLLATION_حد_بندي){
        أرجع tr_Coll_نالا[fn_عدد - UTRACE_COLLATION_شروعات];
    } وإلا إذا(UTRACE_UDATA_شروعات <= fn_عدد && fn_عدد < UTRACE_RES_معلومات_حد_بندي){
        أرجع tr_Res_معلومات_نالا[fn_عدد - UTRACE_UDATA_شروعات];
    } وإلا {
        أرجع "[BOGUS Trace Function Number]";
    }
}

