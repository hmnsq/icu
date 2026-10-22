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
#ଅନ୍ତର୍ଭାଵ "utypeinfo.h"  // for 'typeid' to work 

#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/strenum.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "uenumimp.h"
#ଅନ୍ତର୍ଭାଵ "ustrenum.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(ଆକାର(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    ଯଦି (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ଅଚଳ {
  ଫେରନ୍ତୁ nullptr;
}

ଅଚଳ ଅକ୍ଷର*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ଅଚଳ UnicodeString *s=snext(status);
    ଯଦି(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        ଯଦି(U_SUCCESS(status)) {
            ଯଦି(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            ଫେରନ୍ତୁ chars;
        }
    }

    ଫେରନ୍ତୁ nullptr;
}

ଅଚଳ char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ଅଚଳ UnicodeString *s=snext(status);
    ଯଦି(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ଯଦି(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        ଫେରନ୍ତୁ unistr.getTerminatedBuffer();
    }

    ଫେରନ୍ତୁ nullptr;
}

ଅଚଳ UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ଅଚଳ ଅକ୍ଷର *s=next(&length, status);
    ଫେରନ୍ତୁ setChars(s, length, status);
}

ଶୂନ୍ୟ
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    ଯଦି(U_SUCCESS(status) && capacity>charsCapacity) {
        ଯଦି(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        ଯଦି(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<ଅକ୍ଷର*>(uprv_malloc(capacity));
        ଯଦି(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=ଆକାର(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(ଅଚଳ ଅକ୍ଷର* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    ଯଦି(U_SUCCESS(status) && s!=nullptr) {
        ଯଦି(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        ଯଦି(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            ଫେରନ୍ତୁ &unistr;
        } ଅନ୍ୟଥା {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    ଫେରନ୍ତୁ nullptr;
}
ତାର୍କିକ
StringEnumeration::operator==(ଅଚଳ StringEnumeration& that)ଅଚଳ {
    ଫେରନ୍ତୁ typeid(*ଏହି) == typeid(that); 
}

ତାର୍କିକ
StringEnumeration::operator!=(ଅଚଳ StringEnumeration& that)ଅଚଳ {
    ଫେରନ୍ତୁ !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    ଯଦି (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        ଫେରନ୍ତୁ nullptr;
    }
    UStringEnumeration *result = ନୂତନ UStringEnumeration(uenumToAdopt);
    ଯଦି (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) ଅଚଳ {
    ଫେରନ୍ତୁ uenum_ଗଣନା(uenum, &status);
}

ଅଚଳ ଅକ୍ଷର* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    ଫେରନ୍ତୁ uenum_next(uenum, resultLength, &status);
}

ଅଚଳ UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    ଅଚଳ char16_t* str = uenum_unext(uenum, &length, &status);
    ଯଦି (str == nullptr || U_FAILURE(status)) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଫେରନ୍ତୁ &unistr.setTo(str, length);
}

ଶୂନ୍ୟ UStringEnumeration::reset(UErrorCode& status) {
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
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ସ୍ଥିର int32_t U_CALLCONV
ustrenum_ଗଣନା(UEnumeration* en,
               UErrorCode* ec)
{
    ଫେରନ୍ତୁ THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ସ୍ଥିର ଅଚଳ char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    ଫେରନ୍ତୁ THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    ଫେରନ୍ତୁ THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ସ୍ଥିର ଅଚଳ UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ଗଣନା,
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
    ଯଦି (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(ଆକାର(UEnumeration));
        ଯଦି (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            uprv_memcpy(result, &USTRENUM_VT, ଆକାର(USTRENUM_VT));
            result->context = adopted;
        }
    }
    ଯଦି (result == nullptr) {
        delete adopted;
    }
    ଫେରନ୍ତୁ result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ସ୍ଥିର int32_t U_CALLCONV
ucharstrenum_ଗଣନା(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ଫେରନ୍ତୁ ((UCharStringEnumeration*)en)->count;
}

ସ୍ଥିର ଅଚଳ char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ଯଦି (e->index >= e->count) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ char16_t* result = ((ଅଚଳ char16_t**)e->uenum.context)[e->index++];
    ଯଦି (resultLength) {
        *resultLength = u_strlen(result);
    }
    ଫେରନ୍ତୁ result;
}


ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    ଯଦି (e->index >= e->count) {
        ଫେରନ୍ତୁ nullptr;
    }
    ଅଚଳ ଅକ୍ଷର* result = ((ଅଚଳ ଅକ୍ଷର**)e->uenum.context)[e->index++];
    ଯଦି (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ଶୂନ୍ୟ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

ସ୍ଥିର ଅଚଳ UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ଗଣନା,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

ସ୍ଥିର ଅଚଳ UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ଗଣନା,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND ଅଚଳ ଅକ୍ଷର* ଅଚଳ strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ଯଦି (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(ଆକାର(UCharStringEnumeration));
        ଯଦି (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            U_ASSERT((ଅକ୍ଷର*)result==(ଅକ୍ଷର*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, ଆକାର(UCHARSTRENUM_VT));
            result->uenum.context = (ଶୂନ୍ୟ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ଫେରନ୍ତୁ (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND ଅଚଳ char16_t* ଅଚଳ strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    ଯଦି (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(ଆକାର(UCharStringEnumeration));
        ଯଦି (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } ଅନ୍ୟଥା {
            U_ASSERT((ଅକ୍ଷର*)result==(ଅକ୍ଷର*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, ଆକାର(UCHARSTRENUM_U_VT));
            result->uenum.context = (ଶୂନ୍ୟ*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    ଫେରନ୍ତୁ (UEnumeration*) result;
}


// end C Wrapper
