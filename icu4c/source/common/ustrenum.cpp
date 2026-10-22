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
#include "utypeinfo.h"  // for 'typeid' to work 

#include "unicode/ustring.h"
#include "unicode/strenum.h"
#include "unicode/putil.h"
#include "uenumimp.h"
#include "ustrenum.h"
#include "cstring.h"
#include "cmemory.h"
#include "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(parimaa_na(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    ပေႃး (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() const {
  ပွၵ်ႈ nullptr;
}

const var_na*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ပေႃး(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ပေႃး(U_SUCCESS(status)) {
            ပေႃး(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ပွၵ်ႈ chars;
        }
    }

    ပွၵ်ႈ nullptr;
}

const char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ပေႃး(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ပေႃး(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ပွၵ်ႈ unistr.getTerminatedBuffer();
    }

    ပွၵ်ႈ nullptr;
}

const UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const var_na *s=next(&length, status);
    ပွၵ်ႈ setChars(s, length, status);
}

void
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ပေႃး(U_SUCCESS(status) && capacity>charsCapacity) {
        ပေႃး(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ပေႃး(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<var_na*>(uprv_malloc(capacity));
        ပေႃး(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=parimaa_na(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ပေႃးဢမ်ႇ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(const var_na* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ပေႃး(U_SUCCESS(status) && s!=nullptr) {
        ပေႃး(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ပေႃး(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ပွၵ်ႈ &unistr;
        } ပေႃးဢမ်ႇ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ပွၵ်ႈ nullptr;
}
bool
StringEnumeration::operator==(const StringEnumeration& that)const {
    ပွၵ်ႈ typeid(*this) == typeid(that); 
}

bool
StringEnumeration::operator!=(const StringEnumeration& that)const {
    ပွၵ်ႈ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ပေႃး (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ပွၵ်ႈ nullptr;
    }
    UStringEnumeration *result = new UStringEnumeration(uenumToAdopt);
    ပေႃး (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ပွၵ်ႈ nullptr;
    }
    ပွၵ်ႈ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) const {
    ပွၵ်ႈ uenum_count(uenum, &status);
}

const var_na* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ပွၵ်ႈ uenum_next(uenum, resultLength, &status);
}

const UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const char16_t* str = uenum_unext(uenum, &length, &status);
    ပေႃး (str == nullptr || U_FAILURE(status)) {
        ပွၵ်ႈ nullptr;
    }
    ပွၵ်ႈ &unistr.setTo(str, length);
}

void UStringEnumeration::reset(UErrorCode& status) {
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
static void U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static int32_t U_CALLCONV
ustrenum_count(UEnumeration* en,
               UErrorCode* ec)
{
    ပွၵ်ႈ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ပွၵ်ႈ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const var_na* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ပွၵ်ႈ THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static void U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
static const UEnumeration USTRENUM_VT = {
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
    ပေႃး (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(parimaa_na(UEnumeration));
        ပေႃး (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ပေႃးဢမ်ႇ {
            uprv_memcpy(result, &USTRENUM_VT, parimaa_na(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ပေႃး (result == nullptr) {
        delete adopted;
    }
    ပွၵ်ႈ result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

typedef sa_nracanaa UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

static void U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

static int32_t U_CALLCONV
ucharstrenum_count(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ပွၵ်ႈ ((UCharStringEnumeration*)en)->count;
}

static const char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ပေႃး (e->index >= e->count) {
        ပွၵ်ႈ nullptr;
    }
    const char16_t* result = ((const char16_t**)e->uenum.context)[e->index++];
    ပေႃး (resultLength) {
        *resultLength = u_strlen(result);
    }
    ပွၵ်ႈ result;
}


static const var_na* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ပေႃး (e->index >= e->count) {
        ပွၵ်ႈ nullptr;
    }
    const var_na* result = ((const var_na**)e->uenum.context)[e->index++];
    ပေႃး (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ပွၵ်ႈ result;
}

static void U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

static const UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_count,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

static const UEnumeration UCHARSTRENUM_U_VT = {
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
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND const var_na* const strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ပေႃး (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ပေႃး (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ပေႃးဢမ်ႇ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, parimaa_na(UCHARSTRENUM_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ပွၵ်ႈ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND const char16_t* const strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ပေႃး (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ပေႃး (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ပေႃးဢမ်ႇ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, parimaa_na(UCHARSTRENUM_U_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ပွၵ်ႈ (UEnumeration*) result;
}


// end C Wrapper
