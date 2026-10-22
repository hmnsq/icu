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
    : chars(chars_میانگیر), chars_ظرفیت(حجم(chars_میانگیر)) {
}

StringEnumeration::~StringEnumeration() {
    إذا (chars != اشاره_تهی && chars != chars_میانگیر) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  أرجع اشاره_تهی;
}

ثابت حرف*
StringEnumeration::next(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(وضعیت);
    إذا(U_SUCCESS(وضعیت) && s!=اشاره_تهی) {
        unistr=*s;
        ensure_Chars_ظرفیت(unistr.length()+1, وضعیت);
        إذا(U_SUCCESS(وضعیت)) {
            إذا(نتیجه_درازی!=اشاره_تهی) {
                *نتیجه_درازی=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_ظرفیت, US_INV);
            أرجع chars;
        }
    }

    أرجع اشاره_تهی;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(وضعیت);
    إذا(U_SUCCESS(وضعیت) && s!=اشاره_تهی) {
        unistr=*s;
        إذا(نتیجه_درازی!=اشاره_تهی) {
            *نتیجه_درازی=unistr.length();
        }
        أرجع unistr.get_Terminated_میانگیر();
    }

    أرجع اشاره_تهی;
}

ثابت UnicodeString*
StringEnumeration::snext(UError_کد& وضعیت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, وضعیت);
    أرجع مجموعه_Chars(s, length, وضعیت);
}

