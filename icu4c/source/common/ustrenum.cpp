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
#அந்தர்பாவ "utypeinfo.h"  // for 'typeid' to work 

#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/strenum.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "uenumimp.h"
#அந்தர்பாவ "ustrenum.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(அளவு(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    எனில் (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() மாறிலி {
  திரும்பு nullptr;
}

மாறிலி எழுத்து*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    மாறிலி UnicodeString *s=snext(status);
    எனில்(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        எனில்(U_SUCCESS(status)) {
            எனில்(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            திரும்பு chars;
        }
    }

    திரும்பு nullptr;
}

மாறிலி char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    மாறிலி UnicodeString *s=snext(status);
    எனில்(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        எனில்(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        திரும்பு unistr.getTerminatedBuffer();
    }

    திரும்பு nullptr;
}

மாறிலி UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    மாறிலி எழுத்து *s=next(&length, status);
    திரும்பு setChars(s, length, status);
}

வெறுமை
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    எனில்(U_SUCCESS(status) && capacity>charsCapacity) {
        எனில்(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        எனில்(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<எழுத்து*>(uprv_malloc(capacity));
        எனில்(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=அளவு(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(மாறிலி எழுத்து* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    எனில்(U_SUCCESS(status) && s!=nullptr) {
        எனில்(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        எனில்(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            திரும்பு &unistr;
        } இல்லையெனில் {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    திரும்பு nullptr;
}
தருக்க
StringEnumeration::operator==(மாறிலி StringEnumeration& that)மாறிலி {
    திரும்பு typeid(*இது) == typeid(that); 
}

தருக்க
StringEnumeration::operator!=(மாறிலி StringEnumeration& that)மாறிலி {
    திரும்பு !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    எனில் (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        திரும்பு nullptr;
    }
    UStringEnumeration *result = புதிய UStringEnumeration(uenumToAdopt);
    எனில் (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        திரும்பு nullptr;
    }
    திரும்பு result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) மாறிலி {
    திரும்பு uenum_மொத்தம்(uenum, &status);
}

மாறிலி எழுத்து* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    திரும்பு uenum_next(uenum, resultLength, &status);
}

மாறிலி UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    மாறிலி char16_t* str = uenum_unext(uenum, &length, &status);
    எனில் (str == nullptr || U_FAILURE(status)) {
        திரும்பு nullptr;
    }
    திரும்பு &unistr.setTo(str, length);
}

வெறுமை UStringEnumeration::reset(UErrorCode& status) {
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
நிலையான வெறுமை U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
நிலையான int32_t U_CALLCONV
ustrenum_மொத்தம்(UEnumeration* en,
               UErrorCode* ec)
{
    திரும்பு THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
நிலையான மாறிலி char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    திரும்பு THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
நிலையான மாறிலி எழுத்து* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    திரும்பு THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
நிலையான வெறுமை U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
நிலையான மாறிலி UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_மொத்தம்,
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
    எனில் (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(அளவு(UEnumeration));
        எனில் (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            uprv_memcpy(result, &USTRENUM_VT, அளவு(USTRENUM_VT));
            result->context = adopted;
        }
    }
    எனில் (result == nullptr) {
        delete adopted;
    }
    திரும்பு result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

வகைவரை கட்டமைப்பு UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

நிலையான வெறுமை U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

நிலையான int32_t U_CALLCONV
ucharstrenum_மொத்தம்(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    திரும்பு ((UCharStringEnumeration*)en)->count;
}

நிலையான மாறிலி char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    எனில் (e->index >= e->count) {
        திரும்பு nullptr;
    }
    மாறிலி char16_t* result = ((மாறிலி char16_t**)e->uenum.context)[e->index++];
    எனில் (resultLength) {
        *resultLength = u_strlen(result);
    }
    திரும்பு result;
}


நிலையான மாறிலி எழுத்து* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    எனில் (e->index >= e->count) {
        திரும்பு nullptr;
    }
    மாறிலி எழுத்து* result = ((மாறிலி எழுத்து**)e->uenum.context)[e->index++];
    எனில் (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    திரும்பு result;
}

நிலையான வெறுமை U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

நிலையான மாறிலி UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_மொத்தம்,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

நிலையான மாறிலி UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_மொத்தம்,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND மாறிலி எழுத்து* மாறிலி strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    எனில் (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(அளவு(UCharStringEnumeration));
        எனில் (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            U_ASSERT((எழுத்து*)result==(எழுத்து*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, அளவு(UCHARSTRENUM_VT));
            result->uenum.context = (வெறுமை*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    திரும்பு (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND மாறிலி char16_t* மாறிலி strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    எனில் (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(அளவு(UCharStringEnumeration));
        எனில் (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } இல்லையெனில் {
            U_ASSERT((எழுத்து*)result==(எழுத்து*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, அளவு(UCHARSTRENUM_U_VT));
            result->uenum.context = (வெறுமை*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    திரும்பு (UEnumeration*) result;
}


// end C Wrapper
