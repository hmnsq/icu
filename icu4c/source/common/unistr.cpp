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
استفاده نطاق std;

//DEBUGGING
فراغ
print(ثابت UnicodeString& s,
      ثابت حرف *name)
{
  char16_t c;
  خروجی << name << ":|";
  لكل(صحيح i = 0; i < s.length(); ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      خروجی << "[0x" << hex << s[i] << "]";
    وإلا
      خروجی << (حرف) s[i];
  }
  خروجی << '|' << پایان_سطر;
}

فراغ
print(ثابت char16_t *s,
      int32_t len,
      ثابت حرف *name)
{
  char16_t c;
  خروجی << name << ":|";
  لكل(صحيح i = 0; i < len; ++i) {
    c = s[i];
    إذا(c>= 0x007E || c < 0x0020)
      خروجی << "[0x" << hex << s[i] << "]";
    وإلا
      خروجی << (حرف) s[i];
  }
  خروجی << '|' << پایان_سطر;
}
// END DEBUGGING
#پایان_اگر

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
UnicodeString_charAt(int32_t جابه_جایی, فراغ *context) {
    أرجع ((icu::UnicodeString*) context)->charAt(جابه_جایی);
}
U_CDECL_انتها

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
عملگر+ (ثابت UnicodeString &s1, ثابت UnicodeString &s2) {
  int32_t جمع_Lengths;
  إذا (uprv_add32_سرریز(s1.length(), s2.length(), &جمع_Lengths)) {
    UnicodeString bogus;
    bogus.مجموعه_مقصد_Bogus();
    أرجع bogus;
  }
  إذا (جمع_Lengths != INT32_MAX) {
    ++جمع_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جمع_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_داخلی_Concat(ثابت UnicodeString &s1, std::u16string_view s2) {
  int32_t جمع_Lengths;
  إذا (s2.length() > INT32_MAX ||
      uprv_add32_سرریز(s1.length(), static_cast<int32_t>(s2.length()), &جمع_Lengths)) {
    UnicodeString bogus;
    bogus.مجموعه_مقصد_Bogus();
    أرجع bogus;
  }
  إذا (جمع_Lengths != INT32_MAX) {
    ++جمع_Lengths;  // space for a terminating NUL if we need one
  }
  أرجع UnicodeString(جمع_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

فراغ
UnicodeString::جمع_کن_Ref() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  أرجع umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_شمار() ثابت {
  أرجع umtx_بارگیری_Acquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

فراغ
UnicodeString::releaseArray() {
  إذا((fUnion.fFields.f_درازی_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_درازی_And_Flags = 0;
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
      مجموعه_درازی(length);
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
      مجموعه_درازی(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_درازی_And_Flags = kLength1 | kShortString;
  fUnion.f_پشته_Fields.f_میانگیر[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = خطأ;
  U16_APPEND(fUnion.f_پشته_Fields.f_میانگیر, i, US_STACKBUF_اندازه, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  إذا(!isError) {
    مجموعه_Short_درازی(i);
  }
}

UnicodeString::UnicodeString(ثابت char16_t *متن,
                             int32_t متن_درازی) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  doAppend(متن, 0, متن_درازی);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                             int32_t متن_درازی) {
  fUnion.fFields.f_درازی_And_Flags = kReadonlyAlias;
  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == اشاره_تهی) {
    // treat as an empty string, do not alias
    مجموعه_مقصد_خالی();
  } وإلا إذا(متن_درازی < -1 ||
            (متن_درازی == -1 && !isTerminated) ||
            (متن_درازی >= 0 && isTerminated && متن[متن_درازی] != 0)
  ) {
    مجموعه_مقصد_Bogus();
  } وإلا {
    إذا(متن_درازی == -1) {
      // text is terminated, or else it would have failed the above test
      متن_درازی = u_strlen(متن);
    }
    مجموعه_Array(const_cast<char16_t *>(متن), متن_درازی,
             isTerminated ? متن_درازی + 1 : متن_درازی);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_درازی,
                             int32_t buff_ظرفیت) {
  fUnion.fFields.f_درازی_And_Flags = kWritableAlias;
  إذا(buff == اشاره_تهی) {
    // treat as an empty string, do not alias
    مجموعه_مقصد_خالی();
  } وإلا إذا(buff_درازی < -1 || buff_ظرفیت < 0 || buff_درازی > buff_ظرفیت) {
    مجموعه_مقصد_Bogus();
  } وإلا {
    إذا(buff_درازی == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ثابت char16_t *p = buff, *حد = buff + buff_ظرفیت;
      طالما(p != حد && *p != 0) {
        ++p;
      }
      buff_درازی = static_cast<int32_t>(p - buff);
    }
    مجموعه_Array(buff, buff_درازی, buff_ظرفیت);
  }
}

UnicodeString::UnicodeString(ثابت حرف *src, int32_t length, EInvariant) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  إذا(src==اشاره_تهی) {
    // treat as an empty string
  } وإلا {
    إذا(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    إذا(cloneArrayIfNeeded(length, length, خطأ)) {
      u_chars_مقصد_UChars(src, get_Array_شروع(), length);
      مجموعه_درازی(length);
    } وإلا {
      مجموعه_مقصد_Bogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view متن U_LIFETIME_BOUND) {
  UnicodeString نتیجه;
  إذا (متن.length() <= INT32_MAX) {
    نتیجه.مجموعه_مقصد(خطأ, متن.داده(), static_cast<int32_t>(متن.length()));
  } وإلا {
    نتیجه.مجموعه_مقصد_Bogus();
  }
  أرجع نتیجه;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ثابت UnicodeString& متن U_LIFETIME_BOUND) {
  UnicodeString نتیجه;
  إذا (متن.isBogus()) {
    نتیجه.مجموعه_مقصد_Bogus();
  } وإلا {
    نتیجه.مجموعه_مقصد(خطأ, متن.get_میانگیر(), متن.length());
  }
  أرجع نتیجه;
}

#إذا U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ثابت حرف *codepage_داده) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  إذا (codepage_داده != اشاره_تهی) {
    مجموعه_مقصد_UTF8(codepage_داده);
  }
}

UnicodeString::UnicodeString(ثابت حرف *codepage_داده, int32_t داده_درازی) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  إذا (codepage_داده == اشاره_تهی || داده_درازی == 0 || داده_درازی < -1) {
    أرجع;
  }
  إذا(داده_درازی == -1) {
    داده_درازی = static_cast<int32_t>(uprv_strlen(codepage_داده));
  }
  مجموعه_مقصد_UTF8(StringPiece(codepage_داده, داده_درازی));
}

// else see unistr_cnv.cpp
#پایان_اگر

UnicodeString::UnicodeString(ثابت UnicodeString& that) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, درست);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروع) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  مجموعه_مقصد(that, src_شروع);
}

UnicodeString::UnicodeString(ثابت UnicodeString& that,
                             int32_t src_شروع,
                             int32_t src_درازی) {
  fUnion.fFields.f_درازی_And_Flags = kShortString;
  مجموعه_مقصد(that, src_شروع, src_درازی);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ثابت {
  أرجع اشاره_تهی;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ثابت {
  Local_اشاره_گر<UnicodeString> clonedString(جديد UnicodeString(*هذا));
  أرجع clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : اشاره_تهی;
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

int32_t get_Grow_ظرفیت(int32_t new_درازی) {
  int32_t grow_اندازه = (new_درازی >> 2) + k_Grow_اندازه;
  إذا(grow_اندازه <= (k_Max_ظرفیت - new_درازی)) {
    أرجع new_درازی + grow_اندازه;
  } وإلا {
    أرجع k_Max_ظرفیت;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  إذا(capacity <= US_STACKBUF_اندازه) {
    fUnion.fFields.f_درازی_And_Flags = kShortString;
    أرجع درست;
  }
  إذا(capacity <= k_Max_ظرفیت) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t رقم_بایتها = حجم(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    رقم_بایتها = (رقم_بایتها + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(رقم_بایتها));
    إذا(array != اشاره_تهی) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      رقم_بایتها -= حجم(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.f_ظرفیت = static_cast<int32_t>(رقم_بایتها / U_SIZEOF_UCHAR);
      fUnion.fFields.f_درازی_And_Flags = kLongString;
      أرجع درست;
    }
  }
  fUnion.fFields.f_درازی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = اشاره_تهی;
  fUnion.fFields.f_ظرفیت = 0;
  أرجع خطأ;
}

//========================================
// Destructor
//========================================

#اگر_تعریف_شده UNISTR_شمار_FINAL_STRING_LENGTHS
ساكن u_atomic_int32_t final_درازی_Counts[0x400];  // UnicodeString::kMaxShortLength+1
ساكن u_atomic_int32_t beyond_شمار(0);

U_CAPI فراغ unistr_printLengths() {
  int32_t i;
  لكل(i = 0; i <= 59; ++i) {
    چاپ_کن("%2d,  %9d\n", i, (int32_t)final_درازی_Counts[i]);
  }
  int32_t beyond = beyond_شمار;
  لكل(; i < UPRV_LENGTHOF(final_درازی_Counts); ++i) {
    beyond += final_درازی_Counts[i];
  }
  چاپ_کن(">59, %9d\n", beyond);
}
#پایان_اگر

UnicodeString::~UnicodeString()
{
#اگر_تعریف_شده UNISTR_شمار_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  إذا((fUnion.fFields.f_درازی_And_Flags&(k_Open_Get_میانگیر|kReadonlyAlias|kWritableAlias)) == 0) {
    إذا(has_Short_درازی()) {
      umtx_atomic_inc(final_درازی_Counts + get_Short_درازی());
    } وإلا {
      umtx_atomic_inc(&beyond_شمار);
    }
  }
#پایان_اگر

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString نتیجه;
  نتیجه.مجموعه_مقصد_UTF8(utf8);
  أرجع نتیجه;
}

UnicodeString UnicodeString::fromUTF32(ثابت UChar32 *utf32, int32_t length) {
  UnicodeString نتیجه;
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
    char16_t *utf16 = نتیجه.get_میانگیر(capacity);
    int32_t length16;
    UError_کد error_کد = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, نتیجه.get_ظرفیت(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        اشاره_تهی,    // Don't care about number of substitutions.
        &error_کد);
    نتیجه.release_میانگیر(length16);
    إذا(error_کد == U_میانگیر_سرریز_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      استمر;
    } وإلا إذا(U_FAILURE(error_کد)) {
      نتیجه.مجموعه_مقصد_Bogus();
    }
    اكسر;
  } طالما(درست);
  أرجع نتیجه;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::عملگر=(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ثابت UnicodeString& src) U_LIFETIME_BOUND {
  أرجع copyFrom(src, درست);
}

UnicodeString&
UnicodeString::copyFrom(ثابت UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  إذا(هذا == &src) {
    أرجع *هذا;
  }

  // is the right side bogus?
  إذا(src.isBogus()) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  }

  // delete the current contents
  releaseArray();

  إذا(src.is_خالی()) {
    // empty string - use the stack buffer
    مجموعه_مقصد_خالی();
    أرجع *هذا;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_درازی_And_Flags = src.fUnion.fFields.f_درازی_And_Flags;
  بدل(src.fUnion.fFields.f_درازی_And_Flags & kAllStorageFlags) {
  حالة kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_پشته_Fields.f_میانگیر, src.fUnion.f_پشته_Fields.f_میانگیر,
                get_Short_درازی() * U_SIZEOF_UCHAR);
    اكسر;
  حالة kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).جمع_کن_Ref();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
    إذا(!has_Short_درازی()) {
      fUnion.fFields.f_درازی = src.fUnion.fFields.f_درازی;
    }
    اكسر;
  حالة kReadonlyAlias:
    إذا(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
      إذا(!has_Short_درازی()) {
        fUnion.fFields.f_درازی = src.fUnion.fFields.f_درازی;
      }
      اكسر;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  حالة kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_درازی = src.length();
    إذا(allocate(src_درازی)) {
      u_memcpy(get_Array_شروع(), src.get_Array_شروع(), src_درازی);
      مجموعه_درازی(src_درازی);
      اكسر;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  افتراضي:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_درازی_And_Flags = kIsBogus;
    fUnion.fFields.fArray = اشاره_تهی;
    fUnion.fFields.f_ظرفیت = 0;
    اكسر;
  }

  أرجع *هذا;
}

UnicodeString& UnicodeString::عملگر=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, درست);
  أرجع *هذا;
}

// Same as move assignment except without memory management.
فراغ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool مجموعه_Src_مقصد_Bogus) noexcept {
  int16_t درازی_And_Flags = fUnion.fFields.f_درازی_And_Flags = src.fUnion.fFields.f_درازی_And_Flags;
  إذا(درازی_And_Flags & k_Using_پشته_میانگیر) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    إذا(هذا != &src) {
      uprv_memcpy(fUnion.f_پشته_Fields.f_میانگیر, src.fUnion.f_پشته_Fields.f_میانگیر,
                  get_Short_درازی() * U_SIZEOF_UCHAR);
    }
  } وإلا {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.f_ظرفیت = src.fUnion.fFields.f_ظرفیت;
    إذا(!has_Short_درازی()) {
      fUnion.fFields.f_درازی = src.fUnion.fFields.f_درازی;
    }
    إذا(مجموعه_Src_مقصد_Bogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_درازی_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = اشاره_تهی;
      src.fUnion.fFields.f_ظرفیت = 0;
    }
  }
}

فراغ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString موقت;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  موقت.copyFieldsFrom(*هذا, خطأ);
  هذا->copyFieldsFrom(other, خطأ);
  other.copyFieldsFrom(موقت, خطأ);
  // Set temp to an empty string so that other's memory is not released twice.
  موقت.fUnion.fFields.f_درازی_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ثابت {
    UnicodeString نتیجه(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    إذا (نتیجه.isBogus()) {
        أرجع نتیجه;
    }
    ثابت char16_t *array = get_میانگیر();
    int32_t len = length();
    int32_t prev = 0;
    لكل (int32_t i=0;;) {
        إذا (i == len) {
            نتیجه.append(array, prev, len - prev);
            اكسر;
        }
        إذا (array[i++] == 0x5C /*'\\'*/) {
            نتیجه.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            إذا (c < 0) {
                نتیجه.remove(); // return empty string
                اكسر; // invalid escape sequence
            }
            نتیجه.append(c);
            prev = i;
        }
    }
    أرجع نتیجه;
}

UChar32 UnicodeString::unescapeAt(int32_t &جابه_جایی) ثابت {
    أرجع u_unescapeAt(UnicodeString_charAt, &جابه_جایی, length(), (فراغ*)هذا);
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
              int32_t src_درازی) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع خطأ;
  }
  
  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == اشاره_تهی) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? درست : خطأ;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروع();

  chars += شروع;
  srcChars += src_شروع;

  // get the srcLength if necessary
  إذا(src_درازی < 0) {
    src_درازی = u_strlen(srcChars + src_شروع);
  }

  إذا (length != src_درازی) {
    أرجع خطأ;
  }

  إذا(length == 0 || chars == srcChars) {
    أرجع درست;
  }

  أرجع u_memcmp(chars, srcChars, src_درازی) == 0;
}

int8_t
UnicodeString::doCompare( int32_t شروع,
              int32_t length,
              ثابت char16_t *srcChars,
              int32_t src_شروع,
              int32_t src_درازی) ثابت
{
  // compare illegal string values
  إذا(isBogus()) {
    أرجع -1;
  }
  
  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == اشاره_تهی) {
    // treat const char16_t *srcChars==nullptr as an empty string
    أرجع length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ثابت char16_t *chars = get_Array_شروع();

  chars += شروع;
  srcChars += src_شروع;

  int32_t min_درازی;
  int8_t درازی_نتیجه;

  // get the srcLength if necessary
  إذا(src_درازی < 0) {
    src_درازی = u_strlen(srcChars + src_شروع);
  }

  // are we comparing different lengths?
  إذا(length != src_درازی) {
    إذا(length < src_درازی) {
      min_درازی = length;
      درازی_نتیجه = -1;
    } وإلا {
      min_درازی = src_درازی;
      درازی_نتیجه = 1;
    }
  } وإلا {
    min_درازی = length;
    درازی_نتیجه = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  إذا(min_درازی > 0 && chars != srcChars) {
    int32_t نتیجه;

#   إذا U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      نتیجه = uprv_memcmp(chars, srcChars, min_درازی * حجم(char16_t));
      إذا(نتیجه != 0) {
        أرجع (int8_t)(نتیجه >> 15 | 1);
      }
#   وإلا
      // little-endian: compare char16_t units
      افعل {
        نتیجه = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        إذا(نتیجه != 0) {
          أرجع static_cast<int8_t>(نتیجه >> 15 | 1);
        }
      } طالما(--min_درازی > 0);
#   endif
  }
  أرجع درازی_نتیجه;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::do_Compare_کد_نقطه_ترتیب(int32_t شروع,
                                       int32_t length,
                                       ثابت char16_t *srcChars,
                                       int32_t src_شروع,
                                       int32_t src_درازی) ثابت
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  إذا(isBogus()) {
    أرجع -1;
  }

  // pin indices to legal values
  pinIndices(شروع, length);

  إذا(srcChars == اشاره_تهی) {
    src_شروع = src_درازی = 0;
  }

  int32_t diff = uprv_strCompare(get_Array_شروع() + شروع, length, (srcChars!=اشاره_تهی)?(srcChars + src_شروع):اشاره_تهی, src_درازی, خطأ, درست);
  /* translate the 32-bit result into an 8-bit one */
  إذا(diff!=0) {
    أرجع static_cast<int8_t>(diff >> 15 | 1);
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_درازی() ثابت {
    أرجع length();
}

char16_t
UnicodeString::getCharAt(int32_t جابه_جایی) ثابت {
  أرجع charAt(جابه_جایی);
}

UChar32
UnicodeString::getChar32At(int32_t جابه_جایی) ثابت {
  أرجع char32At(جابه_جایی);
}

UChar32
UnicodeString::char32At(int32_t جابه_جایی) ثابت
{
  int32_t len = length();
  إذا (static_cast<uint32_t>(جابه_جایی) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروع();
    UChar32 c;
    U16_GET(array, 0, جابه_جایی, len, c);
    أرجع c;
  } وإلا {
    أرجع kInvalidUChar;
  }
}

int32_t
UnicodeString::get_Char32_شروع(int32_t جابه_جایی) ثابت {
  إذا (static_cast<uint32_t>(جابه_جایی) < static_cast<uint32_t>(length())) {
    ثابت char16_t *array = get_Array_شروع();
    U16_مجموعه_CP_شروع(array, 0, جابه_جایی);
    أرجع جابه_جایی;
  } وإلا {
    أرجع 0;
  }
}

int32_t
UnicodeString::get_Char32_حد(int32_t جابه_جایی) ثابت {
  int32_t len = length();
  إذا (static_cast<uint32_t>(جابه_جایی) < static_cast<uint32_t>(len)) {
    ثابت char16_t *array = get_Array_شروع();
    U16_مجموعه_CP_حد(array, 0, جابه_جایی, len);
    أرجع جابه_جایی;
  } وإلا {
    أرجع len;
  }
}

int32_t
UnicodeString::شمار_Char32(int32_t شروع, int32_t length) ثابت {
  pinIndices(شروع, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  أرجع u_شمار_Char32(get_Array_شروع()+شروع, length);
}

UBool
UnicodeString::has_بیشتر_Char32_Than(int32_t شروع, int32_t length, int32_t number) ثابت {
  pinIndices(شروع, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  أرجع u_str_Has_بیشتر_Char32_Than(get_Array_شروع()+شروع, length, number);
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

  ثابت char16_t *array = get_Array_شروع();
  إذا(delta>0) {
    U16_FWD_N(array, شاخص, len, delta);
  } وإلا {
    U16_پشت_N(array, 0, شاخص, -delta);
  }

  أرجع شاخص;
}

فراغ
UnicodeString::do_استخراج(int32_t شروع,
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
UnicodeString::extract(Char16Ptr dest, int32_t dest_ظرفیت,
                       UError_کد &error_کد) ثابت {
  int32_t len = length();
  إذا(U_SUCCESS(error_کد)) {
    إذا (isBogus() || dest_ظرفیت < 0 || (dest_ظرفیت > 0 && dest == اشاره_تهی)) {
      error_کد=U_ILLEGAL_آرگومان_ERROR;
    } وإلا {
      ثابت char16_t *array = get_Array_شروع();
      إذا(len>0 && len<=dest_ظرفیت && array!=dest) {
        u_memcpy(dest, array, len);
      }
      أرجع u_terminateUChars(dest, dest_ظرفیت, len, &error_کد);
    }
  }

  أرجع len;
}

int32_t
UnicodeString::extract(int32_t شروع,
                       int32_t length,
                       حرف *هدف,
                       int32_t هدف_ظرفیت,
                       تعداد EInvariant) ثابت
{
  // if the arguments are illegal, then do nothing
  إذا(هدف_ظرفیت < 0 || (هدف_ظرفیت > 0 && هدف == اشاره_تهی)) {
    أرجع 0;
  }

  // pin the indices to legal values
  pinIndices(شروع, length);

  إذا(length <= هدف_ظرفیت) {
    u_UChars_مقصد_Chars(get_Array_شروع() + شروع, هدف, length);
  }
  UError_کد وضعیت = U_ZERO_ERROR;
  أرجع u_terminateChars(هدف, هدف_ظرفیت, length, &وضعیت);
}

UnicodeString
UnicodeString::موقت_Sub_String(int32_t شروع, int32_t len) ثابت U_LIFETIME_BOUND {
  pinIndices(شروع, len);
  ثابت char16_t *array = get_میانگیر();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  إذا(array==اشاره_تهی) {
    array=fUnion.f_پشته_Fields.f_میانگیر;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  أرجع UnicodeString(خطأ, array + شروع, len);
}

int32_t
UnicodeString::مقصد_UTF8(int32_t شروع, int32_t len,
                      حرف *هدف, int32_t capacity) ثابت {
  pinIndices(شروع, len);
  int32_t length8;
  UError_کد error_کد = U_ZERO_ERROR;
  u_str_مقصد_UTF8_With_Sub(هدف, capacity, &length8,
                     get_میانگیر() + شروع, len,
                     0xFFFD,  // Standard substitution character.
                     اشاره_تهی,    // Don't care about number of substitutions.
                     &error_کد);
  أرجع length8;
}

#إذا U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t شروع, int32_t len,
                       حرف *هدف, uint32_t dst_اندازه) ثابت {
  // if the arguments are illegal, then do nothing
  إذا (/*dstSize < 0 || */(dst_اندازه > 0 && هدف == اشاره_تهی)) {
    أرجع 0;
  }
  أرجع مقصد_UTF8(شروع, len, هدف, dst_اندازه <= 0x7fffffff ? static_cast<int32_t>(dst_اندازه) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#پایان_اگر

فراغ 
UnicodeString::استخراج_Between(int32_t شروع,
                  int32_t حد,
                  UnicodeString& هدف) ثابت {
  pin_شاخص(شروع);
  pin_شاخص(حد);
  do_استخراج(شروع, حد - شروع, هدف);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
فراغ
UnicodeString::مقصد_UTF8(ByteSink &sink) ثابت {
  int32_t length16 = length();
  إذا(length16 != 0) {
    حرف پشته_میانگیر[1024];
    int32_t capacity = static_cast<int32_t>(حجم(پشته_میانگیر));
    UBool utf8IsOwned = خطأ;
    حرف *utf8 = sink.Get_Append_میانگیر(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      پشته_میانگیر, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UError_کد error_کد = U_ZERO_ERROR;
    u_str_مقصد_UTF8_With_Sub(utf8, capacity, &length8,
                       get_میانگیر(), length16,
                       0xFFFD,  // Standard substitution character.
                       اشاره_تهی,    // Don't care about number of substitutions.
                       &error_کد);
    إذا(error_کد == U_میانگیر_سرریز_ERROR) {
      utf8 = static_cast<حرف*>(uprv_malloc(length8));
      إذا(utf8 != اشاره_تهی) {
        utf8IsOwned = درست;
        error_کد = U_ZERO_ERROR;
        u_str_مقصد_UTF8_With_Sub(utf8, length8, &length8,
                           get_میانگیر(), length16,
                           0xFFFD,  // Standard substitution character.
                           اشاره_تهی,    // Don't care about number of substitutions.
                           &error_کد);
      } وإلا {
        error_کد = U_حافظه_تخصیص_ERROR;
      }
    }
    إذا(U_SUCCESS(error_کد)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    إذا(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::مقصد_UTF32(UChar32 *utf32, int32_t capacity, UError_کد &error_کد) ثابت {
  int32_t length32=0;
  إذا(U_SUCCESS(error_کد)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_str_مقصد_UTF32_With_Sub(utf32, capacity, &length32,
        get_میانگیر(), length(),
        0xfffd,  // Substitution character.
        اشاره_تهی,    // Don't care about number of substitutions.
        &error_کد);
  }
  أرجع length32;
}

int32_t 
UnicodeString::شاخص_از(ثابت char16_t *srcChars,
               int32_t src_شروع,
               int32_t src_درازی,
               int32_t شروع,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == اشاره_تهی || src_شروع < 0 || src_درازی == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_درازی < 0 && srcChars[src_شروع] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروع, length);

  // find the first occurrence of the substring
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *تطابق = u_str_پیدا_کن_اولی(array + شروع, length, srcChars + src_شروع, src_درازی);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

int32_t
UnicodeString::do_شاخص_از(char16_t c,
             int32_t شروع,
             int32_t length) ثابت
{
  // pin indices
  pinIndices(شروع, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *تطابق = u_memchr(array + شروع, c, length);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

int32_t
UnicodeString::do_شاخص_از(UChar32 c,
                         int32_t شروع,
                         int32_t length) ثابت {
  // pin indices
  pinIndices(شروع, length);

  // find the first occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *تطابق = u_memchr32(array + شروع, c, length);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

int32_t 
UnicodeString::آخرین_شاخص_از(ثابت char16_t *srcChars,
               int32_t src_شروع,
               int32_t src_درازی,
               int32_t شروع,
               int32_t length) ثابت
{
  إذا (isBogus() || srcChars == اشاره_تهی || src_شروع < 0 || src_درازی == 0) {
    أرجع -1;
  }

  // UnicodeString does not find empty substrings
  إذا(src_درازی < 0 && srcChars[src_شروع] == 0) {
    أرجع -1;
  }

  // get the indices within bounds
  pinIndices(شروع, length);

  // find the last occurrence of the substring
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *تطابق = u_str_پیدا_کن_آخرین(array + شروع, length, srcChars + src_شروع, src_درازی);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

int32_t
UnicodeString::do_آخرین_شاخص_از(char16_t c,
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
  ثابت char16_t *تطابق = u_memrchr(array + شروع, c, length);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

int32_t
UnicodeString::do_آخرین_شاخص_از(UChar32 c,
                             int32_t شروع,
                             int32_t length) ثابت {
  // pin indices
  pinIndices(شروع, length);

  // find the last occurrence of c
  ثابت char16_t *array = get_Array_شروع();
  ثابت char16_t *تطابق = u_memrchr32(array + شروع, c, length);
  إذا(تطابق == اشاره_تهی) {
    أرجع -1;
  } وإلا {
    أرجع static_cast<int32_t>(تطابق - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::پیدا_کن_And_Replace(int32_t شروع,
                              int32_t length,
                              ثابت UnicodeString& old_متن,
                              int32_t old_شروع,
                              int32_t old_درازی,
                              ثابت UnicodeString& new_متن,
                              int32_t new_شروع,
                              int32_t new_درازی) U_LIFETIME_BOUND
{
  إذا(isBogus() || old_متن.isBogus() || new_متن.isBogus()) {
    أرجع *هذا;
  }

  pinIndices(شروع, length);
  old_متن.pinIndices(old_شروع, old_درازی);
  new_متن.pinIndices(new_شروع, new_درازی);

  إذا(old_درازی == 0) {
    أرجع *هذا;
  }

  طالما(length > 0 && length >= old_درازی) {
    int32_t pos = شاخص_از(old_متن, old_شروع, old_درازی, شروع, length);
    إذا(pos < 0) {
      // no more oldText's here: done
      اكسر;
    } وإلا {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_درازی, new_متن, new_شروع, new_درازی);
      length -= pos + old_درازی - شروع;
      شروع = pos + new_درازی;
    }
  }

  أرجع *هذا;
}


فراغ
UnicodeString::مجموعه_مقصد_Bogus()
{
  releaseArray();

  fUnion.fFields.f_درازی_And_Flags = kIsBogus;
  fUnion.fFields.fArray = اشاره_تهی;
  fUnion.fFields.f_ظرفیت = 0;
}

// turn a bogus string into an empty one
فراغ
UnicodeString::unBogus() {
  إذا(fUnion.fFields.f_درازی_And_Flags & kIsBogus) {
    مجموعه_مقصد_خالی();
  }
}

ثابت char16_t*
UnicodeString::get_Terminated_میانگیر() U_LIFETIME_BOUND {
  إذا(!isWritable()) {
    أرجع اشاره_تهی;
  }
  char16_t *array = get_Array_شروع();
  int32_t len = length();
  إذا(len < get_ظرفیت()) {
    إذا(fUnion.fFields.f_درازی_And_Flags & k_میانگیر_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      إذا(array[len] == 0) {
        أرجع array;
      }
    } وإلا إذا(((fUnion.fFields.f_درازی_And_Flags & kRefCounted) == 0 || ref_شمار() == 1)) {
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
    أرجع اشاره_تهی;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::مجموعه_مقصد(UBool isTerminated,
                     ConstChar16Ptr متن_Ptr U_LIFETIME_BOUND,
                     int32_t متن_درازی) U_LIFETIME_BOUND
{
  إذا(fUnion.fFields.f_درازی_And_Flags & k_Open_Get_میانگیر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  ثابت char16_t *متن = متن_Ptr;
  إذا(متن == اشاره_تهی) {
    // treat as an empty string, do not alias
    releaseArray();
    مجموعه_مقصد_خالی();
    أرجع *هذا;
  }

  إذا( متن_درازی < -1 ||
      (متن_درازی == -1 && !isTerminated) ||
      (متن_درازی >= 0 && isTerminated && متن[متن_درازی] != 0)
  ) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  }

  releaseArray();

  إذا(متن_درازی == -1) {
    // text is terminated, or else it would have failed the above test
    متن_درازی = u_strlen(متن);
  }
  fUnion.fFields.f_درازی_And_Flags = kReadonlyAlias;
  مجموعه_Array(const_cast<char16_t*>(متن), متن_درازی, isTerminated ? متن_درازی + 1 : متن_درازی);
  أرجع *هذا;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::مجموعه_مقصد(char16_t* میانگیر,
                     int32_t buff_درازی U_LIFETIME_BOUND,
                     int32_t buff_ظرفیت) U_LIFETIME_BOUND {
  إذا(fUnion.fFields.f_درازی_And_Flags & k_Open_Get_میانگیر) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    أرجع *هذا;
  }

  إذا(میانگیر == اشاره_تهی) {
    // treat as an empty string, do not alias
    releaseArray();
    مجموعه_مقصد_خالی();
    أرجع *هذا;
  }

  إذا(buff_درازی < -1 || buff_ظرفیت < 0 || buff_درازی > buff_ظرفیت) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  } وإلا إذا(buff_درازی == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ثابت char16_t *p = میانگیر, *حد = میانگیر + buff_ظرفیت;
    طالما(p != حد && *p != 0) {
      ++p;
    }
    buff_درازی = static_cast<int32_t>(p - میانگیر);
  }

  releaseArray();

  fUnion.fFields.f_درازی_And_Flags = kWritableAlias;
  مجموعه_Array(میانگیر, buff_درازی, buff_ظرفیت);
  أرجع *هذا;
}

UnicodeString& UnicodeString::مجموعه_مقصد_UTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  إذا(length <= US_STACKBUF_اندازه) {
    capacity = US_STACKBUF_اندازه;
  } وإلا {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_میانگیر(capacity);
  int32_t length16;
  UError_کد error_کد = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, get_ظرفیت(), &length16,
      utf8.داده(), length,
      0xfffd,  // Substitution character.
      اشاره_تهی,    // Don't care about number of substitutions.
      &error_کد);
  release_میانگیر(length16);
  إذا(U_FAILURE(error_کد)) {
    مجموعه_مقصد_Bogus();
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::مجموعه_Char_At(int32_t جابه_جایی, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  إذا(cloneArrayIfNeeded() && len > 0) {
    إذا(جابه_جایی < 0) {
      جابه_جایی = 0;
    } وإلا إذا(جابه_جایی >= len) {
      جابه_جایی = len - 1;
    }

    get_Array_شروع()[جابه_جایی] = c;
  }
  أرجع *هذا;
}

UnicodeString&
UnicodeString::replace(int32_t شروع,
                       int32_t درازی_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t میانگیر[U16_MAX_درازی];
  int32_t count = 0;
  UBool isError = خطأ;
  U16_APPEND(میانگیر, count, U16_MAX_درازی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  أرجع doReplace(شروع, درازی_4, میانگیر, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t میانگیر[U16_MAX_درازی];
  int32_t درازی_4 = 0;
  UBool isError = خطأ;
  U16_APPEND(میانگیر, درازی_4, U16_MAX_درازی, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  أرجع isError ? *هذا : doAppend(میانگیر, 0, درازی_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع,
                         int32_t length,
                         ثابت UnicodeString& src,
                         int32_t src_شروع,
                         int32_t src_درازی) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(src_شروع, src_درازی);

  // get the characters from src
  // and replace the range in ourselves with them
  أرجع doReplace(شروع, length, src.get_Array_شروع(), src_شروع, src_درازی);
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع,
                         int32_t length,
                         ثابت char16_t* srcChars,
                         int32_t src_شروع,
                         int32_t src_درازی) U_LIFETIME_BOUND
{
  إذا(!isWritable()) {
    أرجع *هذا;
  }

  int32_t old_درازی = هذا->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  إذا((fUnion.fFields.f_درازی_And_Flags&k_میانگیر_Is_Readonly) && src_درازی == 0) {
    إذا(شروع == 0) {
      // remove prefix by adjusting the array pointer
      pin_شاخص(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.f_ظرفیت -= length;
      مجموعه_درازی(old_درازی - length);
      أرجع *هذا;
    } وإلا {
      pin_شاخص(شروع);
      إذا(length >= (old_درازی - شروع)) {
        // remove suffix by reducing the length (like truncate())
        مجموعه_درازی(شروع);
        fUnion.fFields.f_ظرفیت = شروع;  // not NUL-terminated any more
        أرجع *هذا;
      }
    }
  }

  إذا(شروع == old_درازی) {
    أرجع doAppend(srcChars, src_شروع, src_درازی);
  }

  إذا (srcChars == اشاره_تهی) {
    src_درازی = 0;
  } وإلا {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += src_شروع;
    إذا (src_درازی < 0) {
      // get the srcLength if necessary
      src_درازی = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(شروع, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_درازی = old_درازی - length;
  إذا(src_درازی > (INT32_MAX - new_درازی)) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  }
  new_درازی += src_درازی;

  // Check for insertion into ourself
  ثابت char16_t *oldArray = get_Array_شروع();
  إذا (is_میانگیر_Writable() &&
      oldArray < srcChars + src_درازی &&
      srcChars < oldArray + old_درازی) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_درازی);
    إذا (copy.isBogus()) {
      مجموعه_مقصد_Bogus();
      أرجع *هذا;
    }
    أرجع doReplace(شروع, length, copy.get_Array_شروع(), 0, src_درازی);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_پشته_میانگیر[US_STACKBUF_اندازه];
  إذا((fUnion.fFields.f_درازی_And_Flags&k_Using_پشته_میانگیر) && (new_درازی > US_STACKBUF_اندازه)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_پشته_میانگیر, oldArray, old_درازی);
    oldArray = old_پشته_میانگیر;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *میانگیر_مقصد_Delete = اشاره_تهی;
  إذا(!cloneArrayIfNeeded(new_درازی, get_Grow_ظرفیت(new_درازی),
                         خطأ, &میانگیر_مقصد_Delete)
  ) {
    أرجع *هذا;
  }

  // now do the replace

  char16_t *newArray = get_Array_شروع();
  إذا(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, شروع);
    us_arrayCopy(oldArray, شروع + length,
                 newArray, شروع + src_درازی,
                 old_درازی - (شروع + length));
  } وإلا إذا(length != src_درازی) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, شروع + length,
                 newArray, شروع + src_درازی,
                 old_درازی - (شروع + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, شروع, src_درازی);

  مجموعه_درازی(new_درازی);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  إذا (میانگیر_مقصد_Delete) {
    uprv_free(میانگیر_مقصد_Delete);
  }

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doReplace(int32_t شروع, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  }
  أرجع doReplace(شروع, length, src.داده(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ثابت UnicodeString& src,
                        int32_t src_شروع,
                        int32_t src_درازی) U_LIFETIME_BOUND {
  إذا(src_درازی == 0) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  src.pinIndices(src_شروع, src_درازی);
  أرجع doAppend(src.get_Array_شروع(), src_شروع, src_درازی);
}

UnicodeString&
UnicodeString::doAppend(ثابت char16_t* srcChars,
                        int32_t src_شروع,
                        int32_t src_درازی) U_LIFETIME_BOUND {
  إذا(!isWritable() || src_درازی == 0 || srcChars == اشاره_تهی) {
    أرجع *هذا;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += src_شروع;

  إذا(src_درازی < 0) {
    // get the srcLength if necessary
    إذا((src_درازی = u_strlen(srcChars)) == 0) {
      أرجع *هذا;
    }
  }

  int32_t old_درازی = length();
  int32_t new_درازی;

  إذا (src_درازی <= get_ظرفیت() - old_درازی && is_میانگیر_Writable()) {
    new_درازی = old_درازی + src_درازی;
    // Faster than a memmove
    إذا (src_درازی <= 4) {
      char16_t *arr = get_Array_شروع();
      arr[old_درازی] = srcChars[0];
      إذا (src_درازی > 1) arr[old_درازی+1] = srcChars[1];
      إذا (src_درازی > 2) arr[old_درازی+2] = srcChars[2];
      إذا (src_درازی > 3) arr[old_درازی+3] = srcChars[3];
      مجموعه_درازی(new_درازی);
      أرجع *هذا;
    }
  } وإلا {
    إذا (uprv_add32_سرریز(old_درازی, src_درازی, &new_درازی)) {
      مجموعه_مقصد_Bogus();
      أرجع *هذا;
    }

    // Check for append onto ourself
    ثابت char16_t* oldArray = get_Array_شروع();
    إذا (is_میانگیر_Writable() &&
        oldArray < srcChars + src_درازی &&
        srcChars < oldArray + old_درازی) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_درازی);
      إذا (copy.isBogus()) {
        مجموعه_مقصد_Bogus();
        أرجع *هذا;
      }
      أرجع doAppend(copy.get_Array_شروع(), 0, src_درازی);
    }

    // optimize append() onto a large-enough, owned string
    إذا (!cloneArrayIfNeeded(new_درازی, get_Grow_ظرفیت(new_درازی))) {
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
  إذا(srcChars != newArray + old_درازی) {
    us_arrayCopy(srcChars, 0, newArray, old_درازی, src_درازی);
  }
  مجموعه_درازی(new_درازی);

  أرجع *هذا;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  إذا (!isWritable() || src.empty()) {
    أرجع *هذا;
  }
  إذا (src.length() > INT32_MAX) {
    مجموعه_مقصد_Bogus();
    أرجع *هذا;
  }
  أرجع doAppend(src.داده(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
فراغ
UnicodeString::handleReplaceBetween(int32_t شروع,
                                    int32_t حد,
                                    ثابت UnicodeString& متن) {
    replaceBetween(شروع, حد, متن);
}

/**
 * Replaceable API
 */
فراغ 
UnicodeString::copy(int32_t شروع, int32_t حد, int32_t dest) {
    إذا (حد <= شروع) {
        أرجع; // Nothing to do; avoid bogus malloc call
    }
    char16_t* متن = static_cast<char16_t*>(uprv_malloc(حجم(char16_t) * (حد - شروع)));
    // Check to make sure text is not null.
    إذا (متن != اشاره_تهی) {
	    استخراج_Between(شروع, حد, متن, 0);
	    insert(dest, متن, 0, حد - شروع);    
	    uprv_free(متن);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_داده() ثابت {
    أرجع درست;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_داده() ثابت {
    أرجع خطأ;
}

UnicodeString&
UnicodeString::do_معکوس(int32_t شروع, int32_t length) U_LIFETIME_BOUND {
  إذا(length <= 1 || !cloneArrayIfNeeded()) {
    أرجع *هذا;
  }

  // pin the indices to legal values
  pinIndices(شروع, length);
  إذا(length <= 1) {  // pinIndices() might have shrunk the length
    أرجع *هذا;
  }

  char16_t *چپ = get_Array_شروع() + شروع;
  char16_t *راست = چپ + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = خطأ;

  // Before the loop we know left<right because length>=2.
  افعل {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *چپ));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*چپ++ = *راست));
    *راست-- = swap;
  } طالما(چپ < راست);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*چپ));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  إذا(hasSupplementary) {
    char16_t swap2;

    چپ = get_Array_شروع() + شروع;
    راست = چپ + length - 1; // -1 so that we can look at *(left+1) if left<right
    طالما(چپ < راست) {
      إذا(U16_IS_TRAIL(swap = *چپ) && U16_IS_LEAD(swap2 = *(چپ + 1))) {
        *چپ++ = swap2;
        *چپ++ = swap;
      } وإلا {
        ++چپ;
      }
    }
  }

  أرجع *هذا;
}

UBool 
UnicodeString::padLeading(int32_t هدف_درازی,
                          char16_t padChar)
{
  int32_t old_درازی = length();
  إذا(old_درازی >= هدف_درازی || !cloneArrayIfNeeded(هدف_درازی)) {
    أرجع خطأ;
  } وإلا {
    // move contents up by padding width
    char16_t *array = get_Array_شروع();
    int32_t شروع = هدف_درازی - old_درازی;
    us_arrayCopy(array, 0, array, شروع, old_درازی);

    // fill in padding character
    طالما(--شروع >= 0) {
      array[شروع] = padChar;
    }
    مجموعه_درازی(هدف_درازی);
    أرجع درست;
  }
}

UBool 
UnicodeString::padTrailing(int32_t هدف_درازی,
                           char16_t padChar)
{
  int32_t old_درازی = length();
  إذا(old_درازی >= هدف_درازی || !cloneArrayIfNeeded(هدف_درازی)) {
    أرجع خطأ;
  } وإلا {
    // fill in padding character
    char16_t *array = get_Array_شروع();
    int32_t length = هدف_درازی;
    طالما(--length >= old_درازی) {
      array[length] = padChar;
    }
    مجموعه_درازی(هدف_درازی);
    أرجع درست;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::do_Hash_کد() ثابت
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hash_کد = ustr_hashUCharsN(get_Array_شروع(), length());
    إذا (hash_کد == k_Invalid_Hash_کد) {
        hash_کد = k_خالی_Hash_کد;
    }
    أرجع hash_کد;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_میانگیر(int32_t min_ظرفیت) U_LIFETIME_BOUND {
  إذا(min_ظرفیت>=-1 && cloneArrayIfNeeded(min_ظرفیت)) {
    fUnion.fFields.f_درازی_And_Flags|=k_Open_Get_میانگیر;
    مجموعه_Zero_درازی();
    أرجع get_Array_شروع();
  } وإلا {
    أرجع اشاره_تهی;
  }
}

فراغ
UnicodeString::release_میانگیر(int32_t new_درازی) {
  إذا(fUnion.fFields.f_درازی_And_Flags&k_Open_Get_میانگیر && new_درازی>=-1) {
    // set the new fLength
    int32_t capacity=get_ظرفیت();
    إذا(new_درازی==-1) {
      // the new length is the string length, capped by fCapacity
      ثابت char16_t *array=get_Array_شروع(), *p=array, *حد=array+capacity;
      طالما(p<حد && *p!=0) {
        ++p;
      }
      new_درازی = static_cast<int32_t>(p - array);
    } وإلا إذا(new_درازی>capacity) {
      new_درازی=capacity;
    }
    مجموعه_درازی(new_درازی);
    fUnion.fFields.f_درازی_And_Flags&=~k_Open_Get_میانگیر;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t new_ظرفیت,
                                  int32_t grow_ظرفیت,
                                  UBool doCopyArray,
                                  int32_t **p_میانگیر_مقصد_Delete,
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
     fUnion.fFields.f_درازی_And_Flags & k_میانگیر_Is_Readonly ||
     (fUnion.fFields.f_درازی_And_Flags & kRefCounted && ref_شمار() > 1) ||
     new_ظرفیت > get_ظرفیت()
  ) {
    // check growCapacity for default value and use of the stack buffer
    إذا(grow_ظرفیت < 0) {
      grow_ظرفیت = new_ظرفیت;
    } وإلا إذا(new_ظرفیت <= US_STACKBUF_اندازه && grow_ظرفیت > US_STACKBUF_اندازه) {
      grow_ظرفیت = US_STACKBUF_اندازه;
    } وإلا إذا(new_ظرفیت > grow_ظرفیت) {
      مجموعه_مقصد_Bogus();
      أرجع خطأ;  // bad inputs
    }
    إذا(grow_ظرفیت > k_Max_ظرفیت) {
      مجموعه_مقصد_Bogus();
      أرجع خطأ;
    }

    // save old values
    char16_t old_پشته_میانگیر[US_STACKBUF_اندازه];
    char16_t *oldArray;
    int32_t old_درازی = length();
    int16_t flags = fUnion.fFields.f_درازی_And_Flags;

    إذا(flags&k_Using_پشته_میانگیر) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      إذا(doCopyArray && grow_ظرفیت > US_STACKBUF_اندازه) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_پشته_Fields.f_میانگیر, 0, old_پشته_میانگیر, 0, old_درازی);
        oldArray = old_پشته_میانگیر;
      } وإلا {
        oldArray = اشاره_تهی; // no need to copy from the stack buffer to itself
      }
    } وإلا {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=اشاره_تهی); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    إذا(allocate(grow_ظرفیت) ||
       (new_ظرفیت < grow_ظرفیت && allocate(new_ظرفیت))
    ) {
      إذا(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_درازی = old_درازی;
        new_ظرفیت = get_ظرفیت();
        إذا(new_ظرفیت < min_درازی) {
          min_درازی = new_ظرفیت;
        }
        إذا(oldArray != اشاره_تهی) {
          us_arrayCopy(oldArray, 0, get_Array_شروع(), 0, min_درازی);
        }
        مجموعه_درازی(min_درازی);
      } وإلا {
        مجموعه_Zero_درازی();
      }

      // release the old array
      إذا(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_شمار = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        إذا(umtx_atomic_dec(p_Ref_شمار) == 0) {
          إذا (p_میانگیر_مقصد_Delete == اشاره_تهی) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((فراغ *)p_Ref_شمار);
          } وإلا {
            // the caller requested to delete it himself
            *p_میانگیر_مقصد_Delete = reinterpret_cast<int32_t*>(p_Ref_شمار);
          }
        }
      }
    } وإلا {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      إذا(!(flags&k_Using_پشته_میانگیر)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_درازی_And_Flags = flags;
      مجموعه_مقصد_Bogus();
      أرجع خطأ;
    }
  }
  أرجع درست;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::append_کد_Unit(char16_t c) {
  أرجع str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::append_کد_نقطه(UChar32 c) {
  char16_t میانگیر[U16_MAX_درازی];
  int32_t c_درازی = 0;
  UBool isError = خطأ;
  U16_APPEND(میانگیر, c_درازی, U16_MAX_درازی, c, isError);
  أرجع !isError && str.doAppend(میانگیر, 0, c_درازی).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ثابت char16_t *s, int32_t length) {
  أرجع str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserve_Append_ظرفیت(int32_t append_ظرفیت) {
  أرجع str.cloneArrayIfNeeded(str.length() + append_ظرفیت);
}

char16_t* UnicodeStringAppendable::get_Append_میانگیر(int32_t min_ظرفیت,
                                                   int32_t desired_ظرفیت_Hint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratch_ظرفیت,
                                                   int32_t* نتیجه_ظرفیت) U_LIFETIME_BOUND {
  إذا(min_ظرفیت < 1 || scratch_ظرفیت < min_ظرفیت) {
    *نتیجه_ظرفیت = 0;
    أرجع اشاره_تهی;
  }
  int32_t old_درازی = str.length();
  إذا(min_ظرفیت <= (k_Max_ظرفیت - old_درازی) &&
      desired_ظرفیت_Hint <= (k_Max_ظرفیت - old_درازی) &&
      str.cloneArrayIfNeeded(old_درازی + min_ظرفیت, old_درازی + desired_ظرفیت_Hint)) {
    *نتیجه_ظرفیت = str.get_ظرفیت() - old_درازی;
    أرجع str.get_Array_شروع() + old_درازی;
  }
  *نتیجه_ظرفیت = scratch_ظرفیت;
  أرجع scratch;
}

U_NAMESPACE_انتها

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ثابت UElement کلید) {
    ثابت UnicodeString *str = (ثابت UnicodeString*) کلید.اشاره_گر;
    أرجع (str == اشاره_تهی) ? 0 : str->hash_کد();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ثابت UElement key1, ثابت UElement key2) {
    ثابت UnicodeString *str1 = (ثابت UnicodeString*) key1.اشاره_گر;
    ثابت UnicodeString *str2 = (ثابت UnicodeString*) key2.اشاره_گر;
    إذا (str1 == str2) {
        أرجع درست;
    }
    إذا (str1 == اشاره_تهی || str2 == اشاره_تهی) {
        أرجع خطأ;
    }
    أرجع *str1 == *str2;
}

#اگر_تعریف_شده U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#إذا defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#رهنمود GCC diagnostic push
#رهنمود GCC diagnostic ignored "-Wunused-function"
ساكن فراغ uprv_UnicodeStringDummy() {
    حذف [] (جديد UnicodeString[2]);
}
#رهنمود GCC diagnostic pop
#پایان_اگر
#پایان_اگر