فراغ
StringEnumeration::ensure_Chars_ظرفیت(int32_t capacity, UError_کد &وضعیت) {
    إذا(U_SUCCESS(وضعیت) && capacity>chars_ظرفیت) {
        إذا(capacity<(chars_ظرفیت+chars_ظرفیت/2)) {
            // avoid allocation thrashing
            capacity=chars_ظرفیت+chars_ظرفیت/2;
        }
        إذا(chars!=chars_میانگیر) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        إذا(chars==اشاره_تهی) {
            chars=chars_میانگیر;
            chars_ظرفیت=حجم(chars_میانگیر);
            وضعیت=U_حافظه_تخصیص_ERROR;
        } وإلا {
            chars_ظرفیت=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::مجموعه_Chars(ثابت حرف* s, int32_t length, UError_کد& وضعیت) U_LIFETIME_BOUND {
    إذا(U_SUCCESS(وضعیت) && s!=اشاره_تهی) {
        إذا(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *میانگیر=unistr.get_میانگیر(length+1);
        إذا(میانگیر!=اشاره_تهی) {
            u_chars_مقصد_UChars(s, میانگیر, length);
            میانگیر[length]=0;
            unistr.release_میانگیر(length);
            أرجع &unistr;
        } وإلا {
            وضعیت=U_حافظه_تخصیص_ERROR;
        }
    }

    أرجع اشاره_تهی;
}
منطقي
StringEnumeration::عملگر==(ثابت StringEnumeration& that)ثابت {
    أرجع typeid(*هذا) == typeid(that); 
}

منطقي
StringEnumeration::عملگر!=(ثابت StringEnumeration& that)ثابت {
    أرجع !عملگر==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_مقصد_Adopt, UError_کد &وضعیت) {
    إذا (U_FAILURE(وضعیت)) {
        uenum_close(uenum_مقصد_Adopt);
        أرجع اشاره_تهی;
    }
    UStringEnumeration *نتیجه = جديد UStringEnumeration(uenum_مقصد_Adopt);
    إذا (نتیجه == اشاره_تهی) {
        وضعیت = U_حافظه_تخصیص_ERROR;
        uenum_close(uenum_مقصد_Adopt);
        أرجع اشاره_تهی;
    }
    أرجع نتیجه;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_کد& وضعیت) ثابت {
    أرجع uenum_شمار(uenum, &وضعیت);
}

ثابت حرف* UStringEnumeration::next(int32_t* نتیجه_درازی, UError_کد& وضعیت) U_LIFETIME_BOUND {
    أرجع uenum_بعدی(uenum, نتیجه_درازی, &وضعیت);
}

ثابت UnicodeString* UStringEnumeration::snext(UError_کد& وضعیت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &وضعیت);
    إذا (str == اشاره_تهی || U_FAILURE(وضعیت)) {
        أرجع اشاره_تهی;
    }
    أرجع &unistr.مجموعه_مقصد(str, length);
}

فراغ UStringEnumeration::reset(UError_کد& وضعیت) {
    uenum_reset(uenum, &وضعیت);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_انتها

// C wrapper --------------------------------------------------------------- ***

#تعریف_کن THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    حذف THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن int32_t U_CALLCONV
ustrenum_شمار(UEnumeration* en,
               UError_کد* ec)
{
    أرجع THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* نتیجه_درازی,
               UError_کد* ec)
{
    أرجع THIS(en)->unext(نتیجه_درازی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت حرف* U_CALLCONV
ustrenum_بعدی(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* نتیجه_درازی,
              UError_کد* ec)
{
    أرجع THIS(en)->next(نتیجه_درازی, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_کد* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساكن ثابت UEnumeration USTRENUM_VT = {
    اشاره_تهی,
    اشاره_تهی, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_شمار,
    ustrenum_unext,
    ustrenum_بعدی,
    ustrenum_reset
};

U_CDECL_انتها

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_کد* ec) { 
    UEnumeration* نتیجه = اشاره_تهی;
    إذا (U_SUCCESS(*ec) && adopted != اشاره_تهی) {
        نتیجه = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        إذا (نتیجه == اشاره_تهی) {
            *ec = U_حافظه_تخصیص_ERROR;
        } وإلا {
            uprv_memcpy(نتیجه, &USTRENUM_VT, حجم(USTRENUM_VT));
            نتیجه->context = adopted;
        }
    }
    إذا (نتیجه == اشاره_تهی) {
        حذف adopted;
    }
    أرجع نتیجه;
}

// C wrapper --------------------------------------------------------------- ***

U_CDECL_BEGIN

تعريفنوع بنية UCharStringEnumeration {
    UEnumeration uenum;
    int32_t شاخص, count;
} UCharStringEnumeration;

ساكن فراغ U_CALLCONV
ucharstrenum_close(UEnumeration* en) {
    uprv_free(en);
}

ساكن int32_t U_CALLCONV
ucharstrenum_شمار(UEnumeration* en,
                   UError_کد* /*ec*/) {
    أرجع ((UCharStringEnumeration*)en)->count;
}

ساكن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجه_درازی,
                  UError_کد* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->شاخص >= e->count) {
        أرجع اشاره_تهی;
    }
    ثابت char16_t* نتیجه = ((ثابت char16_t**)e->uenum.context)[e->شاخص++];
    إذا (نتیجه_درازی) {
        *نتیجه_درازی = u_strlen(نتیجه);
    }
    أرجع نتیجه;
}


ساكن ثابت حرف* U_CALLCONV
ucharstrenum_بعدی(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* نتیجه_درازی,
                  UError_کد* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->شاخص >= e->count) {
        أرجع اشاره_تهی;
    }
    ثابت حرف* نتیجه = ((ثابت حرف**)e->uenum.context)[e->شاخص++];
    إذا (نتیجه_درازی) {
        *نتیجه_درازی = (int32_t)uprv_strlen(نتیجه);
    }
    أرجع نتیجه;
}

ساكن فراغ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_کد* /*ec*/) {
    ((UCharStringEnumeration*)en)->شاخص = 0;
}

ساكن ثابت UEnumeration UCHARSTRENUM_VT = {
    اشاره_تهی,
    اشاره_تهی, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمار,
    uenum_unextDefault,
    ucharstrenum_بعدی,
    ucharstrenum_reset
};

ساكن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    اشاره_تهی,
    اشاره_تهی, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمار,
    ucharstrenum_unext,
    uenum_بعدی_Default,
    ucharstrenum_reset
};

U_CDECL_انتها

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_رشتهها_Enumeration(U_LIFETIME_BOUND ثابت حرف* ثابت رشتهها[],
                                 int32_t count,
                                 UError_کد* ec) {
    UCharStringEnumeration* نتیجه = اشاره_تهی;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || رشتهها != اشاره_تهی)) {
        نتیجه = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتیجه == اشاره_تهی) {
            *ec = U_حافظه_تخصیص_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتیجه==(حرف*)(&نتیجه->uenum));
            uprv_memcpy(نتیجه, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            نتیجه->uenum.context = (فراغ*)رشتهها;
            نتیجه->شاخص = 0;
            نتیجه->count = count;
        }
    }
    أرجع (UEnumeration*) نتیجه;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_رشتهها_Enumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت رشتهها[],
                                  int32_t count,
                                  UError_کد* ec) {
    UCharStringEnumeration* نتیجه = اشاره_تهی;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || رشتهها != اشاره_تهی)) {
        نتیجه = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (نتیجه == اشاره_تهی) {
            *ec = U_حافظه_تخصیص_ERROR;
        } وإلا {
            U_ASSERT((حرف*)نتیجه==(حرف*)(&نتیجه->uenum));
            uprv_memcpy(نتیجه, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            نتیجه->uenum.context = (فراغ*)رشتهها;
            نتیجه->شاخص = 0;
            نتیجه->count = count;
        }
    }
    أرجع (UEnumeration*) نتیجه;
}


// end C Wrapper
