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

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utrace.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utracimp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ucln_cmn.h"


ਸਥਿਰ UTrace_ਇੰਦਰਾਜ     *p_Trace_ਇੰਦਰਾਜ_Func = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ UTraceExit      *pTraceExitFunc  = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ UTrace_ਅੰਕੜੇ      *p_Trace_ਅੰਕੜੇ_Func  = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ ਅਚਲ ਖਾਲੀ      *gTraceContext   = ਖਾਲੀ_ਸੂਚਕ;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ਸਥਿਰ int32_t
utrace_level = UTRACE_ERROR;

U_CAPI ਖਾਲੀ U_EXPORT2
utrace_ਇੰਦਰਾਜ(int32_t fn_ਸੰਖਿਆ) {
    ਜੇ (p_Trace_ਇੰਦਰਾਜ_Func != ਖਾਲੀ_ਸੂਚਕ) {
        (*p_Trace_ਇੰਦਰਾਜ_Func)(gTraceContext, fn_ਸੰਖਿਆ);
    }
}


ਸਥਿਰ ਅਚਲ ਅੱਖਰ gExitFmt[]             = "Returns.";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ g_Exit_Fmt_ਮੁੱਲ[]        = "Returns %d.";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ g_Exit_Fmt_ਸਥਿਤੀ[]       = "Returns.  Status = %d.";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ g_Exit_Fmt_ਮੁੱਲ_ਸਥਿਤੀ[]  = "Returns %d.  Status = %d.";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ g_Exit_Fmt_Ptr_ਸਥਿਤੀ[]    = "Returns %d.  Status = %p.";

