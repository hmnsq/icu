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

#ضمن <string_view>

#ضمن "unicode/utypes.h"
#ضمن "unicode/appendable.h"
#ضمن "unicode/putil.h"
#ضمن "cstring.h"
#ضمن "cmemory.h"
#ضمن "unicode/ustring.h"
#ضمن "unicode/unistr.h"
#ضمن "unicode/utf.h"
#ضمن "unicode/utf16.h"
#ضمن "uelement.h"
#ضمن "ustr_imp.h"
#ضمن "umutex.h"
#ضمن "uassert.h"

#إذا 0

#ضمن <iostream>
ورتوں نطاق std;

//DEBUGGING
فراغ
print(ثابت UnicodeString& s,
      ثابت حرف *name)
{
  char16_t c;
  باہر << name << ":|";
  لكل(صحيح i = 0; i < s.length(); ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      باہر << "[0x" << hex << s[i] << "]";
    وإلا
      باہر << (حرف) s[i];
  }
  باہر << '|' << سطر_انت;
}

فراغ
print(ثابت char16_t *s,
      int32_t len,
      ثابت حرف *name)
{
  char16_t c;
  باہر << name << ":|";
  لكل(صحيح i = 0; i < len; ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      باہر << "[0x" << hex << s[i] << "]";
    وإلا
      باہر << (حرف) s[i];
  }
  باہر << '|' << سطر_انت;
}
// END DEBUGGING
#جے_انت

// Local function definitions for now

