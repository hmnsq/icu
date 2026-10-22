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


ساكن UTrace_ثبت     *p_Trace_ثبت_Func = تش_اشاره;
ساكن UTraceExit      *pTraceExitFunc  = تش_اشاره;
ساكن UTrace_معلومات      *p_Trace_معلومات_Func  = تش_اشاره;
ساكن ثابت فراغ      *gTraceContext   = تش_اشاره;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساكن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI فراغ U_EXPORT2
utrace_ثبت(int32_t fn_شمېره) {
    إذا (p_Trace_ثبت_Func != تش_اشاره) {
        (*p_Trace_ثبت_Func)(gTraceContext, fn_شمېره);
    }
}


ساكن ثابت حرف gExitFmt[]             = "Returns.";
ساكن ثابت حرف g_Exit_Fmt_ارزښت[]        = "Returns %d.";
ساكن ثابت حرف g_Exit_Fmt_حالت[]       = "Returns.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_ارزښت_حالت[]  = "Returns %d.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_Ptr_حالت[]    = "Returns %d.  Status = %p.";

U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fn_شمېره, int32_t returnType, ...) {
    إذا (pTraceExitFunc != تش_اشاره) {
        va_list     دلیلونه_2;
        ثابت حرف *fmt;

        بدل (returnType) {
        حالة 0:
            fmt = gExitFmt;
            اكسر;
        حالة UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_ارزښت;
            اكسر;
        حالة UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_حالت;
            اكسر;
        حالة UTRACE_EXITV_I32 | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_ارزښت_حالت;
            اكسر;
        حالة UTRACE_EXITV_PTR | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_Ptr_حالت;
            اكسر;
        افتراضي:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(دلیلونه_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_شمېره, fmt, دلیلونه_2);
        va_end(دلیلونه_2);
    }
}
 

 
U_CAPI فراغ U_EXPORT2 
utrace_معلومات(int32_t fn_شمېره, int32_t level, ثابت حرف *fmt, ...) {
    إذا (p_Trace_معلومات_Func != تش_اشاره) {
           va_list دلیلونه_2;
           va_start(دلیلونه_2, fmt ); 
           (*p_Trace_معلومات_Func)(gTraceContext, fn_شمېره, level, fmt, دلیلونه_2);
           va_end(دلیلونه_2);
    }
}


ساكن فراغ outputChar(حرف c, حرف *بهر_Buf, int32_t *بهر_Ix, int32_t capacity, int32_t indent) {
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
    إذا (*بهر_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *بهر_Ix < capacity && بهر_Buf[(*بهر_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *بهر_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        لكل(i=0; i<indent; i++) {
            إذا (*بهر_Ix < capacity) {
                بهر_Buf[*بهر_Ix] = ' ';
            }
            (*بهر_Ix)++;
        }
    }

    إذا (*بهر_Ix < capacity) {
        بهر_Buf[*بهر_Ix] = c;
    }
    إذا (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*بهر_Ix)++;
    }
}

ساكن فراغ output_Hex_بایټونه(int64_t val, int32_t chars_منزل_Output,
                           حرف *بهر_Buf, int32_t *بهر_Ix, int32_t capacity) {
    ساكن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t ښويدنه_شمېر;
    لكل  (ښويدنه_شمېر=(chars_منزل_Output-1)*4; ښويدنه_شمېر >= 0; ښويدنه_شمېر-=4) {
        حرف c = gHexChars[(val >> ښويدنه_شمېر) & 0xf];
        outputChar(c, بهر_Buf, بهر_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساكن فراغ output_Ptr_بایټونه(فراغ *val, حرف *بهر_Buf, int32_t *بهر_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#إذا !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(فراغ *) - 1;
#د_که_پای

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    لكل (i=0; i<حجم(فراغ *); i++) {
        output_Hex_بایټونه(*p, 2, بهر_Buf, بهر_Ix, capacity);
        p += incVal;
    }
}

ساكن فراغ outputString(ثابت حرف *s, حرف *بهر_Buf, int32_t *بهر_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    إذا (s==تش_اشاره) {
        s = "*NULL*";
    }
    افعل {
        c = s[i++];
        outputChar(c, بهر_Buf, بهر_Ix, capacity, indent);
    } طالما (c != 0);
}
        


ساكن فراغ outputUString(ثابت char16_t *s, int32_t len,
                          حرف *بهر_Buf, int32_t *بهر_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    إذا (s==تش_اشاره) {
        outputString(تش_اشاره, بهر_Buf, بهر_Ix, capacity, indent);
        أرجع;
    }

    لكل (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_بایټونه(c, 4, بهر_Buf, بهر_Ix, capacity);
        outputChar(' ', بهر_Buf, بهر_Ix, capacity, indent);
        إذا (len == -1 && c==0) {
            اكسر;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *بهر_Buf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list دلیلونه_2) {
    int32_t   بهر_Ix  = 0;
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
            outputChar(fmtC, بهر_Buf, &بهر_Ix, capacity, indent);
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
            c = (حرف)va_arg(دلیلونه_2, int32_t);
            outputChar(c, بهر_Buf, &بهر_Ix, capacity, indent);
            اكسر;

        حالة 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(دلیلونه_2, حرف *);
            outputString((ثابت حرف *)ptrArg, بهر_Buf, &بهر_Ix, capacity, indent);
            اكسر;

        حالة 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(دلیلونه_2, حرف *);             /* Ptr    */
            intArg = va_arg(دلیلونه_2, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, بهر_Buf, &بهر_Ix, capacity, indent);
            اكسر;

        حالة 'b':
            /*  8 bit int  */
            intArg = va_arg(دلیلونه_2, صحيح);
            output_Hex_بایټونه(intArg, 2, بهر_Buf, &بهر_Ix, capacity);
            اكسر;

        حالة 'h':
            /*  16 bit int  */
            intArg = va_arg(دلیلونه_2, صحيح);
            output_Hex_بایټونه(intArg, 4, بهر_Buf, &بهر_Ix, capacity);
            اكسر;

        حالة 'd':
            /*  32 bit int  */
            intArg = va_arg(دلیلونه_2, صحيح);
            output_Hex_بایټونه(intArg, 8, بهر_Buf, &بهر_Ix, capacity);
            اكسر;

        حالة 'l':
            /*  64 bit long  */
            longArg = va_arg(دلیلونه_2, int64_t);
            output_Hex_بایټونه(longArg, 16, بهر_Buf, &بهر_Ix, capacity);
            اكسر;
            
        حالة 'p':
            /*  Pointers.   */
            ptrArg = va_arg(دلیلونه_2, حرف *);
            output_Ptr_بایټونه(ptrArg, بهر_Buf, &بهر_Ix, capacity);
            اكسر;

        حالة 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', بهر_Buf, &بهر_Ix, capacity, indent);
            fmtIx--;
            اكسر;

        حالة 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     ویکټور_Type;
                int32_t  ویکټور_Len;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                فراغ     **ptrPtr;
                int32_t   chars_منزل_Output = 0;
                int32_t   i;
                
                ویکټور_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                إذا (ویکټور_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(دلیلونه_2, فراغ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (فراغ **)i8Ptr;
                ویکټور_Len = va_arg(دلیلونه_2, int32_t);
                إذا (ptrPtr == تش_اشاره) {
                    outputString("*NULL* ", بهر_Buf, &بهر_Ix, capacity, indent);
                } وإلا {
                    لكل (i=0; i<ویکټور_Len || ویکټور_Len==-1; i++) { 
                        بدل (ویکټور_Type) {
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
                            output_Ptr_بایټونه(*ptrPtr, بهر_Buf, &بهر_Ix, capacity);
                            longArg = *ptrPtr==تش_اشاره? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            اكسر;
                        حالة 'c':
                            chars_منزل_Output = 0;
                            outputChar(*i8Ptr, بهر_Buf, &بهر_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            اكسر;
                        حالة 's':
                            chars_منزل_Output = 0;
                            outputString((ثابت حرف *)*ptrPtr, بهر_Buf, &بهر_Ix, capacity, indent);
                            outputChar('\n', بهر_Buf, &بهر_Ix, capacity, indent);
                            longArg = *ptrPtr==تش_اشاره? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                        حالة 'S':
                            chars_منزل_Output = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, بهر_Buf, &بهر_Ix, capacity, indent);
                            outputChar('\n', بهر_Buf, &بهر_Ix, capacity, indent);
                            longArg = *ptrPtr==تش_اشاره? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                            
                        }
                        إذا (chars_منزل_Output > 0) {
                            output_Hex_بایټونه(longArg, chars_منزل_Output, بهر_Buf, &بهر_Ix, capacity);
                            outputChar(' ', بهر_Buf, &بهر_Ix, capacity, indent);
                        }
                        إذا (ویکټور_Len == -1 && longArg == 0) {
                            اكسر;
                        }
                    }
                }
                outputChar('[', بهر_Buf, &بهر_Ix, capacity, indent);
                output_Hex_بایټونه(ویکټور_Len, 8, بهر_Buf, &بهر_Ix, capacity);
                outputChar(']', بهر_Buf, &بهر_Ix, capacity, indent);
            }
            اكسر;


        افتراضي:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, بهر_Buf, &بهر_Ix, capacity, indent);
        }
    }
    outputChar(0, بهر_Buf, &بهر_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    أرجع بهر_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *بهر_Buf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list دلیلونه_2;
    va_start(دلیلونه_2, fmt ); 
    retVal = utrace_vformat(بهر_Buf, capacity, indent, fmt, دلیلونه_2);
    va_end(دلیلونه_2);
    أرجع retVal;
}


U_CAPI فراغ U_EXPORT2
utrace_سېټ_Functions(ثابت فراغ *context,
                    UTrace_ثبت *e, UTraceExit *x, UTrace_معلومات *d) {
    p_Trace_ثبت_Func = e;
    pTraceExitFunc  = x;
    p_Trace_معلومات_Func  = d;
    gTraceContext   = context;
}


U_CAPI فراغ U_EXPORT2
utrace_getFunctions(ثابت فراغ **context,
                    UTrace_ثبت **e, UTraceExit **x, UTrace_معلومات **d) {
    *e = p_Trace_ثبت_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_معلومات_Func;
    *context = gTraceContext;
}

U_CAPI فراغ U_EXPORT2
utrace_سېټ_Level(int32_t level) {
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
    p_Trace_ثبت_Func = تش_اشاره;
    pTraceExitFunc  = تش_اشاره;
    p_Trace_معلومات_Func  = تش_اشاره;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = تش_اشاره;
    أرجع ریښتیا;
}


ساكن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    تش_اشاره
};


ساكن ثابت حرف * ثابت
tr_Conv_نومونه[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    تش_اشاره
};

    
ساكن ثابت حرف * ثابت
tr_Coll_نومونه[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    تش_اشاره
};


ساكن ثابت حرف* ثابت
tr_Res_معلومات_نومونه[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    تش_اشاره
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fn_شمېره) {
    إذا(UTRACE_FUNCTION_پیل <= fn_شمېره && fn_شمېره < UTRACE_FUNCTION_حد) {
        أرجع trFnName[fn_شمېره];
    } وإلا إذا(UTRACE_CONVERSION_پیل <= fn_شمېره && fn_شمېره < UTRACE_CONVERSION_حد) {
        أرجع tr_Conv_نومونه[fn_شمېره - UTRACE_CONVERSION_پیل];
    } وإلا إذا(UTRACE_COLLATION_پیل <= fn_شمېره && fn_شمېره < UTRACE_COLLATION_حد){
        أرجع tr_Coll_نومونه[fn_شمېره - UTRACE_COLLATION_پیل];
    } وإلا إذا(UTRACE_UDATA_پیل <= fn_شمېره && fn_شمېره < UTRACE_RES_معلومات_حد){
        أرجع tr_Res_معلومات_نومونه[fn_شمېره - UTRACE_UDATA_پیل];
    } وإلا {
        أرجع "[BOGUS Trace Function Number]";
    }
}

