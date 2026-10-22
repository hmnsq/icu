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
#അന്തര്ഭാവ "utypeinfo.h"  // for 'typeid' to work 

#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/strenum.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "uenumimp.h"
#അന്തര്ഭാവ "ustrenum.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(വലുപ്പം(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    എങ്കിൽ (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() സ്ഥിരം {
  തിരികെ nullptr;
}

സ്ഥിരം അക്ഷരം*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    സ്ഥിരം UnicodeString *s=snext(status);
    എങ്കിൽ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        എങ്കിൽ(U_SUCCESS(status)) {
            എങ്കിൽ(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            തിരികെ chars;
        }
    }

    തിരികെ nullptr;
}

സ്ഥിരം char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    സ്ഥിരം UnicodeString *s=snext(status);
    എങ്കിൽ(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        എങ്കിൽ(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        തിരികെ unistr.getTerminatedBuffer();
    }

    തിരികെ nullptr;
}

സ്ഥിരം UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    സ്ഥിരം അക്ഷരം *s=next(&length, status);
    തിരികെ setChars(s, length, status);
}

ശൂന്യം
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    എങ്കിൽ(U_SUCCESS(status) && capacity>charsCapacity) {
        എങ്കിൽ(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        എങ്കിൽ(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<അക്ഷരം*>(uprv_malloc(capacity));
        എങ്കിൽ(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=വലുപ്പം(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(സ്ഥിരം അക്ഷരം* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    എങ്കിൽ(U_SUCCESS(status) && s!=nullptr) {
        എങ്കിൽ(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        എങ്കിൽ(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            തിരികെ &unistr;
        } അല്ലെങ്കിൽ {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    തിരികെ nullptr;
}
തർക്കം
StringEnumeration::operator==(സ്ഥിരം StringEnumeration& that)സ്ഥിരം {
    തിരികെ typeid(*ഇത്) == typeid(that); 
}

തർക്കം
StringEnumeration::operator!=(സ്ഥിരം StringEnumeration& that)സ്ഥിരം {
    തിരികെ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    എങ്കിൽ (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        തിരികെ nullptr;
    }
    UStringEnumeration *result = പുതിയ UStringEnumeration(uenumToAdopt);
    എങ്കിൽ (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        തിരികെ nullptr;
    }
    തിരികെ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) സ്ഥിരം {
    തിരികെ uenum_എണ്ണം(uenum, &status);
}

സ്ഥിരം അക്ഷരം* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    തിരികെ uenum_next(uenum, resultLength, &status);
}

സ്ഥിരം UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    സ്ഥിരം char16_t* str = uenum_unext(uenum, &length, &status);
    എങ്കിൽ (str == nullptr || U_FAILURE(status)) {
        തിരികെ nullptr;
    }
    തിരികെ &unistr.setTo(str, length);
}

ശൂന്യം UStringEnumeration::reset(UErrorCode& status) {
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
സ്ഥിതം ശൂന്യം U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
സ്ഥിതം int32_t U_CALLCONV
ustrenum_എണ്ണം(UEnumeration* en,
               UErrorCode* ec)
{
    തിരികെ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
സ്ഥിതം സ്ഥിരം char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    തിരികെ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
സ്ഥിതം സ്ഥിരം അക്ഷരം* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    തിരികെ THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
സ്ഥിതം ശൂന്യം U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
സ്ഥിതം സ്ഥിരം UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_എണ്ണം,
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
    എങ്കിൽ (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(വലുപ്പം(UEnumeration));
        എങ്കിൽ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            uprv_memcpy(result, &USTRENUM_VT, വലുപ്പം(USTRENUM_VT));
            result->context = adopted;
        }
    }
    എങ്കിൽ (result == nullptr) {
        delete adopted;
    }
    തിരികെ result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

തരനിർവചനം ഘടന UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

സ്ഥിതം ശൂന്യം U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

സ്ഥിതം int32_t U_CALLCONV
ucharstrenum_എണ്ണം(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    തിരികെ ((UCharStringEnumeration*)en)->count;
}

സ്ഥിതം സ്ഥിരം char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    എങ്കിൽ (e->index >= e->count) {
        തിരികെ nullptr;
    }
    സ്ഥിരം char16_t* result = ((സ്ഥിരം char16_t**)e->uenum.context)[e->index++];
    എങ്കിൽ (resultLength) {
        *resultLength = u_strlen(result);
    }
    തിരികെ result;
}


സ്ഥിതം സ്ഥിരം അക്ഷരം* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    എങ്കിൽ (e->index >= e->count) {
        തിരികെ nullptr;
    }
    സ്ഥിരം അക്ഷരം* result = ((സ്ഥിരം അക്ഷരം**)e->uenum.context)[e->index++];
    എങ്കിൽ (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    തിരികെ result;
}

സ്ഥിതം ശൂന്യം U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

സ്ഥിതം സ്ഥിരം UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_എണ്ണം,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

സ്ഥിതം സ്ഥിരം UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_എണ്ണം,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND സ്ഥിരം അക്ഷരം* സ്ഥിരം strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    എങ്കിൽ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(വലുപ്പം(UCharStringEnumeration));
        എങ്കിൽ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            U_ASSERT((അക്ഷരം*)result==(അക്ഷരം*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, വലുപ്പം(UCHARSTRENUM_VT));
            result->uenum.context = (ശൂന്യം*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    തിരികെ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND സ്ഥിരം char16_t* സ്ഥിരം strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    എങ്കിൽ (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(വലുപ്പം(UCharStringEnumeration));
        എങ്കിൽ (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } അല്ലെങ്കിൽ {
            U_ASSERT((അക്ഷരം*)result==(അക്ഷരം*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, വലുപ്പം(UCHARSTRENUM_U_VT));
            result->uenum.context = (ശൂന്യം*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    തിരികെ (UEnumeration*) result;
}


// end C Wrapper
