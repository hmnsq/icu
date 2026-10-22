// © 2016 and later: Unicode, Inc. and others.
// License & terms of use: http://www.unicode.org/copyright.html
/*
******************************************************************************
* Copyright (C) 1999-2016, International Business Machines Corporation and
* others. All Rights Reserved.
******************************************************************************
*
* File unistr.cpp
*
* Modification History:
*
*   Date        Name        Description
*   09/25/98    stephen     Creation.
*   04/20/99    stephen     Overhauled per 4/16 code review.
*   07/09/99    stephen     Renamed {hi,lo},{byte,word} to icu_X for HP/UX
*   11/18/99    aliu        Added handleReplaceBetween() to make inherit from
*                           Replaceable.
*   06/25/01    grhoten     Removed the dependency on iostream
******************************************************************************
*/

#ಅನ್ತರ್ಭಾವ <string_view>

#ಅನ್ತರ್ಭಾವ "unicode/utypes.h"
#ಅನ್ತರ್ಭಾವ "unicode/appendable.h"
#ಅನ್ತರ್ಭಾವ "unicode/putil.h"
#ಅನ್ತರ್ಭಾವ "cstring.h"
#ಅನ್ತರ್ಭಾವ "cmemory.h"
#ಅನ್ತರ್ಭಾವ "unicode/ustring.h"
#ಅನ್ತರ್ಭಾವ "unicode/unistr.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf.h"
#ಅನ್ತರ್ಭಾವ "unicode/utf16.h"
#ಅನ್ತರ್ಭಾವ "uelement.h"
#ಅನ್ತರ್ಭಾವ "ustr_imp.h"
#ಅನ್ತರ್ಭಾವ "umutex.h"
#ಅನ್ತರ್ಭಾವ "uassert.h"

#ಆದರೆ 0

#ಅನ್ತರ್ಭಾವ <iostream>
ಬಳಸಿ ನಾಮಾಕಾಶ std;

//DEBUGGING
ಶೂನ್ಯ
print(ಸ್ಥಿರಾಂಕ UnicodeString& s,
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  char16_t c;
  ಹೊರಗೆ << name << ":|";
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < s.length(); ++i) {
    c = s[i];
    ಆದರೆ(c>= 0x007E || c < 0x0020)
      ಹೊರಗೆ << "[0x" << hex << s[i] << "]";
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೊರಗೆ << (ಅಕ್ಷರ) s[i];
  }
  ಹೊರಗೆ << '|' << ಸಾಲು_ಅಂತ್ಯ;
}

ಶೂನ್ಯ
print(ಸ್ಥಿರಾಂಕ char16_t *s,
      int32_t len,
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
{
  char16_t c;
  ಹೊರಗೆ << name << ":|";
  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < len; ++i) {
    c = s[i];
    ಆದರೆ(c>= 0x007E || c < 0x0020)
      ಹೊರಗೆ << "[0x" << hex << s[i] << "]";
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೊರಗೆ << (ಅಕ್ಷರ) s[i];
  }
  ಹೊರಗೆ << '|' << ಸಾಲು_ಅಂತ್ಯ;
}
// END DEBUGGING
#ಆದರೆ_ಅಂತ್ಯ

// Local function definitions for now

