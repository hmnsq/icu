// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1998-2012, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ "utypeinfo.h"  // for 'typeid' to work

#ಅನ್ತರ್ಭಾವ "unicode/uchriter.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"

U_NAMESPACE_BEGIN

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UCharCharacterIterator)

UCharCharacterIterator::UCharCharacterIterator()
  : CharacterIterator(),
  ಪಠ್ಯ(ಶೂನ್ಯಸೂಚಕ)
{
    // never default construct!
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ಪಠ್ಯ_Ptr U_LIFETIME_BOUND,
                                               int32_t length)
  : CharacterIterator(ಪಠ್ಯ_Ptr != ಶೂನ್ಯಸೂಚಕ ? (length >= 0 ? length : u_strlen(ಪಠ್ಯ_Ptr)) : 0),
  ಪಠ್ಯ(ಪಠ್ಯ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ಪಠ್ಯ_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t position)
  : CharacterIterator(ಪಠ್ಯ_Ptr != ಶೂನ್ಯಸೂಚಕ ? (length >= 0 ? length : u_strlen(ಪಠ್ಯ_Ptr)) : 0, position),
  ಪಠ್ಯ(ಪಠ್ಯ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ConstChar16Ptr ಪಠ್ಯ_Ptr U_LIFETIME_BOUND,
                                               int32_t length,
                                               int32_t ಪಠ್ಯ_Begin,
                                               int32_t ಪಠ್ಯ_ಕೊನೆ,
                                               int32_t position)
  : CharacterIterator(ಪಠ್ಯ_Ptr != ಶೂನ್ಯಸೂಚಕ ? (length >= 0 ? length : u_strlen(ಪಠ್ಯ_Ptr)) : 0,
                      ಪಠ್ಯ_Begin, ಪಠ್ಯ_ಕೊನೆ, position),
  ಪಠ್ಯ(ಪಠ್ಯ_Ptr)
{
}

UCharCharacterIterator::UCharCharacterIterator(ಸ್ಥಿರಾಂಕ UCharCharacterIterator& that)
: CharacterIterator(that),
  ಪಠ್ಯ(that.ಪಠ್ಯ)
{
}

UCharCharacterIterator&
UCharCharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UCharCharacterIterator& that) U_LIFETIME_BOUND {
    CharacterIterator::ಕ್ರಿಯಾಕಾರಕ=(that);
    ಪಠ್ಯ = that.ಪಠ್ಯ;
    ಹಿಂತಿರುಗಿ *ಇದು;
}

UCharCharacterIterator::~UCharCharacterIterator() {
}

ತಾರ್ಕಿಕ
UCharCharacterIterator::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ForwardCharacterIterator& that) ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (ಇದು == &that) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (typeid(*ಇದು) != typeid(that)) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    ಸ್ಥಿರಾಂಕ UCharCharacterIterator&    realThat = static_cast<ಸ್ಥಿರಾಂಕ UCharCharacterIterator&>(that);

    ಹಿಂತಿರುಗಿ ಪಠ್ಯ == realThat.ಪಠ್ಯ
        && ಪಠ್ಯ_ಉದ್ದಳತೆ == realThat.ಪಠ್ಯ_ಉದ್ದಳತೆ
        && pos == realThat.pos
        && begin == realThat.begin
        && ಕೊನೆ == realThat.ಕೊನೆ;
}

int32_t
UCharCharacterIterator::hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ustr_hashUCharsN(ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ) ^ pos ^ begin ^ ಕೊನೆ;
}

UCharCharacterIterator*
UCharCharacterIterator::clone() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಹೊಸ UCharCharacterIterator(*ಇದು);
}

