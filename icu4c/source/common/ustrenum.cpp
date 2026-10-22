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
#समावेश "utypeinfo.h"  // for 'typeid' to work 

#समावेश "unicode/ustring.h"
#समावेश "unicode/strenum.h"
#समावेश "unicode/putil.h"
#समावेश "uenumimp.h"
#समावेश "ustrenum.h"
#समावेश "cstring.h"
#समावेश "cmemory.h"
#समावेश "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_बफर), charsCapacity(माप(chars_बफर)) {
}

StringEnumeration::~StringEnumeration() {
    अगर (chars != nullptr && chars != chars_बफर) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() स्थिर {
  वापस nullptr;
}

स्थिर अक्षर*
StringEnumeration::next(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND {
    स्थिर UnicodeString *s=snext(status);
    अगर(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        ensureCharsCapacity(unistr.length()+1, status);
        अगर(U_SUCCESS(status)) {
            अगर(परिणाम_लंबाई!=nullptr) {
                *परिणाम_लंबाई=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, charsCapacity, US_INV);
            वापस chars;
        }
    }

    वापस nullptr;
}

स्थिर char16_t*
StringEnumeration::unext(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND {
    स्थिर UnicodeString *s=snext(status);
    अगर(U_SUCCESS(status) && s!=nullptr) {
        unistr=*s;
        अगर(परिणाम_लंबाई!=nullptr) {
            *परिणाम_लंबाई=unistr.length();
        }
        वापस unistr.get_Terminated_बफर();
    }

    वापस nullptr;
}

स्थिर UnicodeString*
StringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    स्थिर अक्षर *s=next(&length, status);
    वापस setChars(s, length, status);
}

व्योम
StringEnumeration::ensureCharsCapacity(int32_t capacity, UErrorCode &status) {
    अगर(U_SUCCESS(status) && capacity>charsCapacity) {
        अगर(capacity<(charsCapacity+charsCapacity/2)) {
            // avoid allocation thrashing
            capacity=charsCapacity+charsCapacity/2;
        }
        अगर(chars!=chars_बफर) {
            uprv_free(chars);
        }
        chars = static_cast<अक्षर*>(uprv_malloc(capacity));
        अगर(chars==nullptr) {
            chars=chars_बफर;
            charsCapacity=माप(chars_बफर);
            status=U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            charsCapacity=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::setChars(स्थिर अक्षर* s, int32_t length, UErrorCode& status) U_LIFETIME_BOUND {
    अगर(U_SUCCESS(status) && s!=nullptr) {
        अगर(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *बफर=unistr.get_बफर(length+1);
        अगर(बफर!=nullptr) {
            u_charsToUChars(s, बफर, length);
            बफर[length]=0;
            unistr.release_बफर(length);
            वापस &unistr;
        } अन्यथा {
            status=U_MEMORY_ALLOCATION_ERROR;
        }
    }

    वापस nullptr;
}
bool
StringEnumeration::चालक==(स्थिर StringEnumeration& that)स्थिर {
    वापस typeid(*यह) == typeid(that); 
}

bool
StringEnumeration::चालक!=(स्थिर StringEnumeration& that)स्थिर {
    वापस !चालक==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenumToAdopt, UErrorCode &status) {
    अगर (U_FAILURE(status)) {
        uenum_close(uenumToAdopt);
        वापस nullptr;
    }
    UStringEnumeration *परिणाम = नया UStringEnumeration(uenumToAdopt);
    अगर (परिणाम == nullptr) {
        status = U_MEMORY_ALLOCATION_ERROR;
        uenum_close(uenumToAdopt);
        वापस nullptr;
    }
    वापस परिणाम;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UErrorCode& status) स्थिर {
    वापस uenum_गिनती(uenum, &status);
}

स्थिर अक्षर* UStringEnumeration::next(int32_t* परिणाम_लंबाई, UErrorCode& status) U_LIFETIME_BOUND {
    वापस uenum_अग्र(uenum, परिणाम_लंबाई, &status);
}

स्थिर UnicodeString* UStringEnumeration::snext(UErrorCode& status) U_LIFETIME_BOUND {
    int32_t length;
    स्थिर char16_t* str = uenum_unext(uenum, &length, &status);
    अगर (str == nullptr || U_FAILURE(status)) {
        वापस nullptr;
    }
    वापस &unistr.setTo(str, length);
}

व्योम UStringEnumeration::reset(UErrorCode& status) {
    uenum_reset(uenum, &status);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_END

// C wrapper --------------------------------------------------------------- ***

#घोषणा THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
अटल व्योम U_CALLCONV
ustrenum_close(UEnumeration* en) {
    मिटाओ THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
अटल int32_t U_CALLCONV
ustrenum_गिनती(UEnumeration* en,
               UErrorCode* ec)
{
    वापस THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
अटल स्थिर char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* परिणाम_लंबाई,
               UErrorCode* ec)
{
    वापस THIS(en)->unext(परिणाम_लंबाई, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
अटल स्थिर अक्षर* U_CALLCONV
ustrenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* परिणाम_लंबाई,
              UErrorCode* ec)
{
    वापस THIS(en)->next(परिणाम_लंबाई, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
अटल व्योम U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UErrorCode* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
अटल स्थिर UEnumeration USTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_गिनती,
    ustrenum_unext,
    ustrenum_अग्र,
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
    UEnumeration* परिणाम = nullptr;
    अगर (U_SUCCESS(*ec) && adopted != nullptr) {
        परिणाम = (UEnumeration*) uprv_malloc(माप(UEnumeration));
        अगर (परिणाम == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            uprv_memcpy(परिणाम, &USTRENUM_VT, माप(USTRENUM_VT));
            परिणाम->context = adopted;
        }
    }
    अगर (परिणाम == nullptr) {
        मिटाओ adopted;
    }
    वापस परिणाम;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

प्रकार काष्ठा UCharStringEnumeration {
    UEnumeration uenum;
    int32_t सूचकांक, count;
} UCharStringEnumeration;

अटल व्योम U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

अटल int32_t U_CALLCONV
ucharstrenum_गिनती(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    वापस ((UCharStringEnumeration*)en)->count;
}

अटल स्थिर char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_लंबाई,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    अगर (e->सूचकांक >= e->count) {
        वापस nullptr;
    }
    स्थिर char16_t* परिणाम = ((स्थिर char16_t**)e->uenum.context)[e->सूचकांक++];
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = u_strlen(परिणाम);
    }
    वापस परिणाम;
}


अटल स्थिर अक्षर* U_CALLCONV
ucharstrenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_लंबाई,
                  UErrorCode* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    अगर (e->सूचकांक >= e->count) {
        वापस nullptr;
    }
    स्थिर अक्षर* परिणाम = ((स्थिर अक्षर**)e->uenum.context)[e->सूचकांक++];
    अगर (परिणाम_लंबाई) {
        *परिणाम_लंबाई = (int32_t)uprv_strlen(परिणाम);
    }
    वापस परिणाम;
}

अटल व्योम U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UErrorCode* /*ec*/) {
    ((UCharStringEnumeration*)en)->सूचकांक = 0;
}

अटल स्थिर UEnumeration UCHARSTRENUM_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गिनती,
    uenum_unextDefault,
    ucharstrenum_अग्र,
    ucharstrenum_reset
};

अटल स्थिर UEnumeration UCHARSTRENUM_U_VT = {
    nullptr,
    nullptr, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गिनती,
    ucharstrenum_unext,
    uenum_अग्र_Default,
    ucharstrenum_reset
};

U_CDECL_END

U_CAPI UEnumeration* U_EXPORT2
uenum_openCharStringsEnumeration(U_LIFETIME_BOUND स्थिर अक्षर* स्थिर strings[],
                                 int32_t count,
                                 UErrorCode* ec) {
    UCharStringEnumeration* परिणाम = nullptr;
    अगर (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        परिणाम = (UCharStringEnumeration*) uprv_malloc(माप(UCharStringEnumeration));
        अगर (परिणाम == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((अक्षर*)परिणाम==(अक्षर*)(&परिणाम->uenum));
            uprv_memcpy(परिणाम, &UCHARSTRENUM_VT, माप(UCHARSTRENUM_VT));
            परिणाम->uenum.context = (व्योम*)strings;
            परिणाम->सूचकांक = 0;
            परिणाम->count = count;
        }
    }
    वापस (UEnumeration*) परिणाम;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_openUCharStringsEnumeration(U_LIFETIME_BOUND स्थिर char16_t* स्थिर strings[],
                                  int32_t count,
                                  UErrorCode* ec) {
    UCharStringEnumeration* परिणाम = nullptr;
    अगर (U_SUCCESS(*ec) && count >= 0 && (count == 0 || strings != nullptr)) {
        परिणाम = (UCharStringEnumeration*) uprv_malloc(माप(UCharStringEnumeration));
        अगर (परिणाम == nullptr) {
            *ec = U_MEMORY_ALLOCATION_ERROR;
        } अन्यथा {
            U_ASSERT((अक्षर*)परिणाम==(अक्षर*)(&परिणाम->uenum));
            uprv_memcpy(परिणाम, &UCHARSTRENUM_U_VT, माप(UCHARSTRENUM_U_VT));
            परिणाम->uenum.context = (व्योम*)strings;
            परिणाम->सूचकांक = 0;
            परिणाम->count = count;
        }
    }
    वापस (UEnumeration*) परिणाम;
}


// end C Wrapper
