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

#شامل "unicode/utrace.h"
#شامل "utracimp.h"
#شامل "cstring.h"
#شامل "uassert.h"
#شامل "ucln_cmn.h"


ساکن UTrace_اندراج     *p_Trace_اندراج_Func = خالی_اشاریہ;
ساکن UTraceExit      *pTraceExitFunc  = خالی_اشاریہ;
ساکن UTrace_ڈیٹا      *p_Trace_ڈیٹا_Func  = خالی_اشاریہ;
ساکن ثابت خالی      *gTraceContext   = خالی_اشاریہ;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ساکن int32_t
utrace_level = UTRACE_ERROR;

U_CAPI خالی U_EXPORT2
utrace_اندراج(int32_t fn_نمبر) {
    اگر (p_Trace_اندراج_Func != خالی_اشاریہ) {
        (*p_Trace_اندراج_Func)(gTraceContext, fn_نمبر);
    }
}


ساکن ثابت حرف gExitFmt[]             = "Returns.";
ساکن ثابت حرف g_Exit_Fmt_قدر[]        = "Returns %d.";
ساکن ثابت حرف g_Exit_Fmt_حالت[]       = "Returns.  Status = %d.";
ساکن ثابت حرف g_Exit_Fmt_قدر_حالت[]  = "Returns %d.  Status = %d.";
ساکن ثابت حرف g_Exit_Fmt_Ptr_حالت[]    = "Returns %d.  Status = %p.";