char16_t
UCharCharacterIterator::first() {
    pos = begin;
    ಆದರೆ(pos < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::ಮೊದಲ_Post_Inc() {
    pos = begin;
    ಆದರೆ(pos < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[pos++];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::ಕೊನೆಯ() {
    pos = ಕೊನೆ;
    ಆದರೆ(pos > begin) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[--pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::ಗಣ_ಸೂಚ್ಯಂಕ(int32_t position) {
    ಆದರೆ(position < begin) {
        pos = begin;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(position > ಕೊನೆ) {
        pos = ಕೊನೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pos = position;
    }
    ಆದರೆ(pos < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::current() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (pos >= begin && pos < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::next() {
    ಆದರೆ (pos + 1 < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[++pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* make current() return DONE */
        pos = ಕೊನೆ;
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

char16_t
UCharCharacterIterator::ಮುಂದಿನ_Post_Inc() {
    ಆದರೆ (pos < ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[pos++];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UBool
UCharCharacterIterator::has_ಮುಂದಿನ() {
    ಹಿಂತಿರುಗಿ pos < ಕೊನೆ;
}

char16_t
UCharCharacterIterator::ಹಿಂದಿನ() {
    ಆದರೆ (pos > begin) {
        ಹಿಂತಿರುಗಿ ಪಠ್ಯ[--pos];
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UBool
UCharCharacterIterator::has_ಹಿಂದಿನ() {
    ಹಿಂತಿರುಗಿ pos > begin;
}

UChar32
UCharCharacterIterator::first32() {
    pos = begin;
    ಆದರೆ(pos < ಕೊನೆ) {
        int32_t i = pos;
        UChar32 c;
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, ಕೊನೆ, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::first32PostInc() {
    pos = begin;
    ಆದರೆ(pos < ಕೊನೆ) {
        UChar32 c;
        U16_ಮುಂದಿನ(ಪಠ್ಯ, pos, ಕೊನೆ, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::last32() {
    pos = ಕೊನೆ;
    ಆದರೆ(pos > begin) {
        UChar32 c;
        U16_PREV(ಪಠ್ಯ, begin, pos, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::ಗಣ_Index32(int32_t position) {
    ಆದರೆ(position < begin) {
        position = begin;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(position > ಕೊನೆ) {
        position = ಕೊನೆ;
    }
    ಆದರೆ(position < ಕೊನೆ) {
        U16_ಗಣ_CP_ಆರಂಭ(ಪಠ್ಯ, begin, position);
        int32_t i = ಇದು->pos = position;
        UChar32 c;
        U16_ಮುಂದಿನ(ಪಠ್ಯ, i, ಕೊನೆ, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಇದು->pos = position;
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::current32() ಸ್ಥಿರಾಂಕ {
    ಆದರೆ (pos >= begin && pos < ಕೊನೆ) {
        UChar32 c;
        U16_GET(ಪಠ್ಯ, begin, pos, ಕೊನೆ, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::next32() {
    ಆದರೆ (pos < ಕೊನೆ) {
        U16_FWD_1(ಪಠ್ಯ, pos, ಕೊನೆ);
        ಆದರೆ(pos < ಕೊನೆ) {
            int32_t i = pos;
            UChar32 c;
            U16_ಮುಂದಿನ(ಪಠ್ಯ, i, ಕೊನೆ, c);
            ಹಿಂತಿರುಗಿ c;
        }
    }
    /* make current() return DONE */
    pos = ಕೊನೆ;
    ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
}

UChar32
UCharCharacterIterator::next32PostInc() {
    ಆದರೆ (pos < ಕೊನೆ) {
        UChar32 c;
        U16_ಮುಂದಿನ(ಪಠ್ಯ, pos, ಕೊನೆ, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

UChar32
UCharCharacterIterator::previous32() {
    ಆದರೆ (pos > begin) {
        UChar32 c;
        U16_PREV(ಪಠ್ಯ, begin, pos, c);
        ಹಿಂತಿರುಗಿ c;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಮುಗಿದ_2;
    }
}

int32_t
UCharCharacterIterator::move(int32_t delta, CharacterIterator::EOrigin origin) {
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ k_ಆರಂಭ:
        pos = begin + delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ kCurrent:
        pos += delta;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ k_ಕೊನೆ:
        pos = ಕೊನೆ + delta;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(pos < begin) {
        pos = begin;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pos > ಕೊನೆ) {
        pos = ಕೊನೆ;
    }

    ಹಿಂತಿರುಗಿ pos;
}

int32_t
UCharCharacterIterator::move32(int32_t delta, CharacterIterator::EOrigin origin) {
    // this implementation relies on the "safe" version of the UTF macros
    // (or the trustworthiness of the caller)
    ಆಯ್ಕೆ(origin) {
    ಸ್ಥಿತಿ k_ಆರಂಭ:
        pos = begin;
        ಆದರೆ(delta > 0) {
            U16_FWD_N(ಪಠ್ಯ, pos, ಕೊನೆ, delta);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ kCurrent:
        ಆದರೆ(delta > 0) {
            U16_FWD_N(ಪಠ್ಯ, pos, ಕೊನೆ, delta);
        } ಇಲ್ಲದಿದ್ದರೆ {
            U16_ಹಿಂಭಾಗ_N(ಪಠ್ಯ, begin, pos, -delta);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ k_ಕೊನೆ:
        pos = ಕೊನೆ;
        ಆದರೆ(delta < 0) {
            U16_ಹಿಂಭಾಗ_N(ಪಠ್ಯ, begin, pos, -delta);
        }
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ pos;
}

ಶೂನ್ಯ UCharCharacterIterator::ಗಣ_ಪಠ್ಯ(ConstChar16Ptr new_ಪಠ್ಯ,
                                     int32_t      new_ಪಠ್ಯ_ಉದ್ದಳತೆ) {
    ಪಠ್ಯ = new_ಪಠ್ಯ;
    ಆದರೆ (new_ಪಠ್ಯ == ಶೂನ್ಯಸೂಚಕ || new_ಪಠ್ಯ_ಉದ್ದಳತೆ < 0) {
        new_ಪಠ್ಯ_ಉದ್ದಳತೆ = 0;
    }
    ಕೊನೆ = ಪಠ್ಯ_ಉದ್ದಳತೆ = new_ಪಠ್ಯ_ಉದ್ದಳತೆ;
    pos = begin = 0;
}

ಶೂನ್ಯ
UCharCharacterIterator::get_ಪಠ್ಯ(UnicodeString& ಫಲಿತಾಂಶ) {
    ಫಲಿತಾಂಶ = UnicodeString(ಪಠ್ಯ, ಪಠ್ಯ_ಉದ್ದಳತೆ);
}

U_NAMESPACE_ಕೊನೆ
