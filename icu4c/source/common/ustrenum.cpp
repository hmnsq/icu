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
#अन्तर्भाव "utypeinfo.h"  // for 'typeid' to work 

#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/strenum.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "uenumimp.h"
#अन्तर्भाव "ustrenum.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(परिमाण(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    सचे (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ध्रुव {
  पटिनिवत्त nullptr;
}

ध्रुव वर्ण*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(status);
    सचे(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        सचे(U_SUCCESS(status)) {
            सचे(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            पटिनिवत्त chars;
        }
    }

    पटिनिवत्त nullptr;
}

ध्रुव char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(status);
    सचे(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        सचे(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        पटिनिवत्त unistr.getTerminatedBuffer();
    }

    पटिनिवत्त nullptr;
}

ध्रुव UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव वर्ण *s=next(&length, status);
    पटिनिवत्त setChars(s, length, status);
}

शून्य
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    सचे(U_SUCCESS(status) && capacity>charsCapacity) {
        सचे(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        सचे(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<वर्ण*>(uprv_malloc(capacity));
        सचे(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=परिमाण(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ध्रुव वर्ण* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    सचे(U_SUCCESS(status) && s!=nullptr) {
        सचे(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        सचे(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            पटिनिवत्त &unistr;
        } अञ्ञथा {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    पटिनिवत्त nullptr;
}
तार्किक
StringEnumeration::operator==(ध्रुव StringEnumeration& that)ध्रुव {
    पटिनिवत्त typeid(*एतत्) == typeid(that); 
}

तार्किक
StringEnumeration::operator!=(ध्रुव StringEnumeration& that)ध्रुव {
    पटिनिवत्त !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    सचे (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        पटिनिवत्त nullptr;
    }
    UStringEnumeration *result = नूतन UStringEnumeration(uenumToAdopt);
    सचे (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ध्रुव {
    पटिनिवत्त uenum_गणनम्(uenum, &status);
}

ध्रुव वर्ण* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    पटिनिवत्त uenum_next(uenum, resultLength, &status);
}

ध्रुव UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव char16_t* str = uenum_unext(uenum, &length, &status);
    सचे (str == nullptr || U_FAILURE(status)) {
        पटिनिवत्त nullptr;
    }
    पटिनिवत्त &unistr.setTo(str, length);
}

शून्य UStringEnumeration::reset(UErrorCode& status) {
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
स्थिर शून्य U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर int32_t U_CALLCONV
ustrenum_गणनम्(UEnumeration* en,
               UErrorCode* ec)
{
    पटिनिवत्त THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    पटिनिवत्त THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव वर्ण* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    पटिनिवत्त THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर शून्य U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
स्थिर ध्रुव UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_गणनम्,
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
    सचे (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(परिमाण(UEnumeration));
        सचे (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            uprv_memcpy(result, &USTRENUM_VT, परिमाण(USTRENUM_VT));
            result->context = adopted;
        }
    }
    सचे (result == nullptr) {
        delete adopted;
    }
    पटिनिवत्त result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

प्रकारनिर्देश संरचना UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

स्थिर शून्य U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

स्थिर int32_t U_CALLCONV
ucharstrenum_गणनम्(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    पटिनिवत्त ((UCharStringEnumeration*)en)->count;
}

स्थिर ध्रुव char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    सचे (e->index >= e->count) {
        पटिनिवत्त nullptr;
    }
    ध्रुव char16_t* result = ((ध्रुव char16_t**)e->uenum.context)[e->index++];
    सचे (resultLength) {
        *resultLength = u_strlen(result);
    }
    पटिनिवत्त result;
}


स्थिर ध्रुव वर्ण* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    सचे (e->index >= e->count) {
        पटिनिवत्त nullptr;
    }
    ध्रुव वर्ण* result = ((ध्रुव वर्ण**)e->uenum.context)[e->index++];
    सचे (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    पटिनिवत्त result;
}

स्थिर शून्य U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

स्थिर ध्रुव UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गणनम्,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

स्थिर ध्रुव UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गणनम्,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ध्रुव वर्ण* ध्रुव strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    सचे (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        सचे (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            U_ASSERT((वर्ण*)result==(वर्ण*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, परिमाण(UCHARSTRENUM_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    पटिनिवत्त (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ध्रुव char16_t* ध्रुव strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    सचे (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        सचे (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अञ्ञथा {
            U_ASSERT((वर्ण*)result==(वर्ण*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, परिमाण(UCHARSTRENUM_U_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    पटिनिवत्त (UEnumeration*) result;
}


// end C Wrapper
