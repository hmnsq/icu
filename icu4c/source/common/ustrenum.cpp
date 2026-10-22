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
    : chars(chars_بفر), chars_صلاحيت(حجم(chars_بفر)) {
}

StringEnumeration::~StringEnumeration() {
    إذا (chars != خالي_اشارو && chars != chars_بفر) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  أرجع خالي_اشارو;
}

ثابت حرف*
StringEnumeration::next(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=خالي_اشارو) {
        unistr=*s;
        ensure_Chars_صلاحيت(unistr.length()+1, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(نتيجو_ڊيگهائي!=خالي_اشارو) {
                *نتيجو_ڊيگهائي=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_صلاحيت, US_INV);
            أرجع chars;
        }
    }

    أرجع خالي_اشارو;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=خالي_اشارو) {
        unistr=*s;
        إذا(نتيجو_ڊيگهائي!=خالي_اشارو) {
            *نتيجو_ڊيگهائي=unistr.length();
        }
        أرجع unistr.get_Terminated_بفر();
    }

    أرجع خالي_اشارو;
}

ثابت UnicodeString*
StringEnumeration::snext(UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, حالت);
    أرجع سيٽ_Chars(s, length, حالت);
}

فراغ
StringEnumeration::ensure_Chars_صلاحيت(int32_t capacity, UError_ڪوڊ &حالت) {
    إذا(U_SUCCESS(حالت) && capacity>chars_صلاحيت) {
        إذا(capacity<(chars_صلاحيت+chars_صلاحيت/2)) {
            // avoid allocation thrashing
            capacity=chars_صلاحيت+chars_صلاحيت/2;
        }
        إذا(chars!=chars_بفر) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        إذا(chars==خالي_اشارو) {
            chars=chars_بفر;
            chars_صلاحيت=حجم(chars_بفر);
            حالت=U_يادگيري_ورهاست_ERROR;
        } وإلا {
            chars_صلاحيت=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::سيٽ_Chars(ثابت حرف* s, int32_t length, UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    إذا(U_SUCCESS(حالت) && s!=خالي_اشارو) {
        إذا(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *بفر=unistr.get_بفر(length+1);
        إذا(بفر!=خالي_اشارو) {
            u_chars_منزل_UChars(s, بفر, length);
            بفر[length]=0;
            unistr.release_بفر(length);
            أرجع &unistr;
        } وإلا {
            حالت=U_يادگيري_ورهاست_ERROR;
        }
    }

    أرجع خالي_اشارو;
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
        UEnumeration *uenum_منزل_Adopt, UError_ڪوڊ &حالت) {
    إذا (U_FAILURE(حالت)) {
        uenum_close(uenum_منزل_Adopt);
        أرجع خالي_اشارو;
    }
    UStringEnumeration *نتيجو = جديد UStringEnumeration(uenum_منزل_Adopt);
    إذا (نتيجو == خالي_اشارو) {
        حالت = U_يادگيري_ورهاست_ERROR;
        uenum_close(uenum_منزل_Adopt);
        أرجع خالي_اشارو;
    }
    أرجع نتيجو;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_ڪوڊ& حالت) ثابت {
    أرجع uenum_ڳڻپ(uenum, &حالت);
}

ثابت حرف* UStringEnumeration::next(int32_t* نتيجو_ڊيگهائي, UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    أرجع uenum_ايندڙ(uenum, نتيجو_ڊيگهائي, &حالت);
}

ثابت UnicodeString* UStringEnumeration::snext(UError_ڪوڊ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &حالت);
    إذا (str == خالي_اشارو || U_FAILURE(حالت)) {
        أرجع خالي_اشارو;
    }
    أرجع &unistr.سيٽ_منزل(str, length);
}

فراغ UStringEnumeration::reset(UError_ڪوڊ& حالت) {
    uenum_reset(uenum, &حالت);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_پڇاڙي

// C wrapper --------------------------------------------------------------- ***

#وصف_ڪريو THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    ميٽيو THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن int32_t U_CALLCONV
ustrenum_ڳڻپ(UEnumeration* en,
               UError_ڪوڊ* ec)
{
    أرجع THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* نتيجو_ڊيگهائي,
               UError_ڪوڊ* ec)
{
    أرجع THIS(en)->unext(نتيجو_ڊيگهائي, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت حرف* U_CALLCONV
ustrenum_ايندڙ(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* نتيجو_ڊيگهائي,
              UError_ڪوڊ* ec)
{
    أرجع THIS(en)->next(نتيجو_ڊيگهائي, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_ڪوڊ* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساكن ثابت UEnumeration USTRENUM_VT = {
    خالي_اشارو,
    خالي_اشارو, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_ڳڻپ,
    ustrenum_unext,
    ustrenum_ايندڙ,
    ustrenum_reset
};

U_CDECL_پڇاڙي

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_ڪوڊ* ec) { 
    UEnumeration* نتيجو = خالي_اشارو;
    إذا (U_SUCCESS(*ec) && adopted != خالي_اشارو) {
        نتيجو = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        إذا (نتيجو == خالي_اشارو) {
            *ec = U_يادگيري_ورهاست_ERROR;
        } وإلا {
            uprv_memcpy(نتيجو, &USTRENUM_VT, حجم(USTRENUM_VT));
            نتيجو->context = adopted;
        }
    }
    إذا (نتيجو == خالي_اشارو) {
        ميٽيو adopted;
    }
    أرجع نتيجو;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

تعريفنوع بنية UCharStringEnumeration {
    UEnumeration uenum;
    int32_t اشاريو, count;
} UCharStringEnumeration;

ساكن فراغ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ساكن int32_t U_CALLCONV
ucharstrenum_ڳڻپ(UEnumeration* en,
                   UError_ڪوڊ* /*ec*/) {
    أرجع ((UCharStringEnumeration*)en)->count;
}

ساكن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتيجو_ڊيگهائي,
                  UError_ڪوڊ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->اشاريو >= e->count) {
        أرجع خالي_اشارو;
    }
    ثابت char16_t* نتيجو = ((ثابت char16_t**)e->uenum.context)[e->اشاريو++];
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = u_strlen(نتيجو);
    }
    أرجع نتيجو;
}


ساكن ثابت حرف* U_CALLCONV
ucharstrenum_ايندڙ(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتيجو_ڊيگهائي,
                  UError_ڪوڊ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->اشاريو >= e->count) {
        أرجع خالي_اشارو;
    }
    ثابت حرف* نتيجو = ((ثابت حرف**)e->uenum.context)[e->اشاريو++];
    إذا (نتيجو_ڊيگهائي) {
        *نتيجو_ڊيگهائي = (int32_t)uprv_strlen(نتيجو);
    }
    أرجع نتيجو;
}

ساكن فراغ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_ڪوڊ* /*ec*/) {
    ((UCharStringEnumeration*)en)->اشاريو = 0;
}

ساكن ثابت UEnumeration UCHARSTRENUM_VT = {
    خالي_اشارو,
    خالي_اشارو, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ڳڻپ,
    uenum_unextDefault,
    ucharstrenum_ايندڙ,
    ucharstrenum_reset
};

ساكن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    خالي_اشارو,
    خالي_اشارو, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_ڳڻپ,
    ucharstrenum_unext,
    uenum_ايندڙ_Default,
    ucharstrenum_reset
};

U_CDECL_پڇاڙي

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_لڙيون_Enumeration(U_LIFETIME_BOUND ثابت حرف* ثابت لڙيون[],
                                 int32_t count,
                                 UError_ڪوڊ* ec) {
    UCharStringEnumeration* نتيجو = خالي_اشارو;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڙيون != خالي_اشارو)) {
        نتيجو = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتيجو == خالي_اشارو) {
            *ec = U_يادگيري_ورهاست_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتيجو==(حرف*)(&نتيجو->uenum));
            uprv_memcpy(نتيجو, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            نتيجو->uenum.context = (فراغ*)لڙيون;
            نتيجو->اشاريو = 0;
            نتيجو->count = count;
        }
    }
    أرجع (UEnumeration*) نتيجو;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_لڙيون_Enumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت لڙيون[],
                                  int32_t count,
                                  UError_ڪوڊ* ec) {
    UCharStringEnumeration* نتيجو = خالي_اشارو;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || لڙيون != خالي_اشارو)) {
        نتيجو = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتيجو == خالي_اشارو) {
            *ec = U_يادگيري_ورهاست_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتيجو==(حرف*)(&نتيجو->uenum));
            uprv_memcpy(نتيجو, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            نتيجو->uenum.context = (فراغ*)لڙيون;
            نتيجو->اشاريو = 0;
            نتيجو->count = count;
        }
    }
    أرجع (UEnumeration*) نتيجو;
}


// end C Wrapper
