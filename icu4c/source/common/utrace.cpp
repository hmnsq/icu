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

#अन्तर्भाव "unicode/utrace.h"
#अन्तर्भाव "utracimp.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "uassert.h"
#अन्तर्भाव "ucln_cmn.h"


स्थिर UTrace_प्रविष्टिः     *p_Trace_प्रविष्टिः_Func = शून्यसूचकः;
स्थिर UTraceExit      *pTraceExitFunc  = शून्यसूचकः;
स्थिर UTrace_दत्त      *p_Trace_दत्त_Func  = शून्यसूचकः;
स्थिर ध्रुव शून्य      *gTraceContext   = शून्यसूचकः;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
स्थिर int32_t
utrace_level = UTRACE_ERROR;

U_CAPI शून्य U_EXPORT2
utrace_प्रविष्टिः(int32_t fn_सङ्ख्या) {
    यदि (p_Trace_प्रविष्टिः_Func != शून्यसूचकः) {
        (*p_Trace_प्रविष्टिः_Func)(gTraceContext, fn_सङ्ख्या);
    }
}


स्थिर ध्रुव वर्ण gExitFmt[]             = "Returns.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_मानम्[]        = "Returns %d.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_स्थितिः[]       = "Returns.  Status = %d.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_मानम्_स्थितिः[]  = "Returns %d.  Status = %d.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_Ptr_स्थितिः[]    = "Returns %d.  Status = %p.";

U_CAPI शून्य U_EXPORT2
utrace_exit(int32_t fn_सङ्ख्या, int32_t returnType, ...) {
    यदि (pTraceExitFunc != शून्यसूचकः) {
        va_list     तर्काः_2;
        ध्रुव वर्ण *fmt;

        विकल्प (returnType) {
        पक्ष 0:
            fmt = gExitFmt;
            भङ्ग;
        पक्ष UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_मानम्;
            भङ्ग;
        पक्ष UTRACE_EXITV_स्थितिः:
            fmt = g_Exit_Fmt_स्थितिः;
            भङ्ग;
        पक्ष UTRACE_EXITV_I32 | UTRACE_EXITV_स्थितिः:
            fmt = g_Exit_Fmt_मानम्_स्थितिः;
            भङ्ग;
        पक्ष UTRACE_EXITV_PTR | UTRACE_EXITV_स्थितिः:
            fmt = g_Exit_Fmt_Ptr_स्थितिः;
            भङ्ग;
        स्वभाव:
            UPRV_UNREACHABLE_EXIT;
        }

        va_start(तर्काः_2, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_सङ्ख्या, fmt, तर्काः_2);
        va_end(तर्काः_2);
    }
}
 

 
U_CAPI शून्य U_EXPORT2 
utrace_दत्त(int32_t fn_सङ्ख्या, int32_t level, ध्रुव वर्ण *fmt, ...) {
    यदि (p_Trace_दत्त_Func != शून्यसूचकः) {
           va_list तर्काः_2;
           va_start(तर्काः_2, fmt ); 
           (*p_Trace_दत्त_Func)(gTraceContext, fn_सङ्ख्या, level, fmt, तर्काः_2);
           va_end(तर्काः_2);
    }
}


स्थिर शून्य outputChar(वर्ण c, वर्ण *बहिर्गमनम्_Buf, int32_t *बहिर्गमनम्_Ix, int32_t capacity, int32_t indent) {
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
    यदि (*बहिर्गमनम्_Ix==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *बहिर्गमनम्_Ix < capacity && बहिर्गमनम्_Buf[(*बहिर्गमनम्_Ix)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *बहिर्गमनम्_Ix>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        प्रति(i=0; i<indent; i++) {
            यदि (*बहिर्गमनम्_Ix < capacity) {
                बहिर्गमनम्_Buf[*बहिर्गमनम्_Ix] = ' ';
            }
            (*बहिर्गमनम्_Ix)++;
        }
    }

    यदि (*बहिर्गमनम्_Ix < capacity) {
        बहिर्गमनम्_Buf[*बहिर्गमनम्_Ix] = c;
    }
    यदि (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*बहिर्गमनम्_Ix)++;
    }
}

