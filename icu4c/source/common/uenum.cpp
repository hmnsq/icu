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

#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "cmemory.h"

/* Layout of the baseContext buffer. */
প্রকারলেখ কাঠামো {
    int32_t len;  /* number of bytes available starting at 'data' */
    অক্ষর    তথ্য; /* actual data starts here */
} UEnum_বাফার;

/* Extra bytes to allocate in the baseContext buffer. */
স্থির ধ্রুবক int32_t PAD = 8;

/* Return a pointer to the baseContext buffer, possibly allocating
   or reallocating it if at least 'capacity' bytes are not available. */
স্থির শূন্য* get_বাফার_2(UEnumeration* en U_LIFETIME_BOUND, int32_t capacity) {

    যদি (en->baseContext != nullptr) {
        যদি (static_cast<UEnum_বাফার*>(en->baseContext)->len < capacity) {
            capacity += PAD;
            en->baseContext = uprv_realloc(en->baseContext,
                                           আকার(int32_t) + capacity);
            যদি (en->baseContext == nullptr) {
                ফেরত nullptr;
            }
            static_cast<UEnum_বাফার*>(en->baseContext)->len = capacity;
        }
    } অন্যথা {
        capacity += PAD;
        en->baseContext = uprv_malloc(আকার(int32_t) + capacity);
        যদি (en->baseContext == nullptr) {
            ফেরত nullptr;
        }
        static_cast<UEnum_বাফার*>(en->baseContext)->len = capacity;
    }
    
    ফেরত static_cast<শূন্য*>(&static_cast<UEnum_বাফার*>(en->baseContext)->তথ্য);
}

U_CAPI শূন্য U_EXPORT2
uenum_close(UEnumeration* en)
{
    যদি (en) {
        যদি (en->close != nullptr) {
            যদি (en->baseContext) {
                uprv_free(en->baseContext);
            }
            en->close(en);
        } অন্যথা { /* this seems dangerous, but we better kill the object */
            uprv_free(en);
        }
    }
}

U_CAPI int32_t U_EXPORT2
uenum_গুনতি(UEnumeration* en, UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ফেরত -1;
    }
    যদি (en->count != nullptr) {
        ফেরত en->count(en, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ফেরত -1;
    }
}

/* Don't call this directly. Only uenum_unext should be calling this. */
U_CAPI ধ্রুবক char16_t* U_EXPORT2
uenum_unextDefault(UEnumeration* en U_LIFETIME_BOUND,
                   int32_t* ফলাফল_দৈর্ঘ্য,
                   UErrorCode* status)
{
    char16_t *ustr = nullptr;
    int32_t len = 0;
    যদি (en->next != nullptr) {
        ধ্রুবক অক্ষর *cstr = en->next(en, &len, status);
        যদি (cstr != nullptr) {
            ustr = (char16_t*) get_বাফার_2(en, (len+1) * আকার(char16_t));
            যদি (ustr == nullptr) {
                *status = U_MEMORY_ALLOCATION_ERROR;
            } অন্যথা {
                u_charsToUChars(cstr, ustr, len);
                ustr[len] = 0;
            }
        }
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
    }
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = len;
    }
    ফেরত ustr;
}

/* Don't call this directly. Only uenum_next should be calling this. */
U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uenum_পরবর্তী_Default(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ফলাফল_দৈর্ঘ্য,
                  UErrorCode* status)
{
    যদি (en->u_পরবর্তী != nullptr) {
        অক্ষর *অস্থায়ী_Char_Val;
        ধ্রুবক char16_t *অস্থায়ী_UChar_Val = en->u_পরবর্তী(en, ফলাফল_দৈর্ঘ্য, status);
        যদি (অস্থায়ী_UChar_Val == nullptr) {
            ফেরত nullptr;
        }
        অস্থায়ী_Char_Val = (অক্ষর*)
            get_বাফার_2(en, (*ফলাফল_দৈর্ঘ্য+1) * আকার(অক্ষর));
        যদি (!অস্থায়ী_Char_Val) {
            *status = U_MEMORY_ALLOCATION_ERROR;
            ফেরত nullptr;
        }
        u_UCharsToChars(অস্থায়ী_UChar_Val, অস্থায়ী_Char_Val, *ফলাফল_দৈর্ঘ্য);
        অস্থায়ী_Char_Val[*ফলাফল_দৈর্ঘ্য] = 0;
        ফেরত অস্থায়ী_Char_Val;
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }
}

U_CAPI ধ্রুবক char16_t* U_EXPORT2
uenum_unext(UEnumeration* en U_LIFETIME_BOUND,
            int32_t* ফলাফল_দৈর্ঘ্য,
            UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (en->u_পরবর্তী != nullptr) {
        ফেরত en->u_পরবর্তী(en, ফলাফল_দৈর্ঘ্য, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }
}

U_CAPI ধ্রুবক অক্ষর* U_EXPORT2
uenum_পরবর্তী(UEnumeration* en U_LIFETIME_BOUND,
           int32_t* ফলাফল_দৈর্ঘ্য,
           UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ফেরত nullptr;
    }
    যদি (en->next != nullptr) {
        যদি (ফলাফল_দৈর্ঘ্য != nullptr) {
            ফেরত en->next(en, ফলাফল_দৈর্ঘ্য, status);
        }
        অন্যথা {
            int32_t dummy_দৈর্ঘ্য=0;
            ফেরত en->next(en, &dummy_দৈর্ঘ্য, status);
        }
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
        ফেরত nullptr;
    }
}

U_CAPI শূন্য U_EXPORT2
uenum_reset(UEnumeration* en, UErrorCode* status)
{
    যদি (!en || U_FAILURE(*status)) {
        ফেরত;
    }
    যদি (en->reset != nullptr) {
        en->reset(en, status);
    } অন্যথা {
        *status = U_UNSUPPORTED_ERROR;
    }
}
