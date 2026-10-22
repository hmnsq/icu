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
#ضمن "utypeinfo.h"  // for 'typeid' to work 

#ضمن "unicode/ustring.h"
#ضمن "unicode/strenum.h"
#ضمن "unicode/putil.h"
#ضمن "uenumimp.h"
#ضمن "ustrenum.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "uassert.h"

U_NAMESPACE_BEGIN
// StringEnumeration implementation ---------------------------------------- ***

StringEnumeration::StringEnumeration()
    : chars(chars_بفر), chars_سمرتھا(حجم(chars_بفر)) {
}

StringEnumeration::~StringEnumeration() {
    إذا (chars != خالی_سوچک && chars != chars_بفر) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  أرجع خالی_سوچک;
}

ثابت حرف*
StringEnumeration::next(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=خالی_سوچک) {
        unistr=*s;
        ensure_Chars_سمرتھا(unistr.length()+1, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(نتیجہ_لمائی!=خالی_سوچک) {
                *نتیجہ_لمائی=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_سمرتھا, US_INV);
            أرجع chars;
        }
    }

    أرجع خالی_سوچک;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=خالی_سوچک) {
        unistr=*s;
        إذا(نتیجہ_لمائی!=خالی_سوچک) {
            *نتیجہ_لمائی=unistr.length();
        }
        أرجع unistr.get_Terminated_بفر();
    }

    أرجع خالی_سوچک;
}

ثابت UnicodeString*
StringEnumeration::snext(UError_کوڈ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, حالت);
    أرجع سیٹ_Chars(s, length, حالت);
}

