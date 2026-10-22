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
#ضمن "utypeinfo.h"  // for 'typeid' to work 

#ضمن "unicode/ustring.h"
#ضمن "unicode/strenum.h"
#ضمن "unicode/putil.h"
#ضمن "uenumimp.h"
#ضمن "ustrenum.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(حجم(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    ܐܢ (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  ܦܢܐ nullptr;
}

ثابت حرف*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(status);
    ܐܢ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ܐܢ(U_SUCCESS(status)) {
            ܐܢ(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ܦܢܐ chars;
        }
    }

    ܦܢܐ nullptr;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(status);
    ܐܢ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ܐܢ(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ܦܢܐ unistr.getTerminatedBuffer();
    }

    ܦܢܐ nullptr;
}

ثابت UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, status);
    ܦܢܐ setChars(s, length, status);
}

فراغ
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ܐܢ(U_SUCCESS(status) && capacity>charsCapacity) {
        ܐܢ(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ܐܢ(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        ܐܢ(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=حجم(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ثابت حرف* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ܐܢ(U_SUCCESS(status) && s!=nullptr) {
        ܐܢ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ܐܢ(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ܦܢܐ &unistr;
        } ܐܚܪܢܐ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ܦܢܐ nullptr;
}
منطقي
StringEnumeration::operator==(ثابت StringEnumeration& that)ثابت {
    ܦܢܐ typeid(*هذا) == typeid(that); 
}

منطقي
StringEnumeration::operator!=(ثابت StringEnumeration& that)ثابت {
    ܦܢܐ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ܐܢ (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ܦܢܐ nullptr;
    }
    UStringEnumeration *result = جديد UStringEnumeration(uenumToAdopt);
    ܐܢ (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ܦܢܐ nullptr;
    }
    ܦܢܐ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ثابت {
    ܦܢܐ uenum_count(uenum, &status);
}

ثابت حرف* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ܦܢܐ uenum_next(uenum, resultLength, &status);
}

ثابت UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &status);
    ܐܢ (str == nullptr || U_FAILURE(status)) {
        ܦܢܐ nullptr;
    }
    ܦܢܐ &unistr.setTo(str, length);
}

فراغ UStringEnumeration::reset(UErrorCode& status) {
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
ساكن فراغ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن int32_t U_CALLCONV
ustrenum_count(UEnumeration* en,
               UErrorCode* ec)
{
    ܦܢܐ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ܦܢܐ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت حرف* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ܦܢܐ THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساكن ثابت UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_count,
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
    ܐܢ (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        ܐܢ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ {
            uprv_memcpy(result, &USTRENUM_VT, حجم(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ܐܢ (result == nullptr) {
        delete adopted;
    }
    ܦܢܐ result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

تعريفنوع بنية UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

ساكن فراغ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ساكن int32_t U_CALLCONV
ucharstrenum_count(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ܦܢܐ ((UCharStringEnumeration*)en)->count;
}

ساكن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ܐܢ (e->index >= e->count) {
        ܦܢܐ nullptr;
    }
    ثابت char16_t* result = ((ثابت char16_t**)e->uenum.context)[e->index++];
    ܐܢ (resultLength) {
        *resultLength = u_strlen(result);
    }
    ܦܢܐ result;
}


ساكن ثابت حرف* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ܐܢ (e->index >= e->count) {
        ܦܢܐ nullptr;
    }
    ثابت حرف* result = ((ثابت حرف**)e->uenum.context)[e->index++];
    ܐܢ (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ܦܢܐ result;
}

ساكن فراغ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

ساكن ثابت UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_count,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

ساكن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_count,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ثابت حرف* ثابت strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ܐܢ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        ܐܢ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ {
            U_ASSERT((حرف*)result==(حرف*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            result->uenum.context = (فراغ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ܦܢܐ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ܐܢ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        ܐܢ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ܐܚܪܢܐ {
            U_ASSERT((حرف*)result==(حرف*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            result->uenum.context = (فراغ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ܦܢܐ (UEnumeration*) result;
}


// end C Wrapper
