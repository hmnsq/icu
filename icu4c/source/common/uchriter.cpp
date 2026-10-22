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
  متن(تش_اشاره)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(متن_Ptr != تش_اشاره ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != تش_اشاره ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t متن_Begin,
                                               int32_t متن_پای_ټکی,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != تش_اشاره ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0,
                      متن_Begin, متن_پای_ټکی, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ثابت UCharCharacterIterator& that)
: CharacterIterator(that),
  متن(that.متن)
{
}

UCharCharacterIterator&
UCharCharacterIterator::چلوونکی=(ثابت UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::چلوونکی=(that);
    متن = that.متن;
    أرجع *هذا;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقي
UCharCharacterIterator::چلوونکی==(ثابت ForwardCharacterIterator& that) ثابت {
    إذا (هذا == &that) {
        أرجع ریښتیا;
    }
    إذا (typeid(*هذا) != typeid(that)) {
        أرجع خطأ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    أرجع متن == realThat.متن
        && متن_اوږدتیا == realThat.متن_اوږدتیا
        && pos == realThat.pos
        && begin == realThat.begin
        && پای_ټکی == realThat.پای_ټکی;
}

int32_t
UCharCharacterIterator::hash_کوډ() ثابت {
    أرجع ustr_hashUCharsN(متن, متن_اوږدتیا) ^ pos ^ begin ^ پای_ټکی;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    أرجع جديد UCharCharacterIterator(*هذا);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    إذا(pos < پای_ټکی) {
        أرجع متن[pos];
    } وإلا {
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::لومړی_Post_Inc() {
    pos = begin;
    إذا(pos < پای_ټکی) {
        أرجع متن[pos++];
    } وإلا {
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::وروستی() {
    pos = پای_ټکی;
    إذا(pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::سېټ_شاخص(int32_t position) {
    إذا(position < begin) {
        pos = begin;
    } وإلا إذا(position > پای_ټکی) {
        pos = پای_ټکی;
    } وإلا {
        pos = position;
    }
    إذا(pos < پای_ټکی) {
        أرجع متن[pos];
    } وإلا {
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    إذا (pos >= begin && pos < پای_ټکی) {
        أرجع متن[pos];
    } وإلا {
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    إذا (pos + 1 < پای_ټکی) {
        أرجع متن[++pos];
    } وإلا {
        /* make current() return DONE */
        pos = پای_ټکی;
        أرجع بشپړ_2;
    }
}

char16_t
UCharCharacterIterator::بل_Post_Inc() {
    إذا (pos < پای_ټکی) {
        أرجع متن[pos++];
    } وإلا {
        أرجع بشپړ_2;
    }
}

UBool
UCharCharacterIterator::has_بل() {
    أرجع pos < پای_ټکی;
}

char16_t
UCharCharacterIterator::پخوانی() {
    إذا (pos > begin) {
        أرجع متن[--pos];
    } وإلا {
        أرجع بشپړ_2;
    }
}

UBool
UCharCharacterIterator::has_پخوانی() {
    أرجع pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    إذا(pos < پای_ټکی) {
        int32_t i = pos;
        UChar32 c;
        U16_بل(متن, i, پای_ټکی, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    إذا(pos < پای_ټکی) {
        UChar32 c;
        U16_بل(متن, pos, پای_ټکی, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = پای_ټکی;
    إذا(pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::سېټ_Index32(int32_t position) {
    إذا(position < begin) {
        position = begin;
    } وإلا إذا(position > پای_ټکی) {
        position = پای_ټکی;
    }
    إذا(position < پای_ټکی) {
        U16_سېټ_CP_پیل(متن, begin, position);
        int32_t i = هذا->pos = position;
        UChar32 c;
        U16_بل(متن, i, پای_ټکی, c);
        أرجع c;
    } وإلا {
        هذا->pos = position;
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    إذا (pos >= begin && pos < پای_ټکی) {
        UChar32 c;
        U16_GET(متن, begin, pos, پای_ټکی, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    إذا (pos < پای_ټکی) {
        U16_FWD_1(متن, pos, پای_ټکی);
        إذا(pos < پای_ټکی) {
            int32_t i = pos;
            UChar32 c;
            U16_بل(متن, i, پای_ټکی, c);
            أرجع c;
        }
    }
    /* make current() return DONE */
    pos = پای_ټکی;
    أرجع بشپړ_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    إذا (pos < پای_ټکی) {
        UChar32 c;
        U16_بل(متن, pos, پای_ټکی, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    إذا (pos > begin) {
        UChar32 c;
        U16_PREV(متن, begin, pos, c);
        أرجع c;
    } وإلا {
        أرجع بشپړ_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    بدل(origin) {
    حالة k_پیل:
        pos = begin + delta;
        اكسر;
    حالة kCurrent:
        pos += delta;
        اكسر;
    حالة k_پای_ټکی:
        pos = پای_ټکی + delta;
        اكسر;
    افتراضي:
        اكسر;
    }

    إذا(pos < begin) {
        pos = begin;
    } وإلا إذا(pos > پای_ټکی) {
        pos = پای_ټکی;
    }

    أرجع pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    بدل(origin) {
    حالة k_پیل:
        pos = begin;
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, پای_ټکی, delta);
        }
        اكسر;
    حالة kCurrent:
        إذا(delta > 0) {
            U16_FWD_N(متن, pos, پای_ټکی, delta);
        } وإلا {
            U16_شا_N(متن, begin, pos, -delta);
        }
        اكسر;
    حالة k_پای_ټکی:
        pos = پای_ټکی;
        إذا(delta < 0) {
            U16_شا_N(متن, begin, pos, -delta);
        }
        اكسر;
    افتراضي:
        اكسر;
    }

    أرجع pos;
}

فراغ UCharCharacterIterator::سېټ_متن(ConstChar16Ptr new_متن,
                                     int32_t      new_متن_اوږدتیا) {
    متن = new_متن;
    إذا (new_متن == تش_اشاره || new_متن_اوږدتیا < 0) {
        new_متن_اوږدتیا = 0;
    }
    پای_ټکی = متن_اوږدتیا = new_متن_اوږدتیا;
    pos = begin = 0;
}

فراغ
UCharCharacterIterator::get_متن(UnicodeString& پایله) {
    پایله = UnicodeString(متن, متن_اوږدتیا);
}

U_NAMESPACE_پای_ټکی
