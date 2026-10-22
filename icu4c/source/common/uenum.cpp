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

#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "cmemory.h"

/* Layout of the baseContext buffer. */
प्रकारनिर्देश संरचना {
    int32_t len;  /* number of bytes available starting at 'data' */
    वर्ण    दत्त; /* actual data starts here */
} UEnum_बफर;

/* Extra bytes to allocate in the baseContext buffer. */
स्थिर ध्रुव int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
स्थिर शून्य* get_बफर_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    यदि (en->आधारः_Context != शून्यसूचकः) {
        यदि (static_cast<UEnum_बफर*>(en->आधारः_Context)->len < capacity) {
            capacity += PAD;
            en->आधारः_Context = uprv_realloc(en->आधारः_Context,
                                           परिमाण(int32_t) + capacity);
            यदि (en->आधारः_Context == शून्यसूचकः) {
                प्रत्यागम शून्यसूचकः;
            }
            static_cast<UEnum_बफर*>(en->आधारः_Context)->len = capacity;
        }
    } अन्यथा {
        capacity += PAD;
        en->आधारः_Context = uprv_malloc(परिमाण(int32_t) + capacity);
        यदि (en->आधारः_Context == शून्यसूचकः) {
            प्रत्यागम शून्यसूचकः;
        }
        static_cast<UEnum_बफर*>(en->आधारः_Context)->len = capacity;
    }
    
    प्रत्यागम static_cast<शून्य*>(&static_cast<UEnum_बफर*>(en->आधारः_Context)->दत्त);
}

U_CAPI शून्य U_EXPORT2
uenum_close(UEnumeration* en)
{
    यदि (en) {
        यदि (en->close != शून्यसूचकः) {
            यदि (en->आधारः_Context) {
                uprv_free(en->आधारः_Context);
            }
            en->close(en);
        } अन्यथा { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_गणनम्(UEnumeration* en, UError_संहिता* स्थितिः)
{
    यदि (!en || U_FAILURE(*स्थितिः)) {
        प्रत्यागम -1;
    }
    यदि (en->count != शून्यसूचकः) {
        प्रत्यागम en->count(en, स्थितिः);
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* परिणाम_दीर्घता,
                   UError_संहिता* स्थितिः)
{
    char16_t *ustr = शून्यसूचकः;
    int32_t len = 0;
    यदि (en->next != शून्यसूचकः) {
        ध्रुव वर्ण *cstr = en->next(en, &len, स्थितिः);
        यदि (cstr != शून्यसूचकः) {
            ustr = (char16_t*) get_बफर_2(en, (len+1) * परिमाण(char16_t));
            यदि (ustr == शून्यसूचकः) {
                *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            } अन्यथा {
                u_chars_गन्तव्यम्_UChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
    }
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = len;
    }
    प्रत्यागम ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_अग्र_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_दीर्घता,
                  UError_संहिता* स्थितिः)
{
    यदि (en->u_अग्र != शून्यसूचकः) {
        वर्ण *अस्थायी_Char_Val;
        ध्रुव char16_t *अस्थायी_UChar_Val = en->u_अग्र(en, परिणाम_दीर्घता, स्थितिः);
        यदि (अस्थायी_UChar_Val == शून्यसूचकः) {
            प्रत्यागम शून्यसूचकः;
        }
        अस्थायी_Char_Val = (वर्ण*)
            get_बफर_2(en, (*परिणाम_दीर्घता+1) * परिमाण(वर्ण));
        यदि (!अस्थायी_Char_Val) {
            *स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
            प्रत्यागम शून्यसूचकः;
        }
        u_UChars_गन्तव्यम्_Chars(अस्थायी_UChar_Val, अस्थायी_Char_Val, *परिणाम_दीर्घता);
        अस्थायी_Char_Val[*परिणाम_दीर्घता] = 0;
        प्रत्यागम अस्थायी_Char_Val;
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
}

U_CAPI ध्रुव char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* परिणाम_दीर्घता,
            UError_संहिता* स्थितिः)
{
    यदि (!en || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (en->u_अग्र != शून्यसूचकः) {
        प्रत्यागम en->u_अग्र(en, परिणाम_दीर्घता, स्थितिः);
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
}

U_CAPI ध्रुव वर्ण* U_EXPORT2
uenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* परिणाम_दीर्घता,
           UError_संहिता* स्थितिः)
{
    यदि (!en || U_FAILURE(*स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    यदि (en->next != शून्यसूचकः) {
        यदि (परिणाम_दीर्घता != शून्यसूचकः) {
            प्रत्यागम en->next(en, परिणाम_दीर्घता, स्थितिः);
        }
        अन्यथा {
            int32_t dummy_दीर्घता=0;
            प्रत्यागम en->next(en, &dummy_दीर्घता, स्थितिः);
        }
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
        प्रत्यागम शून्यसूचकः;
    }
}

U_CAPI शून्य U_EXPORT2
uenum_reset(UEnumeration* en, UError_संहिता* स्थितिः)
{
    यदि (!en || U_FAILURE(*स्थितिः)) {
        प्रत्यागम;
    }
    यदि (en->reset != शून्यसूचकः) {
        en->reset(en, स्थितिः);
    } अन्यथा {
        *स्थितिः = U_UNSUPPORTED_ERROR;
    }
}
