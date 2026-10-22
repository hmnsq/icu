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
  متن(خالي_اشارو)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(متن_Ptr != خالي_اشارو ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالي_اشارو ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t متن_Begin,
                                               int32_t متن_پڇاڙي,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالي_اشارو ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0,
                      متن_Begin, متن_پڇاڙي, position),
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
        && متن_ڊيگهائي == realThat.متن_ڊيگهائي
        && pos == realThat.pos
        && begin == realThat.begin
        && پڇاڙي == realThat.پڇاڙي;
}

int32_t
UCharCharacterIterator::hash_ڪوڊ() ثابت {
    أرجع ustr_hashUCharsN(متن, متن_ڊيگهائي) ^ pos ^ begin ^ پڇاڙي;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    أرجع جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    إذا(pos < پڇاڙي) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::پهريون_Post_Inc() {
    pos = begin;
    إذا(pos < پڇاڙي) {
        أرجع متن[pos++];
    } وإلا {
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::آخري() {
    pos = پڇاڙي;
    إذا(pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::سيٽ_اشاريو(int32_t position) {
    إذا(position < begin) {
        pos = begin;
    } وإلا إذا(position > پڇاڙي) {
        pos = پڇاڙي;
    } وإلا {
        pos = position;
    }
    إذا(pos < پڇاڙي) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    إذا (pos >= begin && pos < پڇاڙي) {
        أرجع متن[pos];
    } وإلا {
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    إذا (pos + 1 < پڇاڙي) {
        أرجع متن[++pos];
    } وإلا {
        /* make current() return DONE */
        pos = پڇاڙي;
        أرجع پورو_2;
    }
}

char16_t
UCharCharacterIterator::ايندڙ_Post_Inc() {
    إذا (pos < پڇاڙي) {
        أرجع متن[pos++];
    } وإلا {
        أرجع پورو_2;
    }
}

UBool
UCharCharacterIterator::has_ايندڙ() {
    أرجع pos < پڇاڙي;
}

char16_t
UCharCharacterIterator::اڳيون() {
    إذا (pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع پورو_2;
    }
}

UBool
UCharCharacterIterator::has_اڳيون() {
    أرجع pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    إذا(pos < پڇاڙي) {
        int32_t i = pos;
        UChar32 c;
        U16_ايندڙ(متن, i, پڇاڙي, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    إذا(pos < پڇاڙي) {
        UChar32 c;
        U16_ايندڙ(متن, pos, پڇاڙي, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = پڇاڙي;
    إذا(pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::سيٽ_Index32(int32_t position) {
    إذا(position < begin) {
        position = begin;
    } وإلا إذا(position > پڇاڙي) {
        position = پڇاڙي;
    }
    إذا(position < پڇاڙي) {
        U16_سيٽ_CP_شروعات(متن, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_ايندڙ(متن, i, پڇاڙي, c);
        أرجع c;
    } وإلا {
        هذا->pos = position;
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    إذا (pos >= begin && pos < پڇاڙي) {
        UChar32 c;
        U16_GET(متن, begin, pos, پڇاڙي, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    إذا (pos < پڇاڙي) {
        U16_FWD_1(متن, pos, پڇاڙي);
        إذا(pos < پڇاڙي) {
            int32_t i = pos;
            UChar32 c;
            U16_ايندڙ(متن, i, پڇاڙي, c);
            أرجع c;
        }
    }
    /* make current() return DONE */
    pos = پڇاڙي;
    أرجع پورو_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    إذا (pos < پڇاڙي) {
        UChar32 c;
        U16_ايندڙ(متن, pos, پڇاڙي, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    إذا (pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع پورو_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    بدل(origin) {
    حالة k_شروعات:
        pos = begin + delta;
        اكسر;
    حالة kCurrent:
        pos += delta;
        اكسر;
    حالة k_پڇاڙي:
        pos = پڇاڙي + delta;
        اكسر;
    افتراضي:
        اكسر;
    }

    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > پڇاڙي) {
        pos = پڇاڙي;
    }

    أرجع pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة k_شروعات:
        pos = begin;
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, پڇاڙي, delta);
        }
        اكسر;
    حالة kCurrent:
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, پڇاڙي, delta);
        } وإلا {
            U16_پٺ_N(متن, begin, pos, -delta);
        }
        اكسر;
    حالة k_پڇاڙي:
        pos = پڇاڙي;
        إذا(delta < 0) {
            U16_پٺ_N(متن, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    أرجع pos;
}

فراغ UCharCharacterIterator::سيٽ_متن(ConstChar16Ptr new_متن,
                                     int32_t      new_متن_ڊيگهائي) {
    متن = new_متن;
    إذا (new_متن == خالي_اشارو || new_متن_ڊيگهائي < 0) {
        new_متن_ڊيگهائي = 0;
    }
    پڇاڙي = متن_ڊيگهائي = new_متن_ڊيگهائي;
    pos = begin = 0;
}

فراغ
UCharCharacterIterator::get_متن(UnicodeString& نتيجو) {
    نتيجو = UnicodeString(متن, متن_ڊيگهائي);
}

U_NAMESPACE_پڇاڙي
