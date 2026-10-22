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


ساكن UTrace_اندراج     *p_Trace_اندراج_Func = خالی_سوچک;
ساكن UTraceExit      *pTraceExitFunc  = خالی_سوچک;
ساكن UTrace_انکڑے      *p_Trace_انکڑے_Func  = خالی_سوچک;
ساكن ثابت فراغ      *gTraceContext   = خالی_سوچک;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساكن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI فراغ U_EXPORT2
utrace_اندراج(int32_t fn_نمبر) {
    إذا (p_Trace_اندراج_Func != خالی_سوچک) {
        (*p_Trace_اندراج_Func)(gTraceContext, fn_نمبر);
    }
}


ساكن ثابت حرف gExitFmt[]             = "Returns.";
ساكن ثابت حرف g_Exit_Fmt_قدر[]        = "Returns %d.";
ساكن ثابت حرف g_Exit_Fmt_حالت[]       = "Returns.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_قدر_حالت[]  = "Returns %d.  Status = %d.";
ساكن ثابت حرف g_Exit_Fmt_Ptr_حالت[]    = "Returns %d.  Status = %p.";

U_CAPI فراغ U_EXPORT2
utrace_exit(int32_t fn_نمبر, int32_t returnType, ...) {
    إذا (pTraceExitFunc != خالی_سوچک) {
        va_list     دلیلاں_2;
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

        va_start(دلیلاں_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_نمبر, fmt, دلیلاں_2);
        va_end(دلیلاں_2);
    }
}
 

 
U_CAPI فراغ U_EXPORT2 
utrace_انکڑے(int32_t fn_نمبر, int32_t level, ثابت حرف *fmt, ...) {
    إذا (p_Trace_انکڑے_Func != خالی_سوچک) {
           va_list دلیلاں_2;
           va_start(دلیلاں_2, fmt ); 
           (*p_Trace_انکڑے_Func)(gTraceContext, fn_نمبر, level, fmt, دلیلاں_2);
           va_end(دلیلاں_2);
    }
}


