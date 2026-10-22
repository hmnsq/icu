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
کارول نطاق std;

//DEBUGGING
فراغ
print(ثابت UnicodeString& s,
      ثابت حرف *name)
{
  char16_t c;
  وتنه << name << ":|";
  لكل(صحيح i = 0; i < s.length(); ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      وتنه << "[0x" << hex << s[i] << "]";
    وإلا
      وتنه << (حرف) s[i];
  }
  وتنه << '|' << کرښه_پای;
}

فراغ
print(ثابت char16_t *s,
      int32_t len,
      ثابت حرف *name)
{
  char16_t c;
  وتنه << name << ":|";
  لكل(صحيح i = 0; i < len; ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      وتنه << "[0x" << hex << s[i] << "]";
    وإلا
      وتنه << (حرف) s[i];
  }
  وتنه << '|' << کرښه_پای;
}
// END DEBUGGING
#د_که_پای

// Local function definitions for now

// need to copy areas that may overlap
ساكن
مضمن فراغ
us_arrayCopy(ثابت char16_t *src, int32_t src_پیل,
         char16_t *dst, int32_t dst_پیل, int32_t count)
{
  إذا(count>0) {
    uprv_memmove(dst+dst_پیل, src+src_پیل, (size_t)count*حجم(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ساكن char16_t U_CALLCONV
UnicodeString_charAt(int32_t دننه_واټن, فراغ *context) {
    أرجع ((icu::UnicodeString*) context)->charAt(دننه_واټن);
}
U_CDECL_پای_ټکی

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
چلوونکی+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t مجموعه_Lengths;
  إذا (uprv_add32_ډکېدنه(s1.length(), s2.length(), &مجموعه_Lengths)) {
    UnicodeString bogus;
    bogus.سېټ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (مجموعه_Lengths != INT32_MAX) {
    ++مجموعه_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(مجموعه_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_دننی_Concat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t مجموعه_Lengths;
  إذا (s2.length() > INT32_MAX ||
      uprv_add32_ډکېدنه(s1.length(), static_cast<int32_t>(s2.length()), &مجموعه_Lengths)) {
    UnicodeString bogus;
    bogus.سېټ_منزل_Bogus();
    أرجع bogus;
  }
  إذا (مجموعه_Lengths != INT32_MAX) {
    ++مجموعه_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(مجموعه_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

فراغ
UnicodeString::جمع_کړه_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  أرجع umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_شمېر() ثابت {
  أرجع umtx_بارول_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

فراغ
UnicodeString::releaseArray() {
  إذا((fUnion.fFields.f_اوږدتیا_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_اوږدتیا_And_Flags = 0;
  إذا (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } وإلا إذا(c <= 0xffff) {
    int32_t length = count;
    إذا(capacity < length) {
      capacity = length;
    }
    إذا(allocate(capacity)) {
      char16_t *array = get_Array_پیل();
      char16_t unit = static_cast<char16_t>(c);
      لكل(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      سېټ_اوږدتیا(length);
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
      char16_t *array = get_Array_پیل();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      لكل(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      سېټ_اوږدتیا(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kLength1 | kShortString;
  fUnion.f_ډېری_Fields.f_بفر[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = خطأ;
  U16_APPEND(fUnion.f_ډېری_Fields.f_بفر, i, US_STACKBUF_اندازه, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  إذا(!isError) {
    سېټ_Short_اوږدتیا(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *متن,
                             int32_t متن_اوږدتیا) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  doAppend(متن, 0, متن_اوږدتیا);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                             int32_t متن_اوږدتیا) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kReadonlyAlias;
  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == تش_اشاره) {
    // treat as an empty string, do not alias
    سېټ_منزل_تشه();
  } وإلا إذا(متن_اوږدتیا < -1 ||
            (متن_اوږدتیا == -1 && !isTerminated) ||
            (متن_اوږدتیا >= 0 && isTerminated && متن[متن_اوږدتیا] != 0)
  ) {
    سېټ_منزل_Bogus();
  } وإلا {
    إذا(متن_اوږدتیا == -1) {
      // text is terminated, or else it would have failed the above test
      متن_اوږدتیا = u_strlen(متن);
    }
    سېټ_Array(const_cast<char16_t *>(متن), متن_اوږدتیا,
             isTerminated ? متن_اوږدتیا + 1 : متن_اوږدتیا);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_اوږدتیا,
                             int32_t buff_ظرفیت) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kWritableAlias;
  إذا(buff == تش_اشاره) {
    // treat as an empty string, do not alias
    سېټ_منزل_تشه();
  } وإلا إذا(buff_اوږدتیا < -1 || buff_ظرفیت < 0 || buff_اوږدتیا > buff_ظرفیت) {
    سېټ_منزل_Bogus();
  } وإلا {
    إذا(buff_اوږدتیا == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *حد = buff + buff_ظرفیت;
      طالما(p != حد && *p != 0) {
        ++p;
      }
      buff_اوږدتیا = static_cast<int32_t>(p - buff);
    }
    سېټ_Array(buff, buff_اوږدتیا, buff_ظرفیت);
  }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t length, EInvariant) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  إذا(src==تش_اشاره) {
    // treat as an empty string
  } وإلا {
    إذا(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    إذا(cloneArrayIfNeeded(length, length, خطأ)) {
      u_chars_منزل_UChars(src, get_Array_پیل(), length);
      سېټ_اوږدتیا(length);
    } وإلا {
      سېټ_منزل_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND) {
  UnicodeString پایله;
  إذا (متن.length() <= INT32_MAX) {
    پایله.سېټ_منزل(خطأ, متن.معلومات(), static_cast<int32_t>(متن.length()));
  } وإلا {
    پایله.سېټ_منزل_Bogus();
  }
  أرجع پایله;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
  UnicodeString پایله;
  إذا (متن.isBogus()) {
    پایله.سېټ_منزل_Bogus();
  } وإلا {
    پایله.سېټ_منزل(خطأ, متن.get_بفر(), متن.length());
  }
  أرجع پایله;
}

#إذا U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  إذا (codepage_معلومات != تش_اشاره) {
    سېټ_منزل_UTF8(codepage_معلومات);
  }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_معلومات, int32_t معلومات_اوږدتیا) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  إذا (codepage_معلومات == تش_اشاره || معلومات_اوږدتیا == 0 || معلومات_اوږدتیا < -1) {
    أرجع;
  }
  إذا(معلومات_اوږدتیا == -1) {
    معلومات_اوږدتیا = static_cast<int32_t>(uprv_strlen(codepage_معلومات));
  }
  سېټ_منزل_UTF8(StringPiece(codepage_معلومات, معلومات_اوږدتیا));
}

// else see unistr_cnv.cpp
#د_که_پای

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, ریښتیا);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_پیل) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  سېټ_منزل(that, src_پیل);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_پیل,
                             int32_t src_اوږدتیا) {
  fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
  سېټ_منزل(that, src_پیل, src_اوږدتیا);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  أرجع تش_اشاره;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  Local_اشاره_ګر<UnicodeString> clonedString(جديد UnicodeString(*هذا));
  أرجع clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : تش_اشاره;
}

//========================================
// array allocation
//========================================

نطاق {

ثابت int32_t k_Grow_اندازه = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ثابت int32_t k_Max_ظرفیت = 0x7ffffff5;

int32_t get_Grow_ظرفیت(int32_t new_اوږدتیا) {
  int32_t grow_اندازه = (new_اوږدتیا >> 2) + k_Grow_اندازه;
  إذا(grow_اندازه <= (k_Max_ظرفیت - new_اوږدتیا)) {
    أرجع new_اوږدتیا + grow_اندازه;
  } وإلا {
    أرجع k_Max_ظرفیت;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  إذا(capacity <= US_STACKBUF_اندازه) {
    fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
    أرجع ریښتیا;
  }
  إذا(capacity <= k_Max_ظرفیت) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t عدد_بایټونه = حجم(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    عدد_بایټونه = (عدد_بایټونه + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(عدد_بایټونه));
    إذا(array != تش_اشاره) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      عدد_بایټونه -= حجم(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_ظرفیت = static_cast<int32_t>(عدد_بایټونه / U_SIZEOF_UCHAR);
      fUnion.fFields.f_اوږدتیا_And_Flags = kLongString;
      أرجع ریښتیا;
    }
  }
  fUnion.fFields.f_اوږدتیا_And_Flags = kIsBogus;
  fUnion.fFields.fArray = تش_اشاره;
  fUnion.fFields.f_ظرفیت = 0;
  أرجع خطأ;
}

//========================================
// Destructor
//========================================

#که_تعریف_شوی UNISTR_شمېر_FINAL_STRING_LENGTHS
ساكن u_atomic_int32_t final_اوږدتیا_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ساكن u_atomic_int32_t beyond_شمېر(0);

U_CAPI فراغ unistr_printLengths() {
  int32_t i;
  لكل(i = 0; i <= 59; ++i) {
    چاپ_کړه("%2d,  %9d\n", i, (int32_t)final_اوږدتیا_Counts[i]);
  }
  int32_t beyond = beyond_شمېر;
  لكل(; i < UPRV_LENGTHOF(final_اوږدتیا_Counts); ++i) {
    beyond += final_اوږدتیا_Counts[i];
  }
  چاپ_کړه(">59, %9d\n", beyond);
}
#د_که_پای

UnicodeString::~UnicodeString()
{
#که_تعریف_شوی UNISTR_شمېر_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  إذا((fUnion.fFields.f_اوږدتیا_And_Flags&(k_Open_Get_بفر|kReadonlyAlias|kWritableAlias)) == 0) {
    إذا(has_Short_اوږدتیا()) {
      umtx_atomic_inc(final_اوږدتیا_Counts + get_Short_اوږدتیا());
    } وإلا {
      umtx_atomic_inc(&beyond_شمېر);
    }
  }
#د_که_پای

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString پایله;
  پایله.سېټ_منزل_UTF8(utf8);
  أرجع پایله;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString پایله;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  إذا(length <= US_STACKBUF_اندازه) {
    capacity = US_STACKBUF_اندازه;
  } وإلا {
    capacity = length + (length >> 4) + 4;
  }
  افعل {
    char16_t *utf16 = پایله.get_بفر(capacity);
    int32_t length16;
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, پایله.get_ظرفیت(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        تش_اشاره,    // Don't care about number of substitutions.
        &error_کوډ);
    پایله.release_بفر(length16);
    إذا(error_کوډ == U_بفر_ډکېدنه_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      استمر;
    } وإلا إذا(U_FAILURE(error_کوډ)) {
      پایله.سېټ_منزل_Bogus();
    }
    اكسر;
  } طالما(ریښتیا);
  أرجع پایله;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::چلوونکی=(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src, ریښتیا);
}

UnicodeString&
UnicodeString::copyFrom(ثابت UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  إذا(هذا == &src) {
    أرجع *هذا;
  }

  // is the right side bogus?
  إذا(src.isBogus()) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  }

  // delete the current contents
  releaseArray();

  إذا(src.is_تشه()) {
    // empty string - use the stack buffer
    سېټ_منزل_تشه();
    أرجع *هذا;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_اوږدتیا_And_Flags = src.fUnion.fFields.f_اوږدتیا_And_Flags;
  بدل(src.fUnion.fFields.f_اوږدتیا_And_Flags & kAllStorageFlags) {
  حالة kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ډېری_Fields.f_بفر, src.fUnion.f_ډېری_Fields.f_بفر,
                get_Short_اوږدتیا() * U_SIZEOF_UCHAR);
    اكسر;
  حالة kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).جمع_کړه_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
    إذا(!has_Short_اوږدتیا()) {
      fUnion.fFields.f_اوږدتیا = src.fUnion.fFields.f_اوږدتیا;
    }
    اكسر;
  حالة kReadonlyAlias:
    إذا(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
      إذا(!has_Short_اوږدتیا()) {
        fUnion.fFields.f_اوږدتیا = src.fUnion.fFields.f_اوږدتیا;
      }
      اكسر;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالة kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_اوږدتیا = src.length();
    إذا(allocate(src_اوږدتیا)) {
      u_memcpy(get_Array_پیل(), src.get_Array_پیل(), src_اوږدتیا);
      سېټ_اوږدتیا(src_اوږدتیا);
      اكسر;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  افتراضي:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_اوږدتیا_And_Flags = kIsBogus;
    fUnion.fFields.fArray = تش_اشاره;
    fUnion.fFields.f_ظرفیت = 0;
    اكسر;
  }

  أرجع *هذا;
}

UnicodeString& UnicodeString::چلوونکی=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, ریښتیا);
  أرجع *هذا;
}

// Same as move assignment except without memory management.
فراغ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool سېټ_Src_منزل_Bogus) noexcept {
  int16_t اوږدتیا_And_Flags = fUnion.fFields.f_اوږدتیا_And_Flags = src.fUnion.fFields.f_اوږدتیا_And_Flags;
  إذا(اوږدتیا_And_Flags & k_Using_ډېری_بفر) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    إذا(هذا != &src) {
      uprv_memcpy(fUnion.f_ډېری_Fields.f_بفر, src.fUnion.f_ډېری_Fields.f_بفر,
                  get_Short_اوږدتیا() * U_SIZEOF_UCHAR);
    }
  } وإلا {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
    إذا(!has_Short_اوږدتیا()) {
      fUnion.fFields.f_اوږدتیا = src.fUnion.fFields.f_اوږدتیا;
    }
    إذا(سېټ_Src_منزل_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_اوږدتیا_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = تش_اشاره;
      src.fUnion.fFields.f_ظرفیت = 0;
    }
  }
}

فراغ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString لنډمهاله;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  لنډمهاله.copyFieldsFrom(*هذا, خطأ);
  هذا->copyFieldsFrom(other, خطأ);
  other.copyFieldsFrom(لنډمهاله, خطأ);
  // Set temp to an empty string so that other's memory is not released twice.
  لنډمهاله.fUnion.fFields.f_اوږدتیا_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString پایله(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    إذا (پایله.isBogus()) {
        أرجع پایله;
    }
    ثابت char16_t *array = get_بفر();
    int32_t len = length();
    int32_t prev = 0;
    لكل (int32_t i=0;;) {
        إذا (i == len) {
            پایله.append(array, prev, len - prev);
            اكسر;
        }
        إذا (array[i++] == 0x5C /*'\\'*/) {
            پایله.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            إذا (c < 0) {
                پایله.remove(); // return empty string
                اكسر; // invalid escape sequence
            }
            پایله.append(c);
            prev = i;
        }
    }
    أرجع پایله;
}

UChar32 UnicodeString::unescapeAt(int32_t &دننه_واټن) ثابت {
    أرجع u_unescapeAt(UnicodeString_charAt, &دننه_واټن, length(), (فراغ*)هذا);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ثابت char16_t *متن, int32_t len) ثابت {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  أرجع uprv_memcmp(get_Array_پیل(), متن, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t پیل,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_پیل,
              int32_t src_اوږدتیا) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع خطأ;
  }
  
  // pin indices to legal values
  pinIndices(پیل, length);

  إذا(srcChars == تش_اشاره) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? ریښتیا : خطأ;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_پیل();

  chars += پیل;
  srcChars += src_پیل;

  // get the srcLength if necessary
  إذا(src_اوږدتیا < 0) {
    src_اوږدتیا = u_strlen(srcChars + src_پیل);
  }

  إذا (length != src_اوږدتیا) {
    أرجع خطأ;
  }

  إذا(length == 0 || chars == srcChars) {
    أرجع ریښتیا;
  }

  أرجع u_memcmp(chars, srcChars, src_اوږدتیا) == 0;
}

int8_t
UnicodeString::doCompare( int32_t پیل,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_پیل,
              int32_t src_اوږدتیا) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع -1;
  }
  
  // pin indices to legal values
  pinIndices(پیل, length);

  إذا(srcChars == تش_اشاره) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_پیل();

  chars += پیل;
  srcChars += src_پیل;

  int32_t min_اوږدتیا;
  int8_t اوږدتیا_پایله;

  // get the srcLength if necessary
  إذا(src_اوږدتیا < 0) {
    src_اوږدتیا = u_strlen(srcChars + src_پیل);
  }

  // are we comparing different lengths?
  إذا(length != src_اوږدتیا) {
    إذا(length < src_اوږدتیا) {
      min_اوږدتیا = length;
      اوږدتیا_پایله = -1;
    } وإلا {
      min_اوږدتیا = src_اوږدتیا;
      اوږدتیا_پایله = 1;
    }
  } وإلا {
    min_اوږدتیا = length;
    اوږدتیا_پایله = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  إذا(min_اوږدتیا > 0 && chars != srcChars) {
    int32_t پایله;

#   إذا U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      پایله = uprv_memcmp(chars, srcChars, min_اوږدتیا * حجم(char16_t));
      إذا(پایله != 0) {
        أرجع (int8_t)(پایله >> 15 | 1);
      }
#   وإلا
      // little-endian: compare char16_t units
      افعل {
        پایله = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        إذا(پایله != 0) {
          أرجع static_cast<int8_t>(پایله >> 15 | 1);
        }
      } طالما(--min_اوږدتیا > 0);
#   endif
  }
  أرجع اوږدتیا_پایله;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_کوډ_ټکی_ترتیب(int32_t پیل,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t src_پیل,
                                       int32_t src_اوږدتیا) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(پیل, length);

  إذا(srcChars == تش_اشاره) {
    src_پیل = src_اوږدتیا = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_پیل() + پیل, length, (srcChars!=تش_اشاره)?(srcChars + src_پیل):تش_اشاره, src_اوږدتیا, خطأ, ریښتیا);
  /* translate the 32-bit result into an 8-bit one */
  إذا(diff!=0) {
    أرجع static_cast<int8_t>(diff >> 15 | 1);
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_اوږدتیا() ثابت {
    أرجع length();
}

char16_t
UnicodeString::getCharAt(int32_t دننه_واټن) ثابت {
  أرجع charAt(دننه_واټن);
}

UChar32
UnicodeString::getChar32At(int32_t دننه_واټن) ثابت {
  أرجع char32At(دننه_واټن);
}

UChar32
UnicodeString::char32At(int32_t دننه_واټن) ثابت
{
  int32_t len = length();
  إذا (static_cast<uint32_t>(دننه_واټن) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_پیل();
    UChar32 c;
    U16_GET(array, 0, دننه_واټن, len, c);
    أرجع c;
  } وإلا {
    أرجع kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_پیل(int32_t دننه_واټن) ثابت {
  إذا (static_cast<uint32_t>(دننه_واټن) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = get_Array_پیل();
    U16_سېټ_CP_پیل(array, 0, دننه_واټن);
    أرجع دننه_واټن;
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_Char32_حد(int32_t دننه_واټن) ثابت {
  int32_t len = length();
  إذا (static_cast<uint32_t>(دننه_واټن) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_پیل();
    U16_سېټ_CP_حد(array, 0, دننه_واټن, len);
    أرجع دننه_واټن;
  } وإلا {
    أرجع len;
  }
}

int32_t
UnicodeString::شمېر_Char32(int32_t پیل, int32_t length) ثابت {
  pinIndices(پیل, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  أرجع u_شمېر_Char32(get_Array_پیل()+پیل, length);
}

UBool
UnicodeString::has_نور_Char32_Than(int32_t پیل, int32_t length, int32_t number) ثابت {
  pinIndices(پیل, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  أرجع u_str_Has_نور_Char32_Than(get_Array_پیل()+پیل, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t شاخص, int32_t delta) ثابت {
  // pin index
  int32_t len = length();
  إذا(شاخص<0) {
    شاخص=0;
  } وإلا إذا(شاخص>len) {
    شاخص=len;
  }

  ثابت char16_t *array = get_Array_پیل();
  إذا(delta>0) {
    U16_FWD_N(array, شاخص, len, delta);
  } وإلا {
    U16_شا_N(array, 0, شاخص, -delta);
  }

  أرجع شاخص;
}

فراغ
UnicodeString::do_ایستل(int32_t پیل,
             int32_t length,
             char16_t *dst,
             int32_t dst_پیل) ثابت
{
  // pin indices to legal values
  pinIndices(پیل, length);

  // do not copy anything if we alias dst itself
  ثابت char16_t *array = get_Array_پیل();
  إذا(array + پیل != dst + dst_پیل) {
    us_arrayCopy(array, پیل, dst, dst_پیل, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t dest_ظرفیت,
                       UError_کوډ &error_کوډ) ثابت {
  int32_t len = length();
  إذا(U_SUCCESS(error_کوډ)) {
    إذا (isBogus() || dest_ظرفیت < 0 || (dest_ظرفیت > 0 && dest == تش_اشاره)) {
      error_کوډ=U_ILLEGAL_دلیل_ERROR;
    } وإلا {
      ثابت char16_t *array = get_Array_پیل();
      إذا(len>0 && len<=dest_ظرفیت && array!=dest) {
        u_memcpy(dest, array, len);
      }
      أرجع u_terminateUChars(dest, dest_ظرفیت, len, &error_کوډ);
    }
  }

  أرجع len;
}

int32_t
UnicodeString::extract(int32_t پیل,
                       int32_t length,
                       حرف *موخه,
                       int32_t موخه_ظرفیت,
                       تعداد EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  إذا(موخه_ظرفیت < 0 || (موخه_ظرفیت > 0 && موخه == تش_اشاره)) {
    أرجع 0;
  }

  // pin the indices to legal values
  pinIndices(پیل, length);

  إذا(length <= موخه_ظرفیت) {
    u_UChars_منزل_Chars(get_Array_پیل() + پیل, موخه, length);
  }
  UError_کوډ حالت = U_ZERO_ERROR;
  أرجع u_terminateChars(موخه, موخه_ظرفیت, length, &حالت);
}

UnicodeString
UnicodeString::لنډمهاله_Sub_String(int32_t پیل, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(پیل, len);
  ثابت char16_t *array = get_بفر();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  إذا(array==تش_اشاره) {
    array=fUnion.f_ډېری_Fields.f_بفر;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  أرجع UnicodeString(خطأ, array + پیل, len);
}

int32_t
UnicodeString::منزل_UTF8(int32_t پیل, int32_t len,
                      حرف *موخه, int32_t capacity) ثابت {
  pinIndices(پیل, len);
  int32_t length8;
  UError_کوډ error_کوډ = U_ZERO_ERROR;
  u_str_منزل_UTF8_With_Sub(موخه, capacity, &length8,
                     get_بفر() + پیل, len,
                     0xFFFD,  // Standard substitution character.
                     تش_اشاره,    // Don't care about number of substitutions.
                     &error_کوډ);
  أرجع length8;
}

#إذا U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t پیل, int32_t len,
                       حرف *موخه, uint32_t dst_اندازه) ثابت {
  // if the arguments are illegal, then do nothing
  إذا (/*dstSize < 0 || */(dst_اندازه > 0 && موخه == تش_اشاره)) {
    أرجع 0;
  }
  أرجع منزل_UTF8(پیل, len, موخه, dst_اندازه <= 0x7fffffff ? static_cast<int32_t>(dst_اندازه) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#د_که_پای

فراغ 
UnicodeString::ایستل_Between(int32_t پیل,
                  int32_t حد,
                  UnicodeString& موخه) ثابت {
  pin_شاخص(پیل);
  pin_شاخص(حد);
  do_ایستل(پیل, حد - پیل, موخه);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
فراغ
UnicodeString::منزل_UTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  إذا(length16 != 0) {
    حرف ډېری_بفر[1024];
    int32_t capacity = static_cast<int32_t>(حجم(ډېری_بفر));
    UBool utf8IsOwned = خطأ;
    حرف *utf8 = sink.Get_Append_بفر(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ډېری_بفر, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_کوډ error_کوډ = U_ZERO_ERROR;
    u_str_منزل_UTF8_With_Sub(utf8, capacity, &length8,
                       get_بفر(), length16,
                       0xFFFD,  // Standard substitution character.
                       تش_اشاره,    // Don't care about number of substitutions.
                       &error_کوډ);
    إذا(error_کوډ == U_بفر_ډکېدنه_ERROR) {
      utf8 = static_cast<حرف*>(uprv_malloc(length8));
      إذا(utf8 != تش_اشاره) {
        utf8IsOwned = ریښتیا;
        error_کوډ = U_ZERO_ERROR;
        u_str_منزل_UTF8_With_Sub(utf8, length8, &length8,
                           get_بفر(), length16,
                           0xFFFD,  // Standard substitution character.
                           تش_اشاره,    // Don't care about number of substitutions.
                           &error_کوډ);
      } وإلا {
        error_کوډ = U_حافظه_ویش_ERROR;
      }
    }
    إذا(U_SUCCESS(error_کوډ)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    إذا(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::منزل_UTF32(UChar32 *utf32, int32_t capacity, UError_کوډ &error_کوډ) ثابت {
  int32_t length32=0;
  إذا(U_SUCCESS(error_کوډ)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_منزل_UTF32_With_Sub(utf32, capacity, &length32,
        get_بفر(), length(),
        0xfffd,  // Substitution character.
        تش_اشاره,    // Don't care about number of substitutions.
        &error_کوډ);
  }
  أرجع length32;
}

int32_t 
UnicodeString::شاخص_د(ثابت char16_t *srcChars,
               int32_t src_پیل,
               int32_t src_اوږدتیا,
               int32_t پیل,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == تش_اشاره || src_پیل < 0 || src_اوږدتیا == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_اوږدتیا < 0 && srcChars[src_پیل] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(پیل, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_str_پیدا_کړه_لومړی(array + پیل, length, srcChars + src_پیل, src_اوږدتیا);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

int32_t
UnicodeString::do_شاخص_د(char16_t c,
             int32_t پیل,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(پیل, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_memchr(array + پیل, c, length);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

int32_t
UnicodeString::do_شاخص_د(UChar32 c,
                         int32_t پیل,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(پیل, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_memchr32(array + پیل, c, length);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

int32_t 
UnicodeString::وروستی_شاخص_د(ثابت char16_t *srcChars,
               int32_t src_پیل,
               int32_t src_اوږدتیا,
               int32_t پیل,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == تش_اشاره || src_پیل < 0 || src_اوږدتیا == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_اوږدتیا < 0 && srcChars[src_پیل] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(پیل, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_str_پیدا_کړه_وروستی(array + پیل, length, srcChars + src_پیل, src_اوږدتیا);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

int32_t
UnicodeString::do_وروستی_شاخص_د(char16_t c,
                 int32_t پیل,
                 int32_t length) ثابت
{
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices
  pinIndices(پیل, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_memrchr(array + پیل, c, length);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

int32_t
UnicodeString::do_وروستی_شاخص_د(UChar32 c,
                             int32_t پیل,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(پیل, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_پیل();
  ثابت char16_t *سمون = u_memrchr32(array + پیل, c, length);
  إذا(سمون == تش_اشاره) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(سمون - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::پیدا_کړه_And_Replace(int32_t پیل,
                              int32_t length,
                              ثابت UnicodeString& old_متن,
                              int32_t old_پیل,
                              int32_t old_اوږدتیا,
                              ثابت UnicodeString& new_متن,
                              int32_t new_پیل,
                              int32_t new_اوږدتیا) U_LIFETIME_BOUND
{
  إذا(isBogus() || old_متن.isBogus() || new_متن.isBogus()) {
    أرجع *هذا;
  }

  pinIndices(پیل, length);
  old_متن.pinIndices(old_پیل, old_اوږدتیا);
  new_متن.pinIndices(new_پیل, new_اوږدتیا);

  إذا(old_اوږدتیا == 0) {
    أرجع *هذا;
  }

  طالما(length > 0 && length >= old_اوږدتیا) {
    int32_t pos = شاخص_د(old_متن, old_پیل, old_اوږدتیا, پیل, length);
    إذا(pos < 0) {
      // no more oldText's here: done
      اكسر;
    } وإلا {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_اوږدتیا, new_متن, new_پیل, new_اوږدتیا);
      length -= pos + old_اوږدتیا - پیل;
      پیل = pos + new_اوږدتیا;
    }
  }

  أرجع *هذا;
}


فراغ
UnicodeString::سېټ_منزل_Bogus()
{
  releaseArray();

  fUnion.fFields.f_اوږدتیا_And_Flags = kIsBogus;
  fUnion.fFields.fArray = تش_اشاره;
  fUnion.fFields.f_ظرفیت = 0;
}

// turn a bogus string into an empty one
فراغ
UnicodeString::unBogus() {
  إذا(fUnion.fFields.f_اوږدتیا_And_Flags & kIsBogus) {
    سېټ_منزل_تشه();
  }
}

ثابت char16_t*
UnicodeString::get_Terminated_بفر() U_LIFETIME_BOUND {
  إذا(!isWritable()) {
    أرجع تش_اشاره;
  }
  char16_t *array = get_Array_پیل();
  int32_t len = length();
  إذا(len < get_ظرفیت()) {
    إذا(fUnion.fFields.f_اوږدتیا_And_Flags & k_بفر_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      إذا(array[len] == 0) {
        أرجع array;
      }
    } وإلا إذا(((fUnion.fFields.f_اوږدتیا_And_Flags & kRefCounted) == 0 || ref_شمېر() == 1)) {
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
    array = get_Array_پیل();
    array[len] = 0;
    أرجع array;
  } وإلا {
    أرجع تش_اشاره;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سېټ_منزل(UBool isTerminated,
                     ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                     int32_t متن_اوږدتیا) U_LIFETIME_BOUND
{
  إذا(fUnion.fFields.f_اوږدتیا_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == تش_اشاره) {
    // treat as an empty string, do not alias
    releaseArray();
    سېټ_منزل_تشه();
    أرجع *هذا;
  }

  إذا( متن_اوږدتیا < -1 ||
      (متن_اوږدتیا == -1 && !isTerminated) ||
      (متن_اوږدتیا >= 0 && isTerminated && متن[متن_اوږدتیا] != 0)
  ) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  }

  releaseArray();

  إذا(متن_اوږدتیا == -1) {
    // text is terminated, or else it would have failed the above test
    متن_اوږدتیا = u_strlen(متن);
  }
  fUnion.fFields.f_اوږدتیا_And_Flags = kReadonlyAlias;
  سېټ_Array(const_cast<char16_t*>(متن), متن_اوږدتیا, isTerminated ? متن_اوږدتیا + 1 : متن_اوږدتیا);
  أرجع *هذا;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::سېټ_منزل(char16_t* بفر,
                     int32_t buff_اوږدتیا U_LIFETIME_BOUND,
                     int32_t buff_ظرفیت) U_LIFETIME_BOUND {
  إذا(fUnion.fFields.f_اوږدتیا_And_Flags & k_Open_Get_بفر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  إذا(بفر == تش_اشاره) {
    // treat as an empty string, do not alias
    releaseArray();
    سېټ_منزل_تشه();
    أرجع *هذا;
  }

  إذا(buff_اوږدتیا < -1 || buff_ظرفیت < 0 || buff_اوږدتیا > buff_ظرفیت) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  } وإلا إذا(buff_اوږدتیا == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = بفر, *حد = بفر + buff_ظرفیت;
    طالما(p != حد && *p != 0) {
      ++p;
    }
    buff_اوږدتیا = static_cast<int32_t>(p - بفر);
  }

  releaseArray();

  fUnion.fFields.f_اوږدتیا_And_Flags = kWritableAlias;
  سېټ_Array(بفر, buff_اوږدتیا, buff_ظرفیت);
  أرجع *هذا;
}

UnicodeString& UnicodeString::سېټ_منزل_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  إذا(length <= US_STACKBUF_اندازه) {
    capacity = US_STACKBUF_اندازه;
  } وإلا {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_بفر(capacity);
  int32_t length16;
  UError_کوډ error_کوډ = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_ظرفیت(), &length16,
      utf8.معلومات(), length,
      0xfffd,  // Substitution character.
      تش_اشاره,    // Don't care about number of substitutions.
      &error_کوډ);
  release_بفر(length16);
  إذا(U_FAILURE(error_کوډ)) {
    سېټ_منزل_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::سېټ_Char_At(int32_t دننه_واټن, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  إذا(cloneArrayIfNeeded() && len > 0) {
    إذا(دننه_واټن < 0) {
      دننه_واټن = 0;
    } وإلا إذا(دننه_واټن >= len) {
      دننه_واټن = len - 1;
    }

    get_Array_پیل()[دننه_واټن] = c;
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::replace(int32_t پیل,
                       int32_t اوږدتیا_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_اوږدتیا];
  int32_t count = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, count, U16_MAX_اوږدتیا, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  أرجع doReplace(پیل, اوږدتیا_4, بفر, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t بفر[U16_MAX_اوږدتیا];
  int32_t اوږدتیا_4 = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, اوږدتیا_4, U16_MAX_اوږدتیا, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  أرجع isError ? *هذا : doAppend(بفر, 0, اوږدتیا_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t پیل,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t src_پیل,
                         int32_t src_اوږدتیا) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_پیل, src_اوږدتیا);

  // get the characters from src
  // and replace the range in ourselves with them
  أرجع doReplace(پیل, length, src.get_Array_پیل(), src_پیل, src_اوږدتیا);
}

UnicodeString&
UnicodeString::doReplace(int32_t پیل,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t src_پیل,
                         int32_t src_اوږدتیا) U_LIFETIME_BOUND
{
  إذا(!isWritable()) {
    أرجع *هذا;
  }

  int32_t old_اوږدتیا = هذا->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  إذا((fUnion.fFields.f_اوږدتیا_And_Flags&k_بفر_Is_Readonly) && src_اوږدتیا == 0) {
    إذا(پیل == 0) {
      // remove prefix by adjusting the array pointer
      pin_شاخص(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_ظرفیت -= length;
      سېټ_اوږدتیا(old_اوږدتیا - length);
      أرجع *هذا;
    } وإلا {
      pin_شاخص(پیل);
      إذا(length >= (old_اوږدتیا - پیل)) {
        // remove suffix by reducing the length (like truncate())
        سېټ_اوږدتیا(پیل);
        fUnion.fFields.f_ظرفیت = پیل;  // not NUL-terminated any more
        أرجع *هذا;
      }
    }
  }

  إذا(پیل == old_اوږدتیا) {
    أرجع doAppend(srcChars, src_پیل, src_اوږدتیا);
  }

  إذا (srcChars == تش_اشاره) {
    src_اوږدتیا = 0;
  } وإلا {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_پیل;
    إذا (src_اوږدتیا < 0) {
      // get the srcLength if necessary
      src_اوږدتیا = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(پیل, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_اوږدتیا = old_اوږدتیا - length;
  إذا(src_اوږدتیا > (INT32_MAX - new_اوږدتیا)) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  }
  new_اوږدتیا += src_اوږدتیا;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = get_Array_پیل();
  إذا (is_بفر_Writable() &&
      oldArray < srcChars + src_اوږدتیا &&
      srcChars < oldArray + old_اوږدتیا) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_اوږدتیا);
    إذا (copy.isBogus()) {
      سېټ_منزل_Bogus();
      أرجع *هذا;
    }
    أرجع doReplace(پیل, length, copy.get_Array_پیل(), 0, src_اوږدتیا);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ډېری_بفر[US_STACKBUF_اندازه];
  إذا((fUnion.fFields.f_اوږدتیا_And_Flags&k_Using_ډېری_بفر) && (new_اوږدتیا > US_STACKBUF_اندازه)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ډېری_بفر, oldArray, old_اوږدتیا);
    oldArray = old_ډېری_بفر;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *بفر_منزل_Delete = تش_اشاره;
  إذا(!cloneArrayIfNeeded(new_اوږدتیا, get_Grow_ظرفیت(new_اوږدتیا),
                         خطأ, &بفر_منزل_Delete)
  ) {
    أرجع *هذا;
  }

  // now do the replace

  char16_t *newArray = get_Array_پیل();
  إذا(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, پیل);
    us_arrayCopy(oldArray, پیل + length,
                 newArray, پیل + src_اوږدتیا,
                 old_اوږدتیا - (پیل + length));
  } وإلا إذا(length != src_اوږدتیا) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, پیل + length,
                 newArray, پیل + src_اوږدتیا,
                 old_اوږدتیا - (پیل + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, پیل, src_اوږدتیا);

  سېټ_اوږدتیا(new_اوږدتیا);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  إذا (بفر_منزل_Delete) {
    uprv_free(بفر_منزل_Delete);
  }

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doReplace(int32_t پیل, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doReplace(پیل, length, src.معلومات(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t src_پیل,
                        int32_t src_اوږدتیا) U_LIFETIME_BOUND {
  إذا(src_اوږدتیا == 0) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  src.pinIndices(src_پیل, src_اوږدتیا);
  أرجع doAppend(src.get_Array_پیل(), src_پیل, src_اوږدتیا);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t src_پیل,
                        int32_t src_اوږدتیا) U_LIFETIME_BOUND {
  إذا(!isWritable() || src_اوږدتیا == 0 || srcChars == تش_اشاره) {
    أرجع *هذا;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_پیل;

  إذا(src_اوږدتیا < 0) {
    // get the srcLength if necessary
    إذا((src_اوږدتیا = u_strlen(srcChars)) == 0) {
      أرجع *هذا;
    }
  }

  int32_t old_اوږدتیا = length();
  int32_t new_اوږدتیا;

  إذا (src_اوږدتیا <= get_ظرفیت() - old_اوږدتیا && is_بفر_Writable()) {
    new_اوږدتیا = old_اوږدتیا + src_اوږدتیا;
    // Faster than a memmove
    إذا (src_اوږدتیا <= 4) {
      char16_t *arr = get_Array_پیل();
      arr[old_اوږدتیا] = srcChars[0];
      إذا (src_اوږدتیا > 1) arr[old_اوږدتیا+1] = srcChars[1];
      إذا (src_اوږدتیا > 2) arr[old_اوږدتیا+2] = srcChars[2];
      إذا (src_اوږدتیا > 3) arr[old_اوږدتیا+3] = srcChars[3];
      سېټ_اوږدتیا(new_اوږدتیا);
      أرجع *هذا;
    }
  } وإلا {
    إذا (uprv_add32_ډکېدنه(old_اوږدتیا, src_اوږدتیا, &new_اوږدتیا)) {
      سېټ_منزل_Bogus();
      أرجع *هذا;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = get_Array_پیل();
    إذا (is_بفر_Writable() &&
        oldArray < srcChars + src_اوږدتیا &&
        srcChars < oldArray + old_اوږدتیا) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_اوږدتیا);
      إذا (copy.isBogus()) {
        سېټ_منزل_Bogus();
        أرجع *هذا;
      }
      أرجع doAppend(copy.get_Array_پیل(), 0, src_اوږدتیا);
    }

    // optimize append() onto a large-enough, owned string
    إذا (!cloneArrayIfNeeded(new_اوږدتیا, get_Grow_ظرفیت(new_اوږدتیا))) {
      أرجع *هذا;
    }
  }

  char16_t *newArray = get_Array_پیل();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  إذا(srcChars != newArray + old_اوږدتیا) {
    us_arrayCopy(srcChars, 0, newArray, old_اوږدتیا, src_اوږدتیا);
  }
  سېټ_اوږدتیا(new_اوږدتیا);

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable() || src.empty()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    سېټ_منزل_Bogus();
    أرجع *هذا;
  }
  أرجع doAppend(src.معلومات(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
فراغ
UnicodeString::handleReplaceBetween(int32_t پیل,
                                    int32_t حد,
                                    ثابت UnicodeString& متن) {
    replaceBetween(پیل, حد, متن);
}

/**
 * Replaceable API
 */
فراغ 
UnicodeString::copy(int32_t پیل, int32_t حد, int32_t dest) {
    إذا (حد <= پیل) {
        أرجع; // Nothing to do; avoid bogus malloc call
    }
    char16_t* متن = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * (حد - پیل)));
    // Check to make sure text is not null.
    إذا (متن != تش_اشاره) {
	    ایستل_Between(پیل, حد, متن, 0);
	    insert(dest, متن, 0, حد - پیل);    
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
    أرجع ریښتیا;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_معلومات() ثابت {
    أرجع خطأ;
}

UnicodeString&
UnicodeString::do_سرچپه(int32_t پیل, int32_t length) U_LIFETIME_BOUND {
  إذا(length <= 1 || !cloneArrayIfNeeded()) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  pinIndices(پیل, length);
  إذا(length <= 1) {  // pinIndices() might have shrunk the length
    أرجع *هذا;
  }

  char16_t *کیڼ = get_Array_پیل() + پیل;
  char16_t *ښی = کیڼ + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = خطأ;

  // Before the loop we know left<right because length>=2.
  افعل {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *کیڼ));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کیڼ++ = *ښی));
    *ښی-- = swap;
  } طالما(کیڼ < ښی);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*کیڼ));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  إذا(hasSupplementary) {
    char16_t swap2;

    کیڼ = get_Array_پیل() + پیل;
    ښی = کیڼ + length - 1; // -1 so that we can look at *(left+1) if left<right
    طالما(کیڼ < ښی) {
      إذا(U16_IS_TRAIL(swap = *کیڼ) && U16_IS_LEAD(swap2 = *(کیڼ + 1))) {
        *کیڼ++ = swap2;
        *کیڼ++ = swap;
      } وإلا {
        ++کیڼ;
      }
    }
  }

  أرجع *هذا;
}

UBool 
UnicodeString::padLeading(int32_t موخه_اوږدتیا,
                          char16_t padChar)
{
  int32_t old_اوږدتیا = length();
  إذا(old_اوږدتیا >= موخه_اوږدتیا || !cloneArrayIfNeeded(موخه_اوږدتیا)) {
    أرجع خطأ;
  } وإلا {
    // move contents up by padding width
    char16_t *array = get_Array_پیل();
    int32_t پیل = موخه_اوږدتیا - old_اوږدتیا;
    us_arrayCopy(array, 0, array, پیل, old_اوږدتیا);

    // fill in padding character
    طالما(--پیل >= 0) {
      array[پیل] = padChar;
    }
    سېټ_اوږدتیا(موخه_اوږدتیا);
    أرجع ریښتیا;
  }
}

UBool 
UnicodeString::padTrailing(int32_t موخه_اوږدتیا,
                           char16_t padChar)
{
  int32_t old_اوږدتیا = length();
  إذا(old_اوږدتیا >= موخه_اوږدتیا || !cloneArrayIfNeeded(موخه_اوږدتیا)) {
    أرجع خطأ;
  } وإلا {
    // fill in padding character
    char16_t *array = get_Array_پیل();
    int32_t length = موخه_اوږدتیا;
    طالما(--length >= old_اوږدتیا) {
      array[length] = padChar;
    }
    سېټ_اوږدتیا(موخه_اوږدتیا);
    أرجع ریښتیا;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_کوډ() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_کوډ = ustr_hashUCharsN(get_Array_پیل(), length());
    إذا (hash_کوډ == k_Invalid_Hash_کوډ) {
        hash_کوډ = k_تشه_Hash_کوډ;
    }
    أرجع hash_کوډ;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_بفر(int32_t min_ظرفیت) U_LIFETIME_BOUND {
  إذا(min_ظرفیت>=-1 && cloneArrayIfNeeded(min_ظرفیت)) {
    fUnion.fFields.f_اوږدتیا_And_Flags|=k_Open_Get_بفر;
    سېټ_Zero_اوږدتیا();
    أرجع get_Array_پیل();
  } وإلا {
    أرجع تش_اشاره;
  }
}

فراغ
UnicodeString::release_بفر(int32_t new_اوږدتیا) {
  إذا(fUnion.fFields.f_اوږدتیا_And_Flags&k_Open_Get_بفر && new_اوږدتیا>=-1) {
    // set the new fLength
    int32_t capacity=get_ظرفیت();
    إذا(new_اوږدتیا==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=get_Array_پیل(), *p=array, *حد=array+capacity;
      طالما(p<حد && *p!=0) {
        ++p;
      }
      new_اوږدتیا = static_cast<int32_t>(p - array);
    } وإلا إذا(new_اوږدتیا>capacity) {
      new_اوږدتیا=capacity;
    }
    سېټ_اوږدتیا(new_اوږدتیا);
    fUnion.fFields.f_اوږدتیا_And_Flags&=~k_Open_Get_بفر;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_ظرفیت,
                                  int32_t grow_ظرفیت,
                                  UBool doCopyArray,
                                  int32_t **p_بفر_منزل_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  إذا(new_ظرفیت == -1) {
    new_ظرفیت = get_ظرفیت();
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
     fUnion.fFields.f_اوږدتیا_And_Flags & k_بفر_Is_Readonly ||
     (fUnion.fFields.f_اوږدتیا_And_Flags & kRefCounted && ref_شمېر() > 1) ||
     new_ظرفیت > get_ظرفیت()
  ) {
    // check growCapacity for default value and use of the stack buffer
    إذا(grow_ظرفیت < 0) {
      grow_ظرفیت = new_ظرفیت;
    } وإلا إذا(new_ظرفیت <= US_STACKBUF_اندازه && grow_ظرفیت > US_STACKBUF_اندازه) {
      grow_ظرفیت = US_STACKBUF_اندازه;
    } وإلا إذا(new_ظرفیت > grow_ظرفیت) {
      سېټ_منزل_Bogus();
      أرجع خطأ;  // bad inputs
    }
    إذا(grow_ظرفیت > k_Max_ظرفیت) {
      سېټ_منزل_Bogus();
      أرجع خطأ;
    }

    // save old values
    char16_t old_ډېری_بفر[US_STACKBUF_اندازه];
    char16_t *oldArray;
    int32_t old_اوږدتیا = length();
    int16_t flags = fUnion.fFields.f_اوږدتیا_And_Flags;

    إذا(flags&k_Using_ډېری_بفر) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      إذا(doCopyArray && grow_ظرفیت > US_STACKBUF_اندازه) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ډېری_Fields.f_بفر, 0, old_ډېری_بفر, 0, old_اوږدتیا);
        oldArray = old_ډېری_بفر;
      } وإلا {
        oldArray = تش_اشاره; // no need to copy from the stack buffer to itself
      }
    } وإلا {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=تش_اشاره); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    إذا(allocate(grow_ظرفیت) ||
       (new_ظرفیت < grow_ظرفیت && allocate(new_ظرفیت))
    ) {
      إذا(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_اوږدتیا = old_اوږدتیا;
        new_ظرفیت = get_ظرفیت();
        إذا(new_ظرفیت < min_اوږدتیا) {
          min_اوږدتیا = new_ظرفیت;
        }
        إذا(oldArray != تش_اشاره) {
          us_arrayCopy(oldArray, 0, get_Array_پیل(), 0, min_اوږدتیا);
        }
        سېټ_اوږدتیا(min_اوږدتیا);
      } وإلا {
        سېټ_Zero_اوږدتیا();
      }

      // release the old array
      إذا(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_شمېر = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        إذا(umtx_atomic_dec(p_Ref_شمېر) == 0) {
          إذا (p_بفر_منزل_Delete == تش_اشاره) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((فراغ *)p_Ref_شمېر);
          } وإلا {
            // the caller requested to delete it himself
            *p_بفر_منزل_Delete = reinterpret_cast<int32_t*>(p_Ref_شمېر);
          }
        }
      }
    } وإلا {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      إذا(!(flags&k_Using_ډېری_بفر)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_اوږدتیا_And_Flags = flags;
      سېټ_منزل_Bogus();
      أرجع خطأ;
    }
  }
  أرجع ریښتیا;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_کوډ_Unit(char16_t c) {
  أرجع str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_کوډ_ټکی(UChar32 c) {
  char16_t بفر[U16_MAX_اوږدتیا];
  int32_t c_اوږدتیا = 0;
  UBool isError = خطأ;
  U16_APPEND(بفر, c_اوږدتیا, U16_MAX_اوږدتیا, c, isError);
  أرجع !isError && str.doAppend(بفر, 0, c_اوږدتیا).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  أرجع str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_ظرفیت(int32_t append_ظرفیت) {
  أرجع str.cloneArrayIfNeeded(str.length() + append_ظرفیت);
}

char16_t* UnicodeStringAppendable::get_Append_بفر(int32_t min_ظرفیت,
                                                   int32_t desired_ظرفیت_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_ظرفیت,
                                                   int32_t* پایله_ظرفیت) U_LIFETIME_BOUND {
  إذا(min_ظرفیت < 1 || scratch_ظرفیت < min_ظرفیت) {
    *پایله_ظرفیت = 0;
    أرجع تش_اشاره;
  }
  int32_t old_اوږدتیا = str.length();
  إذا(min_ظرفیت <= (k_Max_ظرفیت - old_اوږدتیا) &&
      desired_ظرفیت_Hint <= (k_Max_ظرفیت - old_اوږدتیا) &&
      str.cloneArrayIfNeeded(old_اوږدتیا + min_ظرفیت, old_اوږدتیا + desired_ظرفیت_Hint)) {
    *پایله_ظرفیت = str.get_ظرفیت() - old_اوږدتیا;
    أرجع str.get_Array_پیل() + old_اوږدتیا;
  }
  *پایله_ظرفیت = scratch_ظرفیت;
  أرجع scratch;
}

U_NAMESPACE_پای_ټکی

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement کلی) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلی.اشاره_ګر;
    أرجع (str == تش_اشاره) ? 0 : str->hash_کوډ();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشاره_ګر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشاره_ګر;
    إذا (str1 == str2) {
        أرجع ریښتیا;
    }
    إذا (str1 == تش_اشاره || str2 == تش_اشاره) {
        أرجع خطأ;
    }
    أرجع *str1 == *str2;
}

#که_تعریف_شوی U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#إذا defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#لارښوونه GCC diagnostic push
#لارښوونه GCC diagnostic ignored "-Wunused-function"
ساكن فراغ uprv_UnicodeStringDummy() {
    ړنګول [] (جديد UnicodeString[2]);
}
#لارښوونه GCC diagnostic pop
#د_که_پای
#د_که_پای
