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


स्थिर UTraceEntry     *pTraceEntryFunc = nullptr;
स्थिर UTraceExit      *pTraceExitFunc  = nullptr;
स्थिर UTraceData      *pTraceDataFunc  = nullptr;
स्थिर ध्रुव शून्य      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
स्थिर int32_t
utrace_level = UTRACE_ERROR;

U_CAPI शून्य U_EXPORT2
utrace_entry(int32_t fn_संख्या) {
    यदि (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_संख्या);
    }
}


स्थिर ध्रुव वर्ण gExitFmt[]             = "Returns.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_मूल्य[]        = "Returns %d.";
स्थिर ध्रुव वर्ण gExitFmtStatus[]       = "Returns.  Status = %d.";
स्थिर ध्रुव वर्ण g_Exit_Fmt_मूल्य_Status[]  = "Returns %d.  Status = %d.";
स्थिर ध्रुव वर्ण gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI शून्य U_EXPORT2
utrace_exit(int32_t fn_संख्या, int32_t returnType, ...) {
    यदि (pTraceExitFunc != nullptr) {
        बहु_सूची     args;
        ध्रुव वर्ण *fmt;

        विकल्प (returnType) {
        पक्ष 0:
            fmt = gExitFmt;
            भङ्ग;
        पक्ष UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_मूल्य;
            भङ्ग;
        पक्ष UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            भङ्ग;
        पक्ष UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_मूल्य_Status;
            भङ्ग;
        पक्ष UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            भङ्ग;
        स्वभाव:
            UPRV_UNREACHABLE_EXIT;
        }

        बहु_शुरू(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_संख्या, fmt, args);
        बहु_पूर्ण(args);
    }
}
 

 
U_CAPI शून्य U_EXPORT2 
utrace_data(int32_t fn_संख्या, int32_t level, ध्रुव वर्ण *fmt, ...) {
    यदि (pTraceDataFunc != nullptr) {
           बहु_सूची args;
           बहु_शुरू(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_संख्या, level, fmt, args);
           बहु_पूर्ण(args);
    }
}


