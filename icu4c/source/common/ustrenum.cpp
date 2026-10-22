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
#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work 

#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/strenum.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "uenumimp.h"
#ಅನ್ತರ್ಭಾವ "ustrenum.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(ಪರಿಮಾಣ(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    ಯದಿ (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ಧ್ರುವ {
  ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಧ್ರುವ ವರ್ಣ*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ಧ್ರುವ UnicodeString *s=snext(status);
    ಯದಿ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ಯದಿ(U_SUCCESS(status)) {
            ಯದಿ(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ಪ್ರತ್ಯಾಗಮ chars;
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಧ್ರುವ char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ಧ್ರುವ UnicodeString *s=snext(status);
    ಯದಿ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ಯದಿ(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ಪ್ರತ್ಯಾಗಮ unistr.getTerminatedBuffer();
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}

ಧ್ರುವ UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ಧ್ರುವ ವರ್ಣ *s=next(&length, status);
    ಪ್ರತ್ಯಾಗಮ setChars(s, length, status);
}

ಶೂನ್ಯ
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ಯದಿ(U_SUCCESS(status) && capacity>charsCapacity) {
        ಯದಿ(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ಯದಿ(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<ವರ್ಣ*>(uprv_malloc(capacity));
        ಯದಿ(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=ಪರಿಮಾಣ(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ಧ್ರುವ ವರ್ಣ* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ಯದಿ(U_SUCCESS(status) && s!=nullptr) {
        ಯದಿ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ಯದಿ(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ಪ್ರತ್ಯಾಗಮ &unistr;
        } ಅನ್ಯಥಾ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ಪ್ರತ್ಯಾಗಮ nullptr;
}
ತಾರ್ಕಿಕ
StringEnumeration::operator==(ಧ್ರುವ StringEnumeration& that)ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ typeid(*ಏತತ್) == typeid(that); 
}

ತಾರ್ಕಿಕ
StringEnumeration::operator!=(ಧ್ರುವ StringEnumeration& that)ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ಯದಿ (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    UStringEnumeration *result = ನೂತನ UStringEnumeration(uenumToAdopt);
    ಯದಿ (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ಧ್ರುವ {
    ಪ್ರತ್ಯಾಗಮ uenum_ಎಣಿಕೆ(uenum, &status);
}

ಧ್ರುವ ವರ್ಣ* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ಪ್ರತ್ಯಾಗಮ uenum_next(uenum, resultLength, &status);
}

ಧ್ರುವ UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ಧ್ರುವ char16_t* str = uenum_unext(uenum, &length, &status);
    ಯದಿ (str == nullptr || U_FAILURE(status)) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಪ್ರತ್ಯಾಗಮ &unistr.setTo(str, length);
}

ಶೂನ್ಯ UStringEnumeration::reset(UErrorCode& status) {
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
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ int32_t U_CALLCONV
ustrenum_ಎಣಿಕೆ(UEnumeration* en,
               UErrorCode* ec)
{
    ಪ್ರತ್ಯಾಗಮ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಧ್ರುವ char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ಪ್ರತ್ಯಾಗಮ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ಪ್ರತ್ಯಾಗಮ THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ಸ್ಥಿರ ಧ್ರುವ UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ಎಣಿಕೆ,
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
    ಯದಿ (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(ಪರಿಮಾಣ(UEnumeration));
        ಯದಿ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            uprv_memcpy(result, &USTRENUM_VT, ಪರಿಮಾಣ(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ಯದಿ (result == nullptr) {
        delete adopted;
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ಸ್ಥಿರ int32_t U_CALLCONV
ucharstrenum_ಎಣಿಕೆ(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ಪ್ರತ್ಯಾಗಮ ((UCharStringEnumeration*)en)->count;
}

ಸ್ಥಿರ ಧ್ರುವ char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ಯದಿ (e->index >= e->count) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ char16_t* result = ((ಧ್ರುವ char16_t**)e->uenum.context)[e->index++];
    ಯದಿ (resultLength) {
        *resultLength = u_strlen(result);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}


ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ಯದಿ (e->index >= e->count) {
        ಪ್ರತ್ಯಾಗಮ nullptr;
    }
    ಧ್ರುವ ವರ್ಣ* result = ((ಧ್ರುವ ವರ್ಣ**)e->uenum.context)[e->index++];
    ಯದಿ (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಶೂನ್ಯ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

ಸ್ಥಿರ ಧ್ರುವ UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ಎಣಿಕೆ,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

ಸ್ಥಿರ ಧ್ರುವ UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ಎಣಿಕೆ,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ಯದಿ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(ಪರಿಮಾಣ(UCharStringEnumeration));
        ಯದಿ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            U_ASSERT((ವರ್ಣ*)result==(ವರ್ಣ*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, ಪರಿಮಾಣ(UCHARSTRENUM_VT));
            result->uenum.context = (ಶೂನ್ಯ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ಪ್ರತ್ಯಾಗಮ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ಧ್ರುವ char16_t* ಧ್ರುವ strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ಯದಿ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(ಪರಿಮಾಣ(UCharStringEnumeration));
        ಯದಿ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ಅನ್ಯಥಾ {
            U_ASSERT((ವರ್ಣ*)result==(ವರ್ಣ*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, ಪರಿಮಾಣ(UCHARSTRENUM_U_VT));
            result->uenum.context = (ಶೂನ್ಯ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ಪ್ರತ್ಯಾಗಮ (UEnumeration*) result;
}


// end C Wrapper
