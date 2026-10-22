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
स्थिर स्थिरांक शून्य      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
स्थिर int32_t
utrace_level = UTRACE_ERROR;

U_CAPI शून्य U_EXPORT2
utrace_entry(int32_t fn_संख्या) {
    जर (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_संख्या);
    }
}


स्थिर स्थिरांक अक्षर gExitFmt[]             = "Returns.";
स्थिर स्थिरांक अक्षर g_Exit_Fmt_मूल्य[]        = "Returns %d.";
स्थिर स्थिरांक अक्षर gExitFmtStatus[]       = "Returns.  Status = %d.";
स्थिर स्थिरांक अक्षर g_Exit_Fmt_मूल्य_Status[]  = "Returns %d.  Status = %d.";
स्थिर स्थिरांक अक्षर gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI शून्य U_EXPORT2
utrace_exit(int32_t fn_संख्या, int32_t returnType, ...) {
    जर (pTraceExitFunc != nullptr) {
        बहु_सूची     args;
        स्थिरांक अक्षर *fmt;

        निवड (returnType) {
        स्थिती 0:
            fmt = gExitFmt;
            थांबा;
        स्थिती UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_मूल्य;
            थांबा;
        स्थिती UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            थांबा;
        स्थिती UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_मूल्य_Status;
            थांबा;
        स्थिती UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            थांबा;
        पूर्वनिर्धारित:
            UPRV_UNREACHABLE_EXIT;
        }

        बहु_शुरू(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_संख्या, fmt, args);
        बहु_पूर्ण(args);
    }
}
 

 
U_CAPI शून्य U_EXPORT2 
utrace_data(int32_t fn_संख्या, int32_t level, स्थिरांक अक्षर *fmt, ...) {
    जर (pTraceDataFunc != nullptr) {
           बहु_सूची args;
           बहु_शुरू(args, fmt ); 
           (*pTraceDataFunc)(gTraceContext, fn_संख्या, level, fmt, args);
           बहु_पूर्ण(args);
    }
}