U_CAPI خالی U_EXPORT2
utrace_exit(int32_t fn_نمبر, int32_t returnType, ...) {
    اگر (pTraceExitFunc != خالی_اشاریہ) {
        va_list     دلائل_2;
        ثابت حرف *fmt;

        انتخاب (returnType) {
        حالت 0:
            fmt = gExitFmt;
            توڑو;
        حالت UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_قدر;
            توڑو;
        حالت UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_حالت;
            توڑو;
        حالت UTRACE_EXITV_I32 | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_قدر_حالت;
            توڑو;
        حالت UTRACE_EXITV_PTR | UTRACE_EXITV_حالت:
            fmt = g_Exit_Fmt_Ptr_حالت;
            توڑو;
        طےشدہ:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(دلائل_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_نمبر, fmt, دلائل_2);
        va_end(دلائل_2);
    }
}
 

 
U_CAPI خالی U_EXPORT2 
utrace_ڈیٹا(int32_t fn_نمبر, int32_t level, ثابت حرف *fmt, ...) {
    اگر (p_Trace_ڈیٹا_Func != خالی_اشاریہ) {
           va_list دلائل_2;
           va_start(دلائل_2, fmt ); 
           (*p_Trace_ڈیٹا_Func)(gTraceContext, fn_نمبر, level, fmt, دلائل_2);
           va_end(دلائل_2);
    }
}


ساکن خالی outputChar(حرف c, حرف *برآمد_Buf, int32_t *برآمد_Ix, int32_t capacity, int32_t indent) {
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
    اگر (*برآمد_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *برآمد_Ix < capacity && برآمد_Buf[(*برآمد_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *برآمد_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        برائے(i=0; i<indent; i++) {
            اگر (*برآمد_Ix < capacity) {
                برآمد_Buf[*برآمد_Ix] = ' ';
            }
            (*برآمد_Ix)++;
        }
    }

    اگر (*برآمد_Ix < capacity) {
        برآمد_Buf[*برآمد_Ix] = c;
    }
    اگر (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*برآمد_Ix)++;
    }
}

ساکن خالی output_Hex_بائٹیں(int64_t val, int32_t chars_مقصد_Output,
                           حرف *برآمد_Buf, int32_t *برآمد_Ix, int32_t capacity) {
    ساکن ثابت حرف gHexChars[] = "0123456789abcdef";
    int32_t سرکاؤ_گنتی;
    برائے  (سرکاؤ_گنتی=(chars_مقصد_Output-1)*4; سرکاؤ_گنتی >= 0; سرکاؤ_گنتی-=4) {
        حرف c = gHexChars[(val >> سرکاؤ_گنتی) & 0xf];
        outputChar(c, برآمد_Buf, برآمد_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ساکن خالی output_Ptr_بائٹیں(خالی *val, حرف *برآمد_Buf, int32_t *برآمد_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    حرف* p = reinterpret_cast<حرف*>(&val); /* point to current byte to output in the ptr val  */

#اگر !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += حجم(خالی *) - 1;
#اگر_ختم

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    برائے (i=0; i<حجم(خالی *); i++) {
        output_Hex_بائٹیں(*p, 2, برآمد_Buf, برآمد_Ix, capacity);
        p += incVal;
    }
}

ساکن خالی outputString(ثابت حرف *s, حرف *برآمد_Buf, int32_t *برآمد_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    حرف    c;
    اگر (s==خالی_اشاریہ) {
        s = "*NULL*";
    }
    کرو {
        c = s[i++];
        outputChar(c, برآمد_Buf, برآمد_Ix, capacity, indent);
    } جب (c != 0);
}
        


ساکن خالی outputUString(ثابت char16_t *s, int32_t len,
                          حرف *برآمد_Buf, int32_t *برآمد_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    اگر (s==خالی_اشاریہ) {
        outputString(خالی_اشاریہ, برآمد_Buf, برآمد_Ix, capacity, indent);
        واپس;
    }

    برائے (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_بائٹیں(c, 4, برآمد_Buf, برآمد_Ix, capacity);
        outputChar(' ', برآمد_Buf, برآمد_Ix, capacity, indent);
        اگر (len == -1 && c==0) {
            توڑو;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(حرف *برآمد_Buf, int32_t capacity, int32_t indent, ثابت حرف *fmt, va_list دلائل_2) {
    int32_t   برآمد_Ix  = 0;
    int32_t   fmtIx  = 0;
    حرف      fmtC;
    حرف      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    حرف      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    برائے (;;) {
        fmtC = fmt[fmtIx++];
        اگر (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, برآمد_Buf, &برآمد_Ix, capacity, indent);
            اگر (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                توڑو;
            }
            جاری;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        انتخاب (fmtC) {
        حالت 'c':
            /* single 8 bit char   */
            c = (حرف)va_arg(دلائل_2, int32_t);
            outputChar(c, برآمد_Buf, &برآمد_Ix, capacity, indent);
            توڑو;

        حالت 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(دلائل_2, حرف *);
            outputString((ثابت حرف *)ptrArg, برآمد_Buf, &برآمد_Ix, capacity, indent);
            توڑو;

        حالت 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(دلائل_2, حرف *);             /* Ptr    */
            intArg = va_arg(دلائل_2, int32_t);            /* Length */
            outputUString((ثابت char16_t *)ptrArg, intArg, برآمد_Buf, &برآمد_Ix, capacity, indent);
            توڑو;

        حالت 'b':
            /*  8 bit int  */
            intArg = va_arg(دلائل_2, صحیح);
            output_Hex_بائٹیں(intArg, 2, برآمد_Buf, &برآمد_Ix, capacity);
            توڑو;

        حالت 'h':
            /*  16 bit int  */
            intArg = va_arg(دلائل_2, صحیح);
            output_Hex_بائٹیں(intArg, 4, برآمد_Buf, &برآمد_Ix, capacity);
            توڑو;

        حالت 'd':
            /*  32 bit int  */
            intArg = va_arg(دلائل_2, صحیح);
            output_Hex_بائٹیں(intArg, 8, برآمد_Buf, &برآمد_Ix, capacity);
            توڑو;

        حالت 'l':
            /*  64 bit long  */
            longArg = va_arg(دلائل_2, int64_t);
            output_Hex_بائٹیں(longArg, 16, برآمد_Buf, &برآمد_Ix, capacity);
            توڑو;
            
        حالت 'p':
            /*  Pointers.   */
            ptrArg = va_arg(دلائل_2, حرف *);
            output_Ptr_بائٹیں(ptrArg, برآمد_Buf, &برآمد_Ix, capacity);
            توڑو;

        حالت 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', برآمد_Buf, &برآمد_Ix, capacity, indent);
            fmtIx--;
            توڑو;

        حالت 'v':
            {
                /* Vector of values, e.g. %vh */
                حرف     سمتیہ_Type;
                int32_t  سمتیہ_Len;
                ثابت حرف   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                خالی     **ptrPtr;
                int32_t   chars_مقصد_Output = 0;
                int32_t   i;
                
                سمتیہ_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                اگر (سمتیہ_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ثابت حرف *)va_arg(دلائل_2, خالی*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (خالی **)i8Ptr;
                سمتیہ_Len = va_arg(دلائل_2, int32_t);
                اگر (ptrPtr == خالی_اشاریہ) {
                    outputString("*NULL* ", برآمد_Buf, &برآمد_Ix, capacity, indent);
                } ورنہ {
                    برائے (i=0; i<سمتیہ_Len || سمتیہ_Len==-1; i++) { 
                        انتخاب (سمتیہ_Type) {
                        حالت 'b':
                            chars_مقصد_Output = 2;
                            longArg = *i8Ptr++;
                            توڑو;
                        حالت 'h':
                            chars_مقصد_Output = 4;
                            longArg = *i16Ptr++;
                            توڑو;
                        حالت 'd':
                            chars_مقصد_Output = 8;
                            longArg = *i32Ptr++;
                            توڑو;
                        حالت 'l':
                            chars_مقصد_Output = 16;
                            longArg = *i64Ptr++;
                            توڑو;
                        حالت 'p':
                            chars_مقصد_Output = 0;
                            output_Ptr_بائٹیں(*ptrPtr, برآمد_Buf, &برآمد_Ix, capacity);
                            longArg = *ptrPtr==خالی_اشاریہ? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            توڑو;
                        حالت 'c':
                            chars_مقصد_Output = 0;
                            outputChar(*i8Ptr, برآمد_Buf, &برآمد_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            توڑو;
                        حالت 's':
                            chars_مقصد_Output = 0;
                            outputString((ثابت حرف *)*ptrPtr, برآمد_Buf, &برآمد_Ix, capacity, indent);
                            outputChar('\n', برآمد_Buf, &برآمد_Ix, capacity, indent);
                            longArg = *ptrPtr==خالی_اشاریہ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            توڑو;

                        حالت 'S':
                            chars_مقصد_Output = 0;
                            outputUString((ثابت char16_t *)*ptrPtr, -1, برآمد_Buf, &برآمد_Ix, capacity, indent);
                            outputChar('\n', برآمد_Buf, &برآمد_Ix, capacity, indent);
                            longArg = *ptrPtr==خالی_اشاریہ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            توڑو;

                            
                        }
                        اگر (chars_مقصد_Output > 0) {
                            output_Hex_بائٹیں(longArg, chars_مقصد_Output, برآمد_Buf, &برآمد_Ix, capacity);
                            outputChar(' ', برآمد_Buf, &برآمد_Ix, capacity, indent);
                        }
                        اگر (سمتیہ_Len == -1 && longArg == 0) {
                            توڑو;
                        }
                    }
                }
                outputChar('[', برآمد_Buf, &برآمد_Ix, capacity, indent);
                output_Hex_بائٹیں(سمتیہ_Len, 8, برآمد_Buf, &برآمد_Ix, capacity);
                outputChar(']', برآمد_Buf, &برآمد_Ix, capacity, indent);
            }
            توڑو;


        طےشدہ:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, برآمد_Buf, &برآمد_Ix, capacity, indent);
        }
    }
    outputChar(0, برآمد_Buf, &برآمد_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    واپس برآمد_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(حرف *برآمد_Buf, int32_t capacity,
                int32_t indent, ثابت حرف *fmt,  ...) {
    int32_t retVal;
    va_list دلائل_2;
    va_start(دلائل_2, fmt ); 
    retVal = utrace_vformat(برآمد_Buf, capacity, indent, fmt, دلائل_2);
    va_end(دلائل_2);
    واپس retVal;
}


U_CAPI خالی U_EXPORT2
utrace_سیٹ_Functions(ثابت خالی *context,
                    UTrace_اندراج *e, UTraceExit *x, UTrace_ڈیٹا *d) {
    p_Trace_اندراج_Func = e;
    pTraceExitFunc  = x;
    p_Trace_ڈیٹا_Func  = d;
    gTraceContext   = context;
}


U_CAPI خالی U_EXPORT2
utrace_getFunctions(ثابت خالی **context,
                    UTrace_اندراج **e, UTraceExit **x, UTrace_ڈیٹا **d) {
    *e = p_Trace_اندراج_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_ڈیٹا_Func;
    *context = gTraceContext;
}

U_CAPI خالی U_EXPORT2
utrace_سیٹ_Level(int32_t level) {
    اگر (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    اگر (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    واپس utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    p_Trace_اندراج_Func = خالی_اشاریہ;
    pTraceExitFunc  = خالی_اشاریہ;
    p_Trace_ڈیٹا_Func  = خالی_اشاریہ;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = خالی_اشاریہ;
    واپس سچ;
}


ساکن ثابت حرف * ثابت
trFnName[] = {
    "u_init",
    "u_cleanup",
    خالی_اشاریہ
};


ساکن ثابت حرف * ثابت
tr_Conv_نام[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    خالی_اشاریہ
};

    
ساکن ثابت حرف * ثابت
tr_Coll_نام[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    خالی_اشاریہ
};


ساکن ثابت حرف* ثابت
tr_Res_ڈیٹا_نام[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    خالی_اشاریہ
};

                
U_CAPI ثابت حرف * U_EXPORT2
utrace_functionName(int32_t fn_نمبر) {
    اگر(UTRACE_FUNCTION_آغاز <= fn_نمبر && fn_نمبر < UTRACE_FUNCTION_حد_بندی) {
        واپس trFnName[fn_نمبر];
    } ورنہ اگر(UTRACE_CONVERSION_آغاز <= fn_نمبر && fn_نمبر < UTRACE_CONVERSION_حد_بندی) {
        واپس tr_Conv_نام[fn_نمبر - UTRACE_CONVERSION_آغاز];
    } ورنہ اگر(UTRACE_COLLATION_آغاز <= fn_نمبر && fn_نمبر < UTRACE_COLLATION_حد_بندی){
        واپس tr_Coll_نام[fn_نمبر - UTRACE_COLLATION_آغاز];
    } ورنہ اگر(UTRACE_UDATA_آغاز <= fn_نمبر && fn_نمبر < UTRACE_RES_ڈیٹا_حد_بندی){
        واپس tr_Res_ڈیٹا_نام[fn_نمبر - UTRACE_UDATA_آغاز];
    } ورنہ {
        واپس "[BOGUS Trace Function Number]";
    }
}

