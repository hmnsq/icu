// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ਅਨ੍ਤਰ੍ਭਾਵ "utypeinfo.h"  // for 'typeid' to work

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/uchriter.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  ਪਾਠ(ਖਾਲੀ_ਸੂਚਕ)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ਪਾਠ_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(ਪਾਠ_Ptr != ਖਾਲੀ_ਸੂਚਕ ? (length >= 0 ? length : u_strlen(ਪਾਠ_Ptr)) : 0),
  ਪਾਠ(ਪਾਠ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ਪਾਠ_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(ਪਾਠ_Ptr != ਖਾਲੀ_ਸੂਚਕ ? (length >= 0 ? length : u_strlen(ਪਾਠ_Ptr)) : 0, position),
  ਪਾਠ(ਪਾਠ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ਪਾਠ_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t ਪਾਠ_Begin,
                                               int32_t ਪਾਠ_ਸਿਰਾ,
                                               int32_t position)
  : CharacterIterator(ਪਾਠ_Ptr != ਖਾਲੀ_ਸੂਚਕ ? (length >= 0 ? length : u_strlen(ਪਾਠ_Ptr)) : 0,
                      ਪਾਠ_Begin, ਪਾਠ_ਸਿਰਾ, position),
  ਪਾਠ(ਪਾਠ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ਅਚਲ UCharCharacterIterator& that)
: CharacterIterator(that),
  ਪਾਠ(that.ਪਾਠ)
{
}

UCharCharacterIterator&
UCharCharacterIterator::ਸੰਚਾਲਕ=(ਅਚਲ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::ਸੰਚਾਲਕ=(that);
    ਪਾਠ = that.ਪਾਠ;
    ਵਾਪਸ *ਇਹ;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

ਤਾਰਕਿਕ
UCharCharacterIterator::ਸੰਚਾਲਕ==(ਅਚਲ ForwardCharacterIterator& that) ਅਚਲ {
    ਜੇ (ਇਹ == &that) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (typeid(*ਇਹ) != typeid(that)) {
        ਵਾਪਸ ਝੂਠ;
    }

    ਅਚਲ UCharCharacterIterator&    realThat = static_cast<ਅਚਲ UCharCharacterIterator&>(that);

    ਵਾਪਸ ਪਾਠ == realThat.ਪਾਠ
        && ਪਾਠ_ਲੰਮਾਈ == realThat.ਪਾਠ_ਲੰਮਾਈ
        && pos == realThat.pos
        && begin == realThat.begin
        && ਸਿਰਾ == realThat.ਸਿਰਾ;
}

int32_t
UCharCharacterIterator::hash_ਕੋਡ() ਅਚਲ {
    ਵਾਪਸ ustr_hashUCharsN(ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ) ^ pos ^ begin ^ ਸਿਰਾ;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ਅਚਲ {
    ਵਾਪਸ ਨਵਾਂ UCharCharacterIterator(*ਇਹ);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ਜੇ(pos < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[pos];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::ਪਹਿਲਾ_Post_Inc() {
    pos = begin;
    ਜੇ(pos < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[pos++];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::ਆਖਰੀ() {
    pos = ਸਿਰਾ;
    ਜੇ(pos > begin) {
        ਵਾਪਸ ਪਾਠ[--pos];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::ਸੈੱਟ_ਸੂਚਕਾਂਕ(int32_t position) {
    ਜੇ(position < begin) {
        pos = begin;
    } ਨਹੀਂਤਾਂ ਜੇ(position > ਸਿਰਾ) {
        pos = ਸਿਰਾ;
    } ਨਹੀਂਤਾਂ {
        pos = position;
    }
    ਜੇ(pos < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[pos];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::current() ਅਚਲ {
    ਜੇ (pos >= begin && pos < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[pos];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    ਜੇ (pos + 1 < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[++pos];
    } ਨਹੀਂਤਾਂ {
        /* make current() return DONE */
        pos = ਸਿਰਾ;
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

char16_t
UCharCharacterIterator::ਅਗਲਾ_Post_Inc() {
    ਜੇ (pos < ਸਿਰਾ) {
        ਵਾਪਸ ਪਾਠ[pos++];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UBool
UCharCharacterIterator::has_ਅਗਲਾ() {
    ਵਾਪਸ pos < ਸਿਰਾ;
}

char16_t
UCharCharacterIterator::ਪਿਛਲਾ() {
    ਜੇ (pos > begin) {
        ਵਾਪਸ ਪਾਠ[--pos];
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UBool
UCharCharacterIterator::has_ਪਿਛਲਾ() {
    ਵਾਪਸ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ਜੇ(pos < ਸਿਰਾ) {
        int32_t i = pos;
        UChar32 c;
        U16_ਅਗਲਾ(ਪਾਠ, i, ਸਿਰਾ, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ਜੇ(pos < ਸਿਰਾ) {
        UChar32 c;
        U16_ਅਗਲਾ(ਪਾਠ, pos, ਸਿਰਾ, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = ਸਿਰਾ;
    ਜੇ(pos > begin) {
        UChar32 c;
        U16_PREV(ਪਾਠ, begin, pos, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::ਸੈੱਟ_Index32(int32_t position) {
    ਜੇ(position < begin) {
        position = begin;
    } ਨਹੀਂਤਾਂ ਜੇ(position > ਸਿਰਾ) {
        position = ਸਿਰਾ;
    }
    ਜੇ(position < ਸਿਰਾ) {
        U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(ਪਾਠ, begin, position);
        int32_t i = ਇਹ->pos = position;
        UChar32 c;
        U16_ਅਗਲਾ(ਪਾਠ, i, ਸਿਰਾ, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਇਹ->pos = position;
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::current32() ਅਚਲ {
    ਜੇ (pos >= begin && pos < ਸਿਰਾ) {
        UChar32 c;
        U16_GET(ਪਾਠ, begin, pos, ਸਿਰਾ, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ਜੇ (pos < ਸਿਰਾ) {
        U16_FWD_1(ਪਾਠ, pos, ਸਿਰਾ);
        ਜੇ(pos < ਸਿਰਾ) {
            int32_t i = pos;
            UChar32 c;
            U16_ਅਗਲਾ(ਪਾਠ, i, ਸਿਰਾ, c);
            ਵਾਪਸ c;
        }
    }
    /* make current() return DONE */
    pos = ਸਿਰਾ;
    ਵਾਪਸ ਪੂਰਾ_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ਜੇ (pos < ਸਿਰਾ) {
        UChar32 c;
        U16_ਅਗਲਾ(ਪਾਠ, pos, ਸਿਰਾ, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ਜੇ (pos > begin) {
        UChar32 c;
        U16_PREV(ਪਾਠ, begin, pos, c);
        ਵਾਪਸ c;
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਪੂਰਾ_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ਚੋਣ(origin) {
    ਹਾਲਤ k_ਸ਼ੁਰੂ:
        pos = begin + delta;
        ਰੋਕੋ;
    ਹਾਲਤ kCurrent:
        pos += delta;
        ਰੋਕੋ;
    ਹਾਲਤ k_ਸਿਰਾ:
        pos = ਸਿਰਾ + delta;
        ਰੋਕੋ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਜੇ(pos < begin) {
        pos = begin;
    } ਨਹੀਂਤਾਂ ਜੇ(pos > ਸਿਰਾ) {
        pos = ਸਿਰਾ;
    }

    ਵਾਪਸ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ਚੋਣ(origin) {
    ਹਾਲਤ k_ਸ਼ੁਰੂ:
        pos = begin;
        ਜੇ(delta > 0) {
            U16_FWD_N(ਪਾਠ, pos, ਸਿਰਾ, delta);
        }
        ਰੋਕੋ;
    ਹਾਲਤ kCurrent:
        ਜੇ(delta > 0) {
            U16_FWD_N(ਪਾਠ, pos, ਸਿਰਾ, delta);
        } ਨਹੀਂਤਾਂ {
            U16_ਪਿੱਠ_N(ਪਾਠ, begin, pos, -delta);
        }
        ਰੋਕੋ;
    ਹਾਲਤ k_ਸਿਰਾ:
        pos = ਸਿਰਾ;
        ਜੇ(delta < 0) {
            U16_ਪਿੱਠ_N(ਪਾਠ, begin, pos, -delta);
        }
        ਰੋਕੋ;
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਵਾਪਸ pos;
}

ਖਾਲੀ UCharCharacterIterator::ਸੈੱਟ_ਪਾਠ(ConstChar16Ptr new_ਪਾਠ,
                                     int32_t      new_ਪਾਠ_ਲੰਮਾਈ) {
    ਪਾਠ = new_ਪਾਠ;
    ਜੇ (new_ਪਾਠ == ਖਾਲੀ_ਸੂਚਕ || new_ਪਾਠ_ਲੰਮਾਈ < 0) {
        new_ਪਾਠ_ਲੰਮਾਈ = 0;
    }
    ਸਿਰਾ = ਪਾਠ_ਲੰਮਾਈ = new_ਪਾਠ_ਲੰਮਾਈ;
    pos = begin = 0;
}

ਖਾਲੀ
UCharCharacterIterator::get_ਪਾਠ(UnicodeString& ਨਤੀਜਾ) {
    ਨਤੀਜਾ = UnicodeString(ਪਾਠ, ਪਾਠ_ਲੰਮਾਈ);
}

U_NAMESPACE_ਸਿਰਾ
