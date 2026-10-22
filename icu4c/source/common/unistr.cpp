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

#شامل <string_view>

#شامل "unicode/utypes.h"
#شامل "unicode/appendable.h"
#شامل "unicode/putil.h"
#شامل "cstring.h"
#شامل "cmemory.h"
#شامل "unicode/ustring.h"
#شامل "unicode/unistr.h"
#شامل "unicode/utf.h"
#شامل "unicode/utf16.h"
#شامل "uelement.h"
#شامل "ustr_imp.h"
#شامل "umutex.h"
#شامل "uassert.h"

#اگر 0

#شامل <iostream>
استعمال نامخلا std;

//DEBUGGING
خالی
print(ثابت UnicodeString& s,
      ثابت حرف *name)
{
  char16_t c;
  خارج << name << ":|";
  برائے(صحیح i = 0; i < s.length(); ++i) {
    c = s[i];
    اگر(c>= 0x007E || c < 0x0020)
      خارج << "[0x" << hex << s[i] << "]";
    ورنہ
      خارج << (حرف) s[i];
  }
  خارج << '|' << سطر_ختم;
}

خالی
print(ثابت char16_t *s,
      int32_t len,
      ثابت حرف *name)
{
  char16_t c;
  خارج << name << ":|";
  برائے(صحیح i = 0; i < len; ++i) {
    c = s[i];
    اگر(c>= 0x007E || c < 0x0020)
      خارج << "[0x" << hex << s[i] << "]";
    ورنہ
      خارج << (حرف) s[i];
  }
  خارج << '|' << سطر_ختم;
}
// END DEBUGGING
#اگر_ختم

// Local function definitions for now

