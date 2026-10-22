// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
*******************************************************************************
*
*   Copyright (C) 2002-2012, International Business Machines
*   Corporation and others.  All Rights Reserved.
*
*******************************************************************************
*   file name:  uenum.c
*   encoding:   UTF-8
*   tab size:   8 (not used)
*   indentation:2
*
*   created on: 2002jul08
*   created by: Vladimir Weinstein
*/

#అన్తర్భావ "unicode/putil.h"
#అన్తర్భావ "uenumimp.h"
#అన్తర్భావ "cmemory.h"

/* Layout of the baseContext buffer. */
రకనిర్వచనం నిర్మాణం {
    int32_t len;  /* number of bytes available starting at 'data' */
    అక్షరం    దత్తాంశం; /* actual data starts here */
} UEnum_బఫరు;

/* Extra bytes to allocate in the baseContext buffer. */
స్థిర స్థిరరాశి int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
స్థిర శూన్య* get_బఫరు_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    ఒకవేళ (en->పునాది_Context != శూన్యసూచిక) {
        ఒకవేళ (static_cast<UEnum_బఫరు*>(en->పునాది_Context)->len < capacity) {
            capacity += PAD;
            en->పునాది_Context = uprv_realloc(en->పునాది_Context,
                                           పరిమాణం(int32_t) + capacity);
            ఒకవేళ (en->పునాది_Context == శూన్యసూచిక) {
                తిరిగి శూన్యసూచిక;
            }
            static_cast<UEnum_బఫరు*>(en->పునాది_Context)->len = capacity;
        }
    } లేకపోతే {
        capacity += PAD;
        en->పునాది_Context = uprv_malloc(పరిమాణం(int32_t) + capacity);
        ఒకవేళ (en->పునాది_Context == శూన్యసూచిక) {
            తిరిగి శూన్యసూచిక;
        }
        static_cast<UEnum_బఫరు*>(en->పునాది_Context)->len = capacity;
    }
    
    తిరిగి static_cast<శూన్య*>(&static_cast<UEnum_బఫరు*>(en->పునాది_Context)->దత్తాంశం);
}

U_CAPI శూన్య U_EXPORT2
uenum_close(UEnumeration* en)
{
    ఒకవేళ (en) {
        ఒకవేళ (en->close != శూన్యసూచిక) {
            ఒకవేళ (en->పునాది_Context) {
                uprv_free(en->పునాది_Context);
            }
            en->close(en);
        } లేకపోతే { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_లెక్క(UEnumeration* en, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (!en || U_FAILURE(*స్థితి_2)) {
        తిరిగి -1;
    }
    ఒకవేళ (en->count != శూన్యసూచిక) {
        తిరిగి en->count(en, స్థితి_2);
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI స్థిరరాశి char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* ఫలితం_నిడివి,
                   UError_సంకేతం* స్థితి_2)
{
    char16_t *ustr = శూన్యసూచిక;
    int32_t len = 0;
    ఒకవేళ (en->next != శూన్యసూచిక) {
        స్థిరరాశి అక్షరం *cstr = en->next(en, &len, స్థితి_2);
        ఒకవేళ (cstr != శూన్యసూచిక) {
            ustr = (char16_t*) get_బఫరు_2(en, (len+1) * పరిమాణం(char16_t));
            ఒకవేళ (ustr == శూన్యసూచిక) {
                *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            } లేకపోతే {
                u_chars_గమ్యం_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
    }
    ఒకవేళ (ఫలితం_నిడివి) {
        *ఫలితం_నిడివి = len;
    }
    తిరిగి ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uenum_తదుపరి_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ఫలితం_నిడివి,
                  UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (en->u_తదుపరి != శూన్యసూచిక) {
        అక్షరం *తాత్కాలిక_Char_Val;
        స్థిరరాశి char16_t *తాత్కాలిక_UChar_Val = en->u_తదుపరి(en, ఫలితం_నిడివి, స్థితి_2);
        ఒకవేళ (తాత్కాలిక_UChar_Val == శూన్యసూచిక) {
            తిరిగి శూన్యసూచిక;
        }
        తాత్కాలిక_Char_Val = (అక్షరం*)
            get_బఫరు_2(en, (*ఫలితం_నిడివి+1) * పరిమాణం(అక్షరం));
        ఒకవేళ (!తాత్కాలిక_Char_Val) {
            *స్థితి_2 = U_జ్ఞాపకం_కేటాయింపు_ERROR;
            తిరిగి శూన్యసూచిక;
        }
        u_UChars_గమ్యం_Chars(తాత్కాలిక_UChar_Val, తాత్కాలిక_Char_Val, *ఫలితం_నిడివి);
        తాత్కాలిక_Char_Val[*ఫలితం_నిడివి] = 0;
        తిరిగి తాత్కాలిక_Char_Val;
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI స్థిరరాశి char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* ఫలితం_నిడివి,
            UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (!en || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (en->u_తదుపరి != శూన్యసూచిక) {
        తిరిగి en->u_తదుపరి(en, ఫలితం_నిడివి, స్థితి_2);
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI స్థిరరాశి అక్షరం* U_EXPORT2
uenum_తదుపరి(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* ఫలితం_నిడివి,
           UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (!en || U_FAILURE(*స్థితి_2)) {
        తిరిగి శూన్యసూచిక;
    }
    ఒకవేళ (en->next != శూన్యసూచిక) {
        ఒకవేళ (ఫలితం_నిడివి != శూన్యసూచిక) {
            తిరిగి en->next(en, ఫలితం_నిడివి, స్థితి_2);
        }
        లేకపోతే {
            int32_t dummy_నిడివి=0;
            తిరిగి en->next(en, &dummy_నిడివి, స్థితి_2);
        }
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
        తిరిగి శూన్యసూచిక;
    }
}

U_CAPI శూన్య U_EXPORT2
uenum_reset(UEnumeration* en, UError_సంకేతం* స్థితి_2)
{
    ఒకవేళ (!en || U_FAILURE(*స్థితి_2)) {
        తిరిగి;
    }
    ఒకవేళ (en->reset != శూన్యసూచిక) {
        en->reset(en, స్థితి_2);
    } లేకపోతే {
        *స్థితి_2 = U_UNSUPPORTED_ERROR;
    }
}
