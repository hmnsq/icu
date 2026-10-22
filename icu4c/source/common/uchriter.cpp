// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ضمن "utypeinfo.h"  // for 'typeid' to work

#ضمن "unicode/uchriter.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/utf16.h"
#ضمن "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  متن(اشاره_تهی)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(متن_Ptr != اشاره_تهی ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != اشاره_تهی ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t متن_Begin,
                                               int32_t متن_انتها,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != اشاره_تهی ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0,
                      متن_Begin, متن_انتها, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ثابت UCharCharacterIterator& that)
: CharacterIterator(that),
  متن(that.متن)
{
}

UCharCharacterIterator&
UCharCharacterIterator::عملگر=(ثابت UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::عملگر=(that);
    متن = that.متن;
    أرجع *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::عملگر==(ثابت ForwardCharacterIterator& that) ثابت {
    إذا (هذا == &that) {
        أرجع درست;
    }
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    أرجع متن == realThat.متن
        && متن_درازی == realThat.متن_درازی
        && pos == realThat.pos
        && begin == realThat.begin
        && انتها == realThat.انتها;
}

int32_t
UCharCharacterIterator::hash_کد() ثابت {
    أرجع ustr_hashUCharsN(متن, متن_درازی) ^ pos ^ begin ^ انتها;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    أرجع جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    إذا(pos < انتها) {
        أرجع متن[pos];
    } وإلا {
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::اولی_Post_Inc() {
    pos = begin;
    إذا(pos < انتها) {
        أرجع متن[pos++];
    } وإلا {
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::آخرین() {
    pos = انتها;
    إذا(pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::مجموعه_شاخص(int32_t position) {
    إذا(position < begin) {
        pos = begin;
    } وإلا إذا(position > انتها) {
        pos = انتها;
    } وإلا {
        pos = position;
    }
    إذا(pos < انتها) {
        أرجع متن[pos];
    } وإلا {
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    إذا (pos >= begin && pos < انتها) {
        أرجع متن[pos];
    } وإلا {
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    إذا (pos + 1 < انتها) {
        أرجع متن[++pos];
    } وإلا {
        /* make current() return DONE */
        pos = انتها;
        أرجع انجامشده_2;
    }
}

char16_t
UCharCharacterIterator::بعدی_Post_Inc() {
    إذا (pos < انتها) {
        أرجع متن[pos++];
    } وإلا {
        أرجع انجامشده_2;
    }
}

UBool
UCharCharacterIterator::has_بعدی() {
    أرجع pos < انتها;
}

char16_t
UCharCharacterIterator::قبلی() {
    إذا (pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع انجامشده_2;
    }
}

UBool
UCharCharacterIterator::has_قبلی() {
    أرجع pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    إذا(pos < انتها) {
        int32_t i = pos;
        UChar32 c;
        U16_بعدی(متن, i, انتها, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    إذا(pos < انتها) {
        UChar32 c;
        U16_بعدی(متن, pos, انتها, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = انتها;
    إذا(pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::مجموعه_Index32(int32_t position) {
    إذا(position < begin) {
        position = begin;
    } وإلا إذا(position > انتها) {
        position = انتها;
    }
    إذا(position < انتها) {
        U16_مجموعه_CP_شروع(متن, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_بعدی(متن, i, انتها, c);
        أرجع c;
    } وإلا {
        هذا->pos = position;
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    إذا (pos >= begin && pos < انتها) {
        UChar32 c;
        U16_GET(متن, begin, pos, انتها, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    إذا (pos < انتها) {
        U16_FWD_1(متن, pos, انتها);
        إذا(pos < انتها) {
            int32_t i = pos;
            UChar32 c;
            U16_بعدی(متن, i, انتها, c);
            أرجع c;
        }
    }
    /* make current() return DONE */
    pos = انتها;
    أرجع انجامشده_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    إذا (pos < انتها) {
        UChar32 c;
        U16_بعدی(متن, pos, انتها, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    إذا (pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع انجامشده_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    بدل(origin) {
    حالة k_شروع:
        pos = begin + delta;
        اكسر;
    حالة kCurrent:
        pos += delta;
        اكسر;
    حالة k_انتها:
        pos = انتها + delta;
        اكسر;
    افتراضي:
        اكسر;
    }

    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > انتها) {
        pos = انتها;
    }

    أرجع pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة k_شروع:
        pos = begin;
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, انتها, delta);
        }
        اكسر;
    حالة kCurrent:
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, انتها, delta);
        } وإلا {
            U16_پشت_N(متن, begin, pos, -delta);
        }
        اكسر;
    حالة k_انتها:
        pos = انتها;
        إذا(delta < 0) {
            U16_پشت_N(متن, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    أرجع pos;
}

فراغ UCharCharacterIterator::مجموعه_متن(ConstChar16Ptr new_متن,
                                     int32_t      new_متن_درازی) {
    متن = new_متن;
    إذا (new_متن == اشاره_تهی || new_متن_درازی < 0) {
        new_متن_درازی = 0;
    }
    انتها = متن_درازی = new_متن_درازی;
    pos = begin = 0;
}

فراغ
UCharCharacterIterator::get_متن(UnicodeString& نتیجه) {
    نتیجه = UnicodeString(متن, متن_درازی);
}

U_NAMESPACE_انتها
