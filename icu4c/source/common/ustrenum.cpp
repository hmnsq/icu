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
    : chars(chars_بفر), chars_ظرفیت(حجم(chars_بفر)) {
}

StringEnumeration::~StringEnumeration() {
    إذا (chars != تش_اشاره && chars != chars_بفر) {
        uprv_free(chars);
    }
}

// StringEnumeration base class clone() default implementation, does not clone
StringEnumeration *
StringEnumeration::clone() ثابت {
  أرجع تش_اشاره;
}

ثابت حرف*
StringEnumeration::next(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=تش_اشاره) {
        unistr=*s;
        ensure_Chars_ظرفیت(unistr.length()+1, حالت);
        إذا(U_SUCCESS(حالت)) {
            إذا(پایله_اوږدتیا!=تش_اشاره) {
                *پایله_اوږدتیا=unistr.length();
            }
            unistr.extract(0, INT32_MAX, chars, chars_ظرفیت, US_INV);
            أرجع chars;
        }
    }

    أرجع تش_اشاره;
}

ثابت char16_t*
StringEnumeration::unext(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND {
    ثابت UnicodeString *s=snext(حالت);
    إذا(U_SUCCESS(حالت) && s!=تش_اشاره) {
        unistr=*s;
        إذا(پایله_اوږدتیا!=تش_اشاره) {
            *پایله_اوږدتیا=unistr.length();
        }
        أرجع unistr.get_Terminated_بفر();
    }

    أرجع تش_اشاره;
}

ثابت UnicodeString*
StringEnumeration::snext(UError_کوډ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت حرف *s=next(&length, حالت);
    أرجع سېټ_Chars(s, length, حالت);
}

فراغ
StringEnumeration::ensure_Chars_ظرفیت(int32_t capacity, UError_کوډ &حالت) {
    إذا(U_SUCCESS(حالت) && capacity>chars_ظرفیت) {
        إذا(capacity<(chars_ظرفیت+chars_ظرفیت/2)) {
            // avoid allocation thrashing
            capacity=chars_ظرفیت+chars_ظرفیت/2;
        }
        إذا(chars!=chars_بفر) {
            uprv_free(chars);
        }
        chars = static_cast<حرف*>(uprv_malloc(capacity));
        إذا(chars==تش_اشاره) {
            chars=chars_بفر;
            chars_ظرفیت=حجم(chars_بفر);
            حالت=U_حافظه_ویش_ERROR;
        } وإلا {
            chars_ظرفیت=capacity;
        }
    }
}

UnicodeString*
StringEnumeration::سېټ_Chars(ثابت حرف* s, int32_t length, UError_کوډ& حالت) U_LIFETIME_BOUND {
    إذا(U_SUCCESS(حالت) && s!=تش_اشاره) {
        إذا(length<0) {
            length = static_cast<int32_t>(uprv_strlen(s));
        }

        char16_t *بفر=unistr.get_بفر(length+1);
        إذا(بفر!=تش_اشاره) {
            u_chars_منزل_UChars(s, بفر, length);
            بفر[length]=0;
            unistr.release_بفر(length);
            أرجع &unistr;
        } وإلا {
            حالت=U_حافظه_ویش_ERROR;
        }
    }

    أرجع تش_اشاره;
}
منطقي
StringEnumeration::چلوونکی==(ثابت StringEnumeration& that)ثابت {
    أرجع typeid(*هذا) == typeid(that); 
}

منطقي
StringEnumeration::چلوونکی!=(ثابت StringEnumeration& that)ثابت {
    أرجع !چلوونکی==(that);
}

// UStringEnumeration implementation --------------------------------------- ***

UStringEnumeration * U_EXPORT2
UStringEnumeration::fromUEnumeration(
        UEnumeration *uenum_منزل_Adopt, UError_کوډ &حالت) {
    إذا (U_FAILURE(حالت)) {
        uenum_close(uenum_منزل_Adopt);
        أرجع تش_اشاره;
    }
    UStringEnumeration *پایله = جديد UStringEnumeration(uenum_منزل_Adopt);
    إذا (پایله == تش_اشاره) {
        حالت = U_حافظه_ویش_ERROR;
        uenum_close(uenum_منزل_Adopt);
        أرجع تش_اشاره;
    }
    أرجع پایله;
}

UStringEnumeration::UStringEnumeration(UEnumeration* _uenum) :
    uenum(_uenum) {
    U_ASSERT(_uenum != 0);
}

UStringEnumeration::~UStringEnumeration() {
    uenum_close(uenum);
}

int32_t UStringEnumeration::count(UError_کوډ& حالت) ثابت {
    أرجع uenum_شمېر(uenum, &حالت);
}

ثابت حرف* UStringEnumeration::next(int32_t* پایله_اوږدتیا, UError_کوډ& حالت) U_LIFETIME_BOUND {
    أرجع uenum_بل(uenum, پایله_اوږدتیا, &حالت);
}

ثابت UnicodeString* UStringEnumeration::snext(UError_کوډ& حالت) U_LIFETIME_BOUND {
    int32_t length;
    ثابت char16_t* str = uenum_unext(uenum, &length, &حالت);
    إذا (str == تش_اشاره || U_FAILURE(حالت)) {
        أرجع تش_اشاره;
    }
    أرجع &unistr.سېټ_منزل(str, length);
}

فراغ UStringEnumeration::reset(UError_کوډ& حالت) {
    uenum_reset(uenum, &حالت);
}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UStringEnumeration)
U_NAMESPACE_پای_ټکی

