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
    : chars(charsBuffer), charsCapacity(आकार(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    जर (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() स्थिरांक {
  परत nullptr;
}

स्थिरांक अक्षर*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    स्थिरांक UnicodeString *s=snext(status);
    जर(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        जर(U_SUCCESS(status)) {
            जर(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            परत chars;
        }
    }

    परत nullptr;
}

स्थिरांक char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    स्थिरांक UnicodeString *s=snext(status);
    जर(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        जर(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        परत unistr.getTerminatedBuffer();
    }

    परत nullptr;
}

स्थिरांक UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    स्थिरांक अक्षर *s=next(&length, status);
    परत setChars(s, length, status);
}

शून्य
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    जर(U_SUCCESS(status) && capacity>charsCapacity) {
        जर(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        जर(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<अक्षर*>(uprv_malloc(capacity));
        जर(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=आकार(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(स्थिरांक अक्षर* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    जर(U_SUCCESS(status) && s!=nullptr) {
        जर(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        जर(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            परत &unistr;
        } अन्यथा {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    परत nullptr;
}
तार्किक
StringEnumeration::चालक==(स्थिरांक StringEnumeration& that)स्थिरांक {
    परत typeid(*हे) == typeid(that); 
}

तार्किक
StringEnumeration::चालक!=(स्थिरांक StringEnumeration& that)स्थिरांक {
    परत !चालक==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    जर (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        परत nullptr;
    }
    UStringEnumeration *result = नवीन UStringEnumeration(uenumToAdopt);
    जर (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        परत nullptr;
    }
    परत result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) स्थिरांक {
    परत uenum_मोजणी(uenum, &status);
}

स्थिरांक अक्षर* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    परत uenum_next(uenum, resultLength, &status);
}

स्थिरांक UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    स्थिरांक char16_t* str = uenum_unext(uenum, &length, &status);
    जर (str == nullptr || U_FAILURE(status)) {
        परत nullptr;
    }
    परत &unistr.setTo(str, length);
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
ustrenum_मोजणी(UEnumeration* en,
               UErrorCode* ec)
{
    परत THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर स्थिरांक char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    परत THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर स्थिरांक अक्षर* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    परत THIS(en)->next(resultLength, *ec);
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
स्थिर स्थिरांक UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_मोजणी,
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
    जर (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(आकार(UEnumeration));
        जर (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_memcpy(result, &USTRENUM_VT, आकार(USTRENUM_VT));
            result->context = adopted;
        }
    }
    जर (result == nullptr) {
        delete adopted;
    }
    परत result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

प्रकारव्याख्या संरचना UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

स्थिर शून्य U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

स्थिर int32_t U_CALLCONV
ucharstrenum_मोजणी(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    परत ((UCharStringEnumeration*)en)->count;
}

स्थिर स्थिरांक char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    जर (e->index >= e->count) {
        परत nullptr;
    }
    स्थिरांक char16_t* result = ((स्थिरांक char16_t**)e->uenum.context)[e->index++];
    जर (resultLength) {
        *resultLength = u_strlen(result);
    }
    परत result;
}


स्थिर स्थिरांक अक्षर* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    जर (e->index >= e->count) {
        परत nullptr;
    }
    स्थिरांक अक्षर* result = ((स्थिरांक अक्षर**)e->uenum.context)[e->index++];
    जर (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    परत result;
}

स्थिर शून्य U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

स्थिर स्थिरांक UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_मोजणी,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

स्थिर स्थिरांक UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_मोजणी,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND स्थिरांक अक्षर* स्थिरांक strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    जर (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(आकार(UCharStringEnumeration));
        जर (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((अक्षर*)result==(अक्षर*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, आकार(UCHARSTRENUM_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    परत (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND स्थिरांक char16_t* स्थिरांक strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    जर (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(आकार(UCharStringEnumeration));
        जर (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((अक्षर*)result==(अक्षर*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, आकार(UCHARSTRENUM_U_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    परत (UEnumeration*) result;
}


// end C Wrapper
