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
    : chars(charsBuffer), charsCapacity(পরিমাণ(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    যদি (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ধ্রুব {
  প্রত্যাগম nullptr;
}

ধ্রুব বর্ণ*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ধ্রুব UnicodeString *s=snext(status);
    যদি(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        যদি(U_SUCCESS(status)) {
            যদি(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            প্রত্যাগম chars;
        }
    }

    প্রত্যাগম nullptr;
}

ধ্রুব char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ধ্রুব UnicodeString *s=snext(status);
    যদি(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        যদি(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        প্রত্যাগম unistr.getTerminatedBuffer();
    }

    প্রত্যাগম nullptr;
}

ধ্রুব UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ধ্রুব বর্ণ *s=next(&length, status);
    প্রত্যাগম setChars(s, length, status);
}

শূন্য
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    যদি(U_SUCCESS(status) && capacity>charsCapacity) {
        যদি(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        যদি(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<বর্ণ*>(uprv_malloc(capacity));
        যদি(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=পরিমাণ(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ধ্রুব বর্ণ* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    যদি(U_SUCCESS(status) && s!=nullptr) {
        যদি(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        যদি(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            প্রত্যাগম &unistr;
        } অন্যথা {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    প্রত্যাগম nullptr;
}
তার্কিক
StringEnumeration::operator==(ধ্রুব StringEnumeration& that)ধ্রুব {
    প্রত্যাগম typeid(*এতত্) == typeid(that); 
}

তার্কিক
StringEnumeration::operator!=(ধ্রুব StringEnumeration& that)ধ্রুব {
    প্রত্যাগম !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    যদি (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        প্রত্যাগম nullptr;
    }
    UStringEnumeration *result = নূতন UStringEnumeration(uenumToAdopt);
    যদি (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ধ্রুব {
    প্রত্যাগম uenum_গুনতি(uenum, &status);
}

ধ্রুব বর্ণ* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    প্রত্যাগম uenum_next(uenum, resultLength, &status);
}

ধ্রুব UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ধ্রুব char16_t* str = uenum_unext(uenum, &length, &status);
    যদি (str == nullptr || U_FAILURE(status)) {
        প্রত্যাগম nullptr;
    }
    প্রত্যাগম &unistr.setTo(str, length);
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
    প্রত্যাগম THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির ধ্রুব char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    প্রত্যাগম THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
স্থির ধ্রুব বর্ণ* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    প্রত্যাগম THIS(en)->next(resultLength, *ec);
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
স্থির ধ্রুব UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_গুনতি,
    ustrenum_unext,
    ustrenum_next,
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
    UEnumeration* result = nullptr;
    যদি (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(পরিমাণ(UEnumeration));
        যদি (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            uprv_memcpy(result, &USTRENUM_VT, পরিমাণ(USTRENUM_VT));
            result->context = adopted;
        }
    }
    যদি (result == nullptr) {
        delete adopted;
    }
    প্রত্যাগম result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

প্রকারনির্দেশ সংরচনা UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

স্থির শূন্য U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

স্থির int32_t U_CALLCONV
ucharstrenum_গুনতি(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    প্রত্যাগম ((UCharStringEnumeration*)en)->count;
}

স্থির ধ্রুব char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    যদি (e->index >= e->count) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব char16_t* result = ((ধ্রুব char16_t**)e->uenum.context)[e->index++];
    যদি (resultLength) {
        *resultLength = u_strlen(result);
    }
    প্রত্যাগম result;
}


স্থির ধ্রুব বর্ণ* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    যদি (e->index >= e->count) {
        প্রত্যাগম nullptr;
    }
    ধ্রুব বর্ণ* result = ((ধ্রুব বর্ণ**)e->uenum.context)[e->index++];
    যদি (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    প্রত্যাগম result;
}

স্থির শূন্য U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

স্থির ধ্রুব UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_গুনতি,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

স্থির ধ্রুব UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_গুনতি,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ধ্রুব বর্ণ* ধ্রুব strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    যদি (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(পরিমাণ(UCharStringEnumeration));
        যদি (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            U_ASSERT((বর্ণ*)result==(বর্ণ*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, পরিমাণ(UCHARSTRENUM_VT));
            result->uenum.context = (শূন্য*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    প্রত্যাগম (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ধ্রুব char16_t* ধ্রুব strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    যদি (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(পরিমাণ(UCharStringEnumeration));
        যদি (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } অন্যথা {
            U_ASSERT((বর্ণ*)result==(বর্ণ*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, পরিমাণ(UCHARSTRENUM_U_VT));
            result->uenum.context = (শূন্য*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    প্রত্যাগম (UEnumeration*) result;
}


// end C Wrapper
