// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
**********************************************************************
* Copyright (c) 2002-2014, International Business Machines
* Corporation and others.  All Rights Reserved.
**********************************************************************
* Author: Alan Liu
* Created: November 11 2002
* Since: ICU 2.4
**********************************************************************
*/
#অন্তর্ভাব "utypeinfo.h"  // for 'typeid' to work 

#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/strenum.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "uenumimp.h"
#অন্তর্ভাব "ustrenum.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_বাফার), charsCapacity(আকার(chars_বাফার)) {
}

StringEnumeration::~StringEnumeration() {
    যদি (chars != nullptr && chars != chars_বাফার) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ধ্রুবক {
  ফেরত nullptr;
}

ধ্রুবক অক্ষর*
StringEnumeration::next(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND {
    ধ্রুবক UnicodeString *s=snext(status);
    যদি(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        যদি(U_SUCCESS(status)) {
            যদি(ফলাফল_দৈর্ঘ্য!=nullptr) {
                *ফলাফল_দৈর্ঘ্য=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ফেরত chars;
        }
    }

    ফেরত nullptr;
}

ধ্রুবক char16_t*
StringEnumeration::unext(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND {
    ধ্রুবক UnicodeString *s=snext(status);
    যদি(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        যদি(ফলাফল_দৈর্ঘ্য!=nullptr) {
            *ফলাফল_দৈর্ঘ্য=unistr.length();
        }
        ফেরত unistr.get_Terminated_বাফার();
    }

    ফেরত nullptr;
}

ধ্রুবক UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ধ্রুবক অক্ষর *s=next(&length, status);
    ফেরত setChars(s, length, status);
}

শূন্য
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    যদি(U_SUCCESS(status) && capacity>charsCapacity) {
        যদি(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        যদি(chars!=chars_বাফার) {
            uprv_free(chars);
        }
        chars = static_cast<অক্ষর*>(uprv_malloc(capacity));
        যদি(chars==nullptr) {
            chars=chars_বাফার;
            charsCapacity=আকার(chars_বাফার);
            status=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ধ্রুবক অক্ষর* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    যদি(U_SUCCESS(status) && s!=nullptr) {
        যদি(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *বাফার=unistr.get_বাফার(length+1);
        যদি(বাফার!=nullptr) {
            u_charsToUChars(s, বাফার, length);
            বাফার[length]=0;
            unistr.release_বাফার(length);
            ফেরত &unistr;
        } অন্যথা {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ফেরত nullptr;
}
যৌক্তিক
StringEnumeration::operator==(ধ্রুবক StringEnumeration& that)ধ্রুবক {
    ফেরত typeid(*এই) == typeid(that); 
}

যৌক্তিক
StringEnumeration::operator!=(ধ্রুবক StringEnumeration& that)ধ্রুবক {
    ফেরত !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ফেরত nullptr;
    }
    UStringEnumeration *ফলাফল = নতুন UStringEnumeration(uenumToAdopt);
    যদি (ফলাফল == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ফেরত nullptr;
    }
    ফেরত ফলাফল;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ধ্রুবক {
    ফেরত uenum_গুনতি(uenum, &status);
}

ধ্রুবক অক্ষর* UStringEnumeration::next(int32_t* ফলাফল_দৈর্ঘ্য, UErrorCode& status) U_LIFETIME_BOUND {
    ফেরত uenum_পরবর্তী(uenum, ফলাফল_দৈর্ঘ্য, &status);
}

ধ্রুবক UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ধ্রুবক char16_t* str = uenum_unext(uenum, &length, &status);
    যদি (str == nullptr || U_FAILURE(status)) {
        ফেরত nullptr;
    }
    ফেরত &unistr.setTo(str, length);
}

শূন্য UStringEnumeration::reset(UErrorCode& status) {
    uenum_reset(uenum, &status);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_END

// C wrapper --------------------------------------------------------------- ***

#define THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির শূন্য U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির int32_t U_CALLCONV
ustrenum_গুনতি(UEnumeration* en,
               UErrorCode* ec)
{
    ফেরত THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির ধ্রুবক char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* ফলাফল_দৈর্ঘ্য,
               UErrorCode* ec)
{
    ফেরত THIS(en)->unext(ফলাফল_দৈর্ঘ্য, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির ধ্রুবক অক্ষর* U_CALLCONV
ustrenum_পরবর্তী(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* ফলাফল_দৈর্ঘ্য,
              UErrorCode* ec)
{
    ফেরত THIS(en)->next(ফলাফল_দৈর্ঘ্য, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির শূন্য U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
স্থির ধ্রুবক UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_গুনতি,
    ustrenum_unext,
    ustrenum_পরবর্তী,
    ustrenum_reset
};

U_CDECL_END

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UErrorCode* ec) { 
    UEnumeration* ফলাফল = nullptr;
    যদি (U_SUCCESS(*ec) && adopted != nullptr) {
        ফলাফল = (UEnumeration*) uprv_malloc(আকার(UEnumeration));
        যদি (ফলাফল == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uprv_memcpy(ফলাফল, &USTRENUM_VT, আকার(USTRENUM_VT));
            ফলাফল->context = adopted;
        }
    }
    যদি (ফলাফল == nullptr) {
        delete adopted;
    }
    ফেরত ফলাফল;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

প্রকারলেখ কাঠামো UCharStringEnumeration {
    UEnumeration uenum;
    int32_t সূচক, count;
} UCharStringEnumeration;

স্থির শূন্য U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

স্থির int32_t U_CALLCONV
ucharstrenum_গুনতি(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ফেরত ((UCharStringEnumeration*)en)->count;
}

স্থির ধ্রুবক char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ফলাফল_দৈর্ঘ্য,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    যদি (e->সূচক >= e->count) {
        ফেরত nullptr;
    }
    ধ্রুবক char16_t* ফলাফল = ((ধ্রুবক char16_t**)e->uenum.context)[e->সূচক++];
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = u_strlen(ফলাফল);
    }
    ফেরত ফলাফল;
}


স্থির ধ্রুবক অক্ষর* U_CALLCONV
ucharstrenum_পরবর্তী(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* ফলাফল_দৈর্ঘ্য,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    যদি (e->সূচক >= e->count) {
        ফেরত nullptr;
    }
    ধ্রুবক অক্ষর* ফলাফল = ((ধ্রুবক অক্ষর**)e->uenum.context)[e->সূচক++];
    যদি (ফলাফল_দৈর্ঘ্য) {
        *ফলাফল_দৈর্ঘ্য = (int32_t)uprv_strlen(ফলাফল);
    }
    ফেরত ফলাফল;
}

স্থির শূন্য U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->সূচক = 0;
}

স্থির ধ্রুবক UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_গুনতি,
    uenum_unextDefault,
    ucharstrenum_পরবর্তী,
    ucharstrenum_reset
};

স্থির ধ্রুবক UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_গুনতি,
    ucharstrenum_unext,
    uenum_পরবর্তী_Default,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ধ্রুবক অক্ষর* ধ্রুবক strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* ফলাফল = nullptr;
    যদি (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        ফলাফল = (UCharStringEnumeration*) uprv_malloc(আকার(UCharStringEnumeration));
        যদি (ফলাফল == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            U_ASSERT((অক্ষর*)ফলাফল==(অক্ষর*)(&ফলাফল->uenum));
            uprv_memcpy(ফলাফল, &UCHARSTRENUM_VT, আকার(UCHARSTRENUM_VT));
            ফলাফল->uenum.context = (শূন্য*)strings;
            ফলাফল->সূচক = 0;
            ফলাফল->count = count;
        }
    }
    ফেরত (UEnumeration*) ফলাফল;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ধ্রুবক char16_t* ধ্রুবক strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* ফলাফল = nullptr;
    যদি (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        ফলাফল = (UCharStringEnumeration*) uprv_malloc(আকার(UCharStringEnumeration));
        যদি (ফলাফল == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            U_ASSERT((অক্ষর*)ফলাফল==(অক্ষর*)(&ফলাফল->uenum));
            uprv_memcpy(ফলাফল, &UCHARSTRENUM_U_VT, আকার(UCHARSTRENUM_U_VT));
            ফলাফল->uenum.context = (শূন্য*)strings;
            ফলাফল->সূচক = 0;
            ফলাফল->count = count;
        }
    }
    ফেরত (UEnumeration*) ফলাফল;
}


// end C Wrapper