स्थिर शून्य output_Hex_अष्टकानि(int64_t val, int32_t chars_गन्तव्यम्_Output,
                           वर्ण *बहिर्गमनम्_Buf, int32_t *बहिर्गमनम्_Ix, int32_t capacity) {
    स्थिर ध्रुव वर्ण gHexChars[] = "0123456789abcdef";
    int32_t सरणम्_गणनम्;
    प्रति  (सरणम्_गणनम्=(chars_गन्तव्यम्_Output-1)*4; सरणम्_गणनम् >= 0; सरणम्_गणनम्-=4) {
        वर्ण c = gHexChars[(val >> सरणम्_गणनम्) & 0xf];
        outputChar(c, बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
स्थिर शून्य output_Ptr_अष्टकानि(शून्य *val, वर्ण *बहिर्गमनम्_Buf, int32_t *बहिर्गमनम्_Ix, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    वर्ण* p = reinterpret_cast<वर्ण*>(&val); /* point to current byte to output in the ptr val  */

#यदि !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += परिमाण(शून्य *) - 1;
#यदि_अन्तः

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    प्रति (i=0; i<परिमाण(शून्य *); i++) {
        output_Hex_अष्टकानि(*p, 2, बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity);
        p += incVal;
    }
}

स्थिर शून्य outputString(ध्रुव वर्ण *s, वर्ण *बहिर्गमनम्_Buf, int32_t *बहिर्गमनम्_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    वर्ण    c;
    यदि (s==शून्यसूचकः) {
        s = "*NULL*";
    }
    कुरु {
        c = s[i++];
        outputChar(c, बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity, indent);
    } यावत् (c != 0);
}
        


स्थिर शून्य outputUString(ध्रुव char16_t *s, int32_t len,
                          वर्ण *बहिर्गमनम्_Buf, int32_t *बहिर्गमनम्_Ix, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    यदि (s==शून्यसूचकः) {
        outputString(शून्यसूचकः, बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity, indent);
        प्रत्यागम;
    }

    प्रति (i=0; i<len || len==-1; i++) {
        c = s[i];
        output_Hex_अष्टकानि(c, 4, बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity);
        outputChar(' ', बहिर्गमनम्_Buf, बहिर्गमनम्_Ix, capacity, indent);
        यदि (len == -1 && c==0) {
            भङ्ग;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(वर्ण *बहिर्गमनम्_Buf, int32_t capacity, int32_t indent, ध्रुव वर्ण *fmt, va_list तर्काः_2) {
    int32_t   बहिर्गमनम्_Ix  = 0;
    int32_t   fmtIx  = 0;
    वर्ण      fmtC;
    वर्ण      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    वर्ण      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    प्रति (;;) {
        fmtC = fmt[fmtIx++];
        यदि (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            यदि (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                भङ्ग;
            }
            अनुवर्त;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        विकल्प (fmtC) {
        पक्ष 'c':
            /* single 8 bit char   */
            c = (वर्ण)va_arg(तर्काः_2, int32_t);
            outputChar(c, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            भङ्ग;

        पक्ष 's':
            /* char * string, NUL terminated.  */
            ptrArg = va_arg(तर्काः_2, वर्ण *);
            outputString((ध्रुव वर्ण *)ptrArg, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            भङ्ग;

        पक्ष 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = va_arg(तर्काः_2, वर्ण *);             /* Ptr    */
            intArg = va_arg(तर्काः_2, int32_t);            /* Length */
            outputUString((ध्रुव char16_t *)ptrArg, intArg, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            भङ्ग;

        पक्ष 'b':
            /*  8 bit int  */
            intArg = va_arg(तर्काः_2, पूर्णाङ्क);
            output_Hex_अष्टकानि(intArg, 2, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
            भङ्ग;

        पक्ष 'h':
            /*  16 bit int  */
            intArg = va_arg(तर्काः_2, पूर्णाङ्क);
            output_Hex_अष्टकानि(intArg, 4, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
            भङ्ग;

        पक्ष 'd':
            /*  32 bit int  */
            intArg = va_arg(तर्काः_2, पूर्णाङ्क);
            output_Hex_अष्टकानि(intArg, 8, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
            भङ्ग;

        पक्ष 'l':
            /*  64 bit long  */
            longArg = va_arg(तर्काः_2, int64_t);
            output_Hex_अष्टकानि(longArg, 16, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
            भङ्ग;
            
        पक्ष 'p':
            /*  Pointers.   */
            ptrArg = va_arg(तर्काः_2, वर्ण *);
            output_Ptr_अष्टकानि(ptrArg, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
            भङ्ग;

        पक्ष 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            fmtIx--;
            भङ्ग;

        पक्ष 'v':
            {
                /* Vector of values, e.g. %vh */
                वर्ण     सदिशम्_Type;
                int32_t  सदिशम्_Len;
                ध्रुव वर्ण   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                शून्य     **ptrPtr;
                int32_t   chars_गन्तव्यम्_Output = 0;
                int32_t   i;
                
                सदिशम्_Type = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                यदि (सदिशम्_Type != 0) {
                    fmtIx++;
                }
                i8Ptr = (ध्रुव वर्ण *)va_arg(तर्काः_2, शून्य*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (शून्य **)i8Ptr;
                सदिशम्_Len = va_arg(तर्काः_2, int32_t);
                यदि (ptrPtr == शून्यसूचकः) {
                    outputString("*NULL* ", बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                } अन्यथा {
                    प्रति (i=0; i<सदिशम्_Len || सदिशम्_Len==-1; i++) { 
                        विकल्प (सदिशम्_Type) {
                        पक्ष 'b':
                            chars_गन्तव्यम्_Output = 2;
                            longArg = *i8Ptr++;
                            भङ्ग;
                        पक्ष 'h':
                            chars_गन्तव्यम्_Output = 4;
                            longArg = *i16Ptr++;
                            भङ्ग;
                        पक्ष 'd':
                            chars_गन्तव्यम्_Output = 8;
                            longArg = *i32Ptr++;
                            भङ्ग;
                        पक्ष 'l':
                            chars_गन्तव्यम्_Output = 16;
                            longArg = *i64Ptr++;
                            भङ्ग;
                        पक्ष 'p':
                            chars_गन्तव्यम्_Output = 0;
                            output_Ptr_अष्टकानि(*ptrPtr, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
                            longArg = *ptrPtr==शून्यसूचकः? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            भङ्ग;
                        पक्ष 'c':
                            chars_गन्तव्यम्_Output = 0;
                            outputChar(*i8Ptr, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            भङ्ग;
                        पक्ष 's':
                            chars_गन्तव्यम्_Output = 0;
                            outputString((ध्रुव वर्ण *)*ptrPtr, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                            outputChar('\n', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                            longArg = *ptrPtr==शून्यसूचकः? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            भङ्ग;

                        पक्ष 'S':
                            chars_गन्तव्यम्_Output = 0;
                            outputUString((ध्रुव char16_t *)*ptrPtr, -1, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                            outputChar('\n', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                            longArg = *ptrPtr==शून्यसूचकः? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            भङ्ग;

                            
                        }
                        यदि (chars_गन्तव्यम्_Output > 0) {
                            output_Hex_अष्टकानि(longArg, chars_गन्तव्यम्_Output, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
                            outputChar(' ', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                        }
                        यदि (सदिशम्_Len == -1 && longArg == 0) {
                            भङ्ग;
                        }
                    }
                }
                outputChar('[', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
                output_Hex_अष्टकानि(सदिशम्_Len, 8, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity);
                outputChar(']', बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
            }
            भङ्ग;


        स्वभाव:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);
        }
    }
    outputChar(0, बहिर्गमनम्_Buf, &बहिर्गमनम्_Ix, capacity, indent);  /* Make sure that output is NUL terminated   */
    प्रत्यागम बहिर्गमनम्_Ix + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(वर्ण *बहिर्गमनम्_Buf, int32_t capacity,
                int32_t indent, ध्रुव वर्ण *fmt,  ...) {
    int32_t retVal;
    va_list तर्काः_2;
    va_start(तर्काः_2, fmt ); 
    retVal = utrace_vformat(बहिर्गमनम्_Buf, capacity, indent, fmt, तर्काः_2);
    va_end(तर्काः_2);
    प्रत्यागम retVal;
}


U_CAPI शून्य U_EXPORT2
utrace_समुच्चयः_Functions(ध्रुव शून्य *context,
                    UTrace_प्रविष्टिः *e, UTraceExit *x, UTrace_दत्त *d) {
    p_Trace_प्रविष्टिः_Func = e;
    pTraceExitFunc  = x;
    p_Trace_दत्त_Func  = d;
    gTraceContext   = context;
}


U_CAPI शून्य U_EXPORT2
utrace_getFunctions(ध्रुव शून्य **context,
                    UTrace_प्रविष्टिः **e, UTraceExit **x, UTrace_दत्त **d) {
    *e = p_Trace_प्रविष्टिः_Func;
    *x = pTraceExitFunc;
    *d = p_Trace_दत्त_Func;
    *context = gTraceContext;
}

U_CAPI शून्य U_EXPORT2
utrace_समुच्चयः_Level(int32_t level) {
    यदि (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    यदि (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    प्रत्यागम utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    p_Trace_प्रविष्टिः_Func = शून्यसूचकः;
    pTraceExitFunc  = शून्यसूचकः;
    p_Trace_दत्त_Func  = शून्यसूचकः;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = शून्यसूचकः;
    प्रत्यागम सत्य;
}


स्थिर ध्रुव वर्ण * ध्रुव
trFnName[] = {
    "u_init",
    "u_cleanup",
    शून्यसूचकः
};


स्थिर ध्रुव वर्ण * ध्रुव
tr_Conv_नामानि[] = {
    "ucnv_open",
    "ucnv_openPackage",
    "ucnv_openAlgorithmic",
    "ucnv_clone",
    "ucnv_close",
    "ucnv_flushCache",
    "ucnv_load",
    "ucnv_unload",
    शून्यसूचकः
};

    
स्थिर ध्रुव वर्ण * ध्रुव
tr_Coll_नामानि[] = {
    "ucol_open",
    "ucol_close",
    "ucol_strcoll",
    "ucol_getSortKey",
    "ucol_getLocale",
    "ucol_nextSortKeyPart",
    "ucol_strcollIter",
    "ucol_openFromShortString",
    "ucol_strcollUTF8",
    शून्यसूचकः
};


स्थिर ध्रुव वर्ण* ध्रुव
tr_Res_दत्त_नामानि[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    शून्यसूचकः
};

                
U_CAPI ध्रुव वर्ण * U_EXPORT2
utrace_functionName(int32_t fn_सङ्ख्या) {
    यदि(UTRACE_FUNCTION_START <= fn_सङ्ख्या && fn_सङ्ख्या < UTRACE_FUNCTION_सीमा) {
        प्रत्यागम trFnName[fn_सङ्ख्या];
    } अन्यथा यदि(UTRACE_CONVERSION_START <= fn_सङ्ख्या && fn_सङ्ख्या < UTRACE_CONVERSION_सीमा) {
        प्रत्यागम tr_Conv_नामानि[fn_सङ्ख्या - UTRACE_CONVERSION_START];
    } अन्यथा यदि(UTRACE_COLLATION_START <= fn_सङ्ख्या && fn_सङ्ख्या < UTRACE_COLLATION_सीमा){
        प्रत्यागम tr_Coll_नामानि[fn_सङ्ख्या - UTRACE_COLLATION_START];
    } अन्यथा यदि(UTRACE_UDATA_START <= fn_सङ्ख्या && fn_सङ्ख्या < UTRACE_RES_दत्त_सीमा){
        प्रत्यागम tr_Res_दत्त_नामानि[fn_सङ्ख्या - UTRACE_UDATA_START];
    } अन्यथा {
        प्रत्यागम "[BOGUS Trace Function Number]";
    }
}