// need to copy areas that may overlap
ಸ್ಥಿರ
ಸಾಲೊಳಗೆ ಶೂನ್ಯ
us_arrayCopy(ಸ್ಥಿರಾಂಕ char16_t *src, int32_t src_ಆರಂಭ,
         char16_t *dst, int32_t dst_ಆರಂಭ, int32_t count)
{
  ಆದರೆ(count>0) {
    uprv_memmove(dst+dst_ಆರಂಭ, src+src_ಆರಂಭ, (size_t)count*ಗಾತ್ರ(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ಸ್ಥಿರ char16_t U_CALLCONV
UnicodeString_charAt(int32_t ಸ್ಥಾನಾಂತರ, ಶೂನ್ಯ *context) {
    ಹಿಂತಿರುಗಿ ((icu::UnicodeString*) context)->charAt(ಸ್ಥಾನಾಂತರ);
}
U_CDECL_ಕೊನೆ

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
ಕ್ರಿಯಾಕಾರಕ+ (ಸ್ಥಿರಾಂಕ UnicodeString &s1, ಸ್ಥಿರಾಂಕ UnicodeString &s2) {
  int32_t ಮೊತ್ತ_Lengths;
  ಆದರೆ (uprv_add32_ಅತಿಪ್ರವಾಹ(s1.length(), s2.length(), &ಮೊತ್ತ_Lengths)) {
    UnicodeString bogus;
    bogus.ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ bogus;
  }
  ಆದರೆ (ಮೊತ್ತ_Lengths != INT32_MAX) {
    ++ಮೊತ್ತ_Lengths;  // space for a terminating NUL if we need one
  }
  ಹಿಂತಿರುಗಿ UnicodeString(ಮೊತ್ತ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_ಆಂತರಿಕ_Concat(ಸ್ಥಿರಾಂಕ UnicodeString &s1, std::u16string_view s2) {
  int32_t ಮೊತ್ತ_Lengths;
  ಆದರೆ (s2.length() > INT32_MAX ||
      uprv_add32_ಅತಿಪ್ರವಾಹ(s1.length(), static_cast<int32_t>(s2.length()), &ಮೊತ್ತ_Lengths)) {
    UnicodeString bogus;
    bogus.ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ bogus;
  }
  ಆದರೆ (ಮೊತ್ತ_Lengths != INT32_MAX) {
    ++ಮೊತ್ತ_Lengths;  // space for a terminating NUL if we need one
  }
  ಹಿಂತಿರುಗಿ UnicodeString(ಮೊತ್ತ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

ಶೂನ್ಯ
UnicodeString::ಕೂಡಿಸು_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  ಹಿಂತಿರುಗಿ umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ umtx_ತುಂಬು_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

ಶೂನ್ಯ
UnicodeString::releaseArray() {
  ಆದರೆ((fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = 0;
  ಆದರೆ (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c <= 0xffff) {
    int32_t length = count;
    ಆದರೆ(capacity < length) {
      capacity = length;
    }
    ಆದರೆ(allocate(capacity)) {
      char16_t *array = get_Array_ಆರಂಭ();
      char16_t unit = static_cast<char16_t>(c);
      ಗಾಗಿ(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      ಗಣ_ಉದ್ದಳತೆ(length);
    }
  } ಇಲ್ಲದಿದ್ದರೆ {  // supplementary code point, write surrogate pairs
    ಆದರೆ(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ಹಿಂತಿರುಗಿ;
    }
    int32_t length = count * 2;
    ಆದರೆ(capacity < length) {
      capacity = length;
    }
    ಆದರೆ(allocate(capacity)) {
      char16_t *array = get_Array_ಆರಂಭ();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      ಗಾಗಿ(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      ಗಣ_ಉದ್ದಳತೆ(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kLength1 | kShortString;
  fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = ಸುಳ್ಳು;
  U16_APPEND(fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್, i, US_STACKBUF_ಗಾತ್ರ, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  ಆದರೆ(!isError) {
    ಗಣ_Short_ಉದ್ದಳತೆ(i);
  }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ,
                             int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  doAppend(ಪಠ್ಯ, 0, ಪಠ್ಯ_ಉದ್ದಳತೆ);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr ಪಠ್ಯ_Ptr U_LIFETIME_BOUND,
                             int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kReadonlyAlias;
  ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ = ಪಠ್ಯ_Ptr;
  ಆದರೆ(ಪಠ್ಯ == ಶೂನ್ಯಸೂಚಕ) {
    // treat as an empty string, do not alias
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ < -1 ||
            (ಪಠ್ಯ_ಉದ್ದಳತೆ == -1 && !isTerminated) ||
            (ಪಠ್ಯ_ಉದ್ದಳತೆ >= 0 && isTerminated && ಪಠ್ಯ[ಪಠ್ಯ_ಉದ್ದಳತೆ] != 0)
  ) {
    ಗಣ_ಗಮ್ಯ_Bogus();
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ == -1) {
      // text is terminated, or else it would have failed the above test
      ಪಠ್ಯ_ಉದ್ದಳತೆ = u_strlen(ಪಠ್ಯ);
    }
    ಗಣ_Array(const_cast<char16_t *>(ಪಠ್ಯ), ಪಠ್ಯ_ಉದ್ದಳತೆ,
             isTerminated ? ಪಠ್ಯ_ಉದ್ದಳತೆ + 1 : ಪಠ್ಯ_ಉದ್ದಳತೆ);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_ಉದ್ದಳತೆ,
                             int32_t buff_ಸಾಮರ್ಥ್ಯ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kWritableAlias;
  ಆದರೆ(buff == ಶೂನ್ಯಸೂಚಕ) {
    // treat as an empty string, do not alias
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(buff_ಉದ್ದಳತೆ < -1 || buff_ಸಾಮರ್ಥ್ಯ < 0 || buff_ಉದ್ದಳತೆ > buff_ಸಾಮರ್ಥ್ಯ) {
    ಗಣ_ಗಮ್ಯ_Bogus();
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(buff_ಉದ್ದಳತೆ == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ಸ್ಥಿರಾಂಕ char16_t *p = buff, *ಎಲ್ಲೆ = buff + buff_ಸಾಮರ್ಥ್ಯ;
      ಆಗ(p != ಎಲ್ಲೆ && *p != 0) {
        ++p;
      }
      buff_ಉದ್ದಳತೆ = static_cast<int32_t>(p - buff);
    }
    ಗಣ_Array(buff, buff_ಉದ್ದಳತೆ, buff_ಸಾಮರ್ಥ್ಯ);
  }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, int32_t length, EInvariant) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  ಆದರೆ(src==ಶೂನ್ಯಸೂಚಕ) {
    // treat as an empty string
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    ಆದರೆ(cloneArrayIfNeeded(length, length, ಸುಳ್ಳು)) {
      u_chars_ಗಮ್ಯ_UChars(src, get_Array_ಆರಂಭ(), length);
      ಗಣ_ಉದ್ದಳತೆ(length);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಗಣ_ಗಮ್ಯ_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view ಪಠ್ಯ U_LIFETIME_BOUND) {
  UnicodeString ಫಲಿತಾಂಶ;
  ಆದರೆ (ಪಠ್ಯ.length() <= INT32_MAX) {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, ಪಠ್ಯ.ದತ್ತಾಂಶ(), static_cast<int32_t>(ಪಠ್ಯ.length()));
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ U_LIFETIME_BOUND) {
  UnicodeString ಫಲಿತಾಂಶ;
  ಆದರೆ (ಪಠ್ಯ.isBogus()) {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ(ಸುಳ್ಳು, ಪಠ್ಯ.get_ಬಫರ್(), ಪಠ್ಯ.length());
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಆದರೆ U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  ಆದರೆ (codepage_ದತ್ತಾಂಶ != ಶೂನ್ಯಸೂಚಕ) {
    ಗಣ_ಗಮ್ಯ_UTF8(codepage_ದತ್ತಾಂಶ);
  }
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *codepage_ದತ್ತಾಂಶ, int32_t ದತ್ತಾಂಶ_ಉದ್ದಳತೆ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  ಆದರೆ (codepage_ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == 0 || ದತ್ತಾಂಶ_ಉದ್ದಳತೆ < -1) {
    ಹಿಂತಿರುಗಿ;
  }
  ಆದರೆ(ದತ್ತಾಂಶ_ಉದ್ದಳತೆ == -1) {
    ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = static_cast<int32_t>(uprv_strlen(codepage_ದತ್ತಾಂಶ));
  }
  ಗಣ_ಗಮ್ಯ_UTF8(StringPiece(codepage_ದತ್ತಾಂಶ, ದತ್ತಾಂಶ_ಉದ್ದಳತೆ));
}

// else see unistr_cnv.cpp
#ಆದರೆ_ಅಂತ್ಯ

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString& that) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, ನಿಜ);
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString& that,
                             int32_t src_ಆರಂಭ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  ಗಣ_ಗಮ್ಯ(that, src_ಆರಂಭ);
}

UnicodeString::UnicodeString(ಸ್ಥಿರಾಂಕ UnicodeString& that,
                             int32_t src_ಆರಂಭ,
                             int32_t src_ಉದ್ದಳತೆ) {
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
  ಗಣ_ಗಮ್ಯ(that, src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ಸ್ಥಿರಾಂಕ {
  Local_ಸೂಚಕ<UnicodeString> clonedString(ಹೊಸ UnicodeString(*ಇದು));
  ಹಿಂತಿರುಗಿ clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : ಶೂನ್ಯಸೂಚಕ;
}

//========================================
// array allocation
//========================================

ನಾಮಾಕಾಶ {

ಸ್ಥಿರಾಂಕ int32_t k_Grow_ಗಾತ್ರ = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ಸ್ಥಿರಾಂಕ int32_t k_Max_ಸಾಮರ್ಥ್ಯ = 0x7ffffff5;

int32_t get_Grow_ಸಾಮರ್ಥ್ಯ(int32_t new_ಉದ್ದಳತೆ) {
  int32_t grow_ಗಾತ್ರ = (new_ಉದ್ದಳತೆ >> 2) + k_Grow_ಗಾತ್ರ;
  ಆದರೆ(grow_ಗಾತ್ರ <= (k_Max_ಸಾಮರ್ಥ್ಯ - new_ಉದ್ದಳತೆ)) {
    ಹಿಂತಿರುಗಿ new_ಉದ್ದಳತೆ + grow_ಗಾತ್ರ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ k_Max_ಸಾಮರ್ಥ್ಯ;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  ಆದರೆ(capacity <= US_STACKBUF_ಗಾತ್ರ) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
  ಆದರೆ(capacity <= k_Max_ಸಾಮರ್ಥ್ಯ) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t ಅಂಕೆ_ಬೈಟುಗಳು = ಗಾತ್ರ(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    ಅಂಕೆ_ಬೈಟುಗಳು = (ಅಂಕೆ_ಬೈಟುಗಳು + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(ಅಂಕೆ_ಬೈಟುಗಳು));
    ಆದರೆ(array != ಶೂನ್ಯಸೂಚಕ) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      ಅಂಕೆ_ಬೈಟುಗಳು -= ಗಾತ್ರ(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = static_cast<int32_t>(ಅಂಕೆ_ಬೈಟುಗಳು / U_SIZEOF_UCHAR);
      fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kLongString;
      ಹಿಂತಿರುಗಿ ನಿಜ;
    }
  }
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kIsBogus;
  fUnion.fFields.fArray = ಶೂನ್ಯಸೂಚಕ;
  fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = 0;
  ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

//========================================
// Destructor
//========================================

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNISTR_ಎಣಿಕೆ_FINAL_STRING_LENGTHS
ಸ್ಥಿರ u_atomic_int32_t final_ಉದ್ದಳತೆ_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ಸ್ಥಿರ u_atomic_int32_t beyond_ಎಣಿಕೆ(0);

U_CAPI ಶೂನ್ಯ unistr_printLengths() {
  int32_t i;
  ಗಾಗಿ(i = 0; i <= 59; ++i) {
    ಬರೆ("%2d,  %9d\n", i, (int32_t)final_ಉದ್ದಳತೆ_Counts[i]);
  }
  int32_t beyond = beyond_ಎಣಿಕೆ;
  ಗಾಗಿ(; i < UPRV_LENGTHOF(final_ಉದ್ದಳತೆ_Counts); ++i) {
    beyond += final_ಉದ್ದಳತೆ_Counts[i];
  }
  ಬರೆ(">59, %9d\n", beyond);
}
#ಆದರೆ_ಅಂತ್ಯ

UnicodeString::~UnicodeString()
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNISTR_ಎಣಿಕೆ_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  ಆದರೆ((fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags&(k_Open_Get_ಬಫರ್|kReadonlyAlias|kWritableAlias)) == 0) {
    ಆದರೆ(has_Short_ಉದ್ದಳತೆ()) {
      umtx_atomic_inc(final_ಉದ್ದಳತೆ_Counts + get_Short_ಉದ್ದಳತೆ());
    } ಇಲ್ಲದಿದ್ದರೆ {
      umtx_atomic_inc(&beyond_ಎಣಿಕೆ);
    }
  }
#ಆದರೆ_ಅಂತ್ಯ

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString ಫಲಿತಾಂಶ;
  ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_UTF8(utf8);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UnicodeString UnicodeString::fromUTF32(ಸ್ಥಿರಾಂಕ UChar32 *utf32, int32_t length) {
  UnicodeString ಫಲಿತಾಂಶ;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  ಆದರೆ(length <= US_STACKBUF_ಗಾತ್ರ) {
    capacity = US_STACKBUF_ಗಾತ್ರ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    capacity = length + (length >> 4) + 4;
  }
  ಮಾಡು {
    char16_t *utf16 = ಫಲಿತಾಂಶ.get_ಬಫರ್(capacity);
    int32_t length16;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, ಫಲಿತಾಂಶ.get_ಸಾಮರ್ಥ್ಯ(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
        &error_ಸಂಕೇತ);
    ಫಲಿತಾಂಶ.release_ಬಫರ್(length16);
    ಆದರೆ(error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      ಮುಂದುವರಿ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
      ಫಲಿತಾಂಶ.ಗಣ_ಗಮ್ಯ_Bogus();
    }
    ನಿಲ್ಲಿಸು;
  } ಆಗ(ನಿಜ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ UnicodeString& src) U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ಸ್ಥಿರಾಂಕ UnicodeString& src) U_LIFETIME_BOUND {
  ಹಿಂತಿರುಗಿ copyFrom(src, ನಿಜ);
}

UnicodeString&
UnicodeString::copyFrom(ಸ್ಥಿರಾಂಕ UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  ಆದರೆ(ಇದು == &src) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // is the right side bogus?
  ಆದರೆ(src.isBogus()) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // delete the current contents
  releaseArray();

  ಆದರೆ(src.is_ಖಾಲಿಗಳು()) {
    // empty string - use the stack buffer
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = src.fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags;
  ಆಯ್ಕೆ(src.fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & kAllStorageFlags) {
  ಸ್ಥಿತಿ kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್, src.fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್,
                get_Short_ಉದ್ದಳತೆ() * U_SIZEOF_UCHAR);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).ಕೂಡಿಸು_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = src.fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ(!has_Short_ಉದ್ದಳತೆ()) {
      fUnion.fFields.f_ಉದ್ದಳತೆ = src.fUnion.fFields.f_ಉದ್ದಳತೆ;
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ kReadonlyAlias:
    ಆದರೆ(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = src.fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ;
      ಆದರೆ(!has_Short_ಉದ್ದಳತೆ()) {
        fUnion.fFields.f_ಉದ್ದಳತೆ = src.fUnion.fFields.f_ಉದ್ದಳತೆ;
      }
      ನಿಲ್ಲಿಸು;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  ಸ್ಥಿತಿ kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_ಉದ್ದಳತೆ = src.length();
    ಆದರೆ(allocate(src_ಉದ್ದಳತೆ)) {
      u_memcpy(get_Array_ಆರಂಭ(), src.get_Array_ಆರಂಭ(), src_ಉದ್ದಳತೆ);
      ಗಣ_ಉದ್ದಳತೆ(src_ಉದ್ದಳತೆ);
      ನಿಲ್ಲಿಸು;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  ಪೂರ್ವನಿಯೋಜಿತ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kIsBogus;
    fUnion.fFields.fArray = ಶೂನ್ಯಸೂಚಕ;
    fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = 0;
    ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString& UnicodeString::ಕ್ರಿಯಾಕಾರಕ=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, ನಿಜ);
  ಹಿಂತಿರುಗಿ *ಇದು;
}

// Same as move assignment except without memory management.
ಶೂನ್ಯ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool ಗಣ_Src_ಗಮ್ಯ_Bogus) noexcept {
  int16_t ಉದ್ದಳತೆ_And_Flags = fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = src.fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags;
  ಆದರೆ(ಉದ್ದಳತೆ_And_Flags & k_Using_ರಾಶಿ_ಬಫರ್) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    ಆದರೆ(ಇದು != &src) {
      uprv_memcpy(fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್, src.fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್,
                  get_Short_ಉದ್ದಳತೆ() * U_SIZEOF_UCHAR);
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = src.fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ;
    ಆದರೆ(!has_Short_ಉದ್ದಳತೆ()) {
      fUnion.fFields.f_ಉದ್ದಳತೆ = src.fUnion.fFields.f_ಉದ್ದಳತೆ;
    }
    ಆದರೆ(ಗಣ_Src_ಗಮ್ಯ_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = ಶೂನ್ಯಸೂಚಕ;
      src.fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = 0;
    }
  }
}

ಶೂನ್ಯ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString ತಾತ್ಕಾಲಿಕ;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  ತಾತ್ಕಾಲಿಕ.copyFieldsFrom(*ಇದು, ಸುಳ್ಳು);
  ಇದು->copyFieldsFrom(other, ಸುಳ್ಳು);
  other.copyFieldsFrom(ತಾತ್ಕಾಲಿಕ, ಸುಳ್ಳು);
  // Set temp to an empty string so that other's memory is not released twice.
  ತಾತ್ಕಾಲಿಕ.fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ಸ್ಥಿರಾಂಕ {
    UnicodeString ಫಲಿತಾಂಶ(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    ಆದರೆ (ಫಲಿತಾಂಶ.isBogus()) {
        ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    ಸ್ಥಿರಾಂಕ char16_t *array = get_ಬಫರ್();
    int32_t len = length();
    int32_t prev = 0;
    ಗಾಗಿ (int32_t i=0;;) {
        ಆದರೆ (i == len) {
            ಫಲಿತಾಂಶ.append(array, prev, len - prev);
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (array[i++] == 0x5C /*'\\'*/) {
            ಫಲಿತಾಂಶ.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            ಆದರೆ (c < 0) {
                ಫಲಿತಾಂಶ.remove(); // return empty string
                ನಿಲ್ಲಿಸು; // invalid escape sequence
            }
            ಫಲಿತಾಂಶ.append(c);
            prev = i;
        }
    }
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

UChar32 UnicodeString::unescapeAt(int32_t &ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ u_unescapeAt(UnicodeString_charAt, &ಸ್ಥಾನಾಂತರ, length(), (ಶೂನ್ಯ*)ಇದು);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ, int32_t len) ಸ್ಥಿರಾಂಕ {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ಹಿಂತಿರುಗಿ uprv_memcmp(get_Array_ಆರಂಭ(), ಪಠ್ಯ, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t ಆರಂಭ,
              int32_t length,
              ಸ್ಥಿರಾಂಕ char16_t *srcChars,
              int32_t src_ಆರಂಭ,
              int32_t src_ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ
{
  // compare illegal string values
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }
  
  // pin indices to legal values
  pinIndices(ಆರಂಭ, length);

  ಆದರೆ(srcChars == ಶೂನ್ಯಸೂಚಕ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ಹಿಂತಿರುಗಿ length == 0 ? ನಿಜ : ಸುಳ್ಳು;
  }

  // get the correct pointer
  ಸ್ಥಿರಾಂಕ char16_t *chars = get_Array_ಆರಂಭ();

  chars += ಆರಂಭ;
  srcChars += src_ಆರಂಭ;

  // get the srcLength if necessary
  ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
    src_ಉದ್ದಳತೆ = u_strlen(srcChars + src_ಆರಂಭ);
  }

  ಆದರೆ (length != src_ಉದ್ದಳತೆ) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }

  ಆದರೆ(length == 0 || chars == srcChars) {
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }

  ಹಿಂತಿರುಗಿ u_memcmp(chars, srcChars, src_ಉದ್ದಳತೆ) == 0;
}

int8_t
UnicodeString::doCompare( int32_t ಆರಂಭ,
              int32_t length,
              ಸ್ಥಿರಾಂಕ char16_t *srcChars,
              int32_t src_ಆರಂಭ,
              int32_t src_ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ
{
  // compare illegal string values
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ -1;
  }
  
  // pin indices to legal values
  pinIndices(ಆರಂಭ, length);

  ಆದರೆ(srcChars == ಶೂನ್ಯಸೂಚಕ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ಹಿಂತಿರುಗಿ length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ಸ್ಥಿರಾಂಕ char16_t *chars = get_Array_ಆರಂಭ();

  chars += ಆರಂಭ;
  srcChars += src_ಆರಂಭ;

  int32_t min_ಉದ್ದಳತೆ;
  int8_t ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;

  // get the srcLength if necessary
  ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
    src_ಉದ್ದಳತೆ = u_strlen(srcChars + src_ಆರಂಭ);
  }

  // are we comparing different lengths?
  ಆದರೆ(length != src_ಉದ್ದಳತೆ) {
    ಆದರೆ(length < src_ಉದ್ದಳತೆ) {
      min_ಉದ್ದಳತೆ = length;
      ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = -1;
    } ಇಲ್ಲದಿದ್ದರೆ {
      min_ಉದ್ದಳತೆ = src_ಉದ್ದಳತೆ;
      ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = 1;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    min_ಉದ್ದಳತೆ = length;
    ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  ಆದರೆ(min_ಉದ್ದಳತೆ > 0 && chars != srcChars) {
    int32_t ಫಲಿತಾಂಶ;

#   ಆದರೆ U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      ಫಲಿತಾಂಶ = uprv_memcmp(chars, srcChars, min_ಉದ್ದಳತೆ * ಗಾತ್ರ(char16_t));
      ಆದರೆ(ಫಲಿತಾಂಶ != 0) {
        ಹಿಂತಿರುಗಿ (int8_t)(ಫಲಿತಾಂಶ >> 15 | 1);
      }
#   ಇಲ್ಲದಿದ್ದರೆ
      // little-endian: compare char16_t units
      ಮಾಡು {
        ಫಲಿತಾಂಶ = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        ಆದರೆ(ಫಲಿತಾಂಶ != 0) {
          ಹಿಂತಿರುಗಿ static_cast<int8_t>(ಫಲಿತಾಂಶ >> 15 | 1);
        }
      } ಆಗ(--min_ಉದ್ದಳತೆ > 0);
#   endif
  }
  ಹಿಂತಿರುಗಿ ಉದ್ದಳತೆ_ಫಲಿತಾಂಶ;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_ಸಂಕೇತ_ಬಿಂದು_ಕ್ರಮ(int32_t ಆರಂಭ,
                                       int32_t length,
                                       ಸ್ಥಿರಾಂಕ char16_t *srcChars,
                                       int32_t src_ಆರಂಭ,
                                       int32_t src_ಉದ್ದಳತೆ) ಸ್ಥಿರಾಂಕ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // pin indices to legal values
  pinIndices(ಆರಂಭ, length);

  ಆದರೆ(srcChars == ಶೂನ್ಯಸೂಚಕ) {
    src_ಆರಂಭ = src_ಉದ್ದಳತೆ = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_ಆರಂಭ() + ಆರಂಭ, length, (srcChars!=ಶೂನ್ಯಸೂಚಕ)?(srcChars + src_ಆರಂಭ):ಶೂನ್ಯಸೂಚಕ, src_ಉದ್ದಳತೆ, ಸುಳ್ಳು, ನಿಜ);
  /* translate the 32-bit result into an 8-bit one */
  ಆದರೆ(diff!=0) {
    ಹಿಂತಿರುಗಿ static_cast<int8_t>(diff >> 15 | 1);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ 0;
  }
}

int32_t
UnicodeString::get_ಉದ್ದಳತೆ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ length();
}

char16_t
UnicodeString::getCharAt(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ charAt(ಸ್ಥಾನಾಂತರ);
}

UChar32
UnicodeString::getChar32At(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
  ಹಿಂತಿರುಗಿ char32At(ಸ್ಥಾನಾಂತರ);
}

UChar32
UnicodeString::char32At(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ
{
  int32_t len = length();
  ಆದರೆ (static_cast<uint32_t>(ಸ್ಥಾನಾಂತರ) < static_cast<uint32_t>(len)) {
    ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
    UChar32 c;
    U16_GET(array, 0, ಸ್ಥಾನಾಂತರ, len, c);
    ಹಿಂತಿರುಗಿ c;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_ಆರಂಭ(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
  ಆದರೆ (static_cast<uint32_t>(ಸ್ಥಾನಾಂತರ) < static_cast<uint32_t>(length())) {
    ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
    U16_ಗಣ_CP_ಆರಂಭ(array, 0, ಸ್ಥಾನಾಂತರ);
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ 0;
  }
}

int32_t
UnicodeString::get_Char32_ಎಲ್ಲೆ(int32_t ಸ್ಥಾನಾಂತರ) ಸ್ಥಿರಾಂಕ {
  int32_t len = length();
  ಆದರೆ (static_cast<uint32_t>(ಸ್ಥಾನಾಂತರ) < static_cast<uint32_t>(len)) {
    ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
    U16_ಗಣ_CP_ಎಲ್ಲೆ(array, 0, ಸ್ಥಾನಾಂತರ, len);
    ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ len;
  }
}

int32_t
UnicodeString::ಎಣಿಕೆ_Char32(int32_t ಆರಂಭ, int32_t length) ಸ್ಥಿರಾಂಕ {
  pinIndices(ಆರಂಭ, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ಹಿಂತಿರುಗಿ u_ಎಣಿಕೆ_Char32(get_Array_ಆರಂಭ()+ಆರಂಭ, length);
}

UBool
UnicodeString::has_ಇನ್ನಷ್ಟು_Char32_Than(int32_t ಆರಂಭ, int32_t length, int32_t number) ಸ್ಥಿರಾಂಕ {
  pinIndices(ಆರಂಭ, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ಹಿಂತಿರುಗಿ u_str_Has_ಇನ್ನಷ್ಟು_Char32_Than(get_Array_ಆರಂಭ()+ಆರಂಭ, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t ಸೂಚ್ಯಂಕ, int32_t delta) ಸ್ಥಿರಾಂಕ {
  // pin index
  int32_t len = length();
  ಆದರೆ(ಸೂಚ್ಯಂಕ<0) {
    ಸೂಚ್ಯಂಕ=0;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ>len) {
    ಸೂಚ್ಯಂಕ=len;
  }

  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಆದರೆ(delta>0) {
    U16_FWD_N(array, ಸೂಚ್ಯಂಕ, len, delta);
  } ಇಲ್ಲದಿದ್ದರೆ {
    U16_ಹಿಂಭಾಗ_N(array, 0, ಸೂಚ್ಯಂಕ, -delta);
  }

  ಹಿಂತಿರುಗಿ ಸೂಚ್ಯಂಕ;
}

ಶೂನ್ಯ
UnicodeString::do_ಹೊರತೆಗೆ(int32_t ಆರಂಭ,
             int32_t length,
             char16_t *dst,
             int32_t dst_ಆರಂಭ) ಸ್ಥಿರಾಂಕ
{
  // pin indices to legal values
  pinIndices(ಆರಂಭ, length);

  // do not copy anything if we alias dst itself
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಆದರೆ(array + ಆರಂಭ != dst + dst_ಆರಂಭ) {
    us_arrayCopy(array, ಆರಂಭ, dst, dst_ಆರಂಭ, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_ಸಾಮರ್ಥ್ಯ,
                       UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
  int32_t len = length();
  ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
    ಆದರೆ (isBogus() || dest_ಸಾಮರ್ಥ್ಯ < 0 || (dest_ಸಾಮರ್ಥ್ಯ > 0 && dest == ಶೂನ್ಯಸೂಚಕ)) {
      error_ಸಂಕೇತ=U_ILLEGAL_ವಾದ_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
      ಆದರೆ(len>0 && len<=dest_ಸಾಮರ್ಥ್ಯ && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ಹಿಂತಿರುಗಿ u_terminateUChars(dest, dest_ಸಾಮರ್ಥ್ಯ, len, &error_ಸಂಕೇತ);
    }
  }

  ಹಿಂತಿರುಗಿ len;
}

int32_t
UnicodeString::extract(int32_t ಆರಂಭ,
                       int32_t length,
                       ಅಕ್ಷರ *ಗುರಿ,
                       int32_t ಗುರಿ_ಸಾಮರ್ಥ್ಯ,
                       ಎಣಿಕೆ EInvariant) ಸ್ಥಿರಾಂಕ
{
  // if the arguments are illegal, then do nothing
  ಆದರೆ(ಗುರಿ_ಸಾಮರ್ಥ್ಯ < 0 || (ಗುರಿ_ಸಾಮರ್ಥ್ಯ > 0 && ಗುರಿ == ಶೂನ್ಯಸೂಚಕ)) {
    ಹಿಂತಿರುಗಿ 0;
  }

  // pin the indices to legal values
  pinIndices(ಆರಂಭ, length);

  ಆದರೆ(length <= ಗುರಿ_ಸಾಮರ್ಥ್ಯ) {
    u_UChars_ಗಮ್ಯ_Chars(get_Array_ಆರಂಭ() + ಆರಂಭ, ಗುರಿ, length);
  }
  UError_ಸಂಕೇತ ಸ್ಥಿತಿ_2 = U_ZERO_ERROR;
  ಹಿಂತಿರುಗಿ u_terminateChars(ಗುರಿ, ಗುರಿ_ಸಾಮರ್ಥ್ಯ, length, &ಸ್ಥಿತಿ_2);
}

UnicodeString
UnicodeString::ತಾತ್ಕಾಲಿಕ_Sub_String(int32_t ಆರಂಭ, int32_t len) ಸ್ಥಿರಾಂಕ U_LIFETIME_BOUND {
  pinIndices(ಆರಂಭ, len);
  ಸ್ಥಿರಾಂಕ char16_t *array = get_ಬಫರ್();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  ಆದರೆ(array==ಶೂನ್ಯಸೂಚಕ) {
    array=fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ಹಿಂತಿರುಗಿ UnicodeString(ಸುಳ್ಳು, array + ಆರಂಭ, len);
}

int32_t
UnicodeString::ಗಮ್ಯ_UTF8(int32_t ಆರಂಭ, int32_t len,
                      ಅಕ್ಷರ *ಗುರಿ, int32_t capacity) ಸ್ಥಿರಾಂಕ {
  pinIndices(ಆರಂಭ, len);
  int32_t length8;
  UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
  u_str_ಗಮ್ಯ_UTF8_With_Sub(ಗುರಿ, capacity, &length8,
                     get_ಬಫರ್() + ಆರಂಭ, len,
                     0xFFFD,  // Standard substitution character.
                     ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
                     &error_ಸಂಕೇತ);
  ಹಿಂತಿರುಗಿ length8;
}

#ಆದರೆ U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ಆರಂಭ, int32_t len,
                       ಅಕ್ಷರ *ಗುರಿ, uint32_t dst_ಗಾತ್ರ) ಸ್ಥಿರಾಂಕ {
  // if the arguments are illegal, then do nothing
  ಆದರೆ (/*dstSize < 0 || */(dst_ಗಾತ್ರ > 0 && ಗುರಿ == ಶೂನ್ಯಸೂಚಕ)) {
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ ಗಮ್ಯ_UTF8(ಆರಂಭ, len, ಗುರಿ, dst_ಗಾತ್ರ <= 0x7fffffff ? static_cast<int32_t>(dst_ಗಾತ್ರ) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#ಆದರೆ_ಅಂತ್ಯ

ಶೂನ್ಯ 
UnicodeString::ಹೊರತೆಗೆ_Between(int32_t ಆರಂಭ,
                  int32_t ಎಲ್ಲೆ,
                  UnicodeString& ಗುರಿ) ಸ್ಥಿರಾಂಕ {
  pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ);
  pin_ಸೂಚ್ಯಂಕ(ಎಲ್ಲೆ);
  do_ಹೊರತೆಗೆ(ಆರಂಭ, ಎಲ್ಲೆ - ಆರಂಭ, ಗುರಿ);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
ಶೂನ್ಯ
UnicodeString::ಗಮ್ಯ_UTF8(ByteSink &sink) ಸ್ಥಿರಾಂಕ {
  int32_t length16 = length();
  ಆದರೆ(length16 != 0) {
    ಅಕ್ಷರ ರಾಶಿ_ಬಫರ್[1024];
    int32_t capacity = static_cast<int32_t>(ಗಾತ್ರ(ರಾಶಿ_ಬಫರ್));
    UBool utf8IsOwned = ಸುಳ್ಳು;
    ಅಕ್ಷರ *utf8 = sink.Get_Append_ಬಫರ್(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ರಾಶಿ_ಬಫರ್, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
    u_str_ಗಮ್ಯ_UTF8_With_Sub(utf8, capacity, &length8,
                       get_ಬಫರ್(), length16,
                       0xFFFD,  // Standard substitution character.
                       ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
                       &error_ಸಂಕೇತ);
    ಆದರೆ(error_ಸಂಕೇತ == U_ಬಫರ್_ಅತಿಪ್ರವಾಹ_ERROR) {
      utf8 = static_cast<ಅಕ್ಷರ*>(uprv_malloc(length8));
      ಆದರೆ(utf8 != ಶೂನ್ಯಸೂಚಕ) {
        utf8IsOwned = ನಿಜ;
        error_ಸಂಕೇತ = U_ZERO_ERROR;
        u_str_ಗಮ್ಯ_UTF8_With_Sub(utf8, length8, &length8,
                           get_ಬಫರ್(), length16,
                           0xFFFD,  // Standard substitution character.
                           ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
                           &error_ಸಂಕೇತ);
      } ಇಲ್ಲದಿದ್ದರೆ {
        error_ಸಂಕೇತ = U_ಸ್ಮರಣೆ_ಹಂಚಿಕೆ_ERROR;
      }
    }
    ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    ಆದರೆ(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::ಗಮ್ಯ_UTF32(UChar32 *utf32, int32_t capacity, UError_ಸಂಕೇತ &error_ಸಂಕೇತ) ಸ್ಥಿರಾಂಕ {
  int32_t length32=0;
  ಆದರೆ(U_SUCCESS(error_ಸಂಕೇತ)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_ಗಮ್ಯ_UTF32_With_Sub(utf32, capacity, &length32,
        get_ಬಫರ್(), length(),
        0xfffd,  // Substitution character.
        ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
        &error_ಸಂಕೇತ);
  }
  ಹಿಂತಿರುಗಿ length32;
}

int32_t 
UnicodeString::ಸೂಚ್ಯಂಕ_ದ(ಸ್ಥಿರಾಂಕ char16_t *srcChars,
               int32_t src_ಆರಂಭ,
               int32_t src_ಉದ್ದಳತೆ,
               int32_t ಆರಂಭ,
               int32_t length) ಸ್ಥಿರಾಂಕ
{
  ಆದರೆ (isBogus() || srcChars == ಶೂನ್ಯಸೂಚಕ || src_ಆರಂಭ < 0 || src_ಉದ್ದಳತೆ == 0) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // UnicodeString does not find empty substrings
  ಆದರೆ(src_ಉದ್ದಳತೆ < 0 && srcChars[src_ಆರಂಭ] == 0) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // get the indices within bounds
  pinIndices(ಆರಂಭ, length);

  // find the first occurrence of the substring
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_str_ಕಂಡುಹಿಡಿ_ಮೊದಲ(array + ಆರಂಭ, length, srcChars + src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

int32_t
UnicodeString::do_ಸೂಚ್ಯಂಕ_ದ(char16_t c,
             int32_t ಆರಂಭ,
             int32_t length) ಸ್ಥಿರಾಂಕ
{
  // pin indices
  pinIndices(ಆರಂಭ, length);

  // find the first occurrence of c
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_memchr(array + ಆರಂಭ, c, length);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

int32_t
UnicodeString::do_ಸೂಚ್ಯಂಕ_ದ(UChar32 c,
                         int32_t ಆರಂಭ,
                         int32_t length) ಸ್ಥಿರಾಂಕ {
  // pin indices
  pinIndices(ಆರಂಭ, length);

  // find the first occurrence of c
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_memchr32(array + ಆರಂಭ, c, length);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

int32_t 
UnicodeString::ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(ಸ್ಥಿರಾಂಕ char16_t *srcChars,
               int32_t src_ಆರಂಭ,
               int32_t src_ಉದ್ದಳತೆ,
               int32_t ಆರಂಭ,
               int32_t length) ಸ್ಥಿರಾಂಕ
{
  ಆದರೆ (isBogus() || srcChars == ಶೂನ್ಯಸೂಚಕ || src_ಆರಂಭ < 0 || src_ಉದ್ದಳತೆ == 0) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // UnicodeString does not find empty substrings
  ಆದರೆ(src_ಉದ್ದಳತೆ < 0 && srcChars[src_ಆರಂಭ] == 0) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // get the indices within bounds
  pinIndices(ಆರಂಭ, length);

  // find the last occurrence of the substring
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_str_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ(array + ಆರಂಭ, length, srcChars + src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

int32_t
UnicodeString::do_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(char16_t c,
                 int32_t ಆರಂಭ,
                 int32_t length) ಸ್ಥಿರಾಂಕ
{
  ಆದರೆ(isBogus()) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // pin indices
  pinIndices(ಆರಂಭ, length);

  // find the last occurrence of c
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_memrchr(array + ಆರಂಭ, c, length);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

int32_t
UnicodeString::do_ಕೊನೆಯ_ಸೂಚ್ಯಂಕ_ದ(UChar32 c,
                             int32_t ಆರಂಭ,
                             int32_t length) ಸ್ಥಿರಾಂಕ {
  // pin indices
  pinIndices(ಆರಂಭ, length);

  // find the last occurrence of c
  ಸ್ಥಿರಾಂಕ char16_t *array = get_Array_ಆರಂಭ();
  ಸ್ಥಿರಾಂಕ char16_t *ಹೊಂದಿಕೆ = u_memrchr32(array + ಆರಂಭ, c, length);
  ಆದರೆ(ಹೊಂದಿಕೆ == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ -1;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ static_cast<int32_t>(ಹೊಂದಿಕೆ - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::ಕಂಡುಹಿಡಿ_And_Replace(int32_t ಆರಂಭ,
                              int32_t length,
                              ಸ್ಥಿರಾಂಕ UnicodeString& old_ಪಠ್ಯ,
                              int32_t old_ಆರಂಭ,
                              int32_t old_ಉದ್ದಳತೆ,
                              ಸ್ಥಿರಾಂಕ UnicodeString& new_ಪಠ್ಯ,
                              int32_t new_ಆರಂಭ,
                              int32_t new_ಉದ್ದಳತೆ) U_LIFETIME_BOUND
{
  ಆದರೆ(isBogus() || old_ಪಠ್ಯ.isBogus() || new_ಪಠ್ಯ.isBogus()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  pinIndices(ಆರಂಭ, length);
  old_ಪಠ್ಯ.pinIndices(old_ಆರಂಭ, old_ಉದ್ದಳತೆ);
  new_ಪಠ್ಯ.pinIndices(new_ಆರಂಭ, new_ಉದ್ದಳತೆ);

  ಆದರೆ(old_ಉದ್ದಳತೆ == 0) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  ಆಗ(length > 0 && length >= old_ಉದ್ದಳತೆ) {
    int32_t pos = ಸೂಚ್ಯಂಕ_ದ(old_ಪಠ್ಯ, old_ಆರಂಭ, old_ಉದ್ದಳತೆ, ಆರಂಭ, length);
    ಆದರೆ(pos < 0) {
      // no more oldText's here: done
      ನಿಲ್ಲಿಸು;
    } ಇಲ್ಲದಿದ್ದರೆ {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_ಉದ್ದಳತೆ, new_ಪಠ್ಯ, new_ಆರಂಭ, new_ಉದ್ದಳತೆ);
      length -= pos + old_ಉದ್ದಳತೆ - ಆರಂಭ;
      ಆರಂಭ = pos + new_ಉದ್ದಳತೆ;
    }
  }

  ಹಿಂತಿರುಗಿ *ಇದು;
}


ಶೂನ್ಯ
UnicodeString::ಗಣ_ಗಮ್ಯ_Bogus()
{
  releaseArray();

  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kIsBogus;
  fUnion.fFields.fArray = ಶೂನ್ಯಸೂಚಕ;
  fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = 0;
}

// turn a bogus string into an empty one
ಶೂನ್ಯ
UnicodeString::unBogus() {
  ಆದರೆ(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & kIsBogus) {
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
  }
}

ಸ್ಥಿರಾಂಕ char16_t*
UnicodeString::get_Terminated_ಬಫರ್() U_LIFETIME_BOUND {
  ಆದರೆ(!isWritable()) {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  char16_t *array = get_Array_ಆರಂಭ();
  int32_t len = length();
  ಆದರೆ(len < get_ಸಾಮರ್ಥ್ಯ()) {
    ಆದರೆ(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & k_ಬಫರ್_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      ಆದರೆ(array[len] == 0) {
        ಹಿಂತಿರುಗಿ array;
      }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(((fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & kRefCounted) == 0 || ref_ಎಣಿಕೆ() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ಹಿಂತಿರುಗಿ array;
    }
  }
  ಆದರೆ(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = get_Array_ಆರಂಭ();
    array[len] = 0;
    ಹಿಂತಿರುಗಿ array;
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::ಗಣ_ಗಮ್ಯ(UBool isTerminated,
                     ConstChar16Ptr ಪಠ್ಯ_Ptr U_LIFETIME_BOUND,
                     int32_t ಪಠ್ಯ_ಉದ್ದಳತೆ) U_LIFETIME_BOUND
{
  ಆದರೆ(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & k_Open_Get_ಬಫರ್) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  ಸ್ಥಿರಾಂಕ char16_t *ಪಠ್ಯ = ಪಠ್ಯ_Ptr;
  ಆದರೆ(ಪಠ್ಯ == ಶೂನ್ಯಸೂಚಕ) {
    // treat as an empty string, do not alias
    releaseArray();
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  ಆದರೆ( ಪಠ್ಯ_ಉದ್ದಳತೆ < -1 ||
      (ಪಠ್ಯ_ಉದ್ದಳತೆ == -1 && !isTerminated) ||
      (ಪಠ್ಯ_ಉದ್ದಳತೆ >= 0 && isTerminated && ಪಠ್ಯ[ಪಠ್ಯ_ಉದ್ದಳತೆ] != 0)
  ) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  releaseArray();

  ಆದರೆ(ಪಠ್ಯ_ಉದ್ದಳತೆ == -1) {
    // text is terminated, or else it would have failed the above test
    ಪಠ್ಯ_ಉದ್ದಳತೆ = u_strlen(ಪಠ್ಯ);
  }
  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kReadonlyAlias;
  ಗಣ_Array(const_cast<char16_t*>(ಪಠ್ಯ), ಪಠ್ಯ_ಉದ್ದಳತೆ, isTerminated ? ಪಠ್ಯ_ಉದ್ದಳತೆ + 1 : ಪಠ್ಯ_ಉದ್ದಳತೆ);
  ಹಿಂತಿರುಗಿ *ಇದು;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::ಗಣ_ಗಮ್ಯ(char16_t* ಬಫರ್,
                     int32_t buff_ಉದ್ದಳತೆ U_LIFETIME_BOUND,
                     int32_t buff_ಸಾಮರ್ಥ್ಯ) U_LIFETIME_BOUND {
  ಆದರೆ(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & k_Open_Get_ಬಫರ್) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  ಆದರೆ(ಬಫರ್ == ಶೂನ್ಯಸೂಚಕ) {
    // treat as an empty string, do not alias
    releaseArray();
    ಗಣ_ಗಮ್ಯ_ಖಾಲಿಗಳು();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  ಆದರೆ(buff_ಉದ್ದಳತೆ < -1 || buff_ಸಾಮರ್ಥ್ಯ < 0 || buff_ಉದ್ದಳತೆ > buff_ಸಾಮರ್ಥ್ಯ) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(buff_ಉದ್ದಳತೆ == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ಸ್ಥಿರಾಂಕ char16_t *p = ಬಫರ್, *ಎಲ್ಲೆ = ಬಫರ್ + buff_ಸಾಮರ್ಥ್ಯ;
    ಆಗ(p != ಎಲ್ಲೆ && *p != 0) {
      ++p;
    }
    buff_ಉದ್ದಳತೆ = static_cast<int32_t>(p - ಬಫರ್);
  }

  releaseArray();

  fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = kWritableAlias;
  ಗಣ_Array(ಬಫರ್, buff_ಉದ್ದಳತೆ, buff_ಸಾಮರ್ಥ್ಯ);
  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString& UnicodeString::ಗಣ_ಗಮ್ಯ_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  ಆದರೆ(length <= US_STACKBUF_ಗಾತ್ರ) {
    capacity = US_STACKBUF_ಗಾತ್ರ;
  } ಇಲ್ಲದಿದ್ದರೆ {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_ಬಫರ್(capacity);
  int32_t length16;
  UError_ಸಂಕೇತ error_ಸಂಕೇತ = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_ಸಾಮರ್ಥ್ಯ(), &length16,
      utf8.ದತ್ತಾಂಶ(), length,
      0xfffd,  // Substitution character.
      ಶೂನ್ಯಸೂಚಕ,    // Don't care about number of substitutions.
      &error_ಸಂಕೇತ);
  release_ಬಫರ್(length16);
  ಆದರೆ(U_FAILURE(error_ಸಂಕೇತ)) {
    ಗಣ_ಗಮ್ಯ_Bogus();
  }
  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString&
UnicodeString::ಗಣ_Char_At(int32_t ಸ್ಥಾನಾಂತರ, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  ಆದರೆ(cloneArrayIfNeeded() && len > 0) {
    ಆದರೆ(ಸ್ಥಾನಾಂತರ < 0) {
      ಸ್ಥಾನಾಂತರ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸ್ಥಾನಾಂತರ >= len) {
      ಸ್ಥಾನಾಂತರ = len - 1;
    }

    get_Array_ಆರಂಭ()[ಸ್ಥಾನಾಂತರ] = c;
  }
  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString&
UnicodeString::replace(int32_t ಆರಂಭ,
                       int32_t ಉದ್ದಳತೆ_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t ಬಫರ್[U16_MAX_ಉದ್ದಳತೆ];
  int32_t count = 0;
  UBool isError = ಸುಳ್ಳು;
  U16_APPEND(ಬಫರ್, count, U16_MAX_ಉದ್ದಳತೆ, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ಹಿಂತಿರುಗಿ doReplace(ಆರಂಭ, ಉದ್ದಳತೆ_4, ಬಫರ್, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t ಬಫರ್[U16_MAX_ಉದ್ದಳತೆ];
  int32_t ಉದ್ದಳತೆ_4 = 0;
  UBool isError = ಸುಳ್ಳು;
  U16_APPEND(ಬಫರ್, ಉದ್ದಳತೆ_4, U16_MAX_ಉದ್ದಳತೆ, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ಹಿಂತಿರುಗಿ isError ? *ಇದು : doAppend(ಬಫರ್, 0, ಉದ್ದಳತೆ_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t ಆರಂಭ,
                         int32_t length,
                         ಸ್ಥಿರಾಂಕ UnicodeString& src,
                         int32_t src_ಆರಂಭ,
                         int32_t src_ಉದ್ದಳತೆ) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_ಆರಂಭ, src_ಉದ್ದಳತೆ);

  // get the characters from src
  // and replace the range in ourselves with them
  ಹಿಂತಿರುಗಿ doReplace(ಆರಂಭ, length, src.get_Array_ಆರಂಭ(), src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
}

UnicodeString&
UnicodeString::doReplace(int32_t ಆರಂಭ,
                         int32_t length,
                         ಸ್ಥಿರಾಂಕ char16_t* srcChars,
                         int32_t src_ಆರಂಭ,
                         int32_t src_ಉದ್ದಳತೆ) U_LIFETIME_BOUND
{
  ಆದರೆ(!isWritable()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  int32_t old_ಉದ್ದಳತೆ = ಇದು->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  ಆದರೆ((fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags&k_ಬಫರ್_Is_Readonly) && src_ಉದ್ದಳತೆ == 0) {
    ಆದರೆ(ಆರಂಭ == 0) {
      // remove prefix by adjusting the array pointer
      pin_ಸೂಚ್ಯಂಕ(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ -= length;
      ಗಣ_ಉದ್ದಳತೆ(old_ಉದ್ದಳತೆ - length);
      ಹಿಂತಿರುಗಿ *ಇದು;
    } ಇಲ್ಲದಿದ್ದರೆ {
      pin_ಸೂಚ್ಯಂಕ(ಆರಂಭ);
      ಆದರೆ(length >= (old_ಉದ್ದಳತೆ - ಆರಂಭ)) {
        // remove suffix by reducing the length (like truncate())
        ಗಣ_ಉದ್ದಳತೆ(ಆರಂಭ);
        fUnion.fFields.f_ಸಾಮರ್ಥ್ಯ = ಆರಂಭ;  // not NUL-terminated any more
        ಹಿಂತಿರುಗಿ *ಇದು;
      }
    }
  }

  ಆದರೆ(ಆರಂಭ == old_ಉದ್ದಳತೆ) {
    ಹಿಂತಿರುಗಿ doAppend(srcChars, src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
  }

  ಆದರೆ (srcChars == ಶೂನ್ಯಸೂಚಕ) {
    src_ಉದ್ದಳತೆ = 0;
  } ಇಲ್ಲದಿದ್ದರೆ {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_ಆರಂಭ;
    ಆದರೆ (src_ಉದ್ದಳತೆ < 0) {
      // get the srcLength if necessary
      src_ಉದ್ದಳತೆ = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(ಆರಂಭ, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ - length;
  ಆದರೆ(src_ಉದ್ದಳತೆ > (INT32_MAX - new_ಉದ್ದಳತೆ)) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  new_ಉದ್ದಳತೆ += src_ಉದ್ದಳತೆ;

  // Check for insertion into ourself
  ಸ್ಥಿರಾಂಕ char16_t *oldArray = get_Array_ಆರಂಭ();
  ಆದರೆ (is_ಬಫರ್_Writable() &&
      oldArray < srcChars + src_ಉದ್ದಳತೆ &&
      srcChars < oldArray + old_ಉದ್ದಳತೆ) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_ಉದ್ದಳತೆ);
    ಆದರೆ (copy.isBogus()) {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
    ಹಿಂತಿರುಗಿ doReplace(ಆರಂಭ, length, copy.get_Array_ಆರಂಭ(), 0, src_ಉದ್ದಳತೆ);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ರಾಶಿ_ಬಫರ್[US_STACKBUF_ಗಾತ್ರ];
  ಆದರೆ((fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags&k_Using_ರಾಶಿ_ಬಫರ್) && (new_ಉದ್ದಳತೆ > US_STACKBUF_ಗಾತ್ರ)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ರಾಶಿ_ಬಫರ್, oldArray, old_ಉದ್ದಳತೆ);
    oldArray = old_ರಾಶಿ_ಬಫರ್;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *ಬಫರ್_ಗಮ್ಯ_Delete = ಶೂನ್ಯಸೂಚಕ;
  ಆದರೆ(!cloneArrayIfNeeded(new_ಉದ್ದಳತೆ, get_Grow_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ),
                         ಸುಳ್ಳು, &ಬಫರ್_ಗಮ್ಯ_Delete)
  ) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // now do the replace

  char16_t *newArray = get_Array_ಆರಂಭ();
  ಆದರೆ(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, ಆರಂಭ);
    us_arrayCopy(oldArray, ಆರಂಭ + length,
                 newArray, ಆರಂಭ + src_ಉದ್ದಳತೆ,
                 old_ಉದ್ದಳತೆ - (ಆರಂಭ + length));
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(length != src_ಉದ್ದಳತೆ) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, ಆರಂಭ + length,
                 newArray, ಆರಂಭ + src_ಉದ್ದಳತೆ,
                 old_ಉದ್ದಳತೆ - (ಆರಂಭ + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, ಆರಂಭ, src_ಉದ್ದಳತೆ);

  ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  ಆದರೆ (ಬಫರ್_ಗಮ್ಯ_Delete) {
    uprv_free(ಬಫರ್_ಗಮ್ಯ_Delete);
  }

  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString&
UnicodeString::doReplace(int32_t ಆರಂಭ, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  ಆದರೆ (!isWritable()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  ಆದರೆ (src.length() > INT32_MAX) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  ಹಿಂತಿರುಗಿ doReplace(ಆರಂಭ, length, src.ದತ್ತಾಂಶ(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ಸ್ಥಿರಾಂಕ UnicodeString& src,
                        int32_t src_ಆರಂಭ,
                        int32_t src_ಉದ್ದಳತೆ) U_LIFETIME_BOUND {
  ಆದರೆ(src_ಉದ್ದಳತೆ == 0) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // pin the indices to legal values
  src.pinIndices(src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
  ಹಿಂತಿರುಗಿ doAppend(src.get_Array_ಆರಂಭ(), src_ಆರಂಭ, src_ಉದ್ದಳತೆ);
}

UnicodeString&
UnicodeString::doAppend(ಸ್ಥಿರಾಂಕ char16_t* srcChars,
                        int32_t src_ಆರಂಭ,
                        int32_t src_ಉದ್ದಳತೆ) U_LIFETIME_BOUND {
  ಆದರೆ(!isWritable() || src_ಉದ್ದಳತೆ == 0 || srcChars == ಶೂನ್ಯಸೂಚಕ) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_ಆರಂಭ;

  ಆದರೆ(src_ಉದ್ದಳತೆ < 0) {
    // get the srcLength if necessary
    ಆದರೆ((src_ಉದ್ದಳತೆ = u_strlen(srcChars)) == 0) {
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
  }

  int32_t old_ಉದ್ದಳತೆ = length();
  int32_t new_ಉದ್ದಳತೆ;

  ಆದರೆ (src_ಉದ್ದಳತೆ <= get_ಸಾಮರ್ಥ್ಯ() - old_ಉದ್ದಳತೆ && is_ಬಫರ್_Writable()) {
    new_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ + src_ಉದ್ದಳತೆ;
    // Faster than a memmove
    ಆದರೆ (src_ಉದ್ದಳತೆ <= 4) {
      char16_t *arr = get_Array_ಆರಂಭ();
      arr[old_ಉದ್ದಳತೆ] = srcChars[0];
      ಆದರೆ (src_ಉದ್ದಳತೆ > 1) arr[old_ಉದ್ದಳತೆ+1] = srcChars[1];
      ಆದರೆ (src_ಉದ್ದಳತೆ > 2) arr[old_ಉದ್ದಳತೆ+2] = srcChars[2];
      ಆದರೆ (src_ಉದ್ದಳತೆ > 3) arr[old_ಉದ್ದಳತೆ+3] = srcChars[3];
      ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ (uprv_add32_ಅತಿಪ್ರವಾಹ(old_ಉದ್ದಳತೆ, src_ಉದ್ದಳತೆ, &new_ಉದ್ದಳತೆ)) {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ *ಇದು;
    }

    // Check for append onto ourself
    ಸ್ಥಿರಾಂಕ char16_t* oldArray = get_Array_ಆರಂಭ();
    ಆದರೆ (is_ಬಫರ್_Writable() &&
        oldArray < srcChars + src_ಉದ್ದಳತೆ &&
        srcChars < oldArray + old_ಉದ್ದಳತೆ) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_ಉದ್ದಳತೆ);
      ಆದರೆ (copy.isBogus()) {
        ಗಣ_ಗಮ್ಯ_Bogus();
        ಹಿಂತಿರುಗಿ *ಇದು;
      }
      ಹಿಂತಿರುಗಿ doAppend(copy.get_Array_ಆರಂಭ(), 0, src_ಉದ್ದಳತೆ);
    }

    // optimize append() onto a large-enough, owned string
    ಆದರೆ (!cloneArrayIfNeeded(new_ಉದ್ದಳತೆ, get_Grow_ಸಾಮರ್ಥ್ಯ(new_ಉದ್ದಳತೆ))) {
      ಹಿಂತಿರುಗಿ *ಇದು;
    }
  }

  char16_t *newArray = get_Array_ಆರಂಭ();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  ಆದರೆ(srcChars != newArray + old_ಉದ್ದಳತೆ) {
    us_arrayCopy(srcChars, 0, newArray, old_ಉದ್ದಳತೆ, src_ಉದ್ದಳತೆ);
  }
  ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);

  ಹಿಂತಿರುಗಿ *ಇದು;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  ಆದರೆ (!isWritable() || src.empty()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  ಆದರೆ (src.length() > INT32_MAX) {
    ಗಣ_ಗಮ್ಯ_Bogus();
    ಹಿಂತಿರುಗಿ *ಇದು;
  }
  ಹಿಂತಿರುಗಿ doAppend(src.ದತ್ತಾಂಶ(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
ಶೂನ್ಯ
UnicodeString::handleReplaceBetween(int32_t ಆರಂಭ,
                                    int32_t ಎಲ್ಲೆ,
                                    ಸ್ಥಿರಾಂಕ UnicodeString& ಪಠ್ಯ) {
    replaceBetween(ಆರಂಭ, ಎಲ್ಲೆ, ಪಠ್ಯ);
}

/**
 * Replaceable API
 */
ಶೂನ್ಯ 
UnicodeString::copy(int32_t ಆರಂಭ, int32_t ಎಲ್ಲೆ, int32_t dest) {
    ಆದರೆ (ಎಲ್ಲೆ <= ಆರಂಭ) {
        ಹಿಂತಿರುಗಿ; // Nothing to do; avoid bogus malloc call
    }
    char16_t* ಪಠ್ಯ = static_cast<char16_t*>(uprv_malloc(ಗಾತ್ರ(char16_t) * (ಎಲ್ಲೆ - ಆರಂಭ)));
    // Check to make sure text is not null.
    ಆದರೆ (ಪಠ್ಯ != ಶೂನ್ಯಸೂಚಕ) {
	    ಹೊರತೆಗೆ_Between(ಆರಂಭ, ಎಲ್ಲೆ, ಪಠ್ಯ, 0);
	    insert(dest, ಪಠ್ಯ, 0, ಎಲ್ಲೆ - ಆರಂಭ);    
	    uprv_free(ಪಠ್ಯ);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ನಿಜ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_ದತ್ತಾಂಶ() ಸ್ಥಿರಾಂಕ {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

UnicodeString&
UnicodeString::do_ಹಿಮ್ಮುಖ(int32_t ಆರಂಭ, int32_t length) U_LIFETIME_BOUND {
  ಆದರೆ(length <= 1 || !cloneArrayIfNeeded()) {
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  // pin the indices to legal values
  pinIndices(ಆರಂಭ, length);
  ಆದರೆ(length <= 1) {  // pinIndices() might have shrunk the length
    ಹಿಂತಿರುಗಿ *ಇದು;
  }

  char16_t *ಎಡ = get_Array_ಆರಂಭ() + ಆರಂಭ;
  char16_t *ಬಲ = ಎಡ + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = ಸುಳ್ಳು;

  // Before the loop we know left<right because length>=2.
  ಮಾಡು {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *ಎಡ));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ಎಡ++ = *ಬಲ));
    *ಬಲ-- = swap;
  } ಆಗ(ಎಡ < ಬಲ);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ಎಡ));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  ಆದರೆ(hasSupplementary) {
    char16_t swap2;

    ಎಡ = get_Array_ಆರಂಭ() + ಆರಂಭ;
    ಬಲ = ಎಡ + length - 1; // -1 so that we can look at *(left+1) if left<right
    ಆಗ(ಎಡ < ಬಲ) {
      ಆದರೆ(U16_IS_TRAIL(swap = *ಎಡ) && U16_IS_LEAD(swap2 = *(ಎಡ + 1))) {
        *ಎಡ++ = swap2;
        *ಎಡ++ = swap;
      } ಇಲ್ಲದಿದ್ದರೆ {
        ++ಎಡ;
      }
    }
  }

  ಹಿಂತಿರುಗಿ *ಇದು;
}

UBool 
UnicodeString::padLeading(int32_t ಗುರಿ_ಉದ್ದಳತೆ,
                          char16_t padChar)
{
  int32_t old_ಉದ್ದಳತೆ = length();
  ಆದರೆ(old_ಉದ್ದಳತೆ >= ಗುರಿ_ಉದ್ದಳತೆ || !cloneArrayIfNeeded(ಗುರಿ_ಉದ್ದಳತೆ)) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  } ಇಲ್ಲದಿದ್ದರೆ {
    // move contents up by padding width
    char16_t *array = get_Array_ಆರಂಭ();
    int32_t ಆರಂಭ = ಗುರಿ_ಉದ್ದಳತೆ - old_ಉದ್ದಳತೆ;
    us_arrayCopy(array, 0, array, ಆರಂಭ, old_ಉದ್ದಳತೆ);

    // fill in padding character
    ಆಗ(--ಆರಂಭ >= 0) {
      array[ಆರಂಭ] = padChar;
    }
    ಗಣ_ಉದ್ದಳತೆ(ಗುರಿ_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t ಗುರಿ_ಉದ್ದಳತೆ,
                           char16_t padChar)
{
  int32_t old_ಉದ್ದಳತೆ = length();
  ಆದರೆ(old_ಉದ್ದಳತೆ >= ಗುರಿ_ಉದ್ದಳತೆ || !cloneArrayIfNeeded(ಗುರಿ_ಉದ್ದಳತೆ)) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  } ಇಲ್ಲದಿದ್ದರೆ {
    // fill in padding character
    char16_t *array = get_Array_ಆರಂಭ();
    int32_t length = ಗುರಿ_ಉದ್ದಳತೆ;
    ಆಗ(--length >= old_ಉದ್ದಳತೆ) {
      array[length] = padChar;
    }
    ಗಣ_ಉದ್ದಳತೆ(ಗುರಿ_ಉದ್ದಳತೆ);
    ಹಿಂತಿರುಗಿ ನಿಜ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_ಸಂಕೇತ() ಸ್ಥಿರಾಂಕ
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_ಸಂಕೇತ = ustr_hashUCharsN(get_Array_ಆರಂಭ(), length());
    ಆದರೆ (hash_ಸಂಕೇತ == k_Invalid_Hash_ಸಂಕೇತ) {
        hash_ಸಂಕೇತ = k_ಖಾಲಿಗಳು_Hash_ಸಂಕೇತ;
    }
    ಹಿಂತಿರುಗಿ hash_ಸಂಕೇತ;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ) U_LIFETIME_BOUND {
  ಆದರೆ(min_ಸಾಮರ್ಥ್ಯ>=-1 && cloneArrayIfNeeded(min_ಸಾಮರ್ಥ್ಯ)) {
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags|=k_Open_Get_ಬಫರ್;
    ಗಣ_Zero_ಉದ್ದಳತೆ();
    ಹಿಂತಿರುಗಿ get_Array_ಆರಂಭ();
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
}

ಶೂನ್ಯ
UnicodeString::release_ಬಫರ್(int32_t new_ಉದ್ದಳತೆ) {
  ಆದರೆ(fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags&k_Open_Get_ಬಫರ್ && new_ಉದ್ದಳತೆ>=-1) {
    // set the new fLength
    int32_t capacity=get_ಸಾಮರ್ಥ್ಯ();
    ಆದರೆ(new_ಉದ್ದಳತೆ==-1) {
      // the new length is the string length, capped by fCapacity
      ಸ್ಥಿರಾಂಕ char16_t *array=get_Array_ಆರಂಭ(), *p=array, *ಎಲ್ಲೆ=array+capacity;
      ಆಗ(p<ಎಲ್ಲೆ && *p!=0) {
        ++p;
      }
      new_ಉದ್ದಳತೆ = static_cast<int32_t>(p - array);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಉದ್ದಳತೆ>capacity) {
      new_ಉದ್ದಳತೆ=capacity;
    }
    ಗಣ_ಉದ್ದಳತೆ(new_ಉದ್ದಳತೆ);
    fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags&=~k_Open_Get_ಬಫರ್;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_ಸಾಮರ್ಥ್ಯ,
                                  int32_t grow_ಸಾಮರ್ಥ್ಯ,
                                  UBool doCopyArray,
                                  int32_t **p_ಬಫರ್_ಗಮ್ಯ_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ == -1) {
    new_ಸಾಮರ್ಥ್ಯ = get_ಸಾಮರ್ಥ್ಯ();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  ಆದರೆ(!isWritable()) {
    ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  ಆದರೆ(forceClone ||
     fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & k_ಬಫರ್_Is_Readonly ||
     (fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags & kRefCounted && ref_ಎಣಿಕೆ() > 1) ||
     new_ಸಾಮರ್ಥ್ಯ > get_ಸಾಮರ್ಥ್ಯ()
  ) {
    // check growCapacity for default value and use of the stack buffer
    ಆದರೆ(grow_ಸಾಮರ್ಥ್ಯ < 0) {
      grow_ಸಾಮರ್ಥ್ಯ = new_ಸಾಮರ್ಥ್ಯ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ <= US_STACKBUF_ಗಾತ್ರ && grow_ಸಾಮರ್ಥ್ಯ > US_STACKBUF_ಗಾತ್ರ) {
      grow_ಸಾಮರ್ಥ್ಯ = US_STACKBUF_ಗಾತ್ರ;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ > grow_ಸಾಮರ್ಥ್ಯ) {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;  // bad inputs
    }
    ಆದರೆ(grow_ಸಾಮರ್ಥ್ಯ > k_Max_ಸಾಮರ್ಥ್ಯ) {
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }

    // save old values
    char16_t old_ರಾಶಿ_ಬಫರ್[US_STACKBUF_ಗಾತ್ರ];
    char16_t *oldArray;
    int32_t old_ಉದ್ದಳತೆ = length();
    int16_t flags = fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags;

    ಆದರೆ(flags&k_Using_ರಾಶಿ_ಬಫರ್) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      ಆದರೆ(doCopyArray && grow_ಸಾಮರ್ಥ್ಯ > US_STACKBUF_ಗಾತ್ರ) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ರಾಶಿ_Fields.f_ಬಫರ್, 0, old_ರಾಶಿ_ಬಫರ್, 0, old_ಉದ್ದಳತೆ);
        oldArray = old_ರಾಶಿ_ಬಫರ್;
      } ಇಲ್ಲದಿದ್ದರೆ {
        oldArray = ಶೂನ್ಯಸೂಚಕ; // no need to copy from the stack buffer to itself
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=ಶೂನ್ಯಸೂಚಕ); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    ಆದರೆ(allocate(grow_ಸಾಮರ್ಥ್ಯ) ||
       (new_ಸಾಮರ್ಥ್ಯ < grow_ಸಾಮರ್ಥ್ಯ && allocate(new_ಸಾಮರ್ಥ್ಯ))
    ) {
      ಆದರೆ(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_ಉದ್ದಳತೆ = old_ಉದ್ದಳತೆ;
        new_ಸಾಮರ್ಥ್ಯ = get_ಸಾಮರ್ಥ್ಯ();
        ಆದರೆ(new_ಸಾಮರ್ಥ್ಯ < min_ಉದ್ದಳತೆ) {
          min_ಉದ್ದಳತೆ = new_ಸಾಮರ್ಥ್ಯ;
        }
        ಆದರೆ(oldArray != ಶೂನ್ಯಸೂಚಕ) {
          us_arrayCopy(oldArray, 0, get_Array_ಆರಂಭ(), 0, min_ಉದ್ದಳತೆ);
        }
        ಗಣ_ಉದ್ದಳತೆ(min_ಉದ್ದಳತೆ);
      } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಣ_Zero_ಉದ್ದಳತೆ();
      }

      // release the old array
      ಆದರೆ(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_ಎಣಿಕೆ = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        ಆದರೆ(umtx_atomic_dec(p_Ref_ಎಣಿಕೆ) == 0) {
          ಆದರೆ (p_ಬಫರ್_ಗಮ್ಯ_Delete == ಶೂನ್ಯಸೂಚಕ) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((ಶೂನ್ಯ *)p_Ref_ಎಣಿಕೆ);
          } ಇಲ್ಲದಿದ್ದರೆ {
            // the caller requested to delete it himself
            *p_ಬಫರ್_ಗಮ್ಯ_Delete = reinterpret_cast<int32_t*>(p_Ref_ಎಣಿಕೆ);
          }
        }
      }
    } ಇಲ್ಲದಿದ್ದರೆ {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      ಆದರೆ(!(flags&k_Using_ರಾಶಿ_ಬಫರ್)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_ಉದ್ದಳತೆ_And_Flags = flags;
      ಗಣ_ಗಮ್ಯ_Bogus();
      ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
  }
  ಹಿಂತಿರುಗಿ ನಿಜ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_ಸಂಕೇತ_Unit(char16_t c) {
  ಹಿಂತಿರುಗಿ str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_ಸಂಕೇತ_ಬಿಂದು(UChar32 c) {
  char16_t ಬಫರ್[U16_MAX_ಉದ್ದಳತೆ];
  int32_t c_ಉದ್ದಳತೆ = 0;
  UBool isError = ಸುಳ್ಳು;
  U16_APPEND(ಬಫರ್, c_ಉದ್ದಳತೆ, U16_MAX_ಉದ್ದಳತೆ, c, isError);
  ಹಿಂತಿರುಗಿ !isError && str.doAppend(ಬಫರ್, 0, c_ಉದ್ದಳತೆ).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ಸ್ಥಿರಾಂಕ char16_t *s, int32_t length) {
  ಹಿಂತಿರುಗಿ str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_ಸಾಮರ್ಥ್ಯ(int32_t append_ಸಾಮರ್ಥ್ಯ) {
  ಹಿಂತಿರುಗಿ str.cloneArrayIfNeeded(str.length() + append_ಸಾಮರ್ಥ್ಯ);
}

char16_t* UnicodeStringAppendable::get_Append_ಬಫರ್(int32_t min_ಸಾಮರ್ಥ್ಯ,
                                                   int32_t desired_ಸಾಮರ್ಥ್ಯ_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_ಸಾಮರ್ಥ್ಯ,
                                                   int32_t* ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ) U_LIFETIME_BOUND {
  ಆದರೆ(min_ಸಾಮರ್ಥ್ಯ < 1 || scratch_ಸಾಮರ್ಥ್ಯ < min_ಸಾಮರ್ಥ್ಯ) {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ = 0;
    ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
  }
  int32_t old_ಉದ್ದಳತೆ = str.length();
  ಆದರೆ(min_ಸಾಮರ್ಥ್ಯ <= (k_Max_ಸಾಮರ್ಥ್ಯ - old_ಉದ್ದಳತೆ) &&
      desired_ಸಾಮರ್ಥ್ಯ_Hint <= (k_Max_ಸಾಮರ್ಥ್ಯ - old_ಉದ್ದಳತೆ) &&
      str.cloneArrayIfNeeded(old_ಉದ್ದಳತೆ + min_ಸಾಮರ್ಥ್ಯ, old_ಉದ್ದಳತೆ + desired_ಸಾಮರ್ಥ್ಯ_Hint)) {
    *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ = str.get_ಸಾಮರ್ಥ್ಯ() - old_ಉದ್ದಳತೆ;
    ಹಿಂತಿರುಗಿ str.get_Array_ಆರಂಭ() + old_ಉದ್ದಳತೆ;
  }
  *ಫಲಿತಾಂಶ_ಸಾಮರ್ಥ್ಯ = scratch_ಸಾಮರ್ಥ್ಯ;
  ಹಿಂತಿರುಗಿ scratch;
}

U_NAMESPACE_ಕೊನೆ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ಸ್ಥಿರಾಂಕ UElement ಕೀಲಿ) {
    ಸ್ಥಿರಾಂಕ UnicodeString *str = (ಸ್ಥಿರಾಂಕ UnicodeString*) ಕೀಲಿ.ಸೂಚಕ;
    ಹಿಂತಿರುಗಿ (str == ಶೂನ್ಯಸೂಚಕ) ? 0 : str->hash_ಸಂಕೇತ();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ಸ್ಥಿರಾಂಕ UElement key1, ಸ್ಥಿರಾಂಕ UElement key2) {
    ಸ್ಥಿರಾಂಕ UnicodeString *str1 = (ಸ್ಥಿರಾಂಕ UnicodeString*) key1.ಸೂಚಕ;
    ಸ್ಥಿರಾಂಕ UnicodeString *str2 = (ಸ್ಥಿರಾಂಕ UnicodeString*) key2.ಸೂಚಕ;
    ಆದರೆ (str1 == str2) {
        ಹಿಂತಿರುಗಿ ನಿಜ;
    }
    ಆದರೆ (str1 == ಶೂನ್ಯಸೂಚಕ || str2 == ಶೂನ್ಯಸೂಚಕ) {
        ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
    }
    ಹಿಂತಿರುಗಿ *str1 == *str2;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#ಆದರೆ defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#ಸೂಚನೆ GCC diagnostic push
#ಸೂಚನೆ GCC diagnostic ignored "-Wunused-function"
ಸ್ಥಿರ ಶೂನ್ಯ uprv_UnicodeStringDummy() {
    ಅಳಿಸು [] (ಹೊಸ UnicodeString[2]);
}
#ಸೂಚನೆ GCC diagnostic pop
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
