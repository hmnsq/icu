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
استعمال نطاق std;

//DEBUGGING
فراغ
print(ثابت UnicodeString& s,
      ثابت حرف *name)
{
  char16_t c;
  ٻاهر << name << ":|";
  لكل(صحيح i = 0; i < s.length(); ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      ٻاهر << "[0x" << hex << s[i] << "]";
    وإلا
      ٻاهر << (حرف) s[i];
  }
  ٻاهر << '|' << سٽ_پڄاڻي;
}

فراغ
print(ثابت char16_t *s,
      int32_t len,
      ثابت حرف *name)
{
  char16_t c;
  ٻاهر << name << ":|";
  لكل(صحيح i = 0; i < len; ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      ٻاهر << "[0x" << hex << s[i] << "]";
    وإلا
      ٻاهر << (حرف) s[i];
  }
  ٻاهر << '|' << سٽ_پڄاڻي;
}
// END DEBUGGING
#جيڪڏهن_پورو

// Local function definitions for now

// need to copy areas that may overlap
ساكن
مضمن فراغ
us_arrayCopy(ثابت char16_t *src, int32_t src_شروعات,
         char16_t *dst, int32_t dst_شروعات, int32_t count)
{
  إذا(count>0) {
    uprv_memmove(dst+dst_شروعات, src+src_شروعات, (size_t)count*حجم(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ساكن char16_t U_CALLCONV
UnicodeString_charAt(int32_t وٿي, فراغ *context) {
    أرجع ((icu::UnicodeString*) context)->charAt(وٿي);
}
U_CDECL_پڇاڙي

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
عامل+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t جوڙ_Lengths;
  إذا (uprv_add32_حد_کان_مٿي(s1.length(), s2.length(), &جوڙ_Lengths)) {
    UnicodeString bogus;
    bogus.سيٽ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (جوڙ_Lengths != INT32_MAX) {
    ++جوڙ_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جوڙ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_اندروني_Concat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t جوڙ_Lengths;
  إذا (s2.length() > INT32_MAX ||
      uprv_add32_حد_کان_مٿي(s1.length(), static_cast<int32_t>(s2.length()), &جوڙ_Lengths)) {
    UnicodeString bogus;
    bogus.سيٽ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (جوڙ_Lengths != INT32_MAX) {
    ++جوڙ_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جوڙ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

فراغ
UnicodeString::جوڙيو_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  أرجع umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_ڳڻپ() ثابت {
  أرجع umtx_ڀريو_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

فراغ
UnicodeString::releaseArray() {
  إذا((fUnion.fFields.f_ڊيگهائي_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = 0;
  إذا (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } وإلا إذا(c <= 0xffff) {
    int32_t length = count;
    إذا(capacity < length) {
      capacity = length;
    }
    إذا(allocate(capacity)) {
      char16_t *array = get_Array_شروعات();
      char16_t unit = static_cast<char16_t>(c);
      لكل(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      سيٽ_ڊيگهائي(length);
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
      char16_t *array = get_Array_شروعات();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      لكل(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      سيٽ_ڊيگهائي(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kLength1 | kShortString;
  fUnion.f_ڍير_Fields.f_بفر[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = خطأ;
  U16_APPEND(fUnion.f_ڍير_Fields.f_بفر, i, US_STACKBUF_سائيز, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  إذا(!isError) {
    سيٽ_Short_ڊيگهائي(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *متن,
                             int32_t متن_ڊيگهائي) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  doAppend(متن, 0, متن_ڊيگهائي);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                             int32_t متن_ڊيگهائي) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kReadonlyAlias;
  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == خالي_اشارو) {
    // treat as an empty string, do not alias
    سيٽ_منزل_خالي_جايون();
  } وإلا إذا(متن_ڊيگهائي < -1 ||
            (متن_ڊيگهائي == -1 && !isTerminated) ||
            (متن_ڊيگهائي >= 0 && isTerminated && متن[متن_ڊيگهائي] != 0)
  ) {
    سيٽ_منزل_Bogus();
  } وإلا {
    إذا(متن_ڊيگهائي == -1) {
      // text is terminated, or else it would have failed the above test
      متن_ڊيگهائي = u_strlen(متن);
    }
    سيٽ_Array(const_cast<char16_t *>(متن), متن_ڊيگهائي,
             isTerminated ? متن_ڊيگهائي + 1 : متن_ڊيگهائي);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_ڊيگهائي,
                             int32_t buff_صلاحيت) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kWritableAlias;
  إذا(buff == خالي_اشارو) {
    // treat as an empty string, do not alias
    سيٽ_منزل_خالي_جايون();
  } وإلا إذا(buff_ڊيگهائي < -1 || buff_صلاحيت < 0 || buff_ڊيگهائي > buff_صلاحيت) {
    سيٽ_منزل_Bogus();
  } وإلا {
    إذا(buff_ڊيگهائي == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *حد_بندي = buff + buff_صلاحيت;
      طالما(p != حد_بندي && *p != 0) {
        ++p;
      }
      buff_ڊيگهائي = static_cast<int32_t>(p - buff);
    }
    سيٽ_Array(buff, buff_ڊيگهائي, buff_صلاحيت);
  }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t length, EInvariant) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  إذا(src==خالي_اشارو) {
    // treat as an empty string
  } وإلا {
    إذا(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    إذا(cloneArrayIfNeeded(length, length, خطأ)) {
      u_chars_منزل_UChars(src, get_Array_شروعات(), length);
      سيٽ_ڊيگهائي(length);
    } وإلا {
      سيٽ_منزل_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND) {
  UnicodeString نتيجو;
  إذا (متن.length() <= INT32_MAX) {
    نتيجو.سيٽ_منزل(خطأ, متن.معلومات(), static_cast<int32_t>(متن.length()));
  } وإلا {
    نتيجو.سيٽ_منزل_Bogus();
  }
  أرجع نتيجو;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
  UnicodeString نتيجو;
  إذا (متن.isBogus()) {
    نتيجو.سيٽ_منزل_Bogus();
  } وإلا {
    نتيجو.سيٽ_منزل(خطأ, متن.get_بفر(), متن.length());
  }
  أرجع نتيجو;
}

#إذا U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  إذا (codepage_معلومات != خالي_اشارو) {
    سيٽ_منزل_UTF8(codepage_معلومات);
  }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات, int32_t معلومات_ڊيگهائي) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  إذا (codepage_معلومات == خالي_اشارو || معلومات_ڊيگهائي == 0 || معلومات_ڊيگهائي < -1) {
    أرجع;
  }
  إذا(معلومات_ڊيگهائي == -1) {
    معلومات_ڊيگهائي = static_cast<int32_t>(uprv_strlen(codepage_معلومات));
  }
  سيٽ_منزل_UTF8(StringPiece(codepage_معلومات, معلومات_ڊيگهائي));
}

// else see unistr_cnv.cpp
#جيڪڏهن_پورو

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, سچ);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروعات) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  سيٽ_منزل(that, src_شروعات);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروعات,
                             int32_t src_ڊيگهائي) {
  fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
  سيٽ_منزل(that, src_شروعات, src_ڊيگهائي);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  أرجع خالي_اشارو;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  Local_اشارو<UnicodeString> clonedString(جديد UnicodeString(*هذا));
  أرجع clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : خالي_اشارو;
}

//========================================
// array allocation
//========================================

نطاق {

ثابت int32_t k_Grow_سائيز = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ثابت int32_t k_Max_صلاحيت = 0x7ffffff5;

int32_t get_Grow_صلاحيت(int32_t new_ڊيگهائي) {
  int32_t grow_سائيز = (new_ڊيگهائي >> 2) + k_Grow_سائيز;
  إذا(grow_سائيز <= (k_Max_صلاحيت - new_ڊيگهائي)) {
    أرجع new_ڊيگهائي + grow_سائيز;
  } وإلا {
    أرجع k_Max_صلاحيت;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  إذا(capacity <= US_STACKBUF_سائيز) {
    fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
    أرجع سچ;
  }
  إذا(capacity <= k_Max_صلاحيت) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t انگ_بائيٽون = حجم(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    انگ_بائيٽون = (انگ_بائيٽون + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(انگ_بائيٽون));
    إذا(array != خالي_اشارو) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      انگ_بائيٽون -= حجم(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_صلاحيت = static_cast<int32_t>(انگ_بائيٽون / U_SIZEOF_UCHAR);
      fUnion.fFields.f_ڊيگهائي_And_Flags = kLongString;
      أرجع سچ;
    }
  }
  fUnion.fFields.f_ڊيگهائي_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالي_اشارو;
  fUnion.fFields.f_صلاحيت = 0;
  أرجع خطأ;
}

//========================================
// Destructor
//========================================

#جيڪڏهن_وصف_ٿيل UNISTR_ڳڻپ_FINAL_STRING_LENGTHS
ساكن u_atomic_int32_t final_ڊيگهائي_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ساكن u_atomic_int32_t beyond_ڳڻپ(0);

U_CAPI فراغ unistr_printLengths() {
  int32_t i;
  لكل(i = 0; i <= 59; ++i) {
    لکو("%2d,  %9d\n", i, (int32_t)final_ڊيگهائي_Counts[i]);
  }
  int32_t beyond = beyond_ڳڻپ;
  لكل(; i < UPRV_LENGTHOF(final_ڊيگهائي_Counts); ++i) {
    beyond += final_ڊيگهائي_Counts[i];
  }
  لکو(">59, %9d\n", beyond);
}
#جيڪڏهن_پورو

UnicodeString::~UnicodeString()
{
#جيڪڏهن_وصف_ٿيل UNISTR_ڳڻپ_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  إذا((fUnion.fFields.f_ڊيگهائي_And_Flags&(k_Open_Get_بفر|kReadonlyAlias|kWritableAlias)) == 0) {
    إذا(has_Short_ڊيگهائي()) {
      umtx_atomic_inc(final_ڊيگهائي_Counts + get_Short_ڊيگهائي());
    } وإلا {
      umtx_atomic_inc(&beyond_ڳڻپ);
    }
  }
#جيڪڏهن_پورو

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString نتيجو;
  نتيجو.سيٽ_منزل_UTF8(utf8);
  أرجع نتيجو;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString نتيجو;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  إذا(length <= US_STACKBUF_سائيز) {
    capacity = US_STACKBUF_سائيز;
  } وإلا {
    capacity = length + (length >> 4) + 4;
  }
  افعل {
    char16_t *utf16 = نتيجو.get_بفر(capacity);
    int32_t length16;
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, نتيجو.get_صلاحيت(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        خالي_اشارو,    // Don't care about number of substitutions.
        &error_ڪوڊ);
    نتيجو.release_بفر(length16);
    إذا(error_ڪوڊ == U_بفر_حد_کان_مٿي_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      استمر;
    } وإلا إذا(U_FAILURE(error_ڪوڊ)) {
      نتيجو.سيٽ_منزل_Bogus();
    }
    اكسر;
  } طالما(سچ);
  أرجع نتيجو;
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
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  }

  // delete the current contents
  releaseArray();

  إذا(src.is_خالي_جايون()) {
    // empty string - use the stack buffer
    سيٽ_منزل_خالي_جايون();
    أرجع *هذا;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_ڊيگهائي_And_Flags = src.fUnion.fFields.f_ڊيگهائي_And_Flags;
  بدل(src.fUnion.fFields.f_ڊيگهائي_And_Flags & kAllStorageFlags) {
  حالة kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ڍير_Fields.f_بفر, src.fUnion.f_ڍير_Fields.f_بفر,
                get_Short_ڊيگهائي() * U_SIZEOF_UCHAR);
    اكسر;
  حالة kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).جوڙيو_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_صلاحيت = src.fUnion.fFields.f_صلاحيت;
    إذا(!has_Short_ڊيگهائي()) {
      fUnion.fFields.f_ڊيگهائي = src.fUnion.fFields.f_ڊيگهائي;
    }
    اكسر;
  حالة kReadonlyAlias:
    إذا(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_صلاحيت = src.fUnion.fFields.f_صلاحيت;
      إذا(!has_Short_ڊيگهائي()) {
        fUnion.fFields.f_ڊيگهائي = src.fUnion.fFields.f_ڊيگهائي;
      }
      اكسر;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالة kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_ڊيگهائي = src.length();
    إذا(allocate(src_ڊيگهائي)) {
      u_memcpy(get_Array_شروعات(), src.get_Array_شروعات(), src_ڊيگهائي);
      سيٽ_ڊيگهائي(src_ڊيگهائي);
      اكسر;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  افتراضي:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_ڊيگهائي_And_Flags = kIsBogus;
    fUnion.fFields.fArray = خالي_اشارو;
    fUnion.fFields.f_صلاحيت = 0;
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
فراغ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool سيٽ_Src_منزل_Bogus) noexcept {
  int16_t ڊيگهائي_And_Flags = fUnion.fFields.f_ڊيگهائي_And_Flags = src.fUnion.fFields.f_ڊيگهائي_And_Flags;
  إذا(ڊيگهائي_And_Flags & k_Using_ڍير_بفر) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    إذا(هذا != &src) {
      uprv_memcpy(fUnion.f_ڍير_Fields.f_بفر, src.fUnion.f_ڍير_Fields.f_بفر,
                  get_Short_ڊيگهائي() * U_SIZEOF_UCHAR);
    }
  } وإلا {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_صلاحيت = src.fUnion.fFields.f_صلاحيت;
    إذا(!has_Short_ڊيگهائي()) {
      fUnion.fFields.f_ڊيگهائي = src.fUnion.fFields.f_ڊيگهائي;
    }
    إذا(سيٽ_Src_منزل_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_ڊيگهائي_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = خالي_اشارو;
      src.fUnion.fFields.f_صلاحيت = 0;
    }
  }
}

فراغ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString عارضي;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  عارضي.copyFieldsFrom(*هذا, خطأ);
  هذا->copyFieldsFrom(other, خطأ);
  other.copyFieldsFrom(عارضي, خطأ);
  // Set temp to an empty string so that other's memory is not released twice.
  عارضي.fUnion.fFields.f_ڊيگهائي_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString نتيجو(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    إذا (نتيجو.isBogus()) {
        أرجع نتيجو;
    }
    ثابت char16_t *array = get_بفر();
    int32_t len = length();
    int32_t prev = 0;
    لكل (int32_t i=0;;) {
        إذا (i == len) {
            نتيجو.append(array, prev, len - prev);
            اكسر;
        }
        إذا (array[i++] == 0x5C /*'\\'*/) {
            نتيجو.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            إذا (c < 0) {
                نتيجو.remove(); // return empty string
                اكسر; // invalid escape sequence
            }
            نتيجو.append(c);
            prev = i;
        }
    }
    أرجع نتيجو;
}

UChar32 UnicodeString::unescapeAt(int32_t &وٿي) ثابت {
    أرجع u_unescapeAt(UnicodeString_charAt, &وٿي, length(), (فراغ*)هذا);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ثابت char16_t *متن, int32_t len) ثابت {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  أرجع uprv_memcmp(get_Array_شروعات(), متن, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t شروعات,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_شروعات,
              int32_t src_ڊيگهائي) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع خطأ;
  }
  
  // pin indices to legal values
  pinIndices(شروعات, length);

  إذا(srcChars == خالي_اشارو) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? سچ : خطأ;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروعات();

  chars += شروعات;
  srcChars += src_شروعات;

  // get the srcLength if necessary
  إذا(src_ڊيگهائي < 0) {
    src_ڊيگهائي = u_strlen(srcChars + src_شروعات);
  }

  إذا (length != src_ڊيگهائي) {
    أرجع خطأ;
  }

  إذا(length == 0 || chars == srcChars) {
    أرجع سچ;
  }

  أرجع u_memcmp(chars, srcChars, src_ڊيگهائي) == 0;
}

int8_t
UnicodeString::doCompare( int32_t شروعات,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_شروعات,
              int32_t src_ڊيگهائي) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع -1;
  }
  
  // pin indices to legal values
  pinIndices(شروعات, length);

  إذا(srcChars == خالي_اشارو) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروعات();

  chars += شروعات;
  srcChars += src_شروعات;

  int32_t min_ڊيگهائي;
  int8_t ڊيگهائي_نتيجو;

  // get the srcLength if necessary
  إذا(src_ڊيگهائي < 0) {
    src_ڊيگهائي = u_strlen(srcChars + src_شروعات);
  }

  // are we comparing different lengths?
  إذا(length != src_ڊيگهائي) {
    إذا(length < src_ڊيگهائي) {
      min_ڊيگهائي = length;
      ڊيگهائي_نتيجو = -1;
    } وإلا {
      min_ڊيگهائي = src_ڊيگهائي;
      ڊيگهائي_نتيجو = 1;
    }
  } وإلا {
    min_ڊيگهائي = length;
    ڊيگهائي_نتيجو = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  إذا(min_ڊيگهائي > 0 && chars != srcChars) {
    int32_t نتيجو;

#   إذا U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      نتيجو = uprv_memcmp(chars, srcChars, min_ڊيگهائي * حجم(char16_t));
      إذا(نتيجو != 0) {
        أرجع (int8_t)(نتيجو >> 15 | 1);
      }
#   وإلا
      // little-endian: compare char16_t units
      افعل {
        نتيجو = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        إذا(نتيجو != 0) {
          أرجع static_cast<int8_t>(نتيجو >> 15 | 1);
        }
      } طالما(--min_ڊيگهائي > 0);
#   endif
  }
  أرجع ڊيگهائي_نتيجو;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_ڪوڊ_نقطو_ترتيب(int32_t شروعات,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t src_شروعات,
                                       int32_t src_ڊيگهائي) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(شروعات, length);

  إذا(srcChars == خالي_اشارو) {
    src_شروعات = src_ڊيگهائي = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_شروعات() + شروعات, length, (srcChars!=خالي_اشارو)?(srcChars + src_شروعات):خالي_اشارو, src_ڊيگهائي, خطأ, سچ);
  /* translate the 32-bit result into an 8-bit one */
  إذا(diff!=0) {
    أرجع static_cast<int8_t>(diff >> 15 | 1);
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_ڊيگهائي() ثابت {
    أرجع length();
}

char16_t
UnicodeString::getCharAt(int32_t وٿي) ثابت {
  أرجع charAt(وٿي);
}

UChar32
UnicodeString::getChar32At(int32_t وٿي) ثابت {
  أرجع char32At(وٿي);
}

UChar32
UnicodeString::char32At(int32_t وٿي) ثابت
{
  int32_t len = length();
  إذا (static_cast<uint32_t>(وٿي) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروعات();
    UChar32 c;
    U16_GET(array, 0, وٿي, len, c);
    أرجع c;
  } وإلا {
    أرجع kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_شروعات(int32_t وٿي) ثابت {
  إذا (static_cast<uint32_t>(وٿي) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = get_Array_شروعات();
    U16_سيٽ_CP_شروعات(array, 0, وٿي);
    أرجع وٿي;
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_Char32_حد_بندي(int32_t وٿي) ثابت {
  int32_t len = length();
  إذا (static_cast<uint32_t>(وٿي) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروعات();
    U16_سيٽ_CP_حد_بندي(array, 0, وٿي, len);
    أرجع وٿي;
  } وإلا {
    أرجع len;
  }
}

int32_t
UnicodeString::ڳڻپ_Char32(int32_t شروعات, int32_t length) ثابت {
  pinIndices(شروعات, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  أرجع u_ڳڻپ_Char32(get_Array_شروعات()+شروعات, length);
}

UBool
UnicodeString::has_وڌيڪ_Char32_Than(int32_t شروعات, int32_t length, int32_t number) ثابت {
  pinIndices(شروعات, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  أرجع u_str_Has_وڌيڪ_Char32_Than(get_Array_شروعات()+شروعات, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t اشاريو, int32_t delta) ثابت {
  // pin index
  int32_t len = length();
  إذا(اشاريو<0) {
    اشاريو=0;
  } وإلا إذا(اشاريو>len) {
    اشاريو=len;
  }

  ثابت char16_t *array = get_Array_شروعات();
  إذا(delta>0) {
    U16_FWD_N(array, اشاريو, len, delta);
  } وإلا {
    U16_پٺ_N(array, 0, اشاريو, -delta);
  }

  أرجع اشاريو;
}

فراغ
UnicodeString::do_ڪڍو(int32_t شروعات,
             int32_t length,
             char16_t *dst,
             int32_t dst_شروعات) ثابت
{
  // pin indices to legal values
  pinIndices(شروعات, length);

  // do not copy anything if we alias dst itself
  ثابت char16_t *array = get_Array_شروعات();
  إذا(array + شروعات != dst + dst_شروعات) {
    us_arrayCopy(array, شروعات, dst, dst_شروعات, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_صلاحيت,
                       UError_ڪوڊ &error_ڪوڊ) ثابت {
  int32_t len = length();
  إذا(U_SUCCESS(error_ڪوڊ)) {
    إذا (isBogus() || dest_صلاحيت < 0 || (dest_صلاحيت > 0 && dest == خالي_اشارو)) {
      error_ڪوڊ=U_ILLEGAL_دليل_ERROR;
    } وإلا {
      ثابت char16_t *array = get_Array_شروعات();
      إذا(len>0 && len<=dest_صلاحيت && array!=dest) {
        u_memcpy(dest, array, len);
      }
      أرجع u_terminateUChars(dest, dest_صلاحيت, len, &error_ڪوڊ);
    }
  }

  أرجع len;
}

int32_t
UnicodeString::extract(int32_t شروعات,
                       int32_t length,
                       حرف *هدف,
                       int32_t هدف_صلاحيت,
                       تعداد EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  إذا(هدف_صلاحيت < 0 || (هدف_صلاحيت > 0 && هدف == خالي_اشارو)) {
    أرجع 0;
  }

  // pin the indices to legal values
  pinIndices(شروعات, length);

  إذا(length <= هدف_صلاحيت) {
    u_UChars_منزل_Chars(get_Array_شروعات() + شروعات, هدف, length);
  }
  UError_ڪوڊ حالت = U_ZERO_ERROR;
  أرجع u_terminateChars(هدف, هدف_صلاحيت, length, &حالت);
}

UnicodeString
UnicodeString::عارضي_Sub_String(int32_t شروعات, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(شروعات, len);
  ثابت char16_t *array = get_بفر();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  إذا(array==خالي_اشارو) {
    array=fUnion.f_ڍير_Fields.f_بفر;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  أرجع UnicodeString(خطأ, array + شروعات, len);
}

int32_t
UnicodeString::منزل_UTF8(int32_t شروعات, int32_t len,
                      حرف *هدف, int32_t capacity) ثابت {
  pinIndices(شروعات, len);
  int32_t length8;
  UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
  u_str_منزل_UTF8_With_Sub(هدف, capacity, &length8,
                     get_بفر() + شروعات, len,
                     0xFFFD,  // Standard substitution character.
                     خالي_اشارو,    // Don't care about number of substitutions.
                     &error_ڪوڊ);
  أرجع length8;
}

#إذا U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t شروعات, int32_t len,
                       حرف *هدف, uint32_t dst_سائيز) ثابت {
  // if the arguments are illegal, then do nothing
  إذا (/*dstSize < 0 || */(dst_سائيز > 0 && هدف == خالي_اشارو)) {
    أرجع 0;
  }
  أرجع منزل_UTF8(شروعات, len, هدف, dst_سائيز <= 0x7fffffff ? static_cast<int32_t>(dst_سائيز) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#جيڪڏهن_پورو

فراغ 
UnicodeString::ڪڍو_Between(int32_t شروعات,
                  int32_t حد_بندي,
                  UnicodeString& هدف) ثابت {
  pin_اشاريو(شروعات);
  pin_اشاريو(حد_بندي);
  do_ڪڍو(شروعات, حد_بندي - شروعات, هدف);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
فراغ
UnicodeString::منزل_UTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  إذا(length16 != 0) {
    حرف ڍير_بفر[1024];
    int32_t capacity = static_cast<int32_t>(حجم(ڍير_بفر));
    UBool utf8IsOwned = خطأ;
    حرف *utf8 = sink.Get_Append_بفر(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ڍير_بفر, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
    u_str_منزل_UTF8_With_Sub(utf8, capacity, &length8,
                       get_بفر(), length16,
                       0xFFFD,  // Standard substitution character.
                       خالي_اشارو,    // Don't care about number of substitutions.
                       &error_ڪوڊ);
    إذا(error_ڪوڊ == U_بفر_حد_کان_مٿي_ERROR) {
      utf8 = static_cast<حرف*>(uprv_malloc(length8));
      إذا(utf8 != خالي_اشارو) {
        utf8IsOwned = سچ;
        error_ڪوڊ = U_ZERO_ERROR;
        u_str_منزل_UTF8_With_Sub(utf8, length8, &length8,
                           get_بفر(), length16,
                           0xFFFD,  // Standard substitution character.
                           خالي_اشارو,    // Don't care about number of substitutions.
                           &error_ڪوڊ);
      } وإلا {
        error_ڪوڊ = U_يادگيري_ورهاست_ERROR;
      }
    }
    إذا(U_SUCCESS(error_ڪوڊ)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    إذا(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::منزل_UTF32(UChar32 *utf32, int32_t capacity, UError_ڪوڊ &error_ڪوڊ) ثابت {
  int32_t length32=0;
  إذا(U_SUCCESS(error_ڪوڊ)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_منزل_UTF32_With_Sub(utf32, capacity, &length32,
        get_بفر(), length(),
        0xfffd,  // Substitution character.
        خالي_اشارو,    // Don't care about number of substitutions.
        &error_ڪوڊ);
  }
  أرجع length32;
}

int32_t 
UnicodeString::اشاريو_جو(ثابت char16_t *srcChars,
               int32_t src_شروعات,
               int32_t src_ڊيگهائي,
               int32_t شروعات,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == خالي_اشارو || src_شروعات < 0 || src_ڊيگهائي == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_ڊيگهائي < 0 && srcChars[src_شروعات] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروعات, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_str_ڳوليو_پهريون(array + شروعات, length, srcChars + src_شروعات, src_ڊيگهائي);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

int32_t
UnicodeString::do_اشاريو_جو(char16_t c,
             int32_t شروعات,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(شروعات, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_memchr(array + شروعات, c, length);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

int32_t
UnicodeString::do_اشاريو_جو(UChar32 c,
                         int32_t شروعات,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(شروعات, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_memchr32(array + شروعات, c, length);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

int32_t 
UnicodeString::آخري_اشاريو_جو(ثابت char16_t *srcChars,
               int32_t src_شروعات,
               int32_t src_ڊيگهائي,
               int32_t شروعات,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == خالي_اشارو || src_شروعات < 0 || src_ڊيگهائي == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_ڊيگهائي < 0 && srcChars[src_شروعات] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروعات, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_str_ڳوليو_آخري(array + شروعات, length, srcChars + src_شروعات, src_ڊيگهائي);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

int32_t
UnicodeString::do_آخري_اشاريو_جو(char16_t c,
                 int32_t شروعات,
                 int32_t length) ثابت
{
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices
  pinIndices(شروعات, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_memrchr(array + شروعات, c, length);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

int32_t
UnicodeString::do_آخري_اشاريو_جو(UChar32 c,
                             int32_t شروعات,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(شروعات, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_شروعات();
  ثابت char16_t *ميلاپ = u_memrchr32(array + شروعات, c, length);
  إذا(ميلاپ == خالي_اشارو) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(ميلاپ - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::ڳوليو_And_Replace(int32_t شروعات,
                              int32_t length,
                              ثابت UnicodeString& old_متن,
                              int32_t old_شروعات,
                              int32_t old_ڊيگهائي,
                              ثابت UnicodeString& new_متن,
                              int32_t new_شروعات,
                              int32_t new_ڊيگهائي) U_LIFETIME_BOUND
{
  إذا(isBogus() || old_متن.isBogus() || new_متن.isBogus()) {
    أرجع *هذا;
  }

  pinIndices(شروعات, length);
  old_متن.pinIndices(old_شروعات, old_ڊيگهائي);
  new_متن.pinIndices(new_شروعات, new_ڊيگهائي);

  إذا(old_ڊيگهائي == 0) {
    أرجع *هذا;
  }

  طالما(length > 0 && length >= old_ڊيگهائي) {
    int32_t pos = اشاريو_جو(old_متن, old_شروعات, old_ڊيگهائي, شروعات, length);
    إذا(pos < 0) {
      // no more oldText's here: done
      اكسر;
    } وإلا {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_ڊيگهائي, new_متن, new_شروعات, new_ڊيگهائي);
      length -= pos + old_ڊيگهائي - شروعات;
      شروعات = pos + new_ڊيگهائي;
    }
  }

  أرجع *هذا;
}


فراغ
UnicodeString::سيٽ_منزل_Bogus()
{
  releaseArray();

  fUnion.fFields.f_ڊيگهائي_And_Flags = kIsBogus;
  fUnion.fFields.fArray = خالي_اشارو;
  fUnion.fFields.f_صلاحيت = 0;
}

// turn a bogus string into an empty one
فراغ
UnicodeString::unBogus() {
  إذا(fUnion.fFields.f_ڊيگهائي_And_Flags & kIsBogus) {
    سيٽ_منزل_خالي_جايون();
  }
}

ثابت char16_t*
UnicodeString::get_Terminated_بفر() U_LIFETIME_BOUND {
  إذا(!isWritable()) {
    أرجع خالي_اشارو;
  }
  char16_t *array = get_Array_شروعات();
  int32_t len = length();
  إذا(len < get_صلاحيت()) {
    إذا(fUnion.fFields.f_ڊيگهائي_And_Flags & k_بفر_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      إذا(array[len] == 0) {
        أرجع array;
      }
    } وإلا إذا(((fUnion.fFields.f_ڊيگهائي_And_Flags & kRefCounted) == 0 || ref_ڳڻپ() == 1)) {
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
    array = get_Array_شروعات();
    array[len] = 0;
    أرجع array;
  } وإلا {
    أرجع خالي_اشارو;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سيٽ_منزل(UBool isTerminated,
                     ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                     int32_t متن_ڊيگهائي) U_LIFETIME_BOUND
{
  إذا(fUnion.fFields.f_ڊيگهائي_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == خالي_اشارو) {
    // treat as an empty string, do not alias
    releaseArray();
    سيٽ_منزل_خالي_جايون();
    أرجع *هذا;
  }

  إذا( متن_ڊيگهائي < -1 ||
      (متن_ڊيگهائي == -1 && !isTerminated) ||
      (متن_ڊيگهائي >= 0 && isTerminated && متن[متن_ڊيگهائي] != 0)
  ) {
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  }

  releaseArray();

  إذا(متن_ڊيگهائي == -1) {
    // text is terminated, or else it would have failed the above test
    متن_ڊيگهائي = u_strlen(متن);
  }
  fUnion.fFields.f_ڊيگهائي_And_Flags = kReadonlyAlias;
  سيٽ_Array(const_cast<char16_t*>(متن), متن_ڊيگهائي, isTerminated ? متن_ڊيگهائي + 1 : متن_ڊيگهائي);
  أرجع *هذا;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سيٽ_منزل(char16_t* بفر,
                     int32_t buff_ڊيگهائي U_LIFETIME_BOUND,
                     int32_t buff_صلاحيت) U_LIFETIME_BOUND {
  إذا(fUnion.fFields.f_ڊيگهائي_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  إذا(بفر == خالي_اشارو) {
    // treat as an empty string, do not alias
    releaseArray();
    سيٽ_منزل_خالي_جايون();
    أرجع *هذا;
  }

  إذا(buff_ڊيگهائي < -1 || buff_صلاحيت < 0 || buff_ڊيگهائي > buff_صلاحيت) {
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  } وإلا إذا(buff_ڊيگهائي == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = بفر, *حد_بندي = بفر + buff_صلاحيت;
    طالما(p != حد_بندي && *p != 0) {
      ++p;
    }
    buff_ڊيگهائي = static_cast<int32_t>(p - بفر);
  }

  releaseArray();

  fUnion.fFields.f_ڊيگهائي_And_Flags = kWritableAlias;
  سيٽ_Array(بفر, buff_ڊيگهائي, buff_صلاحيت);
  أرجع *هذا;
}

UnicodeString& UnicodeString::سيٽ_منزل_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  إذا(length <= US_STACKBUF_سائيز) {
    capacity = US_STACKBUF_سائيز;
  } وإلا {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_بفر(capacity);
  int32_t length16;
  UError_ڪوڊ error_ڪوڊ = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_صلاحيت(), &length16,
      utf8.معلومات(), length,
      0xfffd,  // Substitution character.
      خالي_اشارو,    // Don't care about number of substitutions.
      &error_ڪوڊ);
  release_بفر(length16);
  إذا(U_FAILURE(error_ڪوڊ)) {
    سيٽ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::سيٽ_Char_At(int32_t وٿي, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  إذا(cloneArrayIfNeeded() && len > 0) {
    إذا(وٿي < 0) {
      وٿي = 0;
    } وإلا إذا(وٿي >= len) {
      وٿي = len - 1;
    }

    get_Array_شروعات()[وٿي] = c;
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::replace(int32_t شروعات,
                       int32_t ڊيگهائي_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_ڊيگهائي];
  int32_t count = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, count, U16_MAX_ڊيگهائي, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  أرجع doReplace(شروعات, ڊيگهائي_4, بفر, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_ڊيگهائي];
  int32_t ڊيگهائي_4 = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, ڊيگهائي_4, U16_MAX_ڊيگهائي, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  أرجع isError ? *هذا : doAppend(بفر, 0, ڊيگهائي_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروعات,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t src_شروعات,
                         int32_t src_ڊيگهائي) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_شروعات, src_ڊيگهائي);

  // get the characters from src
  // and replace the range in ourselves with them
  أرجع doReplace(شروعات, length, src.get_Array_شروعات(), src_شروعات, src_ڊيگهائي);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروعات,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t src_شروعات,
                         int32_t src_ڊيگهائي) U_LIFETIME_BOUND
{
  إذا(!isWritable()) {
    أرجع *هذا;
  }

  int32_t old_ڊيگهائي = هذا->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  إذا((fUnion.fFields.f_ڊيگهائي_And_Flags&k_بفر_Is_Readonly) && src_ڊيگهائي == 0) {
    إذا(شروعات == 0) {
      // remove prefix by adjusting the array pointer
      pin_اشاريو(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_صلاحيت -= length;
      سيٽ_ڊيگهائي(old_ڊيگهائي - length);
      أرجع *هذا;
    } وإلا {
      pin_اشاريو(شروعات);
      إذا(length >= (old_ڊيگهائي - شروعات)) {
        // remove suffix by reducing the length (like truncate())
        سيٽ_ڊيگهائي(شروعات);
        fUnion.fFields.f_صلاحيت = شروعات;  // not NUL-terminated any more
        أرجع *هذا;
      }
    }
  }

  إذا(شروعات == old_ڊيگهائي) {
    أرجع doAppend(srcChars, src_شروعات, src_ڊيگهائي);
  }

  إذا (srcChars == خالي_اشارو) {
    src_ڊيگهائي = 0;
  } وإلا {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_شروعات;
    إذا (src_ڊيگهائي < 0) {
      // get the srcLength if necessary
      src_ڊيگهائي = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(شروعات, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_ڊيگهائي = old_ڊيگهائي - length;
  إذا(src_ڊيگهائي > (INT32_MAX - new_ڊيگهائي)) {
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  }
  new_ڊيگهائي += src_ڊيگهائي;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = get_Array_شروعات();
  إذا (is_بفر_Writable() &&
      oldArray < srcChars + src_ڊيگهائي &&
      srcChars < oldArray + old_ڊيگهائي) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_ڊيگهائي);
    إذا (copy.isBogus()) {
      سيٽ_منزل_Bogus();
      أرجع *هذا;
    }
    أرجع doReplace(شروعات, length, copy.get_Array_شروعات(), 0, src_ڊيگهائي);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ڍير_بفر[US_STACKBUF_سائيز];
  إذا((fUnion.fFields.f_ڊيگهائي_And_Flags&k_Using_ڍير_بفر) && (new_ڊيگهائي > US_STACKBUF_سائيز)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ڍير_بفر, oldArray, old_ڊيگهائي);
    oldArray = old_ڍير_بفر;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *بفر_منزل_Delete = خالي_اشارو;
  إذا(!cloneArrayIfNeeded(new_ڊيگهائي, get_Grow_صلاحيت(new_ڊيگهائي),
                         خطأ, &بفر_منزل_Delete)
  ) {
    أرجع *هذا;
  }

  // now do the replace

  char16_t *newArray = get_Array_شروعات();
  إذا(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, شروعات);
    us_arrayCopy(oldArray, شروعات + length,
                 newArray, شروعات + src_ڊيگهائي,
                 old_ڊيگهائي - (شروعات + length));
  } وإلا إذا(length != src_ڊيگهائي) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, شروعات + length,
                 newArray, شروعات + src_ڊيگهائي,
                 old_ڊيگهائي - (شروعات + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, شروعات, src_ڊيگهائي);

  سيٽ_ڊيگهائي(new_ڊيگهائي);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doReplace(int32_t شروعات, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doReplace(شروعات, length, src.معلومات(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t src_شروعات,
                        int32_t src_ڊيگهائي) U_LIFETIME_BOUND {
  إذا(src_ڊيگهائي == 0) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  src.pinIndices(src_شروعات, src_ڊيگهائي);
  أرجع doAppend(src.get_Array_شروعات(), src_شروعات, src_ڊيگهائي);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t src_شروعات,
                        int32_t src_ڊيگهائي) U_LIFETIME_BOUND {
  إذا(!isWritable() || src_ڊيگهائي == 0 || srcChars == خالي_اشارو) {
    أرجع *هذا;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_شروعات;

  إذا(src_ڊيگهائي < 0) {
    // get the srcLength if necessary
    إذا((src_ڊيگهائي = u_strlen(srcChars)) == 0) {
      أرجع *هذا;
    }
  }

  int32_t old_ڊيگهائي = length();
  int32_t new_ڊيگهائي;

  إذا (src_ڊيگهائي <= get_صلاحيت() - old_ڊيگهائي && is_بفر_Writable()) {
    new_ڊيگهائي = old_ڊيگهائي + src_ڊيگهائي;
    // Faster than a memmove
    إذا (src_ڊيگهائي <= 4) {
      char16_t *arr = get_Array_شروعات();
      arr[old_ڊيگهائي] = srcChars[0];
      إذا (src_ڊيگهائي > 1) arr[old_ڊيگهائي+1] = srcChars[1];
      إذا (src_ڊيگهائي > 2) arr[old_ڊيگهائي+2] = srcChars[2];
      إذا (src_ڊيگهائي > 3) arr[old_ڊيگهائي+3] = srcChars[3];
      سيٽ_ڊيگهائي(new_ڊيگهائي);
      أرجع *هذا;
    }
  } وإلا {
    إذا (uprv_add32_حد_کان_مٿي(old_ڊيگهائي, src_ڊيگهائي, &new_ڊيگهائي)) {
      سيٽ_منزل_Bogus();
      أرجع *هذا;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = get_Array_شروعات();
    إذا (is_بفر_Writable() &&
        oldArray < srcChars + src_ڊيگهائي &&
        srcChars < oldArray + old_ڊيگهائي) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_ڊيگهائي);
      إذا (copy.isBogus()) {
        سيٽ_منزل_Bogus();
        أرجع *هذا;
      }
      أرجع doAppend(copy.get_Array_شروعات(), 0, src_ڊيگهائي);
    }

    // optimize append() onto a large-enough, owned string
    إذا (!cloneArrayIfNeeded(new_ڊيگهائي, get_Grow_صلاحيت(new_ڊيگهائي))) {
      أرجع *هذا;
    }
  }

  char16_t *newArray = get_Array_شروعات();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  إذا(srcChars != newArray + old_ڊيگهائي) {
    us_arrayCopy(srcChars, 0, newArray, old_ڊيگهائي, src_ڊيگهائي);
  }
  سيٽ_ڊيگهائي(new_ڊيگهائي);

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable() || src.empty()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سيٽ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doAppend(src.معلومات(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
فراغ
UnicodeString::handleReplaceBetween(int32_t شروعات,
                                    int32_t حد_بندي,
                                    ثابت UnicodeString& متن) {
    replaceBetween(شروعات, حد_بندي, متن);
}

/**
 * Replaceable API
 */
فراغ 
UnicodeString::copy(int32_t شروعات, int32_t حد_بندي, int32_t dest) {
    إذا (حد_بندي <= شروعات) {
        أرجع; // Nothing to do; avoid bogus malloc call
    }
    char16_t* متن = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * (حد_بندي - شروعات)));
    // Check to make sure text is not null.
    إذا (متن != خالي_اشارو) {
	    ڪڍو_Between(شروعات, حد_بندي, متن, 0);
	    insert(dest, متن, 0, حد_بندي - شروعات);    
	    uprv_free(متن);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_معلومات() ثابت {
    أرجع سچ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_معلومات() ثابت {
    أرجع خطأ;
}

UnicodeString&
UnicodeString::do_ابتو(int32_t شروعات, int32_t length) U_LIFETIME_BOUND {
  إذا(length <= 1 || !cloneArrayIfNeeded()) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  pinIndices(شروعات, length);
  إذا(length <= 1) {  // pinIndices() might have shrunk the length
    أرجع *هذا;
  }

  char16_t *کاٻي = get_Array_شروعات() + شروعات;
  char16_t *ساڄي = کاٻي + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = خطأ;

  // Before the loop we know left<right because length>=2.
  افعل {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *کاٻي));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کاٻي++ = *ساڄي));
    *ساڄي-- = swap;
  } طالما(کاٻي < ساڄي);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کاٻي));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  إذا(hasSupplementary) {
    char16_t swap2;

    کاٻي = get_Array_شروعات() + شروعات;
    ساڄي = کاٻي + length - 1; // -1 so that we can look at *(left+1) if left<right
    طالما(کاٻي < ساڄي) {
      إذا(U16_IS_TRAIL(swap = *کاٻي) && U16_IS_LEAD(swap2 = *(کاٻي + 1))) {
        *کاٻي++ = swap2;
        *کاٻي++ = swap;
      } وإلا {
        ++کاٻي;
      }
    }
  }

  أرجع *هذا;
}

UBool 
UnicodeString::padLeading(int32_t هدف_ڊيگهائي,
                          char16_t padChar)
{
  int32_t old_ڊيگهائي = length();
  إذا(old_ڊيگهائي >= هدف_ڊيگهائي || !cloneArrayIfNeeded(هدف_ڊيگهائي)) {
    أرجع خطأ;
  } وإلا {
    // move contents up by padding width
    char16_t *array = get_Array_شروعات();
    int32_t شروعات = هدف_ڊيگهائي - old_ڊيگهائي;
    us_arrayCopy(array, 0, array, شروعات, old_ڊيگهائي);

    // fill in padding character
    طالما(--شروعات >= 0) {
      array[شروعات] = padChar;
    }
    سيٽ_ڊيگهائي(هدف_ڊيگهائي);
    أرجع سچ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t هدف_ڊيگهائي,
                           char16_t padChar)
{
  int32_t old_ڊيگهائي = length();
  إذا(old_ڊيگهائي >= هدف_ڊيگهائي || !cloneArrayIfNeeded(هدف_ڊيگهائي)) {
    أرجع خطأ;
  } وإلا {
    // fill in padding character
    char16_t *array = get_Array_شروعات();
    int32_t length = هدف_ڊيگهائي;
    طالما(--length >= old_ڊيگهائي) {
      array[length] = padChar;
    }
    سيٽ_ڊيگهائي(هدف_ڊيگهائي);
    أرجع سچ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_ڪوڊ() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_ڪوڊ = ustr_hashUCharsN(get_Array_شروعات(), length());
    إذا (hash_ڪوڊ == k_Invalid_Hash_ڪوڊ) {
        hash_ڪوڊ = k_خالي_جايون_Hash_ڪوڊ;
    }
    أرجع hash_ڪوڊ;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_بفر(int32_t min_صلاحيت) U_LIFETIME_BOUND {
  إذا(min_صلاحيت>=-1 && cloneArrayIfNeeded(min_صلاحيت)) {
    fUnion.fFields.f_ڊيگهائي_And_Flags|=k_Open_Get_بفر;
    سيٽ_Zero_ڊيگهائي();
    أرجع get_Array_شروعات();
  } وإلا {
    أرجع خالي_اشارو;
  }
}

فراغ
UnicodeString::release_بفر(int32_t new_ڊيگهائي) {
  إذا(fUnion.fFields.f_ڊيگهائي_And_Flags&k_Open_Get_بفر && new_ڊيگهائي>=-1) {
    // set the new fLength
    int32_t capacity=get_صلاحيت();
    إذا(new_ڊيگهائي==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=get_Array_شروعات(), *p=array, *حد_بندي=array+capacity;
      طالما(p<حد_بندي && *p!=0) {
        ++p;
      }
      new_ڊيگهائي = static_cast<int32_t>(p - array);
    } وإلا إذا(new_ڊيگهائي>capacity) {
      new_ڊيگهائي=capacity;
    }
    سيٽ_ڊيگهائي(new_ڊيگهائي);
    fUnion.fFields.f_ڊيگهائي_And_Flags&=~k_Open_Get_بفر;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_صلاحيت,
                                  int32_t grow_صلاحيت,
                                  UBool doCopyArray,
                                  int32_t **p_بفر_منزل_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  إذا(new_صلاحيت == -1) {
    new_صلاحيت = get_صلاحيت();
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
     fUnion.fFields.f_ڊيگهائي_And_Flags & k_بفر_Is_Readonly ||
     (fUnion.fFields.f_ڊيگهائي_And_Flags & kRefCounted && ref_ڳڻپ() > 1) ||
     new_صلاحيت > get_صلاحيت()
  ) {
    // check growCapacity for default value and use of the stack buffer
    إذا(grow_صلاحيت < 0) {
      grow_صلاحيت = new_صلاحيت;
    } وإلا إذا(new_صلاحيت <= US_STACKBUF_سائيز && grow_صلاحيت > US_STACKBUF_سائيز) {
      grow_صلاحيت = US_STACKBUF_سائيز;
    } وإلا إذا(new_صلاحيت > grow_صلاحيت) {
      سيٽ_منزل_Bogus();
      أرجع خطأ;  // bad inputs
    }
    إذا(grow_صلاحيت > k_Max_صلاحيت) {
      سيٽ_منزل_Bogus();
      أرجع خطأ;
    }

    // save old values
    char16_t old_ڍير_بفر[US_STACKBUF_سائيز];
    char16_t *oldArray;
    int32_t old_ڊيگهائي = length();
    int16_t flags = fUnion.fFields.f_ڊيگهائي_And_Flags;

    إذا(flags&k_Using_ڍير_بفر) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      إذا(doCopyArray && grow_صلاحيت > US_STACKBUF_سائيز) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ڍير_Fields.f_بفر, 0, old_ڍير_بفر, 0, old_ڊيگهائي);
        oldArray = old_ڍير_بفر;
      } وإلا {
        oldArray = خالي_اشارو; // no need to copy from the stack buffer to itself
      }
    } وإلا {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=خالي_اشارو); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    إذا(allocate(grow_صلاحيت) ||
       (new_صلاحيت < grow_صلاحيت && allocate(new_صلاحيت))
    ) {
      إذا(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_ڊيگهائي = old_ڊيگهائي;
        new_صلاحيت = get_صلاحيت();
        إذا(new_صلاحيت < min_ڊيگهائي) {
          min_ڊيگهائي = new_صلاحيت;
        }
        إذا(oldArray != خالي_اشارو) {
          us_arrayCopy(oldArray, 0, get_Array_شروعات(), 0, min_ڊيگهائي);
        }
        سيٽ_ڊيگهائي(min_ڊيگهائي);
      } وإلا {
        سيٽ_Zero_ڊيگهائي();
      }

      // release the old array
      إذا(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_ڳڻپ = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        إذا(umtx_atomic_dec(p_Ref_ڳڻپ) == 0) {
          إذا (p_بفر_منزل_Delete == خالي_اشارو) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((فراغ *)p_Ref_ڳڻپ);
          } وإلا {
            // the caller requested to delete it himself
            *p_بفر_منزل_Delete = reinterpret_cast<int32_t*>(p_Ref_ڳڻپ);
          }
        }
      }
    } وإلا {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      إذا(!(flags&k_Using_ڍير_بفر)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_ڊيگهائي_And_Flags = flags;
      سيٽ_منزل_Bogus();
      أرجع خطأ;
    }
  }
  أرجع سچ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_ڪوڊ_Unit(char16_t c) {
  أرجع str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_ڪوڊ_نقطو(UChar32 c) {
  char16_t بفر[U16_MAX_ڊيگهائي];
  int32_t c_ڊيگهائي = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, c_ڊيگهائي, U16_MAX_ڊيگهائي, c, isError);
  أرجع !isError && str.doAppend(بفر, 0, c_ڊيگهائي).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  أرجع str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_صلاحيت(int32_t append_صلاحيت) {
  أرجع str.cloneArrayIfNeeded(str.length() + append_صلاحيت);
}

char16_t* UnicodeStringAppendable::get_Append_بفر(int32_t min_صلاحيت,
                                                   int32_t desired_صلاحيت_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_صلاحيت,
                                                   int32_t* نتيجو_صلاحيت) U_LIFETIME_BOUND {
  إذا(min_صلاحيت < 1 || scratch_صلاحيت < min_صلاحيت) {
    *نتيجو_صلاحيت = 0;
    أرجع خالي_اشارو;
  }
  int32_t old_ڊيگهائي = str.length();
  إذا(min_صلاحيت <= (k_Max_صلاحيت - old_ڊيگهائي) &&
      desired_صلاحيت_Hint <= (k_Max_صلاحيت - old_ڊيگهائي) &&
      str.cloneArrayIfNeeded(old_ڊيگهائي + min_صلاحيت, old_ڊيگهائي + desired_صلاحيت_Hint)) {
    *نتيجو_صلاحيت = str.get_صلاحيت() - old_ڊيگهائي;
    أرجع str.get_Array_شروعات() + old_ڊيگهائي;
  }
  *نتيجو_صلاحيت = scratch_صلاحيت;
  أرجع scratch;
}

U_NAMESPACE_پڇاڙي

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement ڪنجي) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) ڪنجي.اشارو;
    أرجع (str == خالي_اشارو) ? 0 : str->hash_ڪوڊ();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشارو;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشارو;
    إذا (str1 == str2) {
        أرجع سچ;
    }
    إذا (str1 == خالي_اشارو || str2 == خالي_اشارو) {
        أرجع خطأ;
    }
    أرجع *str1 == *str2;
}

#جيڪڏهن_وصف_ٿيل U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#إذا defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#هدايت GCC diagnostic push
#هدايت GCC diagnostic ignored "-Wunused-function"
ساكن فراغ uprv_UnicodeStringDummy() {
    ميٽيو [] (جديد UnicodeString[2]);
}
#هدايت GCC diagnostic pop
#جيڪڏهن_پورو
#جيڪڏهن_پورو
