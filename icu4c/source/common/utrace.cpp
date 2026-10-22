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

#समावेश "unicode/utrace.h"
#समावेश "utracimp.h"
#समावेश "cstring.h"
#समावेश "uassert.h"
#समावेश "ucln_cmn.h"


अटल UTraceEntry     *pTraceEntryFunc = nullptr;
अटल UTraceExit      *pTraceExitFunc  = nullptr;
अटल UTrace_तथ्य      *p_Trace_तथ्य_Func  = nullptr;
अटल स्थिर व्योम      *gTraceContext   = nullptr;

/**
 * \var utrace_level
 * Trace level variable. Negative for "off".
 */
अटल int32_t
utrace_level = UTRACE_ERROR;

U_CAPI व्योम U_EXPORT2
utrace_entry(int32_t fn_संख्या) {
    अगर (pTraceEntryFunc != nullptr) {
        (*pTraceEntryFunc)(gTraceContext, fn_संख्या);
    }
}


अटल स्थिर अक्षर gExitFmt[]             = "Returns.";
अटल स्थिर अक्षर g_Exit_Fmt_मूल्य[]        = "Returns %d.";
अटल स्थिर अक्षर gExitFmtStatus[]       = "Returns.  Status = %d.";
अटल स्थिर अक्षर g_Exit_Fmt_मूल्य_Status[]  = "Returns %d.  Status = %d.";
अटल स्थिर अक्षर gExitFmtPtrStatus[]    = "Returns %d.  Status = %p.";

U_CAPI व्योम U_EXPORT2
utrace_exit(int32_t fn_संख्या, int32_t returnType, ...) {
    अगर (pTraceExitFunc != nullptr) {
        बहु_सूची     args;
        स्थिर अक्षर *fmt;

        चयन (returnType) {
        हाल 0:
            fmt = gExitFmt;
            अवरोध;
        हाल UTRACE_EXITV_I32:
            fmt = g_Exit_Fmt_मूल्य;
            अवरोध;
        हाल UTRACE_EXITV_STATUS:
            fmt = gExitFmtStatus;
            अवरोध;
        हाल UTRACE_EXITV_I32 | UTRACE_EXITV_STATUS:
            fmt = g_Exit_Fmt_मूल्य_Status;
            अवरोध;
        हाल UTRACE_EXITV_PTR | UTRACE_EXITV_STATUS:
            fmt = gExitFmtPtrStatus;
            अवरोध;
        शेष:
            UPRV_UNREACHABLE_EXIT;
        }

        बहु_शुरू(args, returnType);
        (*pTraceExitFunc)(gTraceContext, fn_संख्या, fmt, args);
        बहु_पूर्ण(args);
    }
}
 

 
U_CAPI व्योम U_EXPORT2 
utrace_तथ्य(int32_t fn_संख्या, int32_t level, स्थिर अक्षर *fmt, ...) {
    अगर (p_Trace_तथ्य_Func != nullptr) {
           बहु_सूची args;
           बहु_शुरू(args, fmt ); 
           (*p_Trace_तथ्य_Func)(gTraceContext, fn_संख्या, level, fmt, args);
           बहु_पूर्ण(args);
    }
}


अटल व्योम outputChar(अक्षर c, अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
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
    अगर (*outIx==0 ||   /* case 1. */
        (c!='\n' && c!=0 && *outIx < capacity && outBuf[(*outIx)-1]=='\n') ||  /* case 2. */
        (c=='\n' && *outIx>=capacity))    /* case 3 */
    {
        /* At the start of a line.  Indent. */
        क्रम(i=0; i<indent; i++) {
            अगर (*outIx < capacity) {
                outBuf[*outIx] = ' ';
            }
            (*outIx)++;
        }
    }

    अगर (*outIx < capacity) {
        outBuf[*outIx] = c;
    }
    अगर (c != 0) {
        /* NULs only appear as end-of-string terminators.  Move them to the output
         *  buffer, but do not update the length of the buffer, so that any
         *  following output will overwrite the NUL. */
        (*outIx)++;
    }
}