स्थिर शून्य outputChar(वर्ण c, वर्ण *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    यदि (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        प्रति(i=0; i<indent; i++) {
            यदि (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    यदि (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    यदि (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

स्थिर शून्य outputHexBytes(int64_t val, int32_t charsToOutput,
                           वर्ण *outBuf, int32_t *outIx, int32_t capacity) {
    स्थिर ध्रुव वर्ण gHexChars[] = "0123456789abcdef";
    int32_t shift_गिनती;
    प्रति  (shift_गिनती=(charsToOutput-1)*4; shift_गिनती >= 0; shift_गिनती-=4) {
        वर्ण c = gHexChars[(val >> shift_गिनती) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
स्थिर शून्य outputPtrBytes(शून्य *val, वर्ण *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    वर्ण* p = reinterpret_cast<वर्ण*>(&val); /* point to current byte to output in the ptr val  */

#यदि !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += परिमाण(शून्य *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    प्रति (i=0; i<परिमाण(शून्य *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

स्थिर शून्य outputString(ध्रुव वर्ण *s, वर्ण *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    वर्ण    c;
    यदि (s==nullptr) {
        s = "*NULL*";
    }
    कुरु {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } यावत् (c != 0);
}
        


स्थिर शून्य outputUString(ध्रुव char16_t *s, int32_t len,
                          वर्ण *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    यदि (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        प्रत्यागम;
    }

    प्रति (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        यदि (len == -1 && c==0) {
            भङ्ग;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(वर्ण *outBuf, int32_t capacity, int32_t indent, ध्रुव वर्ण *fmt, बहु_सूची args) {
    int32_t   outIx  = 0;
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
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
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
            c = (वर्ण)बहु_तर्क(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            भङ्ग;

        पक्ष 's':
            /* char * string, NUL terminated.  */
            ptrArg = बहु_तर्क(args, वर्ण *);
            outputString((ध्रुव वर्ण *)ptrArg, outBuf, &outIx, capacity, indent);
            भङ्ग;

        पक्ष 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = बहु_तर्क(args, वर्ण *);             /* Ptr    */
            intArg = बहु_तर्क(args, int32_t);            /* Length */
            outputUString((ध्रुव char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            भङ्ग;

        पक्ष 'b':
            /*  8 bit int  */
            intArg = बहु_तर्क(args, पूर्णाङ्क);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            भङ्ग;

        पक्ष 'h':
            /*  16 bit int  */
            intArg = बहु_तर्क(args, पूर्णाङ्क);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            भङ्ग;

        पक्ष 'd':
            /*  32 bit int  */
            intArg = बहु_तर्क(args, पूर्णाङ्क);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            भङ्ग;

        पक्ष 'l':
            /*  64 bit long  */
            longArg = बहु_तर्क(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            भङ्ग;
            
        पक्ष 'p':
            /*  Pointers.   */
            ptrArg = बहु_तर्क(args, वर्ण *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            भङ्ग;

        पक्ष 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            भङ्ग;

        पक्ष 'v':
            {
                /* Vector of values, e.g. %vh */
                वर्ण     vectorType;
                int32_t  vectorLen;
                ध्रुव वर्ण   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                शून्य     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                यदि (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (ध्रुव वर्ण *)बहु_तर्क(args, शून्य*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (शून्य **)i8Ptr;
                vectorLen = बहु_तर्क(args, int32_t);
                यदि (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } अन्यथा {
                    प्रति (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        विकल्प (vectorType) {
                        पक्ष 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            भङ्ग;
                        पक्ष 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            भङ्ग;
                        पक्ष 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            भङ्ग;
                        पक्ष 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            भङ्ग;
                        पक्ष 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            भङ्ग;
                        पक्ष 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            भङ्ग;
                        पक्ष 's':
                            charsToOutput = 0;
                            outputString((ध्रुव वर्ण *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            भङ्ग;

                        पक्ष 'S':
                            charsToOutput = 0;
                            outputUString((ध्रुव char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            भङ्ग;

                            
                        }
                        यदि (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        यदि (vectorLen == -1 && longArg == 0) {
                            भङ्ग;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            भङ्ग;


        स्वभाव:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    प्रत्यागम outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(वर्ण *outBuf, int32_t capacity,
                int32_t indent, ध्रुव वर्ण *fmt,  ...) {
    int32_t retVal;
    बहु_सूची args;
    बहु_शुरू(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    बहु_पूर्ण(args);
    प्रत्यागम retVal;
}


U_CAPI शून्य U_EXPORT2
utrace_setFunctions(ध्रुव शून्य *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI शून्य U_EXPORT2
utrace_getFunctions(ध्रुव शून्य **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI शून्य U_EXPORT2
utrace_setLevel(int32_t level) {
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
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    प्रत्यागम सत्य;
}


स्थिर ध्रुव वर्ण * ध्रुव
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


स्थिर ध्रुव वर्ण * ध्रुव
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

    
स्थिर ध्रुव वर्ण * ध्रुव
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


स्थिर ध्रुव वर्ण* ध्रुव
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI ध्रुव वर्ण * U_EXPORT2
utrace_functionName(int32_t fn_संख्या) {
    यदि(UTRACE_FUNCTION_START <= fn_संख्या && fn_संख्या < UTRACE_FUNCTION_LIMIT) {
        प्रत्यागम trFnName[fn_संख्या];
    } अन्यथा यदि(UTRACE_CONVERSION_START <= fn_संख्या && fn_संख्या < UTRACE_CONVERSION_LIMIT) {
        प्रत्यागम trConvNames[fn_संख्या - UTRACE_CONVERSION_START];
    } अन्यथा यदि(UTRACE_COLLATION_START <= fn_संख्या && fn_संख्या < UTRACE_COLLATION_LIMIT){
        प्रत्यागम trCollNames[fn_संख्या - UTRACE_COLLATION_START];
    } अन्यथा यदि(UTRACE_UDATA_START <= fn_संख्या && fn_संख्या < UTRACE_RES_DATA_LIMIT){
        प्रत्यागम trResDataNames[fn_संख्या - UTRACE_UDATA_START];
    } अन्यथा {
        प्रत्यागम "[BOGUS Trace Function Number]";
    }
}