// need to copy areas that may overlap
ساکن
اندرونی خالی
us_arrayCopy(ثابت char16_t *src, int32_t src_آغاز,
         char16_t *dst, int32_t dst_آغاز, int32_t count)
{
  اگر(count>0) {
    uprv_memmove(dst+dst_آغاز, src+src_آغاز, (size_t)count*حجم(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ساکن char16_t U_CALLCONV
UnicodeString_charAt(int32_t انحراف, خالی *context) {
    واپس ((icu::UnicodeString*) context)->charAt(انحراف);
}
U_CDECL_انتہا

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
عامل+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t مجموع_Lengths;
  اگر (uprv_add32_حد_سے_تجاوز(s1.length(), s2.length(), &مجموع_Lengths)) {
    UnicodeString bogus;
    bogus.سیٹ_مقصد_Bogus();
    واپس bogus;
  }
  اگر (مجموع_Lengths != INT32_MAX) {
    ++مجموع_Lengths;  // space for a terminating NUL if we need one
  }
  واپس UnicodeString(مجموع_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_اندرونی_Concat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t مجموع_Lengths;
  اگر (s2.length() > INT32_MAX ||
      uprv_add32_حد_سے_تجاوز(s1.length(), static_cast<int32_t>(s2.length()), &مجموع_Lengths)) {
    UnicodeString bogus;
    bogus.سیٹ_مقصد_Bogus();
    واپس bogus;
  }
  اگر (مجموع_Lengths != INT32_MAX) {
    ++مجموع_Lengths;  // space for a terminating NUL if we need one
  }
  واپس UnicodeString(مجموع_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

خالی
UnicodeString::جوڑو_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  واپس umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_گنتی() ثابت {
  واپس umtx_بھرو_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

خالی
UnicodeString::releaseArray() {
  اگر((fUnion.fFields.f_لمبائی_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_لمبائی_And_Flags = 0;
  اگر (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } ورنہ اگر(c <= 0xffff) {
    int32_t length = count;
    اگر(capacity < length) {
      capacity = length;
    }
    اگر(allocate(capacity)) {
      char16_t *array = get_Array_آغاز();
      char16_t unit = static_cast<char16_t>(c);
      برائے(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      سیٹ_لمبائی(length);
    }
  } ورنہ {  // supplementary code point, write surrogate pairs
    اگر(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      واپس;
    }
    int32_t length = count * 2;
    اگر(capacity < length) {
      capacity = length;
    }
    اگر(allocate(capacity)) {
      char16_t *array = get_Array_آغاز();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      برائے(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      سیٹ_لمبائی(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_لمبائی_And_Flags = kLength1 | kShortString;
  fUnion.f_ڈھیر_Fields.f_بفر[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = جھوٹ;
  U16_APPEND(fUnion.f_ڈھیر_Fields.f_بفر, i, US_STACKBUF_سائز, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  اگر(!isError) {
    سیٹ_Short_لمبائی(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *متن,
                             int32_t متن_لمبائی) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  doAppend(متن, 0, متن_لمبائی);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                             int32_t متن_لمبائی) {
  fUnion.fFields.f_لمبائی_And_Flags = kReadonlyAlias;
  ثابت char16_t *متن = متن_Ptr;
  اگر(متن == خالی_اشاریہ) {
    // treat as an empty string, do not alias
    سیٹ_مقصد_خالی_خانے();
  } ورنہ اگر(متن_لمبائی < -1 ||
            (متن_لمبائی == -1 && !isTerminated) ||
            (متن_لمبائی >= 0 && isTerminated && متن[متن_لمبائی] != 0)
  ) {
    سیٹ_مقصد_Bogus();
  } ورنہ {
    اگر(متن_لمبائی == -1) {
      // text is terminated, or else it would have failed the above test
      متن_لمبائی = u_strlen(متن);
    }
    سیٹ_Array(const_cast<char16_t *>(متن), متن_لمبائی,
             isTerminated ? متن_لمبائی + 1 : متن_لمبائی);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_لمبائی,
                             int32_t buff_استعداد) {
  fUnion.fFields.f_لمبائی_And_Flags = kWritableAlias;
  اگر(buff == خالی_اشاریہ) {
    // treat as an empty string, do not alias
    سیٹ_مقصد_خالی_خانے();
  } ورنہ اگر(buff_لمبائی < -1 || buff_استعداد < 0 || buff_لمبائی > buff_استعداد) {
    سیٹ_مقصد_Bogus();
  } ورنہ {
    اگر(buff_لمبائی == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *حد_بندی = buff + buff_استعداد;
      جب(p != حد_بندی && *p != 0) {
        ++p;
      }
      buff_لمبائی = static_cast<int32_t>(p - buff);
    }
    سیٹ_Array(buff, buff_لمبائی, buff_استعداد);
  }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t length, EInvariant) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  اگر(src==خالی_اشاریہ) {
    // treat as an empty string
  } ورنہ {
    اگر(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    اگر(cloneArrayIfNeeded(length, length, جھوٹ)) {
      u_chars_مقصد_UChars(src, get_Array_آغاز(), length);
      سیٹ_لمبائی(length);
    } ورنہ {
      سیٹ_مقصد_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND) {
  UnicodeString نتیجہ;
  اگر (متن.length() <= INT32_MAX) {
    نتیجہ.سیٹ_مقصد(جھوٹ, متن.ڈیٹا(), static_cast<int32_t>(متن.length()));
  } ورنہ {
    نتیجہ.سیٹ_مقصد_Bogus();
  }
  واپس نتیجہ;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
  UnicodeString نتیجہ;
  اگر (متن.isBogus()) {
    نتیجہ.سیٹ_مقصد_Bogus();
  } ورنہ {
    نتیجہ.سیٹ_مقصد(جھوٹ, متن.get_بفر(), متن.length());
  }
  واپس نتیجہ;
}

#اگر U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  اگر (codepage_ڈیٹا != خالی_اشاریہ) {
    سیٹ_مقصد_UTF8(codepage_ڈیٹا);
  }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_ڈیٹا, int32_t ڈیٹا_لمبائی) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  اگر (codepage_ڈیٹا == خالی_اشاریہ || ڈیٹا_لمبائی == 0 || ڈیٹا_لمبائی < -1) {
    واپس;
  }
  اگر(ڈیٹا_لمبائی == -1) {
    ڈیٹا_لمبائی = static_cast<int32_t>(uprv_strlen(codepage_ڈیٹا));
  }
  سیٹ_مقصد_UTF8(StringPiece(codepage_ڈیٹا, ڈیٹا_لمبائی));
}

// else see unistr_cnv.cpp
#اگر_ختم

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, سچ);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_آغاز) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  سیٹ_مقصد(that, src_آغاز);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_آغاز,
                             int32_t src_لمبائی) {
  fUnion.fFields.f_لمبائی_And_Flags = kShortString;
  سیٹ_مقصد(that, src_آغاز, src_لمبائی);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  واپس خالی_اشاریہ;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  Local_اشارہ_گر<UnicodeString> clonedString(نیا UnicodeString(*یہ));
  واپس clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : خالی_اشاریہ;
}

//========================================
// array allocation
//========================================

نامخلا {

ثابت int32_t k_Grow_سائز = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ثابت int32_t k_Max_استعداد = 0x7ffffff5;

int32_t get_Grow_استعداد(int32_t new_لمبائی) {
  int32_t grow_سائز = (new_لمبائی >> 2) + k_Grow_سائز;
  اگر(grow_سائز <= (k_Max_استعداد - new_لمبائی)) {
    واپس new_لمبائی + grow_سائز;
  } ورنہ {
    واپس k_Max_استعداد;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  اگر(capacity <= US_STACKBUF_سائز) {
    fUnion.fFields.f_لمبائی_And_Flags = kShortString;
    واپس سچ;
  }
  اگر(capacity <= k_Max_استعداد) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t عدد_بائٹیں = حجم(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    عدد_بائٹیں = (عدد_بائٹیں + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(عدد_بائٹیں));
    اگر(array != خالی_اشاریہ) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      عدد_بائٹیں -= حجم(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_استعداد = static_cast<int32_t>(عدد_بائٹیں / U_SIZEOF_UCHAR);
      fUnion.fFields.f_لمبائی_And_Flags = kLongString;
      واپس سچ;
    }
  }
  fUnion.fFields.f_لمبائی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالی_اشاریہ;
  fUnion.fFields.f_استعداد = 0;
  واپس جھوٹ;
}

//========================================
// Destructor
//========================================

#اگر_تعریف_شدہ UNISTR_گنتی_FINAL_STRING_LENGTHS
ساکن u_atomic_int32_t final_لمبائی_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ساکن u_atomic_int32_t beyond_گنتی(0);

U_CAPI خالی unistr_printLengths() {
  int32_t i;
  برائے(i = 0; i <= 59; ++i) {
    لکھو("%2d,  %9d\n", i, (int32_t)final_لمبائی_Counts[i]);
  }
  int32_t beyond = beyond_گنتی;
  برائے(; i < UPRV_LENGTHOF(final_لمبائی_Counts); ++i) {
    beyond += final_لمبائی_Counts[i];
  }
  لکھو(">59, %9d\n", beyond);
}
#اگر_ختم

UnicodeString::~UnicodeString()
{
#اگر_تعریف_شدہ UNISTR_گنتی_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  اگر((fUnion.fFields.f_لمبائی_And_Flags&(k_Open_Get_بفر|kReadonlyAlias|kWritableAlias)) == 0) {
    اگر(has_Short_لمبائی()) {
      umtx_atomic_inc(final_لمبائی_Counts + get_Short_لمبائی());
    } ورنہ {
      umtx_atomic_inc(&beyond_گنتی);
    }
  }
#اگر_ختم

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString نتیجہ;
  نتیجہ.سیٹ_مقصد_UTF8(utf8);
  واپس نتیجہ;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString نتیجہ;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  اگر(length <= US_STACKBUF_سائز) {
    capacity = US_STACKBUF_سائز;
  } ورنہ {
    capacity = length + (length >> 4) + 4;
  }
  کرو {
    char16_t *utf16 = نتیجہ.get_بفر(capacity);
    int32_t length16;
    UError_رمز error_رمز = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, نتیجہ.get_استعداد(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        خالی_اشاریہ,    // Don't care about number of substitutions.
        &error_رمز);
    نتیجہ.release_بفر(length16);
    اگر(error_رمز == U_بفر_حد_سے_تجاوز_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      جاری;
    } ورنہ اگر(U_FAILURE(error_رمز)) {
      نتیجہ.سیٹ_مقصد_Bogus();
    }
    توڑو;
  } جب(سچ);
  واپس نتیجہ;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::عامل=(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  واپس copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  واپس copyFrom(src, سچ);
}

UnicodeString&
UnicodeString::copyFrom(ثابت UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  اگر(یہ == &src) {
    واپس *یہ;
  }

  // is the right side bogus?
  اگر(src.isBogus()) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  }

  // delete the current contents
  releaseArray();

  اگر(src.is_خالی_خانے()) {
    // empty string - use the stack buffer
    سیٹ_مقصد_خالی_خانے();
    واپس *یہ;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_لمبائی_And_Flags = src.fUnion.fFields.f_لمبائی_And_Flags;
  انتخاب(src.fUnion.fFields.f_لمبائی_And_Flags & kAllStorageFlags) {
  حالت kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ڈھیر_Fields.f_بفر, src.fUnion.f_ڈھیر_Fields.f_بفر,
                get_Short_لمبائی() * U_SIZEOF_UCHAR);
    توڑو;
  حالت kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).جوڑو_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_استعداد = src.fUnion.fFields.f_استعداد;
    اگر(!has_Short_لمبائی()) {
      fUnion.fFields.f_لمبائی = src.fUnion.fFields.f_لمبائی;
    }
    توڑو;
  حالت kReadonlyAlias:
    اگر(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_استعداد = src.fUnion.fFields.f_استعداد;
      اگر(!has_Short_لمبائی()) {
        fUnion.fFields.f_لمبائی = src.fUnion.fFields.f_لمبائی;
      }
      توڑو;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالت kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_لمبائی = src.length();
    اگر(allocate(src_لمبائی)) {
      u_memcpy(get_Array_آغاز(), src.get_Array_آغاز(), src_لمبائی);
      سیٹ_لمبائی(src_لمبائی);
      توڑو;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  طےشدہ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_لمبائی_And_Flags = kIsBogus;
    fUnion.fFields.fArray = خالی_اشاریہ;
    fUnion.fFields.f_استعداد = 0;
    توڑو;
  }

  واپس *یہ;
}

UnicodeString& UnicodeString::عامل=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, سچ);
  واپس *یہ;
}

// Same as move assignment except without memory management.
خالی UnicodeString::copyFieldsFrom(UnicodeString &src, UBool سیٹ_Src_مقصد_Bogus) noexcept {
  int16_t لمبائی_And_Flags = fUnion.fFields.f_لمبائی_And_Flags = src.fUnion.fFields.f_لمبائی_And_Flags;
  اگر(لمبائی_And_Flags & k_Using_ڈھیر_بفر) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    اگر(یہ != &src) {
      uprv_memcpy(fUnion.f_ڈھیر_Fields.f_بفر, src.fUnion.f_ڈھیر_Fields.f_بفر,
                  get_Short_لمبائی() * U_SIZEOF_UCHAR);
    }
  } ورنہ {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_استعداد = src.fUnion.fFields.f_استعداد;
    اگر(!has_Short_لمبائی()) {
      fUnion.fFields.f_لمبائی = src.fUnion.fFields.f_لمبائی;
    }
    اگر(سیٹ_Src_مقصد_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_لمبائی_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = خالی_اشاریہ;
      src.fUnion.fFields.f_استعداد = 0;
    }
  }
}

خالی UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString عارضی;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  عارضی.copyFieldsFrom(*یہ, جھوٹ);
  یہ->copyFieldsFrom(other, جھوٹ);
  other.copyFieldsFrom(عارضی, جھوٹ);
  // Set temp to an empty string so that other's memory is not released twice.
  عارضی.fUnion.fFields.f_لمبائی_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString نتیجہ(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    اگر (نتیجہ.isBogus()) {
        واپس نتیجہ;
    }
    ثابت char16_t *array = get_بفر();
    int32_t len = length();
    int32_t پچھلا = 0;
    برائے (int32_t i=0;;) {
        اگر (i == len) {
            نتیجہ.append(array, پچھلا, len - پچھلا);
            توڑو;
        }
        اگر (array[i++] == 0x5C /*'\\'*/) {
            نتیجہ.append(array, پچھلا, (i - 1) - پچھلا);
            UChar32 c = unescapeAt(i); // advances i
            اگر (c < 0) {
                نتیجہ.remove(); // return empty string
                توڑو; // invalid escape sequence
            }
            نتیجہ.append(c);
            پچھلا = i;
        }
    }
    واپس نتیجہ;
}

UChar32 UnicodeString::unescapeAt(int32_t &انحراف) ثابت {
    واپس u_unescapeAt(UnicodeString_charAt, &انحراف, length(), (خالی*)یہ);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ثابت char16_t *متن, int32_t len) ثابت {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  واپس uprv_memcmp(get_Array_آغاز(), متن, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t آغاز,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_آغاز,
              int32_t src_لمبائی) ثابت
{
  // compare illegal string values
  اگر(isBogus()) {
    واپس جھوٹ;
  }
  
  // pin indices to legal values
  pinIndices(آغاز, length);

  اگر(srcChars == خالی_اشاریہ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    واپس length == 0 ? سچ : جھوٹ;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_آغاز();

  chars += آغاز;
  srcChars += src_آغاز;

  // get the srcLength if necessary
  اگر(src_لمبائی < 0) {
    src_لمبائی = u_strlen(srcChars + src_آغاز);
  }

  اگر (length != src_لمبائی) {
    واپس جھوٹ;
  }

  اگر(length == 0 || chars == srcChars) {
    واپس سچ;
  }

  واپس u_memcmp(chars, srcChars, src_لمبائی) == 0;
}

int8_t
UnicodeString::doCompare( int32_t آغاز,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_آغاز,
              int32_t src_لمبائی) ثابت
{
  // compare illegal string values
  اگر(isBogus()) {
    واپس -1;
  }
  
  // pin indices to legal values
  pinIndices(آغاز, length);

  اگر(srcChars == خالی_اشاریہ) {
    // treat const char16_t *srcChars==nullptr as an empty string
    واپس length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_آغاز();

  chars += آغاز;
  srcChars += src_آغاز;

  int32_t min_لمبائی;
  int8_t لمبائی_نتیجہ;

  // get the srcLength if necessary
  اگر(src_لمبائی < 0) {
    src_لمبائی = u_strlen(srcChars + src_آغاز);
  }

  // are we comparing different lengths?
  اگر(length != src_لمبائی) {
    اگر(length < src_لمبائی) {
      min_لمبائی = length;
      لمبائی_نتیجہ = -1;
    } ورنہ {
      min_لمبائی = src_لمبائی;
      لمبائی_نتیجہ = 1;
    }
  } ورنہ {
    min_لمبائی = length;
    لمبائی_نتیجہ = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  اگر(min_لمبائی > 0 && chars != srcChars) {
    int32_t نتیجہ;

#   اگر U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      نتیجہ = uprv_memcmp(chars, srcChars, min_لمبائی * حجم(char16_t));
      اگر(نتیجہ != 0) {
        واپس (int8_t)(نتیجہ >> 15 | 1);
      }
#   ورنہ
      // little-endian: compare char16_t units
      کرو {
        نتیجہ = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        اگر(نتیجہ != 0) {
          واپس static_cast<int8_t>(نتیجہ >> 15 | 1);
        }
      } جب(--min_لمبائی > 0);
#   endif
  }
  واپس لمبائی_نتیجہ;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_رمز_نقطہ_ترتیب(int32_t آغاز,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t src_آغاز,
                                       int32_t src_لمبائی) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  اگر(isBogus()) {
    واپس -1;
  }

  // pin indices to legal values
  pinIndices(آغاز, length);

  اگر(srcChars == خالی_اشاریہ) {
    src_آغاز = src_لمبائی = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_آغاز() + آغاز, length, (srcChars!=خالی_اشاریہ)?(srcChars + src_آغاز):خالی_اشاریہ, src_لمبائی, جھوٹ, سچ);
  /* translate the 32-bit result into an 8-bit one */
  اگر(diff!=0) {
    واپس static_cast<int8_t>(diff >> 15 | 1);
  } ورنہ {
    واپس 0;
  }
}

int32_t
UnicodeString::get_لمبائی() ثابت {
    واپس length();
}

char16_t
UnicodeString::getCharAt(int32_t انحراف) ثابت {
  واپس charAt(انحراف);
}

UChar32
UnicodeString::getChar32At(int32_t انحراف) ثابت {
  واپس char32At(انحراف);
}

UChar32
UnicodeString::char32At(int32_t انحراف) ثابت
{
  int32_t len = length();
  اگر (static_cast<uint32_t>(انحراف) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_آغاز();
    UChar32 c;
    U16_GET(array, 0, انحراف, len, c);
    واپس c;
  } ورنہ {
    واپس kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_آغاز(int32_t انحراف) ثابت {
  اگر (static_cast<uint32_t>(انحراف) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = get_Array_آغاز();
    U16_سیٹ_CP_آغاز(array, 0, انحراف);
    واپس انحراف;
  } ورنہ {
    واپس 0;
  }
}

int32_t
UnicodeString::get_Char32_حد_بندی(int32_t انحراف) ثابت {
  int32_t len = length();
  اگر (static_cast<uint32_t>(انحراف) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_آغاز();
    U16_سیٹ_CP_حد_بندی(array, 0, انحراف, len);
    واپس انحراف;
  } ورنہ {
    واپس len;
  }
}

int32_t
UnicodeString::گنتی_Char32(int32_t آغاز, int32_t length) ثابت {
  pinIndices(آغاز, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  واپس u_گنتی_Char32(get_Array_آغاز()+آغاز, length);
}

UBool
UnicodeString::has_مزید_Char32_Than(int32_t آغاز, int32_t length, int32_t number) ثابت {
  pinIndices(آغاز, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  واپس u_str_Has_مزید_Char32_Than(get_Array_آغاز()+آغاز, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t اشاریہ, int32_t delta) ثابت {
  // pin index
  int32_t len = length();
  اگر(اشاریہ<0) {
    اشاریہ=0;
  } ورنہ اگر(اشاریہ>len) {
    اشاریہ=len;
  }

  ثابت char16_t *array = get_Array_آغاز();
  اگر(delta>0) {
    U16_FWD_N(array, اشاریہ, len, delta);
  } ورنہ {
    U16_عقب_N(array, 0, اشاریہ, -delta);
  }

  واپس اشاریہ;
}

خالی
UnicodeString::do_نکالو(int32_t آغاز,
             int32_t length,
             char16_t *dst,
             int32_t dst_آغاز) ثابت
{
  // pin indices to legal values
  pinIndices(آغاز, length);

  // do not copy anything if we alias dst itself
  ثابت char16_t *array = get_Array_آغاز();
  اگر(array + آغاز != dst + dst_آغاز) {
    us_arrayCopy(array, آغاز, dst, dst_آغاز, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_استعداد,
                       UError_رمز &error_رمز) ثابت {
  int32_t len = length();
  اگر(U_SUCCESS(error_رمز)) {
    اگر (isBogus() || dest_استعداد < 0 || (dest_استعداد > 0 && dest == خالی_اشاریہ)) {
      error_رمز=U_ILLEGAL_دلیل_ERROR;
    } ورنہ {
      ثابت char16_t *array = get_Array_آغاز();
      اگر(len>0 && len<=dest_استعداد && array!=dest) {
        u_memcpy(dest, array, len);
      }
      واپس u_terminateUChars(dest, dest_استعداد, len, &error_رمز);
    }
  }

  واپس len;
}

int32_t
UnicodeString::extract(int32_t آغاز,
                       int32_t length,
                       حرف *ہدف,
                       int32_t ہدف_استعداد,
                       شمار EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  اگر(ہدف_استعداد < 0 || (ہدف_استعداد > 0 && ہدف == خالی_اشاریہ)) {
    واپس 0;
  }

  // pin the indices to legal values
  pinIndices(آغاز, length);

  اگر(length <= ہدف_استعداد) {
    u_UChars_مقصد_Chars(get_Array_آغاز() + آغاز, ہدف, length);
  }
  UError_رمز حالت_2 = U_ZERO_ERROR;
  واپس u_terminateChars(ہدف, ہدف_استعداد, length, &حالت_2);
}

UnicodeString
UnicodeString::عارضی_Sub_String(int32_t آغاز, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(آغاز, len);
  ثابت char16_t *array = get_بفر();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  اگر(array==خالی_اشاریہ) {
    array=fUnion.f_ڈھیر_Fields.f_بفر;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  واپس UnicodeString(جھوٹ, array + آغاز, len);
}

int32_t
UnicodeString::مقصد_UTF8(int32_t آغاز, int32_t len,
                      حرف *ہدف, int32_t capacity) ثابت {
  pinIndices(آغاز, len);
  int32_t length8;
  UError_رمز error_رمز = U_ZERO_ERROR;
  u_str_مقصد_UTF8_With_Sub(ہدف, capacity, &length8,
                     get_بفر() + آغاز, len,
                     0xFFFD,  // Standard substitution character.
                     خالی_اشاریہ,    // Don't care about number of substitutions.
                     &error_رمز);
  واپس length8;
}

#اگر U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t آغاز, int32_t len,
                       حرف *ہدف, uint32_t dst_سائز) ثابت {
  // if the arguments are illegal, then do nothing
  اگر (/*dstSize < 0 || */(dst_سائز > 0 && ہدف == خالی_اشاریہ)) {
    واپس 0;
  }
  واپس مقصد_UTF8(آغاز, len, ہدف, dst_سائز <= 0x7fffffff ? static_cast<int32_t>(dst_سائز) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#اگر_ختم

خالی 
UnicodeString::نکالو_Between(int32_t آغاز,
                  int32_t حد_بندی,
                  UnicodeString& ہدف) ثابت {
  pin_اشاریہ(آغاز);
  pin_اشاریہ(حد_بندی);
  do_نکالو(آغاز, حد_بندی - آغاز, ہدف);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
خالی
UnicodeString::مقصد_UTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  اگر(length16 != 0) {
    حرف ڈھیر_بفر[1024];
    int32_t capacity = static_cast<int32_t>(حجم(ڈھیر_بفر));
    UBool utf8IsOwned = جھوٹ;
    حرف *utf8 = sink.Get_Append_بفر(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ڈھیر_بفر, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_رمز error_رمز = U_ZERO_ERROR;
    u_str_مقصد_UTF8_With_Sub(utf8, capacity, &length8,
                       get_بفر(), length16,
                       0xFFFD,  // Standard substitution character.
                       خالی_اشاریہ,    // Don't care about number of substitutions.
                       &error_رمز);
    اگر(error_رمز == U_بفر_حد_سے_تجاوز_ERROR) {
      utf8 = static_cast<حرف*>(uprv_malloc(length8));
      اگر(utf8 != خالی_اشاریہ) {
        utf8IsOwned = سچ;
        error_رمز = U_ZERO_ERROR;
        u_str_مقصد_UTF8_With_Sub(utf8, length8, &length8,
                           get_بفر(), length16,
                           0xFFFD,  // Standard substitution character.
                           خالی_اشاریہ,    // Don't care about number of substitutions.
                           &error_رمز);
      } ورنہ {
        error_رمز = U_حافظہ_تخصیص_ERROR;
      }
    }
    اگر(U_SUCCESS(error_رمز)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    اگر(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::مقصد_UTF32(UChar32 *utf32, int32_t capacity, UError_رمز &error_رمز) ثابت {
  int32_t length32=0;
  اگر(U_SUCCESS(error_رمز)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_مقصد_UTF32_With_Sub(utf32, capacity, &length32,
        get_بفر(), length(),
        0xfffd,  // Substitution character.
        خالی_اشاریہ,    // Don't care about number of substitutions.
        &error_رمز);
  }
  واپس length32;
}

int32_t 
UnicodeString::اشاریہ_کا(ثابت char16_t *srcChars,
               int32_t src_آغاز,
               int32_t src_لمبائی,
               int32_t آغاز,
               int32_t length) ثابت
{
  اگر (isBogus() || srcChars == خالی_اشاریہ || src_آغاز < 0 || src_لمبائی == 0) {
    واپس -1;
  }

  // UnicodeString does not find empty substrings
  اگر(src_لمبائی < 0 && srcChars[src_آغاز] == 0) {
    واپس -1;
  }

  // get the indices within bounds
  pinIndices(آغاز, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_str_ڈھونڈو_پہلا(array + آغاز, length, srcChars + src_آغاز, src_لمبائی);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

int32_t
UnicodeString::do_اشاریہ_کا(char16_t c,
             int32_t آغاز,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(آغاز, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_memchr(array + آغاز, c, length);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

int32_t
UnicodeString::do_اشاریہ_کا(UChar32 c,
                         int32_t آغاز,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(آغاز, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_memchr32(array + آغاز, c, length);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

int32_t 
UnicodeString::آخری_اشاریہ_کا(ثابت char16_t *srcChars,
               int32_t src_آغاز,
               int32_t src_لمبائی,
               int32_t آغاز,
               int32_t length) ثابت
{
  اگر (isBogus() || srcChars == خالی_اشاریہ || src_آغاز < 0 || src_لمبائی == 0) {
    واپس -1;
  }

  // UnicodeString does not find empty substrings
  اگر(src_لمبائی < 0 && srcChars[src_آغاز] == 0) {
    واپس -1;
  }

  // get the indices within bounds
  pinIndices(آغاز, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_str_ڈھونڈو_آخری(array + آغاز, length, srcChars + src_آغاز, src_لمبائی);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

int32_t
UnicodeString::do_آخری_اشاریہ_کا(char16_t c,
                 int32_t آغاز,
                 int32_t length) ثابت
{
  اگر(isBogus()) {
    واپس -1;
  }

  // pin indices
  pinIndices(آغاز, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_memrchr(array + آغاز, c, length);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

int32_t
UnicodeString::do_آخری_اشاریہ_کا(UChar32 c,
                             int32_t آغاز,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(آغاز, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_آغاز();
  ثابت char16_t *مطابقت = u_memrchr32(array + آغاز, c, length);
  اگر(مطابقت == خالی_اشاریہ) {
    واپس -1;
  } ورنہ {
    واپس static_cast<int32_t>(مطابقت - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::ڈھونڈو_And_Replace(int32_t آغاز,
                              int32_t length,
                              ثابت UnicodeString& old_متن,
                              int32_t old_آغاز,
                              int32_t old_لمبائی,
                              ثابت UnicodeString& new_متن,
                              int32_t new_آغاز,
                              int32_t new_لمبائی) U_LIFETIME_BOUND
{
  اگر(isBogus() || old_متن.isBogus() || new_متن.isBogus()) {
    واپس *یہ;
  }

  pinIndices(آغاز, length);
  old_متن.pinIndices(old_آغاز, old_لمبائی);
  new_متن.pinIndices(new_آغاز, new_لمبائی);

  اگر(old_لمبائی == 0) {
    واپس *یہ;
  }

  جب(length > 0 && length >= old_لمبائی) {
    int32_t pos = اشاریہ_کا(old_متن, old_آغاز, old_لمبائی, آغاز, length);
    اگر(pos < 0) {
      // no more oldText's here: done
      توڑو;
    } ورنہ {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_لمبائی, new_متن, new_آغاز, new_لمبائی);
      length -= pos + old_لمبائی - آغاز;
      آغاز = pos + new_لمبائی;
    }
  }

  واپس *یہ;
}


خالی
UnicodeString::سیٹ_مقصد_Bogus()
{
  releaseArray();

  fUnion.fFields.f_لمبائی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالی_اشاریہ;
  fUnion.fFields.f_استعداد = 0;
}

// turn a bogus string into an empty one
خالی
UnicodeString::unBogus() {
  اگر(fUnion.fFields.f_لمبائی_And_Flags & kIsBogus) {
    سیٹ_مقصد_خالی_خانے();
  }
}

ثابت char16_t*
UnicodeString::get_Terminated_بفر() U_LIFETIME_BOUND {
  اگر(!isWritable()) {
    واپس خالی_اشاریہ;
  }
  char16_t *array = get_Array_آغاز();
  int32_t len = length();
  اگر(len < get_استعداد()) {
    اگر(fUnion.fFields.f_لمبائی_And_Flags & k_بفر_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      اگر(array[len] == 0) {
        واپس array;
      }
    } ورنہ اگر(((fUnion.fFields.f_لمبائی_And_Flags & kRefCounted) == 0 || ref_گنتی() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      واپس array;
    }
  }
  اگر(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = get_Array_آغاز();
    array[len] = 0;
    واپس array;
  } ورنہ {
    واپس خالی_اشاریہ;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سیٹ_مقصد(UBool isTerminated,
                     ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                     int32_t متن_لمبائی) U_LIFETIME_BOUND
{
  اگر(fUnion.fFields.f_لمبائی_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    واپس *یہ;
  }

  ثابت char16_t *متن = متن_Ptr;
  اگر(متن == خالی_اشاریہ) {
    // treat as an empty string, do not alias
    releaseArray();
    سیٹ_مقصد_خالی_خانے();
    واپس *یہ;
  }

  اگر( متن_لمبائی < -1 ||
      (متن_لمبائی == -1 && !isTerminated) ||
      (متن_لمبائی >= 0 && isTerminated && متن[متن_لمبائی] != 0)
  ) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  }

  releaseArray();

  اگر(متن_لمبائی == -1) {
    // text is terminated, or else it would have failed the above test
    متن_لمبائی = u_strlen(متن);
  }
  fUnion.fFields.f_لمبائی_And_Flags = kReadonlyAlias;
  سیٹ_Array(const_cast<char16_t*>(متن), متن_لمبائی, isTerminated ? متن_لمبائی + 1 : متن_لمبائی);
  واپس *یہ;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سیٹ_مقصد(char16_t* بفر,
                     int32_t buff_لمبائی U_LIFETIME_BOUND,
                     int32_t buff_استعداد) U_LIFETIME_BOUND {
  اگر(fUnion.fFields.f_لمبائی_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    واپس *یہ;
  }

  اگر(بفر == خالی_اشاریہ) {
    // treat as an empty string, do not alias
    releaseArray();
    سیٹ_مقصد_خالی_خانے();
    واپس *یہ;
  }

  اگر(buff_لمبائی < -1 || buff_استعداد < 0 || buff_لمبائی > buff_استعداد) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  } ورنہ اگر(buff_لمبائی == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = بفر, *حد_بندی = بفر + buff_استعداد;
    جب(p != حد_بندی && *p != 0) {
      ++p;
    }
    buff_لمبائی = static_cast<int32_t>(p - بفر);
  }

  releaseArray();

  fUnion.fFields.f_لمبائی_And_Flags = kWritableAlias;
  سیٹ_Array(بفر, buff_لمبائی, buff_استعداد);
  واپس *یہ;
}

UnicodeString& UnicodeString::سیٹ_مقصد_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  اگر(length <= US_STACKBUF_سائز) {
    capacity = US_STACKBUF_سائز;
  } ورنہ {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_بفر(capacity);
  int32_t length16;
  UError_رمز error_رمز = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_استعداد(), &length16,
      utf8.ڈیٹا(), length,
      0xfffd,  // Substitution character.
      خالی_اشاریہ,    // Don't care about number of substitutions.
      &error_رمز);
  release_بفر(length16);
  اگر(U_FAILURE(error_رمز)) {
    سیٹ_مقصد_Bogus();
  }
  واپس *یہ;
}

UnicodeString&
UnicodeString::سیٹ_Char_At(int32_t انحراف, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  اگر(cloneArrayIfNeeded() && len > 0) {
    اگر(انحراف < 0) {
      انحراف = 0;
    } ورنہ اگر(انحراف >= len) {
      انحراف = len - 1;
    }

    get_Array_آغاز()[انحراف] = c;
  }
  واپس *یہ;
}

UnicodeString&
UnicodeString::replace(int32_t آغاز,
                       int32_t لمبائی_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_لمبائی];
  int32_t count = 0;
  UBool isError = جھوٹ;
  U16_APPEND(بفر, count, U16_MAX_لمبائی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  واپس doReplace(آغاز, لمبائی_4, بفر, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_لمبائی];
  int32_t لمبائی_4 = 0;
  UBool isError = جھوٹ;
  U16_APPEND(بفر, لمبائی_4, U16_MAX_لمبائی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  واپس isError ? *یہ : doAppend(بفر, 0, لمبائی_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t آغاز,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t src_آغاز,
                         int32_t src_لمبائی) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_آغاز, src_لمبائی);

  // get the characters from src
  // and replace the range in ourselves with them
  واپس doReplace(آغاز, length, src.get_Array_آغاز(), src_آغاز, src_لمبائی);
}

UnicodeString&
UnicodeString::doReplace(int32_t آغاز,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t src_آغاز,
                         int32_t src_لمبائی) U_LIFETIME_BOUND
{
  اگر(!isWritable()) {
    واپس *یہ;
  }

  int32_t old_لمبائی = یہ->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  اگر((fUnion.fFields.f_لمبائی_And_Flags&k_بفر_Is_Readonly) && src_لمبائی == 0) {
    اگر(آغاز == 0) {
      // remove prefix by adjusting the array pointer
      pin_اشاریہ(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_استعداد -= length;
      سیٹ_لمبائی(old_لمبائی - length);
      واپس *یہ;
    } ورنہ {
      pin_اشاریہ(آغاز);
      اگر(length >= (old_لمبائی - آغاز)) {
        // remove suffix by reducing the length (like truncate())
        سیٹ_لمبائی(آغاز);
        fUnion.fFields.f_استعداد = آغاز;  // not NUL-terminated any more
        واپس *یہ;
      }
    }
  }

  اگر(آغاز == old_لمبائی) {
    واپس doAppend(srcChars, src_آغاز, src_لمبائی);
  }

  اگر (srcChars == خالی_اشاریہ) {
    src_لمبائی = 0;
  } ورنہ {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_آغاز;
    اگر (src_لمبائی < 0) {
      // get the srcLength if necessary
      src_لمبائی = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(آغاز, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_لمبائی = old_لمبائی - length;
  اگر(src_لمبائی > (INT32_MAX - new_لمبائی)) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  }
  new_لمبائی += src_لمبائی;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = get_Array_آغاز();
  اگر (is_بفر_Writable() &&
      oldArray < srcChars + src_لمبائی &&
      srcChars < oldArray + old_لمبائی) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_لمبائی);
    اگر (copy.isBogus()) {
      سیٹ_مقصد_Bogus();
      واپس *یہ;
    }
    واپس doReplace(آغاز, length, copy.get_Array_آغاز(), 0, src_لمبائی);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ڈھیر_بفر[US_STACKBUF_سائز];
  اگر((fUnion.fFields.f_لمبائی_And_Flags&k_Using_ڈھیر_بفر) && (new_لمبائی > US_STACKBUF_سائز)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ڈھیر_بفر, oldArray, old_لمبائی);
    oldArray = old_ڈھیر_بفر;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *بفر_مقصد_Delete = خالی_اشاریہ;
  اگر(!cloneArrayIfNeeded(new_لمبائی, get_Grow_استعداد(new_لمبائی),
                         جھوٹ, &بفر_مقصد_Delete)
  ) {
    واپس *یہ;
  }

  // now do the replace

  char16_t *newArray = get_Array_آغاز();
  اگر(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, آغاز);
    us_arrayCopy(oldArray, آغاز + length,
                 newArray, آغاز + src_لمبائی,
                 old_لمبائی - (آغاز + length));
  } ورنہ اگر(length != src_لمبائی) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, آغاز + length,
                 newArray, آغاز + src_لمبائی,
                 old_لمبائی - (آغاز + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, آغاز, src_لمبائی);

  سیٹ_لمبائی(new_لمبائی);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  اگر (بفر_مقصد_Delete) {
    uprv_free(بفر_مقصد_Delete);
  }

  واپس *یہ;
}

UnicodeString&
UnicodeString::doReplace(int32_t آغاز, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  اگر (!isWritable()) {
    واپس *یہ;
  }
  اگر (src.length() > INT32_MAX) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  }
  واپس doReplace(آغاز, length, src.ڈیٹا(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t src_آغاز,
                        int32_t src_لمبائی) U_LIFETIME_BOUND {
  اگر(src_لمبائی == 0) {
    واپس *یہ;
  }

  // pin the indices to legal values
  src.pinIndices(src_آغاز, src_لمبائی);
  واپس doAppend(src.get_Array_آغاز(), src_آغاز, src_لمبائی);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t src_آغاز,
                        int32_t src_لمبائی) U_LIFETIME_BOUND {
  اگر(!isWritable() || src_لمبائی == 0 || srcChars == خالی_اشاریہ) {
    واپس *یہ;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_آغاز;

  اگر(src_لمبائی < 0) {
    // get the srcLength if necessary
    اگر((src_لمبائی = u_strlen(srcChars)) == 0) {
      واپس *یہ;
    }
  }

  int32_t old_لمبائی = length();
  int32_t new_لمبائی;

  اگر (src_لمبائی <= get_استعداد() - old_لمبائی && is_بفر_Writable()) {
    new_لمبائی = old_لمبائی + src_لمبائی;
    // Faster than a memmove
    اگر (src_لمبائی <= 4) {
      char16_t *arr = get_Array_آغاز();
      arr[old_لمبائی] = srcChars[0];
      اگر (src_لمبائی > 1) arr[old_لمبائی+1] = srcChars[1];
      اگر (src_لمبائی > 2) arr[old_لمبائی+2] = srcChars[2];
      اگر (src_لمبائی > 3) arr[old_لمبائی+3] = srcChars[3];
      سیٹ_لمبائی(new_لمبائی);
      واپس *یہ;
    }
  } ورنہ {
    اگر (uprv_add32_حد_سے_تجاوز(old_لمبائی, src_لمبائی, &new_لمبائی)) {
      سیٹ_مقصد_Bogus();
      واپس *یہ;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = get_Array_آغاز();
    اگر (is_بفر_Writable() &&
        oldArray < srcChars + src_لمبائی &&
        srcChars < oldArray + old_لمبائی) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_لمبائی);
      اگر (copy.isBogus()) {
        سیٹ_مقصد_Bogus();
        واپس *یہ;
      }
      واپس doAppend(copy.get_Array_آغاز(), 0, src_لمبائی);
    }

    // optimize append() onto a large-enough, owned string
    اگر (!cloneArrayIfNeeded(new_لمبائی, get_Grow_استعداد(new_لمبائی))) {
      واپس *یہ;
    }
  }

  char16_t *newArray = get_Array_آغاز();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  اگر(srcChars != newArray + old_لمبائی) {
    us_arrayCopy(srcChars, 0, newArray, old_لمبائی, src_لمبائی);
  }
  سیٹ_لمبائی(new_لمبائی);

  واپس *یہ;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  اگر (!isWritable() || src.empty()) {
    واپس *یہ;
  }
  اگر (src.length() > INT32_MAX) {
    سیٹ_مقصد_Bogus();
    واپس *یہ;
  }
  واپس doAppend(src.ڈیٹا(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
خالی
UnicodeString::handleReplaceBetween(int32_t آغاز,
                                    int32_t حد_بندی,
                                    ثابت UnicodeString& متن) {
    replaceBetween(آغاز, حد_بندی, متن);
}

/**
 * Replaceable API
 */
خالی 
UnicodeString::copy(int32_t آغاز, int32_t حد_بندی, int32_t dest) {
    اگر (حد_بندی <= آغاز) {
        واپس; // Nothing to do; avoid bogus malloc call
    }
    char16_t* متن = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * (حد_بندی - آغاز)));
    // Check to make sure text is not null.
    اگر (متن != خالی_اشاریہ) {
	    نکالو_Between(آغاز, حد_بندی, متن, 0);
	    insert(dest, متن, 0, حد_بندی - آغاز);    
	    uprv_free(متن);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_ڈیٹا() ثابت {
    واپس سچ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_ڈیٹا() ثابت {
    واپس جھوٹ;
}

UnicodeString&
UnicodeString::do_الٹ(int32_t آغاز, int32_t length) U_LIFETIME_BOUND {
  اگر(length <= 1 || !cloneArrayIfNeeded()) {
    واپس *یہ;
  }

  // pin the indices to legal values
  pinIndices(آغاز, length);
  اگر(length <= 1) {  // pinIndices() might have shrunk the length
    واپس *یہ;
  }

  char16_t *بائیں = get_Array_آغاز() + آغاز;
  char16_t *دائیں = بائیں + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = جھوٹ;

  // Before the loop we know left<right because length>=2.
  کرو {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *بائیں));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*بائیں++ = *دائیں));
    *دائیں-- = swap;
  } جب(بائیں < دائیں);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*بائیں));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  اگر(hasSupplementary) {
    char16_t swap2;

    بائیں = get_Array_آغاز() + آغاز;
    دائیں = بائیں + length - 1; // -1 so that we can look at *(left+1) if left<right
    جب(بائیں < دائیں) {
      اگر(U16_IS_TRAIL(swap = *بائیں) && U16_IS_LEAD(swap2 = *(بائیں + 1))) {
        *بائیں++ = swap2;
        *بائیں++ = swap;
      } ورنہ {
        ++بائیں;
      }
    }
  }

  واپس *یہ;
}

UBool 
UnicodeString::padLeading(int32_t ہدف_لمبائی,
                          char16_t padChar)
{
  int32_t old_لمبائی = length();
  اگر(old_لمبائی >= ہدف_لمبائی || !cloneArrayIfNeeded(ہدف_لمبائی)) {
    واپس جھوٹ;
  } ورنہ {
    // move contents up by padding width
    char16_t *array = get_Array_آغاز();
    int32_t آغاز = ہدف_لمبائی - old_لمبائی;
    us_arrayCopy(array, 0, array, آغاز, old_لمبائی);

    // fill in padding character
    جب(--آغاز >= 0) {
      array[آغاز] = padChar;
    }
    سیٹ_لمبائی(ہدف_لمبائی);
    واپس سچ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t ہدف_لمبائی,
                           char16_t padChar)
{
  int32_t old_لمبائی = length();
  اگر(old_لمبائی >= ہدف_لمبائی || !cloneArrayIfNeeded(ہدف_لمبائی)) {
    واپس جھوٹ;
  } ورنہ {
    // fill in padding character
    char16_t *array = get_Array_آغاز();
    int32_t length = ہدف_لمبائی;
    جب(--length >= old_لمبائی) {
      array[length] = padChar;
    }
    سیٹ_لمبائی(ہدف_لمبائی);
    واپس سچ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_رمز() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_رمز = ustr_hashUCharsN(get_Array_آغاز(), length());
    اگر (hash_رمز == k_Invalid_Hash_رمز) {
        hash_رمز = k_خالی_خانے_Hash_رمز;
    }
    واپس hash_رمز;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_بفر(int32_t min_استعداد) U_LIFETIME_BOUND {
  اگر(min_استعداد>=-1 && cloneArrayIfNeeded(min_استعداد)) {
    fUnion.fFields.f_لمبائی_And_Flags|=k_Open_Get_بفر;
    سیٹ_Zero_لمبائی();
    واپس get_Array_آغاز();
  } ورنہ {
    واپس خالی_اشاریہ;
  }
}

خالی
UnicodeString::release_بفر(int32_t new_لمبائی) {
  اگر(fUnion.fFields.f_لمبائی_And_Flags&k_Open_Get_بفر && new_لمبائی>=-1) {
    // set the new fLength
    int32_t capacity=get_استعداد();
    اگر(new_لمبائی==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=get_Array_آغاز(), *p=array, *حد_بندی=array+capacity;
      جب(p<حد_بندی && *p!=0) {
        ++p;
      }
      new_لمبائی = static_cast<int32_t>(p - array);
    } ورنہ اگر(new_لمبائی>capacity) {
      new_لمبائی=capacity;
    }
    سیٹ_لمبائی(new_لمبائی);
    fUnion.fFields.f_لمبائی_And_Flags&=~k_Open_Get_بفر;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_استعداد,
                                  int32_t grow_استعداد,
                                  UBool doCopyArray,
                                  int32_t **p_بفر_مقصد_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  اگر(new_استعداد == -1) {
    new_استعداد = get_استعداد();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  اگر(!isWritable()) {
    واپس جھوٹ;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  اگر(forceClone ||
     fUnion.fFields.f_لمبائی_And_Flags & k_بفر_Is_Readonly ||
     (fUnion.fFields.f_لمبائی_And_Flags & kRefCounted && ref_گنتی() > 1) ||
     new_استعداد > get_استعداد()
  ) {
    // check growCapacity for default value and use of the stack buffer
    اگر(grow_استعداد < 0) {
      grow_استعداد = new_استعداد;
    } ورنہ اگر(new_استعداد <= US_STACKBUF_سائز && grow_استعداد > US_STACKBUF_سائز) {
      grow_استعداد = US_STACKBUF_سائز;
    } ورنہ اگر(new_استعداد > grow_استعداد) {
      سیٹ_مقصد_Bogus();
      واپس جھوٹ;  // bad inputs
    }
    اگر(grow_استعداد > k_Max_استعداد) {
      سیٹ_مقصد_Bogus();
      واپس جھوٹ;
    }

    // save old values
    char16_t old_ڈھیر_بفر[US_STACKBUF_سائز];
    char16_t *oldArray;
    int32_t old_لمبائی = length();
    int16_t flags = fUnion.fFields.f_لمبائی_And_Flags;

    اگر(flags&k_Using_ڈھیر_بفر) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      اگر(doCopyArray && grow_استعداد > US_STACKBUF_سائز) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ڈھیر_Fields.f_بفر, 0, old_ڈھیر_بفر, 0, old_لمبائی);
        oldArray = old_ڈھیر_بفر;
      } ورنہ {
        oldArray = خالی_اشاریہ; // no need to copy from the stack buffer to itself
      }
    } ورنہ {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=خالی_اشاریہ); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    اگر(allocate(grow_استعداد) ||
       (new_استعداد < grow_استعداد && allocate(new_استعداد))
    ) {
      اگر(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_لمبائی = old_لمبائی;
        new_استعداد = get_استعداد();
        اگر(new_استعداد < min_لمبائی) {
          min_لمبائی = new_استعداد;
        }
        اگر(oldArray != خالی_اشاریہ) {
          us_arrayCopy(oldArray, 0, get_Array_آغاز(), 0, min_لمبائی);
        }
        سیٹ_لمبائی(min_لمبائی);
      } ورنہ {
        سیٹ_Zero_لمبائی();
      }

      // release the old array
      اگر(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_گنتی = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        اگر(umtx_atomic_dec(p_Ref_گنتی) == 0) {
          اگر (p_بفر_مقصد_Delete == خالی_اشاریہ) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((خالی *)p_Ref_گنتی);
          } ورنہ {
            // the caller requested to delete it himself
            *p_بفر_مقصد_Delete = reinterpret_cast<int32_t*>(p_Ref_گنتی);
          }
        }
      }
    } ورنہ {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      اگر(!(flags&k_Using_ڈھیر_بفر)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_لمبائی_And_Flags = flags;
      سیٹ_مقصد_Bogus();
      واپس جھوٹ;
    }
  }
  واپس سچ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_رمز_Unit(char16_t c) {
  واپس str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_رمز_نقطہ(UChar32 c) {
  char16_t بفر[U16_MAX_لمبائی];
  int32_t c_لمبائی = 0;
  UBool isError = جھوٹ;
  U16_APPEND(بفر, c_لمبائی, U16_MAX_لمبائی, c, isError);
  واپس !isError && str.doAppend(بفر, 0, c_لمبائی).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  واپس str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_استعداد(int32_t append_استعداد) {
  واپس str.cloneArrayIfNeeded(str.length() + append_استعداد);
}

char16_t* UnicodeStringAppendable::get_Append_بفر(int32_t min_استعداد,
                                                   int32_t desired_استعداد_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_استعداد,
                                                   int32_t* نتیجہ_استعداد) U_LIFETIME_BOUND {
  اگر(min_استعداد < 1 || scratch_استعداد < min_استعداد) {
    *نتیجہ_استعداد = 0;
    واپس خالی_اشاریہ;
  }
  int32_t old_لمبائی = str.length();
  اگر(min_استعداد <= (k_Max_استعداد - old_لمبائی) &&
      desired_استعداد_Hint <= (k_Max_استعداد - old_لمبائی) &&
      str.cloneArrayIfNeeded(old_لمبائی + min_استعداد, old_لمبائی + desired_استعداد_Hint)) {
    *نتیجہ_استعداد = str.get_استعداد() - old_لمبائی;
    واپس str.get_Array_آغاز() + old_لمبائی;
  }
  *نتیجہ_استعداد = scratch_استعداد;
  واپس scratch;
}

U_NAMESPACE_انتہا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement کلید) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.اشارہ_گر;
    واپس (str == خالی_اشاریہ) ? 0 : str->hash_رمز();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارہ_گر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارہ_گر;
    اگر (str1 == str2) {
        واپس سچ;
    }
    اگر (str1 == خالی_اشاریہ || str2 == خالی_اشاریہ) {
        واپس جھوٹ;
    }
    واپس *str1 == *str2;
}

#اگر_تعریف_شدہ U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#اگر defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#ہدایت GCC diagnostic push
#ہدایت GCC diagnostic ignored "-Wunused-function"
ساکن خالی uprv_UnicodeStringDummy() {
    حذف [] (نیا UnicodeString[2]);
}
#ہدایت GCC diagnostic pop
#اگر_ختم
#اگر_ختم
