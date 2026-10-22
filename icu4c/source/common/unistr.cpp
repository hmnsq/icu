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

#அந்தர்பாவ <string_view>

#அந்தர்பாவ "unicode/utypes.h"
#அந்தர்பாவ "unicode/appendable.h"
#அந்தர்பாவ "unicode/putil.h"
#அந்தர்பாவ "cstring.h"
#அந்தர்பாவ "cmemory.h"
#அந்தர்பாவ "unicode/ustring.h"
#அந்தர்பாவ "unicode/unistr.h"
#அந்தர்பாவ "unicode/utf.h"
#அந்தர்பாவ "unicode/utf16.h"
#அந்தர்பாவ "uelement.h"
#அந்தர்பாவ "ustr_imp.h"
#அந்தர்பாவ "umutex.h"
#அந்தர்பாவ "uassert.h"

#எனில் 0

#அந்தர்பாவ <iostream>
using நாமாகாஶ std;

//DEBUGGING
வெறுமை
print(மாறிலி UnicodeString& s,
      மாறிலி எழுத்து *name)
{
  char16_t c;
  cout << name << ":|";
  க்கு(முழு i = 0; i < s.length(); ++i) {
    c = s[i];
    எனில்(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    இல்லையெனில்
      cout << (எழுத்து) s[i];
  }
  cout << '|' << endl;
}

வெறுமை
print(மாறிலி char16_t *s,
      int32_t len,
      மாறிலி எழுத்து *name)
{
  char16_t c;
  cout << name << ":|";
  க்கு(முழு i = 0; i < len; ++i) {
    c = s[i];
    எனில்(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    இல்லையெனில்
      cout << (எழுத்து) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
நிலையான
உள்வரி வெறுமை
us_arrayCopy(மாறிலி char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  எனில்(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*அளவு(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
நிலையான char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, வெறுமை *context) {
    திரும்பு ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (மாறிலி UnicodeString &s1, மாறிலி UnicodeString &s2) {
  int32_t sumLengths;
  எனில் (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    திரும்பு bogus;
  }
  எனில் (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  திரும்பு UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(மாறிலி UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  எனில் (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    திரும்பு bogus;
  }
  எனில் (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  திரும்பு UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

வெறுமை
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  திரும்பு umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_மொத்தம்() மாறிலி {
  திரும்பு umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

வெறுமை
UnicodeString::releaseArray() {
  எனில்((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  எனில் (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } இல்லையெனில் எனில்(c <= 0xffff) {
    int32_t length = count;
    எனில்(capacity < length) {
      capacity = length;
    }
    எனில்(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      க்கு(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } இல்லையெனில் {  // supplementary code point, write surrogate pairs
    எனில்(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      திரும்பு;
    }
    int32_t length = count * 2;
    எனில்(capacity < length) {
      capacity = length;
    }
    எனில்(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      க்கு(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_அடுக்கு_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = பொய்;
  U16_APPEND(fUnion.f_அடுக்கு_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  எனில்(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(மாறிலி char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  மாறிலி char16_t *text = textPtr;
  எனில்(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } இல்லையெனில் எனில்(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } இல்லையெனில் {
    எனில்(textLength == -1) {
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
  எனில்(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } இல்லையெனில் எனில்(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } இல்லையெனில் {
    எனில்(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      மாறிலி char16_t *p = buff, *limit = buff + buffCapacity;
      போது(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(மாறிலி எழுத்து *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  எனில்(src==nullptr) {
    // treat as an empty string
  } இல்லையெனில் {
    எனில்(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    எனில்(cloneArrayIfNeeded(length, length, பொய்)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } இல்லையெனில் {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  எனில் (text.length() <= INT32_MAX) {
    result.setTo(பொய், text.data(), static_cast<int32_t>(text.length()));
  } இல்லையெனில் {
    result.setToBogus();
  }
  திரும்பு result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(மாறிலி UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  எனில் (text.isBogus()) {
    result.setToBogus();
  } இல்லையெனில் {
    result.setTo(பொய், text.getBuffer(), text.length());
  }
  திரும்பு result;
}

#எனில் U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  எனில் (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(மாறிலி எழுத்து *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  எனில் (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    திரும்பு;
  }
  எனில்(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(மாறிலி UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, உண்மை);
}

UnicodeString::UnicodeString(மாறிலி UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(மாறிலி UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() மாறிலி {
  திரும்பு nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() மாறிலி {
  LocalPointer<UnicodeString> clonedString(புதிய UnicodeString(*இது));
  திரும்பு clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

நாமாகாஶ {

மாறிலி int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
மாறிலி int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  எனில்(growSize <= (kMaxCapacity - newLength)) {
    திரும்பு newLength + growSize;
  } இல்லையெனில் {
    திரும்பு kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  எனில்(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    திரும்பு உண்மை;
  }
  எனில்(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t இலக்கம்_Bytes = அளவு(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    இலக்கம்_Bytes = (இலக்கம்_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(இலக்கம்_Bytes));
    எனில்(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      இலக்கம்_Bytes -= அளவு(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(இலக்கம்_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      திரும்பு உண்மை;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  திரும்பு பொய்;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_மொத்தம்_FINAL_STRING_LENGTHS
நிலையான u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
நிலையான u_atomic_int32_t beyond_மொத்தம்(0);

U_CAPI வெறுமை unistr_printLengths() {
  int32_t i;
  க்கு(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_மொத்தம்;
  க்கு(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_மொத்தம்_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  எனில்((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    எனில்(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } இல்லையெனில் {
      umtx_atomic_inc(&beyond_மொத்தம்);
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
  திரும்பு result;
}

UnicodeString UnicodeString::fromUTF32(மாறிலி UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  எனில்(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } இல்லையெனில் {
    capacity = length + (length >> 4) + 4;
  }
  செய் {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    எனில்(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      தொடர்;
    } இல்லையெனில் எனில்(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    நிறுத்து;
  } போது(உண்மை);
  திரும்பு result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(மாறிலி UnicodeString& src) U_LIFETIME_BOUND {
  திரும்பு copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(மாறிலி UnicodeString& src) U_LIFETIME_BOUND {
  திரும்பு copyFrom(src, உண்மை);
}

UnicodeString&
UnicodeString::copyFrom(மாறிலி UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  எனில்(இது == &src) {
    திரும்பு *இது;
  }

  // is the right side bogus?
  எனில்(src.isBogus()) {
    setToBogus();
    திரும்பு *இது;
  }

  // delete the current contents
  releaseArray();

  எனில்(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    திரும்பு *இது;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  தேர்வு(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  நிலை kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_அடுக்கு_Fields.fBuffer, src.fUnion.f_அடுக்கு_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    நிறுத்து;
  நிலை kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    எனில்(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    நிறுத்து;
  நிலை kReadonlyAlias:
    எனில்(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      எனில்(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      நிறுத்து;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  நிலை kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    எனில்(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      நிறுத்து;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  இயல்பு:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    நிறுத்து;
  }

  திரும்பு *இது;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, உண்மை);
  திரும்பு *இது;
}

// Same as move assignment except without memory management.
வெறுமை UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  எனில்(lengthAndFlags & k_Using_அடுக்கு_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    எனில்(இது != &src) {
      uprv_memcpy(fUnion.f_அடுக்கு_Fields.fBuffer, src.fUnion.f_அடுக்கு_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } இல்லையெனில் {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    எனில்(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    எனில்(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

வெறுமை UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*இது, பொய்);
  இது->copyFieldsFrom(other, பொய்);
  other.copyFieldsFrom(temp, பொய்);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() மாறிலி {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    எனில் (result.isBogus()) {
        திரும்பு result;
    }
    மாறிலி char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    க்கு (int32_t i=0;;) {
        எனில் (i == len) {
            result.append(array, prev, len - prev);
            நிறுத்து;
        }
        எனில் (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            எனில் (c < 0) {
                result.remove(); // return empty string
                நிறுத்து; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    திரும்பு result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) மாறிலி {
    திரும்பு u_unescapeAt(UnicodeString_charAt, &offset, length(), (வெறுமை*)இது);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(மாறிலி char16_t *text, int32_t len) மாறிலி {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  திரும்பு uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              மாறிலி char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) மாறிலி
{
  // compare illegal string values
  எனில்(isBogus()) {
    திரும்பு பொய்;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  எனில்(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    திரும்பு length == 0 ? உண்மை : பொய்;
  }

  // get the correct pointer
  மாறிலி char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  எனில்(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  எனில் (length != srcLength) {
    திரும்பு பொய்;
  }

  எனில்(length == 0 || chars == srcChars) {
    திரும்பு உண்மை;
  }

  திரும்பு u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              மாறிலி char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) மாறிலி
{
  // compare illegal string values
  எனில்(isBogus()) {
    திரும்பு -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  எனில்(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    திரும்பு length == 0 ? 0 : 1;
  }

  // get the correct pointer
  மாறிலி char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  எனில்(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  எனில்(length != srcLength) {
    எனில்(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } இல்லையெனில் {
      minLength = srcLength;
      lengthResult = 1;
    }
  } இல்லையெனில் {
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

  எனில்(minLength > 0 && chars != srcChars) {
    int32_t result;

#   எனில் U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * அளவு(char16_t));
      எனில்(result != 0) {
        திரும்பு (int8_t)(result >> 15 | 1);
      }
#   இல்லையெனில்
      // little-endian: compare char16_t units
      செய் {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        எனில்(result != 0) {
          திரும்பு static_cast<int8_t>(result >> 15 | 1);
        }
      } போது(--minLength > 0);
#   endif
  }
  திரும்பு lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       மாறிலி char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) மாறிலி
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  எனில்(isBogus()) {
    திரும்பு -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  எனில்(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, பொய், உண்மை);
  /* translate the 32-bit result into an 8-bit one */
  எனில்(diff!=0) {
    திரும்பு static_cast<int8_t>(diff >> 15 | 1);
  } இல்லையெனில் {
    திரும்பு 0;
  }
}

int32_t
UnicodeString::getLength() மாறிலி {
    திரும்பு length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) மாறிலி {
  திரும்பு charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) மாறிலி {
  திரும்பு char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) மாறிலி
{
  int32_t len = length();
  எனில் (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    மாறிலி char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    திரும்பு c;
  } இல்லையெனில் {
    திரும்பு kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) மாறிலி {
  எனில் (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    மாறிலி char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    திரும்பு offset;
  } இல்லையெனில் {
    திரும்பு 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) மாறிலி {
  int32_t len = length();
  எனில் (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    மாறிலி char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    திரும்பு offset;
  } இல்லையெனில் {
    திரும்பு len;
  }
}

int32_t
UnicodeString::மொத்தம்_Char32(int32_t start, int32_t length) மாறிலி {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  திரும்பு u_மொத்தம்_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) மாறிலி {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  திரும்பு u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) மாறிலி {
  // pin index
  int32_t len = length();
  எனில்(index<0) {
    index=0;
  } இல்லையெனில் எனில்(index>len) {
    index=len;
  }

  மாறிலி char16_t *array = getArrayStart();
  எனில்(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } இல்லையெனில் {
    U16_BACK_N(array, 0, index, -delta);
  }

  திரும்பு index;
}

வெறுமை
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) மாறிலி
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  மாறிலி char16_t *array = getArrayStart();
  எனில்(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) மாறிலி {
  int32_t len = length();
  எனில்(U_SUCCESS(errorCode)) {
    எனில் (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } இல்லையெனில் {
      மாறிலி char16_t *array = getArrayStart();
      எனில்(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      திரும்பு u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  திரும்பு len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       எழுத்து *target,
                       int32_t targetCapacity,
                       எண்ணிக்கை EInvariant) மாறிலி
{
  // if the arguments are illegal, then do nothing
  எனில்(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    திரும்பு 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  எனில்(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  திரும்பு u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) மாறிலி U_LIFETIME_BOUND {
  pinIndices(start, len);
  மாறிலி char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  எனில்(array==nullptr) {
    array=fUnion.f_அடுக்கு_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  திரும்பு UnicodeString(பொய், array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      எழுத்து *target, int32_t capacity) மாறிலி {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  திரும்பு length8;
}

#எனில் U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       எழுத்து *target, uint32_t dstSize) மாறிலி {
  // if the arguments are illegal, then do nothing
  எனில் (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    திரும்பு 0;
  }
  திரும்பு toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

வெறுமை 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) மாறிலி {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
வெறுமை
UnicodeString::toUTF8(ByteSink &sink) மாறிலி {
  int32_t length16 = length();
  எனில்(length16 != 0) {
    எழுத்து அடுக்கு_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(அளவு(அடுக்கு_Buffer));
    UBool utf8IsOwned = பொய்;
    எழுத்து *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      அடுக்கு_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    எனில்(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<எழுத்து*>(uprv_malloc(length8));
      எனில்(utf8 != nullptr) {
        utf8IsOwned = உண்மை;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } இல்லையெனில் {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    எனில்(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    எனில்(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) மாறிலி {
  int32_t length32=0;
  எனில்(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  திரும்பு length32;
}

int32_t 
UnicodeString::indexOf(மாறிலி char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) மாறிலி
{
  எனில் (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    திரும்பு -1;
  }

  // UnicodeString does not find empty substrings
  எனில்(srcLength < 0 && srcChars[srcStart] == 0) {
    திரும்பு -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) மாறிலி
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_memchr(array + start, c, length);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) மாறிலி {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_memchr32(array + start, c, length);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(மாறிலி char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) மாறிலி
{
  எனில் (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    திரும்பு -1;
  }

  // UnicodeString does not find empty substrings
  எனில்(srcLength < 0 && srcChars[srcStart] == 0) {
    திரும்பு -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) மாறிலி
{
  எனில்(isBogus()) {
    திரும்பு -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_memrchr(array + start, c, length);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) மாறிலி {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  மாறிலி char16_t *array = getArrayStart();
  மாறிலி char16_t *match = u_memrchr32(array + start, c, length);
  எனில்(match == nullptr) {
    திரும்பு -1;
  } இல்லையெனில் {
    திரும்பு static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              மாறிலி UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              மாறிலி UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  எனில்(isBogus() || oldText.isBogus() || newText.isBogus()) {
    திரும்பு *இது;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  எனில்(oldLength == 0) {
    திரும்பு *இது;
  }

  போது(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    எனில்(pos < 0) {
      // no more oldText's here: done
      நிறுத்து;
    } இல்லையெனில் {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  திரும்பு *இது;
}


வெறுமை
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
வெறுமை
UnicodeString::unBogus() {
  எனில்(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

மாறிலி char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  எனில்(!isWritable()) {
    திரும்பு nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  எனில்(len < getCapacity()) {
    எனில்(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      எனில்(array[len] == 0) {
        திரும்பு array;
      }
    } இல்லையெனில் எனில்(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_மொத்தம்() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      திரும்பு array;
    }
  }
  எனில்(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    திரும்பு array;
  } இல்லையெனில் {
    திரும்பு nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  எனில்(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    திரும்பு *இது;
  }

  மாறிலி char16_t *text = textPtr;
  எனில்(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    திரும்பு *இது;
  }

  எனில்( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    திரும்பு *இது;
  }

  releaseArray();

  எனில்(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  திரும்பு *இது;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  எனில்(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    திரும்பு *இது;
  }

  எனில்(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    திரும்பு *இது;
  }

  எனில்(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    திரும்பு *இது;
  } இல்லையெனில் எனில்(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    மாறிலி char16_t *p = buffer, *limit = buffer + buffCapacity;
    போது(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  திரும்பு *இது;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  எனில்(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } இல்லையெனில் {
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
  எனில்(U_FAILURE(errorCode)) {
    setToBogus();
  }
  திரும்பு *இது;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  எனில்(cloneArrayIfNeeded() && len > 0) {
    எனில்(offset < 0) {
      offset = 0;
    } இல்லையெனில் எனில்(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  திரும்பு *இது;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = பொய்;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  திரும்பு doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = பொய்;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  திரும்பு isError ? *இது : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         மாறிலி UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  திரும்பு doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         மாறிலி char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  எனில்(!isWritable()) {
    திரும்பு *இது;
  }

  int32_t oldLength = இது->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  எனில்((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    எனில்(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      திரும்பு *இது;
    } இல்லையெனில் {
      pinIndex(start);
      எனில்(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        திரும்பு *இது;
      }
    }
  }

  எனில்(start == oldLength) {
    திரும்பு doAppend(srcChars, srcStart, srcLength);
  }

  எனில் (srcChars == nullptr) {
    srcLength = 0;
  } இல்லையெனில் {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    எனில் (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  எனில்(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    திரும்பு *இது;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  மாறிலி char16_t *oldArray = getArrayStart();
  எனில் (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    எனில் (copy.isBogus()) {
      setToBogus();
      திரும்பு *இது;
    }
    திரும்பு doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_அடுக்கு_Buffer[US_STACKBUF_SIZE];
  எனில்((fUnion.fFields.fLengthAndFlags&k_Using_அடுக்கு_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_அடுக்கு_Buffer, oldArray, oldLength);
    oldArray = old_அடுக்கு_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  எனில்(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         பொய், &bufferToDelete)
  ) {
    திரும்பு *இது;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  எனில்(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } இல்லையெனில் எனில்(length != srcLength) {
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
  எனில் (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  திரும்பு *இது;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  எனில் (!isWritable()) {
    திரும்பு *இது;
  }
  எனில் (src.length() > INT32_MAX) {
    setToBogus();
    திரும்பு *இது;
  }
  திரும்பு doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(மாறிலி UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  எனில்(srcLength == 0) {
    திரும்பு *இது;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  திரும்பு doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(மாறிலி char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  எனில்(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    திரும்பு *இது;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  எனில்(srcLength < 0) {
    // get the srcLength if necessary
    எனில்((srcLength = u_strlen(srcChars)) == 0) {
      திரும்பு *இது;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  எனில் (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    எனில் (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      எனில் (srcLength > 1) arr[oldLength+1] = srcChars[1];
      எனில் (srcLength > 2) arr[oldLength+2] = srcChars[2];
      எனில் (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      திரும்பு *இது;
    }
  } இல்லையெனில் {
    எனில் (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      திரும்பு *இது;
    }

    // Check for append onto ourself
    மாறிலி char16_t* oldArray = getArrayStart();
    எனில் (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      எனில் (copy.isBogus()) {
        setToBogus();
        திரும்பு *இது;
      }
      திரும்பு doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    எனில் (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      திரும்பு *இது;
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
  எனில்(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  திரும்பு *இது;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  எனில் (!isWritable() || src.empty()) {
    திரும்பு *இது;
  }
  எனில் (src.length() > INT32_MAX) {
    setToBogus();
    திரும்பு *இது;
  }
  திரும்பு doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
வெறுமை
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    மாறிலி UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
வெறுமை 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    எனில் (limit <= start) {
        திரும்பு; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(அளவு(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    எனில் (text != nullptr) {
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
UBool Replaceable::hasMetaData() மாறிலி {
    திரும்பு உண்மை;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() மாறிலி {
    திரும்பு பொய்;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  எனில்(length <= 1 || !cloneArrayIfNeeded()) {
    திரும்பு *இது;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  எனில்(length <= 1) {  // pinIndices() might have shrunk the length
    திரும்பு *இது;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *வலது = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = பொய்;

  // Before the loop we know left<right because length>=2.
  செய் {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *வலது));
    *வலது-- = swap;
  } போது(left < வலது);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  எனில்(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    வலது = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    போது(left < வலது) {
      எனில்(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } இல்லையெனில் {
        ++left;
      }
    }
  }

  திரும்பு *இது;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  எனில்(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    திரும்பு பொய்;
  } இல்லையெனில் {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    போது(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    திரும்பு உண்மை;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  எனில்(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    திரும்பு பொய்;
  } இல்லையெனில் {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    போது(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    திரும்பு உண்மை;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() மாறிலி
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    எனில் (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    திரும்பு hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  எனில்(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    திரும்பு getArrayStart();
  } இல்லையெனில் {
    திரும்பு nullptr;
  }
}

வெறுமை
UnicodeString::releaseBuffer(int32_t newLength) {
  எனில்(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    எனில்(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      மாறிலி char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      போது(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } இல்லையெனில் எனில்(newLength>capacity) {
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
  எனில்(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  எனில்(!isWritable()) {
    திரும்பு பொய்;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  எனில்(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_மொத்தம்() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    எனில்(growCapacity < 0) {
      growCapacity = newCapacity;
    } இல்லையெனில் எனில்(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } இல்லையெனில் எனில்(newCapacity > growCapacity) {
      setToBogus();
      திரும்பு பொய்;  // bad inputs
    }
    எனில்(growCapacity > kMaxCapacity) {
      setToBogus();
      திரும்பு பொய்;
    }

    // save old values
    char16_t old_அடுக்கு_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    எனில்(flags&k_Using_அடுக்கு_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      எனில்(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_அடுக்கு_Fields.fBuffer, 0, old_அடுக்கு_Buffer, 0, oldLength);
        oldArray = old_அடுக்கு_Buffer;
      } இல்லையெனில் {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } இல்லையெனில் {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    எனில்(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      எனில்(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        எனில்(newCapacity < minLength) {
          minLength = newCapacity;
        }
        எனில்(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } இல்லையெனில் {
        setZeroLength();
      }

      // release the old array
      எனில்(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_மொத்தம் = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        எனில்(umtx_atomic_dec(p_Ref_மொத்தம்) == 0) {
          எனில் (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((வெறுமை *)p_Ref_மொத்தம்);
          } இல்லையெனில் {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_மொத்தம்);
          }
        }
      }
    } இல்லையெனில் {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      எனில்(!(flags&k_Using_அடுக்கு_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      திரும்பு பொய்;
    }
  }
  திரும்பு உண்மை;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  திரும்பு str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = பொய்;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  திரும்பு !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(மாறிலி char16_t *s, int32_t length) {
  திரும்பு str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  திரும்பு str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  எனில்(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    திரும்பு nullptr;
  }
  int32_t oldLength = str.length();
  எனில்(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    திரும்பு str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  திரும்பு scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(மாறிலி UElement சாவி) {
    மாறிலி UnicodeString *str = (மாறிலி UnicodeString*) சாவி.pointer;
    திரும்பு (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(மாறிலி UElement key1, மாறிலி UElement key2) {
    மாறிலி UnicodeString *str1 = (மாறிலி UnicodeString*) key1.pointer;
    மாறிலி UnicodeString *str2 = (மாறிலி UnicodeString*) key2.pointer;
    எனில் (str1 == str2) {
        திரும்பு உண்மை;
    }
    எனில் (str1 == nullptr || str2 == nullptr) {
        திரும்பு பொய்;
    }
    திரும்பு *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#எனில் defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
நிலையான வெறுமை uprv_UnicodeStringDummy() {
    delete [] (புதிய UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
