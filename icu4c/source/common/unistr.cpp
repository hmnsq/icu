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

#ਅਨ੍ਤਰ੍ਭਾਵ <string_view>

#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/appendable.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/putil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cstring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "cmemory.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/ustring.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/unistr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unicode/utf16.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uelement.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ustr_imp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umutex.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uassert.h"

#ਜੇ 0

#ਅਨ੍ਤਰ੍ਭਾਵ <iostream>
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ std;

//DEBUGGING
ਖਾਲੀ
print(ਅਚਲ UnicodeString& s,
      ਅਚਲ ਅੱਖਰ *name)
{
  char16_t c;
  ਬਾਹਰ << name << ":|";
  ਲਈ(ਪੂਰਨ i = 0; i < s.length(); ++i) {
    c = s[i];
    ਜੇ(c>= 0x007E || c < 0x0020)
      ਬਾਹਰ << "[0x" << hex << s[i] << "]";
    ਨਹੀਂਤਾਂ
      ਬਾਹਰ << (ਅੱਖਰ) s[i];
  }
  ਬਾਹਰ << '|' << ਸਤਰ_ਅੰਤ;
}

ਖਾਲੀ
print(ਅਚਲ char16_t *s,
      int32_t len,
      ਅਚਲ ਅੱਖਰ *name)
{
  char16_t c;
  ਬਾਹਰ << name << ":|";
  ਲਈ(ਪੂਰਨ i = 0; i < len; ++i) {
    c = s[i];
    ਜੇ(c>= 0x007E || c < 0x0020)
      ਬਾਹਰ << "[0x" << hex << s[i] << "]";
    ਨਹੀਂਤਾਂ
      ਬਾਹਰ << (ਅੱਖਰ) s[i];
  }
  ਬਾਹਰ << '|' << ਸਤਰ_ਅੰਤ;
}
// END DEBUGGING
#ਜੇ_ਅੰਤ

// Local function definitions for now

