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
#شامل "utypeinfo.h"  // for 'typeid' to work 

#شامل "unicode/ustring.h"
#شامل "unicode/strenum.h"
#شامل "unicode/putil.h"
#شامل "uenumimp.h"
#شامل "ustrenum.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_بفر), chars_استعداد(حجم(chars_بفر)) {
}

StringEnumeration::~StringEnumeration() {
    اگر (chars != خالی_اشاریہ && chars != chars_بفر) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  واپس خالی_اشاریہ;
}

ثابت حرف*
StringEnumeration::next(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت_2);
    اگر(U_SUCCESS(حالت_2) && s!=خالی_اشاریہ) {
        unistr=*s;
        ensure_Chars_استعداد(unistr.length()+1, حالت_2);
        اگر(U_SUCCESS(حالت_2)) {
            اگر(نتیجہ_لمبائی!=خالی_اشاریہ) {
                *نتیجہ_لمبائی=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_استعداد, US_INV);
            واپس chars;
        }
    }

    واپس خالی_اشاریہ;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت_2);
    اگر(U_SUCCESS(حالت_2) && s!=خالی_اشاریہ) {
        unistr=*s;
        اگر(نتیجہ_لمبائی!=خالی_اشاریہ) {
            *نتیجہ_لمبائی=unistr.length();
        }
        واپس unistr.get_Terminated_بفر();
    }

    واپس خالی_اشاریہ;
}

ثابت UnicodeString*
StringEnumeration::snext(UError_رمز& حالت_2) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, حالت_2);
    واپس سیٹ_Chars(s, length, حالت_2);
}

