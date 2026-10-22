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
#અન્તર્ભાવ "utypeinfo.h"  // for 'typeid' to work 

#અન્તર્ભાવ "unicode/ustring.h"
#અન્તર્ભાવ "unicode/strenum.h"
#અન્તર્ભાવ "unicode/putil.h"
#અન્તર્ભાવ "uenumimp.h"
#અન્તર્ભાવ "ustrenum.h"
#અન્તર્ભાવ "cstring.h"
#અન્તર્ભાવ "cmemory.h"
#અન્તર્ભાવ "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(charsBuffer), charsCapacity(કદ(charsBuffer)) {
}

StringEnumeration::~StringEnumeration() {
    જો (chars != nullptr && chars != charsBuffer) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() અચલ {
  પાછા nullptr;
}

અચલ અક્ષર*
StringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    અચલ UnicodeString *s=snext(status);
    જો(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        જો(U_SUCCESS(status)) {
            જો(resultLength!=nullptr) {
                *resultLength=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            પાછા chars;
        }
    }

    પાછા nullptr;
}

અચલ char16_t*
StringEnumeration::unext(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    અચલ UnicodeString *s=snext(status);
    જો(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        જો(resultLength!=nullptr) {
            *resultLength=unistr.length();
        }
        પાછા unistr.getTerminatedBuffer();
    }

    પાછા nullptr;
}

અચલ UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    અચલ અક્ષર *s=next(&length, status);
    પાછા setChars(s, length, status);
}

શૂન્ય
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    જો(U_SUCCESS(status) && capacity>charsCapacity) {
        જો(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        જો(chars!=charsBuffer) {
            uprv_free(chars);
        }
        chars = static_cast<અક્ષર*>(uprv_malloc(capacity));
        જો(chars==nullptr) {
            chars=charsBuffer;
            charsCapacity=કદ(charsBuffer);
            status=U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(અચલ અક્ષર* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    જો(U_SUCCESS(status) && s!=nullptr) {
        જો(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *buffer=unistr.getBuffer(length+1);
        જો(buffer!=nullptr) {
            u_charsToUChars(s, buffer, length);
            buffer[length]=0;
            unistr.releaseBuffer(length);
            પાછા &unistr;
        } નહીંતર {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    પાછા nullptr;
}
તાર્કિક
StringEnumeration::operator==(અચલ StringEnumeration& that)અચલ {
    પાછા typeid(*આ) == typeid(that); 
}

તાર્કિક
StringEnumeration::operator!=(અચલ StringEnumeration& that)અચલ {
    પાછા !operator==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    જો (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        પાછા nullptr;
    }
    UStringEnumeration *result = નવું UStringEnumeration(uenumToAdopt);
    જો (result == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        પાછા nullptr;
    }
    પાછા result;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) અચલ {
    પાછા uenum_ગણતરી(uenum, &status);
}

અચલ અક્ષર* UStringEnumeration::next(int32_t* resultLength, UErrorCode& status) U_LIFETIME_BOUND {
    પાછા uenum_next(uenum, resultLength, &status);
}

અચલ UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    અચલ char16_t* str = uenum_unext(uenum, &length, &status);
    જો (str == nullptr || U_FAILURE(status)) {
        પાછા nullptr;
    }
    પાછા &unistr.setTo(str, length);
}

શૂન્ય UStringEnumeration::reset(UErrorCode& status) {
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
સ્થિર શૂન્ય U_CALLCONV
ustrenum_close(UEnumeration* en) {
    delete THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
સ્થિર int32_t U_CALLCONV
ustrenum_ગણતરી(UEnumeration* en,
               UErrorCode* ec)
{
    પાછા THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
સ્થિર અચલ char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* resultLength,
               UErrorCode* ec)
{
    પાછા THIS(en)->unext(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
સ્થિર અચલ અક્ષર* U_CALLCONV
ustrenum_next(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* resultLength,
              UErrorCode* ec)
{
    પાછા THIS(en)->next(resultLength, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
સ્થિર શૂન્ય U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
સ્થિર અચલ UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ગણતરી,
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
    જો (U_SUCCESS(*ec) && adopted != nullptr) {
        result = (UEnumeration*) uprv_malloc(કદ(UEnumeration));
        જો (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            uprv_memcpy(result, &USTRENUM_VT, કદ(USTRENUM_VT));
            result->context = adopted;
        }
    }
    જો (result == nullptr) {
        delete adopted;
    }
    પાછા result;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

પ્રકારવ્યાખ્યા સંરચના UCharStringEnumeration {
    UEnumeration uenum;
    int32_t index, count;
} UCharStringEnumeration;

સ્થિર શૂન્ય U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

સ્થિર int32_t U_CALLCONV
ucharstrenum_ગણતરી(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    પાછા ((UCharStringEnumeration*)en)->count;
}

સ્થિર અચલ char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    જો (e->index >= e->count) {
        પાછા nullptr;
    }
    અચલ char16_t* result = ((અચલ char16_t**)e->uenum.context)[e->index++];
    જો (resultLength) {
        *resultLength = u_strlen(result);
    }
    પાછા result;
}


સ્થિર અચલ અક્ષર* U_CALLCONV
ucharstrenum_next(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* resultLength,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    જો (e->index >= e->count) {
        પાછા nullptr;
    }
    અચલ અક્ષર* result = ((અચલ અક્ષર**)e->uenum.context)[e->index++];
    જો (resultLength) {
        *resultLength = (int32_t)uprv_strlen(result);
    }
    પાછા result;
}

સ્થિર શૂન્ય U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->index = 0;
}

સ્થિર અચલ UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ગણતરી,
    uenum_unextDefault,
    ucharstrenum_next,
    ucharstrenum_reset
};

સ્થિર અચલ UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ગણતરી,
    ucharstrenum_unext,
    uenum_nextDefault,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND અચલ અક્ષર* અચલ strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    જો (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(કદ(UCharStringEnumeration));
        જો (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            U_ASSERT((અક્ષર*)result==(અક્ષર*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_VT, કદ(UCHARSTRENUM_VT));
            result->uenum.context = (શૂન્ય*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    પાછા (UEnumeration*) result;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND અચલ char16_t* અચલ strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* result = nullptr;
    જો (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        result = (UCharStringEnumeration*) uprv_malloc(કદ(UCharStringEnumeration));
        જો (result == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } નહીંતર {
            U_ASSERT((અક્ષર*)result==(અક્ષર*)(&result->uenum));
            uprv_memcpy(result, &UCHARSTRENUM_U_VT, કદ(UCHARSTRENUM_U_VT));
            result->uenum.context = (શૂન્ય*)strings;
            result->index = 0;
            result->count = count;
        }
    }
    પાછા (UEnumeration*) result;
}


// end C Wrapper