// need to copy areas that may overlap
ਸਥਿਰ
ਸਤਰਅੰਦਰ ਖਾਲੀ
us_arrayCopy(ਅਚਲ char16_t *src, int32_t src_ਸ਼ੁਰੂ,
         char16_t *dst, int32_t dst_ਸ਼ੁਰੂ, int32_t count)
{
  ਜੇ(count>0) {
    uprv_memmove(dst+dst_ਸ਼ੁਰੂ, src+src_ਸ਼ੁਰੂ, (size_t)count*ਆਕਾਰ(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ਸਥਿਰ char16_t U_CALLCONV
UnicodeString_charAt(int32_t ਵਿੱਥ, ਖਾਲੀ *context) {
    ਵਾਪਸ ((icu::UnicodeString*) context)->charAt(ਵਿੱਥ);
}
U_CDECL_ਸਿਰਾ

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
ਸੰਚਾਲਕ+ (ਅਚਲ UnicodeString &s1, ਅਚਲ UnicodeString &s2) {
  int32_t ਜੋੜ_Lengths;
  ਜੇ (uprv_add32_ਉਛਾਲ(s1.length(), s2.length(), &ਜੋੜ_Lengths)) {
    UnicodeString bogus;
    bogus.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ bogus;
  }
  ਜੇ (ਜੋੜ_Lengths != INT32_MAX) {
    ++ਜੋੜ_Lengths;  // space for a terminating NUL if we need one
  }
  ਵਾਪਸ UnicodeString(ਜੋੜ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_ਅੰਦਰੂਨੀ_Concat(ਅਚਲ UnicodeString &s1, std::u16string_view s2) {
  int32_t ਜੋੜ_Lengths;
  ਜੇ (s2.length() > INT32_MAX ||
      uprv_add32_ਉਛਾਲ(s1.length(), static_cast<int32_t>(s2.length()), &ਜੋੜ_Lengths)) {
    UnicodeString bogus;
    bogus.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ bogus;
  }
  ਜੇ (ਜੋੜ_Lengths != INT32_MAX) {
    ++ਜੋੜ_Lengths;  // space for a terminating NUL if we need one
  }
  ਵਾਪਸ UnicodeString(ਜੋੜ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

ਖਾਲੀ
UnicodeString::ਜੋੜੋ_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  ਵਾਪਸ umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_ਗਿਣਤੀ() ਅਚਲ {
  ਵਾਪਸ umtx_ਲੱਦੋ_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

ਖਾਲੀ
UnicodeString::releaseArray() {
  ਜੇ((fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = 0;
  ਜੇ (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } ਨਹੀਂਤਾਂ ਜੇ(c <= 0xffff) {
    int32_t length = count;
    ਜੇ(capacity < length) {
      capacity = length;
    }
    ਜੇ(allocate(capacity)) {
      char16_t *array = get_Array_ਸ਼ੁਰੂ();
      char16_t unit = static_cast<char16_t>(c);
      ਲਈ(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      ਸੈੱਟ_ਲੰਮਾਈ(length);
    }
  } ਨਹੀਂਤਾਂ {  // supplementary code point, write surrogate pairs
    ਜੇ(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ਵਾਪਸ;
    }
    int32_t length = count * 2;
    ਜੇ(capacity < length) {
      capacity = length;
    }
    ਜੇ(allocate(capacity)) {
      char16_t *array = get_Array_ਸ਼ੁਰੂ();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      ਲਈ(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      ਸੈੱਟ_ਲੰਮਾਈ(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kLength1 | kShortString;
  fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = ਝੂਠ;
  U16_APPEND(fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ, i, US_STACKBUF_ਮਾਪ, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  ਜੇ(!isError) {
    ਸੈੱਟ_Short_ਲੰਮਾਈ(i);
  }
}

UnicodeString::UnicodeString(ਅਚਲ char16_t *ਪਾਠ,
                             int32_t ਪਾਠ_ਲੰਮਾਈ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  doAppend(ਪਾਠ, 0, ਪਾਠ_ਲੰਮਾਈ);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr ਪਾਠ_Ptr U_LIFETIME_BOUND,
                             int32_t ਪਾਠ_ਲੰਮਾਈ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kReadonlyAlias;
  ਅਚਲ char16_t *ਪਾਠ = ਪਾਠ_Ptr;
  ਜੇ(ਪਾਠ == ਖਾਲੀ_ਸੂਚਕ) {
    // treat as an empty string, do not alias
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
  } ਨਹੀਂਤਾਂ ਜੇ(ਪਾਠ_ਲੰਮਾਈ < -1 ||
            (ਪਾਠ_ਲੰਮਾਈ == -1 && !isTerminated) ||
            (ਪਾਠ_ਲੰਮਾਈ >= 0 && isTerminated && ਪਾਠ[ਪਾਠ_ਲੰਮਾਈ] != 0)
  ) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  } ਨਹੀਂਤਾਂ {
    ਜੇ(ਪਾਠ_ਲੰਮਾਈ == -1) {
      // text is terminated, or else it would have failed the above test
      ਪਾਠ_ਲੰਮਾਈ = u_strlen(ਪਾਠ);
    }
    ਸੈੱਟ_Array(const_cast<char16_t *>(ਪਾਠ), ਪਾਠ_ਲੰਮਾਈ,
             isTerminated ? ਪਾਠ_ਲੰਮਾਈ + 1 : ਪਾਠ_ਲੰਮਾਈ);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_ਲੰਮਾਈ,
                             int32_t buff_ਸਮਰੱਥਾ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kWritableAlias;
  ਜੇ(buff == ਖਾਲੀ_ਸੂਚਕ) {
    // treat as an empty string, do not alias
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
  } ਨਹੀਂਤਾਂ ਜੇ(buff_ਲੰਮਾਈ < -1 || buff_ਸਮਰੱਥਾ < 0 || buff_ਲੰਮਾਈ > buff_ਸਮਰੱਥਾ) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  } ਨਹੀਂਤਾਂ {
    ਜੇ(buff_ਲੰਮਾਈ == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ਅਚਲ char16_t *p = buff, *ਹੱਦਬੰਦੀ = buff + buff_ਸਮਰੱਥਾ;
      ਜਦੋਂ(p != ਹੱਦਬੰਦੀ && *p != 0) {
        ++p;
      }
      buff_ਲੰਮਾਈ = static_cast<int32_t>(p - buff);
    }
    ਸੈੱਟ_Array(buff, buff_ਲੰਮਾਈ, buff_ਸਮਰੱਥਾ);
  }
}

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *src, int32_t length, EInvariant) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  ਜੇ(src==ਖਾਲੀ_ਸੂਚਕ) {
    // treat as an empty string
  } ਨਹੀਂਤਾਂ {
    ਜੇ(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    ਜੇ(cloneArrayIfNeeded(length, length, ਝੂਠ)) {
      u_chars_ਮੰਜ਼ਿਲ_UChars(src, get_Array_ਸ਼ੁਰੂ(), length);
      ਸੈੱਟ_ਲੰਮਾਈ(length);
    } ਨਹੀਂਤਾਂ {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view ਪਾਠ U_LIFETIME_BOUND) {
  UnicodeString ਨਤੀਜਾ;
  ਜੇ (ਪਾਠ.length() <= INT32_MAX) {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, ਪਾਠ.ਅੰਕੜੇ(), static_cast<int32_t>(ਪਾਠ.length()));
  } ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ਅਚਲ UnicodeString& ਪਾਠ U_LIFETIME_BOUND) {
  UnicodeString ਨਤੀਜਾ;
  ਜੇ (ਪਾਠ.isBogus()) {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  } ਨਹੀਂਤਾਂ {
    ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ(ਝੂਠ, ਪਾਠ.get_ਬਫਰ(), ਪਾਠ.length());
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  ਜੇ (codepage_ਅੰਕੜੇ != ਖਾਲੀ_ਸੂਚਕ) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_UTF8(codepage_ਅੰਕੜੇ);
  }
}

UnicodeString::UnicodeString(ਅਚਲ ਅੱਖਰ *codepage_ਅੰਕੜੇ, int32_t ਅੰਕੜੇ_ਲੰਮਾਈ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  ਜੇ (codepage_ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ_ਲੰਮਾਈ == 0 || ਅੰਕੜੇ_ਲੰਮਾਈ < -1) {
    ਵਾਪਸ;
  }
  ਜੇ(ਅੰਕੜੇ_ਲੰਮਾਈ == -1) {
    ਅੰਕੜੇ_ਲੰਮਾਈ = static_cast<int32_t>(uprv_strlen(codepage_ਅੰਕੜੇ));
  }
  ਸੈੱਟ_ਮੰਜ਼ਿਲ_UTF8(StringPiece(codepage_ਅੰਕੜੇ, ਅੰਕੜੇ_ਲੰਮਾਈ));
}

// else see unistr_cnv.cpp
#ਜੇ_ਅੰਤ

UnicodeString::UnicodeString(ਅਚਲ UnicodeString& that) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, ਸੱਚ);
}

UnicodeString::UnicodeString(ਅਚਲ UnicodeString& that,
                             int32_t src_ਸ਼ੁਰੂ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  ਸੈੱਟ_ਮੰਜ਼ਿਲ(that, src_ਸ਼ੁਰੂ);
}

UnicodeString::UnicodeString(ਅਚਲ UnicodeString& that,
                             int32_t src_ਸ਼ੁਰੂ,
                             int32_t src_ਲੰਮਾਈ) {
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
  ਸੈੱਟ_ਮੰਜ਼ਿਲ(that, src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ਅਚਲ {
  ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ਅਚਲ {
  Local_ਸੂਚਕ<UnicodeString> clonedString(ਨਵਾਂ UnicodeString(*ਇਹ));
  ਵਾਪਸ clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : ਖਾਲੀ_ਸੂਚਕ;
}

//========================================
// array allocation
//========================================

ਨਾਮਾਕਾਸ਼ {

ਅਚਲ int32_t k_Grow_ਮਾਪ = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ਅਚਲ int32_t k_Max_ਸਮਰੱਥਾ = 0x7ffffff5;

int32_t get_Grow_ਸਮਰੱਥਾ(int32_t new_ਲੰਮਾਈ) {
  int32_t grow_ਮਾਪ = (new_ਲੰਮਾਈ >> 2) + k_Grow_ਮਾਪ;
  ਜੇ(grow_ਮਾਪ <= (k_Max_ਸਮਰੱਥਾ - new_ਲੰਮਾਈ)) {
    ਵਾਪਸ new_ਲੰਮਾਈ + grow_ਮਾਪ;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ k_Max_ਸਮਰੱਥਾ;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  ਜੇ(capacity <= US_STACKBUF_ਮਾਪ) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
    ਵਾਪਸ ਸੱਚ;
  }
  ਜੇ(capacity <= k_Max_ਸਮਰੱਥਾ) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t ਅੰਕ_ਬਾਈਟਾਂ = ਆਕਾਰ(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    ਅੰਕ_ਬਾਈਟਾਂ = (ਅੰਕ_ਬਾਈਟਾਂ + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(ਅੰਕ_ਬਾਈਟਾਂ));
    ਜੇ(array != ਖਾਲੀ_ਸੂਚਕ) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      ਅੰਕ_ਬਾਈਟਾਂ -= ਆਕਾਰ(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_ਸਮਰੱਥਾ = static_cast<int32_t>(ਅੰਕ_ਬਾਈਟਾਂ / U_SIZEOF_UCHAR);
      fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kLongString;
      ਵਾਪਸ ਸੱਚ;
    }
  }
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kIsBogus;
  fUnion.fFields.fArray = ਖਾਲੀ_ਸੂਚਕ;
  fUnion.fFields.f_ਸਮਰੱਥਾ = 0;
  ਵਾਪਸ ਝੂਠ;
}

//========================================
// Destructor
//========================================

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNISTR_ਗਿਣਤੀ_FINAL_STRING_LENGTHS
ਸਥਿਰ u_atomic_int32_t final_ਲੰਮਾਈ_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ਸਥਿਰ u_atomic_int32_t beyond_ਗਿਣਤੀ(0);

U_CAPI ਖਾਲੀ unistr_printLengths() {
  int32_t i;
  ਲਈ(i = 0; i <= 59; ++i) {
    ਲਿਖੋ("%2d,  %9d\n", i, (int32_t)final_ਲੰਮਾਈ_Counts[i]);
  }
  int32_t beyond = beyond_ਗਿਣਤੀ;
  ਲਈ(; i < UPRV_LENGTHOF(final_ਲੰਮਾਈ_Counts); ++i) {
    beyond += final_ਲੰਮਾਈ_Counts[i];
  }
  ਲਿਖੋ(">59, %9d\n", beyond);
}
#ਜੇ_ਅੰਤ

UnicodeString::~UnicodeString()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNISTR_ਗਿਣਤੀ_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  ਜੇ((fUnion.fFields.f_ਲੰਮਾਈ_And_Flags&(k_Open_Get_ਬਫਰ|kReadonlyAlias|kWritableAlias)) == 0) {
    ਜੇ(has_Short_ਲੰਮਾਈ()) {
      umtx_atomic_inc(final_ਲੰਮਾਈ_Counts + get_Short_ਲੰਮਾਈ());
    } ਨਹੀਂਤਾਂ {
      umtx_atomic_inc(&beyond_ਗਿਣਤੀ);
    }
  }
#ਜੇ_ਅੰਤ

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString ਨਤੀਜਾ;
  ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_UTF8(utf8);
  ਵਾਪਸ ਨਤੀਜਾ;
}

UnicodeString UnicodeString::fromUTF32(ਅਚਲ UChar32 *utf32, int32_t length) {
  UnicodeString ਨਤੀਜਾ;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  ਜੇ(length <= US_STACKBUF_ਮਾਪ) {
    capacity = US_STACKBUF_ਮਾਪ;
  } ਨਹੀਂਤਾਂ {
    capacity = length + (length >> 4) + 4;
  }
  ਕਰੋ {
    char16_t *utf16 = ਨਤੀਜਾ.get_ਬਫਰ(capacity);
    int32_t length16;
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, ਨਤੀਜਾ.get_ਸਮਰੱਥਾ(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
        &error_ਕੋਡ);
    ਨਤੀਜਾ.release_ਬਫਰ(length16);
    ਜੇ(error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      ਜਾਰੀ;
    } ਨਹੀਂਤਾਂ ਜੇ(U_FAILURE(error_ਕੋਡ)) {
      ਨਤੀਜਾ.ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    }
    ਰੋਕੋ;
  } ਜਦੋਂ(ਸੱਚ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::ਸੰਚਾਲਕ=(ਅਚਲ UnicodeString& src) U_LIFETIME_BOUND {
  ਵਾਪਸ copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ਅਚਲ UnicodeString& src) U_LIFETIME_BOUND {
  ਵਾਪਸ copyFrom(src, ਸੱਚ);
}

UnicodeString&
UnicodeString::copyFrom(ਅਚਲ UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  ਜੇ(ਇਹ == &src) {
    ਵਾਪਸ *ਇਹ;
  }

  // is the right side bogus?
  ਜੇ(src.isBogus()) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  }

  // delete the current contents
  releaseArray();

  ਜੇ(src.is_ਖਾਲੀ_ਥਾਵਾਂ()) {
    // empty string - use the stack buffer
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
    ਵਾਪਸ *ਇਹ;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = src.fUnion.fFields.f_ਲੰਮਾਈ_And_Flags;
  ਚੋਣ(src.fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & kAllStorageFlags) {
  ਹਾਲਤ kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ, src.fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ,
                get_Short_ਲੰਮਾਈ() * U_SIZEOF_UCHAR);
    ਰੋਕੋ;
  ਹਾਲਤ kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).ਜੋੜੋ_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ਸਮਰੱਥਾ = src.fUnion.fFields.f_ਸਮਰੱਥਾ;
    ਜੇ(!has_Short_ਲੰਮਾਈ()) {
      fUnion.fFields.f_ਲੰਮਾਈ = src.fUnion.fFields.f_ਲੰਮਾਈ;
    }
    ਰੋਕੋ;
  ਹਾਲਤ kReadonlyAlias:
    ਜੇ(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_ਸਮਰੱਥਾ = src.fUnion.fFields.f_ਸਮਰੱਥਾ;
      ਜੇ(!has_Short_ਲੰਮਾਈ()) {
        fUnion.fFields.f_ਲੰਮਾਈ = src.fUnion.fFields.f_ਲੰਮਾਈ;
      }
      ਰੋਕੋ;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  ਹਾਲਤ kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_ਲੰਮਾਈ = src.length();
    ਜੇ(allocate(src_ਲੰਮਾਈ)) {
      u_memcpy(get_Array_ਸ਼ੁਰੂ(), src.get_Array_ਸ਼ੁਰੂ(), src_ਲੰਮਾਈ);
      ਸੈੱਟ_ਲੰਮਾਈ(src_ਲੰਮਾਈ);
      ਰੋਕੋ;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  ਮੂਲ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kIsBogus;
    fUnion.fFields.fArray = ਖਾਲੀ_ਸੂਚਕ;
    fUnion.fFields.f_ਸਮਰੱਥਾ = 0;
    ਰੋਕੋ;
  }

  ਵਾਪਸ *ਇਹ;
}

UnicodeString& UnicodeString::ਸੰਚਾਲਕ=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, ਸੱਚ);
  ਵਾਪਸ *ਇਹ;
}

// Same as move assignment except without memory management.
ਖਾਲੀ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool ਸੈੱਟ_Src_ਮੰਜ਼ਿਲ_Bogus) noexcept {
  int16_t ਲੰਮਾਈ_And_Flags = fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = src.fUnion.fFields.f_ਲੰਮਾਈ_And_Flags;
  ਜੇ(ਲੰਮਾਈ_And_Flags & k_Using_ਢੇਰ_ਬਫਰ) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    ਜੇ(ਇਹ != &src) {
      uprv_memcpy(fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ, src.fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ,
                  get_Short_ਲੰਮਾਈ() * U_SIZEOF_UCHAR);
    }
  } ਨਹੀਂਤਾਂ {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ਸਮਰੱਥਾ = src.fUnion.fFields.f_ਸਮਰੱਥਾ;
    ਜੇ(!has_Short_ਲੰਮਾਈ()) {
      fUnion.fFields.f_ਲੰਮਾਈ = src.fUnion.fFields.f_ਲੰਮਾਈ;
    }
    ਜੇ(ਸੈੱਟ_Src_ਮੰਜ਼ਿਲ_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = ਖਾਲੀ_ਸੂਚਕ;
      src.fUnion.fFields.f_ਸਮਰੱਥਾ = 0;
    }
  }
}

ਖਾਲੀ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString ਆਰਜ਼ੀ;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  ਆਰਜ਼ੀ.copyFieldsFrom(*ਇਹ, ਝੂਠ);
  ਇਹ->copyFieldsFrom(other, ਝੂਠ);
  other.copyFieldsFrom(ਆਰਜ਼ੀ, ਝੂਠ);
  // Set temp to an empty string so that other's memory is not released twice.
  ਆਰਜ਼ੀ.fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ਅਚਲ {
    UnicodeString ਨਤੀਜਾ(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    ਜੇ (ਨਤੀਜਾ.isBogus()) {
        ਵਾਪਸ ਨਤੀਜਾ;
    }
    ਅਚਲ char16_t *array = get_ਬਫਰ();
    int32_t len = length();
    int32_t prev = 0;
    ਲਈ (int32_t i=0;;) {
        ਜੇ (i == len) {
            ਨਤੀਜਾ.append(array, prev, len - prev);
            ਰੋਕੋ;
        }
        ਜੇ (array[i++] == 0x5C /*'\\'*/) {
            ਨਤੀਜਾ.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            ਜੇ (c < 0) {
                ਨਤੀਜਾ.remove(); // return empty string
                ਰੋਕੋ; // invalid escape sequence
            }
            ਨਤੀਜਾ.append(c);
            prev = i;
        }
    }
    ਵਾਪਸ ਨਤੀਜਾ;
}

UChar32 UnicodeString::unescapeAt(int32_t &ਵਿੱਥ) ਅਚਲ {
    ਵਾਪਸ u_unescapeAt(UnicodeString_charAt, &ਵਿੱਥ, length(), (ਖਾਲੀ*)ਇਹ);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ਅਚਲ char16_t *ਪਾਠ, int32_t len) ਅਚਲ {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ਵਾਪਸ uprv_memcmp(get_Array_ਸ਼ੁਰੂ(), ਪਾਠ, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t ਸ਼ੁਰੂ,
              int32_t length,
              ਅਚਲ char16_t *srcChars,
              int32_t src_ਸ਼ੁਰੂ,
              int32_t src_ਲੰਮਾਈ) ਅਚਲ
{
  // compare illegal string values
  ਜੇ(isBogus()) {
    ਵਾਪਸ ਝੂਠ;
  }
  
  // pin indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  ਜੇ(srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ਵਾਪਸ length == 0 ? ਸੱਚ : ਝੂਠ;
  }

  // get the correct pointer
  ਅਚਲ char16_t *chars = get_Array_ਸ਼ੁਰੂ();

  chars += ਸ਼ੁਰੂ;
  srcChars += src_ਸ਼ੁਰੂ;

  // get the srcLength if necessary
  ਜੇ(src_ਲੰਮਾਈ < 0) {
    src_ਲੰਮਾਈ = u_strlen(srcChars + src_ਸ਼ੁਰੂ);
  }

  ਜੇ (length != src_ਲੰਮਾਈ) {
    ਵਾਪਸ ਝੂਠ;
  }

  ਜੇ(length == 0 || chars == srcChars) {
    ਵਾਪਸ ਸੱਚ;
  }

  ਵਾਪਸ u_memcmp(chars, srcChars, src_ਲੰਮਾਈ) == 0;
}

int8_t
UnicodeString::doCompare( int32_t ਸ਼ੁਰੂ,
              int32_t length,
              ਅਚਲ char16_t *srcChars,
              int32_t src_ਸ਼ੁਰੂ,
              int32_t src_ਲੰਮਾਈ) ਅਚਲ
{
  // compare illegal string values
  ਜੇ(isBogus()) {
    ਵਾਪਸ -1;
  }
  
  // pin indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  ਜੇ(srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ਵਾਪਸ length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ਅਚਲ char16_t *chars = get_Array_ਸ਼ੁਰੂ();

  chars += ਸ਼ੁਰੂ;
  srcChars += src_ਸ਼ੁਰੂ;

  int32_t min_ਲੰਮਾਈ;
  int8_t ਲੰਮਾਈ_ਨਤੀਜਾ;

  // get the srcLength if necessary
  ਜੇ(src_ਲੰਮਾਈ < 0) {
    src_ਲੰਮਾਈ = u_strlen(srcChars + src_ਸ਼ੁਰੂ);
  }

  // are we comparing different lengths?
  ਜੇ(length != src_ਲੰਮਾਈ) {
    ਜੇ(length < src_ਲੰਮਾਈ) {
      min_ਲੰਮਾਈ = length;
      ਲੰਮਾਈ_ਨਤੀਜਾ = -1;
    } ਨਹੀਂਤਾਂ {
      min_ਲੰਮਾਈ = src_ਲੰਮਾਈ;
      ਲੰਮਾਈ_ਨਤੀਜਾ = 1;
    }
  } ਨਹੀਂਤਾਂ {
    min_ਲੰਮਾਈ = length;
    ਲੰਮਾਈ_ਨਤੀਜਾ = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  ਜੇ(min_ਲੰਮਾਈ > 0 && chars != srcChars) {
    int32_t ਨਤੀਜਾ;

#   ਜੇ U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      ਨਤੀਜਾ = uprv_memcmp(chars, srcChars, min_ਲੰਮਾਈ * ਆਕਾਰ(char16_t));
      ਜੇ(ਨਤੀਜਾ != 0) {
        ਵਾਪਸ (int8_t)(ਨਤੀਜਾ >> 15 | 1);
      }
#   ਨਹੀਂਤਾਂ
      // little-endian: compare char16_t units
      ਕਰੋ {
        ਨਤੀਜਾ = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        ਜੇ(ਨਤੀਜਾ != 0) {
          ਵਾਪਸ static_cast<int8_t>(ਨਤੀਜਾ >> 15 | 1);
        }
      } ਜਦੋਂ(--min_ਲੰਮਾਈ > 0);
#   endif
  }
  ਵਾਪਸ ਲੰਮਾਈ_ਨਤੀਜਾ;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_ਕੋਡ_ਬਿੰਦੂ_ਕ੍ਰਮ(int32_t ਸ਼ੁਰੂ,
                                       int32_t length,
                                       ਅਚਲ char16_t *srcChars,
                                       int32_t src_ਸ਼ੁਰੂ,
                                       int32_t src_ਲੰਮਾਈ) ਅਚਲ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ਜੇ(isBogus()) {
    ਵਾਪਸ -1;
  }

  // pin indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  ਜੇ(srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    src_ਸ਼ੁਰੂ = src_ਲੰਮਾਈ = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_ਸ਼ੁਰੂ() + ਸ਼ੁਰੂ, length, (srcChars!=ਖਾਲੀ_ਸੂਚਕ)?(srcChars + src_ਸ਼ੁਰੂ):ਖਾਲੀ_ਸੂਚਕ, src_ਲੰਮਾਈ, ਝੂਠ, ਸੱਚ);
  /* translate the 32-bit result into an 8-bit one */
  ਜੇ(diff!=0) {
    ਵਾਪਸ static_cast<int8_t>(diff >> 15 | 1);
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ 0;
  }
}

int32_t
UnicodeString::get_ਲੰਮਾਈ() ਅਚਲ {
    ਵਾਪਸ length();
}

char16_t
UnicodeString::getCharAt(int32_t ਵਿੱਥ) ਅਚਲ {
  ਵਾਪਸ charAt(ਵਿੱਥ);
}

UChar32
UnicodeString::getChar32At(int32_t ਵਿੱਥ) ਅਚਲ {
  ਵਾਪਸ char32At(ਵਿੱਥ);
}

UChar32
UnicodeString::char32At(int32_t ਵਿੱਥ) ਅਚਲ
{
  int32_t len = length();
  ਜੇ (static_cast<uint32_t>(ਵਿੱਥ) < static_cast<uint32_t>(len)) {
    ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
    UChar32 c;
    U16_GET(array, 0, ਵਿੱਥ, len, c);
    ਵਾਪਸ c;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_ਸ਼ੁਰੂ(int32_t ਵਿੱਥ) ਅਚਲ {
  ਜੇ (static_cast<uint32_t>(ਵਿੱਥ) < static_cast<uint32_t>(length())) {
    ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
    U16_ਸੈੱਟ_CP_ਸ਼ੁਰੂ(array, 0, ਵਿੱਥ);
    ਵਾਪਸ ਵਿੱਥ;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ 0;
  }
}

int32_t
UnicodeString::get_Char32_ਹੱਦਬੰਦੀ(int32_t ਵਿੱਥ) ਅਚਲ {
  int32_t len = length();
  ਜੇ (static_cast<uint32_t>(ਵਿੱਥ) < static_cast<uint32_t>(len)) {
    ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
    U16_ਸੈੱਟ_CP_ਹੱਦਬੰਦੀ(array, 0, ਵਿੱਥ, len);
    ਵਾਪਸ ਵਿੱਥ;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ len;
  }
}

int32_t
UnicodeString::ਗਿਣਤੀ_Char32(int32_t ਸ਼ੁਰੂ, int32_t length) ਅਚਲ {
  pinIndices(ਸ਼ੁਰੂ, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ਵਾਪਸ u_ਗਿਣਤੀ_Char32(get_Array_ਸ਼ੁਰੂ()+ਸ਼ੁਰੂ, length);
}

UBool
UnicodeString::has_ਹੋਰ_Char32_Than(int32_t ਸ਼ੁਰੂ, int32_t length, int32_t number) ਅਚਲ {
  pinIndices(ਸ਼ੁਰੂ, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ਵਾਪਸ u_str_Has_ਹੋਰ_Char32_Than(get_Array_ਸ਼ੁਰੂ()+ਸ਼ੁਰੂ, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t ਸੂਚਕਾਂਕ, int32_t delta) ਅਚਲ {
  // pin index
  int32_t len = length();
  ਜੇ(ਸੂਚਕਾਂਕ<0) {
    ਸੂਚਕਾਂਕ=0;
  } ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ>len) {
    ਸੂਚਕਾਂਕ=len;
  }

  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਜੇ(delta>0) {
    U16_FWD_N(array, ਸੂਚਕਾਂਕ, len, delta);
  } ਨਹੀਂਤਾਂ {
    U16_ਪਿੱਠ_N(array, 0, ਸੂਚਕਾਂਕ, -delta);
  }

  ਵਾਪਸ ਸੂਚਕਾਂਕ;
}

ਖਾਲੀ
UnicodeString::do_ਕੱਢੋ(int32_t ਸ਼ੁਰੂ,
             int32_t length,
             char16_t *dst,
             int32_t dst_ਸ਼ੁਰੂ) ਅਚਲ
{
  // pin indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  // do not copy anything if we alias dst itself
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਜੇ(array + ਸ਼ੁਰੂ != dst + dst_ਸ਼ੁਰੂ) {
    us_arrayCopy(array, ਸ਼ੁਰੂ, dst, dst_ਸ਼ੁਰੂ, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_ਸਮਰੱਥਾ,
                       UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
  int32_t len = length();
  ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
    ਜੇ (isBogus() || dest_ਸਮਰੱਥਾ < 0 || (dest_ਸਮਰੱਥਾ > 0 && dest == ਖਾਲੀ_ਸੂਚਕ)) {
      error_ਕੋਡ=U_ILLEGAL_ਦਲੀਲ_ERROR;
    } ਨਹੀਂਤਾਂ {
      ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
      ਜੇ(len>0 && len<=dest_ਸਮਰੱਥਾ && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ਵਾਪਸ u_terminateUChars(dest, dest_ਸਮਰੱਥਾ, len, &error_ਕੋਡ);
    }
  }

  ਵਾਪਸ len;
}

int32_t
UnicodeString::extract(int32_t ਸ਼ੁਰੂ,
                       int32_t length,
                       ਅੱਖਰ *ਨਿਸ਼ਾਨਾ,
                       int32_t ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ,
                       ਗਿਣਤੀ EInvariant) ਅਚਲ
{
  // if the arguments are illegal, then do nothing
  ਜੇ(ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ < 0 || (ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ > 0 && ਨਿਸ਼ਾਨਾ == ਖਾਲੀ_ਸੂਚਕ)) {
    ਵਾਪਸ 0;
  }

  // pin the indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  ਜੇ(length <= ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ) {
    u_UChars_ਮੰਜ਼ਿਲ_Chars(get_Array_ਸ਼ੁਰੂ() + ਸ਼ੁਰੂ, ਨਿਸ਼ਾਨਾ, length);
  }
  UError_ਕੋਡ ਸਥਿਤੀ = U_ZERO_ERROR;
  ਵਾਪਸ u_terminateChars(ਨਿਸ਼ਾਨਾ, ਨਿਸ਼ਾਨਾ_ਸਮਰੱਥਾ, length, &ਸਥਿਤੀ);
}

UnicodeString
UnicodeString::ਆਰਜ਼ੀ_Sub_String(int32_t ਸ਼ੁਰੂ, int32_t len) ਅਚਲ U_LIFETIME_BOUND {
  pinIndices(ਸ਼ੁਰੂ, len);
  ਅਚਲ char16_t *array = get_ਬਫਰ();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  ਜੇ(array==ਖਾਲੀ_ਸੂਚਕ) {
    array=fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ਵਾਪਸ UnicodeString(ਝੂਠ, array + ਸ਼ੁਰੂ, len);
}

int32_t
UnicodeString::ਮੰਜ਼ਿਲ_UTF8(int32_t ਸ਼ੁਰੂ, int32_t len,
                      ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, int32_t capacity) ਅਚਲ {
  pinIndices(ਸ਼ੁਰੂ, len);
  int32_t length8;
  UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
  u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(ਨਿਸ਼ਾਨਾ, capacity, &length8,
                     get_ਬਫਰ() + ਸ਼ੁਰੂ, len,
                     0xFFFD,  // Standard substitution character.
                     ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
                     &error_ਕੋਡ);
  ਵਾਪਸ length8;
}

#ਜੇ U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t ਸ਼ੁਰੂ, int32_t len,
                       ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, uint32_t dst_ਮਾਪ) ਅਚਲ {
  // if the arguments are illegal, then do nothing
  ਜੇ (/*dstSize < 0 || */(dst_ਮਾਪ > 0 && ਨਿਸ਼ਾਨਾ == ਖਾਲੀ_ਸੂਚਕ)) {
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ ਮੰਜ਼ਿਲ_UTF8(ਸ਼ੁਰੂ, len, ਨਿਸ਼ਾਨਾ, dst_ਮਾਪ <= 0x7fffffff ? static_cast<int32_t>(dst_ਮਾਪ) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#ਜੇ_ਅੰਤ

ਖਾਲੀ 
UnicodeString::ਕੱਢੋ_Between(int32_t ਸ਼ੁਰੂ,
                  int32_t ਹੱਦਬੰਦੀ,
                  UnicodeString& ਨਿਸ਼ਾਨਾ) ਅਚਲ {
  pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ);
  pin_ਸੂਚਕਾਂਕ(ਹੱਦਬੰਦੀ);
  do_ਕੱਢੋ(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ, ਨਿਸ਼ਾਨਾ);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
ਖਾਲੀ
UnicodeString::ਮੰਜ਼ਿਲ_UTF8(ByteSink &sink) ਅਚਲ {
  int32_t length16 = length();
  ਜੇ(length16 != 0) {
    ਅੱਖਰ ਢੇਰ_ਬਫਰ[1024];
    int32_t capacity = static_cast<int32_t>(ਆਕਾਰ(ਢੇਰ_ਬਫਰ));
    UBool utf8IsOwned = ਝੂਠ;
    ਅੱਖਰ *utf8 = sink.Get_Append_ਬਫਰ(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ਢੇਰ_ਬਫਰ, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
    u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(utf8, capacity, &length8,
                       get_ਬਫਰ(), length16,
                       0xFFFD,  // Standard substitution character.
                       ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
                       &error_ਕੋਡ);
    ਜੇ(error_ਕੋਡ == U_ਬਫਰ_ਉਛਾਲ_ERROR) {
      utf8 = static_cast<ਅੱਖਰ*>(uprv_malloc(length8));
      ਜੇ(utf8 != ਖਾਲੀ_ਸੂਚਕ) {
        utf8IsOwned = ਸੱਚ;
        error_ਕੋਡ = U_ZERO_ERROR;
        u_str_ਮੰਜ਼ਿਲ_UTF8_With_Sub(utf8, length8, &length8,
                           get_ਬਫਰ(), length16,
                           0xFFFD,  // Standard substitution character.
                           ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
                           &error_ਕੋਡ);
      } ਨਹੀਂਤਾਂ {
        error_ਕੋਡ = U_ਸਮ੍ਰਿਤੀ_ਵੰਡ_ERROR;
      }
    }
    ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    ਜੇ(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::ਮੰਜ਼ਿਲ_UTF32(UChar32 *utf32, int32_t capacity, UError_ਕੋਡ &error_ਕੋਡ) ਅਚਲ {
  int32_t length32=0;
  ਜੇ(U_SUCCESS(error_ਕੋਡ)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_ਮੰਜ਼ਿਲ_UTF32_With_Sub(utf32, capacity, &length32,
        get_ਬਫਰ(), length(),
        0xfffd,  // Substitution character.
        ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
        &error_ਕੋਡ);
  }
  ਵਾਪਸ length32;
}

int32_t 
UnicodeString::ਸੂਚਕਾਂਕ_ਦਾ(ਅਚਲ char16_t *srcChars,
               int32_t src_ਸ਼ੁਰੂ,
               int32_t src_ਲੰਮਾਈ,
               int32_t ਸ਼ੁਰੂ,
               int32_t length) ਅਚਲ
{
  ਜੇ (isBogus() || srcChars == ਖਾਲੀ_ਸੂਚਕ || src_ਸ਼ੁਰੂ < 0 || src_ਲੰਮਾਈ == 0) {
    ਵਾਪਸ -1;
  }

  // UnicodeString does not find empty substrings
  ਜੇ(src_ਲੰਮਾਈ < 0 && srcChars[src_ਸ਼ੁਰੂ] == 0) {
    ਵਾਪਸ -1;
  }

  // get the indices within bounds
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the first occurrence of the substring
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_str_ਲੱਭੋ_ਪਹਿਲਾ(array + ਸ਼ੁਰੂ, length, srcChars + src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

int32_t
UnicodeString::do_ਸੂਚਕਾਂਕ_ਦਾ(char16_t c,
             int32_t ਸ਼ੁਰੂ,
             int32_t length) ਅਚਲ
{
  // pin indices
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the first occurrence of c
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_memchr(array + ਸ਼ੁਰੂ, c, length);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

int32_t
UnicodeString::do_ਸੂਚਕਾਂਕ_ਦਾ(UChar32 c,
                         int32_t ਸ਼ੁਰੂ,
                         int32_t length) ਅਚਲ {
  // pin indices
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the first occurrence of c
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_memchr32(array + ਸ਼ੁਰੂ, c, length);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

int32_t 
UnicodeString::ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(ਅਚਲ char16_t *srcChars,
               int32_t src_ਸ਼ੁਰੂ,
               int32_t src_ਲੰਮਾਈ,
               int32_t ਸ਼ੁਰੂ,
               int32_t length) ਅਚਲ
{
  ਜੇ (isBogus() || srcChars == ਖਾਲੀ_ਸੂਚਕ || src_ਸ਼ੁਰੂ < 0 || src_ਲੰਮਾਈ == 0) {
    ਵਾਪਸ -1;
  }

  // UnicodeString does not find empty substrings
  ਜੇ(src_ਲੰਮਾਈ < 0 && srcChars[src_ਸ਼ੁਰੂ] == 0) {
    ਵਾਪਸ -1;
  }

  // get the indices within bounds
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the last occurrence of the substring
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_str_ਲੱਭੋ_ਆਖਰੀ(array + ਸ਼ੁਰੂ, length, srcChars + src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

int32_t
UnicodeString::do_ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(char16_t c,
                 int32_t ਸ਼ੁਰੂ,
                 int32_t length) ਅਚਲ
{
  ਜੇ(isBogus()) {
    ਵਾਪਸ -1;
  }

  // pin indices
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the last occurrence of c
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_memrchr(array + ਸ਼ੁਰੂ, c, length);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

int32_t
UnicodeString::do_ਆਖਰੀ_ਸੂਚਕਾਂਕ_ਦਾ(UChar32 c,
                             int32_t ਸ਼ੁਰੂ,
                             int32_t length) ਅਚਲ {
  // pin indices
  pinIndices(ਸ਼ੁਰੂ, length);

  // find the last occurrence of c
  ਅਚਲ char16_t *array = get_Array_ਸ਼ੁਰੂ();
  ਅਚਲ char16_t *ਮੇਲ = u_memrchr32(array + ਸ਼ੁਰੂ, c, length);
  ਜੇ(ਮੇਲ == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ -1;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ static_cast<int32_t>(ਮੇਲ - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::ਲੱਭੋ_And_Replace(int32_t ਸ਼ੁਰੂ,
                              int32_t length,
                              ਅਚਲ UnicodeString& old_ਪਾਠ,
                              int32_t old_ਸ਼ੁਰੂ,
                              int32_t old_ਲੰਮਾਈ,
                              ਅਚਲ UnicodeString& new_ਪਾਠ,
                              int32_t new_ਸ਼ੁਰੂ,
                              int32_t new_ਲੰਮਾਈ) U_LIFETIME_BOUND
{
  ਜੇ(isBogus() || old_ਪਾਠ.isBogus() || new_ਪਾਠ.isBogus()) {
    ਵਾਪਸ *ਇਹ;
  }

  pinIndices(ਸ਼ੁਰੂ, length);
  old_ਪਾਠ.pinIndices(old_ਸ਼ੁਰੂ, old_ਲੰਮਾਈ);
  new_ਪਾਠ.pinIndices(new_ਸ਼ੁਰੂ, new_ਲੰਮਾਈ);

  ਜੇ(old_ਲੰਮਾਈ == 0) {
    ਵਾਪਸ *ਇਹ;
  }

  ਜਦੋਂ(length > 0 && length >= old_ਲੰਮਾਈ) {
    int32_t pos = ਸੂਚਕਾਂਕ_ਦਾ(old_ਪਾਠ, old_ਸ਼ੁਰੂ, old_ਲੰਮਾਈ, ਸ਼ੁਰੂ, length);
    ਜੇ(pos < 0) {
      // no more oldText's here: done
      ਰੋਕੋ;
    } ਨਹੀਂਤਾਂ {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_ਲੰਮਾਈ, new_ਪਾਠ, new_ਸ਼ੁਰੂ, new_ਲੰਮਾਈ);
      length -= pos + old_ਲੰਮਾਈ - ਸ਼ੁਰੂ;
      ਸ਼ੁਰੂ = pos + new_ਲੰਮਾਈ;
    }
  }

  ਵਾਪਸ *ਇਹ;
}


ਖਾਲੀ
UnicodeString::ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus()
{
  releaseArray();

  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kIsBogus;
  fUnion.fFields.fArray = ਖਾਲੀ_ਸੂਚਕ;
  fUnion.fFields.f_ਸਮਰੱਥਾ = 0;
}

// turn a bogus string into an empty one
ਖਾਲੀ
UnicodeString::unBogus() {
  ਜੇ(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & kIsBogus) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
  }
}

ਅਚਲ char16_t*
UnicodeString::get_Terminated_ਬਫਰ() U_LIFETIME_BOUND {
  ਜੇ(!isWritable()) {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  char16_t *array = get_Array_ਸ਼ੁਰੂ();
  int32_t len = length();
  ਜੇ(len < get_ਸਮਰੱਥਾ()) {
    ਜੇ(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & k_ਬਫਰ_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      ਜੇ(array[len] == 0) {
        ਵਾਪਸ array;
      }
    } ਨਹੀਂਤਾਂ ਜੇ(((fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & kRefCounted) == 0 || ref_ਗਿਣਤੀ() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ਵਾਪਸ array;
    }
  }
  ਜੇ(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = get_Array_ਸ਼ੁਰੂ();
    array[len] = 0;
    ਵਾਪਸ array;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::ਸੈੱਟ_ਮੰਜ਼ਿਲ(UBool isTerminated,
                     ConstChar16Ptr ਪਾਠ_Ptr U_LIFETIME_BOUND,
                     int32_t ਪਾਠ_ਲੰਮਾਈ) U_LIFETIME_BOUND
{
  ਜੇ(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & k_Open_Get_ਬਫਰ) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ਵਾਪਸ *ਇਹ;
  }

  ਅਚਲ char16_t *ਪਾਠ = ਪਾਠ_Ptr;
  ਜੇ(ਪਾਠ == ਖਾਲੀ_ਸੂਚਕ) {
    // treat as an empty string, do not alias
    releaseArray();
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
    ਵਾਪਸ *ਇਹ;
  }

  ਜੇ( ਪਾਠ_ਲੰਮਾਈ < -1 ||
      (ਪਾਠ_ਲੰਮਾਈ == -1 && !isTerminated) ||
      (ਪਾਠ_ਲੰਮਾਈ >= 0 && isTerminated && ਪਾਠ[ਪਾਠ_ਲੰਮਾਈ] != 0)
  ) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  }

  releaseArray();

  ਜੇ(ਪਾਠ_ਲੰਮਾਈ == -1) {
    // text is terminated, or else it would have failed the above test
    ਪਾਠ_ਲੰਮਾਈ = u_strlen(ਪਾਠ);
  }
  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kReadonlyAlias;
  ਸੈੱਟ_Array(const_cast<char16_t*>(ਪਾਠ), ਪਾਠ_ਲੰਮਾਈ, isTerminated ? ਪਾਠ_ਲੰਮਾਈ + 1 : ਪਾਠ_ਲੰਮਾਈ);
  ਵਾਪਸ *ਇਹ;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::ਸੈੱਟ_ਮੰਜ਼ਿਲ(char16_t* ਬਫਰ,
                     int32_t buff_ਲੰਮਾਈ U_LIFETIME_BOUND,
                     int32_t buff_ਸਮਰੱਥਾ) U_LIFETIME_BOUND {
  ਜੇ(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & k_Open_Get_ਬਫਰ) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ਵਾਪਸ *ਇਹ;
  }

  ਜੇ(ਬਫਰ == ਖਾਲੀ_ਸੂਚਕ) {
    // treat as an empty string, do not alias
    releaseArray();
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_ਖਾਲੀ_ਥਾਵਾਂ();
    ਵਾਪਸ *ਇਹ;
  }

  ਜੇ(buff_ਲੰਮਾਈ < -1 || buff_ਸਮਰੱਥਾ < 0 || buff_ਲੰਮਾਈ > buff_ਸਮਰੱਥਾ) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  } ਨਹੀਂਤਾਂ ਜੇ(buff_ਲੰਮਾਈ == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ਅਚਲ char16_t *p = ਬਫਰ, *ਹੱਦਬੰਦੀ = ਬਫਰ + buff_ਸਮਰੱਥਾ;
    ਜਦੋਂ(p != ਹੱਦਬੰਦੀ && *p != 0) {
      ++p;
    }
    buff_ਲੰਮਾਈ = static_cast<int32_t>(p - ਬਫਰ);
  }

  releaseArray();

  fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = kWritableAlias;
  ਸੈੱਟ_Array(ਬਫਰ, buff_ਲੰਮਾਈ, buff_ਸਮਰੱਥਾ);
  ਵਾਪਸ *ਇਹ;
}

UnicodeString& UnicodeString::ਸੈੱਟ_ਮੰਜ਼ਿਲ_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  ਜੇ(length <= US_STACKBUF_ਮਾਪ) {
    capacity = US_STACKBUF_ਮਾਪ;
  } ਨਹੀਂਤਾਂ {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_ਬਫਰ(capacity);
  int32_t length16;
  UError_ਕੋਡ error_ਕੋਡ = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_ਸਮਰੱਥਾ(), &length16,
      utf8.ਅੰਕੜੇ(), length,
      0xfffd,  // Substitution character.
      ਖਾਲੀ_ਸੂਚਕ,    // Don't care about number of substitutions.
      &error_ਕੋਡ);
  release_ਬਫਰ(length16);
  ਜੇ(U_FAILURE(error_ਕੋਡ)) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
  }
  ਵਾਪਸ *ਇਹ;
}

UnicodeString&
UnicodeString::ਸੈੱਟ_Char_At(int32_t ਵਿੱਥ, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  ਜੇ(cloneArrayIfNeeded() && len > 0) {
    ਜੇ(ਵਿੱਥ < 0) {
      ਵਿੱਥ = 0;
    } ਨਹੀਂਤਾਂ ਜੇ(ਵਿੱਥ >= len) {
      ਵਿੱਥ = len - 1;
    }

    get_Array_ਸ਼ੁਰੂ()[ਵਿੱਥ] = c;
  }
  ਵਾਪਸ *ਇਹ;
}

UnicodeString&
UnicodeString::replace(int32_t ਸ਼ੁਰੂ,
                       int32_t ਲੰਮਾਈ_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t ਬਫਰ[U16_MAX_ਲੰਮਾਈ];
  int32_t count = 0;
  UBool isError = ਝੂਠ;
  U16_APPEND(ਬਫਰ, count, U16_MAX_ਲੰਮਾਈ, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ਵਾਪਸ doReplace(ਸ਼ੁਰੂ, ਲੰਮਾਈ_4, ਬਫਰ, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t ਬਫਰ[U16_MAX_ਲੰਮਾਈ];
  int32_t ਲੰਮਾਈ_4 = 0;
  UBool isError = ਝੂਠ;
  U16_APPEND(ਬਫਰ, ਲੰਮਾਈ_4, U16_MAX_ਲੰਮਾਈ, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ਵਾਪਸ isError ? *ਇਹ : doAppend(ਬਫਰ, 0, ਲੰਮਾਈ_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t ਸ਼ੁਰੂ,
                         int32_t length,
                         ਅਚਲ UnicodeString& src,
                         int32_t src_ਸ਼ੁਰੂ,
                         int32_t src_ਲੰਮਾਈ) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);

  // get the characters from src
  // and replace the range in ourselves with them
  ਵਾਪਸ doReplace(ਸ਼ੁਰੂ, length, src.get_Array_ਸ਼ੁਰੂ(), src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
}

UnicodeString&
UnicodeString::doReplace(int32_t ਸ਼ੁਰੂ,
                         int32_t length,
                         ਅਚਲ char16_t* srcChars,
                         int32_t src_ਸ਼ੁਰੂ,
                         int32_t src_ਲੰਮਾਈ) U_LIFETIME_BOUND
{
  ਜੇ(!isWritable()) {
    ਵਾਪਸ *ਇਹ;
  }

  int32_t old_ਲੰਮਾਈ = ਇਹ->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  ਜੇ((fUnion.fFields.f_ਲੰਮਾਈ_And_Flags&k_ਬਫਰ_Is_Readonly) && src_ਲੰਮਾਈ == 0) {
    ਜੇ(ਸ਼ੁਰੂ == 0) {
      // remove prefix by adjusting the array pointer
      pin_ਸੂਚਕਾਂਕ(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_ਸਮਰੱਥਾ -= length;
      ਸੈੱਟ_ਲੰਮਾਈ(old_ਲੰਮਾਈ - length);
      ਵਾਪਸ *ਇਹ;
    } ਨਹੀਂਤਾਂ {
      pin_ਸੂਚਕਾਂਕ(ਸ਼ੁਰੂ);
      ਜੇ(length >= (old_ਲੰਮਾਈ - ਸ਼ੁਰੂ)) {
        // remove suffix by reducing the length (like truncate())
        ਸੈੱਟ_ਲੰਮਾਈ(ਸ਼ੁਰੂ);
        fUnion.fFields.f_ਸਮਰੱਥਾ = ਸ਼ੁਰੂ;  // not NUL-terminated any more
        ਵਾਪਸ *ਇਹ;
      }
    }
  }

  ਜੇ(ਸ਼ੁਰੂ == old_ਲੰਮਾਈ) {
    ਵਾਪਸ doAppend(srcChars, src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
  }

  ਜੇ (srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    src_ਲੰਮਾਈ = 0;
  } ਨਹੀਂਤਾਂ {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_ਸ਼ੁਰੂ;
    ਜੇ (src_ਲੰਮਾਈ < 0) {
      // get the srcLength if necessary
      src_ਲੰਮਾਈ = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_ਲੰਮਾਈ = old_ਲੰਮਾਈ - length;
  ਜੇ(src_ਲੰਮਾਈ > (INT32_MAX - new_ਲੰਮਾਈ)) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  }
  new_ਲੰਮਾਈ += src_ਲੰਮਾਈ;

  // Check for insertion into ourself
  ਅਚਲ char16_t *oldArray = get_Array_ਸ਼ੁਰੂ();
  ਜੇ (is_ਬਫਰ_Writable() &&
      oldArray < srcChars + src_ਲੰਮਾਈ &&
      srcChars < oldArray + old_ਲੰਮਾਈ) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_ਲੰਮਾਈ);
    ਜੇ (copy.isBogus()) {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ *ਇਹ;
    }
    ਵਾਪਸ doReplace(ਸ਼ੁਰੂ, length, copy.get_Array_ਸ਼ੁਰੂ(), 0, src_ਲੰਮਾਈ);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ਢੇਰ_ਬਫਰ[US_STACKBUF_ਮਾਪ];
  ਜੇ((fUnion.fFields.f_ਲੰਮਾਈ_And_Flags&k_Using_ਢੇਰ_ਬਫਰ) && (new_ਲੰਮਾਈ > US_STACKBUF_ਮਾਪ)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ਢੇਰ_ਬਫਰ, oldArray, old_ਲੰਮਾਈ);
    oldArray = old_ਢੇਰ_ਬਫਰ;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *ਬਫਰ_ਮੰਜ਼ਿਲ_Delete = ਖਾਲੀ_ਸੂਚਕ;
  ਜੇ(!cloneArrayIfNeeded(new_ਲੰਮਾਈ, get_Grow_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ),
                         ਝੂਠ, &ਬਫਰ_ਮੰਜ਼ਿਲ_Delete)
  ) {
    ਵਾਪਸ *ਇਹ;
  }

  // now do the replace

  char16_t *newArray = get_Array_ਸ਼ੁਰੂ();
  ਜੇ(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, ਸ਼ੁਰੂ);
    us_arrayCopy(oldArray, ਸ਼ੁਰੂ + length,
                 newArray, ਸ਼ੁਰੂ + src_ਲੰਮਾਈ,
                 old_ਲੰਮਾਈ - (ਸ਼ੁਰੂ + length));
  } ਨਹੀਂਤਾਂ ਜੇ(length != src_ਲੰਮਾਈ) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, ਸ਼ੁਰੂ + length,
                 newArray, ਸ਼ੁਰੂ + src_ਲੰਮਾਈ,
                 old_ਲੰਮਾਈ - (ਸ਼ੁਰੂ + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);

  ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  ਜੇ (ਬਫਰ_ਮੰਜ਼ਿਲ_Delete) {
    uprv_free(ਬਫਰ_ਮੰਜ਼ਿਲ_Delete);
  }

  ਵਾਪਸ *ਇਹ;
}

UnicodeString&
UnicodeString::doReplace(int32_t ਸ਼ੁਰੂ, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  ਜੇ (!isWritable()) {
    ਵਾਪਸ *ਇਹ;
  }
  ਜੇ (src.length() > INT32_MAX) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  }
  ਵਾਪਸ doReplace(ਸ਼ੁਰੂ, length, src.ਅੰਕੜੇ(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ਅਚਲ UnicodeString& src,
                        int32_t src_ਸ਼ੁਰੂ,
                        int32_t src_ਲੰਮਾਈ) U_LIFETIME_BOUND {
  ਜੇ(src_ਲੰਮਾਈ == 0) {
    ਵਾਪਸ *ਇਹ;
  }

  // pin the indices to legal values
  src.pinIndices(src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
  ਵਾਪਸ doAppend(src.get_Array_ਸ਼ੁਰੂ(), src_ਸ਼ੁਰੂ, src_ਲੰਮਾਈ);
}

UnicodeString&
UnicodeString::doAppend(ਅਚਲ char16_t* srcChars,
                        int32_t src_ਸ਼ੁਰੂ,
                        int32_t src_ਲੰਮਾਈ) U_LIFETIME_BOUND {
  ਜੇ(!isWritable() || src_ਲੰਮਾਈ == 0 || srcChars == ਖਾਲੀ_ਸੂਚਕ) {
    ਵਾਪਸ *ਇਹ;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_ਸ਼ੁਰੂ;

  ਜੇ(src_ਲੰਮਾਈ < 0) {
    // get the srcLength if necessary
    ਜੇ((src_ਲੰਮਾਈ = u_strlen(srcChars)) == 0) {
      ਵਾਪਸ *ਇਹ;
    }
  }

  int32_t old_ਲੰਮਾਈ = length();
  int32_t new_ਲੰਮਾਈ;

  ਜੇ (src_ਲੰਮਾਈ <= get_ਸਮਰੱਥਾ() - old_ਲੰਮਾਈ && is_ਬਫਰ_Writable()) {
    new_ਲੰਮਾਈ = old_ਲੰਮਾਈ + src_ਲੰਮਾਈ;
    // Faster than a memmove
    ਜੇ (src_ਲੰਮਾਈ <= 4) {
      char16_t *arr = get_Array_ਸ਼ੁਰੂ();
      arr[old_ਲੰਮਾਈ] = srcChars[0];
      ਜੇ (src_ਲੰਮਾਈ > 1) arr[old_ਲੰਮਾਈ+1] = srcChars[1];
      ਜੇ (src_ਲੰਮਾਈ > 2) arr[old_ਲੰਮਾਈ+2] = srcChars[2];
      ਜੇ (src_ਲੰਮਾਈ > 3) arr[old_ਲੰਮਾਈ+3] = srcChars[3];
      ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);
      ਵਾਪਸ *ਇਹ;
    }
  } ਨਹੀਂਤਾਂ {
    ਜੇ (uprv_add32_ਉਛਾਲ(old_ਲੰਮਾਈ, src_ਲੰਮਾਈ, &new_ਲੰਮਾਈ)) {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ *ਇਹ;
    }

    // Check for append onto ourself
    ਅਚਲ char16_t* oldArray = get_Array_ਸ਼ੁਰੂ();
    ਜੇ (is_ਬਫਰ_Writable() &&
        oldArray < srcChars + src_ਲੰਮਾਈ &&
        srcChars < oldArray + old_ਲੰਮਾਈ) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_ਲੰਮਾਈ);
      ਜੇ (copy.isBogus()) {
        ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
        ਵਾਪਸ *ਇਹ;
      }
      ਵਾਪਸ doAppend(copy.get_Array_ਸ਼ੁਰੂ(), 0, src_ਲੰਮਾਈ);
    }

    // optimize append() onto a large-enough, owned string
    ਜੇ (!cloneArrayIfNeeded(new_ਲੰਮਾਈ, get_Grow_ਸਮਰੱਥਾ(new_ਲੰਮਾਈ))) {
      ਵਾਪਸ *ਇਹ;
    }
  }

  char16_t *newArray = get_Array_ਸ਼ੁਰੂ();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  ਜੇ(srcChars != newArray + old_ਲੰਮਾਈ) {
    us_arrayCopy(srcChars, 0, newArray, old_ਲੰਮਾਈ, src_ਲੰਮਾਈ);
  }
  ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);

  ਵਾਪਸ *ਇਹ;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  ਜੇ (!isWritable() || src.empty()) {
    ਵਾਪਸ *ਇਹ;
  }
  ਜੇ (src.length() > INT32_MAX) {
    ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
    ਵਾਪਸ *ਇਹ;
  }
  ਵਾਪਸ doAppend(src.ਅੰਕੜੇ(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
ਖਾਲੀ
UnicodeString::handleReplaceBetween(int32_t ਸ਼ੁਰੂ,
                                    int32_t ਹੱਦਬੰਦੀ,
                                    ਅਚਲ UnicodeString& ਪਾਠ) {
    replaceBetween(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਪਾਠ);
}

/**
 * Replaceable API
 */
ਖਾਲੀ 
UnicodeString::copy(int32_t ਸ਼ੁਰੂ, int32_t ਹੱਦਬੰਦੀ, int32_t dest) {
    ਜੇ (ਹੱਦਬੰਦੀ <= ਸ਼ੁਰੂ) {
        ਵਾਪਸ; // Nothing to do; avoid bogus malloc call
    }
    char16_t* ਪਾਠ = static_cast<char16_t*>(uprv_malloc(ਆਕਾਰ(char16_t) * (ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ)));
    // Check to make sure text is not null.
    ਜੇ (ਪਾਠ != ਖਾਲੀ_ਸੂਚਕ) {
	    ਕੱਢੋ_Between(ਸ਼ੁਰੂ, ਹੱਦਬੰਦੀ, ਪਾਠ, 0);
	    insert(dest, ਪਾਠ, 0, ਹੱਦਬੰਦੀ - ਸ਼ੁਰੂ);    
	    uprv_free(ਪਾਠ);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_ਅੰਕੜੇ() ਅਚਲ {
    ਵਾਪਸ ਸੱਚ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_ਅੰਕੜੇ() ਅਚਲ {
    ਵਾਪਸ ਝੂਠ;
}

UnicodeString&
UnicodeString::do_ਉਲਟ(int32_t ਸ਼ੁਰੂ, int32_t length) U_LIFETIME_BOUND {
  ਜੇ(length <= 1 || !cloneArrayIfNeeded()) {
    ਵਾਪਸ *ਇਹ;
  }

  // pin the indices to legal values
  pinIndices(ਸ਼ੁਰੂ, length);
  ਜੇ(length <= 1) {  // pinIndices() might have shrunk the length
    ਵਾਪਸ *ਇਹ;
  }

  char16_t *ਖੱਬੇ = get_Array_ਸ਼ੁਰੂ() + ਸ਼ੁਰੂ;
  char16_t *ਸੱਜੇ = ਖੱਬੇ + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = ਝੂਠ;

  // Before the loop we know left<right because length>=2.
  ਕਰੋ {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *ਖੱਬੇ));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ਖੱਬੇ++ = *ਸੱਜੇ));
    *ਸੱਜੇ-- = swap;
  } ਜਦੋਂ(ਖੱਬੇ < ਸੱਜੇ);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*ਖੱਬੇ));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  ਜੇ(hasSupplementary) {
    char16_t swap2;

    ਖੱਬੇ = get_Array_ਸ਼ੁਰੂ() + ਸ਼ੁਰੂ;
    ਸੱਜੇ = ਖੱਬੇ + length - 1; // -1 so that we can look at *(left+1) if left<right
    ਜਦੋਂ(ਖੱਬੇ < ਸੱਜੇ) {
      ਜੇ(U16_IS_TRAIL(swap = *ਖੱਬੇ) && U16_IS_LEAD(swap2 = *(ਖੱਬੇ + 1))) {
        *ਖੱਬੇ++ = swap2;
        *ਖੱਬੇ++ = swap;
      } ਨਹੀਂਤਾਂ {
        ++ਖੱਬੇ;
      }
    }
  }

  ਵਾਪਸ *ਇਹ;
}

UBool 
UnicodeString::padLeading(int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ,
                          char16_t padChar)
{
  int32_t old_ਲੰਮਾਈ = length();
  ਜੇ(old_ਲੰਮਾਈ >= ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ || !cloneArrayIfNeeded(ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ)) {
    ਵਾਪਸ ਝੂਠ;
  } ਨਹੀਂਤਾਂ {
    // move contents up by padding width
    char16_t *array = get_Array_ਸ਼ੁਰੂ();
    int32_t ਸ਼ੁਰੂ = ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ - old_ਲੰਮਾਈ;
    us_arrayCopy(array, 0, array, ਸ਼ੁਰੂ, old_ਲੰਮਾਈ);

    // fill in padding character
    ਜਦੋਂ(--ਸ਼ੁਰੂ >= 0) {
      array[ਸ਼ੁਰੂ] = padChar;
    }
    ਸੈੱਟ_ਲੰਮਾਈ(ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ);
    ਵਾਪਸ ਸੱਚ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ,
                           char16_t padChar)
{
  int32_t old_ਲੰਮਾਈ = length();
  ਜੇ(old_ਲੰਮਾਈ >= ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ || !cloneArrayIfNeeded(ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ)) {
    ਵਾਪਸ ਝੂਠ;
  } ਨਹੀਂਤਾਂ {
    // fill in padding character
    char16_t *array = get_Array_ਸ਼ੁਰੂ();
    int32_t length = ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ;
    ਜਦੋਂ(--length >= old_ਲੰਮਾਈ) {
      array[length] = padChar;
    }
    ਸੈੱਟ_ਲੰਮਾਈ(ਨਿਸ਼ਾਨਾ_ਲੰਮਾਈ);
    ਵਾਪਸ ਸੱਚ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_ਕੋਡ() ਅਚਲ
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_ਕੋਡ = ustr_hashUCharsN(get_Array_ਸ਼ੁਰੂ(), length());
    ਜੇ (hash_ਕੋਡ == k_Invalid_Hash_ਕੋਡ) {
        hash_ਕੋਡ = k_ਖਾਲੀ_ਥਾਵਾਂ_Hash_ਕੋਡ;
    }
    ਵਾਪਸ hash_ਕੋਡ;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ) U_LIFETIME_BOUND {
  ਜੇ(min_ਸਮਰੱਥਾ>=-1 && cloneArrayIfNeeded(min_ਸਮਰੱਥਾ)) {
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags|=k_Open_Get_ਬਫਰ;
    ਸੈੱਟ_Zero_ਲੰਮਾਈ();
    ਵਾਪਸ get_Array_ਸ਼ੁਰੂ();
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
}

ਖਾਲੀ
UnicodeString::release_ਬਫਰ(int32_t new_ਲੰਮਾਈ) {
  ਜੇ(fUnion.fFields.f_ਲੰਮਾਈ_And_Flags&k_Open_Get_ਬਫਰ && new_ਲੰਮਾਈ>=-1) {
    // set the new fLength
    int32_t capacity=get_ਸਮਰੱਥਾ();
    ਜੇ(new_ਲੰਮਾਈ==-1) {
      // the new length is the string length, capped by fCapacity
      ਅਚਲ char16_t *array=get_Array_ਸ਼ੁਰੂ(), *p=array, *ਹੱਦਬੰਦੀ=array+capacity;
      ਜਦੋਂ(p<ਹੱਦਬੰਦੀ && *p!=0) {
        ++p;
      }
      new_ਲੰਮਾਈ = static_cast<int32_t>(p - array);
    } ਨਹੀਂਤਾਂ ਜੇ(new_ਲੰਮਾਈ>capacity) {
      new_ਲੰਮਾਈ=capacity;
    }
    ਸੈੱਟ_ਲੰਮਾਈ(new_ਲੰਮਾਈ);
    fUnion.fFields.f_ਲੰਮਾਈ_And_Flags&=~k_Open_Get_ਬਫਰ;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_ਸਮਰੱਥਾ,
                                  int32_t grow_ਸਮਰੱਥਾ,
                                  UBool doCopyArray,
                                  int32_t **p_ਬਫਰ_ਮੰਜ਼ਿਲ_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  ਜੇ(new_ਸਮਰੱਥਾ == -1) {
    new_ਸਮਰੱਥਾ = get_ਸਮਰੱਥਾ();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  ਜੇ(!isWritable()) {
    ਵਾਪਸ ਝੂਠ;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  ਜੇ(forceClone ||
     fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & k_ਬਫਰ_Is_Readonly ||
     (fUnion.fFields.f_ਲੰਮਾਈ_And_Flags & kRefCounted && ref_ਗਿਣਤੀ() > 1) ||
     new_ਸਮਰੱਥਾ > get_ਸਮਰੱਥਾ()
  ) {
    // check growCapacity for default value and use of the stack buffer
    ਜੇ(grow_ਸਮਰੱਥਾ < 0) {
      grow_ਸਮਰੱਥਾ = new_ਸਮਰੱਥਾ;
    } ਨਹੀਂਤਾਂ ਜੇ(new_ਸਮਰੱਥਾ <= US_STACKBUF_ਮਾਪ && grow_ਸਮਰੱਥਾ > US_STACKBUF_ਮਾਪ) {
      grow_ਸਮਰੱਥਾ = US_STACKBUF_ਮਾਪ;
    } ਨਹੀਂਤਾਂ ਜੇ(new_ਸਮਰੱਥਾ > grow_ਸਮਰੱਥਾ) {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਝੂਠ;  // bad inputs
    }
    ਜੇ(grow_ਸਮਰੱਥਾ > k_Max_ਸਮਰੱਥਾ) {
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਝੂਠ;
    }

    // save old values
    char16_t old_ਢੇਰ_ਬਫਰ[US_STACKBUF_ਮਾਪ];
    char16_t *oldArray;
    int32_t old_ਲੰਮਾਈ = length();
    int16_t flags = fUnion.fFields.f_ਲੰਮਾਈ_And_Flags;

    ਜੇ(flags&k_Using_ਢੇਰ_ਬਫਰ) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      ਜੇ(doCopyArray && grow_ਸਮਰੱਥਾ > US_STACKBUF_ਮਾਪ) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ਢੇਰ_Fields.f_ਬਫਰ, 0, old_ਢੇਰ_ਬਫਰ, 0, old_ਲੰਮਾਈ);
        oldArray = old_ਢੇਰ_ਬਫਰ;
      } ਨਹੀਂਤਾਂ {
        oldArray = ਖਾਲੀ_ਸੂਚਕ; // no need to copy from the stack buffer to itself
      }
    } ਨਹੀਂਤਾਂ {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=ਖਾਲੀ_ਸੂਚਕ); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    ਜੇ(allocate(grow_ਸਮਰੱਥਾ) ||
       (new_ਸਮਰੱਥਾ < grow_ਸਮਰੱਥਾ && allocate(new_ਸਮਰੱਥਾ))
    ) {
      ਜੇ(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_ਲੰਮਾਈ = old_ਲੰਮਾਈ;
        new_ਸਮਰੱਥਾ = get_ਸਮਰੱਥਾ();
        ਜੇ(new_ਸਮਰੱਥਾ < min_ਲੰਮਾਈ) {
          min_ਲੰਮਾਈ = new_ਸਮਰੱਥਾ;
        }
        ਜੇ(oldArray != ਖਾਲੀ_ਸੂਚਕ) {
          us_arrayCopy(oldArray, 0, get_Array_ਸ਼ੁਰੂ(), 0, min_ਲੰਮਾਈ);
        }
        ਸੈੱਟ_ਲੰਮਾਈ(min_ਲੰਮਾਈ);
      } ਨਹੀਂਤਾਂ {
        ਸੈੱਟ_Zero_ਲੰਮਾਈ();
      }

      // release the old array
      ਜੇ(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_ਗਿਣਤੀ = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        ਜੇ(umtx_atomic_dec(p_Ref_ਗਿਣਤੀ) == 0) {
          ਜੇ (p_ਬਫਰ_ਮੰਜ਼ਿਲ_Delete == ਖਾਲੀ_ਸੂਚਕ) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((ਖਾਲੀ *)p_Ref_ਗਿਣਤੀ);
          } ਨਹੀਂਤਾਂ {
            // the caller requested to delete it himself
            *p_ਬਫਰ_ਮੰਜ਼ਿਲ_Delete = reinterpret_cast<int32_t*>(p_Ref_ਗਿਣਤੀ);
          }
        }
      }
    } ਨਹੀਂਤਾਂ {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      ਜੇ(!(flags&k_Using_ਢੇਰ_ਬਫਰ)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_ਲੰਮਾਈ_And_Flags = flags;
      ਸੈੱਟ_ਮੰਜ਼ਿਲ_Bogus();
      ਵਾਪਸ ਝੂਠ;
    }
  }
  ਵਾਪਸ ਸੱਚ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_ਕੋਡ_Unit(char16_t c) {
  ਵਾਪਸ str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_ਕੋਡ_ਬਿੰਦੂ(UChar32 c) {
  char16_t ਬਫਰ[U16_MAX_ਲੰਮਾਈ];
  int32_t c_ਲੰਮਾਈ = 0;
  UBool isError = ਝੂਠ;
  U16_APPEND(ਬਫਰ, c_ਲੰਮਾਈ, U16_MAX_ਲੰਮਾਈ, c, isError);
  ਵਾਪਸ !isError && str.doAppend(ਬਫਰ, 0, c_ਲੰਮਾਈ).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ਅਚਲ char16_t *s, int32_t length) {
  ਵਾਪਸ str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_ਸਮਰੱਥਾ(int32_t append_ਸਮਰੱਥਾ) {
  ਵਾਪਸ str.cloneArrayIfNeeded(str.length() + append_ਸਮਰੱਥਾ);
}

char16_t* UnicodeStringAppendable::get_Append_ਬਫਰ(int32_t min_ਸਮਰੱਥਾ,
                                                   int32_t desired_ਸਮਰੱਥਾ_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_ਸਮਰੱਥਾ,
                                                   int32_t* ਨਤੀਜਾ_ਸਮਰੱਥਾ) U_LIFETIME_BOUND {
  ਜੇ(min_ਸਮਰੱਥਾ < 1 || scratch_ਸਮਰੱਥਾ < min_ਸਮਰੱਥਾ) {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ = 0;
    ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
  }
  int32_t old_ਲੰਮਾਈ = str.length();
  ਜੇ(min_ਸਮਰੱਥਾ <= (k_Max_ਸਮਰੱਥਾ - old_ਲੰਮਾਈ) &&
      desired_ਸਮਰੱਥਾ_Hint <= (k_Max_ਸਮਰੱਥਾ - old_ਲੰਮਾਈ) &&
      str.cloneArrayIfNeeded(old_ਲੰਮਾਈ + min_ਸਮਰੱਥਾ, old_ਲੰਮਾਈ + desired_ਸਮਰੱਥਾ_Hint)) {
    *ਨਤੀਜਾ_ਸਮਰੱਥਾ = str.get_ਸਮਰੱਥਾ() - old_ਲੰਮਾਈ;
    ਵਾਪਸ str.get_Array_ਸ਼ੁਰੂ() + old_ਲੰਮਾਈ;
  }
  *ਨਤੀਜਾ_ਸਮਰੱਥਾ = scratch_ਸਮਰੱਥਾ;
  ਵਾਪਸ scratch;
}

U_NAMESPACE_ਸਿਰਾ

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ਅਚਲ UElement ਕੁੰਜੀ) {
    ਅਚਲ UnicodeString *str = (ਅਚਲ UnicodeString*) ਕੁੰਜੀ.ਸੂਚਕ;
    ਵਾਪਸ (str == ਖਾਲੀ_ਸੂਚਕ) ? 0 : str->hash_ਕੋਡ();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ਅਚਲ UElement key1, ਅਚਲ UElement key2) {
    ਅਚਲ UnicodeString *str1 = (ਅਚਲ UnicodeString*) key1.ਸੂਚਕ;
    ਅਚਲ UnicodeString *str2 = (ਅਚਲ UnicodeString*) key2.ਸੂਚਕ;
    ਜੇ (str1 == str2) {
        ਵਾਪਸ ਸੱਚ;
    }
    ਜੇ (str1 == ਖਾਲੀ_ਸੂਚਕ || str2 == ਖਾਲੀ_ਸੂਚਕ) {
        ਵਾਪਸ ਝੂਠ;
    }
    ਵਾਪਸ *str1 == *str2;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#ਜੇ defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#ਹਦਾਇਤ GCC diagnostic push
#ਹਦਾਇਤ GCC diagnostic ignored "-Wunused-function"
ਸਥਿਰ ਖਾਲੀ uprv_UnicodeStringDummy() {
    ਮਿਟਾਓ [] (ਨਵਾਂ UnicodeString[2]);
}
#ਹਦਾਇਤ GCC diagnostic pop
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
