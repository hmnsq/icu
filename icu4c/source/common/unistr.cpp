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

#অন্তর্ভাব <string_view>

#অন্তর্ভাব "unicode/utypes.h"
#অন্তর্ভাব "unicode/appendable.h"
#অন্তর্ভাব "unicode/putil.h"
#অন্তর্ভাব "cstring.h"
#অন্তর্ভাব "cmemory.h"
#অন্তর্ভাব "unicode/ustring.h"
#অন্তর্ভাব "unicode/unistr.h"
#অন্তর্ভাব "unicode/utf.h"
#অন্তর্ভাব "unicode/utf16.h"
#অন্তর্ভাব "uelement.h"
#অন্তর্ভাব "ustr_imp.h"
#অন্তর্ভাব "umutex.h"
#অন্তর্ভাব "uassert.h"

#যদি 0

#অন্তর্ভাব <iostream>
using নামাকাশ std;

//DEBUGGING
শূন্য
print(ধ্রুবক UnicodeString& s,
      ধ্রুবক অক্ষর *name)
{
  char16_t c;
  cout << name << ":|";
  জন্য(পূর্ণসংখ্যা i = 0; i < s.length(); ++i) {
    c = s[i];
    যদি(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    অন্যথা
      cout << (অক্ষর) s[i];
  }
  cout << '|' << endl;
}

শূন্য
print(ধ্রুবক char16_t *s,
      int32_t len,
      ধ্রুবক অক্ষর *name)
{
  char16_t c;
  cout << name << ":|";
  জন্য(পূর্ণসংখ্যা i = 0; i < len; ++i) {
    c = s[i];
    যদি(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    অন্যথা
      cout << (অক্ষর) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
স্থির
অন্তঃরেখ শূন্য
us_arrayCopy(ধ্রুবক char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  যদি(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*আকার(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
স্থির char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, শূন্য *context) {
    ফেরত ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (ধ্রুবক UnicodeString &s1, ধ্রুবক UnicodeString &s2) {
  int32_t যোগ_Lengths;
  যদি (uprv_add32_overflow(s1.length(), s2.length(), &যোগ_Lengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ফেরত bogus;
  }
  যদি (যোগ_Lengths != INT32_MAX) {
    ++যোগ_Lengths;  // space for a terminating NUL if we need one
  }
  ফেরত UnicodeString(যোগ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(ধ্রুবক UnicodeString &s1, std::u16string_view s2) {
  int32_t যোগ_Lengths;
  যদি (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &যোগ_Lengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ফেরত bogus;
  }
  যদি (যোগ_Lengths != INT32_MAX) {
    ++যোগ_Lengths;  // space for a terminating NUL if we need one
  }
  ফেরত UnicodeString(যোগ_Lengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

শূন্য
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  ফেরত umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_গুনতি() ধ্রুবক {
  ফেরত umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

শূন্য
UnicodeString::releaseArray() {
  যদি((fUnion.fFields.f_দৈর্ঘ্য_And_Flags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = 0;
  যদি (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } অন্যথা যদি(c <= 0xffff) {
    int32_t length = count;
    যদি(capacity < length) {
      capacity = length;
    }
    যদি(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      জন্য(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      set_দৈর্ঘ্য(length);
    }
  } অন্যথা {  // supplementary code point, write surrogate pairs
    যদি(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ফেরত;
    }
    int32_t length = count * 2;
    যদি(capacity < length) {
      capacity = length;
    }
    যদি(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      জন্য(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      set_দৈর্ঘ্য(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kLength1 | kShortString;
  fUnion.f_স্তূপ_Fields.f_বাফার[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  int32_t i = 0;
  UBool isError = মিথ্যা;
  U16_APPEND(fUnion.f_স্তূপ_Fields.f_বাফার, i, US_STACKBUF_আকার, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  যদি(!isError) {
    set_Short_দৈর্ঘ্য(i);
  }
}

UnicodeString::UnicodeString(ধ্রুবক char16_t *text,
                             int32_t text_দৈর্ঘ্য) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  doAppend(text, 0, text_দৈর্ঘ্য);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t text_দৈর্ঘ্য) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kReadonlyAlias;
  ধ্রুবক char16_t *text = textPtr;
  যদি(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } অন্যথা যদি(text_দৈর্ঘ্য < -1 ||
            (text_দৈর্ঘ্য == -1 && !isTerminated) ||
            (text_দৈর্ঘ্য >= 0 && isTerminated && text[text_দৈর্ঘ্য] != 0)
  ) {
    setToBogus();
  } অন্যথা {
    যদি(text_দৈর্ঘ্য == -1) {
      // text is terminated, or else it would have failed the above test
      text_দৈর্ঘ্য = u_strlen(text);
    }
    setArray(const_cast<char16_t *>(text), text_দৈর্ঘ্য,
             isTerminated ? text_দৈর্ঘ্য + 1 : text_দৈর্ঘ্য);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buff_দৈর্ঘ্য,
                             int32_t buffCapacity) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kWritableAlias;
  যদি(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } অন্যথা যদি(buff_দৈর্ঘ্য < -1 || buffCapacity < 0 || buff_দৈর্ঘ্য > buffCapacity) {
    setToBogus();
  } অন্যথা {
    যদি(buff_দৈর্ঘ্য == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ধ্রুবক char16_t *p = buff, *limit = buff + buffCapacity;
      যখন(p != limit && *p != 0) {
        ++p;
      }
      buff_দৈর্ঘ্য = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buff_দৈর্ঘ্য, buffCapacity);
  }
}

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *src, int32_t length, EInvariant) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  যদি(src==nullptr) {
    // treat as an empty string
  } অন্যথা {
    যদি(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    যদি(cloneArrayIfNeeded(length, length, মিথ্যা)) {
      u_charsToUChars(src, getArrayStart(), length);
      set_দৈর্ঘ্য(length);
    } অন্যথা {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString ফলাফল;
  যদি (text.length() <= INT32_MAX) {
    ফলাফল.setTo(মিথ্যা, text.তথ্য(), static_cast<int32_t>(text.length()));
  } অন্যথা {
    ফলাফল.setToBogus();
  }
  ফেরত ফলাফল;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ধ্রুবক UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString ফলাফল;
  যদি (text.isBogus()) {
    ফলাফল.setToBogus();
  } অন্যথা {
    ফলাফল.setTo(মিথ্যা, text.get_বাফার(), text.length());
  }
  ফেরত ফলাফল;
}

#যদি U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  যদি (codepage_তথ্য != nullptr) {
    setToUTF8(codepage_তথ্য);
  }
}

UnicodeString::UnicodeString(ধ্রুবক অক্ষর *codepage_তথ্য, int32_t তথ্য_দৈর্ঘ্য) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  // if there's nothing to convert, do nothing
  যদি (codepage_তথ্য == nullptr || তথ্য_দৈর্ঘ্য == 0 || তথ্য_দৈর্ঘ্য < -1) {
    ফেরত;
  }
  যদি(তথ্য_দৈর্ঘ্য == -1) {
    তথ্য_দৈর্ঘ্য = static_cast<int32_t>(uprv_strlen(codepage_তথ্য));
  }
  setToUTF8(StringPiece(codepage_তথ্য, তথ্য_দৈর্ঘ্য));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(ধ্রুবক UnicodeString& that) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, সত্য);
}

UnicodeString::UnicodeString(ধ্রুবক UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(ধ্রুবক UnicodeString& that,
                             int32_t srcStart,
                             int32_t src_দৈর্ঘ্য) {
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
  setTo(that, srcStart, src_দৈর্ঘ্য);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ধ্রুবক {
  ফেরত nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ধ্রুবক {
  LocalPointer<UnicodeString> clonedString(নতুন UnicodeString(*এই));
  ফেরত clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

নামাকাশ {

ধ্রুবক int32_t k_Grow_আকার = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ধ্রুবক int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t new_দৈর্ঘ্য) {
  int32_t grow_আকার = (new_দৈর্ঘ্য >> 2) + k_Grow_আকার;
  যদি(grow_আকার <= (kMaxCapacity - new_দৈর্ঘ্য)) {
    ফেরত new_দৈর্ঘ্য + grow_আকার;
  } অন্যথা {
    ফেরত kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  যদি(capacity <= US_STACKBUF_আকার) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
    ফেরত সত্য;
  }
  যদি(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t অঙ্ক_Bytes = আকার(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    অঙ্ক_Bytes = (অঙ্ক_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(অঙ্ক_Bytes));
    যদি(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      অঙ্ক_Bytes -= আকার(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(অঙ্ক_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kLongString;
      ফেরত সত্য;
    }
  }
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  ফেরত মিথ্যা;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_গুনতি_FINAL_STRING_LENGTHS
স্থির u_atomic_int32_t final_দৈর্ঘ্য_Counts[0x400];  // UnicodeString::kMaxShortLength+1
স্থির u_atomic_int32_t beyond_গুনতি(0);

U_CAPI শূন্য unistr_printLengths() {
  int32_t i;
  জন্য(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)final_দৈর্ঘ্য_Counts[i]);
  }
  int32_t beyond = beyond_গুনতি;
  জন্য(; i < UPRV_LENGTHOF(final_দৈর্ঘ্য_Counts); ++i) {
    beyond += final_দৈর্ঘ্য_Counts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_গুনতি_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  যদি((fUnion.fFields.f_দৈর্ঘ্য_And_Flags&(k_Open_Get_বাফার|kReadonlyAlias|kWritableAlias)) == 0) {
    যদি(has_Short_দৈর্ঘ্য()) {
      umtx_atomic_inc(final_দৈর্ঘ্য_Counts + get_Short_দৈর্ঘ্য());
    } অন্যথা {
      umtx_atomic_inc(&beyond_গুনতি);
    }
  }
#endif

  releaseArray();
}

//========================================
// Factory methods
//========================================

UnicodeString UnicodeString::fromUTF8(StringPiece utf8) {
  UnicodeString ফলাফল;
  ফলাফল.setToUTF8(utf8);
  ফেরত ফলাফল;
}

UnicodeString UnicodeString::fromUTF32(ধ্রুবক UChar32 *utf32, int32_t length) {
  UnicodeString ফলাফল;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  যদি(length <= US_STACKBUF_আকার) {
    capacity = US_STACKBUF_আকার;
  } অন্যথা {
    capacity = length + (length >> 4) + 4;
  }
  করো {
    char16_t *utf16 = ফলাফল.get_বাফার(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, ফলাফল.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    ফলাফল.release_বাফার(length16);
    যদি(errorCode == U_বাফার_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      চালু;
    } অন্যথা যদি(U_FAILURE(errorCode)) {
      ফলাফল.setToBogus();
    }
    বিরতি;
  } যখন(সত্য);
  ফেরত ফলাফল;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(ধ্রুবক UnicodeString& src) U_LIFETIME_BOUND {
  ফেরত copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ধ্রুবক UnicodeString& src) U_LIFETIME_BOUND {
  ফেরত copyFrom(src, সত্য);
}

UnicodeString&
UnicodeString::copyFrom(ধ্রুবক UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  যদি(এই == &src) {
    ফেরত *এই;
  }

  // is the right side bogus?
  যদি(src.isBogus()) {
    setToBogus();
    ফেরত *এই;
  }

  // delete the current contents
  releaseArray();

  যদি(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    ফেরত *এই;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = src.fUnion.fFields.f_দৈর্ঘ্য_And_Flags;
  নির্বাচন(src.fUnion.fFields.f_দৈর্ঘ্য_And_Flags & kAllStorageFlags) {
  ক্ষেত্র kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_স্তূপ_Fields.f_বাফার, src.fUnion.f_স্তূপ_Fields.f_বাফার,
                get_Short_দৈর্ঘ্য() * U_SIZEOF_UCHAR);
    বিরতি;
  ক্ষেত্র kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    যদি(!has_Short_দৈর্ঘ্য()) {
      fUnion.fFields.f_দৈর্ঘ্য = src.fUnion.fFields.f_দৈর্ঘ্য;
    }
    বিরতি;
  ক্ষেত্র kReadonlyAlias:
    যদি(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      যদি(!has_Short_দৈর্ঘ্য()) {
        fUnion.fFields.f_দৈর্ঘ্য = src.fUnion.fFields.f_দৈর্ঘ্য;
      }
      বিরতি;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  ক্ষেত্র kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t src_দৈর্ঘ্য = src.length();
    যদি(allocate(src_দৈর্ঘ্য)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), src_দৈর্ঘ্য);
      set_দৈর্ঘ্য(src_দৈর্ঘ্য);
      বিরতি;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  পূর্বনির্ধারিত:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    বিরতি;
  }

  ফেরত *এই;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, সত্য);
  ফেরত *এই;
}

// Same as move assignment except without memory management.
শূন্য UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t দৈর্ঘ্য_And_Flags = fUnion.fFields.f_দৈর্ঘ্য_And_Flags = src.fUnion.fFields.f_দৈর্ঘ্য_And_Flags;
  যদি(দৈর্ঘ্য_And_Flags & k_Using_স্তূপ_বাফার) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    যদি(এই != &src) {
      uprv_memcpy(fUnion.f_স্তূপ_Fields.f_বাফার, src.fUnion.f_স্তূপ_Fields.f_বাফার,
                  get_Short_দৈর্ঘ্য() * U_SIZEOF_UCHAR);
    }
  } অন্যথা {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    যদি(!has_Short_দৈর্ঘ্য()) {
      fUnion.fFields.f_দৈর্ঘ্য = src.fUnion.fFields.f_দৈর্ঘ্য;
    }
    যদি(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

শূন্য UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString অস্থায়ী;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  অস্থায়ী.copyFieldsFrom(*এই, মিথ্যা);
  এই->copyFieldsFrom(other, মিথ্যা);
  other.copyFieldsFrom(অস্থায়ী, মিথ্যা);
  // Set temp to an empty string so that other's memory is not released twice.
  অস্থায়ী.fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ধ্রুবক {
    UnicodeString ফলাফল(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    যদি (ফলাফল.isBogus()) {
        ফেরত ফলাফল;
    }
    ধ্রুবক char16_t *array = get_বাফার();
    int32_t len = length();
    int32_t পূর্ব = 0;
    জন্য (int32_t i=0;;) {
        যদি (i == len) {
            ফলাফল.append(array, পূর্ব, len - পূর্ব);
            বিরতি;
        }
        যদি (array[i++] == 0x5C /*'\\'*/) {
            ফলাফল.append(array, পূর্ব, (i - 1) - পূর্ব);
            UChar32 c = unescapeAt(i); // advances i
            যদি (c < 0) {
                ফলাফল.remove(); // return empty string
                বিরতি; // invalid escape sequence
            }
            ফলাফল.append(c);
            পূর্ব = i;
        }
    }
    ফেরত ফলাফল;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) ধ্রুবক {
    ফেরত u_unescapeAt(UnicodeString_charAt, &offset, length(), (শূন্য*)এই);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ধ্রুবক char16_t *text, int32_t len) ধ্রুবক {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ফেরত uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              ধ্রুবক char16_t *srcChars,
              int32_t srcStart,
              int32_t src_দৈর্ঘ্য) ধ্রুবক
{
  // compare illegal string values
  যদি(isBogus()) {
    ফেরত মিথ্যা;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ফেরত length == 0 ? সত্য : মিথ্যা;
  }

  // get the correct pointer
  ধ্রুবক char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  যদি(src_দৈর্ঘ্য < 0) {
    src_দৈর্ঘ্য = u_strlen(srcChars + srcStart);
  }

  যদি (length != src_দৈর্ঘ্য) {
    ফেরত মিথ্যা;
  }

  যদি(length == 0 || chars == srcChars) {
    ফেরত সত্য;
  }

  ফেরত u_memcmp(chars, srcChars, src_দৈর্ঘ্য) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ধ্রুবক char16_t *srcChars,
              int32_t srcStart,
              int32_t src_দৈর্ঘ্য) ধ্রুবক
{
  // compare illegal string values
  যদি(isBogus()) {
    ফেরত -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ফেরত length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ধ্রুবক char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t min_দৈর্ঘ্য;
  int8_t দৈর্ঘ্য_ফলাফল;

  // get the srcLength if necessary
  যদি(src_দৈর্ঘ্য < 0) {
    src_দৈর্ঘ্য = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  যদি(length != src_দৈর্ঘ্য) {
    যদি(length < src_দৈর্ঘ্য) {
      min_দৈর্ঘ্য = length;
      দৈর্ঘ্য_ফলাফল = -1;
    } অন্যথা {
      min_দৈর্ঘ্য = src_দৈর্ঘ্য;
      দৈর্ঘ্য_ফলাফল = 1;
    }
  } অন্যথা {
    min_দৈর্ঘ্য = length;
    দৈর্ঘ্য_ফলাফল = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  যদি(min_দৈর্ঘ্য > 0 && chars != srcChars) {
    int32_t ফলাফল;

#   যদি U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      ফলাফল = uprv_memcmp(chars, srcChars, min_দৈর্ঘ্য * আকার(char16_t));
      যদি(ফলাফল != 0) {
        ফেরত (int8_t)(ফলাফল >> 15 | 1);
      }
#   অন্যথা
      // little-endian: compare char16_t units
      করো {
        ফলাফল = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        যদি(ফলাফল != 0) {
          ফেরত static_cast<int8_t>(ফলাফল >> 15 | 1);
        }
      } যখন(--min_দৈর্ঘ্য > 0);
#   endif
  }
  ফেরত দৈর্ঘ্য_ফলাফল;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       ধ্রুবক char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t src_দৈর্ঘ্য) ধ্রুবক
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  যদি(isBogus()) {
    ফেরত -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    srcStart = src_দৈর্ঘ্য = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, src_দৈর্ঘ্য, মিথ্যা, সত্য);
  /* translate the 32-bit result into an 8-bit one */
  যদি(diff!=0) {
    ফেরত static_cast<int8_t>(diff >> 15 | 1);
  } অন্যথা {
    ফেরত 0;
  }
}

int32_t
UnicodeString::get_দৈর্ঘ্য() ধ্রুবক {
    ফেরত length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) ধ্রুবক {
  ফেরত charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) ধ্রুবক {
  ফেরত char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) ধ্রুবক
{
  int32_t len = length();
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ধ্রুবক char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    ফেরত c;
  } অন্যথা {
    ফেরত kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) ধ্রুবক {
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    ধ্রুবক char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    ফেরত offset;
  } অন্যথা {
    ফেরত 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) ধ্রুবক {
  int32_t len = length();
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ধ্রুবক char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    ফেরত offset;
  } অন্যথা {
    ফেরত len;
  }
}

int32_t
UnicodeString::গুনতি_Char32(int32_t start, int32_t length) ধ্রুবক {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ফেরত u_গুনতি_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) ধ্রুবক {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ফেরত u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t সূচক, int32_t delta) ধ্রুবক {
  // pin index
  int32_t len = length();
  যদি(সূচক<0) {
    সূচক=0;
  } অন্যথা যদি(সূচক>len) {
    সূচক=len;
  }

  ধ্রুবক char16_t *array = getArrayStart();
  যদি(delta>0) {
    U16_FWD_N(array, সূচক, len, delta);
  } অন্যথা {
    U16_BACK_N(array, 0, সূচক, -delta);
  }

  ফেরত সূচক;
}

শূন্য
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) ধ্রুবক
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  ধ্রুবক char16_t *array = getArrayStart();
  যদি(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) ধ্রুবক {
  int32_t len = length();
  যদি(U_SUCCESS(errorCode)) {
    যদি (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা {
      ধ্রুবক char16_t *array = getArrayStart();
      যদি(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ফেরত u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  ফেরত len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       অক্ষর *target,
                       int32_t targetCapacity,
                       গণনা EInvariant) ধ্রুবক
{
  // if the arguments are illegal, then do nothing
  যদি(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    ফেরত 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  যদি(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  ফেরত u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::অস্থায়ী_Sub_String(int32_t start, int32_t len) ধ্রুবক U_LIFETIME_BOUND {
  pinIndices(start, len);
  ধ্রুবক char16_t *array = get_বাফার();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  যদি(array==nullptr) {
    array=fUnion.f_স্তূপ_Fields.f_বাফার;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ফেরত UnicodeString(মিথ্যা, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      অক্ষর *target, int32_t capacity) ধ্রুবক {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     get_বাফার() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  ফেরত length8;
}

#যদি U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       অক্ষর *target, uint32_t dst_আকার) ধ্রুবক {
  // if the arguments are illegal, then do nothing
  যদি (/*dstSize < 0 || */(dst_আকার > 0 && target == nullptr)) {
    ফেরত 0;
  }
  ফেরত toUTF8(start, len, target, dst_আকার <= 0x7fffffff ? static_cast<int32_t>(dst_আকার) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

শূন্য 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) ধ্রুবক {
  pin_সূচক(start);
  pin_সূচক(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
শূন্য
UnicodeString::toUTF8(ByteSink &sink) ধ্রুবক {
  int32_t length16 = length();
  যদি(length16 != 0) {
    অক্ষর স্তূপ_বাফার[1024];
    int32_t capacity = static_cast<int32_t>(আকার(স্তূপ_বাফার));
    UBool utf8IsOwned = মিথ্যা;
    অক্ষর *utf8 = sink.Get_Append_বাফার(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      স্তূপ_বাফার, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       get_বাফার(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    যদি(errorCode == U_বাফার_OVERFLOW_ERROR) {
      utf8 = static_cast<অক্ষর*>(uprv_malloc(length8));
      যদি(utf8 != nullptr) {
        utf8IsOwned = সত্য;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           get_বাফার(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } অন্যথা {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    যদি(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    যদি(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) ধ্রুবক {
  int32_t length32=0;
  যদি(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        get_বাফার(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  ফেরত length32;
}

int32_t 
UnicodeString::সূচক_Of(ধ্রুবক char16_t *srcChars,
               int32_t srcStart,
               int32_t src_দৈর্ঘ্য,
               int32_t start,
               int32_t length) ধ্রুবক
{
  যদি (isBogus() || srcChars == nullptr || srcStart < 0 || src_দৈর্ঘ্য == 0) {
    ফেরত -1;
  }

  // UnicodeString does not find empty substrings
  যদি(src_দৈর্ঘ্য < 0 && srcChars[srcStart] == 0) {
    ফেরত -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, src_দৈর্ঘ্য);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_সূচক_Of(char16_t c,
             int32_t start,
             int32_t length) ধ্রুবক
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_memchr(array + start, c, length);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_সূচক_Of(UChar32 c,
                         int32_t start,
                         int32_t length) ধ্রুবক {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_memchr32(array + start, c, length);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::last_সূচক_Of(ধ্রুবক char16_t *srcChars,
               int32_t srcStart,
               int32_t src_দৈর্ঘ্য,
               int32_t start,
               int32_t length) ধ্রুবক
{
  যদি (isBogus() || srcChars == nullptr || srcStart < 0 || src_দৈর্ঘ্য == 0) {
    ফেরত -1;
  }

  // UnicodeString does not find empty substrings
  যদি(src_দৈর্ঘ্য < 0 && srcChars[srcStart] == 0) {
    ফেরত -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, src_দৈর্ঘ্য);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_Last_সূচক_Of(char16_t c,
                 int32_t start,
                 int32_t length) ধ্রুবক
{
  যদি(isBogus()) {
    ফেরত -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_memrchr(array + start, c, length);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::do_Last_সূচক_Of(UChar32 c,
                             int32_t start,
                             int32_t length) ধ্রুবক {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ধ্রুবক char16_t *array = getArrayStart();
  ধ্রুবক char16_t *match = u_memrchr32(array + start, c, length);
  যদি(match == nullptr) {
    ফেরত -1;
  } অন্যথা {
    ফেরত static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              ধ্রুবক UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t old_দৈর্ঘ্য,
                              ধ্রুবক UnicodeString& newText,
                              int32_t newStart,
                              int32_t new_দৈর্ঘ্য) U_LIFETIME_BOUND
{
  যদি(isBogus() || oldText.isBogus() || newText.isBogus()) {
    ফেরত *এই;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, old_দৈর্ঘ্য);
  newText.pinIndices(newStart, new_দৈর্ঘ্য);

  যদি(old_দৈর্ঘ্য == 0) {
    ফেরত *এই;
  }

  যখন(length > 0 && length >= old_দৈর্ঘ্য) {
    int32_t pos = সূচক_Of(oldText, oldStart, old_দৈর্ঘ্য, start, length);
    যদি(pos < 0) {
      // no more oldText's here: done
      বিরতি;
    } অন্যথা {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, old_দৈর্ঘ্য, newText, newStart, new_দৈর্ঘ্য);
      length -= pos + old_দৈর্ঘ্য - start;
      start = pos + new_দৈর্ঘ্য;
    }
  }

  ফেরত *এই;
}


শূন্য
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
শূন্য
UnicodeString::unBogus() {
  যদি(fUnion.fFields.f_দৈর্ঘ্য_And_Flags & kIsBogus) {
    setToEmpty();
  }
}

ধ্রুবক char16_t*
UnicodeString::get_Terminated_বাফার() U_LIFETIME_BOUND {
  যদি(!isWritable()) {
    ফেরত nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  যদি(len < getCapacity()) {
    যদি(fUnion.fFields.f_দৈর্ঘ্য_And_Flags & k_বাফার_Is_Readonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      যদি(array[len] == 0) {
        ফেরত array;
      }
    } অন্যথা যদি(((fUnion.fFields.f_দৈর্ঘ্য_And_Flags & kRefCounted) == 0 || ref_গুনতি() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ফেরত array;
    }
  }
  যদি(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    ফেরত array;
  } অন্যথা {
    ফেরত nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t text_দৈর্ঘ্য) U_LIFETIME_BOUND
{
  যদি(fUnion.fFields.f_দৈর্ঘ্য_And_Flags & k_Open_Get_বাফার) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ফেরত *এই;
  }

  ধ্রুবক char16_t *text = textPtr;
  যদি(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ফেরত *এই;
  }

  যদি( text_দৈর্ঘ্য < -1 ||
      (text_দৈর্ঘ্য == -1 && !isTerminated) ||
      (text_দৈর্ঘ্য >= 0 && isTerminated && text[text_দৈর্ঘ্য] != 0)
  ) {
    setToBogus();
    ফেরত *এই;
  }

  releaseArray();

  যদি(text_দৈর্ঘ্য == -1) {
    // text is terminated, or else it would have failed the above test
    text_দৈর্ঘ্য = u_strlen(text);
  }
  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), text_দৈর্ঘ্য, isTerminated ? text_দৈর্ঘ্য + 1 : text_দৈর্ঘ্য);
  ফেরত *এই;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* বাফার,
                     int32_t buff_দৈর্ঘ্য U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  যদি(fUnion.fFields.f_দৈর্ঘ্য_And_Flags & k_Open_Get_বাফার) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ফেরত *এই;
  }

  যদি(বাফার == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ফেরত *এই;
  }

  যদি(buff_দৈর্ঘ্য < -1 || buffCapacity < 0 || buff_দৈর্ঘ্য > buffCapacity) {
    setToBogus();
    ফেরত *এই;
  } অন্যথা যদি(buff_দৈর্ঘ্য == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ধ্রুবক char16_t *p = বাফার, *limit = বাফার + buffCapacity;
    যখন(p != limit && *p != 0) {
      ++p;
    }
    buff_দৈর্ঘ্য = static_cast<int32_t>(p - বাফার);
  }

  releaseArray();

  fUnion.fFields.f_দৈর্ঘ্য_And_Flags = kWritableAlias;
  setArray(বাফার, buff_দৈর্ঘ্য, buffCapacity);
  ফেরত *এই;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  যদি(length <= US_STACKBUF_আকার) {
    capacity = US_STACKBUF_আকার;
  } অন্যথা {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = get_বাফার(capacity);
  int32_t length16;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, getCapacity(), &length16,
      utf8.তথ্য(), length,
      0xfffd,  // Substitution character.
      nullptr,    // Don't care about number of substitutions.
      &errorCode);
  release_বাফার(length16);
  যদি(U_FAILURE(errorCode)) {
    setToBogus();
  }
  ফেরত *এই;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  যদি(cloneArrayIfNeeded() && len > 0) {
    যদি(offset < 0) {
      offset = 0;
    } অন্যথা যদি(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  ফেরত *এই;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t দৈর্ঘ্য_4,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t বাফার[U16_MAX_দৈর্ঘ্য];
  int32_t count = 0;
  UBool isError = মিথ্যা;
  U16_APPEND(বাফার, count, U16_MAX_দৈর্ঘ্য, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ফেরত doReplace(start, দৈর্ঘ্য_4, বাফার, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t বাফার[U16_MAX_দৈর্ঘ্য];
  int32_t দৈর্ঘ্য_4 = 0;
  UBool isError = মিথ্যা;
  U16_APPEND(বাফার, দৈর্ঘ্য_4, U16_MAX_দৈর্ঘ্য, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ফেরত isError ? *এই : doAppend(বাফার, 0, দৈর্ঘ্য_4);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ধ্রুবক UnicodeString& src,
                         int32_t srcStart,
                         int32_t src_দৈর্ঘ্য) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, src_দৈর্ঘ্য);

  // get the characters from src
  // and replace the range in ourselves with them
  ফেরত doReplace(start, length, src.getArrayStart(), srcStart, src_দৈর্ঘ্য);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ধ্রুবক char16_t* srcChars,
                         int32_t srcStart,
                         int32_t src_দৈর্ঘ্য) U_LIFETIME_BOUND
{
  যদি(!isWritable()) {
    ফেরত *এই;
  }

  int32_t old_দৈর্ঘ্য = এই->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  যদি((fUnion.fFields.f_দৈর্ঘ্য_And_Flags&k_বাফার_Is_Readonly) && src_দৈর্ঘ্য == 0) {
    যদি(start == 0) {
      // remove prefix by adjusting the array pointer
      pin_সূচক(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      set_দৈর্ঘ্য(old_দৈর্ঘ্য - length);
      ফেরত *এই;
    } অন্যথা {
      pin_সূচক(start);
      যদি(length >= (old_দৈর্ঘ্য - start)) {
        // remove suffix by reducing the length (like truncate())
        set_দৈর্ঘ্য(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        ফেরত *এই;
      }
    }
  }

  যদি(start == old_দৈর্ঘ্য) {
    ফেরত doAppend(srcChars, srcStart, src_দৈর্ঘ্য);
  }

  যদি (srcChars == nullptr) {
    src_দৈর্ঘ্য = 0;
  } অন্যথা {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    যদি (src_দৈর্ঘ্য < 0) {
      // get the srcLength if necessary
      src_দৈর্ঘ্য = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t new_দৈর্ঘ্য = old_দৈর্ঘ্য - length;
  যদি(src_দৈর্ঘ্য > (INT32_MAX - new_দৈর্ঘ্য)) {
    setToBogus();
    ফেরত *এই;
  }
  new_দৈর্ঘ্য += src_দৈর্ঘ্য;

  // Check for insertion into ourself
  ধ্রুবক char16_t *oldArray = getArrayStart();
  যদি (is_বাফার_Writable() &&
      oldArray < srcChars + src_দৈর্ঘ্য &&
      srcChars < oldArray + old_দৈর্ঘ্য) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, src_দৈর্ঘ্য);
    যদি (copy.isBogus()) {
      setToBogus();
      ফেরত *এই;
    }
    ফেরত doReplace(start, length, copy.getArrayStart(), 0, src_দৈর্ঘ্য);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_স্তূপ_বাফার[US_STACKBUF_আকার];
  যদি((fUnion.fFields.f_দৈর্ঘ্য_And_Flags&k_Using_স্তূপ_বাফার) && (new_দৈর্ঘ্য > US_STACKBUF_আকার)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_স্তূপ_বাফার, oldArray, old_দৈর্ঘ্য);
    oldArray = old_স্তূপ_বাফার;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *বাফার_To_Delete = nullptr;
  যদি(!cloneArrayIfNeeded(new_দৈর্ঘ্য, getGrowCapacity(new_দৈর্ঘ্য),
                         মিথ্যা, &বাফার_To_Delete)
  ) {
    ফেরত *এই;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  যদি(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_দৈর্ঘ্য,
                 old_দৈর্ঘ্য - (start + length));
  } অন্যথা যদি(length != src_দৈর্ঘ্য) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, start + length,
                 newArray, start + src_দৈর্ঘ্য,
                 old_দৈর্ঘ্য - (start + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, start, src_দৈর্ঘ্য);

  set_দৈর্ঘ্য(new_দৈর্ঘ্য);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  যদি (বাফার_To_Delete) {
    uprv_free(বাফার_To_Delete);
  }

  ফেরত *এই;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  যদি (!isWritable()) {
    ফেরত *এই;
  }
  যদি (src.length() > INT32_MAX) {
    setToBogus();
    ফেরত *এই;
  }
  ফেরত doReplace(start, length, src.তথ্য(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ধ্রুবক UnicodeString& src,
                        int32_t srcStart,
                        int32_t src_দৈর্ঘ্য) U_LIFETIME_BOUND {
  যদি(src_দৈর্ঘ্য == 0) {
    ফেরত *এই;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, src_দৈর্ঘ্য);
  ফেরত doAppend(src.getArrayStart(), srcStart, src_দৈর্ঘ্য);
}

UnicodeString&
UnicodeString::doAppend(ধ্রুবক char16_t* srcChars,
                        int32_t srcStart,
                        int32_t src_দৈর্ঘ্য) U_LIFETIME_BOUND {
  যদি(!isWritable() || src_দৈর্ঘ্য == 0 || srcChars == nullptr) {
    ফেরত *এই;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  যদি(src_দৈর্ঘ্য < 0) {
    // get the srcLength if necessary
    যদি((src_দৈর্ঘ্য = u_strlen(srcChars)) == 0) {
      ফেরত *এই;
    }
  }

  int32_t old_দৈর্ঘ্য = length();
  int32_t new_দৈর্ঘ্য;

  যদি (src_দৈর্ঘ্য <= getCapacity() - old_দৈর্ঘ্য && is_বাফার_Writable()) {
    new_দৈর্ঘ্য = old_দৈর্ঘ্য + src_দৈর্ঘ্য;
    // Faster than a memmove
    যদি (src_দৈর্ঘ্য <= 4) {
      char16_t *arr = getArrayStart();
      arr[old_দৈর্ঘ্য] = srcChars[0];
      যদি (src_দৈর্ঘ্য > 1) arr[old_দৈর্ঘ্য+1] = srcChars[1];
      যদি (src_দৈর্ঘ্য > 2) arr[old_দৈর্ঘ্য+2] = srcChars[2];
      যদি (src_দৈর্ঘ্য > 3) arr[old_দৈর্ঘ্য+3] = srcChars[3];
      set_দৈর্ঘ্য(new_দৈর্ঘ্য);
      ফেরত *এই;
    }
  } অন্যথা {
    যদি (uprv_add32_overflow(old_দৈর্ঘ্য, src_দৈর্ঘ্য, &new_দৈর্ঘ্য)) {
      setToBogus();
      ফেরত *এই;
    }

    // Check for append onto ourself
    ধ্রুবক char16_t* oldArray = getArrayStart();
    যদি (is_বাফার_Writable() &&
        oldArray < srcChars + src_দৈর্ঘ্য &&
        srcChars < oldArray + old_দৈর্ঘ্য) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, src_দৈর্ঘ্য);
      যদি (copy.isBogus()) {
        setToBogus();
        ফেরত *এই;
      }
      ফেরত doAppend(copy.getArrayStart(), 0, src_দৈর্ঘ্য);
    }

    // optimize append() onto a large-enough, owned string
    যদি (!cloneArrayIfNeeded(new_দৈর্ঘ্য, getGrowCapacity(new_দৈর্ঘ্য))) {
      ফেরত *এই;
    }
  }

  char16_t *newArray = getArrayStart();
  // Do not copy characters when
  //   char16_t *buffer=str.getAppendBuffer(...);
  // is followed by
  //   str.append(buffer, length);
  // or
  //   str.appendString(buffer, length)
  // or similar.
  যদি(srcChars != newArray + old_দৈর্ঘ্য) {
    us_arrayCopy(srcChars, 0, newArray, old_দৈর্ঘ্য, src_দৈর্ঘ্য);
  }
  set_দৈর্ঘ্য(new_দৈর্ঘ্য);

  ফেরত *এই;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  যদি (!isWritable() || src.empty()) {
    ফেরত *এই;
  }
  যদি (src.length() > INT32_MAX) {
    setToBogus();
    ফেরত *এই;
  }
  ফেরত doAppend(src.তথ্য(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
শূন্য
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    ধ্রুবক UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
শূন্য 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    যদি (limit <= start) {
        ফেরত; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(আকার(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    যদি (text != nullptr) {
	    extractBetween(start, limit, text, 0);
	    insert(dest, text, 0, limit - start);    
	    uprv_free(text);
    }
}

/**
 * Replaceable API
 *
 * NOTE: This is for the Replaceable class.  There is no rep.cpp,
 * so we implement this function here.
 */
UBool Replaceable::has_Meta_তথ্য() ধ্রুবক {
    ফেরত সত্য;
}

/**
 * Replaceable API
 */
UBool UnicodeString::has_Meta_তথ্য() ধ্রুবক {
    ফেরত মিথ্যা;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  যদি(length <= 1 || !cloneArrayIfNeeded()) {
    ফেরত *এই;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  যদি(length <= 1) {  // pinIndices() might have shrunk the length
    ফেরত *এই;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *ডান = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = মিথ্যা;

  // Before the loop we know left<right because length>=2.
  করো {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *ডান));
    *ডান-- = swap;
  } যখন(left < ডান);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  যদি(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    ডান = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    যখন(left < ডান) {
      যদি(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } অন্যথা {
        ++left;
      }
    }
  }

  ফেরত *এই;
}

UBool 
UnicodeString::padLeading(int32_t target_দৈর্ঘ্য,
                          char16_t padChar)
{
  int32_t old_দৈর্ঘ্য = length();
  যদি(old_দৈর্ঘ্য >= target_দৈর্ঘ্য || !cloneArrayIfNeeded(target_দৈর্ঘ্য)) {
    ফেরত মিথ্যা;
  } অন্যথা {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = target_দৈর্ঘ্য - old_দৈর্ঘ্য;
    us_arrayCopy(array, 0, array, start, old_দৈর্ঘ্য);

    // fill in padding character
    যখন(--start >= 0) {
      array[start] = padChar;
    }
    set_দৈর্ঘ্য(target_দৈর্ঘ্য);
    ফেরত সত্য;
  }
}

UBool 
UnicodeString::padTrailing(int32_t target_দৈর্ঘ্য,
                           char16_t padChar)
{
  int32_t old_দৈর্ঘ্য = length();
  যদি(old_দৈর্ঘ্য >= target_দৈর্ঘ্য || !cloneArrayIfNeeded(target_দৈর্ঘ্য)) {
    ফেরত মিথ্যা;
  } অন্যথা {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = target_দৈর্ঘ্য;
    যখন(--length >= old_দৈর্ঘ্য) {
      array[length] = padChar;
    }
    set_দৈর্ঘ্য(target_দৈর্ঘ্য);
    ফেরত সত্য;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() ধ্রুবক
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    যদি (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    ফেরত hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::get_বাফার(int32_t minCapacity) U_LIFETIME_BOUND {
  যদি(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags|=k_Open_Get_বাফার;
    set_Zero_দৈর্ঘ্য();
    ফেরত getArrayStart();
  } অন্যথা {
    ফেরত nullptr;
  }
}

শূন্য
UnicodeString::release_বাফার(int32_t new_দৈর্ঘ্য) {
  যদি(fUnion.fFields.f_দৈর্ঘ্য_And_Flags&k_Open_Get_বাফার && new_দৈর্ঘ্য>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    যদি(new_দৈর্ঘ্য==-1) {
      // the new length is the string length, capped by fCapacity
      ধ্রুবক char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      যখন(p<limit && *p!=0) {
        ++p;
      }
      new_দৈর্ঘ্য = static_cast<int32_t>(p - array);
    } অন্যথা যদি(new_দৈর্ঘ্য>capacity) {
      new_দৈর্ঘ্য=capacity;
    }
    set_দৈর্ঘ্য(new_দৈর্ঘ্য);
    fUnion.fFields.f_দৈর্ঘ্য_And_Flags&=~k_Open_Get_বাফার;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t newCapacity,
                                  int32_t growCapacity,
                                  UBool doCopyArray,
                                  int32_t **p_বাফার_To_Delete,
                                  UBool forceClone) {
  // default parameters need to be static, therefore
  // the defaults are -1 to have convenience defaults
  যদি(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  যদি(!isWritable()) {
    ফেরত মিথ্যা;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  যদি(forceClone ||
     fUnion.fFields.f_দৈর্ঘ্য_And_Flags & k_বাফার_Is_Readonly ||
     (fUnion.fFields.f_দৈর্ঘ্য_And_Flags & kRefCounted && ref_গুনতি() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    যদি(growCapacity < 0) {
      growCapacity = newCapacity;
    } অন্যথা যদি(newCapacity <= US_STACKBUF_আকার && growCapacity > US_STACKBUF_আকার) {
      growCapacity = US_STACKBUF_আকার;
    } অন্যথা যদি(newCapacity > growCapacity) {
      setToBogus();
      ফেরত মিথ্যা;  // bad inputs
    }
    যদি(growCapacity > kMaxCapacity) {
      setToBogus();
      ফেরত মিথ্যা;
    }

    // save old values
    char16_t old_স্তূপ_বাফার[US_STACKBUF_আকার];
    char16_t *oldArray;
    int32_t old_দৈর্ঘ্য = length();
    int16_t flags = fUnion.fFields.f_দৈর্ঘ্য_And_Flags;

    যদি(flags&k_Using_স্তূপ_বাফার) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      যদি(doCopyArray && growCapacity > US_STACKBUF_আকার) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_স্তূপ_Fields.f_বাফার, 0, old_স্তূপ_বাফার, 0, old_দৈর্ঘ্য);
        oldArray = old_স্তূপ_বাফার;
      } অন্যথা {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } অন্যথা {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    যদি(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      যদি(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t min_দৈর্ঘ্য = old_দৈর্ঘ্য;
        newCapacity = getCapacity();
        যদি(newCapacity < min_দৈর্ঘ্য) {
          min_দৈর্ঘ্য = newCapacity;
        }
        যদি(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, min_দৈর্ঘ্য);
        }
        set_দৈর্ঘ্য(min_দৈর্ঘ্য);
      } অন্যথা {
        set_Zero_দৈর্ঘ্য();
      }

      // release the old array
      যদি(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_গুনতি = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        যদি(umtx_atomic_dec(p_Ref_গুনতি) == 0) {
          যদি (p_বাফার_To_Delete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((শূন্য *)p_Ref_গুনতি);
          } অন্যথা {
            // the caller requested to delete it himself
            *p_বাফার_To_Delete = reinterpret_cast<int32_t*>(p_Ref_গুনতি);
          }
        }
      }
    } অন্যথা {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      যদি(!(flags&k_Using_স্তূপ_বাফার)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.f_দৈর্ঘ্য_And_Flags = flags;
      setToBogus();
      ফেরত মিথ্যা;
    }
  }
  ফেরত সত্য;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  ফেরত str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t বাফার[U16_MAX_দৈর্ঘ্য];
  int32_t c_দৈর্ঘ্য = 0;
  UBool isError = মিথ্যা;
  U16_APPEND(বাফার, c_দৈর্ঘ্য, U16_MAX_দৈর্ঘ্য, c, isError);
  ফেরত !isError && str.doAppend(বাফার, 0, c_দৈর্ঘ্য).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ধ্রুবক char16_t *s, int32_t length) {
  ফেরত str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  ফেরত str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::get_Append_বাফার(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* ফলাফল_Capacity) U_LIFETIME_BOUND {
  যদি(minCapacity < 1 || scratchCapacity < minCapacity) {
    *ফলাফল_Capacity = 0;
    ফেরত nullptr;
  }
  int32_t old_দৈর্ঘ্য = str.length();
  যদি(minCapacity <= (kMaxCapacity - old_দৈর্ঘ্য) &&
      desiredCapacityHint <= (kMaxCapacity - old_দৈর্ঘ্য) &&
      str.cloneArrayIfNeeded(old_দৈর্ঘ্য + minCapacity, old_দৈর্ঘ্য + desiredCapacityHint)) {
    *ফলাফল_Capacity = str.getCapacity() - old_দৈর্ঘ্য;
    ফেরত str.getArrayStart() + old_দৈর্ঘ্য;
  }
  *ফলাফল_Capacity = scratchCapacity;
  ফেরত scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ধ্রুবক UElement চাবি) {
    ধ্রুবক UnicodeString *str = (ধ্রুবক UnicodeString*) চাবি.pointer;
    ফেরত (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ধ্রুবক UElement key1, ধ্রুবক UElement key2) {
    ধ্রুবক UnicodeString *str1 = (ধ্রুবক UnicodeString*) key1.pointer;
    ধ্রুবক UnicodeString *str2 = (ধ্রুবক UnicodeString*) key2.pointer;
    যদি (str1 == str2) {
        ফেরত সত্য;
    }
    যদি (str1 == nullptr || str2 == nullptr) {
        ফেরত মিথ্যা;
    }
    ফেরত *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#যদি defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
স্থির শূন্য uprv_UnicodeStringDummy() {
    delete [] (নতুন UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
