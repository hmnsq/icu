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

#ಅನ್ತರ್ಭಾವ "unicode/utrace.h"
#ಅನ್ತರ್ಭಾವ "utracimp.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"
#ಅನ್ತರ್ಭಾವ "ucln_cmn.h"


ಸ್ಥಿರ UTrace_ನಮೂದು     *p_Trace_ನಮೂದು_Func = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ UTraceExit      *pTraceExitFunc  = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ UTrace_ದತ್ತಾಂಶ      *p_Trace_ದತ್ತಾಂಶ_Func  = ಶೂನ್ಯಸೂಚಕ;
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ      *gTraceContext   = ಶೂನ್ಯಸೂಚಕ;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
ಸ್ಥಿರ int32_t
utrace_level = UTRACE_ERROR;

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrace_ನಮೂದು(int32_t fn_ಸಂಖ್ಯೆ) {
    ಆದರೆ (p_Trace_ನಮೂದು_Func != ಶೂನ್ಯಸೂಚಕ) {
        (*p_Trace_ನಮೂದು_Func)(gTraceContext, fn_ಸಂಖ್ಯೆ);
    }
}


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ gExitFmt[]             = "Returns.";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ g_Exit_Fmt_ಮೌಲ್ಯ[]        = "Returns %d.";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ g_Exit_Fmt_ಸ್ಥಿತಿ[]       = "Returns.  Status = %d.";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ g_Exit_Fmt_ಮೌಲ್ಯ_ಸ್ಥಿತಿ[]  = "Returns %d.  Status = %d.";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ g_Exit_Fmt_Ptr_ಸ್ಥಿತಿ[]    = "Returns %d.  Status = %p.";

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrace_exit(int32_t fn_ಸಂಖ್ಯೆ, int32_t returnType, ...) {
    ಆದರೆ (pTraceExitFunc != ಶೂನ್ಯಸೂಚಕ) {
        va_list     ವಾದಗಳು_2;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt;

        ಆಯ್ಕೆ (returnType) {
        ಸ್ಥಿತಿ 0:
            fmt = gExitFmt;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_ಮೌಲ್ಯ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UTRACE_EXITV_ಸ್ಥಿತಿ:
            fmt = g_Exit_Fmt_ಸ್ಥಿತಿ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UTRACE_EXITV_I32 | UTRACE_EXITV_ಸ್ಥಿತಿ:
            fmt = g_Exit_Fmt_ಮೌಲ್ಯ_ಸ್ಥಿತಿ;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ UTRACE_EXITV_PTR | UTRACE_EXITV_ಸ್ಥಿತಿ:
            fmt = g_Exit_Fmt_Ptr_ಸ್ಥಿತಿ;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(ವಾದಗಳು_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_ಸಂಖ್ಯೆ, fmt, ವಾದಗಳು_2);
        va_end(ವಾದಗಳು_2);
    }
}
 

 
U_CAPI ಶೂನ್ಯ U_EXPORT2 
utrace_ದತ್ತಾಂಶ(int32_t fn_ಸಂಖ್ಯೆ, int32_t level, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) {
    ಆದರೆ (p_Trace_ದತ್ತಾಂಶ_Func != ಶೂನ್ಯಸೂಚಕ) {
           va_list ವಾದಗಳು_2;
           va_start(ವಾದಗಳು_2, fmt ); 
           (*p_Trace_ದತ್ತಾಂಶ_Func)(gTraceContext, fn_ಸಂಖ್ಯೆ, level, fmt, ವಾದಗಳು_2);
           va_end(ವಾದಗಳು_2);
    }
}


ಸ್ಥಿರ ಶೂನ್ಯ outputChar(ಅಕ್ಷರ c, ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t *ಹೊರಹೋಗು_Ix, int32_t capacity, int32_t indent) {
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
    ಆದರೆ (*ಹೊರಹೋಗು_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *ಹೊರಹೋಗು_Ix < capacity && ಹೊರಹೋಗು_Buf[(*ಹೊರಹೋಗು_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *ಹೊರಹೋಗು_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        ಗಾಗಿ(i=0; i<indent; i++) {
            ಆದರೆ (*ಹೊರಹೋಗು_Ix < capacity) {
                ಹೊರಹೋಗು_Buf[*ಹೊರಹೋಗು_Ix] = ' ';
            }
            (*ಹೊರಹೋಗು_Ix)++;
        }
    }

    ಆದರೆ (*ಹೊರಹೋಗು_Ix < capacity) {
        ಹೊರಹೋಗು_Buf[*ಹೊರಹೋಗು_Ix] = c;
    }
    ಆದರೆ (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*ಹೊರಹೋಗು_Ix)++;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ output_Hex_ಬೈಟುಗಳು(int64_t val, int32_t chars_ಗಮ್ಯ_Output,
                           ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t *ಹೊರಹೋಗು_Ix, int32_t capacity) {
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ gHexChars[] = "0123456789abcdef";
    int32_t ಸರಿಸು_ಎಣಿಕೆ;
    ಗಾಗಿ  (ಸರಿಸು_ಎಣಿಕೆ=(chars_ಗಮ್ಯ_Output-1)*4; ಸರಿಸು_ಎಣಿಕೆ >= 0; ಸರಿಸು_ಎಣಿಕೆ-=4) {
        ಅಕ್ಷರ c = gHexChars[(val >> ಸರಿಸು_ಎಣಿಕೆ) & 0xf];
        outputChar(c, ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
ಸ್ಥಿರ ಶೂನ್ಯ output_Ptr_ಬೈಟುಗಳು(ಶೂನ್ಯ *val, ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t *ಹೊರಹೋಗು_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    ಅಕ್ಷರ* p = reinterpret_cast<ಅಕ್ಷರ*>(&val); /* point to current byte to output in the ptr val  */

#ಆದರೆ !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += ಗಾತ್ರ(ಶೂನ್ಯ *) - 1;
#ಆದರೆ_ಅಂತ್ಯ

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    ಗಾಗಿ (i=0; i<ಗಾತ್ರ(ಶೂನ್ಯ *); i++) {
        output_Hex_ಬೈಟುಗಳು(*p, 2, ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity);
        p += incVal;
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ outputString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t *ಹೊರಹೋಗು_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    ಅಕ್ಷರ    c;
    ಆದರೆ (s==ಶೂನ್ಯಸೂಚಕ) {
        s = "*NULL*";
    }
    ಮಾಡು {
        c = s[i++];
        outputChar(c, ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity, indent);
    } ಆಗ (c != 0);
}
        


ಸ್ಥಿರ ಶೂನ್ಯ outputUString(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t len,
                          ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t *ಹೊರಹೋಗು_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    ಆದರೆ (s==ಶೂನ್ಯಸೂಚಕ) {
        outputString(ಶೂನ್ಯಸೂಚಕ, ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity, indent);
        ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_ಬೈಟುಗಳು(c, 4, ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity);
        outputChar(' ', ಹೊರಹೋಗು_Buf, ಹೊರಹೋಗು_Ix, capacity, indent);
        ಆದರೆ (len == -1 && c==0) {
            ನಿಲ್ಲಿಸು;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t capacity, int32_t indent, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ವಾದಗಳು_2) {
    int32_t   ಹೊರಹೋಗು_Ix  = 0;
    int32_t   fmtIx  = 0;
    ಅಕ್ಷರ      fmtC;
    ಅಕ್ಷರ      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    ಅಕ್ಷರ      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    ಗಾಗಿ (;;) {
        fmtC = fmt[fmtIx++];
        ಆದರೆ (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            ಆದರೆ (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                ನಿಲ್ಲಿಸು;
            }
            ಮುಂದುವರಿ;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        ಆಯ್ಕೆ (fmtC) {
        ಸ್ಥಿತಿ 'c':
            /* single 8 bit char   */
            c = (ಅಕ್ಷರ)va_arg(ವಾದಗಳು_2, int32_t);
            outputChar(c, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(ವಾದಗಳು_2, ಅಕ್ಷರ *);
            outputString((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ptrArg, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(ವಾದಗಳು_2, ಅಕ್ಷರ *);             /* Ptr    */
            intArg = va_arg(ವಾದಗಳು_2, int32_t);            /* Length */
            outputUString((ಸ್ಥಿರಾಂಕ char16_t *)ptrArg, intArg, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'b':
            /*  8 bit int  */
            intArg = va_arg(ವಾದಗಳು_2, ಪೂರ್ಣಾಂಕ);
            output_Hex_ಬೈಟುಗಳು(intArg, 2, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'h':
            /*  16 bit int  */
            intArg = va_arg(ವಾದಗಳು_2, ಪೂರ್ಣಾಂಕ);
            output_Hex_ಬೈಟುಗಳು(intArg, 4, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'd':
            /*  32 bit int  */
            intArg = va_arg(ವಾದಗಳು_2, ಪೂರ್ಣಾಂಕ);
            output_Hex_ಬೈಟುಗಳು(intArg, 8, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'l':
            /*  64 bit long  */
            longArg = va_arg(ವಾದಗಳು_2, int64_t);
            output_Hex_ಬೈಟುಗಳು(longArg, 16, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
            ನಿಲ್ಲಿಸು;
            
        ಸ್ಥಿತಿ 'p':
            /*  Pointers.   */
            ptrArg = va_arg(ವಾದಗಳು_2, ಅಕ್ಷರ *);
            output_Ptr_ಬೈಟುಗಳು(ptrArg, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            fmtIx--;
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ 'v':
            {
                /* Vector of values, e.g. %vh */
                ಅಕ್ಷರ     ಸದಿಶ_Type;
                int32_t  ಸದಿಶ_Len;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                ಶೂನ್ಯ     **ptrPtr;
                int32_t   chars_ಗಮ್ಯ_Output = 0;
                int32_t   i;
                
                ಸದಿಶ_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                ಆದರೆ (ಸದಿಶ_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)va_arg(ವಾದಗಳು_2, ಶೂನ್ಯ*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (ಶೂನ್ಯ **)i8Ptr;
                ಸದಿಶ_Len = va_arg(ವಾದಗಳು_2, int32_t);
                ಆದರೆ (ptrPtr == ಶೂನ್ಯಸೂಚಕ) {
                    outputString("*NULL* ", ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                } ಇಲ್ಲದಿದ್ದರೆ {
                    ಗಾಗಿ (i=0; i<ಸದಿಶ_Len || ಸದಿಶ_Len==-1; i++) { 
                        ಆಯ್ಕೆ (ಸದಿಶ_Type) {
                        ಸ್ಥಿತಿ 'b':
                            chars_ಗಮ್ಯ_Output = 2;
                            longArg = *i8Ptr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 'h':
                            chars_ಗಮ್ಯ_Output = 4;
                            longArg = *i16Ptr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 'd':
                            chars_ಗಮ್ಯ_Output = 8;
                            longArg = *i32Ptr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 'l':
                            chars_ಗಮ್ಯ_Output = 16;
                            longArg = *i64Ptr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 'p':
                            chars_ಗಮ್ಯ_Output = 0;
                            output_Ptr_ಬೈಟುಗಳು(*ptrPtr, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
                            longArg = *ptrPtr==ಶೂನ್ಯಸೂಚಕ? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 'c':
                            chars_ಗಮ್ಯ_Output = 0;
                            outputChar(*i8Ptr, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            ನಿಲ್ಲಿಸು;
                        ಸ್ಥಿತಿ 's':
                            chars_ಗಮ್ಯ_Output = 0;
                            outputString((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)*ptrPtr, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                            outputChar('\n', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                            longArg = *ptrPtr==ಶೂನ್ಯಸೂಚಕ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ನಿಲ್ಲಿಸು;

                        ಸ್ಥಿತಿ 'S':
                            chars_ಗಮ್ಯ_Output = 0;
                            outputUString((ಸ್ಥಿರಾಂಕ char16_t *)*ptrPtr, -1, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                            outputChar('\n', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                            longArg = *ptrPtr==ಶೂನ್ಯಸೂಚಕ? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            ನಿಲ್ಲಿಸು;

                            
                        }
                        ಆದರೆ (chars_ಗಮ್ಯ_Output > 0) {
                            output_Hex_ಬೈಟುಗಳು(longArg, chars_ಗಮ್ಯ_Output, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
                            outputChar(' ', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                        }
                        ಆದರೆ (ಸದಿಶ_Len == -1 && longArg == 0) {
                            ನಿಲ್ಲಿಸು;
                        }
                    }
                }
                outputChar('[', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
                output_Hex_ಬೈಟುಗಳು(ಸದಿಶ_Len, 8, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity);
                outputChar(']', ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
            }
            ನಿಲ್ಲಿಸು;


        ಪೂರ್ವನಿಯೋಜಿತ:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);
        }
    }
    outputChar(0, ಹೊರಹೋಗು_Buf, &ಹೊರಹೋಗು_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(ಅಕ್ಷರ *ಹೊರಹೋಗು_Buf, int32_t capacity,
                int32_t indent, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt,  ...) {
    int32_t retVal;
    va_list ವಾದಗಳು_2;
    va_start(ವಾದಗಳು_2, fmt ); 
    retVal = utrace_vformat(ಹೊರಹೋಗು_Buf, capacity, indent, fmt, ವಾದಗಳು_2);
    va_end(ವಾದಗಳು_2);
    ಹಿಂತಿರುಗಿ retVal;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
utrace_ಗಣ_Functions(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *context,
                    UTrace_ನಮೂದು *e, UTraceExit *x, UTrace_ದತ್ತಾಂಶ *d) {
    p_Trace_ನಮೂದು_Func = e;
    pTraceExitFunc  = x;
    p_Trace_ದತ್ತಾಂಶ_Func  = d;
    gTraceContext   = context;
}


U_CAPI ಶೂನ್ಯ U_EXPORT2
utrace_getFunctions(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **context,
                    UTrace_ನಮೂದು **e, UTraceExit **x, UTrace_ದತ್ತಾಂಶ **d) {
    *e = p_Trace_ನಮೂದು_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_ದತ್ತಾಂಶ_Func;
    *context = gTraceContext;
}

U_CAPI ಶೂನ್ಯ U_EXPORT2
utrace_ಗಣ_Level(int32_t level) {
    ಆದರೆ (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    ಆದರೆ (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    ಹಿಂತಿರುಗಿ utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    p_Trace_ನಮೂದು_Func = ಶೂನ್ಯಸೂಚಕ;
    pTraceExitFunc  = ಶೂನ್ಯಸೂಚಕ;
    p_Trace_ದತ್ತಾಂಶ_Func  = ಶೂನ್ಯಸೂಚಕ;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = ಶೂನ್ಯಸೂಚಕ;
    ಹಿಂತಿರುಗಿ ನಿಜ;
}


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ
trFnName[] = {
    "u_init",
    "u_cleanup",
    ಶೂನ್ಯಸೂಚಕ
};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ
tr_Conv_ಹೆಸರುಗಳು[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    ಶೂನ್ಯಸೂಚಕ
};

    
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ
tr_Coll_ಹೆಸರುಗಳು[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    ಶೂನ್ಯಸೂಚಕ
};


ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ
tr_Res_ದತ್ತಾಂಶ_ಹೆಸರುಗಳು[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    ಶೂನ್ಯಸೂಚಕ
};

                
U_CAPI ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * U_EXPORT2
utrace_functionName(int32_t fn_ಸಂಖ್ಯೆ) {
    ಆದರೆ(UTRACE_FUNCTION_ಆರಂಭ <= fn_ಸಂಖ್ಯೆ && fn_ಸಂಖ್ಯೆ < UTRACE_FUNCTION_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ trFnName[fn_ಸಂಖ್ಯೆ];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UTRACE_CONVERSION_ಆರಂಭ <= fn_ಸಂಖ್ಯೆ && fn_ಸಂಖ್ಯೆ < UTRACE_CONVERSION_ಎಲ್ಲೆ) {
        ಹಿಂತಿರುಗಿ tr_Conv_ಹೆಸರುಗಳು[fn_ಸಂಖ್ಯೆ - UTRACE_CONVERSION_ಆರಂಭ];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UTRACE_COLLATION_ಆರಂಭ <= fn_ಸಂಖ್ಯೆ && fn_ಸಂಖ್ಯೆ < UTRACE_COLLATION_ಎಲ್ಲೆ){
        ಹಿಂತಿರುಗಿ tr_Coll_ಹೆಸರುಗಳು[fn_ಸಂಖ್ಯೆ - UTRACE_COLLATION_ಆರಂಭ];
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(UTRACE_UDATA_ಆರಂಭ <= fn_ಸಂಖ್ಯೆ && fn_ಸಂಖ್ಯೆ < UTRACE_RES_ದತ್ತಾಂಶ_ಎಲ್ಲೆ){
        ಹಿಂತಿರುಗಿ tr_Res_ದತ್ತಾಂಶ_ಹೆಸರುಗಳು[fn_ಸಂಖ್ಯೆ - UTRACE_UDATA_ಆರಂಭ];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ "[BOGUS Trace Function Number]";
    }
}