// need to copy areas that may overlap
ساكن
مضمن فراغ
us_arrayCopy(ثابت char16_t *src, int32_t src_شروع,
         char16_t *dst, int32_t dst_شروع, int32_t count)
{
  إذا(count>0) {
    uprv_memmove(dst+dst_شروع, src+src_شروع, (size_t)count*حجم(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ساكن char16_t U_CALLCONV
UnicodeString_charAt(int32_t وتھ, فراغ *context) {
    أرجع ((icu::UnicodeString*) context)->charAt(وتھ);
}
U_CDECL_سرا

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
عامل+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t جوڑ_Lengths;
  إذا (uprv_add32_حد_توں_ودھ(s1.length(), s2.length(), &جوڑ_Lengths)) {
    UnicodeString bogus;
    bogus.سیٹ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (جوڑ_Lengths != INT32_MAX) {
    ++جوڑ_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جوڑ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_اندرونی_Concat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t جوڑ_Lengths;
  إذا (s2.length() > INT32_MAX ||
      uprv_add32_حد_توں_ودھ(s1.length(), static_cast<int32_t>(s2.length()), &جوڑ_Lengths)) {
    UnicodeString bogus;
    bogus.سیٹ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (جوڑ_Lengths != INT32_MAX) {
    ++جوڑ_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جوڑ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

فراغ
UnicodeString::جوڑو_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  أرجع umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_گنتی() ثابت {
  أرجع umtx_لدو_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

فراغ
UnicodeString::releaseArray() {
  إذا((fUnion.fFields.f_لمائی_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_لمائی_And_Flags = 0;
  إذا (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } وإلا إذا(c <= 0xffff) {
    int32_t length = count;
    إذا(capacity < length) {
      capacity = length;
    }
    إذا(allocate(capacity)) {
      char16_t *array = get_Array_شروع();
      char16_t unit = static_cast<char16_t>(c);
      لكل(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      سیٹ_لمائی(length);
    }
  } وإلا {  // supplementary code point, write surrogate pairs
    إذا(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      أرجع;
    }
    int32_t length = count * 2;
    إذا(capacity < length) {
      capacity = length;
    }
    إذا(allocate(capacity)) {
      char16_t *array = get_Array_شروع();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      لكل(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      سیٹ_لمائی(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_لمائی_And_Flags = kLength1 | kShortString;
  fUnion.f_ڈھیر_Fields.f_بفر[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = خطأ;
  U16_APPEND(fUnion.f_ڈھیر_Fields.f_بفر, i, US_STACKBUF_ناپ, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  إذا(!isError) {
    سیٹ_Short_لمائی(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *متن,
                             int32_t متن_لمائی) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  doAppend(متن, 0, متن_لمائی);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                             int32_t متن_لمائی) {
  fUnion.fFields.f_لمائی_And_Flags = kReadonlyAlias;
  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == خالی_سوچک) {
    // treat as an empty string, do not alias
    سیٹ_منزل_خالی_تھاواں();
  } وإلا إذا(متن_لمائی < -1 ||
            (متن_لمائی == -1 && !isTerminated) ||
            (متن_لمائی >= 0 && isTerminated && متن[متن_لمائی] != 0)
  ) {
    سیٹ_منزل_Bogus();
  } وإلا {
    إذا(متن_لمائی == -1) {
      // text is terminated, or else it would have failed the above test
      متن_لمائی = u_strlen(متن);
    }
    سیٹ_Array(const_cast<char16_t *>(متن), متن_لمائی,
             isTerminated ? متن_لمائی + 1 : متن_لمائی);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_لمائی,
                             int32_t buff_سمرتھا) {
  fUnion.fFields.f_لمائی_And_Flags = kWritableAlias;
  إذا(buff == خالی_سوچک) {
    // treat as an empty string, do not alias
    سیٹ_منزل_خالی_تھاواں();
  } وإلا إذا(buff_لمائی < -1 || buff_سمرتھا < 0 || buff_لمائی > buff_سمرتھا) {
    سیٹ_منزل_Bogus();
  } وإلا {
    إذا(buff_لمائی == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *حد_بندی = buff + buff_سمرتھا;
      طالما(p != حد_بندی && *p != 0) {
        ++p;
      }
      buff_لمائی = static_cast<int32_t>(p - buff);
    }
    سیٹ_Array(buff, buff_لمائی, buff_سمرتھا);
  }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t length, EInvariant) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  إذا(src==خالی_سوچک) {
    // treat as an empty string
  } وإلا {
    إذا(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    إذا(cloneArrayIfNeeded(length, length, خطأ)) {
      u_chars_منزل_UChars(src, get_Array_شروع(), length);
      سیٹ_لمائی(length);
    } وإلا {
      سیٹ_منزل_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND) {
  UnicodeString نتیجہ;
  إذا (متن.length() <= INT32_MAX) {
    نتیجہ.سیٹ_منزل(خطأ, متن.انکڑے(), static_cast<int32_t>(متن.length()));
  } وإلا {
    نتیجہ.سیٹ_منزل_Bogus();
  }
  أرجع نتیجہ;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
  UnicodeString نتیجہ;
  إذا (متن.isBogus()) {
    نتیجہ.سیٹ_منزل_Bogus();
  } وإلا {
    نتیجہ.سیٹ_منزل(خطأ, متن.get_بفر(), متن.length());
  }
  أرجع نتیجہ;
}

#إذا U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  إذا (codepage_انکڑے != خالی_سوچک) {
    سیٹ_منزل_UTF8(codepage_انکڑے);
  }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_انکڑے, int32_t انکڑے_لمائی) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  إذا (codepage_انکڑے == خالی_سوچک || انکڑے_لمائی == 0 || انکڑے_لمائی < -1) {
    أرجع;
  }
  إذا(انکڑے_لمائی == -1) {
    انکڑے_لمائی = static_cast<int32_t>(uprv_strlen(codepage_انکڑے));
  }
  سیٹ_منزل_UTF8(StringPiece(codepage_انکڑے, انکڑے_لمائی));
}

// else see unistr_cnv.cpp
#جے_انت

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, سچ);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروع) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  سیٹ_منزل(that, src_شروع);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروع,
                             int32_t src_لمائی) {
  fUnion.fFields.f_لمائی_And_Flags = kShortString;
  سیٹ_منزل(that, src_شروع, src_لمائی);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  أرجع خالی_سوچک;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  Local_اشارہ_گر<UnicodeString> clonedString(جديد UnicodeString(*هذا));
  أرجع clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : خالی_سوچک;
}

//========================================
// array allocation
//========================================

نطاق {

ثابت int32_t k_Grow_ناپ = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ثابت int32_t k_Max_سمرتھا = 0x7ffffff5;

int32_t get_Grow_سمرتھا(int32_t new_لمائی) {
  int32_t grow_ناپ = (new_لمائی >> 2) + k_Grow_ناپ;
  إذا(grow_ناپ <= (k_Max_سمرتھا - new_لمائی)) {
    أرجع new_لمائی + grow_ناپ;
  } وإلا {
    أرجع k_Max_سمرتھا;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  إذا(capacity <= US_STACKBUF_ناپ) {
    fUnion.fFields.f_لمائی_And_Flags = kShortString;
    أرجع سچ;
  }
  إذا(capacity <= k_Max_سمرتھا) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t عدد_بائٹاں = حجم(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    عدد_بائٹاں = (عدد_بائٹاں + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(عدد_بائٹاں));
    إذا(array != خالی_سوچک) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      عدد_بائٹاں -= حجم(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_سمرتھا = static_cast<int32_t>(عدد_بائٹاں / U_SIZEOF_UCHAR);
      fUnion.fFields.f_لمائی_And_Flags = kLongString;
      أرجع سچ;
    }
  }
  fUnion.fFields.f_لمائی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالی_سوچک;
  fUnion.fFields.f_سمرتھا = 0;
  أرجع خطأ;
}

//========================================
// Destructor
//========================================

#جے_تعریف_شدہ UNISTR_گنتی_FINAL_STRING_LENGTHS
ساكن u_atomic_int32_t final_لمائی_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ساكن u_atomic_int32_t beyond_گنتی(0);

U_CAPI فراغ unistr_printLengths() {
  int32_t i;
  لكل(i = 0; i <= 59; ++i) {
    لکھو("%2d,  %9d\n", i, (int32_t)final_لمائی_Counts[i]);
  }
  int32_t beyond = beyond_گنتی;
  لكل(; i < UPRV_LENGTHOF(final_لمائی_Counts); ++i) {
    beyond += final_لمائی_Counts[i];
  }
  لکھو(">59, %9d\n", beyond);
}
#جے_انت

UnicodeString::~UnicodeString()
{
#جے_تعریف_شدہ UNISTR_گنتی_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  إذا((fUnion.fFields.f_لمائی_And_Flags&(k_Open_Get_بفر|kReadonlyAlias|kWritableAlias)) == 0) {
    إذا(has_Short_لمائی()) {
      umtx_atomic_inc(final_لمائی_Counts + get_Short_لمائی());
    } وإلا {
      umtx_atomic_inc(&beyond_گنتی);
    }
  }
#جے_انت

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString نتیجہ;
  نتیجہ.سیٹ_منزل_UTF8(utf8);
  أرجع نتیجہ;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString نتیجہ;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  إذا(length <= US_STACKBUF_ناپ) {
    capacity = US_STACKBUF_ناپ;
  } وإلا {
    capacity = length + (length >> 4) + 4;
  }
  افعل {
    char16_t *utf16 = نتیجہ.get_بفر(capacity);
    int32_t length16;
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, نتیجہ.get_سمرتھا(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        خالی_سوچک,    // Don't care about number of substitutions.
        &error_کوڈ);
    نتیجہ.release_بفر(length16);
    إذا(error_کوڈ == U_بفر_حد_توں_ودھ_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      استمر;
    } وإلا إذا(U_FAILURE(error_کوڈ)) {
      نتیجہ.سیٹ_منزل_Bogus();
    }
    اكسر;
  } طالما(سچ);
  أرجع نتیجہ;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::عامل=(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src, سچ);
}

UnicodeString&
UnicodeString::copyFrom(ثابت UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  إذا(هذا == &src) {
    أرجع *هذا;
  }

  // is the right side bogus?
  إذا(src.isBogus()) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  }

  // delete the current contents
  releaseArray();

  إذا(src.is_خالی_تھاواں()) {
    // empty string - use the stack buffer
    سیٹ_منزل_خالی_تھاواں();
    أرجع *هذا;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_لمائی_And_Flags = src.fUnion.fFields.f_لمائی_And_Flags;
  بدل(src.fUnion.fFields.f_لمائی_And_Flags & kAllStorageFlags) {
  حالة kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ڈھیر_Fields.f_بفر, src.fUnion.f_ڈھیر_Fields.f_بفر,
                get_Short_لمائی() * U_SIZEOF_UCHAR);
    اكسر;
  حالة kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).جوڑو_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_سمرتھا = src.fUnion.fFields.f_سمرتھا;
    إذا(!has_Short_لمائی()) {
      fUnion.fFields.f_لمائی = src.fUnion.fFields.f_لمائی;
    }
    اكسر;
  حالة kReadonlyAlias:
    إذا(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_سمرتھا = src.fUnion.fFields.f_سمرتھا;
      إذا(!has_Short_لمائی()) {
        fUnion.fFields.f_لمائی = src.fUnion.fFields.f_لمائی;
      }
      اكسر;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالة kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_لمائی = src.length();
    إذا(allocate(src_لمائی)) {
      u_memcpy(get_Array_شروع(), src.get_Array_شروع(), src_لمائی);
      سیٹ_لمائی(src_لمائی);
      اكسر;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  افتراضي:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_لمائی_And_Flags = kIsBogus;
    fUnion.fFields.fArray = خالی_سوچک;
    fUnion.fFields.f_سمرتھا = 0;
    اكسر;
  }

  أرجع *هذا;
}

UnicodeString& UnicodeString::عامل=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, سچ);
  أرجع *هذا;
}

// Same as move assignment except without memory management.
فراغ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool سیٹ_Src_منزل_Bogus) noexcept {
  int16_t لمائی_And_Flags = fUnion.fFields.f_لمائی_And_Flags = src.fUnion.fFields.f_لمائی_And_Flags;
  إذا(لمائی_And_Flags & k_Using_ڈھیر_بفر) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    إذا(هذا != &src) {
      uprv_memcpy(fUnion.f_ڈھیر_Fields.f_بفر, src.fUnion.f_ڈھیر_Fields.f_بفر,
                  get_Short_لمائی() * U_SIZEOF_UCHAR);
    }
  } وإلا {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_سمرتھا = src.fUnion.fFields.f_سمرتھا;
    إذا(!has_Short_لمائی()) {
      fUnion.fFields.f_لمائی = src.fUnion.fFields.f_لمائی;
    }
    إذا(سیٹ_Src_منزل_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_لمائی_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = خالی_سوچک;
      src.fUnion.fFields.f_سمرتھا = 0;
    }
  }
}

فراغ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString عارضی;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  عارضی.copyFieldsFrom(*هذا, خطأ);
  هذا->copyFieldsFrom(other, خطأ);
  other.copyFieldsFrom(عارضی, خطأ);
  // Set temp to an empty string so that other's memory is not released twice.
  عارضی.fUnion.fFields.f_لمائی_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString نتیجہ(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    إذا (نتیجہ.isBogus()) {
        أرجع نتیجہ;
    }
    ثابت char16_t *array = get_بفر();
    int32_t len = length();
    int32_t prev = 0;
    لكل (int32_t i=0;;) {
        إذا (i == len) {
            نتیجہ.append(array, prev, len - prev);
            اكسر;
        }
        إذا (array[i++] == 0x5C /*'\\'*/) {
            نتیجہ.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            إذا (c < 0) {
                نتیجہ.remove(); // return empty string
                اكسر; // invalid escape sequence
            }
            نتیجہ.append(c);
            prev = i;
        }
    }
    أرجع نتیجہ;
}

UChar32 UnicodeString::unescapeAt(int32_t &وتھ) ثابت {
    أرجع u_unescapeAt(UnicodeString_charAt, &وتھ, length(), (فراغ*)هذا);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ثابت char16_t *متن, int32_t len) ثابت {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  أرجع uprv_memcmp(get_Array_شروع(), متن, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t شروع,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_شروع,
              int32_t src_لمائی) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع خطأ;
  }
  
  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == خالی_سوچک) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? سچ : خطأ;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروع();

  chars += شروع;
  srcChars += src_شروع;

  // get the srcLength if necessary
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(srcChars + src_شروع);
  }

  إذا (length != src_لمائی) {
    أرجع خطأ;
  }

  إذا(length == 0 || chars == srcChars) {
    أرجع سچ;
  }

  أرجع u_memcmp(chars, srcChars, src_لمائی) == 0;
}

int8_t
UnicodeString::doCompare( int32_t شروع,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_شروع,
              int32_t src_لمائی) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع -1;
  }
  
  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == خالی_سوچک) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروع();

  chars += شروع;
  srcChars += src_شروع;

  int32_t min_لمائی;
  int8_t لمائی_نتیجہ;

  // get the srcLength if necessary
  إذا(src_لمائی < 0) {
    src_لمائی = u_strlen(srcChars + src_شروع);
  }

  // are we comparing different lengths?
  إذا(length != src_لمائی) {
    إذا(length < src_لمائی) {
      min_لمائی = length;
      لمائی_نتیجہ = -1;
    } وإلا {
      min_لمائی = src_لمائی;
      لمائی_نتیجہ = 1;
    }
  } وإلا {
    min_لمائی = length;
    لمائی_نتیجہ = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  إذا(min_لمائی > 0 && chars != srcChars) {
    int32_t نتیجہ;

#   إذا U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      نتیجہ = uprv_memcmp(chars, srcChars, min_لمائی * حجم(char16_t));
      إذا(نتیجہ != 0) {
        أرجع (int8_t)(نتیجہ >> 15 | 1);
      }
#   وإلا
      // little-endian: compare char16_t units
      افعل {
        نتیجہ = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        إذا(نتیجہ != 0) {
          أرجع static_cast<int8_t>(نتیجہ >> 15 | 1);
        }
      } طالما(--min_لمائی > 0);
#   endif
  }
  أرجع لمائی_نتیجہ;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_کوڈ_نقطہ_ترتیب(int32_t شروع,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t src_شروع,
                                       int32_t src_لمائی) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == خالی_سوچک) {
    src_شروع = src_لمائی = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_شروع() + شروع, length, (srcChars!=خالی_سوچک)?(srcChars + src_شروع):خالی_سوچک, src_لمائی, خطأ, سچ);
  /* translate the 32-bit result into an 8-bit one */
  إذا(diff!=0) {
    أرجع static_cast<int8_t>(diff >> 15 | 1);
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_لمائی() ثابت {
    أرجع length();
}

char16_t
UnicodeString::getCharAt(int32_t وتھ) ثابت {
  أرجع charAt(وتھ);
}

UChar32
UnicodeString::getChar32At(int32_t وتھ) ثابت {
  أرجع char32At(وتھ);
}

UChar32
UnicodeString::char32At(int32_t وتھ) ثابت
{
  int32_t len = length();
  إذا (static_cast<uint32_t>(وتھ) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروع();
    UChar32 c;
    U16_GET(array, 0, وتھ, len, c);
    أرجع c;
  } وإلا {
    أرجع kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_شروع(int32_t وتھ) ثابت {
  إذا (static_cast<uint32_t>(وتھ) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = get_Array_شروع();
    U16_سیٹ_CP_شروع(array, 0, وتھ);
    أرجع وتھ;
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_Char32_حد_بندی(int32_t وتھ) ثابت {
  int32_t len = length();
  إذا (static_cast<uint32_t>(وتھ) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروع();
    U16_سیٹ_CP_حد_بندی(array, 0, وتھ, len);
    أرجع وتھ;
  } وإلا {
    أرجع len;
  }
}

int32_t
UnicodeString::گنتی_Char32(int32_t شروع, int32_t length) ثابت {
  pinIndices(شروع, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  أرجع u_گنتی_Char32(get_Array_شروع()+شروع, length);
}

UBool
UnicodeString::has_ہور_Char32_Than(int32_t شروع, int32_t length, int32_t number) ثابت {
  pinIndices(شروع, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  أرجع u_str_Has_ہور_Char32_Than(get_Array_شروع()+شروع, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t اشاریہ, int32_t delta) ثابت {
  // pin index
  int32_t len = length();
  إذا(اشاریہ<0) {
    اشاریہ=0;
  } وإلا إذا(اشاریہ>len) {
    اشاریہ=len;
  }

  ثابت char16_t *array = get_Array_شروع();
  إذا(delta>0) {
    U16_FWD_N(array, اشاریہ, len, delta);
  } وإلا {
    U16_پٹھ_N(array, 0, اشاریہ, -delta);
  }

  أرجع اشاریہ;
}

فراغ
UnicodeString::do_کڈھو(int32_t شروع,
             int32_t length,
             char16_t *dst,
             int32_t dst_شروع) ثابت
{
  // pin indices to legal values
  pinIndices(شروع, length);

  // do not copy anything if we alias dst itself
  ثابت char16_t *array = get_Array_شروع();
  إذا(array + شروع != dst + dst_شروع) {
    us_arrayCopy(array, شروع, dst, dst_شروع, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_سمرتھا,
                       UError_کوڈ &error_کوڈ) ثابت {
  int32_t len = length();
  إذا(U_SUCCESS(error_کوڈ)) {
    إذا (isBogus() || dest_سمرتھا < 0 || (dest_سمرتھا > 0 && dest == خالی_سوچک)) {
      error_کوڈ=U_ILLEGAL_دلیل_ERROR;
    } وإلا {
      ثابت char16_t *array = get_Array_شروع();
      إذا(len>0 && len<=dest_سمرتھا && array!=dest) {
        u_memcpy(dest, array, len);
      }
      أرجع u_terminateUChars(dest, dest_سمرتھا, len, &error_کوڈ);
    }
  }

  أرجع len;
}

int32_t
UnicodeString::extract(int32_t شروع,
                       int32_t length,
                       حرف *نشانہ,
                       int32_t نشانہ_سمرتھا,
                       تعداد EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  إذا(نشانہ_سمرتھا < 0 || (نشانہ_سمرتھا > 0 && نشانہ == خالی_سوچک)) {
    أرجع 0;
  }

  // pin the indices to legal values
  pinIndices(شروع, length);

  إذا(length <= نشانہ_سمرتھا) {
    u_UChars_منزل_Chars(get_Array_شروع() + شروع, نشانہ, length);
  }
  UError_کوڈ حالت = U_ZERO_ERROR;
  أرجع u_terminateChars(نشانہ, نشانہ_سمرتھا, length, &حالت);
}

UnicodeString
UnicodeString::عارضی_Sub_String(int32_t شروع, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(شروع, len);
  ثابت char16_t *array = get_بفر();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  إذا(array==خالی_سوچک) {
    array=fUnion.f_ڈھیر_Fields.f_بفر;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  أرجع UnicodeString(خطأ, array + شروع, len);
}

int32_t
UnicodeString::منزل_UTF8(int32_t شروع, int32_t len,
                      حرف *نشانہ, int32_t capacity) ثابت {
  pinIndices(شروع, len);
  int32_t length8;
  UError_کوڈ error_کوڈ = U_ZERO_ERROR;
  u_str_منزل_UTF8_With_Sub(نشانہ, capacity, &length8,
                     get_بفر() + شروع, len,
                     0xFFFD,  // Standard substitution character.
                     خالی_سوچک,    // Don't care about number of substitutions.
                     &error_کوڈ);
  أرجع length8;
}

#إذا U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t شروع, int32_t len,
                       حرف *نشانہ, uint32_t dst_ناپ) ثابت {
  // if the arguments are illegal, then do nothing
  إذا (/*dstSize < 0 || */(dst_ناپ > 0 && نشانہ == خالی_سوچک)) {
    أرجع 0;
  }
  أرجع منزل_UTF8(شروع, len, نشانہ, dst_ناپ <= 0x7fffffff ? static_cast<int32_t>(dst_ناپ) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#جے_انت

فراغ 
UnicodeString::کڈھو_Between(int32_t شروع,
                  int32_t حد_بندی,
                  UnicodeString& نشانہ) ثابت {
  pin_اشاریہ(شروع);
  pin_اشاریہ(حد_بندی);
  do_کڈھو(شروع, حد_بندی - شروع, نشانہ);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
فراغ
UnicodeString::منزل_UTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  إذا(length16 != 0) {
    حرف ڈھیر_بفر[1024];
    int32_t capacity = static_cast<int32_t>(حجم(ڈھیر_بفر));
    UBool utf8IsOwned = خطأ;
    حرف *utf8 = sink.Get_Append_بفر(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ڈھیر_بفر, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_کوڈ error_کوڈ = U_ZERO_ERROR;
    u_str_منزل_UTF8_With_Sub(utf8, capacity, &length8,
                       get_بفر(), length16,
                       0xFFFD,  // Standard substitution character.
                       خالی_سوچک,    // Don't care about number of substitutions.
                       &error_کوڈ);
    إذا(error_کوڈ == U_بفر_حد_توں_ودھ_ERROR) {
      utf8 = static_cast<حرف*>(uprv_malloc(length8));
      إذا(utf8 != خالی_سوچک) {
        utf8IsOwned = سچ;
        error_کوڈ = U_ZERO_ERROR;
        u_str_منزل_UTF8_With_Sub(utf8, length8, &length8,
                           get_بفر(), length16,
                           0xFFFD,  // Standard substitution character.
                           خالی_سوچک,    // Don't care about number of substitutions.
                           &error_کوڈ);
      } وإلا {
        error_کوڈ = U_حافظہ_ونڈ_ERROR;
      }
    }
    إذا(U_SUCCESS(error_کوڈ)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    إذا(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::منزل_UTF32(UChar32 *utf32, int32_t capacity, UError_کوڈ &error_کوڈ) ثابت {
  int32_t length32=0;
  إذا(U_SUCCESS(error_کوڈ)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_منزل_UTF32_With_Sub(utf32, capacity, &length32,
        get_بفر(), length(),
        0xfffd,  // Substitution character.
        خالی_سوچک,    // Don't care about number of substitutions.
        &error_کوڈ);
  }
  أرجع length32;
}

int32_t 
UnicodeString::اشاریہ_دا(ثابت char16_t *srcChars,
               int32_t src_شروع,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == خالی_سوچک || src_شروع < 0 || src_لمائی == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_لمائی < 0 && srcChars[src_شروع] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروع, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_str_لبھو_پہلا(array + شروع, length, srcChars + src_شروع, src_لمائی);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

int32_t
UnicodeString::do_اشاریہ_دا(char16_t c,
             int32_t شروع,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(شروع, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_memchr(array + شروع, c, length);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

int32_t
UnicodeString::do_اشاریہ_دا(UChar32 c,
                         int32_t شروع,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(شروع, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_memchr32(array + شروع, c, length);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

int32_t 
UnicodeString::آخری_اشاریہ_دا(ثابت char16_t *srcChars,
               int32_t src_شروع,
               int32_t src_لمائی,
               int32_t شروع,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == خالی_سوچک || src_شروع < 0 || src_لمائی == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_لمائی < 0 && srcChars[src_شروع] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروع, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_str_لبھو_آخری(array + شروع, length, srcChars + src_شروع, src_لمائی);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

int32_t
UnicodeString::do_آخری_اشاریہ_دا(char16_t c,
                 int32_t شروع,
                 int32_t length) ثابت
{
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices
  pinIndices(شروع, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_memrchr(array + شروع, c, length);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

int32_t
UnicodeString::do_آخری_اشاریہ_دا(UChar32 c,
                             int32_t شروع,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(شروع, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *میل = u_memrchr32(array + شروع, c, length);
  إذا(میل == خالی_سوچک) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(میل - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::لبھو_And_Replace(int32_t شروع,
                              int32_t length,
                              ثابت UnicodeString& old_متن,
                              int32_t old_شروع,
                              int32_t old_لمائی,
                              ثابت UnicodeString& new_متن,
                              int32_t new_شروع,
                              int32_t new_لمائی) U_LIFETIME_BOUND
{
  إذا(isBogus() || old_متن.isBogus() || new_متن.isBogus()) {
    أرجع *هذا;
  }

  pinIndices(شروع, length);
  old_متن.pinIndices(old_شروع, old_لمائی);
  new_متن.pinIndices(new_شروع, new_لمائی);

  إذا(old_لمائی == 0) {
    أرجع *هذا;
  }

  طالما(length > 0 && length >= old_لمائی) {
    int32_t pos = اشاریہ_دا(old_متن, old_شروع, old_لمائی, شروع, length);
    إذا(pos < 0) {
      // no more oldText's here: done
      اكسر;
    } وإلا {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_لمائی, new_متن, new_شروع, new_لمائی);
      length -= pos + old_لمائی - شروع;
      شروع = pos + new_لمائی;
    }
  }

  أرجع *هذا;
}


فراغ
UnicodeString::سیٹ_منزل_Bogus()
{
  releaseArray();

  fUnion.fFields.f_لمائی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالی_سوچک;
  fUnion.fFields.f_سمرتھا = 0;
}

// turn a bogus string into an empty one
فراغ
UnicodeString::unBogus() {
  إذا(fUnion.fFields.f_لمائی_And_Flags & kIsBogus) {
    سیٹ_منزل_خالی_تھاواں();
  }
}

ثابت char16_t*
UnicodeString::get_Terminated_بفر() U_LIFETIME_BOUND {
  إذا(!isWritable()) {
    أرجع خالی_سوچک;
  }
  char16_t *array = get_Array_شروع();
  int32_t len = length();
  إذا(len < get_سمرتھا()) {
    إذا(fUnion.fFields.f_لمائی_And_Flags & k_بفر_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      إذا(array[len] == 0) {
        أرجع array;
      }
    } وإلا إذا(((fUnion.fFields.f_لمائی_And_Flags & kRefCounted) == 0 || ref_گنتی() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      أرجع array;
    }
  }
  إذا(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = get_Array_شروع();
    array[len] = 0;
    أرجع array;
  } وإلا {
    أرجع خالی_سوچک;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سیٹ_منزل(UBool isTerminated,
                     ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                     int32_t متن_لمائی) U_LIFETIME_BOUND
{
  إذا(fUnion.fFields.f_لمائی_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == خالی_سوچک) {
    // treat as an empty string, do not alias
    releaseArray();
    سیٹ_منزل_خالی_تھاواں();
    أرجع *هذا;
  }

  إذا( متن_لمائی < -1 ||
      (متن_لمائی == -1 && !isTerminated) ||
      (متن_لمائی >= 0 && isTerminated && متن[متن_لمائی] != 0)
  ) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  }

  releaseArray();

  إذا(متن_لمائی == -1) {
    // text is terminated, or else it would have failed the above test
    متن_لمائی = u_strlen(متن);
  }
  fUnion.fFields.f_لمائی_And_Flags = kReadonlyAlias;
  سیٹ_Array(const_cast<char16_t*>(متن), متن_لمائی, isTerminated ? متن_لمائی + 1 : متن_لمائی);
  أرجع *هذا;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سیٹ_منزل(char16_t* بفر,
                     int32_t buff_لمائی U_LIFETIME_BOUND,
                     int32_t buff_سمرتھا) U_LIFETIME_BOUND {
  إذا(fUnion.fFields.f_لمائی_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  إذا(بفر == خالی_سوچک) {
    // treat as an empty string, do not alias
    releaseArray();
    سیٹ_منزل_خالی_تھاواں();
    أرجع *هذا;
  }

  إذا(buff_لمائی < -1 || buff_سمرتھا < 0 || buff_لمائی > buff_سمرتھا) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  } وإلا إذا(buff_لمائی == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = بفر, *حد_بندی = بفر + buff_سمرتھا;
    طالما(p != حد_بندی && *p != 0) {
      ++p;
    }
    buff_لمائی = static_cast<int32_t>(p - بفر);
  }

  releaseArray();

  fUnion.fFields.f_لمائی_And_Flags = kWritableAlias;
  سیٹ_Array(بفر, buff_لمائی, buff_سمرتھا);
  أرجع *هذا;
}

UnicodeString& UnicodeString::سیٹ_منزل_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  إذا(length <= US_STACKBUF_ناپ) {
    capacity = US_STACKBUF_ناپ;
  } وإلا {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_بفر(capacity);
  int32_t length16;
  UError_کوڈ error_کوڈ = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_سمرتھا(), &length16,
      utf8.انکڑے(), length,
      0xfffd,  // Substitution character.
      خالی_سوچک,    // Don't care about number of substitutions.
      &error_کوڈ);
  release_بفر(length16);
  إذا(U_FAILURE(error_کوڈ)) {
    سیٹ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::سیٹ_Char_At(int32_t وتھ, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  إذا(cloneArrayIfNeeded() && len > 0) {
    إذا(وتھ < 0) {
      وتھ = 0;
    } وإلا إذا(وتھ >= len) {
      وتھ = len - 1;
    }

    get_Array_شروع()[وتھ] = c;
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t لمائی_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_لمائی];
  int32_t count = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, count, U16_MAX_لمائی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  أرجع doReplace(شروع, لمائی_4, بفر, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_لمائی];
  int32_t لمائی_4 = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, لمائی_4, U16_MAX_لمائی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  أرجع isError ? *هذا : doAppend(بفر, 0, لمائی_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t src_شروع,
                         int32_t src_لمائی) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_شروع, src_لمائی);

  // get the characters from src
  // and replace the range in ourselves with them
  أرجع doReplace(شروع, length, src.get_Array_شروع(), src_شروع, src_لمائی);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t src_شروع,
                         int32_t src_لمائی) U_LIFETIME_BOUND
{
  إذا(!isWritable()) {
    أرجع *هذا;
  }

  int32_t old_لمائی = هذا->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  إذا((fUnion.fFields.f_لمائی_And_Flags&k_بفر_Is_Readonly) && src_لمائی == 0) {
    إذا(شروع == 0) {
      // remove prefix by adjusting the array pointer
      pin_اشاریہ(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_سمرتھا -= length;
      سیٹ_لمائی(old_لمائی - length);
      أرجع *هذا;
    } وإلا {
      pin_اشاریہ(شروع);
      إذا(length >= (old_لمائی - شروع)) {
        // remove suffix by reducing the length (like truncate())
        سیٹ_لمائی(شروع);
        fUnion.fFields.f_سمرتھا = شروع;  // not NUL-terminated any more
        أرجع *هذا;
      }
    }
  }

  إذا(شروع == old_لمائی) {
    أرجع doAppend(srcChars, src_شروع, src_لمائی);
  }

  إذا (srcChars == خالی_سوچک) {
    src_لمائی = 0;
  } وإلا {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_شروع;
    إذا (src_لمائی < 0) {
      // get the srcLength if necessary
      src_لمائی = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(شروع, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_لمائی = old_لمائی - length;
  إذا(src_لمائی > (INT32_MAX - new_لمائی)) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  }
  new_لمائی += src_لمائی;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = get_Array_شروع();
  إذا (is_بفر_Writable() &&
      oldArray < srcChars + src_لمائی &&
      srcChars < oldArray + old_لمائی) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_لمائی);
    إذا (copy.isBogus()) {
      سیٹ_منزل_Bogus();
      أرجع *هذا;
    }
    أرجع doReplace(شروع, length, copy.get_Array_شروع(), 0, src_لمائی);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ڈھیر_بفر[US_STACKBUF_ناپ];
  إذا((fUnion.fFields.f_لمائی_And_Flags&k_Using_ڈھیر_بفر) && (new_لمائی > US_STACKBUF_ناپ)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ڈھیر_بفر, oldArray, old_لمائی);
    oldArray = old_ڈھیر_بفر;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *بفر_منزل_Delete = خالی_سوچک;
  إذا(!cloneArrayIfNeeded(new_لمائی, get_Grow_سمرتھا(new_لمائی),
                         خطأ, &بفر_منزل_Delete)
  ) {
    أرجع *هذا;
  }

  // now do the replace

  char16_t *newArray = get_Array_شروع();
  إذا(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, شروع);
    us_arrayCopy(oldArray, شروع + length,
                 newArray, شروع + src_لمائی,
                 old_لمائی - (شروع + length));
  } وإلا إذا(length != src_لمائی) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, شروع + length,
                 newArray, شروع + src_لمائی,
                 old_لمائی - (شروع + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, شروع, src_لمائی);

  سیٹ_لمائی(new_لمائی);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doReplace(شروع, length, src.انکڑے(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t src_شروع,
                        int32_t src_لمائی) U_LIFETIME_BOUND {
  إذا(src_لمائی == 0) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  src.pinIndices(src_شروع, src_لمائی);
  أرجع doAppend(src.get_Array_شروع(), src_شروع, src_لمائی);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t src_شروع,
                        int32_t src_لمائی) U_LIFETIME_BOUND {
  إذا(!isWritable() || src_لمائی == 0 || srcChars == خالی_سوچک) {
    أرجع *هذا;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_شروع;

  إذا(src_لمائی < 0) {
    // get the srcLength if necessary
    إذا((src_لمائی = u_strlen(srcChars)) == 0) {
      أرجع *هذا;
    }
  }

  int32_t old_لمائی = length();
  int32_t new_لمائی;

  إذا (src_لمائی <= get_سمرتھا() - old_لمائی && is_بفر_Writable()) {
    new_لمائی = old_لمائی + src_لمائی;
    // Faster than a memmove
    إذا (src_لمائی <= 4) {
      char16_t *arr = get_Array_شروع();
      arr[old_لمائی] = srcChars[0];
      إذا (src_لمائی > 1) arr[old_لمائی+1] = srcChars[1];
      إذا (src_لمائی > 2) arr[old_لمائی+2] = srcChars[2];
      إذا (src_لمائی > 3) arr[old_لمائی+3] = srcChars[3];
      سیٹ_لمائی(new_لمائی);
      أرجع *هذا;
    }
  } وإلا {
    إذا (uprv_add32_حد_توں_ودھ(old_لمائی, src_لمائی, &new_لمائی)) {
      سیٹ_منزل_Bogus();
      أرجع *هذا;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = get_Array_شروع();
    إذا (is_بفر_Writable() &&
        oldArray < srcChars + src_لمائی &&
        srcChars < oldArray + old_لمائی) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_لمائی);
      إذا (copy.isBogus()) {
        سیٹ_منزل_Bogus();
        أرجع *هذا;
      }
      أرجع doAppend(copy.get_Array_شروع(), 0, src_لمائی);
    }

    // optimize append() onto a large-enough, owned string
    إذا (!cloneArrayIfNeeded(new_لمائی, get_Grow_سمرتھا(new_لمائی))) {
      أرجع *هذا;
    }
  }

  char16_t *newArray = get_Array_شروع();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  إذا(srcChars != newArray + old_لمائی) {
    us_arrayCopy(srcChars, 0, newArray, old_لمائی, src_لمائی);
  }
  سیٹ_لمائی(new_لمائی);

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable() || src.empty()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سیٹ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doAppend(src.انکڑے(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
فراغ
UnicodeString::handleReplaceBetween(int32_t شروع,
                                    int32_t حد_بندی,
                                    ثابت UnicodeString& متن) {
    replaceBetween(شروع, حد_بندی, متن);
}

/**
 * Replaceable API
 */
فراغ 
UnicodeString::copy(int32_t شروع, int32_t حد_بندی, int32_t dest) {
    إذا (حد_بندی <= شروع) {
        أرجع; // Nothing to do; avoid bogus malloc call
    }
    char16_t* متن = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * (حد_بندی - شروع)));
    // Check to make sure text is not null.
    إذا (متن != خالی_سوچک) {
	    کڈھو_Between(شروع, حد_بندی, متن, 0);
	    insert(dest, متن, 0, حد_بندی - شروع);    
	    uprv_free(متن);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_انکڑے() ثابت {
    أرجع سچ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_انکڑے() ثابت {
    أرجع خطأ;
}

UnicodeString&
UnicodeString::do_الٹ(int32_t شروع, int32_t length) U_LIFETIME_BOUND {
  إذا(length <= 1 || !cloneArrayIfNeeded()) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  pinIndices(شروع, length);
  إذا(length <= 1) {  // pinIndices() might have shrunk the length
    أرجع *هذا;
  }

  char16_t *کھبے = get_Array_شروع() + شروع;
  char16_t *سجے = کھبے + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = خطأ;

  // Before the loop we know left<right because length>=2.
  افعل {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *کھبے));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کھبے++ = *سجے));
    *سجے-- = swap;
  } طالما(کھبے < سجے);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کھبے));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  إذا(hasSupplementary) {
    char16_t swap2;

    کھبے = get_Array_شروع() + شروع;
    سجے = کھبے + length - 1; // -1 so that we can look at *(left+1) if left<right
    طالما(کھبے < سجے) {
      إذا(U16_IS_TRAIL(swap = *کھبے) && U16_IS_LEAD(swap2 = *(کھبے + 1))) {
        *کھبے++ = swap2;
        *کھبے++ = swap;
      } وإلا {
        ++کھبے;
      }
    }
  }

  أرجع *هذا;
}

UBool 
UnicodeString::padLeading(int32_t نشانہ_لمائی,
                          char16_t padChar)
{
  int32_t old_لمائی = length();
  إذا(old_لمائی >= نشانہ_لمائی || !cloneArrayIfNeeded(نشانہ_لمائی)) {
    أرجع خطأ;
  } وإلا {
    // move contents up by padding width
    char16_t *array = get_Array_شروع();
    int32_t شروع = نشانہ_لمائی - old_لمائی;
    us_arrayCopy(array, 0, array, شروع, old_لمائی);

    // fill in padding character
    طالما(--شروع >= 0) {
      array[شروع] = padChar;
    }
    سیٹ_لمائی(نشانہ_لمائی);
    أرجع سچ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t نشانہ_لمائی,
                           char16_t padChar)
{
  int32_t old_لمائی = length();
  إذا(old_لمائی >= نشانہ_لمائی || !cloneArrayIfNeeded(نشانہ_لمائی)) {
    أرجع خطأ;
  } وإلا {
    // fill in padding character
    char16_t *array = get_Array_شروع();
    int32_t length = نشانہ_لمائی;
    طالما(--length >= old_لمائی) {
      array[length] = padChar;
    }
    سیٹ_لمائی(نشانہ_لمائی);
    أرجع سچ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_کوڈ() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_کوڈ = ustr_hashUCharsN(get_Array_شروع(), length());
    إذا (hash_کوڈ == k_Invalid_Hash_کوڈ) {
        hash_کوڈ = k_خالی_تھاواں_Hash_کوڈ;
    }
    أرجع hash_کوڈ;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_بفر(int32_t min_سمرتھا) U_LIFETIME_BOUND {
  إذا(min_سمرتھا>=-1 && cloneArrayIfNeeded(min_سمرتھا)) {
    fUnion.fFields.f_لمائی_And_Flags|=k_Open_Get_بفر;
    سیٹ_Zero_لمائی();
    أرجع get_Array_شروع();
  } وإلا {
    أرجع خالی_سوچک;
  }
}

فراغ
UnicodeString::release_بفر(int32_t new_لمائی) {
  إذا(fUnion.fFields.f_لمائی_And_Flags&k_Open_Get_بفر && new_لمائی>=-1) {
    // set the new fLength
    int32_t capacity=get_سمرتھا();
    إذا(new_لمائی==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=get_Array_شروع(), *p=array, *حد_بندی=array+capacity;
      طالما(p<حد_بندی && *p!=0) {
        ++p;
      }
      new_لمائی = static_cast<int32_t>(p - array);
    } وإلا إذا(new_لمائی>capacity) {
      new_لمائی=capacity;
    }
    سیٹ_لمائی(new_لمائی);
    fUnion.fFields.f_لمائی_And_Flags&=~k_Open_Get_بفر;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_سمرتھا,
                                  int32_t grow_سمرتھا,
                                  UBool doCopyArray,
                                  int32_t **p_بفر_منزل_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  إذا(new_سمرتھا == -1) {
    new_سمرتھا = get_سمرتھا();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  إذا(!isWritable()) {
    أرجع خطأ;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  إذا(forceClone ||
     fUnion.fFields.f_لمائی_And_Flags & k_بفر_Is_Readonly ||
     (fUnion.fFields.f_لمائی_And_Flags & kRefCounted && ref_گنتی() > 1) ||
     new_سمرتھا > get_سمرتھا()
  ) {
    // check growCapacity for default value and use of the stack buffer
    إذا(grow_سمرتھا < 0) {
      grow_سمرتھا = new_سمرتھا;
    } وإلا إذا(new_سمرتھا <= US_STACKBUF_ناپ && grow_سمرتھا > US_STACKBUF_ناپ) {
      grow_سمرتھا = US_STACKBUF_ناپ;
    } وإلا إذا(new_سمرتھا > grow_سمرتھا) {
      سیٹ_منزل_Bogus();
      أرجع خطأ;  // bad inputs
    }
    إذا(grow_سمرتھا > k_Max_سمرتھا) {
      سیٹ_منزل_Bogus();
      أرجع خطأ;
    }

    // save old values
    char16_t old_ڈھیر_بفر[US_STACKBUF_ناپ];
    char16_t *oldArray;
    int32_t old_لمائی = length();
    int16_t flags = fUnion.fFields.f_لمائی_And_Flags;

    إذا(flags&k_Using_ڈھیر_بفر) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      إذا(doCopyArray && grow_سمرتھا > US_STACKBUF_ناپ) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ڈھیر_Fields.f_بفر, 0, old_ڈھیر_بفر, 0, old_لمائی);
        oldArray = old_ڈھیر_بفر;
      } وإلا {
        oldArray = خالی_سوچک; // no need to copy from the stack buffer to itself
      }
    } وإلا {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=خالی_سوچک); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    إذا(allocate(grow_سمرتھا) ||
       (new_سمرتھا < grow_سمرتھا && allocate(new_سمرتھا))
    ) {
      إذا(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_لمائی = old_لمائی;
        new_سمرتھا = get_سمرتھا();
        إذا(new_سمرتھا < min_لمائی) {
          min_لمائی = new_سمرتھا;
        }
        إذا(oldArray != خالی_سوچک) {
          us_arrayCopy(oldArray, 0, get_Array_شروع(), 0, min_لمائی);
        }
        سیٹ_لمائی(min_لمائی);
      } وإلا {
        سیٹ_Zero_لمائی();
      }

      // release the old array
      إذا(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_گنتی = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        إذا(umtx_atomic_dec(p_Ref_گنتی) == 0) {
          إذا (p_بفر_منزل_Delete == خالی_سوچک) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((فراغ *)p_Ref_گنتی);
          } وإلا {
            // the caller requested to delete it himself
            *p_بفر_منزل_Delete = reinterpret_cast<int32_t*>(p_Ref_گنتی);
          }
        }
      }
    } وإلا {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      إذا(!(flags&k_Using_ڈھیر_بفر)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_لمائی_And_Flags = flags;
      سیٹ_منزل_Bogus();
      أرجع خطأ;
    }
  }
  أرجع سچ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_کوڈ_Unit(char16_t c) {
  أرجع str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_کوڈ_نقطہ(UChar32 c) {
  char16_t بفر[U16_MAX_لمائی];
  int32_t c_لمائی = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, c_لمائی, U16_MAX_لمائی, c, isError);
  أرجع !isError && str.doAppend(بفر, 0, c_لمائی).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  أرجع str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_سمرتھا(int32_t append_سمرتھا) {
  أرجع str.cloneArrayIfNeeded(str.length() + append_سمرتھا);
}

char16_t* UnicodeStringAppendable::get_Append_بفر(int32_t min_سمرتھا,
                                                   int32_t desired_سمرتھا_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_سمرتھا,
                                                   int32_t* نتیجہ_سمرتھا) U_LIFETIME_BOUND {
  إذا(min_سمرتھا < 1 || scratch_سمرتھا < min_سمرتھا) {
    *نتیجہ_سمرتھا = 0;
    أرجع خالی_سوچک;
  }
  int32_t old_لمائی = str.length();
  إذا(min_سمرتھا <= (k_Max_سمرتھا - old_لمائی) &&
      desired_سمرتھا_Hint <= (k_Max_سمرتھا - old_لمائی) &&
      str.cloneArrayIfNeeded(old_لمائی + min_سمرتھا, old_لمائی + desired_سمرتھا_Hint)) {
    *نتیجہ_سمرتھا = str.get_سمرتھا() - old_لمائی;
    أرجع str.get_Array_شروع() + old_لمائی;
  }
  *نتیجہ_سمرتھا = scratch_سمرتھا;
  أرجع scratch;
}

U_NAMESPACE_سرا

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement کلید) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.اشارہ_گر;
    أرجع (str == خالی_سوچک) ? 0 : str->hash_کوڈ();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارہ_گر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارہ_گر;
    إذا (str1 == str2) {
        أرجع سچ;
    }
    إذا (str1 == خالی_سوچک || str2 == خالی_سوچک) {
        أرجع خطأ;
    }
    أرجع *str1 == *str2;
}

#جے_تعریف_شدہ U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#إذا defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#ہدایت GCC diagnostic push
#ہدایت GCC diagnostic ignored "-Wunused-function"
ساكن فراغ uprv_UnicodeStringDummy() {
    مٹاؤ [] (جديد UnicodeString[2]);
}
#ہدایت GCC diagnostic pop
#جے_انت
#جے_انت
