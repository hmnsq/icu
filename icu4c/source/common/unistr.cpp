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

#അന്തര്ഭാവ <string_view>

#അന്തര്ഭാവ "unicode/utypes.h"
#അന്തര്ഭാവ "unicode/appendable.h"
#അന്തര്ഭാവ "unicode/putil.h"
#അന്തര്ഭാവ "cstring.h"
#അന്തര്ഭാവ "cmemory.h"
#അന്തര്ഭാവ "unicode/ustring.h"
#അന്തര്ഭാവ "unicode/unistr.h"
#അന്തര്ഭാവ "unicode/utf.h"
#അന്തര്ഭാവ "unicode/utf16.h"
#അന്തര്ഭാവ "uelement.h"
#അന്തര്ഭാവ "ustr_imp.h"
#അന്തര്ഭാവ "umutex.h"
#അന്തര്ഭാവ "uassert.h"

#എങ്കിൽ 0

#അന്തര്ഭാവ <iostream>
using നാമാകാശ std;

//DEBUGGING
ശൂന്യം
print(സ്ഥിരം UnicodeString& s,
      സ്ഥിരം അക്ഷരം *name)
{
  char16_t c;
  cout << name << ":|";
  വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < s.length(); ++i) {
    c = s[i];
    എങ്കിൽ(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    അല്ലെങ്കിൽ
      cout << (അക്ഷരം) s[i];
  }
  cout << '|' << endl;
}

ശൂന്യം
print(സ്ഥിരം char16_t *s,
      int32_t len,
      സ്ഥിരം അക്ഷരം *name)
{
  char16_t c;
  cout << name << ":|";
  വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < len; ++i) {
    c = s[i];
    എങ്കിൽ(c>= 0x007E || c < 0x0020)
      cout << "[0x" << hex << s[i] << "]";
    അല്ലെങ്കിൽ
      cout << (അക്ഷരം) s[i];
  }
  cout << '|' << endl;
}
// END DEBUGGING
#endif

// Local function definitions for now

// need to copy areas that may overlap
സ്ഥിതം
വരിയിൽ ശൂന്യം
us_arrayCopy(സ്ഥിരം char16_t *src, int32_t srcStart,
         char16_t *dst, int32_t dstStart, int32_t count)
{
  എങ്കിൽ(count>0) {
    uprv_memmove(dst+dstStart, src+srcStart, (size_t)count*വലുപ്പം(*src));
  }
}

// u_unescapeAt() callback to get a char16_t from a UnicodeString
U_CDECL_BEGIN
സ്ഥിതം char16_t U_CALLCONV
UnicodeString_charAt(int32_t offset, ശൂന്യം *context) {
    തിരികെ ((icu::UnicodeString*) context)->charAt(offset);
}
U_CDECL_END

U_NAMESPACE_BEGIN

/* The Replaceable virtual destructor can't be defined in the header
   due to how AIX works with multiple definitions of virtual functions.
*/
Replaceable::~Replaceable() {}

UOBJECT_DEFINE_RTTI_IMPLEMENTATION(UnicodeString)

