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


ساكن UTrace_مدخل     *p_Trace_مدخل_Func = اشاره_تهی;
ساكن UTraceExit      *pTraceExitFunc  = اشاره_تهی;
ساكن UTrace_داده      *p_Trace_داده_Func  = اشاره_تهی;
ساكن ثابت فراغ      *gTraceContext   = اشاره_تهی;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساكن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI فراغ U_EXPORT2
utrace_مدخل(int32_t fn_عدد) {
    إذا (p_Trace_مدخل_Func != اشاره_تهی) {
        (*p_Trace_مدخل_Func)(gTraceContext, fn_عدد);
    }
}


ساكن ثابت حرف gExitFmt[]             = "Returns.";
ساكن ثابت حرف g_Exit_Fmt_مقدار[]        = "Returns %d.";
ساكن ثابت حرف g_Exit_Fmt_وضعیت[]       = "Returns.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_مقدار_وضعیت[]  = "Returns %d.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_Ptr_وضعیت[]    = "Returns %d.  Status = %p.";

U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fn_عدد, int32_t returnType, ...) {
    إذا (pTraceExitFunc != اشاره_تهی) {
        va_list     آرگومانها_2;
        ثابت حرف *fmt;

        بدل (returnType) {
        حالة 0:
            fmt = gExitFmt;
            اكسر;
        حالة UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_مقدار;
            اكسر;
        حالة UTRACE_EXITV_وضعیت:
            fmt = g_Exit_Fmt_وضعیت;
            اكسر;
        حالة UTRACE_EXITV_I32 | UTRACE_EXITV_وضعیت:
            fmt = g_Exit_Fmt_مقدار_وضعیت;
            اكسر;
        حالة UTRACE_EXITV_PTR | UTRACE_EXITV_وضعیت:
            fmt = g_Exit_Fmt_Ptr_وضعیت;
            اكسر;
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(آرگومانها_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_عدد, fmt, آرگومانها_2);
        va_end(آرگومانها_2);
    }
}
 

 
U_CAPI فراغ U_EXPORT2 
utrace_داده(int32_t fn_عدد, int32_t level, ثابت حرف *fmt, ...) {
    إذا (p_Trace_داده_Func != اشاره_تهی) {
           va_list آرگومانها_2;
           va_start(آرگومانها_2, fmt ); 
           (*p_Trace_داده_Func)(gTraceContext, fn_عدد, level, fmt, آرگومانها_2);
           va_end(آرگومانها_2);
    }
}


ساكن فراغ outputChar(حرف c, حرف *بیرون_Buf, int32_t *بیرون_Ix, int32_t capacity, int32_t indent) {
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
    إذا (*بیرون_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *بیرون_Ix < capacity && بیرون_Buf[(*بیرون_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *بیرون_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        لكل(i=0; i<indent; i++) {
            إذا (*بیرون_Ix < capacity) {
                بیرون_Buf[*بیرون_Ix] = ' ';
            }
            (*بیرون_Ix)++;
        }
    }

    إذا (*بیرون_Ix < capacity) {
        بیرون_Buf[*بیرون_Ix] = c;
    }
    إذا (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*بیرون_Ix)++;
    }
}

ساكن فراغ output_Hex_بایتها(int64_t val, int32_t chars_مقصد_Output,
                           حرف *بیرون_Buf, int32_t *بیرون_Ix, int32_t capacity) {
    ساكن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t لغزش_شمار;
    لكل  (لغزش_شمار=(chars_مقصد_Output-1)*4; لغزش_شمار >= 0; لغزش_شمار-=4) {
        حرف c = gHexChars[(val >> لغزش_شمار) & 0xf];
        outputChar(c, بیرون_Buf, بیرون_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساكن فراغ output_Ptr_بایتها(فراغ *val, حرف *بیرون_Buf, int32_t *بیرون_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#إذا !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(فراغ *) - 1;
#پایان_اگر

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    لكل (i=0; i<حجم(فراغ *); i++) {
        output_Hex_بایتها(*p, 2, بیرون_Buf, بیرون_Ix, capacity);
        p += incVal;
    }
}

ساكن فراغ outputString(ثابت حرف *s, حرف *بیرون_Buf, int32_t *بیرون_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    إذا (s==اشاره_تهی) {
        s = "*NULL*";
    }
    افعل {
        c = s[i++];
        outputChar(c, بیرون_Buf, بیرون_Ix, capacity, indent);
    } طالما (c != 0);
}
        


ساكن فراغ outputUString(ثابت char16_t *s, int32_t len,
                          حرف *بیرون_Buf, int32_t *بیرون_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    إذا (s==اشاره_تهی) {
        outputString(اشاره_تهی, بیرون_Buf, بیرون_Ix, capacity, indent);
        أرجع;
    }

    لكل (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_بایتها(c, 4, بیرون_Buf, بیرون_Ix, capacity);
        outputChar(' ', بیرون_Buf, بیرون_Ix, capacity, indent);
        إذا (len == -1 && c==0) {
            اكسر;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *بیرون_Buf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list آرگومانها_2) {
    int32_t   بیرون_Ix  = 0;
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
            outputChar(fmtC, بیرون_Buf, &بیرون_Ix, capacity, indent);
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
            c = (حرف)va_arg(آرگومانها_2, int32_t);
            outputChar(c, بیرون_Buf, &بیرون_Ix, capacity, indent);
            اكسر;

        حالة 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(آرگومانها_2, حرف *);
            outputString((ثابت حرف *)ptrArg, بیرون_Buf, &بیرون_Ix, capacity, indent);
            اكسر;

        حالة 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(آرگومانها_2, حرف *);             /* Ptr    */
            intArg = va_arg(آرگومانها_2, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, بیرون_Buf, &بیرون_Ix, capacity, indent);
            اكسر;

        حالة 'b':
            /*  8 bit int  */
            intArg = va_arg(آرگومانها_2, صحيح);
            output_Hex_بایتها(intArg, 2, بیرون_Buf, &بیرون_Ix, capacity);
            اكسر;

        حالة 'h':
            /*  16 bit int  */
            intArg = va_arg(آرگومانها_2, صحيح);
            output_Hex_بایتها(intArg, 4, بیرون_Buf, &بیرون_Ix, capacity);
            اكسر;

        حالة 'd':
            /*  32 bit int  */
            intArg = va_arg(آرگومانها_2, صحيح);
            output_Hex_بایتها(intArg, 8, بیرون_Buf, &بیرون_Ix, capacity);
            اكسر;

        حالة 'l':
            /*  64 bit long  */
            longArg = va_arg(آرگومانها_2, int64_t);
            output_Hex_بایتها(longArg, 16, بیرون_Buf, &بیرون_Ix, capacity);
            اكسر;
            
        حالة 'p':
            /*  Pointers.   */
            ptrArg = va_arg(آرگومانها_2, حرف *);
            output_Ptr_بایتها(ptrArg, بیرون_Buf, &بیرون_Ix, capacity);
            اكسر;

        حالة 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', بیرون_Buf, &بیرون_Ix, capacity, indent);
            fmtIx--;
            اكسر;

        حالة 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     وکتور_Type;
                int32_t  وکتور_Len;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                فراغ     **ptrPtr;
                int32_t   chars_مقصد_Output = 0;
                int32_t   i;
                
                وکتور_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                إذا (وکتور_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(آرگومانها_2, فراغ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (فراغ **)i8Ptr;
                وکتور_Len = va_arg(آرگومانها_2, int32_t);
                إذا (ptrPtr == اشاره_تهی) {
                    outputString("*NULL* ", بیرون_Buf, &بیرون_Ix, capacity, indent);
                } وإلا {
                    لكل (i=0; i<وکتور_Len || وکتور_Len==-1; i++) { 
                        بدل (وکتور_Type) {
                        حالة 'b':
                            chars_مقصد_Output = 2;
                            longArg = *i8Ptr++;
                            اكسر;
                        حالة 'h':
                            chars_مقصد_Output = 4;
                            longArg = *i16Ptr++;
                            اكسر;
                        حالة 'd':
                            chars_مقصد_Output = 8;
                            longArg = *i32Ptr++;
                            اكسر;
                        حالة 'l':
                            chars_مقصد_Output = 16;
                            longArg = *i64Ptr++;
                            اكسر;
                        حالة 'p':
                            chars_مقصد_Output = 0;
                            output_Ptr_بایتها(*ptrPtr, بیرون_Buf, &بیرون_Ix, capacity);
                            longArg = *ptrPtr==اشاره_تهی? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            اكسر;
                        حالة 'c':
                            chars_مقصد_Output = 0;
                            outputChar(*i8Ptr, بیرون_Buf, &بیرون_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            اكسر;
                        حالة 's':
                            chars_مقصد_Output = 0;
                            outputString((ثابت حرف *)*ptrPtr, بیرون_Buf, &بیرون_Ix, capacity, indent);
                            outputChar('\n', بیرون_Buf, &بیرون_Ix, capacity, indent);
                            longArg = *ptrPtr==اشاره_تهی? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                        حالة 'S':
                            chars_مقصد_Output = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, بیرون_Buf, &بیرون_Ix, capacity, indent);
                            outputChar('\n', بیرون_Buf, &بیرون_Ix, capacity, indent);
                            longArg = *ptrPtr==اشاره_تهی? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                            
                        }
                        إذا (chars_مقصد_Output > 0) {
                            output_Hex_بایتها(longArg, chars_مقصد_Output, بیرون_Buf, &بیرون_Ix, capacity);
                            outputChar(' ', بیرون_Buf, &بیرون_Ix, capacity, indent);
                        }
                        إذا (وکتور_Len == -1 && longArg == 0) {
                            اكسر;
                        }
                    }
                }
                outputChar('[', بیرون_Buf, &بیرون_Ix, capacity, indent);
                output_Hex_بایتها(وکتور_Len, 8, بیرون_Buf, &بیرون_Ix, capacity);
                outputChar(']', بیرون_Buf, &بیرون_Ix, capacity, indent);
            }
            اكسر;


        افتراضي:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, بیرون_Buf, &بیرون_Ix, capacity, indent);
        }
    }
    outputChar(0, بیرون_Buf, &بیرون_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    أرجع بیرون_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *بیرون_Buf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list آرگومانها_2;
    va_start(آرگومانها_2, fmt ); 
    retVal = utrace_vformat(بیرون_Buf, capacity, indent, fmt, آرگومانها_2);
    va_end(آرگومانها_2);
    أرجع retVal;
}


U_CAPI فراغ U_EXPORT2
utrace_مجموعه_Functions(ثابت فراغ *context,
                    UTrace_مدخل *e, UTraceExit *x, UTrace_داده *d) {
    p_Trace_مدخل_Func = e;
    pTraceExitFunc  = x;
    p_Trace_داده_Func  = d;
    gTraceContext   = context;
}


U_CAPI فراغ U_EXPORT2
utrace_getFunctions(ثابت فراغ **context,
                    UTrace_مدخل **e, UTraceExit **x, UTrace_داده **d) {
    *e = p_Trace_مدخل_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_داده_Func;
    *context = gTraceContext;
}

U_CAPI فراغ U_EXPORT2
utrace_مجموعه_Level(int32_t level) {
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
    p_Trace_مدخل_Func = اشاره_تهی;
    pTraceExitFunc  = اشاره_تهی;
    p_Trace_داده_Func  = اشاره_تهی;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = اشاره_تهی;
    أرجع درست;
}


ساكن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    اشاره_تهی
};


ساكن ثابت حرف * ثابت
tr_Conv_نامها[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    اشاره_تهی
};

    
ساكن ثابت حرف * ثابت
tr_Coll_نامها[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    اشاره_تهی
};


ساكن ثابت حرف* ثابت
tr_Res_داده_نامها[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    اشاره_تهی
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fn_عدد) {
    إذا(UTRACE_FUNCTION_شروع <= fn_عدد && fn_عدد < UTRACE_FUNCTION_حد) {
        أرجع trFnName[fn_عدد];
    } وإلا إذا(UTRACE_CONVERSION_شروع <= fn_عدد && fn_عدد < UTRACE_CONVERSION_حد) {
        أرجع tr_Conv_نامها[fn_عدد - UTRACE_CONVERSION_شروع];
    } وإلا إذا(UTRACE_COLLATION_شروع <= fn_عدد && fn_عدد < UTRACE_COLLATION_حد){
        أرجع tr_Coll_نامها[fn_عدد - UTRACE_COLLATION_شروع];
    } وإلا إذا(UTRACE_UDATA_شروع <= fn_عدد && fn_عدد < UTRACE_RES_داده_حد){
        أرجع tr_Res_داده_نامها[fn_عدد - UTRACE_UDATA_شروع];
    } وإلا {
        أرجع "[BOGUS Trace Function Number]";
    }
}