स्थिर शून्य outputChar(अक्षर c, अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    जर (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        साठी(i=0; i<indent; i++) {
            जर (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    जर (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    जर (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

स्थिर शून्य outputHexBytes(int64_t val, int32_t charsToOutput,
                           अक्षर *outBuf, int32_t *outIx, int32_t capacity) {
    स्थिर स्थिरांक अक्षर gHexChars[] = "0123456789abcdef";
    int32_t shift_मोजणी;
    साठी  (shift_मोजणी=(charsToOutput-1)*4; shift_मोजणी >= 0; shift_मोजणी-=4) {
        अक्षर c = gHexChars[(val >> shift_मोजणी) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
स्थिर शून्य outputPtrBytes(शून्य *val, अक्षर *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    अक्षर* p = reinterpret_cast<अक्षर*>(&val); /* point to current byte to output in the ptr val  */

#जर !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += आकार(शून्य *) - 1;
#endif

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    साठी (i=0; i<आकार(शून्य *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

स्थिर शून्य outputString(स्थिरांक अक्षर *s, अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    अक्षर    c;
    जर (s==nullptr) {
        s = "*NULL*";
    }
    करा {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } पर्यंत (c != 0);
}
        


स्थिर शून्य outputUString(स्थिरांक char16_t *s, int32_t len,
                          अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    जर (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        परत;
    }

    साठी (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        जर (len == -1 && c==0) {
            थांबा;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(अक्षर *outBuf, int32_t capacity, int32_t indent, स्थिरांक अक्षर *fmt, बहु_सूची args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    अक्षर      fmtC;
    अक्षर      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    अक्षर      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    साठी (;;) {
        fmtC = fmt[fmtIx++];
        जर (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            जर (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                थांबा;
            }
            सुरूठेवा;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        निवड (fmtC) {
        स्थिती 'c':
            /* single 8 bit char   */
            c = (अक्षर)बहु_तर्क(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            थांबा;

        स्थिती 's':
            /* char * string, NUL terminated.  */
            ptrArg = बहु_तर्क(args, अक्षर *);
            outputString((स्थिरांक अक्षर *)ptrArg, outBuf, &outIx, capacity, indent);
            थांबा;

        स्थिती 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = बहु_तर्क(args, अक्षर *);             /* Ptr    */
            intArg = बहु_तर्क(args, int32_t);            /* Length */
            outputUString((स्थिरांक char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            थांबा;

        स्थिती 'b':
            /*  8 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            थांबा;

        स्थिती 'h':
            /*  16 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            थांबा;

        स्थिती 'd':
            /*  32 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            थांबा;

        स्थिती 'l':
            /*  64 bit long  */
            longArg = बहु_तर्क(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            थांबा;
            
        स्थिती 'p':
            /*  Pointers.   */
            ptrArg = बहु_तर्क(args, अक्षर *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            थांबा;

        स्थिती 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            थांबा;

        स्थिती 'v':
            {
                /* Vector of values, e.g. %vh */
                अक्षर     vectorType;
                int32_t  vectorLen;
                स्थिरांक अक्षर   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                शून्य     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                जर (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (स्थिरांक अक्षर *)बहु_तर्क(args, शून्य*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (शून्य **)i8Ptr;
                vectorLen = बहु_तर्क(args, int32_t);
                जर (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } अन्यथा {
                    साठी (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        निवड (vectorType) {
                        स्थिती 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            थांबा;
                        स्थिती 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            थांबा;
                        स्थिती 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            थांबा;
                        स्थिती 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            थांबा;
                        स्थिती 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            थांबा;
                        स्थिती 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            थांबा;
                        स्थिती 's':
                            charsToOutput = 0;
                            outputString((स्थिरांक अक्षर *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            थांबा;

                        स्थिती 'S':
                            charsToOutput = 0;
                            outputUString((स्थिरांक char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            थांबा;

                            
                        }
                        जर (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        जर (vectorLen == -1 && longArg == 0) {
                            थांबा;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            थांबा;


        पूर्वनिर्धारित:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    परत outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(अक्षर *outBuf, int32_t capacity,
                int32_t indent, स्थिरांक अक्षर *fmt,  ...) {
    int32_t retVal;
    बहु_सूची args;
    बहु_शुरू(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    बहु_पूर्ण(args);
    परत retVal;
}


U_CAPI शून्य U_EXPORT2
utrace_setFunctions(स्थिरांक शून्य *context,
                    UTraceEntry *e, UTraceExit *x, UTraceData *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    pTraceDataFunc  = d;
    gTraceContext   = context;
}


U_CAPI शून्य U_EXPORT2
utrace_getFunctions(स्थिरांक शून्य **context,
                    UTraceEntry **e, UTraceExit **x, UTraceData **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = pTraceDataFunc;
    *context = gTraceContext;
}

U_CAPI शून्य U_EXPORT2
utrace_setLevel(int32_t level) {
    जर (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    जर (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    परत utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    pTraceDataFunc  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    परत सत्य;
}


स्थिर स्थिरांक अक्षर * स्थिरांक
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


स्थिर स्थिरांक अक्षर * स्थिरांक
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

    
स्थिर स्थिरांक अक्षर * स्थिरांक
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


स्थिर स्थिरांक अक्षर* स्थिरांक
trResDataNames[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI स्थिरांक अक्षर * U_EXPORT2
utrace_functionName(int32_t fn_संख्या) {
    जर(UTRACE_FUNCTION_START <= fn_संख्या && fn_संख्या < UTRACE_FUNCTION_LIMIT) {
        परत trFnName[fn_संख्या];
    } अन्यथा जर(UTRACE_CONVERSION_START <= fn_संख्या && fn_संख्या < UTRACE_CONVERSION_LIMIT) {
        परत trConvNames[fn_संख्या - UTRACE_CONVERSION_START];
    } अन्यथा जर(UTRACE_COLLATION_START <= fn_संख्या && fn_संख्या < UTRACE_COLLATION_LIMIT){
        परत trCollNames[fn_संख्या - UTRACE_COLLATION_START];
    } अन्यथा जर(UTRACE_UDATA_START <= fn_संख्या && fn_संख्या < UTRACE_RES_DATA_LIMIT){
        परत trResDataNames[fn_संख्या - UTRACE_UDATA_START];
    } अन्यथा {
        परत "[BOGUS Trace Function Number]";
    }
}