अटल व्योम outputHexBytes(int64_t val, int32_t charsToOutput,
                           अक्षर *outBuf, int32_t *outIx, int32_t capacity) {
    अटल स्थिर अक्षर gHexChars[] = "0123456789abcdef";
    int32_t shift_गिनती;
    क्रम  (shift_गिनती=(charsToOutput-1)*4; shift_गिनती >= 0; shift_गिनती-=4) {
        अक्षर c = gHexChars[(val >> shift_गिनती) & 0xf];
        outputChar(c, outBuf, outIx, capacity, 0);
    }
}

/* Output a pointer value in hex.  Work with any size of pointer   */
अटल व्योम outputPtrBytes(व्योम *val, अक्षर *outBuf, int32_t *outIx, int32_t capacity) {
    uint32_t  i;
    int32_t  incVal = 1;              /* +1 for big endian, -1 for little endian          */
    अक्षर* p = reinterpret_cast<अक्षर*>(&val); /* point to current byte to output in the ptr val  */

#अगर !U_IS_BIG_ENDIAN
    /* Little Endian.  Move p to most significant end of the value      */
    incVal = -1;
    p += माप(व्योम *) - 1;
#पूर्ण_अगर

    /* Loop through the bytes of the ptr as it sits in memory, from 
     * most significant to least significant end                    */
    क्रम (i=0; i<माप(व्योम *); i++) {
        outputHexBytes(*p, 2, outBuf, outIx, capacity);
        p += incVal;
    }
}

अटल व्योम outputString(स्थिर अक्षर *s, अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    अक्षर    c;
    अगर (s==nullptr) {
        s = "*NULL*";
    }
    करो {
        c = s[i++];
        outputChar(c, outBuf, outIx, capacity, indent);
    } जबतक (c != 0);
}
        


अटल व्योम outputUString(स्थिर char16_t *s, int32_t len,
                          अक्षर *outBuf, int32_t *outIx, int32_t capacity, int32_t indent) {
    int32_t i = 0;
    char16_t   c;
    अगर (s==nullptr) {
        outputString(nullptr, outBuf, outIx, capacity, indent);
        वापस;
    }

    क्रम (i=0; i<len || len==-1; i++) {
        c = s[i];
        outputHexBytes(c, 4, outBuf, outIx, capacity);
        outputChar(' ', outBuf, outIx, capacity, indent);
        अगर (len == -1 && c==0) {
            अवरोध;
        }
    }
}
        
