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

#অন্তর্ভাৱ <string_view>

#অন্তর্ভাৱ "unicode/utypes.h"
#অন্তর্ভাৱ "unicode/appendable.h"
#অন্তর্ভাৱ "unicode/putil.h"
#অন্তর্ভাৱ "cstring.h"
#অন্তর্ভাৱ "cmemory.h"
#অন্তর্ভাৱ "unicode/ustring.h"
#অন্তর্ভাৱ "unicode/unistr.h"
#অন্তর্ভাৱ "unicode/utf.h"
#অন্তর্ভাৱ "unicode/utf16.h"
#অন্তর্ভাৱ "uelement.h"
#অন্তর্ভাৱ "ustr_imp.h"
#অন্তর্ভাৱ "umutex.h"
#অন্তর্ভাৱ "uassert.h"

#যদি 0

#অন্তর্ভাৱ <iostream>
using নামাকাশ std;

//DEBUGGING
শূন্য
print(ধ্রুৱ UnicodeString& s,
      ধ্রুৱ ৱর্ণ *name)
{
  char16_t c;
  cout << name << ":|";
  বাবে(পূর্ণাঙ্ক i = 0; i < s.length(); ++i) {
    c = s[i];
    যদি(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    অন্যথা
      cout << (ৱর্ণ) s[i];
  }
  cout << '|' << endl;
}

শূন্য
print(ধ্রুৱ char16_t *s,
      int32_t len,
      ধ্রুৱ ৱর্ণ *name)
{
  char16_t c;
  cout << name << ":|";
  বাবে(পূর্ণাঙ্ক i = 0; i < len; ++i) {
    c = s[i];
    যদি(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    অন্যথা
      cout << (ৱর্ণ) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
স্থির
অন্তঃস্থ শূন্য
us_arrayCopy(ধ্রুৱ char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  যদি(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*পরিমাণ(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
স্থির char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, শূন্য *context) {
    ঘূৰাই ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (ধ্রুৱ UnicodeString &s1, ধ্রুৱ UnicodeString &s2) {
  int32_t sumLengths;
  যদি (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ঘূৰাই bogus;
  }
  যদি (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ঘূৰাই UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(ধ্রুৱ UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  যদি (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ঘূৰাই bogus;
  }
  যদি (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ঘূৰাই UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
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
  ঘূৰাই umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_গুনতি() ধ্রুৱ {
  ঘূৰাই umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

শূন্য
UnicodeString::releaseArray() {
  যদি((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
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
      বাবে(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } অন্যথা {  // supplementary code point, write surrogate pairs
    যদি(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ঘূৰাই;
    }
    int32_t length = count * 2;
    যদি(capacity < length) {
      capacity = length;
    }
    যদি(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      বাবে(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_স্তূপ_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = মিছা;
  U16_APPEND(fUnion.f_স্তূপ_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  যদি(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(ধ্রুৱ char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  ধ্রুৱ char16_t *text = textPtr;
  যদি(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } অন্যথা যদি(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } অন্যথা {
    যদি(textLength == -1) {
      // text is terminated, or else it would have failed the above test
      textLength = u_strlen(text);
    }
    setArray(const_cast<char16_t *>(text), textLength,
             isTerminated ? textLength + 1 : textLength);
  }
}

UnicodeString::UnicodeString(char16_t* buff U_LIFETIME_BOUND,
                             int32_t buffLength,
                             int32_t buffCapacity) {
  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  যদি(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } অন্যথা যদি(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } অন্যথা {
    যদি(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ধ্রুৱ char16_t *p = buff, *limit = buff + buffCapacity;
      যেতিয়ালৈ(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(ধ্রুৱ ৱর্ণ *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  যদি(src==nullptr) {
    // treat as an empty string
  } অন্যথা {
    যদি(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    যদি(cloneArrayIfNeeded(length, length, মিছা)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } অন্যথা {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  যদি (text.length() <= INT32_MAX) {
    result.setTo(মিছা, text.data(), static_cast<int32_t>(text.length()));
  } অন্যথা {
    result.setToBogus();
  }
  ঘূৰাই result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ধ্রুৱ UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  যদি (text.isBogus()) {
    result.setToBogus();
  } অন্যথা {
    result.setTo(মিছা, text.getBuffer(), text.length());
  }
  ঘূৰাই result;
}

#যদি U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ধ্রুৱ ৱর্ণ *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  যদি (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(ধ্রুৱ ৱর্ণ *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  যদি (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    ঘূৰাই;
  }
  যদি(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(ধ্রুৱ UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, সঁচা);
}

UnicodeString::UnicodeString(ধ্রুৱ UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(ধ্রুৱ UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ধ্রুৱ {
  ঘূৰাই nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ধ্রুৱ {
  LocalPointer<UnicodeString> clonedString(নূতন UnicodeString(*এতত্));
  ঘূৰাই clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

নামাকাশ {

ধ্রুৱ int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ধ্রুৱ int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  যদি(growSize <= (kMaxCapacity - newLength)) {
    ঘূৰাই newLength + growSize;
  } অন্যথা {
    ঘূৰাই kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  যদি(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ঘূৰাই সঁচা;
  }
  যদি(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t অঙ্ক_Bytes = পরিমাণ(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    অঙ্ক_Bytes = (অঙ্ক_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(অঙ্ক_Bytes));
    যদি(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      অঙ্ক_Bytes -= পরিমাণ(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(অঙ্ক_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      ঘূৰাই সঁচা;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  ঘূৰাই মিছা;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_গুনতি_FINAL_STRING_LENGTHS
স্থির u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
স্থির u_atomic_int32_t beyond_গুনতি(0);

U_CAPI শূন্য unistr_printLengths() {
  int32_t i;
  বাবে(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_গুনতি;
  বাবে(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
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
  যদি((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    যদি(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
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
  UnicodeString result;
  result.setToUTF8(utf8);
  ঘূৰাই result;
}

UnicodeString UnicodeString::fromUTF32(ধ্রুৱ UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  যদি(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } অন্যথা {
    capacity = length + (length >> 4) + 4;
  }
  কৰা {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    যদি(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      অনুৱর্ত;
    } অন্যথা যদি(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    ভঙ্গ;
  } যেতিয়ালৈ(সঁচা);
  ঘূৰাই result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(ধ্রুৱ UnicodeString& src) U_LIFETIME_BOUND {
  ঘূৰাই copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ধ্রুৱ UnicodeString& src) U_LIFETIME_BOUND {
  ঘূৰাই copyFrom(src, সঁচা);
}

UnicodeString&
UnicodeString::copyFrom(ধ্রুৱ UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  যদি(এতত্ == &src) {
    ঘূৰাই *এতত্;
  }

  // is the right side bogus?
  যদি(src.isBogus()) {
    setToBogus();
    ঘূৰাই *এতত্;
  }

  // delete the current contents
  releaseArray();

  যদি(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    ঘূৰাই *এতত্;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  ৱিকল্প(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  পক্ষ kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_স্তূপ_Fields.fBuffer, src.fUnion.f_স্তূপ_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    ভঙ্গ;
  পক্ষ kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    যদি(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    ভঙ্গ;
  পক্ষ kReadonlyAlias:
    যদি(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      যদি(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      ভঙ্গ;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  পক্ষ kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    যদি(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      ভঙ্গ;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  স্ৱভাৱ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    ভঙ্গ;
  }

  ঘূৰাই *এতত্;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, সঁচা);
  ঘূৰাই *এতত্;
}

// Same as move assignment except without memory management.
শূন্য UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  যদি(lengthAndFlags & k_Using_স্তূপ_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    যদি(এতত্ != &src) {
      uprv_memcpy(fUnion.f_স্তূপ_Fields.fBuffer, src.fUnion.f_স্তূপ_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } অন্যথা {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    যদি(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    যদি(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

শূন্য UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*এতত্, মিছা);
  এতত্->copyFieldsFrom(other, মিছা);
  other.copyFieldsFrom(temp, মিছা);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ধ্রুৱ {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    যদি (result.isBogus()) {
        ঘূৰাই result;
    }
    ধ্রুৱ char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    বাবে (int32_t i=0;;) {
        যদি (i == len) {
            result.append(array, prev, len - prev);
            ভঙ্গ;
        }
        যদি (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            যদি (c < 0) {
                result.remove(); // return empty string
                ভঙ্গ; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    ঘূৰাই result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) ধ্রুৱ {
    ঘূৰাই u_unescapeAt(UnicodeString_charAt, &offset, length(), (শূন্য*)এতত্);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ধ্রুৱ char16_t *text, int32_t len) ধ্রুৱ {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ঘূৰাই uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              ধ্রুৱ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ধ্রুৱ
{
  // compare illegal string values
  যদি(isBogus()) {
    ঘূৰাই মিছা;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ঘূৰাই length == 0 ? সঁচা : মিছা;
  }

  // get the correct pointer
  ধ্রুৱ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  যদি(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  যদি (length != srcLength) {
    ঘূৰাই মিছা;
  }

  যদি(length == 0 || chars == srcChars) {
    ঘূৰাই সঁচা;
  }

  ঘূৰাই u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ধ্রুৱ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ধ্রুৱ
{
  // compare illegal string values
  যদি(isBogus()) {
    ঘূৰাই -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ঘূৰাই length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ধ্রুৱ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  যদি(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  যদি(length != srcLength) {
    যদি(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } অন্যথা {
      minLength = srcLength;
      lengthResult = 1;
    }
  } অন্যথা {
    minLength = length;
    lengthResult = 0;
  }

  /*
   * note that uprv_memcmp() returns an int but we return an int8_t;
   * we need to take care not to truncate the result -
   * one way to do this is to right-shift the value to
   * move the sign bit into the lower 8 bits and making sure that this
   * does not become 0 itself
   */

  যদি(minLength > 0 && chars != srcChars) {
    int32_t result;

#   যদি U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * পরিমাণ(char16_t));
      যদি(result != 0) {
        ঘূৰাই (int8_t)(result >> 15 | 1);
      }
#   অন্যথা
      // little-endian: compare char16_t units
      কৰা {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        যদি(result != 0) {
          ঘূৰাই static_cast<int8_t>(result >> 15 | 1);
        }
      } যেতিয়ালৈ(--minLength > 0);
#   endif
  }
  ঘূৰাই lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       ধ্রুৱ char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) ধ্রুৱ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  যদি(isBogus()) {
    ঘূৰাই -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  যদি(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, মিছা, সঁচা);
  /* translate the 32-bit result into an 8-bit one */
  যদি(diff!=0) {
    ঘূৰাই static_cast<int8_t>(diff >> 15 | 1);
  } অন্যথা {
    ঘূৰাই 0;
  }
}

int32_t
UnicodeString::getLength() ধ্রুৱ {
    ঘূৰাই length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) ধ্রুৱ {
  ঘূৰাই charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) ধ্রুৱ {
  ঘূৰাই char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) ধ্রুৱ
{
  int32_t len = length();
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ধ্রুৱ char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    ঘূৰাই c;
  } অন্যথা {
    ঘূৰাই kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) ধ্রুৱ {
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    ধ্রুৱ char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    ঘূৰাই offset;
  } অন্যথা {
    ঘূৰাই 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) ধ্রুৱ {
  int32_t len = length();
  যদি (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ধ্রুৱ char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    ঘূৰাই offset;
  } অন্যথা {
    ঘূৰাই len;
  }
}

int32_t
UnicodeString::গুনতি_Char32(int32_t start, int32_t length) ধ্রুৱ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ঘূৰাই u_গুনতি_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) ধ্রুৱ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ঘূৰাই u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) ধ্রুৱ {
  // pin index
  int32_t len = length();
  যদি(index<0) {
    index=0;
  } অন্যথা যদি(index>len) {
    index=len;
  }

  ধ্রুৱ char16_t *array = getArrayStart();
  যদি(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } অন্যথা {
    U16_BACK_N(array, 0, index, -delta);
  }

  ঘূৰাই index;
}

শূন্য
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) ধ্রুৱ
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  ধ্রুৱ char16_t *array = getArrayStart();
  যদি(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) ধ্রুৱ {
  int32_t len = length();
  যদি(U_SUCCESS(errorCode)) {
    যদি (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } অন্যথা {
      ধ্রুৱ char16_t *array = getArrayStart();
      যদি(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ঘূৰাই u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  ঘূৰাই len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ৱর্ণ *target,
                       int32_t targetCapacity,
                       গণনা EInvariant) ধ্রুৱ
{
  // if the arguments are illegal, then do nothing
  যদি(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    ঘূৰাই 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  যদি(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  ঘূৰাই u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) ধ্রুৱ U_LIFETIME_BOUND {
  pinIndices(start, len);
  ধ্রুৱ char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  যদি(array==nullptr) {
    array=fUnion.f_স্তূপ_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ঘূৰাই UnicodeString(মিছা, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      ৱর্ণ *target, int32_t capacity) ধ্রুৱ {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  ঘূৰাই length8;
}

#যদি U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       ৱর্ণ *target, uint32_t dstSize) ধ্রুৱ {
  // if the arguments are illegal, then do nothing
  যদি (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    ঘূৰাই 0;
  }
  ঘূৰাই toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

শূন্য 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) ধ্রুৱ {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
শূন্য
UnicodeString::toUTF8(ByteSink &sink) ধ্রুৱ {
  int32_t length16 = length();
  যদি(length16 != 0) {
    ৱর্ণ স্তূপ_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(পরিমাণ(স্তূপ_Buffer));
    UBool utf8IsOwned = মিছা;
    ৱর্ণ *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      স্তূপ_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    যদি(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<ৱর্ণ*>(uprv_malloc(length8));
      যদি(utf8 != nullptr) {
        utf8IsOwned = সঁচা;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
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
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) ধ্রুৱ {
  int32_t length32=0;
  যদি(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  ঘূৰাই length32;
}

int32_t 
UnicodeString::indexOf(ধ্রুৱ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ধ্রুৱ
{
  যদি (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ঘূৰাই -1;
  }

  // UnicodeString does not find empty substrings
  যদি(srcLength < 0 && srcChars[srcStart] == 0) {
    ঘূৰাই -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) ধ্রুৱ
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_memchr(array + start, c, length);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) ধ্রুৱ {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_memchr32(array + start, c, length);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(ধ্রুৱ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ধ্রুৱ
{
  যদি (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ঘূৰাই -1;
  }

  // UnicodeString does not find empty substrings
  যদি(srcLength < 0 && srcChars[srcStart] == 0) {
    ঘূৰাই -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) ধ্রুৱ
{
  যদি(isBogus()) {
    ঘূৰাই -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_memrchr(array + start, c, length);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) ধ্রুৱ {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ধ্রুৱ char16_t *array = getArrayStart();
  ধ্রুৱ char16_t *match = u_memrchr32(array + start, c, length);
  যদি(match == nullptr) {
    ঘূৰাই -1;
  } অন্যথা {
    ঘূৰাই static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              ধ্রুৱ UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              ধ্রুৱ UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  যদি(isBogus() || oldText.isBogus() || newText.isBogus()) {
    ঘূৰাই *এতত্;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  যদি(oldLength == 0) {
    ঘূৰাই *এতত্;
  }

  যেতিয়ালৈ(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    যদি(pos < 0) {
      // no more oldText's here: done
      ভঙ্গ;
    } অন্যথা {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  ঘূৰাই *এতত্;
}


শূন্য
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
শূন্য
UnicodeString::unBogus() {
  যদি(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

ধ্রুৱ char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  যদি(!isWritable()) {
    ঘূৰাই nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  যদি(len < getCapacity()) {
    যদি(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      যদি(array[len] == 0) {
        ঘূৰাই array;
      }
    } অন্যথা যদি(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_গুনতি() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ঘূৰাই array;
    }
  }
  যদি(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    ঘূৰাই array;
  } অন্যথা {
    ঘূৰাই nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  যদি(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ঘূৰাই *এতত্;
  }

  ধ্রুৱ char16_t *text = textPtr;
  যদি(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ঘূৰাই *এতত্;
  }

  যদি( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    ঘূৰাই *এতত্;
  }

  releaseArray();

  যদি(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  ঘূৰাই *এতত্;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  যদি(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ঘূৰাই *এতত্;
  }

  যদি(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ঘূৰাই *এতত্;
  }

  যদি(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    ঘূৰাই *এতত্;
  } অন্যথা যদি(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ধ্রুৱ char16_t *p = buffer, *limit = buffer + buffCapacity;
    যেতিয়ালৈ(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  ঘূৰাই *এতত্;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  যদি(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } অন্যথা {
    capacity = length + 1;  // +1 for the terminating NUL.
  }
  char16_t *utf16 = getBuffer(capacity);
  int32_t length16;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strFromUTF8WithSub(utf16, getCapacity(), &length16,
      utf8.data(), length,
      0xfffd,  // Substitution character.
      nullptr,    // Don't care about number of substitutions.
      &errorCode);
  releaseBuffer(length16);
  যদি(U_FAILURE(errorCode)) {
    setToBogus();
  }
  ঘূৰাই *এতত্;
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
  ঘূৰাই *এতত্;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = মিছা;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ঘূৰাই doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = মিছা;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ঘূৰাই isError ? *এতত্ : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ধ্রুৱ UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  ঘূৰাই doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ধ্রুৱ char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  যদি(!isWritable()) {
    ঘূৰাই *এতত্;
  }

  int32_t oldLength = এতত্->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  যদি((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    যদি(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      ঘূৰাই *এতত্;
    } অন্যথা {
      pinIndex(start);
      যদি(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        ঘূৰাই *এতত্;
      }
    }
  }

  যদি(start == oldLength) {
    ঘূৰাই doAppend(srcChars, srcStart, srcLength);
  }

  যদি (srcChars == nullptr) {
    srcLength = 0;
  } অন্যথা {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    যদি (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  যদি(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    ঘূৰাই *এতত্;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  ধ্রুৱ char16_t *oldArray = getArrayStart();
  যদি (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    যদি (copy.isBogus()) {
      setToBogus();
      ঘূৰাই *এতত্;
    }
    ঘূৰাই doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_স্তূপ_Buffer[US_STACKBUF_SIZE];
  যদি((fUnion.fFields.fLengthAndFlags&k_Using_স্তূপ_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_স্তূপ_Buffer, oldArray, oldLength);
    oldArray = old_স্তূপ_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  যদি(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         মিছা, &bufferToDelete)
  ) {
    ঘূৰাই *এতত্;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  যদি(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } অন্যথা যদি(length != srcLength) {
    // fArray did not change; copy only the portion that isn't changing, leaving a hole
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  }

  // now fill in the hole with the new string
  us_arrayCopy(srcChars, 0, newArray, start, srcLength);

  setLength(newLength);

  // delayed delete in case srcChars == fArray when we started, and
  // to keep oldArray alive for the above operations
  যদি (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  ঘূৰাই *এতত্;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  যদি (!isWritable()) {
    ঘূৰাই *এতত্;
  }
  যদি (src.length() > INT32_MAX) {
    setToBogus();
    ঘূৰাই *এতত্;
  }
  ঘূৰাই doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ধ্রুৱ UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  যদি(srcLength == 0) {
    ঘূৰাই *এতত্;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  ঘূৰাই doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(ধ্রুৱ char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  যদি(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    ঘূৰাই *এতত্;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  যদি(srcLength < 0) {
    // get the srcLength if necessary
    যদি((srcLength = u_strlen(srcChars)) == 0) {
      ঘূৰাই *এতত্;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  যদি (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    যদি (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      যদি (srcLength > 1) arr[oldLength+1] = srcChars[1];
      যদি (srcLength > 2) arr[oldLength+2] = srcChars[2];
      যদি (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      ঘূৰাই *এতত্;
    }
  } অন্যথা {
    যদি (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      ঘূৰাই *এতত্;
    }

    // Check for append onto ourself
    ধ্রুৱ char16_t* oldArray = getArrayStart();
    যদি (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      যদি (copy.isBogus()) {
        setToBogus();
        ঘূৰাই *এতত্;
      }
      ঘূৰাই doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    যদি (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      ঘূৰাই *এতত্;
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
  যদি(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  ঘূৰাই *এতত্;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  যদি (!isWritable() || src.empty()) {
    ঘূৰাই *এতত্;
  }
  যদি (src.length() > INT32_MAX) {
    setToBogus();
    ঘূৰাই *এতত্;
  }
  ঘূৰাই doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
শূন্য
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    ধ্রুৱ UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
শূন্য 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    যদি (limit <= start) {
        ঘূৰাই; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(পরিমাণ(char16_t) * (limit - start)));
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
UBool Replaceable::hasMetaData() ধ্রুৱ {
    ঘূৰাই সঁচা;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() ধ্রুৱ {
    ঘূৰাই মিছা;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  যদি(length <= 1 || !cloneArrayIfNeeded()) {
    ঘূৰাই *এতত্;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  যদি(length <= 1) {  // pinIndices() might have shrunk the length
    ঘূৰাই *এতত্;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *ডান = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = মিছা;

  // Before the loop we know left<right because length>=2.
  কৰা {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *ডান));
    *ডান-- = swap;
  } যেতিয়ালৈ(left < ডান);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  যদি(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    ডান = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    যেতিয়ালৈ(left < ডান) {
      যদি(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } অন্যথা {
        ++left;
      }
    }
  }

  ঘূৰাই *এতত্;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  যদি(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ঘূৰাই মিছা;
  } অন্যথা {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    যেতিয়ালৈ(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    ঘূৰাই সঁচা;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  যদি(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ঘূৰাই মিছা;
  } অন্যথা {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    যেতিয়ালৈ(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    ঘূৰাই সঁচা;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() ধ্রুৱ
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    যদি (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    ঘূৰাই hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  যদি(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    ঘূৰাই getArrayStart();
  } অন্যথা {
    ঘূৰাই nullptr;
  }
}

শূন্য
UnicodeString::releaseBuffer(int32_t newLength) {
  যদি(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    যদি(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      ধ্রুৱ char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      যেতিয়ালৈ(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } অন্যথা যদি(newLength>capacity) {
      newLength=capacity;
    }
    setLength(newLength);
    fUnion.fFields.fLengthAndFlags&=~kOpenGetBuffer;
  }
}

//========================================
// Miscellaneous
//========================================
UBool
UnicodeString::cloneArrayIfNeeded(int32_t newCapacity,
                                  int32_t growCapacity,
                                  UBool doCopyArray,
                                  int32_t **pBufferToDelete,
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
    ঘূৰাই মিছা;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  যদি(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_গুনতি() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    যদি(growCapacity < 0) {
      growCapacity = newCapacity;
    } অন্যথা যদি(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } অন্যথা যদি(newCapacity > growCapacity) {
      setToBogus();
      ঘূৰাই মিছা;  // bad inputs
    }
    যদি(growCapacity > kMaxCapacity) {
      setToBogus();
      ঘূৰাই মিছা;
    }

    // save old values
    char16_t old_স্তূপ_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    যদি(flags&k_Using_স্তূপ_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      যদি(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_স্তূপ_Fields.fBuffer, 0, old_স্তূপ_Buffer, 0, oldLength);
        oldArray = old_স্তূপ_Buffer;
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
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        যদি(newCapacity < minLength) {
          minLength = newCapacity;
        }
        যদি(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } অন্যথা {
        setZeroLength();
      }

      // release the old array
      যদি(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_গুনতি = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        যদি(umtx_atomic_dec(p_Ref_গুনতি) == 0) {
          যদি (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((শূন্য *)p_Ref_গুনতি);
          } অন্যথা {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_গুনতি);
          }
        }
      }
    } অন্যথা {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      যদি(!(flags&k_Using_স্তূপ_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      ঘূৰাই মিছা;
    }
  }
  ঘূৰাই সঁচা;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  ঘূৰাই str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = মিছা;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  ঘূৰাই !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ধ্রুৱ char16_t *s, int32_t length) {
  ঘূৰাই str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  ঘূৰাই str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  যদি(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    ঘূৰাই nullptr;
  }
  int32_t oldLength = str.length();
  যদি(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    ঘূৰাই str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  ঘূৰাই scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ধ্রুৱ UElement চাবি) {
    ধ্রুৱ UnicodeString *str = (ধ্রুৱ UnicodeString*) চাবি.pointer;
    ঘূৰাই (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ধ্রুৱ UElement key1, ধ্রুৱ UElement key2) {
    ধ্রুৱ UnicodeString *str1 = (ধ্রুৱ UnicodeString*) key1.pointer;
    ধ্রুৱ UnicodeString *str2 = (ধ্রুৱ UnicodeString*) key2.pointer;
    যদি (str1 == str2) {
        ঘূৰাই সঁচা;
    }
    যদি (str1 == nullptr || str2 == nullptr) {
        ঘূৰাই মিছা;
    }
    ঘূৰাই *str1 == *str2;
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
    delete [] (নূতন UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
