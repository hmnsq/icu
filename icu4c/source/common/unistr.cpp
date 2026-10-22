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

#ଅନ୍ତର୍ଭାଵ <string_view>

#ଅନ୍ତର୍ଭାଵ "unicode/utypes.h"
#ଅନ୍ତର୍ଭାଵ "unicode/appendable.h"
#ଅନ୍ତର୍ଭାଵ "unicode/putil.h"
#ଅନ୍ତର୍ଭାଵ "cstring.h"
#ଅନ୍ତର୍ଭାଵ "cmemory.h"
#ଅନ୍ତର୍ଭାଵ "unicode/ustring.h"
#ଅନ୍ତର୍ଭାଵ "unicode/unistr.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf.h"
#ଅନ୍ତର୍ଭାଵ "unicode/utf16.h"
#ଅନ୍ତର୍ଭାଵ "uelement.h"
#ଅନ୍ତର୍ଭାଵ "ustr_imp.h"
#ଅନ୍ତର୍ଭାଵ "umutex.h"
#ଅନ୍ତର୍ଭାଵ "uassert.h"

#ଯଦି 0

#ଅନ୍ତର୍ଭାଵ <iostream>
using ନାମାକାଶ std;

//DEBUGGING
ଶୂନ୍ୟ
print(ଅଚଳ UnicodeString& s,
      ଅଚଳ ଅକ୍ଷର *name)
{
  char16_t c;
  cout << name << ":|";
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < s.length(); ++i) {
    c = s[i];
    ଯଦି(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    ଅନ୍ୟଥା
      cout << (ଅକ୍ଷର) s[i];
  }
  cout << '|' << endl;
}

