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
#شامل "utypeinfo.h"  // for 'typeid' to work 

#شامل "unicode/ustring.h"
#شامل "unicode/strenum.h"
#شامل "unicode/putil.h"
#شامل "uenumimp.h"
#شامل "ustrenum.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(اندازه(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    اگر (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  بازگشت nullptr;
}

ثابت نویسه*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(status);
    اگر(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        اگر(U_SUCCESS(status)) {
            اگر(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            بازگشت chars;
        }
    }

    بازگشت nullptr;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(status);
    اگر(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        اگر(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        بازگشت unistr.getTerminatedBuffer();
    }

    بازگشت nullptr;
}

ثابت UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ثابت نویسه *s=next(&length, status);
    بازگشت setChars(s, length, status);
}

تهی
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    اگر(U_SUCCESS(status) && capacity>charsCapacity) {
        اگر(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        اگر(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<نویسه*>(uprv_malloc(capacity));
        اگر(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=اندازه(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ثابت نویسه* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    اگر(U_SUCCESS(status) && s!=nullptr) {
        اگر(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        اگر(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            بازگشت &unistr;
        } وگرنه {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    بازگشت nullptr;
}
منطقی
StringEnumeration::operator==(ثابت StringEnumeration& that)ثابت {
    بازگشت typeid(*این) == typeid(that); 
}

منطقی
StringEnumeration::operator!=(ثابت StringEnumeration& that)ثابت {
    بازگشت !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    اگر (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        بازگشت nullptr;
    }
    UStringEnumeration *result = جدید UStringEnumeration(uenumToAdopt);
    اگر (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        بازگشت nullptr;
    }
    بازگشت result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ثابت {
    بازگشت uenum_شمار(uenum, &status);
}

ثابت نویسه* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    بازگشت uenum_next(uenum, resultLength, &status);
}

ثابت UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &status);
    اگر (str == nullptr || U_FAILURE(status)) {
        بازگشت nullptr;
    }
    بازگشت &unistr.setTo(str, length);
}

تهی UStringEnumeration::reset(UErrorCode& status) {
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
ایستا تهی U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ایستا int32_t U_CALLCONV
ustrenum_شمار(UEnumeration* en,
               UErrorCode* ec)
{
    بازگشت THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ایستا ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    بازگشت THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ایستا ثابت نویسه* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    بازگشت THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ایستا تهی U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ایستا ثابت UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_شمار,
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
    اگر (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(اندازه(UEnumeration));
        اگر (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            uprv_memcpy(result, &USTRENUM_VT, اندازه(USTRENUM_VT));
            result->context = adopted;
        }
    }
    اگر (result == nullptr) {
        delete adopted;
    }
    بازگشت result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

تعریفنوع ساختار UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

ایستا تهی U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ایستا int32_t U_CALLCONV
ucharstrenum_شمار(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    بازگشت ((UCharStringEnumeration*)en)->count;
}

ایستا ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    اگر (e->index >= e->count) {
        بازگشت nullptr;
    }
    ثابت char16_t* result = ((ثابت char16_t**)e->uenum.context)[e->index++];
    اگر (resultLength) {
        *resultLength = u_strlen(result);
    }
    بازگشت result;
}


ایستا ثابت نویسه* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    اگر (e->index >= e->count) {
        بازگشت nullptr;
    }
    ثابت نویسه* result = ((ثابت نویسه**)e->uenum.context)[e->index++];
    اگر (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    بازگشت result;
}

ایستا تهی U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

ایستا ثابت UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمار,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

ایستا ثابت UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمار,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ثابت نویسه* ثابت strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    اگر (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(اندازه(UCharStringEnumeration));
        اگر (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            U_ASSERT((نویسه*)result==(نویسه*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, اندازه(UCHARSTRENUM_VT));
            result->uenum.context = (تهی*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    بازگشت (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    اگر (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(اندازه(UCharStringEnumeration));
        اگر (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } وگرنه {
            U_ASSERT((نویسه*)result==(نویسه*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, اندازه(UCHARSTRENUM_U_VT));
            result->uenum.context = (تهی*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    بازگشت (UEnumeration*) result;
}


// end C Wrapper
