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
    : chars(chars_बफर), chars_क्षमता(परिमाण(chars_बफर)) {
}

StringEnumeration::~StringEnumeration() {
    यदि (chars != शून्यसूचकः && chars != chars_बफर) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ध्रुव {
  प्रत्यागम शून्यसूचकः;
}

ध्रुव वर्ण*
StringEnumeration::next(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(स्थितिः);
    यदि(U_SUCCESS(स्थितिः) && s!=शून्यसूचकः) {
        unistr=*s;
        ensure_Chars_क्षमता(unistr.length()+1, स्थितिः);
        यदि(U_SUCCESS(स्थितिः)) {
            यदि(परिणाम_दीर्घता!=शून्यसूचकः) {
                *परिणाम_दीर्घता=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_क्षमता, US_INV);
            प्रत्यागम chars;
        }
    }

    प्रत्यागम शून्यसूचकः;
}

ध्रुव char16_t*
StringEnumeration::unext(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    ध्रुव UnicodeString *s=snext(स्थितिः);
    यदि(U_SUCCESS(स्थितिः) && s!=शून्यसूचकः) {
        unistr=*s;
        यदि(परिणाम_दीर्घता!=शून्यसूचकः) {
            *परिणाम_दीर्घता=unistr.length();
        }
        प्रत्यागम unistr.get_Terminated_बफर();
    }

    प्रत्यागम शून्यसूचकः;
}

ध्रुव UnicodeString*
StringEnumeration::snext(UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव वर्ण *s=next(&length, स्थितिः);
    प्रत्यागम समुच्चयः_Chars(s, length, स्थितिः);
}

शून्य
StringEnumeration::ensure_Chars_क्षमता(int32_t capacity, UError_संहिता &स्थितिः) {
    यदि(U_SUCCESS(स्थितिः) && capacity>chars_क्षमता) {
        यदि(capacity<(chars_क्षमता+chars_क्षमता/2)) {
            // avoid allocation thrashing
            capacity=chars_क्षमता+chars_क्षमता/2;
        }
        यदि(chars!=chars_बफर) {
            uprv_free(chars);
        }
        chars = static_cast<वर्ण*>(uprv_malloc(capacity));
        यदि(chars==शून्यसूचकः) {
            chars=chars_बफर;
            chars_क्षमता=परिमाण(chars_बफर);
            स्थितिः=U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            chars_क्षमता=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::समुच्चयः_Chars(ध्रुव वर्ण* s, int32_t length, UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    यदि(U_SUCCESS(स्थितिः) && s!=शून्यसूचकः) {
        यदि(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *बफर=unistr.get_बफर(length+1);
        यदि(बफर!=शून्यसूचकः) {
            u_chars_गन्तव्यम्_UChars(s, बफर, length);
            बफर[length]=0;
            unistr.release_बफर(length);
            प्रत्यागम &unistr;
        } अन्यथा {
            स्थितिः=U_स्मृतिः_आवण्टनम्_ERROR;
        }
    }

    प्रत्यागम शून्यसूचकः;
}
तार्किक
StringEnumeration::संक्रियकः==(ध्रुव StringEnumeration& that)ध्रुव {
    प्रत्यागम typeid(*एतत्) == typeid(that); 
}

तार्किक
StringEnumeration::संक्रियकः!=(ध्रुव StringEnumeration& that)ध्रुव {
    प्रत्यागम !संक्रियकः==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_गन्तव्यम्_Adopt, UError_संहिता &स्थितिः) {
    यदि (U_FAILURE(स्थितिः)) {
        uenum_close(uenum_गन्तव्यम्_Adopt);
        प्रत्यागम शून्यसूचकः;
    }
    UStringEnumeration *परिणाम = नूतन UStringEnumeration(uenum_गन्तव्यम्_Adopt);
    यदि (परिणाम == शून्यसूचकः) {
        स्थितिः = U_स्मृतिः_आवण्टनम्_ERROR;
        uenum_close(uenum_गन्तव्यम्_Adopt);
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम परिणाम;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_संहिता& स्थितिः) ध्रुव {
    प्रत्यागम uenum_गणनम्(uenum, &स्थितिः);
}

ध्रुव वर्ण* UStringEnumeration::next(int32_t* परिणाम_दीर्घता, UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    प्रत्यागम uenum_अग्र(uenum, परिणाम_दीर्घता, &स्थितिः);
}

ध्रुव UnicodeString* UStringEnumeration::snext(UError_संहिता& स्थितिः) U_LIFETIME_BOUND {
    int32_t length;
    ध्रुव char16_t* str = uenum_unext(uenum, &length, &स्थितिः);
    यदि (str == शून्यसूचकः || U_FAILURE(स्थितिः)) {
        प्रत्यागम शून्यसूचकः;
    }
    प्रत्यागम &unistr.समुच्चयः_गन्तव्यम्(str, length);
}

शून्य UStringEnumeration::reset(UError_संहिता& स्थितिः) {
    uenum_reset(uenum, &स्थितिः);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_अन्तः

// C wrapper --------------------------------------------------------------- ***

#परिभाषय THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर शून्य U_CALLCONV
ustrenum_close(UEnumeration* en) {
    विलोपय THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर int32_t U_CALLCONV
ustrenum_गणनम्(UEnumeration* en,
               UError_संहिता* ec)
{
    प्रत्यागम THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* परिणाम_दीर्घता,
               UError_संहिता* ec)
{
    प्रत्यागम THIS(en)->unext(परिणाम_दीर्घता, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर ध्रुव वर्ण* U_CALLCONV
ustrenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* परिणाम_दीर्घता,
              UError_संहिता* ec)
{
    प्रत्यागम THIS(en)->next(परिणाम_दीर्घता, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
स्थिर शून्य U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_संहिता* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
स्थिर ध्रुव UEnumeration USTRENUM_VT = {
    शून्यसूचकः,
    शून्यसूचकः, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_गणनम्,
    ustrenum_unext,
    ustrenum_अग्र,
    ustrenum_reset
};

U_CDECL_अन्तः

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_संहिता* ec) { 
    UEnumeration* परिणाम = शून्यसूचकः;
    यदि (U_SUCCESS(*ec) && adopted != शून्यसूचकः) {
        परिणाम = (UEnumeration*) uprv_malloc(परिमाण(UEnumeration));
        यदि (परिणाम == शून्यसूचकः) {
            *ec = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            uprv_memcpy(परिणाम, &USTRENUM_VT, परिमाण(USTRENUM_VT));
            परिणाम->context = adopted;
        }
    }
    यदि (परिणाम == शून्यसूचकः) {
        विलोपय adopted;
    }
    प्रत्यागम परिणाम;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

प्रकारनिर्देश संरचना UCharStringEnumeration {
    UEnumeration uenum;
    int32_t सूचकांक, count;
} UCharStringEnumeration;

स्थिर शून्य U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

स्थिर int32_t U_CALLCONV
ucharstrenum_गणनम्(UEnumeration* en,
                   UError_संहिता* /*ec*/) {
    प्रत्यागम ((UCharStringEnumeration*)en)->count;
}

स्थिर ध्रुव char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_दीर्घता,
                  UError_संहिता* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    यदि (e->सूचकांक >= e->count) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव char16_t* परिणाम = ((ध्रुव char16_t**)e->uenum.context)[e->सूचकांक++];
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = u_strlen(परिणाम);
    }
    प्रत्यागम परिणाम;
}


स्थिर ध्रुव वर्ण* U_CALLCONV
ucharstrenum_अग्र(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* परिणाम_दीर्घता,
                  UError_संहिता* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    यदि (e->सूचकांक >= e->count) {
        प्रत्यागम शून्यसूचकः;
    }
    ध्रुव वर्ण* परिणाम = ((ध्रुव वर्ण**)e->uenum.context)[e->सूचकांक++];
    यदि (परिणाम_दीर्घता) {
        *परिणाम_दीर्घता = (int32_t)uprv_strlen(परिणाम);
    }
    प्रत्यागम परिणाम;
}

स्थिर शून्य U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_संहिता* /*ec*/) {
    ((UCharStringEnumeration*)en)->सूचकांक = 0;
}

स्थिर ध्रुव UEnumeration UCHARSTRENUM_VT = {
    शून्यसूचकः,
    शून्यसूचकः, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गणनम्,
    uenum_unextDefault,
    ucharstrenum_अग्र,
    ucharstrenum_reset
};

स्थिर ध्रुव UEnumeration UCHARSTRENUM_U_VT = {
    शून्यसूचकः,
    शून्यसूचकः, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_गणनम्,
    ucharstrenum_unext,
    uenum_अग्र_Default,
    ucharstrenum_reset
};

U_CDECL_अन्तः

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_वाक्यानि_Enumeration(U_LIFETIME_BOUND ध्रुव वर्ण* ध्रुव वाक्यानि[],
                                 int32_t count,
                                 UError_संहिता* ec) {
    UCharStringEnumeration* परिणाम = शून्यसूचकः;
    यदि (U_SUCCESS(*ec) && count >= 0 && (count == 0 || वाक्यानि != शून्यसूचकः)) {
        परिणाम = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        यदि (परिणाम == शून्यसूचकः) {
            *ec = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            U_ASSERT((वर्ण*)परिणाम==(वर्ण*)(&परिणाम->uenum));
            uprv_memcpy(परिणाम, &UCHARSTRENUM_VT, परिमाण(UCHARSTRENUM_VT));
            परिणाम->uenum.context = (शून्य*)वाक्यानि;
            परिणाम->सूचकांक = 0;
            परिणाम->count = count;
        }
    }
    प्रत्यागम (UEnumeration*) परिणाम;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_वाक्यानि_Enumeration(U_LIFETIME_BOUND ध्रुव char16_t* ध्रुव वाक्यानि[],
                                  int32_t count,
                                  UError_संहिता* ec) {
    UCharStringEnumeration* परिणाम = शून्यसूचकः;
    यदि (U_SUCCESS(*ec) && count >= 0 && (count == 0 || वाक्यानि != शून्यसूचकः)) {
        परिणाम = (UCharStringEnumeration*) uprv_malloc(परिमाण(UCharStringEnumeration));
        यदि (परिणाम == शून्यसूचकः) {
            *ec = U_स्मृतिः_आवण्टनम्_ERROR;
        } अन्यथा {
            U_ASSERT((वर्ण*)परिणाम==(वर्ण*)(&परिणाम->uenum));
            uprv_memcpy(परिणाम, &UCHARSTRENUM_U_VT, परिमाण(UCHARSTRENUM_U_VT));
            परिणाम->uenum.context = (शून्य*)वाक्यानि;
            परिणाम->सूचकांक = 0;
            परिणाम->count = count;
        }
    }
    प्रत्यागम (UEnumeration*) परिणाम;
}


// end C Wrapper