ଶୂନ୍ୟ
print(ଅଚଳ char16_t *s,
      int32_t len,
      ଅଚଳ ଅକ୍ଷର *name)
{
  char16_t c;
  cout << name << ":|";
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; ++i) {
    c = s[i];
    ଯଦି(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    ଅନ୍ୟଥା
      cout << (ଅକ୍ଷର) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
ସ୍ଥିର
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ
us_arrayCopy(ଅଚଳ char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  ଯଦି(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*ଆକାର(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
ସ୍ଥିର char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, ଶୂନ୍ୟ *context) {
    ଫେରନ୍ତୁ ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (ଅଚଳ UnicodeString &s1, ଅଚଳ UnicodeString &s2) {
  int32_t sumLengths;
  ଯଦି (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ଫେରନ୍ତୁ bogus;
  }
  ଯଦି (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ଫେରନ୍ତୁ UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(ଅଚଳ UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  ଯଦି (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    ଫେରନ୍ତୁ bogus;
  }
  ଯଦି (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  ଫେରନ୍ତୁ UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

ଶୂନ୍ୟ
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  ଫେରନ୍ତୁ umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_ଗଣନା() ଅଚଳ {
  ଫେରନ୍ତୁ umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

ଶୂନ୍ୟ
UnicodeString::releaseArray() {
  ଯଦି((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  ଯଦି (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } ଅନ୍ୟଥା ଯଦି(c <= 0xffff) {
    int32_t length = count;
    ଯଦି(capacity < length) {
      capacity = length;
    }
    ଯଦି(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      ପାଇଁ(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } ଅନ୍ୟଥା {  // supplementary code point, write surrogate pairs
    ଯଦି(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      ଫେରନ୍ତୁ;
    }
    int32_t length = count * 2;
    ଯଦି(capacity < length) {
      capacity = length;
    }
    ଯଦି(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      ପାଇଁ(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_ଗଦା_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = ମିଥ୍ୟା;
  U16_APPEND(fUnion.f_ଗଦା_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  ଯଦି(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(ଅଚଳ char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  ଅଚଳ char16_t *text = textPtr;
  ଯଦି(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } ଅନ୍ୟଥା ଯଦି(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } ଅନ୍ୟଥା {
    ଯଦି(textLength == -1) {
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
  ଯଦି(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } ଅନ୍ୟଥା ଯଦି(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } ଅନ୍ୟଥା {
    ଯଦି(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      ଅଚଳ char16_t *p = buff, *limit = buff + buffCapacity;
      ଯେତେବେଳେ(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  ଯଦି(src==nullptr) {
    // treat as an empty string
  } ଅନ୍ୟଥା {
    ଯଦି(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    ଯଦି(cloneArrayIfNeeded(length, length, ମିଥ୍ୟା)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } ଅନ୍ୟଥା {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  ଯଦି (text.length() <= INT32_MAX) {
    result.setTo(ମିଥ୍ୟା, text.data(), static_cast<int32_t>(text.length()));
  } ଅନ୍ୟଥା {
    result.setToBogus();
  }
  ଫେରନ୍ତୁ result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(ଅଚଳ UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  ଯଦି (text.isBogus()) {
    result.setToBogus();
  } ଅନ୍ୟଥା {
    result.setTo(ମିଥ୍ୟା, text.getBuffer(), text.length());
  }
  ଫେରନ୍ତୁ result;
}

#ଯଦି U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  ଯଦି (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(ଅଚଳ ଅକ୍ଷର *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  ଯଦି (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    ଫେରନ୍ତୁ;
  }
  ଯଦି(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(ଅଚଳ UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, ସତ୍ୟ);
}

UnicodeString::UnicodeString(ଅଚଳ UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(ଅଚଳ UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() ଅଚଳ {
  ଫେରନ୍ତୁ nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() ଅଚଳ {
  LocalPointer<UnicodeString> clonedString(ନୂତନ UnicodeString(*ଏହି));
  ଫେରନ୍ତୁ clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

ନାମାକାଶ {

ଅଚଳ int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
ଅଚଳ int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  ଯଦି(growSize <= (kMaxCapacity - newLength)) {
    ଫେରନ୍ତୁ newLength + growSize;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  ଯଦି(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }
  ଯଦି(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t ଅଙ୍କ_Bytes = ଆକାର(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    ଅଙ୍କ_Bytes = (ଅଙ୍କ_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(ଅଙ୍କ_Bytes));
    ଯଦି(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      ଅଙ୍କ_Bytes -= ଆକାର(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(ଅଙ୍କ_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      ଫେରନ୍ତୁ ସତ୍ୟ;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_ଗଣନା_FINAL_STRING_LENGTHS
ସ୍ଥିର u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
ସ୍ଥିର u_atomic_int32_t beyond_ଗଣନା(0);

U_CAPI ଶୂନ୍ୟ unistr_printLengths() {
  int32_t i;
  ପାଇଁ(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_ଗଣନା;
  ପାଇଁ(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_ଗଣନା_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  ଯଦି((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    ଯଦି(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } ଅନ୍ୟଥା {
      umtx_atomic_inc(&beyond_ଗଣନା);
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
  ଫେରନ୍ତୁ result;
}

UnicodeString UnicodeString::fromUTF32(ଅଚଳ UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  ଯଦି(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } ଅନ୍ୟଥା {
    capacity = length + (length >> 4) + 4;
  }
  କର {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    ଯଦି(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      ଜାରି;
    } ଅନ୍ୟଥା ଯଦି(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    ଭାଙ୍ଗ;
  } ଯେତେବେଳେ(ସତ୍ୟ);
  ଫେରନ୍ତୁ result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(ଅଚଳ UnicodeString& src) U_LIFETIME_BOUND {
  ଫେରନ୍ତୁ copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(ଅଚଳ UnicodeString& src) U_LIFETIME_BOUND {
  ଫେରନ୍ତୁ copyFrom(src, ସତ୍ୟ);
}

UnicodeString&
UnicodeString::copyFrom(ଅଚଳ UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  ଯଦି(ଏହି == &src) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  // is the right side bogus?
  ଯଦି(src.isBogus()) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  }

  // delete the current contents
  releaseArray();

  ଯଦି(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    ଫେରନ୍ତୁ *ଏହି;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  ବାଛ(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  ସ୍ଥିତି kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_ଗଦା_Fields.fBuffer, src.fUnion.f_ଗଦା_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    ଯଦି(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି kReadonlyAlias:
    ଯଦି(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      ଯଦି(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      ଭାଙ୍ଗ;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  ସ୍ଥିତି kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    ଯଦି(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      ଭାଙ୍ଗ;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, ସତ୍ୟ);
  ଫେରନ୍ତୁ *ଏହି;
}

// Same as move assignment except without memory management.
ଶୂନ୍ୟ UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  ଯଦି(lengthAndFlags & k_Using_ଗଦା_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    ଯଦି(ଏହି != &src) {
      uprv_memcpy(fUnion.f_ଗଦା_Fields.fBuffer, src.fUnion.f_ଗଦା_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } ଅନ୍ୟଥା {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    ଯଦି(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    ଯଦି(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

ଶୂନ୍ୟ UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*ଏହି, ମିଥ୍ୟା);
  ଏହି->copyFieldsFrom(other, ମିଥ୍ୟା);
  other.copyFieldsFrom(temp, ମିଥ୍ୟା);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() ଅଚଳ {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    ଯଦି (result.isBogus()) {
        ଫେରନ୍ତୁ result;
    }
    ଅଚଳ char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    ପାଇଁ (int32_t i=0;;) {
        ଯଦି (i == len) {
            result.append(array, prev, len - prev);
            ଭାଙ୍ଗ;
        }
        ଯଦି (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            ଯଦି (c < 0) {
                result.remove(); // return empty string
                ଭାଙ୍ଗ; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    ଫେରନ୍ତୁ result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) ଅଚଳ {
    ଫେରନ୍ତୁ u_unescapeAt(UnicodeString_charAt, &offset, length(), (ଶୂନ୍ୟ*)ଏହି);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(ଅଚଳ char16_t *text, int32_t len) ଅଚଳ {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  ଫେରନ୍ତୁ uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              ଅଚଳ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ଅଚଳ
{
  // compare illegal string values
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  ଯଦି(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ଫେରନ୍ତୁ length == 0 ? ସତ୍ୟ : ମିଥ୍ୟା;
  }

  // get the correct pointer
  ଅଚଳ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  ଯଦି(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  ଯଦି (length != srcLength) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  }

  ଯଦି(length == 0 || chars == srcChars) {
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }

  ଫେରନ୍ତୁ u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              ଅଚଳ char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) ଅଚଳ
{
  // compare illegal string values
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  ଯଦି(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    ଫେରନ୍ତୁ length == 0 ? 0 : 1;
  }

  // get the correct pointer
  ଅଚଳ char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  ଯଦି(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  ଯଦି(length != srcLength) {
    ଯଦି(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } ଅନ୍ୟଥା {
      minLength = srcLength;
      lengthResult = 1;
    }
  } ଅନ୍ୟଥା {
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

  ଯଦି(minLength > 0 && chars != srcChars) {
    int32_t result;

#   ଯଦି U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * ଆକାର(char16_t));
      ଯଦି(result != 0) {
        ଫେରନ୍ତୁ (int8_t)(result >> 15 | 1);
      }
#   ଅନ୍ୟଥା
      // little-endian: compare char16_t units
      କର {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        ଯଦି(result != 0) {
          ଫେରନ୍ତୁ static_cast<int8_t>(result >> 15 | 1);
        }
      } ଯେତେବେଳେ(--minLength > 0);
#   endif
  }
  ଫେରନ୍ତୁ lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       ଅଚଳ char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) ଅଚଳ
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  ଯଦି(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, ମିଥ୍ୟା, ସତ୍ୟ);
  /* translate the 32-bit result into an 8-bit one */
  ଯଦି(diff!=0) {
    ଫେରନ୍ତୁ static_cast<int8_t>(diff >> 15 | 1);
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ 0;
  }
}

int32_t
UnicodeString::getLength() ଅଚଳ {
    ଫେରନ୍ତୁ length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) ଅଚଳ {
  ଫେରନ୍ତୁ charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) ଅଚଳ {
  ଫେରନ୍ତୁ char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) ଅଚଳ
{
  int32_t len = length();
  ଯଦି (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ଅଚଳ char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    ଫେରନ୍ତୁ c;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) ଅଚଳ {
  ଯଦି (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    ଅଚଳ char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    ଫେରନ୍ତୁ offset;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) ଅଚଳ {
  int32_t len = length();
  ଯଦି (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    ଅଚଳ char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    ଫେରନ୍ତୁ offset;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ len;
  }
}

int32_t
UnicodeString::ଗଣନା_Char32(int32_t start, int32_t length) ଅଚଳ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  ଫେରନ୍ତୁ u_ଗଣନା_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) ଅଚଳ {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  ଫେରନ୍ତୁ u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) ଅଚଳ {
  // pin index
  int32_t len = length();
  ଯଦି(index<0) {
    index=0;
  } ଅନ୍ୟଥା ଯଦି(index>len) {
    index=len;
  }

  ଅଚଳ char16_t *array = getArrayStart();
  ଯଦି(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } ଅନ୍ୟଥା {
    U16_BACK_N(array, 0, index, -delta);
  }

  ଫେରନ୍ତୁ index;
}

ଶୂନ୍ୟ
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) ଅଚଳ
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  ଅଚଳ char16_t *array = getArrayStart();
  ଯଦି(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) ଅଚଳ {
  int32_t len = length();
  ଯଦି(U_SUCCESS(errorCode)) {
    ଯଦି (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } ଅନ୍ୟଥା {
      ଅଚଳ char16_t *array = getArrayStart();
      ଯଦି(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      ଫେରନ୍ତୁ u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  ଫେରନ୍ତୁ len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       ଅକ୍ଷର *target,
                       int32_t targetCapacity,
                       ଗଣନା EInvariant) ଅଚଳ
{
  // if the arguments are illegal, then do nothing
  ଯଦି(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    ଫେରନ୍ତୁ 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  ଯଦି(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  ଫେରନ୍ତୁ u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) ଅଚଳ U_LIFETIME_BOUND {
  pinIndices(start, len);
  ଅଚଳ char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  ଯଦି(array==nullptr) {
    array=fUnion.f_ଗଦା_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  ଫେରନ୍ତୁ UnicodeString(ମିଥ୍ୟା, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      ଅକ୍ଷର *target, int32_t capacity) ଅଚଳ {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  ଫେରନ୍ତୁ length8;
}

#ଯଦି U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       ଅକ୍ଷର *target, uint32_t dstSize) ଅଚଳ {
  // if the arguments are illegal, then do nothing
  ଯଦି (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

ଶୂନ୍ୟ 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) ଅଚଳ {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
ଶୂନ୍ୟ
UnicodeString::toUTF8(ByteSink &sink) ଅଚଳ {
  int32_t length16 = length();
  ଯଦି(length16 != 0) {
    ଅକ୍ଷର ଗଦା_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(ଆକାର(ଗଦା_Buffer));
    UBool utf8IsOwned = ମିଥ୍ୟା;
    ଅକ୍ଷର *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      ଗଦା_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    ଯଦି(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<ଅକ୍ଷର*>(uprv_malloc(length8));
      ଯଦି(utf8 != nullptr) {
        utf8IsOwned = ସତ୍ୟ;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } ଅନ୍ୟଥା {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    ଯଦି(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    ଯଦି(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) ଅଚଳ {
  int32_t length32=0;
  ଯଦି(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  ଫେରନ୍ତୁ length32;
}

int32_t 
UnicodeString::indexOf(ଅଚଳ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ଅଚଳ
{
  ଯଦି (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ଫେରନ୍ତୁ -1;
  }

  // UnicodeString does not find empty substrings
  ଯଦି(srcLength < 0 && srcChars[srcStart] == 0) {
    ଫେରନ୍ତୁ -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) ଅଚଳ
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_memchr(array + start, c, length);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) ଅଚଳ {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_memchr32(array + start, c, length);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(ଅଚଳ char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) ଅଚଳ
{
  ଯଦି (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    ଫେରନ୍ତୁ -1;
  }

  // UnicodeString does not find empty substrings
  ଯଦି(srcLength < 0 && srcChars[srcStart] == 0) {
    ଫେରନ୍ତୁ -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) ଅଚଳ
{
  ଯଦି(isBogus()) {
    ଫେରନ୍ତୁ -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_memrchr(array + start, c, length);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) ଅଚଳ {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  ଅଚଳ char16_t *array = getArrayStart();
  ଅଚଳ char16_t *match = u_memrchr32(array + start, c, length);
  ଯଦି(match == nullptr) {
    ଫେରନ୍ତୁ -1;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              ଅଚଳ UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              ଅଚଳ UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  ଯଦି(isBogus() || oldText.isBogus() || newText.isBogus()) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  ଯଦି(oldLength == 0) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  ଯେତେବେଳେ(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    ଯଦି(pos < 0) {
      // no more oldText's here: done
      ଭାଙ୍ଗ;
    } ଅନ୍ୟଥା {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  ଫେରନ୍ତୁ *ଏହି;
}


ଶୂନ୍ୟ
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
ଶୂନ୍ୟ
UnicodeString::unBogus() {
  ଯଦି(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

ଅଚଳ char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  ଯଦି(!isWritable()) {
    ଫେରନ୍ତୁ nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  ଯଦି(len < getCapacity()) {
    ଯଦି(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      ଯଦି(array[len] == 0) {
        ଫେରନ୍ତୁ array;
      }
    } ଅନ୍ୟଥା ଯଦି(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_ଗଣନା() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      ଫେରନ୍ତୁ array;
    }
  }
  ଯଦି(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    ଫେରନ୍ତୁ array;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  ଯଦି(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ଫେରନ୍ତୁ *ଏହି;
  }

  ଅଚଳ char16_t *text = textPtr;
  ଯଦି(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ଫେରନ୍ତୁ *ଏହି;
  }

  ଯଦି( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  }

  releaseArray();

  ଯଦି(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  ଫେରନ୍ତୁ *ଏହି;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  ଯଦି(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    ଫେରନ୍ତୁ *ଏହି;
  }

  ଯଦି(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    ଫେରନ୍ତୁ *ଏହି;
  }

  ଯଦି(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  } ଅନ୍ୟଥା ଯଦି(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    ଅଚଳ char16_t *p = buffer, *limit = buffer + buffCapacity;
    ଯେତେବେଳେ(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  ଯଦି(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } ଅନ୍ୟଥା {
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
  ଯଦି(U_FAILURE(errorCode)) {
    setToBogus();
  }
  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  ଯଦି(cloneArrayIfNeeded() && len > 0) {
    ଯଦି(offset < 0) {
      offset = 0;
    } ଅନ୍ୟଥା ଯଦି(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = ମିଥ୍ୟା;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  ଫେରନ୍ତୁ doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = ମିଥ୍ୟା;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  ଫେରନ୍ତୁ isError ? *ଏହି : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ଅଚଳ UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  ଫେରନ୍ତୁ doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         ଅଚଳ char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  ଯଦି(!isWritable()) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  int32_t oldLength = ଏହି->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  ଯଦି((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    ଯଦି(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      ଫେରନ୍ତୁ *ଏହି;
    } ଅନ୍ୟଥା {
      pinIndex(start);
      ଯଦି(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        ଫେରନ୍ତୁ *ଏହି;
      }
    }
  }

  ଯଦି(start == oldLength) {
    ଫେରନ୍ତୁ doAppend(srcChars, srcStart, srcLength);
  }

  ଯଦି (srcChars == nullptr) {
    srcLength = 0;
  } ଅନ୍ୟଥା {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    ଯଦି (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  ଯଦି(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  ଅଚଳ char16_t *oldArray = getArrayStart();
  ଯଦି (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    ଯଦି (copy.isBogus()) {
      setToBogus();
      ଫେରନ୍ତୁ *ଏହି;
    }
    ଫେରନ୍ତୁ doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_ଗଦା_Buffer[US_STACKBUF_SIZE];
  ଯଦି((fUnion.fFields.fLengthAndFlags&k_Using_ଗଦା_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_ଗଦା_Buffer, oldArray, oldLength);
    oldArray = old_ଗଦା_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  ଯଦି(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         ମିଥ୍ୟା, &bufferToDelete)
  ) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  ଯଦି(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } ଅନ୍ୟଥା ଯଦି(length != srcLength) {
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
  ଯଦି (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  ଯଦି (!isWritable()) {
    ଫେରନ୍ତୁ *ଏହି;
  }
  ଯଦି (src.length() > INT32_MAX) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  }
  ଫେରନ୍ତୁ doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(ଅଚଳ UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  ଯଦି(srcLength == 0) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  ଫେରନ୍ତୁ doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(ଅଚଳ char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  ଯଦି(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  ଯଦି(srcLength < 0) {
    // get the srcLength if necessary
    ଯଦି((srcLength = u_strlen(srcChars)) == 0) {
      ଫେରନ୍ତୁ *ଏହି;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  ଯଦି (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    ଯଦି (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      ଯଦି (srcLength > 1) arr[oldLength+1] = srcChars[1];
      ଯଦି (srcLength > 2) arr[oldLength+2] = srcChars[2];
      ଯଦି (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      ଫେରନ୍ତୁ *ଏହି;
    }
  } ଅନ୍ୟଥା {
    ଯଦି (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      ଫେରନ୍ତୁ *ଏହି;
    }

    // Check for append onto ourself
    ଅଚଳ char16_t* oldArray = getArrayStart();
    ଯଦି (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      ଯଦି (copy.isBogus()) {
        setToBogus();
        ଫେରନ୍ତୁ *ଏହି;
      }
      ଫେରନ୍ତୁ doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    ଯଦି (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      ଫେରନ୍ତୁ *ଏହି;
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
  ଯଦି(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  ଫେରନ୍ତୁ *ଏହି;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  ଯଦି (!isWritable() || src.empty()) {
    ଫେରନ୍ତୁ *ଏହି;
  }
  ଯଦି (src.length() > INT32_MAX) {
    setToBogus();
    ଫେରନ୍ତୁ *ଏହି;
  }
  ଫେରନ୍ତୁ doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
ଶୂନ୍ୟ
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    ଅଚଳ UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
ଶୂନ୍ୟ 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    ଯଦି (limit <= start) {
        ଫେରନ୍ତୁ; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(ଆକାର(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    ଯଦି (text != nullptr) {
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
UBool Replaceable::hasMetaData() ଅଚଳ {
    ଫେରନ୍ତୁ ସତ୍ୟ;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() ଅଚଳ {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  ଯଦି(length <= 1 || !cloneArrayIfNeeded()) {
    ଫେରନ୍ତୁ *ଏହି;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  ଯଦି(length <= 1) {  // pinIndices() might have shrunk the length
    ଫେରନ୍ତୁ *ଏହି;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *ଡାହାଣ = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = ମିଥ୍ୟା;

  // Before the loop we know left<right because length>=2.
  କର {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *ଡାହାଣ));
    *ଡାହାଣ-- = swap;
  } ଯେତେବେଳେ(left < ଡାହାଣ);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  ଯଦି(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    ଡାହାଣ = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    ଯେତେବେଳେ(left < ଡାହାଣ) {
      ଯଦି(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } ଅନ୍ୟଥା {
        ++left;
      }
    }
  }

  ଫେରନ୍ତୁ *ଏହି;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  ଯଦି(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  } ଅନ୍ୟଥା {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    ଯେତେବେଳେ(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  ଯଦି(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  } ଅନ୍ୟଥା {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    ଯେତେବେଳେ(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    ଫେରନ୍ତୁ ସତ୍ୟ;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() ଅଚଳ
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    ଯଦି (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    ଫେରନ୍ତୁ hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  ଯଦି(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    ଫେରନ୍ତୁ getArrayStart();
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ nullptr;
  }
}

ଶୂନ୍ୟ
UnicodeString::releaseBuffer(int32_t newLength) {
  ଯଦି(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    ଯଦି(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      ଅଚଳ char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      ଯେତେବେଳେ(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } ଅନ୍ୟଥା ଯଦି(newLength>capacity) {
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
  ଯଦି(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  ଯଦି(!isWritable()) {
    ଫେରନ୍ତୁ ମିଥ୍ୟା;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  ଯଦି(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_ଗଣନା() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    ଯଦି(growCapacity < 0) {
      growCapacity = newCapacity;
    } ଅନ୍ୟଥା ଯଦି(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } ଅନ୍ୟଥା ଯଦି(newCapacity > growCapacity) {
      setToBogus();
      ଫେରନ୍ତୁ ମିଥ୍ୟା;  // bad inputs
    }
    ଯଦି(growCapacity > kMaxCapacity) {
      setToBogus();
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }

    // save old values
    char16_t old_ଗଦା_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    ଯଦି(flags&k_Using_ଗଦା_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      ଯଦି(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_ଗଦା_Fields.fBuffer, 0, old_ଗଦା_Buffer, 0, oldLength);
        oldArray = old_ଗଦା_Buffer;
      } ଅନ୍ୟଥା {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } ଅନ୍ୟଥା {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    ଯଦି(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      ଯଦି(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        ଯଦି(newCapacity < minLength) {
          minLength = newCapacity;
        }
        ଯଦି(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } ଅନ୍ୟଥା {
        setZeroLength();
      }

      // release the old array
      ଯଦି(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_ଗଣନା = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        ଯଦି(umtx_atomic_dec(p_Ref_ଗଣନା) == 0) {
          ଯଦି (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((ଶୂନ୍ୟ *)p_Ref_ଗଣନା);
          } ଅନ୍ୟଥା {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_ଗଣନା);
          }
        }
      }
    } ଅନ୍ୟଥା {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      ଯଦି(!(flags&k_Using_ଗଦା_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
  }
  ଫେରନ୍ତୁ ସତ୍ୟ;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  ଫେରନ୍ତୁ str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = ମିଥ୍ୟା;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  ଫେରନ୍ତୁ !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(ଅଚଳ char16_t *s, int32_t length) {
  ଫେରନ୍ତୁ str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  ଫେରନ୍ତୁ str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  ଯଦି(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    ଫେରନ୍ତୁ nullptr;
  }
  int32_t oldLength = str.length();
  ଯଦି(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    ଫେରନ୍ତୁ str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  ଫେରନ୍ତୁ scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(ଅଚଳ UElement ଚାବି) {
    ଅଚଳ UnicodeString *str = (ଅଚଳ UnicodeString*) ଚାବି.pointer;
    ଫେରନ୍ତୁ (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(ଅଚଳ UElement key1, ଅଚଳ UElement key2) {
    ଅଚଳ UnicodeString *str1 = (ଅଚଳ UnicodeString*) key1.pointer;
    ଅଚଳ UnicodeString *str2 = (ଅଚଳ UnicodeString*) key2.pointer;
    ଯଦି (str1 == str2) {
        ଫେରନ୍ତୁ ସତ୍ୟ;
    }
    ଯଦି (str1 == nullptr || str2 == nullptr) {
        ଫେରନ୍ତୁ ମିଥ୍ୟା;
    }
    ଫେରନ୍ତୁ *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#ଯଦି defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
ସ୍ଥିର ଶୂନ୍ୟ uprv_UnicodeStringDummy() {
    delete [] (ନୂତନ UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