خالی
StringEnumeration::ensure_Chars_استعداد(int32_t capacity, UError_رمز &حالت_2) {
    اگر(U_SUCCESS(حالت_2) && capacity>chars_استعداد) {
        اگر(capacity<(chars_استعداد+chars_استعداد/2)) {
            // avoid allocation thrashing
            capacity=chars_استعداد+chars_استعداد/2;
        }
        اگر(chars!=chars_بفر) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        اگر(chars==خالی_اشاریہ) {
            chars=chars_بفر;
            chars_استعداد=حجم(chars_بفر);
            حالت_2=U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            chars_استعداد=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::سیٹ_Chars(ثابت حرف* s, int32_t length, UError_رمز& حالت_2) U_LIFETIME_BOUND {
    اگر(U_SUCCESS(حالت_2) && s!=خالی_اشاریہ) {
        اگر(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *بفر=unistr.get_بفر(length+1);
        اگر(بفر!=خالی_اشاریہ) {
            u_chars_مقصد_UChars(s, بفر, length);
            بفر[length]=0;
            unistr.release_بفر(length);
            واپس &unistr;
        } ورنہ {
            حالت_2=U_حافظہ_تخصیص_ERROR;
        }
    }

    واپس خالی_اشاریہ;
}
منطقی
StringEnumeration::عامل==(ثابت StringEnumeration& that)ثابت {
    واپس typeid(*یہ) == typeid(that); 
}

منطقی
StringEnumeration::عامل!=(ثابت StringEnumeration& that)ثابت {
    واپس !عامل==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_مقصد_Adopt, UError_رمز &حالت_2) {
    اگر (U_FAILURE(حالت_2)) {
        uenum_close(uenum_مقصد_Adopt);
        واپس خالی_اشاریہ;
    }
    UStringEnumeration *نتیجہ = نیا UStringEnumeration(uenum_مقصد_Adopt);
    اگر (نتیجہ == خالی_اشاریہ) {
        حالت_2 = U_حافظہ_تخصیص_ERROR;
        uenum_close(uenum_مقصد_Adopt);
        واپس خالی_اشاریہ;
    }
    واپس نتیجہ;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_رمز& حالت_2) ثابت {
    واپس uenum_گنتی(uenum, &حالت_2);
}

ثابت حرف* UStringEnumeration::next(int32_t* نتیجہ_لمبائی, UError_رمز& حالت_2) U_LIFETIME_BOUND {
    واپس uenum_اگلا(uenum, نتیجہ_لمبائی, &حالت_2);
}

ثابت UnicodeString* UStringEnumeration::snext(UError_رمز& حالت_2) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &حالت_2);
    اگر (str == خالی_اشاریہ || U_FAILURE(حالت_2)) {
        واپس خالی_اشاریہ;
    }
    واپس &unistr.سیٹ_مقصد(str, length);
}

خالی UStringEnumeration::reset(UError_رمز& حالت_2) {
    uenum_reset(uenum, &حالت_2);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_انتہا

// C wrapper --------------------------------------------------------------- ***

#تعریف_کرو THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساکن خالی U_CALLCONV
ustrenum_close(UEnumeration* en) {
    حذف THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساکن int32_t U_CALLCONV
ustrenum_گنتی(UEnumeration* en,
               UError_رمز* ec)
{
    واپس THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساکن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* نتیجہ_لمبائی,
               UError_رمز* ec)
{
    واپس THIS(en)->unext(نتیجہ_لمبائی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساکن ثابت حرف* U_CALLCONV
ustrenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* نتیجہ_لمبائی,
              UError_رمز* ec)
{
    واپس THIS(en)->next(نتیجہ_لمبائی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساکن خالی U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_رمز* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساکن ثابت UEnumeration USTRENUM_VT = {
    خالی_اشاریہ,
    خالی_اشاریہ, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_گنتی,
    ustrenum_unext,
    ustrenum_اگلا,
    ustrenum_reset
};

U_CDECL_انتہا

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_رمز* ec) { 
    UEnumeration* نتیجہ = خالی_اشاریہ;
    اگر (U_SUCCESS(*ec) && adopted != خالی_اشاریہ) {
        نتیجہ = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        اگر (نتیجہ == خالی_اشاریہ) {
            *ec = U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            uprv_memcpy(نتیجہ, &USTRENUM_VT, حجم(USTRENUM_VT));
            نتیجہ->context = adopted;
        }
    }
    اگر (نتیجہ == خالی_اشاریہ) {
        حذف adopted;
    }
    واپس نتیجہ;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

قسمتعریف ساخت UCharStringEnumeration {
    UEnumeration uenum;
    int32_t اشاریہ, count;
} UCharStringEnumeration;

ساکن خالی U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ساکن int32_t U_CALLCONV
ucharstrenum_گنتی(UEnumeration* en,
                   UError_رمز* /*ec*/) {
    واپس ((UCharStringEnumeration*)en)->count;
}

ساکن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمبائی,
                  UError_رمز* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    اگر (e->اشاریہ >= e->count) {
        واپس خالی_اشاریہ;
    }
    ثابت char16_t* نتیجہ = ((ثابت char16_t**)e->uenum.context)[e->اشاریہ++];
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = u_strlen(نتیجہ);
    }
    واپس نتیجہ;
}


ساکن ثابت حرف* U_CALLCONV
ucharstrenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمبائی,
                  UError_رمز* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    اگر (e->اشاریہ >= e->count) {
        واپس خالی_اشاریہ;
    }
    ثابت حرف* نتیجہ = ((ثابت حرف**)e->uenum.context)[e->اشاریہ++];
    اگر (نتیجہ_لمبائی) {
        *نتیجہ_لمبائی = (int32_t)uprv_strlen(نتیجہ);
    }
    واپس نتیجہ;
}

ساکن خالی U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_رمز* /*ec*/) {
    ((UCharStringEnumeration*)en)->اشاریہ = 0;
}

ساکن ثابت UEnumeration UCHARSTRENUM_VT = {
    خالی_اشاریہ,
    خالی_اشاریہ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_گنتی,
    uenum_unextDefault,
    ucharstrenum_اگلا,
    ucharstrenum_reset
};

ساکن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    خالی_اشاریہ,
    خالی_اشاریہ, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_گنتی,
    ucharstrenum_unext,
    uenum_اگلا_Default,
    ucharstrenum_reset
};

U_CDECL_انتہا

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_لڑیاں_Enumeration(U_LIFETIME_BOUND ثابت حرف* ثابت لڑیاں[],
                                 int32_t count,
                                 UError_رمز* ec) {
    UCharStringEnumeration* نتیجہ = خالی_اشاریہ;
    اگر (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڑیاں != خالی_اشاریہ)) {
        نتیجہ = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        اگر (نتیجہ == خالی_اشاریہ) {
            *ec = U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            U_ASSERT((حرف*)نتیجہ==(حرف*)(&نتیجہ->uenum));
            uprv_memcpy(نتیجہ, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            نتیجہ->uenum.context = (خالی*)لڑیاں;
            نتیجہ->اشاریہ = 0;
            نتیجہ->count = count;
        }
    }
    واپس (UEnumeration*) نتیجہ;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_لڑیاں_Enumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت لڑیاں[],
                                  int32_t count,
                                  UError_رمز* ec) {
    UCharStringEnumeration* نتیجہ = خالی_اشاریہ;
    اگر (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڑیاں != خالی_اشاریہ)) {
        نتیجہ = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        اگر (نتیجہ == خالی_اشاریہ) {
            *ec = U_حافظہ_تخصیص_ERROR;
        } ورنہ {
            U_ASSERT((حرف*)نتیجہ==(حرف*)(&نتیجہ->uenum));
            uprv_memcpy(نتیجہ, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            نتیجہ->uenum.context = (خالی*)لڑیاں;
            نتیجہ->اشاریہ = 0;
            نتیجہ->count = count;
        }
    }
    واپس (UEnumeration*) نتیجہ;
}


// end C Wrapper