U_CAPI ਖਾਲੀ U_EXPORT2
utrace_exit(int32_t fn_ਸੰਖਿਆ, int32_t returnType, ...) {
    ਜੇ (pTraceExitFunc != ਖਾਲੀ_ਸੂਚਕ) {
        va_list     ਦਲੀਲਾਂ_2;
        ਅਚਲ ਅੱਖਰ *fmt;

        ਚੋਣ (returnType) {
        ਹਾਲਤ 0:
            fmt = gExitFmt;
            ਰੋਕੋ;
        ਹਾਲਤ UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_ਮੁੱਲ;
            ਰੋਕੋ;
        ਹਾਲਤ UTRACE_EXITV_ਸਥਿਤੀ:
            fmt = g_Exit_Fmt_ਸਥਿਤੀ;
            ਰੋਕੋ;
        ਹਾਲਤ UTRACE_EXITV_I32 | UTRACE_EXITV_ਸਥਿਤੀ:
            fmt = g_Exit_Fmt_ਮੁੱਲ_ਸਥਿਤੀ;
            ਰੋਕੋ;
        ਹਾਲਤ UTRACE_EXITV_PTR | UTRACE_EXITV_ਸਥਿਤੀ:
            fmt = g_Exit_Fmt_Ptr_ਸਥਿਤੀ;
            ਰੋਕੋ;
        ਮੂਲ:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(ਦਲੀਲਾਂ_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_ਸੰਖਿਆ, fmt, ਦਲੀਲਾਂ_2);
        va_end(ਦਲੀਲਾਂ_2);
    }
}
 

 
U_CAPI ਖਾਲੀ U_EXPORT2 
utrace_ਅੰਕੜੇ(int32_t fn_ਸੰਖਿਆ, int32_t level, ਅਚਲ ਅੱਖਰ *fmt, ...) {
    ਜੇ (p_Trace_ਅੰਕੜੇ_Func != ਖਾਲੀ_ਸੂਚਕ) {
           va_list ਦਲੀਲਾਂ_2;
           va_start(ਦਲੀਲਾਂ_2, fmt ); 
           (*p_Trace_ਅੰਕੜੇ_Func)(gTraceContext, fn_ਸੰਖਿਆ, level, fmt, ਦਲੀਲਾਂ_2);
           va_end(ਦਲੀਲਾਂ_2);
    }
}


ਸਥਿਰ ਖਾਲੀ outputChar(ਅੱਖਰ c, ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t *ਬਾਹਰਲਾ_Ix, int32_t capacity, int32_t indent) {
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
    ਜੇ (*ਬਾਹਰਲਾ_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *ਬਾਹਰਲਾ_Ix < capacity && ਬਾਹਰਲਾ_Buf[(*ਬਾਹਰਲਾ_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *ਬਾਹਰਲਾ_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        ਲਈ(i=0; i<indent; i++) {
            ਜੇ (*ਬਾਹਰਲਾ_Ix < capacity) {
                ਬਾਹਰਲਾ_Buf[*ਬਾਹਰਲਾ_Ix] = ' ';
            }
            (*ਬਾਹਰਲਾ_Ix)++;
        }
    }

    ਜੇ (*ਬਾਹਰਲਾ_Ix < capacity) {
        ਬਾਹਰਲਾ_Buf[*ਬਾਹਰਲਾ_Ix] = c;
    }
    ਜੇ (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*ਬਾਹਰਲਾ_Ix)++;
    }
}

ਸਥਿਰ ਖਾਲੀ output_Hex_ਬਾਈਟਾਂ(int64_t val, int32_t chars_ਮੰਜ਼ਿਲ_Output,
                           ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t *ਬਾਹਰਲਾ_Ix, int32_t capacity) {
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ gHexChars[] = "0123456789abcdef";
    int32_t ਖਿਸਕਾਓ_ਗਿਣਤੀ;
    ਲਈ  (ਖਿਸਕਾਓ_ਗਿਣਤੀ=(chars_ਮੰਜ਼ਿਲ_Output-1)*4; ਖਿਸਕਾਓ_ਗਿਣਤੀ >= 0; ਖਿਸਕਾਓ_ਗਿਣਤੀ-=4) {
        ਅੱਖਰ c = gHexChars[(val >> ਖਿਸਕਾਓ_ਗਿਣਤੀ) & 0xf];
        outputChar(c, ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ਸਥਿਰ ਖਾਲੀ output_Ptr_ਬਾਈਟਾਂ(ਖਾਲੀ *val, ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t *ਬਾਹਰਲਾ_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    ਅੱਖਰ* p = reinterpret_cast<ਅੱਖਰ*>(&val); /* point to current byte to output in the ptr val  */

#ਜੇ !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += ਆਕਾਰ(ਖਾਲੀ *) - 1;
#ਜੇ_ਅੰਤ

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    ਲਈ (i=0; i<ਆਕਾਰ(ਖਾਲੀ *); i++) {
        output_Hex_ਬਾਈਟਾਂ(*p, 2, ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity);
        p += incVal;
    }
}

ਸਥਿਰ ਖਾਲੀ outputString(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t *ਬਾਹਰਲਾ_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    ਅੱਖਰ    c;
    ਜੇ (s==ਖਾਲੀ_ਸੂਚਕ) {
        s = "*NULL*";
    }
    ਕਰੋ {
        c = s[i++];
        outputChar(c, ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity, indent);
    } ਜਦੋਂ (c != 0);
}
        


ਸਥਿਰ ਖਾਲੀ outputUString(ਅਚਲ char16_t *s, int32_t len,
                          ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t *ਬਾਹਰਲਾ_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    ਜੇ (s==ਖਾਲੀ_ਸੂਚਕ) {
        outputString(ਖਾਲੀ_ਸੂਚਕ, ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity, indent);
        ਵਾਪਸ;
    }

    ਲਈ (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_ਬਾਈਟਾਂ(c, 4, ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity);
        outputChar(' ', ਬਾਹਰਲਾ_Buf, ਬਾਹਰਲਾ_Ix, capacity, indent);
        ਜੇ (len == -1 && c==0) {
            ਰੋਕੋ;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t capacity, int32_t indent, ਅਚਲ ਅੱਖਰ *fmt, va_list ਦਲੀਲਾਂ_2) {
    int32_t   ਬਾਹਰਲਾ_Ix  = 0;
    int32_t   fmtIx  = 0;
    ਅੱਖਰ      fmtC;
    ਅੱਖਰ      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    ਅੱਖਰ      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    ਲਈ (;;) {
        fmtC = fmt[fmtIx++];
        ਜੇ (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            ਜੇ (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                ਰੋਕੋ;
            }
            ਜਾਰੀ;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        ਚੋਣ (fmtC) {
        ਹਾਲਤ 'c':
            /* single 8 bit char   */
            c = (ਅੱਖਰ)va_arg(ਦਲੀਲਾਂ_2, int32_t);
            outputChar(c, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            ਰੋਕੋ;

        ਹਾਲਤ 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(ਦਲੀਲਾਂ_2, ਅੱਖਰ *);
            outputString((ਅਚਲ ਅੱਖਰ *)ptrArg, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            ਰੋਕੋ;

        ਹਾਲਤ 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(ਦਲੀਲਾਂ_2, ਅੱਖਰ *);             /* Ptr    */
            intArg = va_arg(ਦਲੀਲਾਂ_2, int32_t);            /* Length */
            outputUString((ਅਚਲ char16_t *)ptrArg, intArg, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            ਰੋਕੋ;

        ਹਾਲਤ 'b':
            /*  8 bit int  */
            intArg = va_arg(ਦਲੀਲਾਂ_2, ਪੂਰਨ);
            output_Hex_ਬਾਈਟਾਂ(intArg, 2, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
            ਰੋਕੋ;

        ਹਾਲਤ 'h':
            /*  16 bit int  */
            intArg = va_arg(ਦਲੀਲਾਂ_2, ਪੂਰਨ);
            output_Hex_ਬਾਈਟਾਂ(intArg, 4, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
            ਰੋਕੋ;

        ਹਾਲਤ 'd':
            /*  32 bit int  */
            intArg = va_arg(ਦਲੀਲਾਂ_2, ਪੂਰਨ);
            output_Hex_ਬਾਈਟਾਂ(intArg, 8, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
            ਰੋਕੋ;

        ਹਾਲਤ 'l':
            /*  64 bit long  */
            longArg = va_arg(ਦਲੀਲਾਂ_2, int64_t);
            output_Hex_ਬਾਈਟਾਂ(longArg, 16, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
            ਰੋਕੋ;
            
        ਹਾਲਤ 'p':
            /*  Pointers.   */
            ptrArg = va_arg(ਦਲੀਲਾਂ_2, ਅੱਖਰ *);
            output_Ptr_ਬਾਈਟਾਂ(ptrArg, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
            ਰੋਕੋ;

        ਹਾਲਤ 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            fmtIx--;
            ਰੋਕੋ;

        ਹਾਲਤ 'v':
            {
                /* Vector of values, e.g. %vh */
                ਅੱਖਰ     ਸਦਿਸ਼_Type;
                int32_t  ਸਦਿਸ਼_Len;
                ਅਚਲ ਅੱਖਰ   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                ਖਾਲੀ     **ptrPtr;
                int32_t   chars_ਮੰਜ਼ਿਲ_Output = 0;
                int32_t   i;
                
                ਸਦਿਸ਼_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                ਜੇ (ਸਦਿਸ਼_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ਅਚਲ ਅੱਖਰ *)va_arg(ਦਲੀਲਾਂ_2, ਖਾਲੀ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (ਖਾਲੀ **)i8Ptr;
                ਸਦਿਸ਼_Len = va_arg(ਦਲੀਲਾਂ_2, int32_t);
                ਜੇ (ptrPtr == ਖਾਲੀ_ਸੂਚਕ) {
                    outputString("*NULL* ", ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                } ਨਹੀਂਤਾਂ {
                    ਲਈ (i=0; i<ਸਦਿਸ਼_Len || ਸਦਿਸ਼_Len==-1; i++) { 
                        ਚੋਣ (ਸਦਿਸ਼_Type) {
                        ਹਾਲਤ 'b':
                            chars_ਮੰਜ਼ਿਲ_Output = 2;
                            longArg = *i8Ptr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 'h':
                            chars_ਮੰਜ਼ਿਲ_Output = 4;
                            longArg = *i16Ptr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 'd':
                            chars_ਮੰਜ਼ਿਲ_Output = 8;
                            longArg = *i32Ptr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 'l':
                            chars_ਮੰਜ਼ਿਲ_Output = 16;
                            longArg = *i64Ptr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 'p':
                            chars_ਮੰਜ਼ਿਲ_Output = 0;
                            output_Ptr_ਬਾਈਟਾਂ(*ptrPtr, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
                            longArg = *ptrPtr==ਖਾਲੀ_ਸੂਚਕ? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 'c':
                            chars_ਮੰਜ਼ਿਲ_Output = 0;
                            outputChar(*i8Ptr, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            ਰੋਕੋ;
                        ਹਾਲਤ 's':
                            chars_ਮੰਜ਼ਿਲ_Output = 0;
                            outputString((ਅਚਲ ਅੱਖਰ *)*ptrPtr, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                            outputChar('\n', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                            longArg = *ptrPtr==ਖਾਲੀ_ਸੂਚਕ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ਰੋਕੋ;

                        ਹਾਲਤ 'S':
                            chars_ਮੰਜ਼ਿਲ_Output = 0;
                            outputUString((ਅਚਲ char16_t *)*ptrPtr, -1, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                            outputChar('\n', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                            longArg = *ptrPtr==ਖਾਲੀ_ਸੂਚਕ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ਰੋਕੋ;

                            
                        }
                        ਜੇ (chars_ਮੰਜ਼ਿਲ_Output > 0) {
                            output_Hex_ਬਾਈਟਾਂ(longArg, chars_ਮੰਜ਼ਿਲ_Output, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
                            outputChar(' ', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                        }
                        ਜੇ (ਸਦਿਸ਼_Len == -1 && longArg == 0) {
                            ਰੋਕੋ;
                        }
                    }
                }
                outputChar('[', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
                output_Hex_ਬਾਈਟਾਂ(ਸਦਿਸ਼_Len, 8, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity);
                outputChar(']', ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
            }
            ਰੋਕੋ;


        ਮੂਲ:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);
        }
    }
    outputChar(0, ਬਾਹਰਲਾ_Buf, &ਬਾਹਰਲਾ_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    ਵਾਪਸ ਬਾਹਰਲਾ_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(ਅੱਖਰ *ਬਾਹਰਲਾ_Buf, int32_t capacity,
                int32_t indent, ਅਚਲ ਅੱਖਰ *fmt,  ...) {
    int32_t retVal;
    va_list ਦਲੀਲਾਂ_2;
    va_start(ਦਲੀਲਾਂ_2, fmt ); 
    retVal = utrace_vformat(ਬਾਹਰਲਾ_Buf, capacity, indent, fmt, ਦਲੀਲਾਂ_2);
    va_end(ਦਲੀਲਾਂ_2);
    ਵਾਪਸ retVal;
}


U_CAPI ਖਾਲੀ U_EXPORT2
utrace_ਸੈੱਟ_Functions(ਅਚਲ ਖਾਲੀ *context,
                    UTrace_ਇੰਦਰਾਜ *e, UTraceExit *x, UTrace_ਅੰਕੜੇ *d) {
    p_Trace_ਇੰਦਰਾਜ_Func = e;
    pTraceExitFunc  = x;
    p_Trace_ਅੰਕੜੇ_Func  = d;
    gTraceContext   = context;
}


U_CAPI ਖਾਲੀ U_EXPORT2
utrace_getFunctions(ਅਚਲ ਖਾਲੀ **context,
                    UTrace_ਇੰਦਰਾਜ **e, UTraceExit **x, UTrace_ਅੰਕੜੇ **d) {
    *e = p_Trace_ਇੰਦਰਾਜ_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_ਅੰਕੜੇ_Func;
    *context = gTraceContext;
}

U_CAPI ਖਾਲੀ U_EXPORT2
utrace_ਸੈੱਟ_Level(int32_t level) {
    ਜੇ (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    ਜੇ (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    ਵਾਪਸ utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    p_Trace_ਇੰਦਰਾਜ_Func = ਖਾਲੀ_ਸੂਚਕ;
    pTraceExitFunc  = ਖਾਲੀ_ਸੂਚਕ;
    p_Trace_ਅੰਕੜੇ_Func  = ਖਾਲੀ_ਸੂਚਕ;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = ਖਾਲੀ_ਸੂਚਕ;
    ਵਾਪਸ ਸੱਚ;
}


ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ
trFnName[] = {
    "u_init",
    "u_cleanup",
    ਖਾਲੀ_ਸੂਚਕ
};


ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ
tr_Conv_ਨਾਂ[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    ਖਾਲੀ_ਸੂਚਕ
};

    
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ
tr_Coll_ਨਾਂ[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    ਖਾਲੀ_ਸੂਚਕ
};


ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ
tr_Res_ਅੰਕੜੇ_ਨਾਂ[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    ਖਾਲੀ_ਸੂਚਕ
};

                
U_CAPI ਅਚਲ ਅੱਖਰ * U_EXPORT2
utrace_functionName(int32_t fn_ਸੰਖਿਆ) {
    ਜੇ(UTRACE_FUNCTION_ਸ਼ੁਰੂ <= fn_ਸੰਖਿਆ && fn_ਸੰਖਿਆ < UTRACE_FUNCTION_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ trFnName[fn_ਸੰਖਿਆ];
    } ਨਹੀਂਤਾਂ ਜੇ(UTRACE_CONVERSION_ਸ਼ੁਰੂ <= fn_ਸੰਖਿਆ && fn_ਸੰਖਿਆ < UTRACE_CONVERSION_ਹੱਦਬੰਦੀ) {
        ਵਾਪਸ tr_Conv_ਨਾਂ[fn_ਸੰਖਿਆ - UTRACE_CONVERSION_ਸ਼ੁਰੂ];
    } ਨਹੀਂਤਾਂ ਜੇ(UTRACE_COLLATION_ਸ਼ੁਰੂ <= fn_ਸੰਖਿਆ && fn_ਸੰਖਿਆ < UTRACE_COLLATION_ਹੱਦਬੰਦੀ){
        ਵਾਪਸ tr_Coll_ਨਾਂ[fn_ਸੰਖਿਆ - UTRACE_COLLATION_ਸ਼ੁਰੂ];
    } ਨਹੀਂਤਾਂ ਜੇ(UTRACE_UDATA_ਸ਼ੁਰੂ <= fn_ਸੰਖਿਆ && fn_ਸੰਖਿਆ < UTRACE_RES_ਅੰਕੜੇ_ਹੱਦਬੰਦੀ){
        ਵਾਪਸ tr_Res_ਅੰਕੜੇ_ਨਾਂ[fn_ਸੰਖਿਆ - UTRACE_UDATA_ਸ਼ੁਰੂ];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ "[BOGUS Trace Function Number]";
    }
}

