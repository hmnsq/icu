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

#अन्तर्भाव <string_view>

#अन्तर्भाव "unicode/utypes.h"
#अन्तर्भाव "unicode/appendable.h"
#अन्तर्भाव "unicode/putil.h"
#अन्तर्भाव "cstring.h"
#अन्तर्भाव "cmemory.h"
#अन्तर्भाव "unicode/ustring.h"
#अन्तर्भाव "unicode/unistr.h"
#अन्तर्भाव "unicode/utf.h"
#अन्तर्भाव "unicode/utf16.h"
#अन्तर्भाव "uelement.h"
#अन्तर्भाव "ustr_imp.h"
#अन्तर्भाव "umutex.h"
#अन्तर्भाव "uassert.h"

#जइ 0

#अन्तर्भाव <iostream>
using नामाकाश std;

//DEBUGGING
शून्य
print(ध्रुव UnicodeString& s,
      ध्रुव वर्ण *name)
{
  char16_t c;
  cout << name << ":|";
  प्रति(पूर्णाङ्क i = 0; i < s.length(); ++i) {
    c = s[i];
    जइ(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    अण्णहा
      cout << (वर्ण) s[i];
  }
  cout << '|' << endl;
}

शून्य
print(ध्रुव char16_t *s,
      int32_t len,
      ध्रुव वर्ण *name)
{
  char16_t c;
  cout << name << ":|";
  प्रति(पूर्णाङ्क i = 0; i < len; ++i) {
    c = s[i];
    जइ(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    अण्णहा
      cout << (वर्ण) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
स्थिर
अन्तःस्थ शून्य
us_arrayCopy(ध्रुव char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  जइ(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*परिमाण(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
स्थिर char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, शून्य *context) {
    प्रत्यागम ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (ध्रुव UnicodeString &s1, ध्रुव UnicodeString &s2) {
  int32_t sumLengths;
  जइ (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    प्रत्यागम bogus;
  }
  जइ (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  प्रत्यागम UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(ध्रुव UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  जइ (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    प्रत्यागम bogus;
  }
  जइ (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  प्रत्यागम UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

शून्य
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  प्रत्यागम umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_गणनम्() ध्रुव {
  प्रत्यागम umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

शून्य
UnicodeString::releaseArray() {
  जइ((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  जइ (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } अण्णहा जइ(c <= 0xffff) {
    int32_t length = count;
    जइ(capacity < length) {
      capacity = length;
    }
    जइ(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      प्रति(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } अण्णहा {  // supplementary code point, write surrogate pairs
    जइ(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      प्रत्यागम;
    }
    int32_t length = count * 2;
    जइ(capacity < length) {
      capacity = length;
    }
    जइ(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      प्रति(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_राशिः_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = मिच्छ;
  U16_APPEND(fUnion.f_राशिः_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  जइ(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(ध्रुव char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  ध्रुव char16_t *text = textPtr;
  जइ(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } अण्णहा जइ(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } अण्णहा {
    जइ(textLength == -1) {
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
  जइ(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } अण्णहा जइ(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } अण्णहा {
    जइ(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ध्रुव char16_t *p = buff, *limit = buff + buffCapacity;
      जाव(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  जइ(src==nullptr) {
    // treat as an empty string
  } अण्णहा {
    जइ(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    जइ(cloneArrayIfNeeded(length, length, मिच्छ)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } अण्णहा {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  जइ (text.length() <= INT32_MAX) {
    result.setTo(मिच्छ, text.data(), static_cast<int32_t>(text.length()));
  } अण्णहा {
    result.setToBogus();
  }
  प्रत्यागम result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ध्रुव UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  जइ (text.isBogus()) {
    result.setToBogus();
  } अण्णहा {
    result.setTo(मिच्छ, text.getBuffer(), text.length());
  }
  प्रत्यागम result;
}

#जइ U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  जइ (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(ध्रुव वर्ण *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  जइ (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    प्रत्यागम;
  }
  जइ(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(ध्रुव UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, सच्च);
}

UnicodeString::UnicodeString(ध्रुव UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(ध्रुव UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ध्रुव {
  प्रत्यागम nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ध्रुव {
  LocalPointer<UnicodeString> clonedString(नूतन UnicodeString(*एतत्));
  प्रत्यागम clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

नामाकाश {

ध्रुव int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ध्रुव int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  जइ(growSize <= (kMaxCapacity - newLength)) {
    प्रत्यागम newLength + growSize;
  } अण्णहा {
    प्रत्यागम kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  जइ(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    प्रत्यागम सच्च;
  }
  जइ(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t अङ्कः_Bytes = परिमाण(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    अङ्कः_Bytes = (अङ्कः_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(अङ्कः_Bytes));
    जइ(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      अङ्कः_Bytes -= परिमाण(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(अङ्कः_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      प्रत्यागम सच्च;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  प्रत्यागम मिच्छ;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_गणनम्_FINAL_STRING_LENGTHS
स्थिर u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
स्थिर u_atomic_int32_t beyond_गणनम्(0);

U_CAPI शून्य unistr_printLengths() {
  int32_t i;
  प्रति(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_गणनम्;
  प्रति(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_गणनम्_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  जइ((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    जइ(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } अण्णहा {
      umtx_atomic_inc(&beyond_गणनम्);
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
  प्रत्यागम result;
}

UnicodeString UnicodeString::fromUTF32(ध्रुव UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  जइ(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } अण्णहा {
    capacity = length + (length >> 4) + 4;
  }
  करेहि {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    जइ(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      अनुवर्त;
    } अण्णहा जइ(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    भङ्ग;
  } जाव(सच्च);
  प्रत्यागम result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(ध्रुव UnicodeString& src) U_LIFETIME_BOUND {
  प्रत्यागम copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ध्रुव UnicodeString& src) U_LIFETIME_BOUND {
  प्रत्यागम copyFrom(src, सच्च);
}

UnicodeString&
UnicodeString::copyFrom(ध्रुव UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  जइ(एतत् == &src) {
    प्रत्यागम *एतत्;
  }

  // is the right side bogus?
  जइ(src.isBogus()) {
    setToBogus();
    प्रत्यागम *एतत्;
  }

  // delete the current contents
  releaseArray();

  जइ(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    प्रत्यागम *एतत्;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  विकल्प(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  पक्ष kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_राशिः_Fields.fBuffer, src.fUnion.f_राशिः_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    भङ्ग;
  पक्ष kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    जइ(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    भङ्ग;
  पक्ष kReadonlyAlias:
    जइ(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      जइ(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      भङ्ग;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  पक्ष kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    जइ(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      भङ्ग;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  स्वभाव:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    भङ्ग;
  }

  प्रत्यागम *एतत्;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, सच्च);
  प्रत्यागम *एतत्;
}

// Same as move assignment except without memory management.
शून्य UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  जइ(lengthAndFlags & k_Using_राशिः_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    जइ(एतत् != &src) {
      uprv_memcpy(fUnion.f_राशिः_Fields.fBuffer, src.fUnion.f_राशिः_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } अण्णहा {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    जइ(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    जइ(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

शून्य UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*एतत्, मिच्छ);
  एतत्->copyFieldsFrom(other, मिच्छ);
  other.copyFieldsFrom(temp, मिच्छ);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ध्रुव {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    जइ (result.isBogus()) {
        प्रत्यागम result;
    }
    ध्रुव char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    प्रति (int32_t i=0;;) {
        जइ (i == len) {
            result.append(array, prev, len - prev);
            भङ्ग;
        }
        जइ (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            जइ (c < 0) {
                result.remove(); // return empty string
                भङ्ग; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    प्रत्यागम result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) ध्रुव {
    प्रत्यागम u_unescapeAt(UnicodeString_charAt, &offset, length(), (शून्य*)एतत्);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ध्रुव char16_t *text, int32_t len) ध्रुव {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  प्रत्यागम uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              ध्रुव char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ध्रुव
{
  // compare illegal string values
  जइ(isBogus()) {
    प्रत्यागम मिच्छ;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  जइ(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    प्रत्यागम length == 0 ? सच्च : मिच्छ;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  जइ(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  जइ (length != srcLength) {
    प्रत्यागम मिच्छ;
  }

  जइ(length == 0 || chars == srcChars) {
    प्रत्यागम सच्च;
  }

  प्रत्यागम u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ध्रुव char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ध्रुव
{
  // compare illegal string values
  जइ(isBogus()) {
    प्रत्यागम -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  जइ(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    प्रत्यागम length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ध्रुव char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  जइ(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  जइ(length != srcLength) {
    जइ(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } अण्णहा {
      minLength = srcLength;
      lengthResult = 1;
    }
  } अण्णहा {
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

  जइ(minLength > 0 && chars != srcChars) {
    int32_t result;

#   जइ U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * परिमाण(char16_t));
      जइ(result != 0) {
        प्रत्यागम (int8_t)(result >> 15 | 1);
      }
#   अण्णहा
      // little-endian: compare char16_t units
      करेहि {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        जइ(result != 0) {
          प्रत्यागम static_cast<int8_t>(result >> 15 | 1);
        }
      } जाव(--minLength > 0);
#   endif
  }
  प्रत्यागम lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       ध्रुव char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) ध्रुव
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  जइ(isBogus()) {
    प्रत्यागम -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  जइ(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, मिच्छ, सच्च);
  /* translate the 32-bit result into an 8-bit one */
  जइ(diff!=0) {
    प्रत्यागम static_cast<int8_t>(diff >> 15 | 1);
  } अण्णहा {
    प्रत्यागम 0;
  }
}

int32_t
UnicodeString::getLength() ध्रुव {
    प्रत्यागम length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) ध्रुव {
  प्रत्यागम charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) ध्रुव {
  प्रत्यागम char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) ध्रुव
{
  int32_t len = length();
  जइ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ध्रुव char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    प्रत्यागम c;
  } अण्णहा {
    प्रत्यागम kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) ध्रुव {
  जइ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    ध्रुव char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    प्रत्यागम offset;
  } अण्णहा {
    प्रत्यागम 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) ध्रुव {
  int32_t len = length();
  जइ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ध्रुव char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    प्रत्यागम offset;
  } अण्णहा {
    प्रत्यागम len;
  }
}

int32_t
UnicodeString::गणनम्_Char32(int32_t start, int32_t length) ध्रुव {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  प्रत्यागम u_गणनम्_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) ध्रुव {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  प्रत्यागम u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) ध्रुव {
  // pin index
  int32_t len = length();
  जइ(index<0) {
    index=0;
  } अण्णहा जइ(index>len) {
    index=len;
  }

  ध्रुव char16_t *array = getArrayStart();
  जइ(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } अण्णहा {
    U16_BACK_N(array, 0, index, -delta);
  }

  प्रत्यागम index;
}

शून्य
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) ध्रुव
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  ध्रुव char16_t *array = getArrayStart();
  जइ(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) ध्रुव {
  int32_t len = length();
  जइ(U_SUCCESS(errorCode)) {
    जइ (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } अण्णहा {
      ध्रुव char16_t *array = getArrayStart();
      जइ(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      प्रत्यागम u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  प्रत्यागम len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       वर्ण *target,
                       int32_t targetCapacity,
                       गणना EInvariant) ध्रुव
{
  // if the arguments are illegal, then do nothing
  जइ(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    प्रत्यागम 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  जइ(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  प्रत्यागम u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) ध्रुव U_LIFETIME_BOUND {
  pinIndices(start, len);
  ध्रुव char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  जइ(array==nullptr) {
    array=fUnion.f_राशिः_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  प्रत्यागम UnicodeString(मिच्छ, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      वर्ण *target, int32_t capacity) ध्रुव {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  प्रत्यागम length8;
}

#जइ U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       वर्ण *target, uint32_t dstSize) ध्रुव {
  // if the arguments are illegal, then do nothing
  जइ (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    प्रत्यागम 0;
  }
  प्रत्यागम toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

शून्य 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) ध्रुव {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
शून्य
UnicodeString::toUTF8(ByteSink &sink) ध्रुव {
  int32_t length16 = length();
  जइ(length16 != 0) {
    वर्ण राशिः_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(परिमाण(राशिः_Buffer));
    UBool utf8IsOwned = मिच्छ;
    वर्ण *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      राशिः_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    जइ(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<वर्ण*>(uprv_malloc(length8));
      जइ(utf8 != nullptr) {
        utf8IsOwned = सच्च;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } अण्णहा {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    जइ(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    जइ(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) ध्रुव {
  int32_t length32=0;
  जइ(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  प्रत्यागम length32;
}

int32_t 
UnicodeString::indexOf(ध्रुव char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ध्रुव
{
  जइ (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    प्रत्यागम -1;
  }

  // UnicodeString does not find empty substrings
  जइ(srcLength < 0 && srcChars[srcStart] == 0) {
    प्रत्यागम -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) ध्रुव
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_memchr(array + start, c, length);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) ध्रुव {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_memchr32(array + start, c, length);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(ध्रुव char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ध्रुव
{
  जइ (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    प्रत्यागम -1;
  }

  // UnicodeString does not find empty substrings
  जइ(srcLength < 0 && srcChars[srcStart] == 0) {
    प्रत्यागम -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) ध्रुव
{
  जइ(isBogus()) {
    प्रत्यागम -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_memrchr(array + start, c, length);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) ध्रुव {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ध्रुव char16_t *array = getArrayStart();
  ध्रुव char16_t *match = u_memrchr32(array + start, c, length);
  जइ(match == nullptr) {
    प्रत्यागम -1;
  } अण्णहा {
    प्रत्यागम static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              ध्रुव UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              ध्रुव UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  जइ(isBogus() || oldText.isBogus() || newText.isBogus()) {
    प्रत्यागम *एतत्;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  जइ(oldLength == 0) {
    प्रत्यागम *एतत्;
  }

  जाव(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    जइ(pos < 0) {
      // no more oldText's here: done
      भङ्ग;
    } अण्णहा {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  प्रत्यागम *एतत्;
}


शून्य
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
शून्य
UnicodeString::unBogus() {
  जइ(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

ध्रुव char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  जइ(!isWritable()) {
    प्रत्यागम nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  जइ(len < getCapacity()) {
    जइ(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      जइ(array[len] == 0) {
        प्रत्यागम array;
      }
    } अण्णहा जइ(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_गणनम्() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      प्रत्यागम array;
    }
  }
  जइ(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    प्रत्यागम array;
  } अण्णहा {
    प्रत्यागम nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  जइ(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    प्रत्यागम *एतत्;
  }

  ध्रुव char16_t *text = textPtr;
  जइ(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    प्रत्यागम *एतत्;
  }

  जइ( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    प्रत्यागम *एतत्;
  }

  releaseArray();

  जइ(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  प्रत्यागम *एतत्;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  जइ(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    प्रत्यागम *एतत्;
  }

  जइ(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    प्रत्यागम *एतत्;
  }

  जइ(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    प्रत्यागम *एतत्;
  } अण्णहा जइ(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ध्रुव char16_t *p = buffer, *limit = buffer + buffCapacity;
    जाव(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  प्रत्यागम *एतत्;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  जइ(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } अण्णहा {
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
  जइ(U_FAILURE(errorCode)) {
    setToBogus();
  }
  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  जइ(cloneArrayIfNeeded() && len > 0) {
    जइ(offset < 0) {
      offset = 0;
    } अण्णहा जइ(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = मिच्छ;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  प्रत्यागम doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = मिच्छ;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  प्रत्यागम isError ? *एतत् : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ध्रुव UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  प्रत्यागम doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ध्रुव char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  जइ(!isWritable()) {
    प्रत्यागम *एतत्;
  }

  int32_t oldLength = एतत्->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  जइ((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    जइ(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      प्रत्यागम *एतत्;
    } अण्णहा {
      pinIndex(start);
      जइ(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        प्रत्यागम *एतत्;
      }
    }
  }

  जइ(start == oldLength) {
    प्रत्यागम doAppend(srcChars, srcStart, srcLength);
  }

  जइ (srcChars == nullptr) {
    srcLength = 0;
  } अण्णहा {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    जइ (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  जइ(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    प्रत्यागम *एतत्;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  ध्रुव char16_t *oldArray = getArrayStart();
  जइ (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    जइ (copy.isBogus()) {
      setToBogus();
      प्रत्यागम *एतत्;
    }
    प्रत्यागम doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_राशिः_Buffer[US_STACKBUF_SIZE];
  जइ((fUnion.fFields.fLengthAndFlags&k_Using_राशिः_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_राशिः_Buffer, oldArray, oldLength);
    oldArray = old_राशिः_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  जइ(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         मिच्छ, &bufferToDelete)
  ) {
    प्रत्यागम *एतत्;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  जइ(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } अण्णहा जइ(length != srcLength) {
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
  जइ (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  जइ (!isWritable()) {
    प्रत्यागम *एतत्;
  }
  जइ (src.length() > INT32_MAX) {
    setToBogus();
    प्रत्यागम *एतत्;
  }
  प्रत्यागम doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ध्रुव UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  जइ(srcLength == 0) {
    प्रत्यागम *एतत्;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  प्रत्यागम doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(ध्रुव char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  जइ(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    प्रत्यागम *एतत्;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  जइ(srcLength < 0) {
    // get the srcLength if necessary
    जइ((srcLength = u_strlen(srcChars)) == 0) {
      प्रत्यागम *एतत्;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  जइ (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    जइ (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      जइ (srcLength > 1) arr[oldLength+1] = srcChars[1];
      जइ (srcLength > 2) arr[oldLength+2] = srcChars[2];
      जइ (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      प्रत्यागम *एतत्;
    }
  } अण्णहा {
    जइ (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      प्रत्यागम *एतत्;
    }

    // Check for append onto ourself
    ध्रुव char16_t* oldArray = getArrayStart();
    जइ (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      जइ (copy.isBogus()) {
        setToBogus();
        प्रत्यागम *एतत्;
      }
      प्रत्यागम doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    जइ (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      प्रत्यागम *एतत्;
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
  जइ(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  प्रत्यागम *एतत्;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  जइ (!isWritable() || src.empty()) {
    प्रत्यागम *एतत्;
  }
  जइ (src.length() > INT32_MAX) {
    setToBogus();
    प्रत्यागम *एतत्;
  }
  प्रत्यागम doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
शून्य
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    ध्रुव UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
शून्य 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    जइ (limit <= start) {
        प्रत्यागम; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(परिमाण(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    जइ (text != nullptr) {
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
UBool Replaceable::hasMetaData() ध्रुव {
    प्रत्यागम सच्च;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() ध्रुव {
    प्रत्यागम मिच्छ;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  जइ(length <= 1 || !cloneArrayIfNeeded()) {
    प्रत्यागम *एतत्;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  जइ(length <= 1) {  // pinIndices() might have shrunk the length
    प्रत्यागम *एतत्;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *दक्षिणः = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = मिच्छ;

  // Before the loop we know left<right because length>=2.
  करेहि {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *दक्षिणः));
    *दक्षिणः-- = swap;
  } जाव(left < दक्षिणः);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  जइ(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    दक्षिणः = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    जाव(left < दक्षिणः) {
      जइ(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } अण्णहा {
        ++left;
      }
    }
  }

  प्रत्यागम *एतत्;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  जइ(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    प्रत्यागम मिच्छ;
  } अण्णहा {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    जाव(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    प्रत्यागम सच्च;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  जइ(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    प्रत्यागम मिच्छ;
  } अण्णहा {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    जाव(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    प्रत्यागम सच्च;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() ध्रुव
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    जइ (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    प्रत्यागम hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  जइ(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    प्रत्यागम getArrayStart();
  } अण्णहा {
    प्रत्यागम nullptr;
  }
}

शून्य
UnicodeString::releaseBuffer(int32_t newLength) {
  जइ(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    जइ(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      ध्रुव char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      जाव(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } अण्णहा जइ(newLength>capacity) {
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
  जइ(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  जइ(!isWritable()) {
    प्रत्यागम मिच्छ;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  जइ(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_गणनम्() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    जइ(growCapacity < 0) {
      growCapacity = newCapacity;
    } अण्णहा जइ(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } अण्णहा जइ(newCapacity > growCapacity) {
      setToBogus();
      प्रत्यागम मिच्छ;  // bad inputs
    }
    जइ(growCapacity > kMaxCapacity) {
      setToBogus();
      प्रत्यागम मिच्छ;
    }

    // save old values
    char16_t old_राशिः_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    जइ(flags&k_Using_राशिः_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      जइ(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_राशिः_Fields.fBuffer, 0, old_राशिः_Buffer, 0, oldLength);
        oldArray = old_राशिः_Buffer;
      } अण्णहा {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } अण्णहा {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    जइ(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      जइ(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        जइ(newCapacity < minLength) {
          minLength = newCapacity;
        }
        जइ(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } अण्णहा {
        setZeroLength();
      }

      // release the old array
      जइ(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_गणनम् = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        जइ(umtx_atomic_dec(p_Ref_गणनम्) == 0) {
          जइ (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((शून्य *)p_Ref_गणनम्);
          } अण्णहा {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_गणनम्);
          }
        }
      }
    } अण्णहा {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      जइ(!(flags&k_Using_राशिः_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      प्रत्यागम मिच्छ;
    }
  }
  प्रत्यागम सच्च;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  प्रत्यागम str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = मिच्छ;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  प्रत्यागम !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ध्रुव char16_t *s, int32_t length) {
  प्रत्यागम str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  प्रत्यागम str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  जइ(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    प्रत्यागम nullptr;
  }
  int32_t oldLength = str.length();
  जइ(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    प्रत्यागम str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  प्रत्यागम scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ध्रुव UElement कुञ्चिका) {
    ध्रुव UnicodeString *str = (ध्रुव UnicodeString*) कुञ्चिका.pointer;
    प्रत्यागम (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ध्रुव UElement key1, ध्रुव UElement key2) {
    ध्रुव UnicodeString *str1 = (ध्रुव UnicodeString*) key1.pointer;
    ध्रुव UnicodeString *str2 = (ध्रुव UnicodeString*) key2.pointer;
    जइ (str1 == str2) {
        प्रत्यागम सच्च;
    }
    जइ (str1 == nullptr || str2 == nullptr) {
        प्रत्यागम मिच्छ;
    }
    प्रत्यागम *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#जइ defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
स्थिर शून्य uprv_UnicodeStringDummy() {
    delete [] (नूतन UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
