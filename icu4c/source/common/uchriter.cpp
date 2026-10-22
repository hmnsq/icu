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
  متن(خالی_سوچک)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(متن_Ptr != خالی_سوچک ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالی_سوچک ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t متن_Begin,
                                               int32_t متن_سرا,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالی_سوچک ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0,
                      متن_Begin, متن_سرا, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ثابت UCharCharacterIterator& that)
: CharacterIterator(that),
  متن(that.متن)
{
}

UCharCharacterIterator&
UCharCharacterIterator::عامل=(ثابت UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::عامل=(that);
    متن = that.متن;
    أرجع *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::عامل==(ثابت ForwardCharacterIterator& that) ثابت {
    إذا (هذا == &that) {
        أرجع سچ;
    }
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    أرجع متن == realThat.متن
        && متن_لمائی == realThat.متن_لمائی
        && pos == realThat.pos
        && begin == realThat.begin
        && سرا == realThat.سرا;
}

int32_t
UCharCharacterIterator::hash_کوڈ() ثابت {
    أرجع ustr_hashUCharsN(متن, متن_لمائی) ^ pos ^ begin ^ سرا;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    أرجع جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    إذا(pos < سرا) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::پہلا_Post_Inc() {
    pos = begin;
    إذا(pos < سرا) {
        أرجع متن[pos++];
    } وإلا {
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::آخری() {
    pos = سرا;
    إذا(pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::سیٹ_اشاریہ(int32_t position) {
    إذا(position < begin) {
        pos = begin;
    } وإلا إذا(position > سرا) {
        pos = سرا;
    } وإلا {
        pos = position;
    }
    إذا(pos < سرا) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    إذا (pos >= begin && pos < سرا) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    إذا (pos + 1 < سرا) {
        أرجع متن[++pos];
    } وإلا {
        /* make current() return DONE */
        pos = سرا;
        أرجع پورا_2;
    }
}

char16_t
UCharCharacterIterator::اگلا_Post_Inc() {
    إذا (pos < سرا) {
        أرجع متن[pos++];
    } وإلا {
        أرجع پورا_2;
    }
}

UBool
UCharCharacterIterator::has_اگلا() {
    أرجع pos < سرا;
}

char16_t
UCharCharacterIterator::پچھلا() {
    إذا (pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع پورا_2;
    }
}

UBool
UCharCharacterIterator::has_پچھلا() {
    أرجع pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    إذا(pos < سرا) {
        int32_t i = pos;
        UChar32 c;
        U16_اگلا(متن, i, سرا, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    إذا(pos < سرا) {
        UChar32 c;
        U16_اگلا(متن, pos, سرا, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = سرا;
    إذا(pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::سیٹ_Index32(int32_t position) {
    إذا(position < begin) {
        position = begin;
    } وإلا إذا(position > سرا) {
        position = سرا;
    }
    إذا(position < سرا) {
        U16_سیٹ_CP_شروع(متن, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_اگلا(متن, i, سرا, c);
        أرجع c;
    } وإلا {
        هذا->pos = position;
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    إذا (pos >= begin && pos < سرا) {
        UChar32 c;
        U16_GET(متن, begin, pos, سرا, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    إذا (pos < سرا) {
        U16_FWD_1(متن, pos, سرا);
        إذا(pos < سرا) {
            int32_t i = pos;
            UChar32 c;
            U16_اگلا(متن, i, سرا, c);
            أرجع c;
        }
    }
    /* make current() return DONE */
    pos = سرا;
    أرجع پورا_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    إذا (pos < سرا) {
        UChar32 c;
        U16_اگلا(متن, pos, سرا, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    إذا (pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع پورا_2;
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
    حالة k_سرا:
        pos = سرا + delta;
        اكسر;
    افتراضي:
        اكسر;
    }

    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > سرا) {
        pos = سرا;
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
            U16_FWD_N(متن, pos, سرا, delta);
        }
        اكسر;
    حالة kCurrent:
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, سرا, delta);
        } وإلا {
            U16_پٹھ_N(متن, begin, pos, -delta);
        }
        اكسر;
    حالة k_سرا:
        pos = سرا;
        إذا(delta < 0) {
            U16_پٹھ_N(متن, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    أرجع pos;
}

فراغ UCharCharacterIterator::سیٹ_متن(ConstChar16Ptr new_متن,
                                     int32_t      new_متن_لمائی) {
    متن = new_متن;
    إذا (new_متن == خالی_سوچک || new_متن_لمائی < 0) {
        new_متن_لمائی = 0;
    }
    سرا = متن_لمائی = new_متن_لمائی;
    pos = begin = 0;
}

فراغ
UCharCharacterIterator::get_متن(UnicodeString& نتیجہ) {
    نتیجہ = UnicodeString(متن, متن_لمائی);
}

U_NAMESPACE_سرا