// C wrapper --------------------------------------------------------------- ***

#تعریف_کړه THIS(en) ((icu::StringEnumeration*)(en->context))

U_CDECL_BEGIN

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_close(UEnumeration* en) {
    ړنګول THIS(en);
    uprv_free(en);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن int32_t U_CALLCONV
ustrenum_شمېر(UEnumeration* en,
               UError_کوډ* ec)
{
    أرجع THIS(en)->count(*ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت char16_t* U_CALLCONV
ustrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
               int32_t* پایله_اوږدتیا,
               UError_کوډ* ec)
{
    أرجع THIS(en)->unext(پایله_اوږدتیا, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن ثابت حرف* U_CALLCONV
ustrenum_بل(UEnumeration* en U_LIFETIME_BOUND,
              int32_t* پایله_اوږدتیا,
              UError_کوډ* ec)
{
    أرجع THIS(en)->next(پایله_اوږدتیا, *ec);
}

/**
 * Wrapper API to make StringEnumeration look like UEnumeration.
 */
ساكن فراغ U_CALLCONV
ustrenum_reset(UEnumeration* en,
               UError_کوډ* ec)
{
    THIS(en)->reset(*ec);
}

/**
 * Pseudo-vtable for UEnumeration wrapper around StringEnumeration.
 * The StringEnumeration pointer will be stored in 'context'.
 */
ساكن ثابت UEnumeration USTRENUM_VT = {
    تش_اشاره,
    تش_اشاره, // store StringEnumeration pointer here
    ustrenum_close,
    ustrenum_شمېر,
    ustrenum_unext,
    ustrenum_بل,
    ustrenum_reset
};

U_CDECL_پای_ټکی

/**
 * Given a StringEnumeration, wrap it in a UEnumeration.  The
 * StringEnumeration is adopted; after this call, the caller must not
 * delete it (regardless of error status).
 */
U_CAPI UEnumeration* U_EXPORT2
uenum_openFromStringEnumeration(icu::StringEnumeration* adopted, UError_کوډ* ec) { 
    UEnumeration* پایله = تش_اشاره;
    إذا (U_SUCCESS(*ec) && adopted != تش_اشاره) {
        پایله = (UEnumeration*) uprv_malloc(حجم(UEnumeration));
        إذا (پایله == تش_اشاره) {
            *ec = U_حافظه_ویش_ERROR;
        } وإلا {
            uprv_memcpy(پایله, &USTRENUM_VT, حجم(USTRENUM_VT));
            پایله->context = adopted;
        }
    }
    إذا (پایله == تش_اشاره) {
        ړنګول adopted;
    }
    أرجع پایله;
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
ucharstrenum_شمېر(UEnumeration* en,
                   UError_کوډ* /*ec*/) {
    أرجع ((UCharStringEnumeration*)en)->count;
}

ساكن ثابت char16_t* U_CALLCONV
ucharstrenum_unext(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* پایله_اوږدتیا,
                  UError_کوډ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->شاخص >= e->count) {
        أرجع تش_اشاره;
    }
    ثابت char16_t* پایله = ((ثابت char16_t**)e->uenum.context)[e->شاخص++];
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = u_strlen(پایله);
    }
    أرجع پایله;
}


ساكن ثابت حرف* U_CALLCONV
ucharstrenum_بل(UEnumeration* en U_LIFETIME_BOUND,
                  int32_t* پایله_اوږدتیا,
                  UError_کوډ* /*ec*/) {
    UCharStringEnumeration *e = (UCharStringEnumeration*) en;
    إذا (e->شاخص >= e->count) {
        أرجع تش_اشاره;
    }
    ثابت حرف* پایله = ((ثابت حرف**)e->uenum.context)[e->شاخص++];
    إذا (پایله_اوږدتیا) {
        *پایله_اوږدتیا = (int32_t)uprv_strlen(پایله);
    }
    أرجع پایله;
}

ساكن فراغ U_CALLCONV
ucharstrenum_reset(UEnumeration* en,
                   UError_کوډ* /*ec*/) {
    ((UCharStringEnumeration*)en)->شاخص = 0;
}

ساكن ثابت UEnumeration UCHARSTRENUM_VT = {
    تش_اشاره,
    تش_اشاره, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمېر,
    uenum_unextDefault,
    ucharstrenum_بل,
    ucharstrenum_reset
};

ساكن ثابت UEnumeration UCHARSTRENUM_U_VT = {
    تش_اشاره,
    تش_اشاره, // store StringEnumeration pointer here
    ucharstrenum_close,
    ucharstrenum_شمېر,
    ucharstrenum_unext,
    uenum_بل_Default,
    ucharstrenum_reset
};

U_CDECL_پای_ټکی

U_CAPI UEnumeration* U_EXPORT2
uenum_open_Char_مزي_ګانې_Enumeration(U_LIFETIME_BOUND ثابت حرف* ثابت مزي_ګانې[],
                                 int32_t count,
                                 UError_کوډ* ec) {
    UCharStringEnumeration* پایله = تش_اشاره;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || مزي_ګانې != تش_اشاره)) {
        پایله = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (پایله == تش_اشاره) {
            *ec = U_حافظه_ویش_ERROR;
        } وإلا {
            U_ASSERT((حرف*)پایله==(حرف*)(&پایله->uenum));
            uprv_memcpy(پایله, &UCHARSTRENUM_VT, حجم(UCHARSTRENUM_VT));
            پایله->uenum.context = (فراغ*)مزي_ګانې;
            پایله->شاخص = 0;
            پایله->count = count;
        }
    }
    أرجع (UEnumeration*) پایله;
}

U_CAPI UEnumeration* U_EXPORT2
uenum_open_UChar_مزي_ګانې_Enumeration(U_LIFETIME_BOUND ثابت char16_t* ثابت مزي_ګانې[],
                                  int32_t count,
                                  UError_کوډ* ec) {
    UCharStringEnumeration* پایله = تش_اشاره;
    إذا (U_SUCCESS(*ec) && count >= 0 && (count == 0 || مزي_ګانې != تش_اشاره)) {
        پایله = (UCharStringEnumeration*) uprv_malloc(حجم(UCharStringEnumeration));
        إذا (پایله == تش_اشاره) {
            *ec = U_حافظه_ویش_ERROR;
        } وإلا {
            U_ASSERT((حرف*)پایله==(حرف*)(&پایله->uenum));
            uprv_memcpy(پایله, &UCHARSTRENUM_U_VT, حجم(UCHARSTRENUM_U_VT));
            پایله->uenum.context = (فراغ*)مزي_ګانې;
            پایله->شاخص = 0;
            پایله->count = count;
        }
    }
    أرجع (UEnumeration*) پایله;
}


// end C Wrapper