U_CAPI int32_t U_EXPORT2
utrace_vformat(अक्षर *outBuf, int32_t capacity, int32_t indent, स्थिर अक्षर *fmt, बहु_सूची args) {
    int32_t   outIx  = 0;
    int32_t   fmtIx  = 0;
    अक्षर      fmtC;
    अक्षर      c;
    int32_t   intArg;
    int64_t   longArg = 0;
    अक्षर      *ptrArg;

    /*   Loop runs once for each character in the format string.
     */
    क्रम (;;) {
        fmtC = fmt[fmtIx++];
        अगर (fmtC != '%') {
            /* Literal character, not part of a %sequence.  Just copy it to the output. */
            outputChar(fmtC, outBuf, &outIx, capacity, indent);
            अगर (fmtC == 0) {
                /* We hit the NUL that terminates the format string.
                 * This is the normal (and only) exit from the loop that
                 * interprets the format
                 */
                अवरोध;
            }
            जारी;
        }

        /* We encountered a '%'.  Pick up the following format char */
        fmtC = fmt[fmtIx++];

        चयन (fmtC) {
        हाल 'c':
            /* single 8 bit char   */
            c = (अक्षर)बहु_तर्क(args, int32_t);
            outputChar(c, outBuf, &outIx, capacity, indent);
            अवरोध;

        हाल 's':
            /* char * string, NUL terminated.  */
            ptrArg = बहु_तर्क(args, अक्षर *);
            outputString((स्थिर अक्षर *)ptrArg, outBuf, &outIx, capacity, indent);
            अवरोध;

        हाल 'S':
            /* char16_t * string, with length, len==-1 for NUL terminated. */
            ptrArg = बहु_तर्क(args, अक्षर *);             /* Ptr    */
            intArg = बहु_तर्क(args, int32_t);            /* Length */
            outputUString((स्थिर char16_t *)ptrArg, intArg, outBuf, &outIx, capacity, indent);
            अवरोध;

        हाल 'b':
            /*  8 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 2, outBuf, &outIx, capacity);
            अवरोध;

        हाल 'h':
            /*  16 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 4, outBuf, &outIx, capacity);
            अवरोध;

        हाल 'd':
            /*  32 bit int  */
            intArg = बहु_तर्क(args, पूर्णांक);
            outputHexBytes(intArg, 8, outBuf, &outIx, capacity);
            अवरोध;

        हाल 'l':
            /*  64 bit long  */
            longArg = बहु_तर्क(args, int64_t);
            outputHexBytes(longArg, 16, outBuf, &outIx, capacity);
            अवरोध;
            
        हाल 'p':
            /*  Pointers.   */
            ptrArg = बहु_तर्क(args, अक्षर *);
            outputPtrBytes(ptrArg, outBuf, &outIx, capacity);
            अवरोध;

        हाल 0:
            /* Single '%' at end of fmt string.  Output as literal '%'.   
             * Back up index into format string so that the terminating NUL will be
             * re-fetched in the outer loop, causing it to terminate.
             */
            outputChar('%', outBuf, &outIx, capacity, indent);
            fmtIx--;
            अवरोध;

        हाल 'v':
            {
                /* Vector of values, e.g. %vh */
                अक्षर     vectorType;
                int32_t  vectorLen;
                स्थिर अक्षर   *i8Ptr;
                int16_t  *i16Ptr;
                int32_t  *i32Ptr;
                int64_t  *i64Ptr;
                व्योम     **ptrPtr;
                int32_t   charsToOutput = 0;
                int32_t   i;
                
                vectorType = fmt[fmtIx];    /* b, h, d, l, p, etc. */
                अगर (vectorType != 0) {
                    fmtIx++;
                }
                i8Ptr = (स्थिर अक्षर *)बहु_तर्क(args, व्योम*);
                i16Ptr = (int16_t *)i8Ptr;
                i32Ptr = (int32_t *)i8Ptr;
                i64Ptr = (int64_t *)i8Ptr;
                ptrPtr = (व्योम **)i8Ptr;
                vectorLen = बहु_तर्क(args, int32_t);
                अगर (ptrPtr == nullptr) {
                    outputString("*NULL* ", outBuf, &outIx, capacity, indent);
                } अन्यथा {
                    क्रम (i=0; i<vectorLen || vectorLen==-1; i++) { 
                        चयन (vectorType) {
                        हाल 'b':
                            charsToOutput = 2;
                            longArg = *i8Ptr++;
                            अवरोध;
                        हाल 'h':
                            charsToOutput = 4;
                            longArg = *i16Ptr++;
                            अवरोध;
                        हाल 'd':
                            charsToOutput = 8;
                            longArg = *i32Ptr++;
                            अवरोध;
                        हाल 'l':
                            charsToOutput = 16;
                            longArg = *i64Ptr++;
                            अवरोध;
                        हाल 'p':
                            charsToOutput = 0;
                            outputPtrBytes(*ptrPtr, outBuf, &outIx, capacity);
                            longArg = *ptrPtr==nullptr? 0: 1;    /* test for nullptr terminated array. */
                            ptrPtr++;
                            अवरोध;
                        हाल 'c':
                            charsToOutput = 0;
                            outputChar(*i8Ptr, outBuf, &outIx, capacity, indent);
                            longArg = *i8Ptr;    /* for test for nullptr terminated array. */
                            i8Ptr++;
                            अवरोध;
                        हाल 's':
                            charsToOutput = 0;
                            outputString((स्थिर अक्षर *)*ptrPtr, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            अवरोध;

                        हाल 'S':
                            charsToOutput = 0;
                            outputUString((स्थिर char16_t *)*ptrPtr, -1, outBuf, &outIx, capacity, indent);
                            outputChar('\n', outBuf, &outIx, capacity, indent);
                            longArg = *ptrPtr==nullptr? 0: 1;   /* for test for nullptr term. array. */
                            ptrPtr++;
                            अवरोध;

                            
                        }
                        अगर (charsToOutput > 0) {
                            outputHexBytes(longArg, charsToOutput, outBuf, &outIx, capacity);
                            outputChar(' ', outBuf, &outIx, capacity, indent);
                        }
                        अगर (vectorLen == -1 && longArg == 0) {
                            अवरोध;
                        }
                    }
                }
                outputChar('[', outBuf, &outIx, capacity, indent);
                outputHexBytes(vectorLen, 8, outBuf, &outIx, capacity);
                outputChar(']', outBuf, &outIx, capacity, indent);
            }
            अवरोध;


        शेष:
            /* %. in format string, where . is some character not in the set
             *    of recognized format chars.  Just output it as if % wasn't there.
             *    (Covers "%%" outputting a single '%')
             */
             outputChar(fmtC, outBuf, &outIx, capacity, indent);
        }
    }
    outputChar(0, outBuf, &outIx, capacity, indent);  /* Make sure that output is NUL terminated   */
    वापस outIx + 1;     /* outIx + 1 because outIx does not increment when outputting final NUL. */
}




U_CAPI int32_t U_EXPORT2
utrace_format(अक्षर *outBuf, int32_t capacity,
                int32_t indent, स्थिर अक्षर *fmt,  ...) {
    int32_t retVal;
    बहु_सूची args;
    बहु_शुरू(args, fmt ); 
    retVal = utrace_vformat(outBuf, capacity, indent, fmt, args);
    बहु_पूर्ण(args);
    वापस retVal;
}


U_CAPI व्योम U_EXPORT2
utrace_setFunctions(स्थिर व्योम *context,
                    UTraceEntry *e, UTraceExit *x, UTrace_तथ्य *d) {
    pTraceEntryFunc = e;
    pTraceExitFunc  = x;
    p_Trace_तथ्य_Func  = d;
    gTraceContext   = context;
}


U_CAPI व्योम U_EXPORT2
utrace_getFunctions(स्थिर व्योम **context,
                    UTraceEntry **e, UTraceExit **x, UTrace_तथ्य **d) {
    *e = pTraceEntryFunc;
    *x = pTraceExitFunc;
    *d = p_Trace_तथ्य_Func;
    *context = gTraceContext;
}

U_CAPI व्योम U_EXPORT2
utrace_setLevel(int32_t level) {
    अगर (level < UTRACE_OFF) {
        level = UTRACE_OFF;
    }
    अगर (level > UTRACE_VERBOSE) {
        level = UTRACE_VERBOSE;
    }
    utrace_level = level;
}

U_CAPI int32_t U_EXPORT2
utrace_getLevel() {
    वापस utrace_level;
}


U_CFUNC UBool 
utrace_cleanup() {
    pTraceEntryFunc = nullptr;
    pTraceExitFunc  = nullptr;
    p_Trace_तथ्य_Func  = nullptr;
    utrace_level    = UTRACE_OFF;
    gTraceContext   = nullptr;
    वापस true;
}


अटल स्थिर अक्षर * स्थिर
trFnName[] = {
    "u_init",
    "u_cleanup",
    nullptr
};


अटल स्थिर अक्षर * स्थिर
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

    
अटल स्थिर अक्षर * स्थिर
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


अटल स्थिर अक्षर* स्थिर
tr_Res_तथ्य_Names[] = {
    "resc",
    "bundle-open",
    "file-open",
    "res-open",
    nullptr
};

                
U_CAPI स्थिर अक्षर * U_EXPORT2
utrace_functionName(int32_t fn_संख्या) {
    अगर(UTRACE_FUNCTION_START <= fn_संख्या && fn_संख्या < UTRACE_FUNCTION_LIMIT) {
        वापस trFnName[fn_संख्या];
    } अन्यथा अगर(UTRACE_CONVERSION_START <= fn_संख्या && fn_संख्या < UTRACE_CONVERSION_LIMIT) {
        वापस trConvNames[fn_संख्या - UTRACE_CONVERSION_START];
    } अन्यथा अगर(UTRACE_COLLATION_START <= fn_संख्या && fn_संख्या < UTRACE_COLLATION_LIMIT){
        वापस trCollNames[fn_संख्या - UTRACE_COLLATION_START];
    } अन्यथा अगर(UTRACE_UDATA_START <= fn_संख्या && fn_संख्या < UTRACE_RES_तथ्य_LIMIT){
        वापस tr_Res_तथ्य_Names[fn_संख्या - UTRACE_UDATA_START];
    } अन्यथा {
        वापस "[BOGUS Trace Function Number]";
    }
}

