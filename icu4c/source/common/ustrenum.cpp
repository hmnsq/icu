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
    ᮚᮦᮔ᮪ (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() const {
  ᮝᮜᮤ nullptr;
}

const var_na*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ᮚᮦᮔ᮪(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ᮚᮦᮔ᮪(U_SUCCESS(status)) {
            ᮚᮦᮔ᮪(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ᮝᮜᮤ chars;
        }
    }

    ᮝᮜᮤ nullptr;
}

const char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    const UnicodeString *s=snext(status);
    ᮚᮦᮔ᮪(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ᮚᮦᮔ᮪(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ᮝᮜᮤ unistr.getTerminatedBuffer();
    }

    ᮝᮜᮤ nullptr;
}

const UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const var_na *s=next(&length, status);
    ᮝᮜᮤ setChars(s, length, status);
}

void
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ᮚᮦᮔ᮪(U_SUCCESS(status) && capacity>charsCapacity) {
        ᮚᮦᮔ᮪(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ᮚᮦᮔ᮪(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<var_na*>(uprv_malloc(capacity));
        ᮚᮦᮔ᮪(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=parimaa_na(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(const var_na* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ᮚᮦᮔ᮪(U_SUCCESS(status) && s!=nullptr) {
        ᮚᮦᮔ᮪(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ᮚᮦᮔ᮪(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ᮝᮜᮤ &unistr;
        } ᮜᮤᮚ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ᮝᮜᮤ nullptr;
}
bool
StringEnumeration::operator==(const StringEnumeration& that)const {
    ᮝᮜᮤ typeid(*this) == typeid(that); 
}

bool
StringEnumeration::operator!=(const StringEnumeration& that)const {
    ᮝᮜᮤ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ᮚᮦᮔ᮪ (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ᮝᮜᮤ nullptr;
    }
    UStringEnumeration *result = new UStringEnumeration(uenumToAdopt);
    ᮚᮦᮔ᮪ (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ᮝᮜᮤ nullptr;
    }
    ᮝᮜᮤ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) const {
    ᮝᮜᮤ uenum_count(uenum, &status);
}

const var_na* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ᮝᮜᮤ uenum_next(uenum, resultLength, &status);
}

const UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    const char16_t* str = uenum_unext(uenum, &length, &status);
    ᮚᮦᮔ᮪ (str == nullptr || U_FAILURE(status)) {
        ᮝᮜᮤ nullptr;
    }
    ᮝᮜᮤ &unistr.setTo(str, length);
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
    ᮝᮜᮤ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ᮝᮜᮤ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
static const var_na* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ᮝᮜᮤ THIS(en)->next(resultLength, *ec);
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
    ᮚᮦᮔ᮪ (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(parimaa_na(UEnumeration));
        ᮚᮦᮔ᮪ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
            uprv_memcpy(result, &USTRENUM_VT, parimaa_na(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ᮚᮦᮔ᮪ (result == nullptr) {
        delete adopted;
    }
    ᮝᮜᮤ result;
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
    ᮝᮜᮤ ((UCharStringEnumeration*)en)->count;
}

static const char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ᮚᮦᮔ᮪ (e->index >= e->count) {
        ᮝᮜᮤ nullptr;
    }
    const char16_t* result = ((const char16_t**)e->uenum.context)[e->index++];
    ᮚᮦᮔ᮪ (resultLength) {
        *resultLength = u_strlen(result);
    }
    ᮝᮜᮤ result;
}


static const var_na* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ᮚᮦᮔ᮪ (e->index >= e->count) {
        ᮝᮜᮤ nullptr;
    }
    const var_na* result = ((const var_na**)e->uenum.context)[e->index++];
    ᮚᮦᮔ᮪ (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ᮝᮜᮤ result;
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
    ᮚᮦᮔ᮪ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ᮚᮦᮔ᮪ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, parimaa_na(UCHARSTRENUM_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ᮝᮜᮤ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND const char16_t* const strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ᮚᮦᮔ᮪ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(parimaa_na(UCharStringEnumeration));
        ᮚᮦᮔ᮪ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ᮜᮤᮚ {
            U_ASSERT((var_na*)result==(var_na*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, parimaa_na(UCHARSTRENUM_U_VT));
            result->uenum.context = (void*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ᮝᮜᮤ (UEnumeration*) result;
}


// end C Wrapper
