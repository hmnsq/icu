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
    ᬬᬾᬦ᭄ (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() const {
  ᬯᬮᬶ nullptr;
}

const var_na*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ᬬᬾᬦ᭄(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ᬬᬾᬦ᭄(U_SUCCESS(status)) {
            ᬬᬾᬦ᭄(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ᬯᬮᬶ chars;
        }
    }

    ᬯᬮᬶ nullptr;
}

const char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ᬬᬾᬦ᭄(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ᬬᬾᬦ᭄(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ᬯᬮᬶ unistr.getTerminatedBuffer();
    }

    ᬯᬮᬶ nullptr;
}

const UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const var_na *s=next(&length, status);
    ᬯᬮᬶ setChars(s, length, status);
}

void
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ᬬᬾᬦ᭄(U_SUCCESS(status) && capacity>charsCapacity) {
        ᬬᬾᬦ᭄(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ᬬᬾᬦ᭄(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<var_na*>(uprv_malloc(capacity));
        ᬬᬾᬦ᭄(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=parimaa_na(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(const var_na* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ᬬᬾᬦ᭄(U_SUCCESS(status) && s!=nullptr) {
        ᬬᬾᬦ᭄(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ᬬᬾᬦ᭄(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ᬯᬮᬶ &unistr;
        } ᬮᬶᬬ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ᬯᬮᬶ nullptr;
}
bool
StringEnumeration::operator==(const StringEnumeration& that)const {
    ᬯᬮᬶ typeid(*this) == typeid(that); 
}

bool
StringEnumeration::operator!=(const StringEnumeration& that)const {
    ᬯᬮᬶ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ᬬᬾᬦ᭄ (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ᬯᬮᬶ nullptr;
    }
    UStringEnumeration *result = new UStringEnumeration(uenumToAdopt);
    ᬬᬾᬦ᭄ (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ᬯᬮᬶ nullptr;
    }
    ᬯᬮᬶ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) const {
    ᬯᬮᬶ uenum_count(uenum, &status);
}

const var_na* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ᬯᬮᬶ uenum_next(uenum, resultLength, &status);
}

const UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const char16_t* str = uenum_unext(uenum, &length, &status);
    ᬬᬾᬦ᭄ (str == nullptr || U_FAILURE(status)) {
        ᬯᬮᬶ nullptr;
    }
    ᬯᬮᬶ &unistr.setTo(str, length);
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
    ᬯᬮᬶ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ᬯᬮᬶ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const var_na* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ᬯᬮᬶ THIS(en)->next(resultLength, *ec);
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
    ᬬᬾᬦ᭄ (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(parimaa_na(UEnumeration));
        ᬬᬾᬦ᭄ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ {
            uprv_memcpy(result, &USTRENUM_VT, parimaa_na(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ᬬᬾᬦ᭄ (result == nullptr) {
        delete adopted;
    }
    ᬯᬮᬶ result;
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
    ᬯᬮᬶ ((UCharStringEnumeration*)en)->count;
}

static const char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ᬬᬾᬦ᭄ (e->index >= e->count) {
        ᬯᬮᬶ nullptr;
    }
    const char16_t* result = ((const char16_t**)e->uenum.context)[e->index++];
    ᬬᬾᬦ᭄ (resultLength) {
        *resultLength = u_strlen(result);
    }
    ᬯᬮᬶ result;
}


static const var_na* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ᬬᬾᬦ᭄ (e->index >= e->count) {
        ᬯᬮᬶ nullptr;
    }
    const var_na* result = ((const var_na**)e->uenum.context)[e->index++];
    ᬬᬾᬦ᭄ (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ᬯᬮᬶ result;
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
    ᬬᬾᬦ᭄ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ᬬᬾᬦ᭄ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, parimaa_na(UCHARSTRENUM_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ᬯᬮᬶ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND const char16_t* const strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ᬬᬾᬦ᭄ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ᬬᬾᬦ᭄ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᬮᬶᬬ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, parimaa_na(UCHARSTRENUM_U_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ᬯᬮᬶ (UEnumeration*) result;
}


// end C Wrapper