UnicodeString U_EXPORT2
operator+ (സ്ഥിരം UnicodeString &s1, സ്ഥിരം UnicodeString &s2) {
  int32_t sumLengths;
  എങ്കിൽ (uprv_add32_overflow(s1.length(), s2.length(), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    തിരികെ bogus;
  }
  എങ്കിൽ (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  തിരികെ UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}

U_COMMON_API UnicodeString U_EXPORT2
unistr_internalConcat(സ്ഥിരം UnicodeString &s1, std::u16string_view s2) {
  int32_t sumLengths;
  എങ്കിൽ (s2.length() > INT32_MAX ||
      uprv_add32_overflow(s1.length(), static_cast<int32_t>(s2.length()), &sumLengths)) {
    UnicodeString bogus;
    bogus.setToBogus();
    തിരികെ bogus;
  }
  എങ്കിൽ (sumLengths != INT32_MAX) {
    ++sumLengths;  // space for a terminating NUL if we need one
  }
  തിരികെ UnicodeString(sumLengths, static_cast<UChar32>(0), 0).append(s1).append(s2);
}


//========================================
// Reference Counting functions, put at top of file so that optimizing compilers
//                               have a chance to automatically inline.
//========================================

ശൂന്യം
UnicodeString::addRef() {
  umtx_atomic_inc(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::removeRef() {
  തിരികെ umtx_atomic_dec(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1);
}

int32_t
UnicodeString::ref_എണ്ണം() സ്ഥിരം {
  തിരികെ umtx_loadAcquire(*(reinterpret_cast<u_atomic_int32_t*>(fUnion.fFields.fArray) - 1));
}

ശൂന്യം
UnicodeString::releaseArray() {
  എങ്കിൽ((fUnion.fFields.fLengthAndFlags & kRefCounted) && removeRef() == 0) {
    uprv_free(reinterpret_cast<int32_t*>(fUnion.fFields.fArray) - 1);
  }
}



//========================================
// Constructors
//========================================

// The default constructor is inline in unistr.h.

UnicodeString::UnicodeString(int32_t capacity, UChar32 c, int32_t count) {
  fUnion.fFields.fLengthAndFlags = 0;
  എങ്കിൽ (count <= 0 || static_cast<uint32_t>(c) > 0x10ffff) {
    // just allocate and do not do anything else
    allocate(capacity);
  } അല്ലെങ്കിൽ എങ്കിൽ(c <= 0xffff) {
    int32_t length = count;
    എങ്കിൽ(capacity < length) {
      capacity = length;
    }
    എങ്കിൽ(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t unit = static_cast<char16_t>(c);
      വേണ്ടി(int32_t i = 0; i < length; ++i) {
        array[i] = unit;
      }
      setLength(length);
    }
  } അല്ലെങ്കിൽ {  // supplementary code point, write surrogate pairs
    എങ്കിൽ(count > (INT32_MAX / 2)) {
      // We would get more than 2G UChars.
      allocate(capacity);
      തിരികെ;
    }
    int32_t length = count * 2;
    എങ്കിൽ(capacity < length) {
      capacity = length;
    }
    എങ്കിൽ(allocate(capacity)) {
      char16_t *array = getArrayStart();
      char16_t lead = U16_LEAD(c);
      char16_t trail = U16_TRAIL(c);
      വേണ്ടി(int32_t i = 0; i < length; i += 2) {
        array[i] = lead;
        array[i + 1] = trail;
      }
      setLength(length);
    }
  }
}

UnicodeString::UnicodeString(char16_t ch) {
  fUnion.fFields.fLengthAndFlags = kLength1 | kShortString;
  fUnion.f_അട്ടി_Fields.fBuffer[0] = ch;
}

UnicodeString::UnicodeString(UChar32 ch) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  int32_t i = 0;
  UBool isError = അസത്യം;
  U16_APPEND(fUnion.f_അട്ടി_Fields.fBuffer, i, US_STACKBUF_SIZE, ch, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then i==0 which is what we want anyway.
  എങ്കിൽ(!isError) {
    setShortLength(i);
  }
}

UnicodeString::UnicodeString(സ്ഥിരം char16_t *text,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  doAppend(text, 0, textLength);
}

UnicodeString::UnicodeString(UBool isTerminated,
                             ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                             int32_t textLength) {
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  സ്ഥിരം char16_t *text = textPtr;
  എങ്കിൽ(text == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } അല്ലെങ്കിൽ എങ്കിൽ(textLength < -1 ||
            (textLength == -1 && !isTerminated) ||
            (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
  } അല്ലെങ്കിൽ {
    എങ്കിൽ(textLength == -1) {
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
  എങ്കിൽ(buff == nullptr) {
    // treat as an empty string, do not alias
    setToEmpty();
  } അല്ലെങ്കിൽ എങ്കിൽ(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
  } അല്ലെങ്കിൽ {
    എങ്കിൽ(buffLength == -1) {
      // fLength = u_strlen(buff); but do not look beyond buffCapacity
      സ്ഥിരം char16_t *p = buff, *limit = buff + buffCapacity;
      ആകുമ്പോൾ(p != limit && *p != 0) {
        ++p;
      }
      buffLength = static_cast<int32_t>(p - buff);
    }
    setArray(buff, buffLength, buffCapacity);
  }
}

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *src, int32_t length, EInvariant) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  എങ്കിൽ(src==nullptr) {
    // treat as an empty string
  } അല്ലെങ്കിൽ {
    എങ്കിൽ(length<0) {
      length = static_cast<int32_t>(uprv_strlen(src));
    }
    എങ്കിൽ(cloneArrayIfNeeded(length, length, അസത്യം)) {
      u_charsToUChars(src, getArrayStart(), length);
      setLength(length);
    } അല്ലെങ്കിൽ {
      setToBogus();
    }
  }
}

UnicodeString
UnicodeString::readOnlyAliasFromU16StringView(std::u16string_view text U_LIFETIME_BOUND) {
  UnicodeString result;
  എങ്കിൽ (text.length() <= INT32_MAX) {
    result.setTo(അസത്യം, text.data(), static_cast<int32_t>(text.length()));
  } അല്ലെങ്കിൽ {
    result.setToBogus();
  }
  തിരികെ result;
}

UnicodeString
UnicodeString::readOnlyAliasFromUnicodeString(സ്ഥിരം UnicodeString& text U_LIFETIME_BOUND) {
  UnicodeString result;
  എങ്കിൽ (text.isBogus()) {
    result.setToBogus();
  } അല്ലെങ്കിൽ {
    result.setTo(അസത്യം, text.getBuffer(), text.length());
  }
  തിരികെ result;
}

#എങ്കിൽ U_CHARSET_IS_UTF8

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  എങ്കിൽ (codepageData != nullptr) {
    setToUTF8(codepageData);
  }
}

UnicodeString::UnicodeString(സ്ഥിരം അക്ഷരം *codepageData, int32_t dataLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  // if there's nothing to convert, do nothing
  എങ്കിൽ (codepageData == nullptr || dataLength == 0 || dataLength < -1) {
    തിരികെ;
  }
  എങ്കിൽ(dataLength == -1) {
    dataLength = static_cast<int32_t>(uprv_strlen(codepageData));
  }
  setToUTF8(StringPiece(codepageData, dataLength));
}

// else see unistr_cnv.cpp
#endif

UnicodeString::UnicodeString(സ്ഥിരം UnicodeString& that) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  copyFrom(that);
}

UnicodeString::UnicodeString(UnicodeString &&src) noexcept {
  copyFieldsFrom(src, സത്യം);
}

UnicodeString::UnicodeString(സ്ഥിരം UnicodeString& that,
                             int32_t srcStart) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart);
}

UnicodeString::UnicodeString(സ്ഥിരം UnicodeString& that,
                             int32_t srcStart,
                             int32_t srcLength) {
  fUnion.fFields.fLengthAndFlags = kShortString;
  setTo(that, srcStart, srcLength);
}

// Replaceable base class clone() default implementation, does not clone
Replaceable *
Replaceable::clone() സ്ഥിരം {
  തിരികെ nullptr;
}

// UnicodeString overrides clone() with a real implementation
UnicodeString *
UnicodeString::clone() സ്ഥിരം {
  LocalPointer<UnicodeString> clonedString(പുതിയ UnicodeString(*ഇത്));
  തിരികെ clonedString.isValid() && !clonedString->isBogus() ? clonedString.orphan() : nullptr;
}

//========================================
// array allocation
//========================================

നാമാകാശ {

സ്ഥിരം int32_t kGrowSize = 128;

// The number of bytes for one int32_t reference counter and capacity UChars
// must fit into a 32-bit size_t (at least when on a 32-bit platform).
// We also add one for the NUL terminator, to avoid reallocation in getTerminatedBuffer(),
// and round up to a multiple of 16 bytes.
// This means that capacity must be at most (0xfffffff0 - 4) / 2 - 1 = 0x7ffffff5.
// (With more complicated checks we could go up to 0x7ffffffd without rounding up,
// but that does not seem worth it.)
സ്ഥിരം int32_t kMaxCapacity = 0x7ffffff5;

int32_t getGrowCapacity(int32_t newLength) {
  int32_t growSize = (newLength >> 2) + kGrowSize;
  എങ്കിൽ(growSize <= (kMaxCapacity - newLength)) {
    തിരികെ newLength + growSize;
  } അല്ലെങ്കിൽ {
    തിരികെ kMaxCapacity;
  }
}

}  // namespace

UBool
UnicodeString::allocate(int32_t capacity) {
  എങ്കിൽ(capacity <= US_STACKBUF_SIZE) {
    fUnion.fFields.fLengthAndFlags = kShortString;
    തിരികെ സത്യം;
  }
  എങ്കിൽ(capacity <= kMaxCapacity) {
    ++capacity;  // for the NUL
    // Switch to size_t which is unsigned so that we can allocate up to 4GB.
    // Reference counter + UChars.
    size_t അക്കം_Bytes = വലുപ്പം(int32_t) + static_cast<size_t>(capacity) * U_SIZEOF_UCHAR;
    // Round up to a multiple of 16.
    അക്കം_Bytes = (അക്കം_Bytes + 15) & ~15;
    int32_t* array = static_cast<int32_t*>(uprv_malloc(അക്കം_Bytes));
    എങ്കിൽ(array != nullptr) {
      // set initial refCount and point behind the refCount
      *array++ = 1;
      അക്കം_Bytes -= വലുപ്പം(int32_t);

      // have fArray point to the first char16_t
      fUnion.fFields.fArray = reinterpret_cast<char16_t*>(array);
      fUnion.fFields.fCapacity = static_cast<int32_t>(അക്കം_Bytes / U_SIZEOF_UCHAR);
      fUnion.fFields.fLengthAndFlags = kLongString;
      തിരികെ സത്യം;
    }
  }
  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
  തിരികെ അസത്യം;
}

//========================================
// Destructor
//========================================

#ifdef UNISTR_എണ്ണം_FINAL_STRING_LENGTHS
സ്ഥിതം u_atomic_int32_t finalLengthCounts[0x400];  // UnicodeString::kMaxShortLength+1
സ്ഥിതം u_atomic_int32_t beyond_എണ്ണം(0);

U_CAPI ശൂന്യം unistr_printLengths() {
  int32_t i;
  വേണ്ടി(i = 0; i <= 59; ++i) {
    printf("%2d,  %9d\n", i, (int32_t)finalLengthCounts[i]);
  }
  int32_t beyond = beyond_എണ്ണം;
  വേണ്ടി(; i < UPRV_LENGTHOF(finalLengthCounts); ++i) {
    beyond += finalLengthCounts[i];
  }
  printf(">59, %9d\n", beyond);
}
#endif

UnicodeString::~UnicodeString()
{
#ifdef UNISTR_എണ്ണം_FINAL_STRING_LENGTHS
  // Count lengths of strings at the end of their lifetime.
  // Useful for discussion of a desirable stack buffer size.
  // Count the contents length, not the optional NUL terminator nor further capacity.
  // Ignore open-buffer strings and strings which alias external storage.
  എങ്കിൽ((fUnion.fFields.fLengthAndFlags&(kOpenGetBuffer|kReadonlyAlias|kWritableAlias)) == 0) {
    എങ്കിൽ(hasShortLength()) {
      umtx_atomic_inc(finalLengthCounts + getShortLength());
    } അല്ലെങ്കിൽ {
      umtx_atomic_inc(&beyond_എണ്ണം);
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
  തിരികെ result;
}

UnicodeString UnicodeString::fromUTF32(സ്ഥിരം UChar32 *utf32, int32_t length) {
  UnicodeString result;
  int32_t capacity;
  // Most UTF-32 strings will be BMP-only and result in a same-length
  // UTF-16 string. We overestimate the capacity just slightly,
  // just in case there are a few supplementary characters.
  എങ്കിൽ(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } അല്ലെങ്കിൽ {
    capacity = length + (length >> 4) + 4;
  }
  ചെയ്യുക {
    char16_t *utf16 = result.getBuffer(capacity);
    int32_t length16;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strFromUTF32WithSub(utf16, result.getCapacity(), &length16,
        utf32, length,
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
    result.releaseBuffer(length16);
    എങ്കിൽ(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      capacity = length16 + 1;  // +1 for the terminating NUL.
      തുടരുക;
    } അല്ലെങ്കിൽ എങ്കിൽ(U_FAILURE(errorCode)) {
      result.setToBogus();
    }
    നിർത്തുക;
  } ആകുമ്പോൾ(സത്യം);
  തിരികെ result;
}

//========================================
// Assignment
//========================================

UnicodeString&
UnicodeString::operator=(സ്ഥിരം UnicodeString& src) U_LIFETIME_BOUND {
  തിരികെ copyFrom(src);
}

UnicodeString&
UnicodeString::fastCopyFrom(സ്ഥിരം UnicodeString& src) U_LIFETIME_BOUND {
  തിരികെ copyFrom(src, സത്യം);
}

UnicodeString&
UnicodeString::copyFrom(സ്ഥിരം UnicodeString& src, UBool fastCopy) U_LIFETIME_BOUND {
  // if assigning to ourselves, do nothing
  എങ്കിൽ(ഇത് == &src) {
    തിരികെ *ഇത്;
  }

  // is the right side bogus?
  എങ്കിൽ(src.isBogus()) {
    setToBogus();
    തിരികെ *ഇത്;
  }

  // delete the current contents
  releaseArray();

  എങ്കിൽ(src.isEmpty()) {
    // empty string - use the stack buffer
    setToEmpty();
    തിരികെ *ഇത്;
  }

  // fLength>0 and not an "open" src.getBuffer(minCapacity)
  fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  തിരഞ്ഞെടുപ്പ്(src.fUnion.fFields.fLengthAndFlags & kAllStorageFlags) {
  അവസ്ഥ kShortString:
    // short string using the stack buffer, do the same
    uprv_memcpy(fUnion.f_അട്ടി_Fields.fBuffer, src.fUnion.f_അട്ടി_Fields.fBuffer,
                getShortLength() * U_SIZEOF_UCHAR);
    നിർത്തുക;
  അവസ്ഥ kLongString:
    // src uses a refCounted string buffer, use that buffer with refCount
    // src is const, use a cast - we don't actually change it
    const_cast<UnicodeString &>(src).addRef();
    // copy all fields, share the reference-counted buffer
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    എങ്കിൽ(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    നിർത്തുക;
  അവസ്ഥ kReadonlyAlias:
    എങ്കിൽ(fastCopy) {
      // src is a readonly alias, do the same
      // -> maintain the readonly alias as such
      fUnion.fFields.fArray = src.fUnion.fFields.fArray;
      fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
      എങ്കിൽ(!hasShortLength()) {
        fUnion.fFields.fLength = src.fUnion.fFields.fLength;
      }
      നിർത്തുക;
    }
    // else if(!fastCopy) fall through to case kWritableAlias
    // -> allocate a new buffer and copy the contents
    U_FALLTHROUGH;
  അവസ്ഥ kWritableAlias: {
    // src is a writable alias; we make a copy of that instead
    int32_t srcLength = src.length();
    എങ്കിൽ(allocate(srcLength)) {
      u_memcpy(getArrayStart(), src.getArrayStart(), srcLength);
      setLength(srcLength);
      നിർത്തുക;
    }
    // if there is not enough memory, then fall through to setting to bogus
    U_FALLTHROUGH;
  }
  സ്ഥിരസ്ഥിതി:
    // if src is bogus, set ourselves to bogus
    // do not call setToBogus() here because fArray and flags are not consistent here
    fUnion.fFields.fLengthAndFlags = kIsBogus;
    fUnion.fFields.fArray = nullptr;
    fUnion.fFields.fCapacity = 0;
    നിർത്തുക;
  }

  തിരികെ *ഇത്;
}

UnicodeString& UnicodeString::operator=(UnicodeString&& src) noexcept U_LIFETIME_BOUND {
  // No explicit check for self move assignment, consistent with standard library.
  // Self move assignment causes no crash nor leak but might make the object bogus.
  releaseArray();
  copyFieldsFrom(src, സത്യം);
  തിരികെ *ഇത്;
}

// Same as move assignment except without memory management.
ശൂന്യം UnicodeString::copyFieldsFrom(UnicodeString &src, UBool setSrcToBogus) noexcept {
  int16_t lengthAndFlags = fUnion.fFields.fLengthAndFlags = src.fUnion.fFields.fLengthAndFlags;
  എങ്കിൽ(lengthAndFlags & k_Using_അട്ടി_Buffer) {
    // Short string using the stack buffer, copy the contents.
    // Check for self assignment to prevent "overlap in memcpy" warnings,
    // although it should be harmless to copy a buffer to itself exactly.
    എങ്കിൽ(ഇത് != &src) {
      uprv_memcpy(fUnion.f_അട്ടി_Fields.fBuffer, src.fUnion.f_അട്ടി_Fields.fBuffer,
                  getShortLength() * U_SIZEOF_UCHAR);
    }
  } അല്ലെങ്കിൽ {
    // In all other cases, copy all fields.
    fUnion.fFields.fArray = src.fUnion.fFields.fArray;
    fUnion.fFields.fCapacity = src.fUnion.fFields.fCapacity;
    എങ്കിൽ(!hasShortLength()) {
      fUnion.fFields.fLength = src.fUnion.fFields.fLength;
    }
    എങ്കിൽ(setSrcToBogus) {
      // Set src to bogus without releasing any memory.
      src.fUnion.fFields.fLengthAndFlags = kIsBogus;
      src.fUnion.fFields.fArray = nullptr;
      src.fUnion.fFields.fCapacity = 0;
    }
  }
}

ശൂന്യം UnicodeString::swap(UnicodeString &other) noexcept {
  UnicodeString temp;  // Empty short string: Known not to need releaseArray().
  // Copy fields without resetting source values in between.
  temp.copyFieldsFrom(*ഇത്, അസത്യം);
  ഇത്->copyFieldsFrom(other, അസത്യം);
  other.copyFieldsFrom(temp, അസത്യം);
  // Set temp to an empty string so that other's memory is not released twice.
  temp.fUnion.fFields.fLengthAndFlags = kShortString;
}

//========================================
// Miscellaneous operations
//========================================

UnicodeString UnicodeString::unescape() സ്ഥിരം {
    UnicodeString result(length(), static_cast<UChar32>(0), static_cast<int32_t>(0)); // construct with capacity
    എങ്കിൽ (result.isBogus()) {
        തിരികെ result;
    }
    സ്ഥിരം char16_t *array = getBuffer();
    int32_t len = length();
    int32_t prev = 0;
    വേണ്ടി (int32_t i=0;;) {
        എങ്കിൽ (i == len) {
            result.append(array, prev, len - prev);
            നിർത്തുക;
        }
        എങ്കിൽ (array[i++] == 0x5C /*'\\'*/) {
            result.append(array, prev, (i - 1) - prev);
            UChar32 c = unescapeAt(i); // advances i
            എങ്കിൽ (c < 0) {
                result.remove(); // return empty string
                നിർത്തുക; // invalid escape sequence
            }
            result.append(c);
            prev = i;
        }
    }
    തിരികെ result;
}

UChar32 UnicodeString::unescapeAt(int32_t &offset) സ്ഥിരം {
    തിരികെ u_unescapeAt(UnicodeString_charAt, &offset, length(), (ശൂന്യം*)ഇത്);
}

//========================================
// Read-only implementation
//========================================
UBool
UnicodeString::doEquals(സ്ഥിരം char16_t *text, int32_t len) സ്ഥിരം {
  // Requires: this not bogus and have same lengths.
  // Byte-wise comparison works for equality regardless of endianness.
  തിരികെ uprv_memcmp(getArrayStart(), text, len * U_SIZEOF_UCHAR) == 0;
}

UBool
UnicodeString::doEqualsSubstring( int32_t start,
              int32_t length,
              സ്ഥിരം char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) സ്ഥിരം
{
  // compare illegal string values
  എങ്കിൽ(isBogus()) {
    തിരികെ അസത്യം;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  എങ്കിൽ(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    തിരികെ length == 0 ? സത്യം : അസത്യം;
  }

  // get the correct pointer
  സ്ഥിരം char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  // get the srcLength if necessary
  എങ്കിൽ(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  എങ്കിൽ (length != srcLength) {
    തിരികെ അസത്യം;
  }

  എങ്കിൽ(length == 0 || chars == srcChars) {
    തിരികെ സത്യം;
  }

  തിരികെ u_memcmp(chars, srcChars, srcLength) == 0;
}

int8_t
UnicodeString::doCompare( int32_t start,
              int32_t length,
              സ്ഥിരം char16_t *srcChars,
              int32_t srcStart,
              int32_t srcLength) സ്ഥിരം
{
  // compare illegal string values
  എങ്കിൽ(isBogus()) {
    തിരികെ -1;
  }
  
  // pin indices to legal values
  pinIndices(start, length);

  എങ്കിൽ(srcChars == nullptr) {
    // treat const char16_t *srcChars==nullptr as an empty string
    തിരികെ length == 0 ? 0 : 1;
  }

  // get the correct pointer
  സ്ഥിരം char16_t *chars = getArrayStart();

  chars += start;
  srcChars += srcStart;

  int32_t minLength;
  int8_t lengthResult;

  // get the srcLength if necessary
  എങ്കിൽ(srcLength < 0) {
    srcLength = u_strlen(srcChars + srcStart);
  }

  // are we comparing different lengths?
  എങ്കിൽ(length != srcLength) {
    എങ്കിൽ(length < srcLength) {
      minLength = length;
      lengthResult = -1;
    } അല്ലെങ്കിൽ {
      minLength = srcLength;
      lengthResult = 1;
    }
  } അല്ലെങ്കിൽ {
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

  എങ്കിൽ(minLength > 0 && chars != srcChars) {
    int32_t result;

#   എങ്കിൽ U_IS_BIG_ENDIAN 
      // big-endian: byte comparison works
      result = uprv_memcmp(chars, srcChars, minLength * വലുപ്പം(char16_t));
      എങ്കിൽ(result != 0) {
        തിരികെ (int8_t)(result >> 15 | 1);
      }
#   അല്ലെങ്കിൽ
      // little-endian: compare char16_t units
      ചെയ്യുക {
        result = static_cast<int32_t>(*(chars++)) - static_cast<int32_t>(*(srcChars++));
        എങ്കിൽ(result != 0) {
          തിരികെ static_cast<int8_t>(result >> 15 | 1);
        }
      } ആകുമ്പോൾ(--minLength > 0);
#   endif
  }
  തിരികെ lengthResult;
}

/* String compare in code point order - doCompare() compares in code unit order. */
int8_t
UnicodeString::doCompareCodePointOrder(int32_t start,
                                       int32_t length,
                                       സ്ഥിരം char16_t *srcChars,
                                       int32_t srcStart,
                                       int32_t srcLength) സ്ഥിരം
{
  // compare illegal string values
  // treat const char16_t *srcChars==nullptr as an empty string
  എങ്കിൽ(isBogus()) {
    തിരികെ -1;
  }

  // pin indices to legal values
  pinIndices(start, length);

  എങ്കിൽ(srcChars == nullptr) {
    srcStart = srcLength = 0;
  }

  int32_t diff = uprv_strCompare(getArrayStart() + start, length, (srcChars!=nullptr)?(srcChars + srcStart):nullptr, srcLength, അസത്യം, സത്യം);
  /* translate the 32-bit result into an 8-bit one */
  എങ്കിൽ(diff!=0) {
    തിരികെ static_cast<int8_t>(diff >> 15 | 1);
  } അല്ലെങ്കിൽ {
    തിരികെ 0;
  }
}

int32_t
UnicodeString::getLength() സ്ഥിരം {
    തിരികെ length();
}

char16_t
UnicodeString::getCharAt(int32_t offset) സ്ഥിരം {
  തിരികെ charAt(offset);
}

UChar32
UnicodeString::getChar32At(int32_t offset) സ്ഥിരം {
  തിരികെ char32At(offset);
}

UChar32
UnicodeString::char32At(int32_t offset) സ്ഥിരം
{
  int32_t len = length();
  എങ്കിൽ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    സ്ഥിരം char16_t *array = getArrayStart();
    UChar32 c;
    U16_GET(array, 0, offset, len, c);
    തിരികെ c;
  } അല്ലെങ്കിൽ {
    തിരികെ kInvalidUChar;
  }
}

int32_t
UnicodeString::getChar32Start(int32_t offset) സ്ഥിരം {
  എങ്കിൽ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(length())) {
    സ്ഥിരം char16_t *array = getArrayStart();
    U16_SET_CP_START(array, 0, offset);
    തിരികെ offset;
  } അല്ലെങ്കിൽ {
    തിരികെ 0;
  }
}

int32_t
UnicodeString::getChar32Limit(int32_t offset) സ്ഥിരം {
  int32_t len = length();
  എങ്കിൽ (static_cast<uint32_t>(offset) < static_cast<uint32_t>(len)) {
    സ്ഥിരം char16_t *array = getArrayStart();
    U16_SET_CP_LIMIT(array, 0, offset, len);
    തിരികെ offset;
  } അല്ലെങ്കിൽ {
    തിരികെ len;
  }
}

int32_t
UnicodeString::എണ്ണം_Char32(int32_t start, int32_t length) സ്ഥിരം {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_countChar32() checks for nullptr
  തിരികെ u_എണ്ണം_Char32(getArrayStart()+start, length);
}

UBool
UnicodeString::hasMoreChar32Than(int32_t start, int32_t length, int32_t number) സ്ഥിരം {
  pinIndices(start, length);
  // if(isBogus()) then fArray==0 and start==0 - u_strHasMoreChar32Than() checks for nullptr
  തിരികെ u_strHasMoreChar32Than(getArrayStart()+start, length, number);
}

int32_t
UnicodeString::moveIndex32(int32_t index, int32_t delta) സ്ഥിരം {
  // pin index
  int32_t len = length();
  എങ്കിൽ(index<0) {
    index=0;
  } അല്ലെങ്കിൽ എങ്കിൽ(index>len) {
    index=len;
  }

  സ്ഥിരം char16_t *array = getArrayStart();
  എങ്കിൽ(delta>0) {
    U16_FWD_N(array, index, len, delta);
  } അല്ലെങ്കിൽ {
    U16_BACK_N(array, 0, index, -delta);
  }

  തിരികെ index;
}

ശൂന്യം
UnicodeString::doExtract(int32_t start,
             int32_t length,
             char16_t *dst,
             int32_t dstStart) സ്ഥിരം
{
  // pin indices to legal values
  pinIndices(start, length);

  // do not copy anything if we alias dst itself
  സ്ഥിരം char16_t *array = getArrayStart();
  എങ്കിൽ(array + start != dst + dstStart) {
    us_arrayCopy(array, start, dst, dstStart, length);
  }
}

int32_t
UnicodeString::extract(Char16Ptr dest, int32_t destCapacity,
                       UErrorCode &errorCode) സ്ഥിരം {
  int32_t len = length();
  എങ്കിൽ(U_SUCCESS(errorCode)) {
    എങ്കിൽ (isBogus() || destCapacity < 0 || (destCapacity > 0 && dest == nullptr)) {
      errorCode=U_ILLEGAL_ARGUMENT_ERROR;
    } അല്ലെങ്കിൽ {
      സ്ഥിരം char16_t *array = getArrayStart();
      എങ്കിൽ(len>0 && len<=destCapacity && array!=dest) {
        u_memcpy(dest, array, len);
      }
      തിരികെ u_terminateUChars(dest, destCapacity, len, &errorCode);
    }
  }

  തിരികെ len;
}

int32_t
UnicodeString::extract(int32_t start,
                       int32_t length,
                       അക്ഷരം *target,
                       int32_t targetCapacity,
                       എണ്ണൽ EInvariant) സ്ഥിരം
{
  // if the arguments are illegal, then do nothing
  എങ്കിൽ(targetCapacity < 0 || (targetCapacity > 0 && target == nullptr)) {
    തിരികെ 0;
  }

  // pin the indices to legal values
  pinIndices(start, length);

  എങ്കിൽ(length <= targetCapacity) {
    u_UCharsToChars(getArrayStart() + start, target, length);
  }
  UErrorCode status = U_ZERO_ERROR;
  തിരികെ u_terminateChars(target, targetCapacity, length, &status);
}

UnicodeString
UnicodeString::tempSubString(int32_t start, int32_t len) സ്ഥിരം U_LIFETIME_BOUND {
  pinIndices(start, len);
  സ്ഥിരം char16_t *array = getBuffer();  // not getArrayStart() to check kIsBogus & kOpenGetBuffer
  എങ്കിൽ(array==nullptr) {
    array=fUnion.f_അട്ടി_Fields.fBuffer;  // anything not nullptr because that would make an empty string
    len=-2;  // bogus result string
  }
  തിരികെ UnicodeString(അസത്യം, array + start, len);
}

int32_t
UnicodeString::toUTF8(int32_t start, int32_t len,
                      അക്ഷരം *target, int32_t capacity) സ്ഥിരം {
  pinIndices(start, len);
  int32_t length8;
  UErrorCode errorCode = U_ZERO_ERROR;
  u_strToUTF8WithSub(target, capacity, &length8,
                     getBuffer() + start, len,
                     0xFFFD,  // Standard substitution character.
                     nullptr,    // Don't care about number of substitutions.
                     &errorCode);
  തിരികെ length8;
}

#എങ്കിൽ U_CHARSET_IS_UTF8

int32_t
UnicodeString::extract(int32_t start, int32_t len,
                       അക്ഷരം *target, uint32_t dstSize) സ്ഥിരം {
  // if the arguments are illegal, then do nothing
  എങ്കിൽ (/*dstSize < 0 || */(dstSize > 0 && target == nullptr)) {
    തിരികെ 0;
  }
  തിരികെ toUTF8(start, len, target, dstSize <= 0x7fffffff ? static_cast<int32_t>(dstSize) : 0x7fffffff);
}

// else see unistr_cnv.cpp
#endif

ശൂന്യം 
UnicodeString::extractBetween(int32_t start,
                  int32_t limit,
                  UnicodeString& target) സ്ഥിരം {
  pinIndex(start);
  pinIndex(limit);
  doExtract(start, limit - start, target);
}

// When converting from UTF-16 to UTF-8, the result will have at most 3 times
// as many bytes as the source has UChars.
// The "worst cases" are writing systems like Indic, Thai and CJK with
// 3:1 bytes:UChars.
ശൂന്യം
UnicodeString::toUTF8(ByteSink &sink) സ്ഥിരം {
  int32_t length16 = length();
  എങ്കിൽ(length16 != 0) {
    അക്ഷരം അട്ടി_Buffer[1024];
    int32_t capacity = static_cast<int32_t>(വലുപ്പം(അട്ടി_Buffer));
    UBool utf8IsOwned = അസത്യം;
    അക്ഷരം *utf8 = sink.GetAppendBuffer(length16 < capacity ? length16 : capacity,
                                      3*length16,
                                      അട്ടി_Buffer, capacity,
                                      &capacity);
    int32_t length8 = 0;
    UErrorCode errorCode = U_ZERO_ERROR;
    u_strToUTF8WithSub(utf8, capacity, &length8,
                       getBuffer(), length16,
                       0xFFFD,  // Standard substitution character.
                       nullptr,    // Don't care about number of substitutions.
                       &errorCode);
    എങ്കിൽ(errorCode == U_BUFFER_OVERFLOW_ERROR) {
      utf8 = static_cast<അക്ഷരം*>(uprv_malloc(length8));
      എങ്കിൽ(utf8 != nullptr) {
        utf8IsOwned = സത്യം;
        errorCode = U_ZERO_ERROR;
        u_strToUTF8WithSub(utf8, length8, &length8,
                           getBuffer(), length16,
                           0xFFFD,  // Standard substitution character.
                           nullptr,    // Don't care about number of substitutions.
                           &errorCode);
      } അല്ലെങ്കിൽ {
        errorCode = U_MEMORY_ALLOCATION_ERROR;
      }
    }
    എങ്കിൽ(U_SUCCESS(errorCode)) {
      sink.Append(utf8, length8);
      sink.Flush();
    }
    എങ്കിൽ(utf8IsOwned) {
      uprv_free(utf8);
    }
  }
}

int32_t
UnicodeString::toUTF32(UChar32 *utf32, int32_t capacity, UErrorCode &errorCode) സ്ഥിരം {
  int32_t length32=0;
  എങ്കിൽ(U_SUCCESS(errorCode)) {
    // getBuffer() and u_strToUTF32WithSub() check for illegal arguments.
    u_strToUTF32WithSub(utf32, capacity, &length32,
        getBuffer(), length(),
        0xfffd,  // Substitution character.
        nullptr,    // Don't care about number of substitutions.
        &errorCode);
  }
  തിരികെ length32;
}

int32_t 
UnicodeString::indexOf(സ്ഥിരം char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) സ്ഥിരം
{
  എങ്കിൽ (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    തിരികെ -1;
  }

  // UnicodeString does not find empty substrings
  എങ്കിൽ(srcLength < 0 && srcChars[srcStart] == 0) {
    തിരികെ -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the first occurrence of the substring
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_strFindFirst(array + start, length, srcChars + srcStart, srcLength);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(char16_t c,
             int32_t start,
             int32_t length) സ്ഥിരം
{
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_memchr(array + start, c, length);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doIndexOf(UChar32 c,
                         int32_t start,
                         int32_t length) സ്ഥിരം {
  // pin indices
  pinIndices(start, length);

  // find the first occurrence of c
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_memchr32(array + start, c, length);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

int32_t 
UnicodeString::lastIndexOf(സ്ഥിരം char16_t *srcChars,
               int32_t srcStart,
               int32_t srcLength,
               int32_t start,
               int32_t length) സ്ഥിരം
{
  എങ്കിൽ (isBogus() || srcChars == nullptr || srcStart < 0 || srcLength == 0) {
    തിരികെ -1;
  }

  // UnicodeString does not find empty substrings
  എങ്കിൽ(srcLength < 0 && srcChars[srcStart] == 0) {
    തിരികെ -1;
  }

  // get the indices within bounds
  pinIndices(start, length);

  // find the last occurrence of the substring
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_strFindLast(array + start, length, srcChars + srcStart, srcLength);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(char16_t c,
                 int32_t start,
                 int32_t length) സ്ഥിരം
{
  എങ്കിൽ(isBogus()) {
    തിരികെ -1;
  }

  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_memrchr(array + start, c, length);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

int32_t
UnicodeString::doLastIndexOf(UChar32 c,
                             int32_t start,
                             int32_t length) സ്ഥിരം {
  // pin indices
  pinIndices(start, length);

  // find the last occurrence of c
  സ്ഥിരം char16_t *array = getArrayStart();
  സ്ഥിരം char16_t *match = u_memrchr32(array + start, c, length);
  എങ്കിൽ(match == nullptr) {
    തിരികെ -1;
  } അല്ലെങ്കിൽ {
    തിരികെ static_cast<int32_t>(match - array);
  }
}

//========================================
// Write implementation
//========================================

UnicodeString&
UnicodeString::findAndReplace(int32_t start,
                              int32_t length,
                              സ്ഥിരം UnicodeString& oldText,
                              int32_t oldStart,
                              int32_t oldLength,
                              സ്ഥിരം UnicodeString& newText,
                              int32_t newStart,
                              int32_t newLength) U_LIFETIME_BOUND
{
  എങ്കിൽ(isBogus() || oldText.isBogus() || newText.isBogus()) {
    തിരികെ *ഇത്;
  }

  pinIndices(start, length);
  oldText.pinIndices(oldStart, oldLength);
  newText.pinIndices(newStart, newLength);

  എങ്കിൽ(oldLength == 0) {
    തിരികെ *ഇത്;
  }

  ആകുമ്പോൾ(length > 0 && length >= oldLength) {
    int32_t pos = indexOf(oldText, oldStart, oldLength, start, length);
    എങ്കിൽ(pos < 0) {
      // no more oldText's here: done
      നിർത്തുക;
    } അല്ലെങ്കിൽ {
      // we found oldText, replace it by newText and go beyond it
      replace(pos, oldLength, newText, newStart, newLength);
      length -= pos + oldLength - start;
      start = pos + newLength;
    }
  }

  തിരികെ *ഇത്;
}


ശൂന്യം
UnicodeString::setToBogus()
{
  releaseArray();

  fUnion.fFields.fLengthAndFlags = kIsBogus;
  fUnion.fFields.fArray = nullptr;
  fUnion.fFields.fCapacity = 0;
}

// turn a bogus string into an empty one
ശൂന്യം
UnicodeString::unBogus() {
  എങ്കിൽ(fUnion.fFields.fLengthAndFlags & kIsBogus) {
    setToEmpty();
  }
}

സ്ഥിരം char16_t*
UnicodeString::getTerminatedBuffer() U_LIFETIME_BOUND {
  എങ്കിൽ(!isWritable()) {
    തിരികെ nullptr;
  }
  char16_t *array = getArrayStart();
  int32_t len = length();
  എങ്കിൽ(len < getCapacity()) {
    എങ്കിൽ(fUnion.fFields.fLengthAndFlags & kBufferIsReadonly) {
      // If len<capacity on a read-only alias, then array[len] is
      // either the original NUL (if constructed with (true, s, length))
      // or one of the original string contents characters (if later truncated),
      // therefore we can assume that array[len] is initialized memory.
      എങ്കിൽ(array[len] == 0) {
        തിരികെ array;
      }
    } അല്ലെങ്കിൽ എങ്കിൽ(((fUnion.fFields.fLengthAndFlags & kRefCounted) == 0 || ref_എണ്ണം() == 1)) {
      // kRefCounted: Do not write the NUL if the buffer is shared.
      // That is mostly safe, except when the length of one copy was modified
      // without copy-on-write, e.g., via truncate(newLength) or remove().
      // Then the NUL would be written into the middle of another copy's string.

      // Otherwise, the buffer is fully writable and it is anyway safe to write the NUL.
      // Do not test if there is a NUL already because it might be uninitialized memory.
      // (That would be safe, but tools like valgrind & Purify would complain.)
      array[len] = 0;
      തിരികെ array;
    }
  }
  എങ്കിൽ(len<INT32_MAX && cloneArrayIfNeeded(len+1)) {
    array = getArrayStart();
    array[len] = 0;
    തിരികെ array;
  } അല്ലെങ്കിൽ {
    തിരികെ nullptr;
  }
}

// setTo() analogous to the readonly-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(UBool isTerminated,
                     ConstChar16Ptr textPtr U_LIFETIME_BOUND,
                     int32_t textLength) U_LIFETIME_BOUND
{
  എങ്കിൽ(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    തിരികെ *ഇത്;
  }

  സ്ഥിരം char16_t *text = textPtr;
  എങ്കിൽ(text == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    തിരികെ *ഇത്;
  }

  എങ്കിൽ( textLength < -1 ||
      (textLength == -1 && !isTerminated) ||
      (textLength >= 0 && isTerminated && text[textLength] != 0)
  ) {
    setToBogus();
    തിരികെ *ഇത്;
  }

  releaseArray();

  എങ്കിൽ(textLength == -1) {
    // text is terminated, or else it would have failed the above test
    textLength = u_strlen(text);
  }
  fUnion.fFields.fLengthAndFlags = kReadonlyAlias;
  setArray(const_cast<char16_t*>(text), textLength, isTerminated ? textLength + 1 : textLength);
  തിരികെ *ഇത്;
}

// setTo() analogous to the writable-aliasing constructor with the same signature
UnicodeString&
UnicodeString::setTo(char16_t* buffer,
                     int32_t buffLength U_LIFETIME_BOUND,
                     int32_t buffCapacity) U_LIFETIME_BOUND {
  എങ്കിൽ(fUnion.fFields.fLengthAndFlags & kOpenGetBuffer) {
    // do not modify a string that has an "open" getBuffer(minCapacity)
    തിരികെ *ഇത്;
  }

  എങ്കിൽ(buffer == nullptr) {
    // treat as an empty string, do not alias
    releaseArray();
    setToEmpty();
    തിരികെ *ഇത്;
  }

  എങ്കിൽ(buffLength < -1 || buffCapacity < 0 || buffLength > buffCapacity) {
    setToBogus();
    തിരികെ *ഇത്;
  } അല്ലെങ്കിൽ എങ്കിൽ(buffLength == -1) {
    // buffLength = u_strlen(buff); but do not look beyond buffCapacity
    സ്ഥിരം char16_t *p = buffer, *limit = buffer + buffCapacity;
    ആകുമ്പോൾ(p != limit && *p != 0) {
      ++p;
    }
    buffLength = static_cast<int32_t>(p - buffer);
  }

  releaseArray();

  fUnion.fFields.fLengthAndFlags = kWritableAlias;
  setArray(buffer, buffLength, buffCapacity);
  തിരികെ *ഇത്;
}

UnicodeString& UnicodeString::setToUTF8(StringPiece utf8) U_LIFETIME_BOUND {
  unBogus();
  int32_t length = utf8.length();
  int32_t capacity;
  // The UTF-16 string will be at most as long as the UTF-8 string.
  എങ്കിൽ(length <= US_STACKBUF_SIZE) {
    capacity = US_STACKBUF_SIZE;
  } അല്ലെങ്കിൽ {
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
  എങ്കിൽ(U_FAILURE(errorCode)) {
    setToBogus();
  }
  തിരികെ *ഇത്;
}

UnicodeString&
UnicodeString::setCharAt(int32_t offset, char16_t c) U_LIFETIME_BOUND
{
  int32_t len = length();
  എങ്കിൽ(cloneArrayIfNeeded() && len > 0) {
    എങ്കിൽ(offset < 0) {
      offset = 0;
    } അല്ലെങ്കിൽ എങ്കിൽ(offset >= len) {
      offset = len - 1;
    }

    getArrayStart()[offset] = c;
  }
  തിരികെ *ഇത്;
}

UnicodeString&
UnicodeString::replace(int32_t start,
                       int32_t _length,
                       UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t count = 0;
  UBool isError = അസത്യം;
  U16_APPEND(buffer, count, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError (srcChar is not a valid code point) then count==0 which means
  // we remove the source segment rather than replacing it with srcChar.
  തിരികെ doReplace(start, _length, buffer, 0, isError ? 0 : count);
}

UnicodeString&
UnicodeString::append(UChar32 srcChar) U_LIFETIME_BOUND {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t _length = 0;
  UBool isError = അസത്യം;
  U16_APPEND(buffer, _length, U16_MAX_LENGTH, srcChar, isError);
  // We test isError so that the compiler does not complain that we don't.
  // If isError then _length==0 which turns the doAppend() into a no-op anyway.
  തിരികെ isError ? *ഇത് : doAppend(buffer, 0, _length);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         സ്ഥിരം UnicodeString& src,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);

  // get the characters from src
  // and replace the range in ourselves with them
  തിരികെ doReplace(start, length, src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doReplace(int32_t start,
                         int32_t length,
                         സ്ഥിരം char16_t* srcChars,
                         int32_t srcStart,
                         int32_t srcLength) U_LIFETIME_BOUND
{
  എങ്കിൽ(!isWritable()) {
    തിരികെ *ഇത്;
  }

  int32_t oldLength = ഇത്->length();

  // optimize (read-only alias).remove(0, start) and .remove(start, end)
  എങ്കിൽ((fUnion.fFields.fLengthAndFlags&kBufferIsReadonly) && srcLength == 0) {
    എങ്കിൽ(start == 0) {
      // remove prefix by adjusting the array pointer
      pinIndex(length);
      fUnion.fFields.fArray += length;
      fUnion.fFields.fCapacity -= length;
      setLength(oldLength - length);
      തിരികെ *ഇത്;
    } അല്ലെങ്കിൽ {
      pinIndex(start);
      എങ്കിൽ(length >= (oldLength - start)) {
        // remove suffix by reducing the length (like truncate())
        setLength(start);
        fUnion.fFields.fCapacity = start;  // not NUL-terminated any more
        തിരികെ *ഇത്;
      }
    }
  }

  എങ്കിൽ(start == oldLength) {
    തിരികെ doAppend(srcChars, srcStart, srcLength);
  }

  എങ്കിൽ (srcChars == nullptr) {
    srcLength = 0;
  } അല്ലെങ്കിൽ {
    // Perform all remaining operations relative to srcChars + srcStart.
    // From this point forward, do not use srcStart.
    srcChars += srcStart;
    എങ്കിൽ (srcLength < 0) {
      // get the srcLength if necessary
      srcLength = u_strlen(srcChars);
    }
  }

  // pin the indices to legal values
  pinIndices(start, length);

  // Calculate the size of the string after the replace.
  // Avoid int32_t overflow.
  int32_t newLength = oldLength - length;
  എങ്കിൽ(srcLength > (INT32_MAX - newLength)) {
    setToBogus();
    തിരികെ *ഇത്;
  }
  newLength += srcLength;

  // Check for insertion into ourself
  സ്ഥിരം char16_t *oldArray = getArrayStart();
  എങ്കിൽ (isBufferWritable() &&
      oldArray < srcChars + srcLength &&
      srcChars < oldArray + oldLength) {
    // Copy into a new UnicodeString and start over
    UnicodeString copy(srcChars, srcLength);
    എങ്കിൽ (copy.isBogus()) {
      setToBogus();
      തിരികെ *ഇത്;
    }
    തിരികെ doReplace(start, length, copy.getArrayStart(), 0, srcLength);
  }

  // cloneArrayIfNeeded(doCopyArray=false) may change fArray but will not copy the current contents;
  // therefore we need to keep the current fArray
  char16_t old_അട്ടി_Buffer[US_STACKBUF_SIZE];
  എങ്കിൽ((fUnion.fFields.fLengthAndFlags&k_Using_അട്ടി_Buffer) && (newLength > US_STACKBUF_SIZE)) {
    // copy the stack buffer contents because it will be overwritten with
    // fUnion.fFields values
    u_memcpy(old_അട്ടി_Buffer, oldArray, oldLength);
    oldArray = old_അട്ടി_Buffer;
  }

  // clone our array and allocate a bigger array if needed
  int32_t *bufferToDelete = nullptr;
  എങ്കിൽ(!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength),
                         അസത്യം, &bufferToDelete)
  ) {
    തിരികെ *ഇത്;
  }

  // now do the replace

  char16_t *newArray = getArrayStart();
  എങ്കിൽ(newArray != oldArray) {
    // if fArray changed, then we need to copy everything except what will change
    us_arrayCopy(oldArray, 0, newArray, 0, start);
    us_arrayCopy(oldArray, start + length,
                 newArray, start + srcLength,
                 oldLength - (start + length));
  } അല്ലെങ്കിൽ എങ്കിൽ(length != srcLength) {
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
  എങ്കിൽ (bufferToDelete) {
    uprv_free(bufferToDelete);
  }

  തിരികെ *ഇത്;
}

UnicodeString&
UnicodeString::doReplace(int32_t start, int32_t length, std::u16string_view src) U_LIFETIME_BOUND {
  എങ്കിൽ (!isWritable()) {
    തിരികെ *ഇത്;
  }
  എങ്കിൽ (src.length() > INT32_MAX) {
    setToBogus();
    തിരികെ *ഇത്;
  }
  തിരികെ doReplace(start, length, src.data(), 0, static_cast<int32_t>(src.length()));
}

// Versions of doReplace() only for append() variants.
// doReplace() and doAppend() optimize for different cases.

UnicodeString&
UnicodeString::doAppend(സ്ഥിരം UnicodeString& src,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  എങ്കിൽ(srcLength == 0) {
    തിരികെ *ഇത്;
  }

  // pin the indices to legal values
  src.pinIndices(srcStart, srcLength);
  തിരികെ doAppend(src.getArrayStart(), srcStart, srcLength);
}

UnicodeString&
UnicodeString::doAppend(സ്ഥിരം char16_t* srcChars,
                        int32_t srcStart,
                        int32_t srcLength) U_LIFETIME_BOUND {
  എങ്കിൽ(!isWritable() || srcLength == 0 || srcChars == nullptr) {
    തിരികെ *ഇത്;
  }

  // Perform all remaining operations relative to srcChars + srcStart.
  // From this point forward, do not use srcStart.
  srcChars += srcStart;

  എങ്കിൽ(srcLength < 0) {
    // get the srcLength if necessary
    എങ്കിൽ((srcLength = u_strlen(srcChars)) == 0) {
      തിരികെ *ഇത്;
    }
  }

  int32_t oldLength = length();
  int32_t newLength;

  എങ്കിൽ (srcLength <= getCapacity() - oldLength && isBufferWritable()) {
    newLength = oldLength + srcLength;
    // Faster than a memmove
    എങ്കിൽ (srcLength <= 4) {
      char16_t *arr = getArrayStart();
      arr[oldLength] = srcChars[0];
      എങ്കിൽ (srcLength > 1) arr[oldLength+1] = srcChars[1];
      എങ്കിൽ (srcLength > 2) arr[oldLength+2] = srcChars[2];
      എങ്കിൽ (srcLength > 3) arr[oldLength+3] = srcChars[3];
      setLength(newLength);
      തിരികെ *ഇത്;
    }
  } അല്ലെങ്കിൽ {
    എങ്കിൽ (uprv_add32_overflow(oldLength, srcLength, &newLength)) {
      setToBogus();
      തിരികെ *ഇത്;
    }

    // Check for append onto ourself
    സ്ഥിരം char16_t* oldArray = getArrayStart();
    എങ്കിൽ (isBufferWritable() &&
        oldArray < srcChars + srcLength &&
        srcChars < oldArray + oldLength) {
      // Copy into a new UnicodeString and start over
      UnicodeString copy(srcChars, srcLength);
      എങ്കിൽ (copy.isBogus()) {
        setToBogus();
        തിരികെ *ഇത്;
      }
      തിരികെ doAppend(copy.getArrayStart(), 0, srcLength);
    }

    // optimize append() onto a large-enough, owned string
    എങ്കിൽ (!cloneArrayIfNeeded(newLength, getGrowCapacity(newLength))) {
      തിരികെ *ഇത്;
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
  എങ്കിൽ(srcChars != newArray + oldLength) {
    us_arrayCopy(srcChars, 0, newArray, oldLength, srcLength);
  }
  setLength(newLength);

  തിരികെ *ഇത്;
}

UnicodeString&
UnicodeString::doAppend(std::u16string_view src) U_LIFETIME_BOUND {
  എങ്കിൽ (!isWritable() || src.empty()) {
    തിരികെ *ഇത്;
  }
  എങ്കിൽ (src.length() > INT32_MAX) {
    setToBogus();
    തിരികെ *ഇത്;
  }
  തിരികെ doAppend(src.data(), 0, static_cast<int32_t>(src.length()));
}

/**
 * Replaceable API
 */
ശൂന്യം
UnicodeString::handleReplaceBetween(int32_t start,
                                    int32_t limit,
                                    സ്ഥിരം UnicodeString& text) {
    replaceBetween(start, limit, text);
}

/**
 * Replaceable API
 */
ശൂന്യം 
UnicodeString::copy(int32_t start, int32_t limit, int32_t dest) {
    എങ്കിൽ (limit <= start) {
        തിരികെ; // Nothing to do; avoid bogus malloc call
    }
    char16_t* text = static_cast<char16_t*>(uprv_malloc(വലുപ്പം(char16_t) * (limit - start)));
    // Check to make sure text is not null.
    എങ്കിൽ (text != nullptr) {
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
UBool Replaceable::hasMetaData() സ്ഥിരം {
    തിരികെ സത്യം;
}

/**
 * Replaceable API
 */
UBool UnicodeString::hasMetaData() സ്ഥിരം {
    തിരികെ അസത്യം;
}

UnicodeString&
UnicodeString::doReverse(int32_t start, int32_t length) U_LIFETIME_BOUND {
  എങ്കിൽ(length <= 1 || !cloneArrayIfNeeded()) {
    തിരികെ *ഇത്;
  }

  // pin the indices to legal values
  pinIndices(start, length);
  എങ്കിൽ(length <= 1) {  // pinIndices() might have shrunk the length
    തിരികെ *ഇത്;
  }

  char16_t *left = getArrayStart() + start;
  char16_t *വലത് = left + length - 1;  // -1 for inclusive boundary (length>=2)
  char16_t swap;
  UBool hasSupplementary = അസത്യം;

  // Before the loop we know left<right because length>=2.
  ചെയ്യുക {
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(swap = *left));
    hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left++ = *വലത്));
    *വലത്-- = swap;
  } ആകുമ്പോൾ(left < വലത്);
  // Make sure to test the middle code unit of an odd-length string.
  // Redundant if the length is even.
  hasSupplementary |= static_cast<UBool>(U16_IS_LEAD(*left));

  /* if there are supplementary code points in the reversed range, then re-swap their surrogates */
  എങ്കിൽ(hasSupplementary) {
    char16_t swap2;

    left = getArrayStart() + start;
    വലത് = left + length - 1; // -1 so that we can look at *(left+1) if left<right
    ആകുമ്പോൾ(left < വലത്) {
      എങ്കിൽ(U16_IS_TRAIL(swap = *left) && U16_IS_LEAD(swap2 = *(left + 1))) {
        *left++ = swap2;
        *left++ = swap;
      } അല്ലെങ്കിൽ {
        ++left;
      }
    }
  }

  തിരികെ *ഇത്;
}

UBool 
UnicodeString::padLeading(int32_t targetLength,
                          char16_t padChar)
{
  int32_t oldLength = length();
  എങ്കിൽ(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    തിരികെ അസത്യം;
  } അല്ലെങ്കിൽ {
    // move contents up by padding width
    char16_t *array = getArrayStart();
    int32_t start = targetLength - oldLength;
    us_arrayCopy(array, 0, array, start, oldLength);

    // fill in padding character
    ആകുമ്പോൾ(--start >= 0) {
      array[start] = padChar;
    }
    setLength(targetLength);
    തിരികെ സത്യം;
  }
}

UBool 
UnicodeString::padTrailing(int32_t targetLength,
                           char16_t padChar)
{
  int32_t oldLength = length();
  എങ്കിൽ(oldLength >= targetLength || !cloneArrayIfNeeded(targetLength)) {
    തിരികെ അസത്യം;
  } അല്ലെങ്കിൽ {
    // fill in padding character
    char16_t *array = getArrayStart();
    int32_t length = targetLength;
    ആകുമ്പോൾ(--length >= oldLength) {
      array[length] = padChar;
    }
    setLength(targetLength);
    തിരികെ സത്യം;
  }
}

//========================================
// Hashing
//========================================
int32_t
UnicodeString::doHashCode() സ്ഥിരം
{
    /* Delegate hash computation to uhash.  This makes UnicodeString
     * hashing consistent with char16_t* hashing.  */
    int32_t hashCode = ustr_hashUCharsN(getArrayStart(), length());
    എങ്കിൽ (hashCode == kInvalidHashCode) {
        hashCode = kEmptyHashCode;
    }
    തിരികെ hashCode;
}

//========================================
// External Buffer
//========================================

char16_t*
UnicodeString::getBuffer(int32_t minCapacity) U_LIFETIME_BOUND {
  എങ്കിൽ(minCapacity>=-1 && cloneArrayIfNeeded(minCapacity)) {
    fUnion.fFields.fLengthAndFlags|=kOpenGetBuffer;
    setZeroLength();
    തിരികെ getArrayStart();
  } അല്ലെങ്കിൽ {
    തിരികെ nullptr;
  }
}

ശൂന്യം
UnicodeString::releaseBuffer(int32_t newLength) {
  എങ്കിൽ(fUnion.fFields.fLengthAndFlags&kOpenGetBuffer && newLength>=-1) {
    // set the new fLength
    int32_t capacity=getCapacity();
    എങ്കിൽ(newLength==-1) {
      // the new length is the string length, capped by fCapacity
      സ്ഥിരം char16_t *array=getArrayStart(), *p=array, *limit=array+capacity;
      ആകുമ്പോൾ(p<limit && *p!=0) {
        ++p;
      }
      newLength = static_cast<int32_t>(p - array);
    } അല്ലെങ്കിൽ എങ്കിൽ(newLength>capacity) {
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
  എങ്കിൽ(newCapacity == -1) {
    newCapacity = getCapacity();
  }

  // while a getBuffer(minCapacity) is "open",
  // prevent any modifications of the string by returning false here
  // if the string is bogus, then only an assignment or similar can revive it
  എങ്കിൽ(!isWritable()) {
    തിരികെ അസത്യം;
  }

  /*
   * We need to make a copy of the array if
   * the buffer is read-only, or
   * the buffer is refCounted (shared), and refCount>1, or
   * the buffer is too small.
   * Return false if memory could not be allocated.
   */
  എങ്കിൽ(forceClone ||
     fUnion.fFields.fLengthAndFlags & kBufferIsReadonly ||
     (fUnion.fFields.fLengthAndFlags & kRefCounted && ref_എണ്ണം() > 1) ||
     newCapacity > getCapacity()
  ) {
    // check growCapacity for default value and use of the stack buffer
    എങ്കിൽ(growCapacity < 0) {
      growCapacity = newCapacity;
    } അല്ലെങ്കിൽ എങ്കിൽ(newCapacity <= US_STACKBUF_SIZE && growCapacity > US_STACKBUF_SIZE) {
      growCapacity = US_STACKBUF_SIZE;
    } അല്ലെങ്കിൽ എങ്കിൽ(newCapacity > growCapacity) {
      setToBogus();
      തിരികെ അസത്യം;  // bad inputs
    }
    എങ്കിൽ(growCapacity > kMaxCapacity) {
      setToBogus();
      തിരികെ അസത്യം;
    }

    // save old values
    char16_t old_അട്ടി_Buffer[US_STACKBUF_SIZE];
    char16_t *oldArray;
    int32_t oldLength = length();
    int16_t flags = fUnion.fFields.fLengthAndFlags;

    എങ്കിൽ(flags&k_Using_അട്ടി_Buffer) {
      U_ASSERT(!(flags&kRefCounted)); /* kRefCounted and kUsingStackBuffer are mutally exclusive */
      എങ്കിൽ(doCopyArray && growCapacity > US_STACKBUF_SIZE) {
        // copy the stack buffer contents because it will be overwritten with
        // fUnion.fFields values
        us_arrayCopy(fUnion.f_അട്ടി_Fields.fBuffer, 0, old_അട്ടി_Buffer, 0, oldLength);
        oldArray = old_അട്ടി_Buffer;
      } അല്ലെങ്കിൽ {
        oldArray = nullptr; // no need to copy from the stack buffer to itself
      }
    } അല്ലെങ്കിൽ {
      oldArray = fUnion.fFields.fArray;
      U_ASSERT(oldArray!=nullptr); /* when stack buffer is not used, oldArray must have a non-nullptr reference */
    }

    // allocate a new array
    എങ്കിൽ(allocate(growCapacity) ||
       (newCapacity < growCapacity && allocate(newCapacity))
    ) {
      എങ്കിൽ(doCopyArray) {
        // copy the contents
        // do not copy more than what fits - it may be smaller than before
        int32_t minLength = oldLength;
        newCapacity = getCapacity();
        എങ്കിൽ(newCapacity < minLength) {
          minLength = newCapacity;
        }
        എങ്കിൽ(oldArray != nullptr) {
          us_arrayCopy(oldArray, 0, getArrayStart(), 0, minLength);
        }
        setLength(minLength);
      } അല്ലെങ്കിൽ {
        setZeroLength();
      }

      // release the old array
      എങ്കിൽ(flags & kRefCounted) {
        // the array is refCounted; decrement and release if 0
        u_atomic_int32_t* p_Ref_എണ്ണം = reinterpret_cast<u_atomic_int32_t*>(oldArray) - 1;
        എങ്കിൽ(umtx_atomic_dec(p_Ref_എണ്ണം) == 0) {
          എങ്കിൽ (pBufferToDelete == nullptr) {
              // Note: cast to (void *) is needed with MSVC, where u_atomic_int32_t
              // is defined as volatile. (Volatile has useful non-standard behavior
              //   with this compiler.)
            uprv_free((ശൂന്യം *)p_Ref_എണ്ണം);
          } അല്ലെങ്കിൽ {
            // the caller requested to delete it himself
            *pBufferToDelete = reinterpret_cast<int32_t*>(p_Ref_എണ്ണം);
          }
        }
      }
    } അല്ലെങ്കിൽ {
      // not enough memory for growCapacity and not even for the smaller newCapacity
      // reset the old values for setToBogus() to release the array
      എങ്കിൽ(!(flags&k_Using_അട്ടി_Buffer)) {
        fUnion.fFields.fArray = oldArray;
      }
      fUnion.fFields.fLengthAndFlags = flags;
      setToBogus();
      തിരികെ അസത്യം;
    }
  }
  തിരികെ സത്യം;
}

// UnicodeStringAppendable ------------------------------------------------- ***

UnicodeStringAppendable::~UnicodeStringAppendable() {}

UBool
UnicodeStringAppendable::appendCodeUnit(char16_t c) {
  തിരികെ str.doAppend(&c, 0, 1).isWritable();
}

UBool
UnicodeStringAppendable::appendCodePoint(UChar32 c) {
  char16_t buffer[U16_MAX_LENGTH];
  int32_t cLength = 0;
  UBool isError = അസത്യം;
  U16_APPEND(buffer, cLength, U16_MAX_LENGTH, c, isError);
  തിരികെ !isError && str.doAppend(buffer, 0, cLength).isWritable();
}

UBool
UnicodeStringAppendable::appendString(സ്ഥിരം char16_t *s, int32_t length) {
  തിരികെ str.doAppend(s, 0, length).isWritable();
}

UBool
UnicodeStringAppendable::reserveAppendCapacity(int32_t appendCapacity) {
  തിരികെ str.cloneArrayIfNeeded(str.length() + appendCapacity);
}

char16_t* UnicodeStringAppendable::getAppendBuffer(int32_t minCapacity,
                                                   int32_t desiredCapacityHint,
                                                   char16_t* scratch U_LIFETIME_BOUND,
                                                   int32_t scratchCapacity,
                                                   int32_t* resultCapacity) U_LIFETIME_BOUND {
  എങ്കിൽ(minCapacity < 1 || scratchCapacity < minCapacity) {
    *resultCapacity = 0;
    തിരികെ nullptr;
  }
  int32_t oldLength = str.length();
  എങ്കിൽ(minCapacity <= (kMaxCapacity - oldLength) &&
      desiredCapacityHint <= (kMaxCapacity - oldLength) &&
      str.cloneArrayIfNeeded(oldLength + minCapacity, oldLength + desiredCapacityHint)) {
    *resultCapacity = str.getCapacity() - oldLength;
    തിരികെ str.getArrayStart() + oldLength;
  }
  *resultCapacity = scratchCapacity;
  തിരികെ scratch;
}

U_NAMESPACE_END

U_NAMESPACE_USE

U_CAPI int32_t U_EXPORT2
uhash_hashUnicodeString(സ്ഥിരം UElement താക്കോൽ) {
    സ്ഥിരം UnicodeString *str = (സ്ഥിരം UnicodeString*) താക്കോൽ.pointer;
    തിരികെ (str == nullptr) ? 0 : str->hashCode();
}

// Moved here from uhash_us.cpp so that using a UVector of UnicodeString*
// does not depend on hashtable code.
U_CAPI UBool U_EXPORT2
uhash_compareUnicodeString(സ്ഥിരം UElement key1, സ്ഥിരം UElement key2) {
    സ്ഥിരം UnicodeString *str1 = (സ്ഥിരം UnicodeString*) key1.pointer;
    സ്ഥിരം UnicodeString *str2 = (സ്ഥിരം UnicodeString*) key2.pointer;
    എങ്കിൽ (str1 == str2) {
        തിരികെ സത്യം;
    }
    എങ്കിൽ (str1 == nullptr || str2 == nullptr) {
        തിരികെ അസത്യം;
    }
    തിരികെ *str1 == *str2;
}

#ifdef U_STATIC_IMPLEMENTATION
/*
This should never be called. It is defined here to make sure that the
virtual vector deleting destructor is defined within unistr.cpp.
The vector deleting destructor is already a part of UObject,
but defining it here makes sure that it is included with this object file.
This makes sure that static library dependencies are kept to a minimum.
*/
#എങ്കിൽ defined(__clang__) || U_GCC_MAJOR_MINOR >= 1100
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
സ്ഥിതം ശൂന്യം uprv_UnicodeStringDummy() {
    delete [] (പുതിയ UnicodeString[2]);
}
#pragma GCC diagnostic pop
#endif
#endif