فراغ
StringEnumeration::ensure_Chars_سمرتھا(int32_t capacity, UError_کوڈ &حالت) {
    إذا(U_SUCCESS(حالت) && capacity>chars_سمرتھا) {
        إذا(capacity<(chars_سمرتھا+chars_سمرتھا/2)) {
            // avoid allocation thrashing
            capacity=chars_سمرتھا+chars_سمرتھا/2;
        }
        إذا(chars!=chars_بفر) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        إذا(chars==خالی_سوچک) {
            chars=chars_بفر;
            chars_سمرتھا=حجم(chars_بفر);
            حالت=U_حافظہ_ونڈ_ERROR;
        } وإلا {
            chars_سمرتھا=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::سیٹ_Chars(ثابت حرف* s, int32_t length, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    إذا(U_SUCCESS(حالت) && s!=خالی_سوچک) {
        إذا(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *بفر=unistr.get_بفر(length+1);
        إذا(بفر!=خالی_سوچک) {
            u_chars_منزل_UChars(s, بفر, length);
            بفر[length]=0;
            unistr.release_بفر(length);
            أرجع &unistr;
        } وإلا {
            حالت=U_حافظہ_ونڈ_ERROR;
        }
    }

    أرجع خالی_سوچک;
}
منطقي
StringEnumeration::عامل==(ثابت StringEnumeration& that)ثابت {
    أرجع typeid(*هذا) == typeid(that); 
}

منطقي
StringEnumeration::عامل!=(ثابت StringEnumeration& that)ثابت {
    أرجع !عامل==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_منزل_Adopt, UError_کوڈ &حالت) {
    إذا (U_FAILURE(حالت)) {
        uenum_close(uenum_منزل_Adopt);
        أرجع خالی_سوچک;
    }
    UStringEnumeration *نتیجہ = جديد UStringEnumeration(uenum_منزل_Adopt);
    إذا (نتیجہ == خالی_سوچک) {
        حالت = U_حافظہ_ونڈ_ERROR;
        uenum_close(uenum_منزل_Adopt);
        أرجع خالی_سوچک;
    }
    أرجع نتیجہ;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_کوڈ& حالت) ثابت {
    أرجع uenum_گنتی(uenum, &حالت);
}

ثابت حرف* UStringEnumeration::next(int32_t* نتیجہ_لمائی, UError_کوڈ& حالت) U_LIFETIME_BOUND {
    أرجع uenum_اگلا(uenum, نتیجہ_لمائی, &حالت);
}

ثابت UnicodeString* UStringEnumeration::snext(UError_کوڈ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &حالت);
    إذا (str == خالی_سوچک || U_FAILURE(حالت)) {
        أرجع خالی_سوچک;
    }
    أرجع &unistr.سیٹ_منزل(str, length);
}

فراغ UStringEnumeration::reset(UError_کوڈ& حالت) {
    uenum_reset(uenum, &حالت);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_سرا

// C wrapper --------------------------------------------------------------- ***

#تعریف_کرو THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    مٹاؤ THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن int32_t U_CALLCONV
ustrenum_گنتی(UEnumeration* en,
               UError_کوڈ* ec)
{
    أرجع THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* نتیجہ_لمائی,
               UError_کوڈ* ec)
{
    أرجع THIS(en)->unext(نتیجہ_لمائی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت حرف* U_CALLCONV
ustrenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* نتیجہ_لمائی,
              UError_کوڈ* ec)
{
    أرجع THIS(en)->next(نتیجہ_لمائی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_کوڈ* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساكن ثابت UEnumeration USTRENUM_VT = {
    خالی_سوچک,
    خالی_سوچک, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_گنتی,
    ustrenum_unext,
    ustrenum_اگلا,
    ustrenum_reset
};

U_CDECL_سرا

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_کوڈ* ec) { 
    UEnumeration* نتیجہ = خالی_سوچک;
    إذا (U_SUCCESS(*ec) && adopted != خالی_سوچک) {
        نتیجہ = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        إذا (نتیجہ == خالی_سوچک) {
            *ec = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            uprv_memcpy(نتیجہ, &USTRENUM_VT, حجم(USTRENUM_VT));
            نتیجہ->context = adopted;
        }
    }
    إذا (نتیجہ == خالی_سوچک) {
        مٹاؤ adopted;
    }
    أرجع نتیجہ;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

تعريفنوع بنية UCharStringEnumeration {
    UEnumeration uenum;
    int32_t اشاریہ, count;
} UCharStringEnumeration;

ساكن فراغ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ساكن int32_t U_CALLCONV
ucharstrenum_گنتی(UEnumeration* en,
                   UError_کوڈ* /*ec*/) {
    أرجع ((UCharStringEnumeration*)en)->count;
}

ساكن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمائی,
                  UError_کوڈ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->اشاریہ >= e->count) {
        أرجع خالی_سوچک;
    }
    ثابت char16_t* نتیجہ = ((ثابت char16_t**)e->uenum.context)[e->اشاریہ++];
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = u_strlen(نتیجہ);
    }
    أرجع نتیجہ;
}


ساكن ثابت حرف* U_CALLCONV
ucharstrenum_اگلا(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجہ_لمائی,
                  UError_کوڈ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->اشاریہ >= e->count) {
        أرجع خالی_سوچک;
    }
    ثابت حرف* نتیجہ = ((ثابت حرف**)e->uenum.context)[e->اشاریہ++];
    إذا (نتیجہ_لمائی) {
        *نتیجہ_لمائی = (int32_t)uprv_strlen(نتیجہ);
    }
    أرجع نتیجہ;
}

ساكن فراغ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_کوڈ* /*ec*/) {
    ((UCharStringEnumeration*)en)->اشاریہ = 0;
}

ساكن ثابت UEnumeration UCHARSTRENUM_VT = {
    خالی_سوچک,
    خالی_سوچک, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_گنتی,
    uenum_unextDefault,
    ucharstrenum_اگلا,
    ucharstrenum_reset
};

ساكن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    خالی_سوچک,
    خالی_سوچک, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_گنتی,
    ucharstrenum_unext,
    uenum_اگلا_Default,
    ucharstrenum_reset
};

U_CDECL_سرا

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_لڑیاں_Enumeration(U_LIFETIME_BOUND ثابت حرف* ثابت لڑیاں[],
                                 int32_t count,
                                 UError_کوڈ* ec) {
    UCharStringEnumeration* نتیجہ = خالی_سوچک;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڑیاں != خالی_سوچک)) {
        نتیجہ = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتیجہ == خالی_سوچک) {
            *ec = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتیجہ==(حرف*)(&نتیجہ->uenum));
            uprv_memcpy(نتیجہ, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            نتیجہ->uenum.context = (فراغ*)لڑیاں;
            نتیجہ->اشاریہ = 0;
            نتیجہ->count = count;
        }
    }
    أرجع (UEnumeration*) نتیجہ;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_لڑیاں_Enumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت لڑیاں[],
                                  int32_t count,
                                  UError_کوڈ* ec) {
    UCharStringEnumeration* نتیجہ = خالی_سوچک;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڑیاں != خالی_سوچک)) {
        نتیجہ = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتیجہ == خالی_سوچک) {
            *ec = U_حافظہ_ونڈ_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتیجہ==(حرف*)(&نتیجہ->uenum));
            uprv_memcpy(نتیجہ, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            نتیجہ->uenum.context = (فراغ*)لڑیاں;
            نتیجہ->اشاریہ = 0;
            نتیجہ->count = count;
        }
    }
    أرجع (UEnumeration*) نتیجہ;
}


// end C Wrapper
