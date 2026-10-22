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
    यदि (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ध्रुव {
  प्रत्यागम nullptr;
}

ध्रुव वर्ण*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(status);
    यदि(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        यदि(U_SUCCESS(status)) {
            यदि(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            प्रत्यागम chars;
        }
    }

    प्रत्यागम nullptr;
}

ध्रुव char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(status);
    यदि(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        यदि(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        प्रत्यागम unistr.getTerminatedBuffer();
    }

    प्रत्यागम nullptr;
}

ध्रुव UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव वर्ण *s=next(&length, status);
    प्रत्यागम setChars(s, length, status);
}

शून्य
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    यदि(U_SUCCESS(status) && capacity>charsCapacity) {
        यदि(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        यदि(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<वर्ण*>(uprv_malloc(capacity));
        यदि(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=परिमाण(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ध्रुव वर्ण* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    यदि(U_SUCCESS(status) && s!=nullptr) {
        यदि(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        यदि(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            प्रत्यागम &unistr;
        } अन्यथा {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    प्रत्यागम nullptr;
}
तार्किक
StringEnumeration::operator==(ध्रुव StringEnumeration& that)ध्रुव {
    प्रत्यागम typeid(*एतत्) == typeid(that); 
}

तार्किक
StringEnumeration::operator!=(ध्रुव StringEnumeration& that)ध्रुव {
    प्रत्यागम !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    यदि (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        प्रत्यागम nullptr;
    }
    UStringEnumeration *result = नूतन UStringEnumeration(uenumToAdopt);
    यदि (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        प्रत्यागम nullptr;
    }
    प्रत्यागम result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ध्रुव {
    प्रत्यागम uenum_मोजणी(uenum, &status);
}

ध्रुव वर्ण* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    प्रत्यागम uenum_next(uenum, resultLength, &status);
}

ध्रुव UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव char16_t* str = uenum_unext(uenum, &length, &status);
    यदि (str == nullptr || U_FAILURE(status)) {
        प्रत्यागम nullptr;
    }
    प्रत्यागम &unistr.setTo(str, length);
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
    प्रत्यागम THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    प्रत्यागम THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव वर्ण* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    प्रत्यागम THIS(en)->next(resultLength, *ec);
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
    यदि (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(परिमाण(UEnumeration));
        यदि (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_memcpy(result, &USTRENUM_VT, परिमाण(USTRENUM_VT));
            result->context = adopted;
        }
    }
    यदि (result == nullptr) {
        delete adopted;
    }
    प्रत्यागम result;
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
ucharstrenum_मोजणी(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    प्रत्यागम ((UCharStringEnumeration*)en)->count;
}

स्थिर ध्रुव char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    यदि (e->index >= e->count) {
        प्रत्यागम nullptr;
    }
    ध्रुव char16_t* result = ((ध्रुव char16_t**)e->uenum.context)[e->index++];
    यदि (resultLength) {
        *resultLength = u_strlen(result);
    }
    प्रत्यागम result;
}


स्थिर ध्रुव वर्ण* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    यदि (e->index >= e->count) {
        प्रत्यागम nullptr;
    }
    ध्रुव वर्ण* result = ((ध्रुव वर्ण**)e->uenum.context)[e->index++];
    यदि (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    प्रत्यागम result;
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
    ucharstrenum_मोजणी,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

स्थिर ध्रुव UEnumeration UCHARSTRENUM_U_VT = {
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
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ध्रुव वर्ण* ध्रुव strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    यदि (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        यदि (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((वर्ण*)result==(वर्ण*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, परिमाण(UCHARSTRENUM_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    प्रत्यागम (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ध्रुव char16_t* ध्रुव strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    यदि (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        यदि (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((वर्ण*)result==(वर्ण*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, परिमाण(UCHARSTRENUM_U_VT));
            result->uenum.context = (शून्य*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    प्रत्यागम (UEnumeration*) result;
}


// end C Wrapper