ساكن فراغ outputChar(حرف c, حرف *باہرلا_Buf, int32_t *باہرلا_Ix, int32_t capacity, int32_t indent) {
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
    إذا (*باہرلا_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *باہرلا_Ix < capacity && باہرلا_Buf[(*باہرلا_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *باہرلا_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        لكل(i=0; i<indent; i++) {
            إذا (*باہرلا_Ix < capacity) {
                باہرلا_Buf[*باہرلا_Ix] = ' ';
            }
            (*باہرلا_Ix)++;
        }
    }

    إذا (*باہرلا_Ix < capacity) {
        باہرلا_Buf[*باہرلا_Ix] = c;
    }
    إذا (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*باہرلا_Ix)++;
    }
}

ساكن فراغ output_Hex_بائٹاں(int64_t val, int32_t chars_منزل_Output,
                           حرف *باہرلا_Buf, int32_t *باہرلا_Ix, int32_t capacity) {
    ساكن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t کھسکاؤ_گنتی;
    لكل  (کھسکاؤ_گنتی=(chars_منزل_Output-1)*4; کھسکاؤ_گنتی >= 0; کھسکاؤ_گنتی-=4) {
        حرف c = gHexChars[(val >> کھسکاؤ_گنتی) & 0xf];
        outputChar(c, باہرلا_Buf, باہرلا_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساكن فراغ output_Ptr_بائٹاں(فراغ *val, حرف *باہرلا_Buf, int32_t *باہرلا_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#إذا !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(فراغ *) - 1;
#جے_انت

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    لكل (i=0; i<حجم(فراغ *); i++) {
        output_Hex_بائٹاں(*p, 2, باہرلا_Buf, باہرلا_Ix, capacity);
        p += incVal;
    }
}

ساكن فراغ outputString(ثابت حرف *s, حرف *باہرلا_Buf, int32_t *باہرلا_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    إذا (s==خالی_سوچک) {
        s = "*NULL*";
    }
    افعل {
        c = s[i++];
        outputChar(c, باہرلا_Buf, باہرلا_Ix, capacity, indent);
    } طالما (c != 0);
}
        


ساكن فراغ outputUString(ثابت char16_t *s, int32_t len,
                          حرف *باہرلا_Buf, int32_t *باہرلا_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    إذا (s==خالی_سوچک) {
        outputString(خالی_سوچک, باہرلا_Buf, باہرلا_Ix, capacity, indent);
        أرجع;
    }

    لكل (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_بائٹاں(c, 4, باہرلا_Buf, باہرلا_Ix, capacity);
        outputChar(' ', باہرلا_Buf, باہرلا_Ix, capacity, indent);
        إذا (len == -1 && c==0) {
            اكسر;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *باہرلا_Buf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list دلیلاں_2) {
    int32_t   باہرلا_Ix  = 0;
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
            outputChar(fmtC, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
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
            c = (حرف)va_arg(دلیلاں_2, int32_t);
            outputChar(c, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
            اكسر;

        حالة 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(دلیلاں_2, حرف *);
            outputString((ثابت حرف *)ptrArg, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
            اكسر;

        حالة 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(دلیلاں_2, حرف *);             /* Ptr    */
            intArg = va_arg(دلیلاں_2, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
            اكسر;

        حالة 'b':
            /*  8 bit int  */
            intArg = va_arg(دلیلاں_2, صحيح);
            output_Hex_بائٹاں(intArg, 2, باہرلا_Buf, &باہرلا_Ix, capacity);
            اكسر;

        حالة 'h':
            /*  16 bit int  */
            intArg = va_arg(دلیلاں_2, صحيح);
            output_Hex_بائٹاں(intArg, 4, باہرلا_Buf, &باہرلا_Ix, capacity);
            اكسر;

        حالة 'd':
            /*  32 bit int  */
            intArg = va_arg(دلیلاں_2, صحيح);
            output_Hex_بائٹاں(intArg, 8, باہرلا_Buf, &باہرلا_Ix, capacity);
            اكسر;

        حالة 'l':
            /*  64 bit long  */
            longArg = va_arg(دلیلاں_2, int64_t);
            output_Hex_بائٹاں(longArg, 16, باہرلا_Buf, &باہرلا_Ix, capacity);
            اكسر;
            
        حالة 'p':
            /*  Pointers.   */
            ptrArg = va_arg(دلیلاں_2, حرف *);
            output_Ptr_بائٹاں(ptrArg, باہرلا_Buf, &باہرلا_Ix, capacity);
            اكسر;

        حالة 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
            fmtIx--;
            اكسر;

        حالة 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     سمتیہ_Type;
                int32_t  سمتیہ_Len;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                فراغ     **ptrPtr;
                int32_t   chars_منزل_Output = 0;
                int32_t   i;
                
                سمتیہ_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                إذا (سمتیہ_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(دلیلاں_2, فراغ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (فراغ **)i8Ptr;
                سمتیہ_Len = va_arg(دلیلاں_2, int32_t);
                إذا (ptrPtr == خالی_سوچک) {
                    outputString("*NULL* ", باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                } وإلا {
                    لكل (i=0; i<سمتیہ_Len || سمتیہ_Len==-1; i++) { 
                        بدل (سمتیہ_Type) {
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
                            output_Ptr_بائٹاں(*ptrPtr, باہرلا_Buf, &باہرلا_Ix, capacity);
                            longArg = *ptrPtr==خالی_سوچک? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            اكسر;
                        حالة 'c':
                            chars_منزل_Output = 0;
                            outputChar(*i8Ptr, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            اكسر;
                        حالة 's':
                            chars_منزل_Output = 0;
                            outputString((ثابت حرف *)*ptrPtr, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                            outputChar('\n', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                            longArg = *ptrPtr==خالی_سوچک? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                        حالة 'S':
                            chars_منزل_Output = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                            outputChar('\n', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                            longArg = *ptrPtr==خالی_سوچک? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            اكسر;

                            
                        }
                        إذا (chars_منزل_Output > 0) {
                            output_Hex_بائٹاں(longArg, chars_منزل_Output, باہرلا_Buf, &باہرلا_Ix, capacity);
                            outputChar(' ', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                        }
                        إذا (سمتیہ_Len == -1 && longArg == 0) {
                            اكسر;
                        }
                    }
                }
                outputChar('[', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
                output_Hex_بائٹاں(سمتیہ_Len, 8, باہرلا_Buf, &باہرلا_Ix, capacity);
                outputChar(']', باہرلا_Buf, &باہرلا_Ix, capacity, indent);
            }
            اكسر;


        افتراضي:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, باہرلا_Buf, &باہرلا_Ix, capacity, indent);
        }
    }
    outputChar(0, باہرلا_Buf, &باہرلا_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    أرجع باہرلا_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *باہرلا_Buf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list دلیلاں_2;
    va_start(دلیلاں_2, fmt ); 
    retVal = utrace_vformat(باہرلا_Buf, capacity, indent, fmt, دلیلاں_2);
    va_end(دلیلاں_2);
    أرجع retVal;
}


U_CAPI فراغ U_EXPORT2
utrace_سیٹ_Functions(ثابت فراغ *context,
                    UTrace_اندراج *e, UTraceExit *x, UTrace_انکڑے *d) {
    p_Trace_اندراج_Func = e;
    pTraceExitFunc  = x;
    p_Trace_انکڑے_Func  = d;
    gTraceContext   = context;
}


U_CAPI فراغ U_EXPORT2
utrace_getFunctions(ثابت فراغ **context,
                    UTrace_اندراج **e, UTraceExit **x, UTrace_انکڑے **d) {
    *e = p_Trace_اندراج_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_انکڑے_Func;
    *context = gTraceContext;
}

U_CAPI فراغ U_EXPORT2
utrace_سیٹ_Level(int32_t level) {
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
    p_Trace_اندراج_Func = خالی_سوچک;
    pTraceExitFunc  = خالی_سوچک;
    p_Trace_انکڑے_Func  = خالی_سوچک;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = خالی_سوچک;
    أرجع سچ;
}


ساكن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    خالی_سوچک
};


ساكن ثابت حرف * ثابت
tr_Conv_ناں[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    خالی_سوچک
};

    
ساكن ثابت حرف * ثابت
tr_Coll_ناں[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    خالی_سوچک
};


ساكن ثابت حرف* ثابت
tr_Res_انکڑے_ناں[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    خالی_سوچک
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fn_نمبر) {
    إذا(UTRACE_FUNCTION_شروع <= fn_نمبر && fn_نمبر < UTRACE_FUNCTION_حد_بندی) {
        أرجع trFnName[fn_نمبر];
    } وإلا إذا(UTRACE_CONVERSION_شروع <= fn_نمبر && fn_نمبر < UTRACE_CONVERSION_حد_بندی) {
        أرجع tr_Conv_ناں[fn_نمبر - UTRACE_CONVERSION_شروع];
    } وإلا إذا(UTRACE_COLLATION_شروع <= fn_نمبر && fn_نمبر < UTRACE_COLLATION_حد_بندی){
        أرجع tr_Coll_ناں[fn_نمبر - UTRACE_COLLATION_شروع];
    } وإلا إذا(UTRACE_UDATA_شروع <= fn_نمبر && fn_نمبر < UTRACE_RES_انکڑے_حد_بندی){
        أرجع tr_Res_انکڑے_ناں[fn_نمبر - UTRACE_UDATA_شروع];
    } وإلا {
        أرجع "[BOGUS Trace Function Number]";
    }
}

