// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#شامل "utypeinfo.h"  // for 'typeid' to work

#شامل "unicode/uchriter.h"
#شامل "unicode/ustring.h"
#شامل "unicode/utf16.h"
#شامل "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  متن(خالی_اشاریہ)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(متن_Ptr != خالی_اشاریہ ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالی_اشاریہ ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0, position),
  متن(متن_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t متن_Begin,
                                               int32_t متن_انتہا,
                                               int32_t position)
  : CharacterIterator(متن_Ptr != خالی_اشاریہ ? (length >= 0 ? length : u_strlen(متن_Ptr)) : 0,
                      متن_Begin, متن_انتہا, position),
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
    واپس *یہ;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

منطقی
UCharCharacterIterator::عامل==(ثابت ForwardCharacterIterator& that) ثابت {
    اگر (یہ == &that) {
        واپس سچ;
    }
    اگر (typeid(*یہ) != typeid(that)) {
        واپس جھوٹ;
    }

    ثابت UCharCharacterIterator&    realThat = static_cast<ثابت UCharCharacterIterator&>(that);

    واپس متن == realThat.متن
        && متن_لمبائی == realThat.متن_لمبائی
        && pos == realThat.pos
        && begin == realThat.begin
        && انتہا == realThat.انتہا;
}

int32_t
UCharCharacterIterator::hash_رمز() ثابت {
    واپس ustr_hashUCharsN(متن, متن_لمبائی) ^ pos ^ begin ^ انتہا;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ثابت {
    واپس نیا UCharCharacterIterator(*یہ);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    اگر(pos < انتہا) {
        واپس متن[pos];
    } ورنہ {
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::پہلا_Post_Inc() {
    pos = begin;
    اگر(pos < انتہا) {
        واپس متن[pos++];
    } ورنہ {
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::آخری() {
    pos = انتہا;
    اگر(pos > begin) {
        واپس متن[--pos];
    } ورنہ {
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::سیٹ_اشاریہ(int32_t position) {
    اگر(position < begin) {
        pos = begin;
    } ورنہ اگر(position > انتہا) {
        pos = انتہا;
    } ورنہ {
        pos = position;
    }
    اگر(pos < انتہا) {
        واپس متن[pos];
    } ورنہ {
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::current() ثابت {
    اگر (pos >= begin && pos < انتہا) {
        واپس متن[pos];
    } ورنہ {
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    اگر (pos + 1 < انتہا) {
        واپس متن[++pos];
    } ورنہ {
        /* make current() return DONE */
        pos = انتہا;
        واپس مکمل_2;
    }
}

char16_t
UCharCharacterIterator::اگلا_Post_Inc() {
    اگر (pos < انتہا) {
        واپس متن[pos++];
    } ورنہ {
        واپس مکمل_2;
    }
}

UBool
UCharCharacterIterator::has_اگلا() {
    واپس pos < انتہا;
}

char16_t
UCharCharacterIterator::پچھلا_2() {
    اگر (pos > begin) {
        واپس متن[--pos];
    } ورنہ {
        واپس مکمل_2;
    }
}

UBool
UCharCharacterIterator::has_پچھلا() {
    واپس pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    اگر(pos < انتہا) {
        int32_t i = pos;
        UChar32 c;
        U16_اگلا(متن, i, انتہا, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    اگر(pos < انتہا) {
        UChar32 c;
        U16_اگلا(متن, pos, انتہا, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = انتہا;
    اگر(pos > begin) {
        UChar32 c;
        U16_پچھلا(متن, begin, pos, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::سیٹ_Index32(int32_t position) {
    اگر(position < begin) {
        position = begin;
    } ورنہ اگر(position > انتہا) {
        position = انتہا;
    }
    اگر(position < انتہا) {
        U16_سیٹ_CP_آغاز(متن, begin, position);
        int32_t i = یہ->pos = position;
        UChar32 c;
        U16_اگلا(متن, i, انتہا, c);
        واپس c;
    } ورنہ {
        یہ->pos = position;
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::current32() ثابت {
    اگر (pos >= begin && pos < انتہا) {
        UChar32 c;
        U16_GET(متن, begin, pos, انتہا, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    اگر (pos < انتہا) {
        U16_FWD_1(متن, pos, انتہا);
        اگر(pos < انتہا) {
            int32_t i = pos;
            UChar32 c;
            U16_اگلا(متن, i, انتہا, c);
            واپس c;
        }
    }
    /* make current() return DONE */
    pos = انتہا;
    واپس مکمل_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    اگر (pos < انتہا) {
        UChar32 c;
        U16_اگلا(متن, pos, انتہا, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    اگر (pos > begin) {
        UChar32 c;
        U16_پچھلا(متن, begin, pos, c);
        واپس c;
    } ورنہ {
        واپس مکمل_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    انتخاب(origin) {
    حالت k_آغاز:
        pos = begin + delta;
        توڑو;
    حالت kCurrent:
        pos += delta;
        توڑو;
    حالت k_انتہا:
        pos = انتہا + delta;
        توڑو;
    طےشدہ:
        توڑو;
    }

    اگر(pos < begin) {
        pos = begin;
    } ورنہ اگر(pos > انتہا) {
        pos = انتہا;
    }

    واپس pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    انتخاب(origin) {
    حالت k_آغاز:
        pos = begin;
        اگر(delta > 0) {
            U16_FWD_N(متن, pos, انتہا, delta);
        }
        توڑو;
    حالت kCurrent:
        اگر(delta > 0) {
            U16_FWD_N(متن, pos, انتہا, delta);
        } ورنہ {
            U16_عقب_N(متن, begin, pos, -delta);
        }
        توڑو;
    حالت k_انتہا:
        pos = انتہا;
        اگر(delta < 0) {
            U16_عقب_N(متن, begin, pos, -delta);
        }
        توڑو;
    طےشدہ:
        توڑو;
    }

    واپس pos;
}

خالی UCharCharacterIterator::سیٹ_متن(ConstChar16Ptr new_متن,
                                     int32_t      new_متن_لمبائی) {
    متن = new_متن;
    اگر (new_متن == خالی_اشاریہ || new_متن_لمبائی < 0) {
        new_متن_لمبائی = 0;
    }
    انتہا = متن_لمبائی = new_متن_لمبائی;
    pos = begin = 0;
}

خالی
UCharCharacterIterator::get_متن(UnicodeString& نتیجہ) {
    نتیجہ = UnicodeString(متن, متن_لمبائی);
}

U_NAMESPACE_انتہا
